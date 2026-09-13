/*
 * flight_security_app/src/main.c
 * Advanced Security Assistant Demo (SDL2)
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ui.h"
#include "data.h"
#include "api.h"

static volatile int running = 1;

void handle_sigint(int sig) { (void)sig; running = 0; }

int main(int argc, char **argv) {
    signal(SIGINT, handle_sigint);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // load demo data
    data_init();

    // start API server
    api_start(43000);

    // start UI
    if (ui_init() != 0) {
        fprintf(stderr, "UI init failed\n");
        api_stop();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // main UI loop blocks until user quits
    ui_run();

    ui_shutdown();
    api_stop();
    data_shutdown();

    TTF_Quit();
    SDL_Quit();
    return 0;
}
