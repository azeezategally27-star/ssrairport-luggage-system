/* ai_detector.h - simple feature-based detector API */

#ifndef AI_DETECTOR_H
#define AI_DETECTOR_H

// inference: returns probability 0..1
double ai_infer(double xray_sig, double metal_val, double weight_anomaly, double watchlist, double behavior);
// explain: fill contributions array (size 6), returns final score in out
double ai_explain_event(const double features[5], double contributions[6]);
// convenience wrapper that takes an Event pointer
void ai_explain(const Event *e, double out_contrib[6]);

#endif
