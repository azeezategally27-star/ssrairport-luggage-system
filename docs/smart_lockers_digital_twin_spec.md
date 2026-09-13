# Smart Locker Digital Twin — Feature Specification

Overview

This document specifies a fully interactive, customizable 3D digital twin of a smart luggage locker storage system for Sir Seewoosagur Ramgoolam Airport (ideal for tourists who land early and want to roam the island). The demo is a desktop application (Electron + Three.js) that visualises a futuristic grid of metallic lockers with smooth animations: sliding drawers, color-coded states, pulsing high-priority lockers, vacancy, bookings, and administrative controls. It is designed for presentations and product specification reports.

Goals

- Provide a realistic interactive 3D twin of the locker facility for demos and testing.
- Allow dynamic customization (grid size, locker sizes, color-coding rules, animations, pricing rules).
- Include advanced operational features: booking flow, NFC/touchless unlock simulation, maintenance, heatmap analytics, exportable reports, and remote API for kiosk/tablet integration.
- Keep the demo lightweight and cross-platform using Electron + Three.js so it runs offline as a native desktop app.

Key features (detailed)

1) Futuristic 3D metallic locker grid
   - Lockers arranged in rows/columns with configurable levels and columns.
   - Smooth slide-out drawers and opening animations using easing and quaternion smoothing.
   - Metallic PBR-like shader approximation using Three.js StandardMaterial.

2) Color-coded states (configurable)
   - Cyan: booked/reserved.
   - Pulsing orange: high-priority / urgent retrieval.
   - Clean white: vacant/available.
   - Soft green: occupied but idle.
   - Gray: maintenance/out-of-order.
   - Transition animations when state changes (smooth color lerp + bloom-like glow).

3) Booking & reservation flow
   - Simulated user booking: choose locker size, duration, and pay (stubbed payment flow).
   - Generates a QR code (canvas) and reservation token to unlock from kiosk app.
   - Auto-expire and release lockers after reservation end.

4) Touchless unlock & access
   - Simulate NFC/QR/ticket scanning to open lockers via the app.
   - Admin override: open/lock/unlock any locker with PIN.

5) Dynamic priority & alerts
   - Mark lockers as "priority" (pulsing orange) when a user requests urgent retrieval.
   - Notifications panel lists active alerts and recent events.

6) Maintenance mode & health checks
   - Set a locker to maintenance: it slides to a locked position and shows gray color.
   - Simulated sensors: door open, battery for electronic lock, temperature sensor.

7) Heatmap & analytics overlay
   - Visual heatmap showing most-used lockers over time (color intensity overlay).
   - KPIs: occupancy rate, avg usage time, available lockers.

8) Customizer & admin panel
   - Live controls to change grid dimensions, locker size, spacing, animation speed, and state color palette.
   - Save/load configurations to JSON for different terminal layouts.

9) Export & reporting
   - Export session logs and analytics to JSON or CSV for reporting.

10) Scenario scripting & replay
   - Scripted scenarios (peak morning arrivals) and replay mode with time scaling for demos.

11) Remote API (local loopback)
   - Simple HTTP/TCP JSON API for kiosk/tablet integrations: list lockers, book, unlock, query status.

12) Accessibility & UX polish
   - Large readable labels, keyboard navigation, color-blind friendly palette toggle, and light-themed UI.

Architecture & implementation notes

- Electron for desktop shell; Three.js for 3D rendering.
- All logic runs locally for the demo; locker state persisted to local JSON files in the app folder.
- Animations are time-step independent and use easing functions for visually appealing motion.
- The demo includes placeholder stubs for payment and NFC; these can be replaced by real integrations.

Run instructions

- cd frontend_lockers_3d
- npm install
- npm run start

Files added

- docs/smart_lockers_digital_twin_spec.md (this document)
- frontend_lockers_3d/
  - package.json, main.js
  - src/index.html, src/styles.css
  - src/app.js, src/locker.js
  - src/assets/diagram_lockers.svg
  - README.md

Next steps

- Replace box lockers with glTF models and PBR materials for higher fidelity.
- Add an optional lightweight backend (Node/Express) to persist multi-user bookings and support multiple kiosk clients.
- Integrate a real QR/NFC library for kiosk hardware.

