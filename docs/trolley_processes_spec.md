# Trolley Processes 3D App — Product Feature Specification

Overview

This document describes a native desktop demo application that visualises and simulates trolley operations at Sir Seewoosagur Ramgoolam (Mauritius) airport. The app is an Electron desktop app (so it runs as a native application) with a high-fidelity Three.js 3D scene and refined animations: trolleys moving along paths, loading/unloading luggage, docking at charging stations, queuing at stands, and interactions with ground staff.

Goals

- Build a fully functioning desktop app (Electron) that runs on Windows/macOS/Linux for demo and presentation.
- Provide smooth, realistic 3D animations for trolley movement (path following, easing, suspension tilt), turning, loading, and interactions.
- Expose control panels (left) for simulation controls and telemetry (right) for analytics/selected trolley details.
- Provide scriptable scenarios and an API (local loopback TCP) for remote control during demo.
- Include diagrams and documentation for your report.

High-level features

1) Real-time 3D scene: airport apron section, stands, service roads, charging stations, and trolley models.
2) Trolley behavior: path-following, dynamic speed, collision avoidance, smooth steering and tilt for realism.
3) Loading/unloading animation: extend/retract baggage arms and cargo attachments with easing.
4) Queuing and dispatch logic: trolleys form queues to serve stands; dispatch manager assigns trolleys to new tasks.
5) Charging & battery simulation: battery level decreases with motion, recharges at charging docks.
6) Failure simulation: breakdowns, slowdowns, and reroutes with animation of recovery.
7) Scheduling & live dispatch: assign trolleys to flights, show ETA and progress bars.
8) Analytics: per-trolley metrics (distance, uptime, handled loads) and global KPIs for throughput.
9) Scenario playback & replay: script events and replay for post-ops review.
10) Remote control API: simple TCP local API for scripted demos and remote triggering.
11) Rich UI: light themed pastel UI, color-coded states (idle, moving, loading, charging, fault).
12) Export logs: save session logs and export telemetry to JSON for report.

Implementation notes

- Electron + Three.js is used so the app is a desktop application, not a website.
- Three.js handles 3D rendering; simple geometries and textured materials used so demo remains lightweight.
- Trolley movement uses CatmullRomCurve3 for smooth paths and quaternion slerp for rotation smoothing.
- Animations are time-delta driven to ensure frame-rate independent motion.
- The app includes all source assets and is documented so your examiner can review how it was implemented.

Run instructions

- cd frontend_trolley_3d
- npm install
- npm run start

Files added

- docs/trolley_processes_spec.md: this doc
- frontend_trolley_3d/: Electron + Three.js app
  - package.json, main.js
  - src/index.html, src/styles.css, src/app.js, src/trolley.js
  - src/assets/diagram_trolley.svg
  - README.md

Next steps

I will push the code into the repository and you can run it locally. After that I can:
- Add NPC ground-staff models and passenger agents.
- Replace simple trolley models with low-poly glTF assets.
- Add an exportable animated GIF recorder for slides.

