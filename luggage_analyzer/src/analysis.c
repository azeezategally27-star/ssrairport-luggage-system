#include "analysis.h"
#include <string.h>
#include <math.h>

static float g_damage[VERTEX_COUNT];

void analysis_init(){ for(int i=0;i<VERTEX_COUNT;i++) g_damage[i]=0.0f; }

static float vdist_sq(float x1,float y1,float z1,float x2,float y2,float z2){
    float dx=x1-x2, dy=y1-y2, dz=z1-z2; return dx*dx+dy*dy+dz*dz;
}

void analysis_apply_impact(float lx, float ly, float lz, float strength){
    // suitcase cube vertex local positions (-1..1 cube)
    const float vx[8] = {-1,-1,1,1,-1,-1,1,1};
    const float vy[8] = {-1,1,-1,1,-1,1,-1,1};
    const float vz[8] = {-1,-1,-1,-1,1,1,1,1};
    for(int i=0;i<8;i++){
        float dsq = vdist_sq(lx,ly,lz, vx[i], vy[i], vz[i]);
        // influence decays with squared distance; add damage scaled by strength
        float inf = 1.0f / (1.0f + 4.0f * dsq);
        g_damage[i] += strength * inf;
        if(g_damage[i] > 1.0f) g_damage[i] = 1.0f;
    }
}

void analysis_get_vertex_damage(float out[VERTEX_COUNT]){ memcpy(out, g_damage, sizeof(g_damage)); }

float analysis_compute_health(){
    // health = 1.0 - weighted average damage
    float sum = 0.0f; for(int i=0;i<8;i++) sum += g_damage[i];
    float avg = sum/8.0f; if(avg < 0) avg = 0; if(avg > 1) avg=1;
    return 1.0f - avg; // 1 = perfect
}

float analysis_suggest_refund(){
    float health = analysis_compute_health();
    // simple refund mapping: 90%+ health -> 0% refund, 50-90 -> linear 10-50%, below 50 -> up to 100%
    if(health >= 0.9f) return 0.0f;
    if(health >= 0.5f) {
        float t = (0.9f - health) / 0.4f; // 0..1
        return 0.10f + t * (0.40f);
    }
    // health < 0.5
    float t = (0.5f - health) / 0.5f; if(t>1) t=1;
    return 0.50f + t * 0.50f; // up to 1.0
}
