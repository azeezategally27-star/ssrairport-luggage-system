# 3D Luggage Stress & Damage Analyzer

This prototype is a native (non-web) application written in C using SDL2 + OpenGL.
It demonstrates a rotating 3D "suitcase" (a box model) with a visual heatmap overlay
that indicates structural damage per-vertex, a visible Health Bar, and a quick
refund claim generator that exports a JSON claim file based on damage severity.

Features
- 3D rotating suitcase model with per-vertex damage values mapped to colors (blue=ok, red=severe)
- Click on the suitcase to apply an impact; impacts increase local damage and update the heatmap
- Health bar displayed on-screen shows current luggage "health" (100% -> 0% damage)
- Press 'c' to create an automated refund claim JSON saved to demo/claims/claim_TIMESTAMP.json
- Light background, purple-ish accent for UI (per your preference)

Build & run (Linux example)
1) Install dependencies: sudo apt install build-essential libsdl2-dev libgl1-mesa-dev
2) cd luggage_analyzer
3) make
4) ./bin/luggage_analyzer

Controls
- Left mouse click on the suitcase: apply impact (adds localized damage)
- Arrow keys: rotate view
- Space: toggle rotation on/off
- 'c': create refund claim JSON file (saved to demo/claims)
- ESC or window close: exit

Notes & next steps
- This is a demo prototype; to make the graphics photorealistic swap in an actual 3D suitcase model (OBJ/GLTF) and use modern GLSL shaders for advanced lighting and normal mapping.
- For production, replace the simple damage propagation with a physics-based simulation (finite element method) or integrate with a forensic damage analysis model.

Files of interest
- src/main.c : application entry, input handling, saving claims
- src/renderer.c : mesh creation, OpenGL drawing and heatmap coloring
- src/analysis.c : damage model (apply impact, compute health, refund mapping)

