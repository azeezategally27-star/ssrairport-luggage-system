#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <stdbool.h>

#define VERTEX_COUNT 8

void analysis_init();
// apply an impact at normalized local coordinates (lx,ly,lz in -1..1) with strength s
void analysis_apply_impact(float lx, float ly, float lz, float strength);
// get damage value per vertex (0..1)
void analysis_get_vertex_damage(float out[VERTEX_COUNT]);
// compute overall health (0..1)
float analysis_compute_health();
// compute suggested refund percentage (0..1) based on damage
float analysis_suggest_refund();

#endif
