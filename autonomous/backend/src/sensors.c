#include "sensors.h"
#include <stdlib.h>
#include <time.h>

static float g_noise = 0.0f;

void sensors_init(){ srand((unsigned)time(NULL)); g_noise = 0.0f; }
void sensors_update(int ms){ g_noise = (rand()%100)/1000.0f; }
float sensors_read_proximity(float x, float y){ (void)x; (void)y; return 0.5f + g_noise; }
float sensors_read_weight(float x, float y){ (void)x; (void)y; return 0.1f + g_noise; }
