#include "renderer.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>

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
    glClearColor(0.95f, 0.95f, 0.98f, 1.0f); // light white background
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(60.0, 1280.0/800.0, 0.1, 2000.0); glMatrixMode(GL_MODELVIEW);
    return true;
}

void renderer_shutdown(){ if(glctx) SDL_GL_DeleteContext(glctx); if(win) SDL_DestroyWindow(win); SDL_Quit(); }

void conveyor_init(){ bag_z = -200; hold = 0; }
void conveyor_update(){ if(hold) return; bag_z += bag_speed; if(bag_z > 300) bag_z = -200; }
void conveyor_hold_current(){ hold = 1; }
void conveyor_toggle_hold(){ hold = !hold; }

bool conveyor_bag_in_tunnel(){ return (bag_z > -20 && bag_z < 20); }

// draw a simple suitcase at given z
static void draw_suitcase(float z){
    glPushMatrix(); glTranslatef(0.0f, 2.0f, z);
    glColor3f(0.6f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    // top
    glVertex3f(-10,2,-6); glVertex3f(10,2,-6); glVertex3f(10,2,6); glVertex3f(-10,2,6);
    // sides
    glVertex3f(-10,-2,-6); glVertex3f(10,-2,-6); glVertex3f(10,-2,6); glVertex3f(-10,-2,6);
    glEnd();
    glPopMatrix();
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

    // suitcase on conveyor
    draw_suitcase((float)bag_z);

    // UI overlay: purple operator panel
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); glOrtho(0,w, h,0,-1,1); glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    // panel background
    glColor3f(0.95f,0.95f,0.98f);
    glBegin(GL_QUADS); glVertex2f(10,10); glVertex2f(360,10); glVertex2f(360,200); glVertex2f(10,200); glEnd();
    // purple header
    glColor3f(0.42f,0.26f,0.90f);
    glBegin(GL_QUADS); glVertex2f(10,10); glVertex2f(360,10); glVertex2f(360,40); glVertex2f(10,40); glEnd();
    // text placeholder (no text rendering in this simple renderer)
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);

    SDL_GL_SwapWindow(win);
}
