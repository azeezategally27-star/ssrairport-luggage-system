/* ui.c - simple SDL2 UI and animation loop for the demo */

#include "ui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "data.h"
#include "animations.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *font = NULL;
static int running = 1;

int ui_init(void) {
    window = SDL_CreateWindow("SSRAirport Flight Assistant Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_SHOWN);
    if (!window) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return -1; }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) { fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError()); SDL_DestroyWindow(window); return -1; }

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (!font) { fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError()); }

    srand((unsigned)time(NULL));

    // start simulated feed thread
    start_simulation_feed();
    return 0;
}

void render_flight_row(Flight *f, int idx, int x, int y, int w, int h) {
    // Background card
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, 255, 250, 253, 255); // off-white
    SDL_RenderFillRect(renderer, &rect);

    // Status accent
    if (strcmp(f->status, "Scheduled") == 0) SDL_SetRenderDrawColor(renderer, 215, 238, 255, 255);
    else if (strcmp(f->status, "Boarding") == 0) SDL_SetRenderDrawColor(renderer, 223, 247, 230, 255);
    else if (strcmp(f->status, "Delayed") == 0) SDL_SetRenderDrawColor(renderer, 255, 230, 235, 255);
    else SDL_SetRenderDrawColor(renderer, 232, 234, 238, 255);
    SDL_Rect accent = { x, y, 8, h };
    SDL_RenderFillRect(renderer, &accent);

    // Text (flight id)
    if (font) {
        SDL_Color col = { 34, 34, 34, 255 };
        char buf[256];
        snprintf(buf, sizeof(buf), "%s  %s -> %s", f->id, f->origin, f->dest);
        SDL_Surface *s = TTF_RenderText_Blended(font, buf, col);
        SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
        SDL_Rect dst = { x+16, y+8, s->w, s->h };
        SDL_RenderCopy(renderer, t, NULL, &dst);
        SDL_FreeSurface(s); SDL_DestroyTexture(t);
    }

    // Baggage progress bar
    int barX = x + 16; int barY = y + h - 28; int barW = w - 32; int barH = 12;
    SDL_Rect bg = { barX, barY, barW, barH };
    SDL_SetRenderDrawColor(renderer, 248, 240, 246, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_Rect fg = { barX, barY, (int)(barW * f->baggage_progress / 100.0), barH };
    SDL_SetRenderDrawColor(renderer, 255, 159, 196, 255);
    SDL_RenderFillRect(renderer, &fg);
}

void ui_run(void) {
    SDL_Event e;
    const int ROW_H = 84;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = 0;
                if (e.key.keysym.sym == SDLK_r) {
                    // reschedule selected (demo: pick first)
                    Flight *f = get_flight_by_index(0);
                    if (f) {
                        reschedule_flight(f->id, 30); // +30 minutes demo
                        append_log("operator", "reschedule", f->id);
                    }
                }
                if (e.key.keysym.sym == SDLK_a) {
                    // show AI suggestion (console for demo)
                    Flight *f = get_flight_by_index(0);
                    if (f) {
                        char suggestion[256];
                        ai_get_suggestion(f->id, suggestion, sizeof(suggestion));
                        printf("AI suggestion for %s: %s\n", f->id, suggestion);
                        append_log("assistant", "suggest", suggestion);
                    }
                }
            }
        }

        // animation tick (update all animated values)
        animations_update_all();

        // render
        SDL_SetRenderDrawColor(renderer, 251, 247, 255, 255);
        SDL_RenderClear(renderer);

        int x = 20, y = 20, w = 860;
        int i = 0;
        for (i=0;i<flight_count();++i) {
            Flight *f = get_flight_by_index(i);
            render_flight_row(f, i, x, y + i * (ROW_H + 12), w, ROW_H);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60fps loop (vsync with renderer present vsync)
    }
}

void ui_shutdown(void) {
    stop_simulation_feed();
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}
