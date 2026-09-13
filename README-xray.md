# X-ray Security Prototype (Mauritian Airport)

This branch contains a native C99 desktop prototype (SDL2 + OpenGL) that simulates a refined X-ray luggage security workflow with a purple & white operator UI, sensor stubs, a readable C "C-cheat" AI stub, and evidence logging.

Overview
- Language: C (C99)
- Rendering: SDL2 + OpenGL (legacy fixed-function for portability)
- Demo mode: runs without ML or external hardware
- Branch: feature/xray-security-refined

Build (Linux)

Requirements:
- gcc, make
- SDL2 development libraries (libsdl2-dev)
- OpenGL development headers (libgl1-mesa-dev)

Install on Debian/Ubuntu:

sudo apt update
sudo apt install build-essential libsdl2-dev libgl1-mesa-dev

Build and run:

make
./bin/xray_demo --sim

Windows (MSYS2)
- Install MSYS2 and packages: mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-mesa
- Open MinGW64 shell, run: make, then ./bin/xray_demo.exe --sim

Files added
- README-xray.md (this file)
- Makefile
- src/main.c
- src/renderer.c / src/renderer.h
- src/xray_simulator.c / src/xray_simulator.h
- src/ai_stub.c / src/ai_stub.h
- src/sensors.c / src/sensors.h
- src/logging.c / src/logging.h
- assets/sample_scenario.txt

Description
- The app simulates luggage moving on a conveyor, generates synthetic X-ray frames, runs the ai_stub to detect prohibited items, fuses sensor signals, computes a threat score, and triggers countermeasures (pause conveyor, save evidence).
- All AI and fusion logic is implemented in plain C with comments to make it easy to inspect.

Privacy & ethics
- Demo-only: evidence is stored locally in demo/xray_evidence/; not encrypted by default. See README-xray.md for details.
