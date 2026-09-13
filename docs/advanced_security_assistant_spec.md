# Advanced Security Assistant — Counterfeit & Threat Detection (Mauritius Airport)

Overview

This document describes the Advanced Assistant AI System for threat detection (counterfeit items and potential terrorists) at Mauritius airport. It details a native C demo application (cross-platform) with a smooth, high-framerate SDL2 UI, a simulated live sensor feed, an explainable AI detector implemented in C for deterministic inference, and many advanced features aimed at realistic behavior for presentation and evaluation.

Goals

- Provide a native desktop app (accessible from any device via TCP API) demonstrating advanced security assistant features for luggage and passenger screening.
- Include realistic smooth animations in the UI showing detected threats, baggage X-ray overlays, passenger flow, and operator workflows.
- Implement >12 advanced features (detection, explainability, incident workflows, rescan/hold rules, remote control, audit logs, training pipeline notes, and more).
- Backend and UI are implemented in C (SDL2 for UI) and are well-commented for teaching and extension.

Note: This is a demo educational system and not for production deployment. Real airport security systems require strict certification, controlled data, and legal/ethical oversight.

Why C + SDL2

- The assignment requested a C backend; SDL2 provides a cross-platform, high-performance rendering layer suitable for smooth animations in native apps.
- The app exposes a simple TCP JSON-like API so it can be controlled remotely from mobile/tablet clients for demo interactions.

High-level features (summary)

1) Real-time X-ray feed simulation: animated conveyor with luggage thumbnails and an X-ray overlay showing highlighted suspicious regions.
2) AI detector (explainable): a deterministic logistic regression-style model implemented in C with pre-trained weights (demo). Produces probability scores and per-feature contributions.
3) Multi-sensor fusion: combine X-ray signature score, metal detector reading, weight anomaly, watchlist match, and behavior analytics to produce a final threat score.
4) Threat classification: counterfeit item vs dangerous item vs suspicious behavior — with different workflows.
5) Operator assistant recommendations: Hold, Allow, Secondary Screening, Escalate.
6) Rescan & manual override: operator can request rescan, hold items, or override decisions with PIN authentication (demo PIN flow).
7) Incident ticketing and workflow: create incident, assign team, track resolution.
8) Remote control API: simple TCP interface to query status, issue holds, request rescan (works from mobile devices for demo).
9) Explainability panel: per-feature visualization showing how each feature influenced the final score.
10) Audit logging & export: logs all actions and detections; exportable CSV/JSON for evidence.
11) Replay & timeline: replay recent events with accelerated animations for review.
12) Priority routing & escalation: auto-escalate high-probability threats and animate dedicated security lanes.
13) Synthetic training pipeline notes: provide Python trainer (sample) and explain how weights were derived from labeled demo data.
14) Privacy & compliance notes: explicit guidance for lawful processing.

Architecture (demo)

- UI: SDL2 + SDL2_ttf for text rendering; main thread handles rendering and UI; worker threads simulate sensors and handle TCP API.
- AI: ai_detector.c implements a feature-based model with weights shipped as a JSON-like C array; inference is fast and deterministic.
- Data: data.c keeps an in-memory queue of events and a small persistent log file for audit.
- IPC/API: simple TCP server (127.0.0.1:43000) accepts text commands; responses are simple JSON strings (no external JSON library required for demo).

Files added

- docs/advanced_security_assistant_spec.md (this document)
- flight_security_app/ (app implementation)
  - Makefile
  - src/main.c
  - src/ui.c, src/ui.h
  - src/data.c, src/data.h
  - src/ai_detector.c, src/ai_detector.h
  - src/api.c, src/api.h
  - assets/ (diagram and SVG screenshot)
  - README.md

Next steps

- Build: cd flight_security_app && make
- Run: ./bin/flight_security_demo
- For the presentation: start the app and optionally connect with the remote example client (scripts/remote_client.py) to demonstrate multi-device control.

Ethics & disclaimer

- This demo uses simulated data and deterministic models for education only. Real security systems must undergo regulation and cannot be used with live passenger data without formal authorization.

Contact

If you want, I can push the full code now into the repository and then extend it with: more polished animations, a persistent SQLite store, or a lightweight Python trainer that creates the weight file used by the C detector.
