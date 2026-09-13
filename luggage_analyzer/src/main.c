#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "analysis.h"

static SDL_Window *win = NULL;
static SDL_GLContext glctx = NULL;
static int width = 1200, height = 800;

// suitcase rotation
static float rotx = -20.0f, roty = 30.0f; static int rotating = 1;

// simple cube vertices
static const float V[8][3] = {
    {-1,-1,-1}, { -1,1,-1}, {1,-1,-1}, {1,1,-1}, {-1,-1,1}, {-1,1,1}, {1,-1,1}, {1,1,1}
};

static void color_from_damage(float d, float *r, float *g, float *b){
    // map 0..1: blue -> cyan -> green -> yellow -> red
    if(d <= 0.0f){ *r=0.0f; *g=0.4f; *b=1.0f; return; }
    if(d >= 1.0f){ *r=1.0f; *g=0.0f; *b=0.0f; return; }
    // interpolate
    if(d < 0.25f){ float t = d/0.25f; *r = 0.0f*(1-t) + 0.0f*t; *g = 0.4f*(1-t) + 1.0f*t; *b = 1.0f*(1-t) + 0.0f*t; return; }
    if(d < 0.5f){ float t = (d-0.25f)/0.25f; *r = 0.0f*(1-t) + 1.0f*t; *g = 1.0f*(1-t) + 1.0f*t; *b = 0.0f*(1-t) + 0.0f*t; return; }
    if(d < 0.75f){ float t = (d-0.5f)/0.25f; *r = 1.0f*(1-t) + 1.0f*t; *g = 1.0f*(1-t) + 0.5f*t; *b = 0.0f*(1-t) + 0.0f*t; return; }
    float t = (d-0.75f)/0.25f; *r = 1.0f*(1-t) + 1.0f*t; *g = 0.5f*(1-t) + 0.0f*t; *b = 0.0f*(1-t) + 0.0f*t; return;
}

static void draw_cube_with_heatmap(){
    float damage[8]; analysis_get_vertex_damage(damage);
    glBegin(GL_QUADS);
    // -Z face
    for(int vi=0;vi<4;vi++){
        int idx = vi==0?0:(vi==1?1:(vi==2?2:3));
        float r,g,b; color_from_damage(damage[idx], &r,&g,&b); glColor3f(r,g,b); glVertex3fv(V[idx]);
    }
    // +Z face
    for(int vi=0;vi<4;vi++){
        int idx = vi==0?4:(vi==1?5:(vi==2?6:7));
        float r,g,b; color_from_damage(damage[idx], &r,&g,&b); glColor3f(r,g,b); glVertex3fv(V[idx]);
    }
    // -X face
    int idxs1[4] = {0,4,6,2}; for(int i=0;i<4;i++){ float r,g,b; color_from_damage(damage[idxs1[i]], &r,&g,&b); glColor3f(r,g,b); glVertex3fv(V[idxs1[i]]); }
    // +X face
    int idxs2[4] = {1,3,7,5}; for(int i=0;i<4;i++){ float r,g,b; color_from_damage(damage[idxs2[i]], &r,&g,&b); glColor3f(r,g,b); glVertex3fv(V[idxs2[i]]); }
    // -Y face
    int idxs3[4] = {0,1,5,4}; for(int i=0;i<4;i++){ float r,g,b; color_from_damage(damage[idxs3[i]], &r,&g,&b); glColor3f(r,g,b); glVertex3fv(V[idxs3[i]]); }
    // +Y face
    int idxs4[4] = {2,6,7,3}; for(int i=0;i<4;i++){ float r,g,b; color_from_damage(damage[idxs4[i]], &r,&g,&b); glColor3f(r,g,b); glVertex3fv(V[idxs4[i]]); }
    glEnd();
}

