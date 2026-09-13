/* ui.c - SDL2 rendering and smooth animations for security assistant demo */

#include "ui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "data.h"
#include "ai_detector.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *font = NULL;
static int running = 1;

static int winW = 1200, winH = 800;

// simple easing function for animations (cubic ease-out)
static float ease_out_cubic(float t) { float p = 1 - t; return 1 - p*p*p; }

int ui_init(void) {
    window = SDL_CreateWindow("SSRAirport Security Assistant - Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, SDL_WINDOW_SHOWN);
    if (!window) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return -1; }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) { fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError()); SDL_DestroyWindow(window); return -1; }

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (!font) { fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError()); }

    return 0;
}

static void draw_text(const char *txt, int x, int y, SDL_Color col) {
    if (!font) return;
    SDL_Surface *s = TTF_RenderUTF8_Blended(font, txt, col);
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect dst = { x, y, s->w, s->h };
    SDL_RenderCopy(renderer, t, NULL, &dst);
    SDL_FreeSurface(s); SDL_DestroyTexture(t);
}

// draw a rounded simple baggage item with highlight
static void draw_bag(int x, int y, int w, int h, SDL_Color color, int highlight) {
    SDL_Rect r = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &r);
    if (highlight) {
        SDL_SetRenderDrawColor(renderer, 255, 120, 160, 200);
        SDL_Rect o = { x-4, y-4, w+8, h+8 };
        SDL_RenderDrawRect(renderer, &o);
    }
}

void ui_run(void) {
    SDL_Event e;
    const int bagW = 140, bagH = 90;
    Uint32 last = SDL_GetTicks();
    float anim_phase = 0.0f;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = 0;
                if (e.key.keysym.sym == SDLK_h) {
                    // show last log
                    char buf[256]; data_get_last_log(buf, sizeof(buf)); printf("Last log: %s\n", buf);
                }
            }
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - last) / 1000.0f; last = now;
        anim_phase += dt * 0.6f; if (anim_phase > 1.0f) anim_phase -= floor(anim_phase);

        // background
        SDL_SetRenderDrawColor(renderer, 250, 248, 255, 255);
        SDL_RenderClear(renderer);

        // left panel: live sensor feed
        SDL_Rect left = { 12, 12, 560, 776 };
        SDL_SetRenderDrawColor(renderer, 255, 250, 254, 255);
        SDL_RenderFillRect(renderer, &left);
        draw_text("Live X-ray Conveyor", 28, 24, (SDL_Color){34,34,34,255});

        // draw conveyor belt area
        int beltX = 40, beltY = 80, beltW = 520, beltH = 500;
        SDL_Rect belt = { beltX, beltY, beltW, beltH };
        SDL_SetRenderDrawColor(renderer, 240,230,240,255);
        SDL_RenderFillRect(renderer, &belt);

        // draw animated bags horizontally across belt
        int bags = data_event_count();
        for (int i=0;i<bags;i++) {
            Event ev; data_get_event(i, &ev);
            // position based on arrival time modded
            float t = fmodf(anim_phase + i*0.15f, 1.0f);
            float ease = ease_out_cubic(t);
            int x = beltX + (int)(ease * (beltW - bagW));
            int y = beltY + 20 + i*(bagH+10) % (beltH - bagH);

            SDL_Color col = { 255, 192, 220, 255 };
            int highlight = (ev.state == EV_THREAT_HOLD || ev.state == EV_THREAT_ESCALATE);
            draw_bag(x, y, bagW, bagH, col, highlight);

            // small overlay with probability
            char score[64]; snprintf(score, sizeof(score), "%.1f%%", ev.threat_score*100.0);
            draw_text(score, x+8, y+6, (SDL_Color){60,60,60,255});

            // if there's a detected hotspot, draw a red circle
            if (ev.hotspot_x >= 0) {
                int cx = x + ev.hotspot_x; int cy = y + ev.hotspot_y;
                SDL_SetRenderDrawColor(renderer, 255, 80, 120, 200);
                SDL_Rect rc = { cx-8, cy-8, 16, 16 };
                SDL_RenderFillRect(renderer, &rc);
            }
        }

        // right panel: selected event details & explainability
        SDL_Rect right = { 588, 12, 600, 776 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &right);
        draw_text("Selected Detection / Explainability", 604, 24, (SDL_Color){34,34,34,255});

        // show top-threat or placeholder
        Event top; int idx = data_get_top_event(&top);
        if (idx >= 0) {
            char hdr[256]; snprintf(hdr, sizeof(hdr), "ID: %s  Flight: %s  Score: %.2f", top.id, top.flight_id, top.threat_score);
            draw_text(hdr, 604, 60, (SDL_Color){20,20,20,255});
            // explainability: bar per feature
            double contributions[6]; ai_explain(&top, contributions);
            const char *labels[6] = {"XraySig","Metal","Weight","Watchlist","Behavior","Other"};
            for (int f=0; f<6; ++f) {
                int bx = 604; int by = 100 + f*40; int bw = 420; int bh = 18;
                SDL_SetRenderDrawColor(renderer, 245, 230, 240, 255);
                SDL_Rect bg = { bx, by, bw, bh };
                SDL_RenderFillRect(renderer, &bg);
                int fill = (int)(bw * fabs(contributions[f]));
                if (contributions[f] >= 0) SDL_SetRenderDrawColor(renderer, 255, 130, 170, 255);
                else SDL_SetRenderDrawColor(renderer, 200, 230, 220, 255);
                SDL_Rect fg = { bx, by, fill, bh };
                SDL_RenderFillRect(renderer, &fg);
                draw_text(labels[f], bx + bw + 8, by, (SDL_Color){60,60,60,255});
            }

            // action buttons (textual for demo)
            draw_text("Actions: [H]old  [R]escan  [E]scalate  [O]verride", 604, 360, (SDL_Color){10,10,10,255});
        } else {
            draw_text("No detections currently", 604, 60, (SDL_Color){100,100,100,255});
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void ui_shutdown(void) {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}
