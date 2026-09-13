#include "onnx_wrapper.h"
#include <stdio.h>

// This is a stub implementation. To enable a real ONNX runtime, compile with -DWITH_ONNX
// and implement these functions to call the ONNX Runtime C API. The stub keeps the demo
// runnable without external dependencies.

#ifndef WITH_ONNX
bool onnx_available(){ return false; }
bool onnx_load_model(const char *path){ (void)path; return false; }
bool onnx_infer(const unsigned char *img, int w, int h, ai_result_t *out){ (void)img; (void)w; (void)h; (void)out; return false; }
#else
// If you define WITH_ONNX, provide implementation that links to ONNX Runtime C API.
// Example: use OrtCreateEnv, OrtCreateSession, OrtRun, etc. This project does not
// include ONNX libraries; you must provide them when building.
#include <onnxruntime_c_api.h>

// Placeholder; real code should manage global model/session handles
static bool g_loaded = false;
bool onnx_available(){ return g_loaded; }
bool onnx_load_model(const char *path){
    // load model via ONNX Runtime C API - IMPLEMENT in your environment
    printf("ONNX: load_model stub called with %s (compile WITH_ONNX to enable)\n", path);
    g_loaded = false; // set true if load succeeds
    return g_loaded;
}
bool onnx_infer(const unsigned char *img, int w, int h, ai_result_t *out){
    (void)img; (void)w; (void)h; (void)out;
    // run inference and fill 'out'
    return false;
}
#endif
