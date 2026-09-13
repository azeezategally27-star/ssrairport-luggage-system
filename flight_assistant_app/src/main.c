/*
 * flight_assistant_app/src/main.c
 * Demo Flight Assistant Panel (SDL2)
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ui.h"
#include "data.h"

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize demo data
    load_demo_data("flights_demo.json");

    // Start UI (blocks into ui.c for clarity)
    if (ui_init() != 0) {
        fprintf(stderr, "UI init failed\n");
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    ui_run();

    ui_shutdown();
    save_logs("flight_assistant_logs.json");

    TTF_Quit();
    SDL_Quit();
    return 0;
}
