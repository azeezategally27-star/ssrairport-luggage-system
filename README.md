# SSRAirport Luggage System — Metaverse & X-Ray Prototype

This repository will host a native (non-web) prototype application in C that combines two focused subsystems for the Mauritian airport use-case:

1) A metaverse-style 3D navigation app that simulates the Sir Seewoosagur Ramgoolam International Airport (labelled segments) and mirrors a user's real-world movements using GPS updates from a mobile sender.
2) A refined, native X-ray luggage security simulation and analysis app (white & purple UI theme) designed as a separate module in the same codebase for later integration with AI-based threat detection and sensor fusion.

This commit adds a scaffold and a working prototype main program for the metaverse side (UDP GPS receiver + simple OpenGL/SDL2 renderer). It provides a clear, extendable structure and build instructions so you can iterate toward a full-featured 3D simulation and AI integrations.

Repository layout (this commit)
- README.md (this file)
- Makefile — simple build using pkg-config
- src/main.c — prototype native app: SDL2 + OpenGL window, UDP GPS listener, simple avatar movement & scene placeholder

Design goals
- Native C application (fast, portable) with a modular structure so you can add:
  - High-quality 3D models via Assimp (.obj, .fbx)
  - Physically-based rendering (PBR) using GLSL shaders
  - Secure GPS transport (DTLS/WebSocket) and a mobile client
  - X-ray subsystem (separate process or module) with AI models for threat detection
  - Sensor/IoT integration and advanced analytics

Segments to model and label in-scene
- Arrivals
- Baggage claim
- Customs
- Security screening
- Check-in halls
- Duty-free / Retail
- Boarding gates (Gate A, Gate B, ...)
- Lounges
- Runways & apron
- Parking & ground transport

Dependencies (for prototype build)
- build-essential, gcc
- libsdl2-dev
- libgl1-mesa-dev (OpenGL)
- libcjson-dev (JSON parsing for incoming GPS messages)
- pthreads

Install on Debian/Ubuntu (example)
1. sudo apt update
2. sudo apt install build-essential libsdl2-dev libgl1-mesa-dev libcjson-dev pkg-config
3. make
4. ./bin/ssrairport_app

How GPS sync works (prototype)
- A mobile client (or test script) sends UDP JSON packets to the desktop app. Example packet: {"lat": -20.430, "lon": 57.683, "alt": 6.5, "bearing": 135.0}
- The app converts lat/lon into local scene coordinates using a configurable reference lat/lon and scale factor. The avatar smoothly interpolates to the latest GPS position.

Security & privacy
- This prototype uses unencrypted UDP for simplicity. For production, use authenticated and encrypted channels and require explicit user consent for tracking.

Next steps — prioritized
1. Add a separate module for the X-ray security flow (white + purple UI) under src/xray/
2. Import high-quality 3D models of the Mauritian airport and implement a map of named segments
3. Replace placeholder renderer with a GLSL PBR pipeline and load models via Assimp
4. Add machine learning models for X-ray threat detection (Python/TensorFlow/PyTorch microservice or onnx runtime in C)
5. Build a mobile client for secure GPS streaming or use existing location-sharing apps for testing

Notes
- This scaffold is a starting point. Complete productionization requires art assets, secure mobile clients, AI model training and validation, and user privacy protections.