static void draw_health_bar(float health){
    // 2D overlay
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    int bar_w = 400, bar_h = 28; int bx = 20, by = 20;
    // background
    glColor3f(0.95f,0.95f,0.98f);
    glBegin(GL_QUADS); glVertex2f(bx,by); glVertex2f(bx+bar_w,by); glVertex2f(bx+bar_w,by+bar_h); glVertex2f(bx,by+bar_h); glEnd();
    // fill
    float fill_w = bar_w * health;
    // purple fill
    glColor3f(0.42f,0.26f,0.90f);
    glBegin(GL_QUADS); glVertex2f(bx+4,by+4); glVertex2f(bx+4+fill_w-8,by+4); glVertex2f(bx+4+fill_w-8,by+bar_h-4); glVertex2f(bx+4,by+bar_h-4); glEnd();
    // text - we don't have text rendering; instead draw a simple numeric indicator using line segments approximation
    glColor3f(0.1f,0.1f,0.1f);
    // numeric percentage as tiny squares
    int pct = (int)(health*100.0f);
    // draw pct as a very simple rectangle cluster (placeholder)
    char buf[16]; snprintf(buf,sizeof(buf),"%d%%", pct);
    // no text routine - print to console as well
    printf("Health: %d%%\n", pct);
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

static void save_claim(){
    float damage[8]; analysis_get_vertex_damage(damage);
    float health = analysis_compute_health();
    float refund = analysis_suggest_refund();
    time_t t = time(NULL);
    char fname[256]; strftime(fname, sizeof(fname), "demo/claims/claim_%Y%m%d_%H%M%S.json", localtime(&t));
    FILE *f = fopen(fname, "w"); if(!f){ perror("claim fopen"); return; }
    fprintf(f, "{\n  \"timestamp\": %ld,\n  \"health\": %.4f,\n  \"refund_suggested\": %.4f,\n  \"vertex_damage\": [", (long)t, health, refund);
    for(int i=0;i<8;i++){ fprintf(f, "%.4f%s", damage[i], i<7?", ":""); }
    fprintf(f, "\n  ]\n}\n"); fclose(f);
    printf("Saved claim to %s\n", fname);
}

int main(int argc, char **argv){
    (void)argc; (void)argv;
    if(SDL_Init(SDL_INIT_VIDEO) != 0){ fprintf(stderr, "SDL init failed: %s\n", SDL_GetError()); return 1; }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    win = SDL_CreateWindow("3D Luggage Stress & Damage Analyzer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!win){ fprintf(stderr, "Create window failed: %s\n", SDL_GetError()); return 1; }
    glctx = SDL_GL_CreateContext(win);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.99f,0.99f,0.995f,1.0f); // light background

    analysis_init();

    int running = 1; Uint32 last = SDL_GetTicks();
    while(running){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if(ev.type == SDL_QUIT) running = 0;
            if(ev.type == SDL_KEYDOWN){
                if(ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
                if(ev.key.keysym.sym == SDLK_SPACE) rotating = !rotating;
                if(ev.key.keysym.sym == SDLK_c) save_claim();
                if(ev.key.keysym.sym == SDLK_LEFT) roty -= 8.0f;
                if(ev.key.keysym.sym == SDLK_RIGHT) roty += 8.0f;
                if(ev.key.keysym.sym == SDLK_UP) rotx -= 8.0f;
                if(ev.key.keysym.sym == SDLK_DOWN) rotx += 8.0f;
            }
            if(ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED){ width = ev.window.data1; height = ev.window.data2; glViewport(0,0,width,height); }
            if(ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT){
                int mx = ev.button.x, my = ev.button.y;
                // simple ray pick: map screen coords to cube local space approximately
                // convert to NDC
                float nx = (2.0f*mx)/width - 1.0f; float ny = 1.0f - (2.0f*my)/height;
                // assume projection and camera are default and cube at origin; approximate local coords by inverse rotation
                float angY = roty * M_PI/180.0f; float angX = rotx * M_PI/180.0f;
                // approximate a point on the front face
                float lx = nx * 0.8f; float ly = ny * 0.8f; float lz = 0.0f;
                // transform back by inverse rotations
                float cosY = cosf(-angY), sinY = sinf(-angY);
                float rx = cosY*lx - sinY*lz;
                float rz = sinY*lx + cosY*lz;
                float cosX = cosf(-angX), sinX = sinf(-angX);
                float ry = cosX*ly - sinX*rz;
                float rz2 = sinX*ly + cosX*rz;
                // apply impact at (rx,ry,rz2)
                analysis_apply_impact(rx, ry, rz2, 0.25f);
            }
        }

        Uint32 now = SDL_GetTicks(); Uint32 dt = now - last; last = now;
        if(rotating){ roty += (dt/1000.0f) * 20.0f; }

        glViewport(0,0,width,height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(45.0, (double)width/(double)height, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity(); gluLookAt(0,0,6, 0,0,0, 0,1,0);

        glPushMatrix(); glRotatef(rotx,1,0,0); glRotatef(roty,0,1,0);
        draw_cube_with_heatmap(); glPopMatrix();

        float health = analysis_compute_health(); draw_health_bar(health);

        SDL_GL_SwapWindow(win);
        SDL_Delay(10);
    }

    SDL_GL_DeleteContext(glctx); SDL_DestroyWindow(win); SDL_Quit();
    return 0;
}
