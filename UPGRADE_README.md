# Modern-GL + ONNX upgrade for X-ray prototype

This branch adds a modern OpenGL renderer and an optional ONNX wrapper hook. Default builds use the modern renderer and the ONNX wrapper is a no-op stub unless compiled with a real ONNX runtime (see onnx_wrapper.c comments).

Build notes
- Requires SDL2 and an OpenGL 3.3+ capable environment.
- Linux: libsdl2-dev libgl1-mesa-dev
- To enable ONNX runtime integration, compile with -DWITH_ONNX and ensure you link against ONNX Runtime C libraries (not included here).

Files added/changed in this push:
- src/renderer.c   (replaced with modern GL/GLSL renderer)
- shaders/vertex.glsl
- shaders/fragment.glsl
- src/onnx_wrapper.h
- src/onnx_wrapper.c  (stub; define WITH_ONNX to implement real calls)
- src/ai_stub.c      (updated to call ONNX wrapper when available)
- Makefile (updated to include shader files in repo; modern renderer compiled by default)

The previous legacy renderer is replaced on this branch with a modern renderer that loads shaders at runtime from the shaders/ directory.
