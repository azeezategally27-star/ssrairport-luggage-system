#include "ai_stub.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "sensors.h"

static int g_demo = 1;

void ai_init(bool demo_mode){ g_demo = demo_mode?1:0; srand((unsigned)time(NULL)); }
void ai_shutdown(){ }

ai_result_t ai_run_on_frame(const xray_frame_t *frame){
    ai_result_t r; memset(&r,0,sizeof(r));
    // generate 1-3 detections pseudo-randomly
    int n = 1 + (rand()%3);
    for(int i=0;i<n && i<8;i++){
        detection_t *d = &r.detections[r.count++];
        d->cls = (item_class_t)(rand()%5);
        d->x = 0.1f + 0.2f*i;
        d->y = 0.2f + 0.1f*(i%2);
        d->w = 0.08f + 0.04f*(i%2);
        d->h = d->w * 1.2f;
        d->confidence = 0.5f + ((rand()%50)/100.0f);
    }
    float obj = 0.0f;
    for(int i=0;i<r.count;i++) obj += r.detections[i].confidence*(1.0f + ((int)r.detections[i].cls)/2.0f);
    r.threat_score = 1.0f - (1.0f / (1.0f + obj*0.5f));
    return r;
}

float ai_fuse_with_sensors(const ai_result_t *res, const sensors_readout_t *s){
    float w_obj = 0.7f, w_metal = 0.15f, w_weight = 0.15f;
    float obj = res->threat_score;
    float metal = s->metal_detected ? 1.0f : 0.0f;
    float weight = s->weight_anomaly; // 0..1
    float fused = w_obj*obj + w_metal*metal + w_weight*weight;
    if(fused > 1.0f) fused = 1.0f;
    return fused;
}
