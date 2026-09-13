#include "renderer.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include "scene_state.h"

static SDL_Window *win = NULL;
static SDL_GLContext glctx = NULL;

// conveyor state
static int bag_z = -200;
static int bag_speed = 2;
static int hold = 0;

bool renderer_init(){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){ fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError()); return false; }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    win = SDL_CreateWindow("X-ray Security - MRU Prototype",
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           1280, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!win){ fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError()); return false; }
    glctx = SDL_GL_CreateContext(win);
    if(!glctx){ fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError()); return false; }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.98f, 0.98f, 0.99f, 1.0f); // light white background
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(60.0, 1280.0/800.0, 0.1, 2000.0); glMatrixMode(GL_MODELVIEW);
    return true;
}

void renderer_shutdown(){ if(glctx) SDL_GL_DeleteContext(glctx); if(win) SDL_DestroyWindow(win); SDL_Quit(); }

void conveyor_init(){ bag_z = -200; hold = 0; }
void conveyor_update(){ if(hold) return; bag_z += bag_speed; if(bag_z > 300) bag_z = -200; }
void conveyor_hold_current(){ hold = 1; }
void conveyor_toggle_hold(){ hold = !hold; }

bool conveyor_bag_in_tunnel(){ return (bag_z > -20 && bag_z < 20); }

static void draw_suitcase(float z){
    glPushMatrix(); glTranslatef(0.0f, 2.0f, z);
    glColor3f(0.6f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(-10,2,-6); glVertex3f(10,2,-6); glVertex3f(10,2,6); glVertex3f(-10,2,6);
    glVertex3f(-10,-2,-6); glVertex3f(10,-2,-6); glVertex3f(10,-2,6); glVertex3f(-10,-2,6);
    glEnd();
    glPopMatrix();
}

static void draw_detection_overlay(const ai_result_t *r, int panel_x, int panel_y, int panel_w, int panel_h){
    if(r->count == 0) return;
    // Draw bounding boxes
    for(int i=0;i<r->count;i++){
        const detection_t *d = &r->detections[i];
        float x = d->x * panel_w + panel_x;
        float y = d->y * panel_h + panel_y;
        float w = d->w * panel_w;
        float h = d->h * panel_h;
        // rectangle
        glLineWidth(2.0f);
        glColor4f(0.42f, 0.26f, 0.90f, 1.0f); // purple
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x+w, y);
        glVertex2f(x+w, y+h);
        glVertex2f(x, y+h);
        glEnd();
        // filled translucent box for confidence
        float alpha = 0.25f * d->confidence;
        glColor4f(0.42f, 0.26f, 0.90f, alpha);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x+w, y);
        glVertex2f(x+w, y+h);
        glVertex2f(x, y+h);
        glEnd();
    }
    // heatmap: coarse grid accumulation
    const int gx = 16, gy = 16;
    float grid[gx*gy]; memset(grid,0,sizeof(grid));
    for(int i=0;i<r->count;i++){
        const detection_t *d = &r->detections[i];
        int left = (int)(d->x * gx);
        int top = (int)(d->y * gy);
        int right = (int)((d->x + d->w) * gx);
        int bottom = (int)((d->y + d->h) * gy);
        if(left<0) left=0; if(top<0) top=0; if(right>gx) right=gx; if(bottom>gy) bottom=gy;
        for(int yy=top; yy<bottom; yy++) for(int xx=left; xx<right; xx++) grid[yy*gx + xx] += d->confidence;
    }
    // draw heatmap cells
    float maxv = 0.0f; for(int i=0;i<gx*gy;i++) if(grid[i]>maxv) maxv = grid[i];
    if(maxv <= 0.0f) return;
    for(int yy=0; yy<gy; yy++){
        for(int xx=0; xx<gx; xx++){
            float v = grid[yy*gx + xx] / maxv; if(v<=0.01f) continue;
            float cell_x = panel_x + (xx * (float)panel_w / gx);
            float cell_y = panel_y + (yy * (float)panel_h / gy);
            float cell_w = (float)panel_w / gx; float cell_h = (float)panel_h / gy;
            // heat color: red-yellow based on v
            float rcol = fminf(1.0f, v*1.6f);
            float gcol = fminf(1.0f, v*0.8f);
            glColor4f(rcol, gcol, 0.1f, 0.35f * v);
            glBegin(GL_QUADS);
            glVertex2f(cell_x, cell_y);
            glVertex2f(cell_x + cell_w, cell_y);
            glVertex2f(cell_x + cell_w, cell_y + cell_h);
            glVertex2f(cell_x, cell_y + cell_h);
            glEnd();
        }
    }
}

void renderer_frame(){
    int w,h; SDL_GetWindowSize(win,&w,&h);
    glViewport(0,0,w,h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(150,120,260, 0,0,0, 0,1,0);

    // ground
    glColor3f(0.98f,0.98f,0.98f); glBegin(GL_QUADS); glVertex3f(-500,-5,-500); glVertex3f(500,-5,-500); glVertex3f(500,-5,500); glVertex3f(-500,-5,500); glEnd();

    // conveyor
    glColor3f(0.7f,0.7f,0.7f);
    glBegin(GL_QUADS);
    glVertex3f(-120,0,-300); glVertex3f(120,0,-300); glVertex3f(120,0,300); glVertex3f(-120,0,300);
    glEnd();

    // tunnel
    glPushMatrix(); glTranslatef(0,40,0);
    glColor3f(1.0f,1.0f,1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-60,-30,-20); glVertex3f(60,-30,-20); glVertex3f(60,30,-20); glVertex3f(-60,30,-20);
    glVertex3f(-60,-30,20); glVertex3f(60,-30,20); glVertex3f(60,30,20); glVertex3f(-60,30,20);
    glEnd();
    glPopMatrix();

    // suitcase
    draw_suitcase((float)bag_z);

    // UI overlay: purple operator panel
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); glOrtho(0,w, h,0,-1,1); glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    // panel background
    glColor3f(0.95f,0.95f,0.98f);
    glBegin(GL_QUADS); glVertex2f(10,10); glVertex2f(360,10); glVertex2f(360,200); glVertex2f(10,200); glEnd();
    // purple header
    glColor3f(0.42f,0.26f,0.90f);
    glBegin(GL_QUADS); glVertex2f(10,10); glVertex2f(360,10); glVertex2f(360,40); glVertex2f(10,40); glEnd();

    // radiograph panel on right
    int pr_w = 320, pr_h = 320;
    int pr_x = w - pr_w - 20;
    int pr_y = 20;
    // panel background
    glColor3f(0.12f, 0.12f, 0.12f);
    glBegin(GL_QUADS); glVertex2f(pr_x, pr_y); glVertex2f(pr_x+pr_w, pr_y); glVertex2f(pr_x+pr_w, pr_y+pr_h); glVertex2f(pr_x, pr_y+pr_h); glEnd();

    // draw overlay detections if any
    ai_result_t res; memset(&res,0,sizeof(res));
    scene_state_get_result(&res);
    draw_detection_overlay(&res, pr_x, pr_y, pr_w, pr_h);

    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);

    SDL_GL_SwapWindow(win);
}
