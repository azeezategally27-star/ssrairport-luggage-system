#include "sensors.h"
#include <stdlib.h>
#include <time.h>

static sensors_readout_t g_readout;

void sensors_init(){ srand((unsigned)time(NULL)); g_readout.metal_detected = false; g_readout.weight_anomaly = 0.0f; }
void sensors_shutdown(){ }

void sensors_update(){
    int r = rand()%1000;
    g_readout.metal_detected = (r < 80); // ~8% chance
    if(rand()%500 < 12) g_readout.weight_anomaly = 0.6f + (rand()%40)/100.0f; else g_readout.weight_anomaly *= 0.9f;
}

sensors_readout_t sensors_get_readout(){ return g_readout; }
