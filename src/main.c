#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "renderer.h"
#include "xray_simulator.h"
#include "ai_stub.h"
#include "sensors.h"
#include "logging.h"
#include "scene_state.h"

int main(int argc, char **argv){
    bool sim = true;
    const char *model_path = NULL;
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"--sim")==0) sim = true;
        if(strcmp(argv[i],"--no-sim")==0) sim = false;
        if(strcmp(argv[i],"--model")==0 && i+1<argc) model_path = argv[++i];
    }

    printf("Starting X-ray demo (sim=%d)\n", sim);

    logging_init("demo/xray_evidence");
    sensors_init();
    ai_init(true); // demo mode
    if(model_path){
        if(onnx_load_model(model_path)) printf("ONNX model loaded: %s\n", model_path);
        else printf("ONNX model failed to load: %s\n", model_path);
    }

    if(!renderer_init()){
        fprintf(stderr, "Failed to initialize renderer\n");
        return 1;
    }

    conveyor_init();

    bool running = true;
    Uint32 last = SDL_GetTicks();
    while(running){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if(ev.type == SDL_QUIT) running = false;
            if(ev.type == SDL_KEYDOWN){
                if(ev.key.keysym.sym == SDLK_ESCAPE) running = false;
                if(ev.key.keysym.sym == SDLK_SPACE) conveyor_toggle_hold();
            }
        }

        sensors_update();
        conveyor_update();

        if(conveyor_bag_in_tunnel()){
            xray_frame_t frame;
            xray_render_current(&frame);
            ai_result_t res = ai_run_on_frame(&frame);
            sensors_readout_t sr = sensors_get_readout();
            float fused = ai_fuse_with_sensors(&res, &sr);
            res.threat_score = fused;

            // publish result for renderer overlays
            scene_state_set_result(&res);

            if(res.threat_score > 0.75f){
                logging_save_evidence(&frame, &res, &sr);
                conveyor_hold_current();
            } else {
                free(frame.pixels);
            }
        }

        renderer_frame();

        Uint32 now = SDL_GetTicks();
        Uint32 dt = now - last;
        if(dt < 16) SDL_Delay(16 - dt);
        last = SDL_GetTicks();
    }

    renderer_shutdown();
    ai_shutdown();
    sensors_shutdown();
    logging_shutdown();
    return 0;
}
