#ifndef ONNX_WRAPPER_H
#define ONNX_WRAPPER_H

#include <stdbool.h>
#include "ai_stub.h"

// Returns true if an ONNX runtime is available and a model is loaded
bool onnx_available();

// Load ONNX model from path. Returns true on success.
bool onnx_load_model(const char *path);

// Run inference on a grayscale image (w*h). Returns true and fills out on success.
bool onnx_infer(const unsigned char *img, int w, int h, ai_result_t *out);

#endif
