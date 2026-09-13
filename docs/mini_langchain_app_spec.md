# Mini LangChain Graph App — Product Feature Specification (Mauritian Airport Biometric Luggage System)

Overview

This document describes a simple demo desktop application (not a website) that visualises a mini "LangChain-style" graph of AI agents, with highly visual diagrams and a biometric feature set for the Mauritian airport luggage system (E-system). The backend is implemented purely in C (demo daemon + data layer). The frontend and visual assets are implemented with Electron (HTML/CSS/JS) for an easy cross-platform packaged desktop app. This file is a product features/specification report suitable for a Software Engineering module submission.

Goals

- Provide a demo desktop application that visually shows a pipeline/graph of AI agents (mini LangChain graph).
- Implement a biometric module with buttons and flows for passenger verification tied to luggage handling.
- Backend: pure C implementation for the server/processing and simulated agent coordination.
- Frontend: Electron app that shows diagrams, agent nodes, interactive controls and biometric buttons.
- Keep the demo simple: no production-level ML model hosting—use stubs or API calls to external services for heavy inference (optional) and focus on integration/visualisation.

Target users

- Course instructor and evaluators (Software Engineering module).
- Product stakeholders for Mauritian airport e-luggage/biometrics system.
- Demo audience (classmates) — show architecture, interactions, and a working demo.

Key features (summary)

- Visual agent graph: nodes representing retriever, QA agent, biometric agent, policy/decision node, and logger.
- Agent details panel: click a node to see its description, inputs, outputs, and recent messages.
- Biometric panel (Mauritian airport): face, passport MRZ, fingerprint scan buttons, register/verify flows.
- Action buttons: Verify Passenger, Register Biometric, Manual Override, View Audit Log, Export Report (CSV/PDF).
- Backend implemented in C:
  - Simple JSON-over-UNIX-socket TCP API (or loopback TCP) for the frontend to call.
  - Local SQLite (or flatfile) storage driver with simple C wrapper.
  - Agent orchestration logic (demo, deterministic) in C — simulates retrieval, reasoning, and verification steps.
- Frontend uses Electron for visuals, D3.js or cytoscape.js for graph rendering, and simple UI components for biometric flows.

How the app works (high level)

1. Start the C backend daemon. It opens a loopback TCP port (example: 127.0.0.1:42042) or UNIX socket and listens for JSON RPC style commands.
2. Start the Electron frontend app. On load it requests the current graph layout and agent states from the C backend.
3. Frontend renders the mini LangChain graph. Each node is interactive: expand/collapse, view last event, run node.
4. When the user clicks a biometric button (e.g., "Scan Face") the frontend simulates an input capture or accepts an uploaded image file; the image data (or placeholder token) is sent to the C backend for verification.
5. Backend runs verification logic (demo): matches the input against stored biometrics (local DB) and returns a confidence score.
6. The decision node consumes the outputs of the biometric agent and the QA/verification agents to produce a final decision: Allow Luggage Processing / Flag For Manual Check.
7. All events are logged to the audit table. The frontend can request logs and render them; users can export logs.

Detailed architecture

- Frontend (Electron — HTML/CSS/JS)
  - Graph renderer: cytoscape.js (recommended) or D3.js for drawing nodes and edges. Uses custom CSS to style agent nodes (icons for each agent type).
  - UI sections:
    - Left: toolbar + biometric buttons
    - Center: interactive graph canvas
    - Right: agent detail panel + event log
    - Bottom: status bar (backend connection, last action result)
  - Buttons and their behaviour (Biometric panel):
    - Scan Face: open camera (if available) or upload image -> POST to backend endpoint /biometric/face/scan
    - Scan Passport MRZ: open file or OCR from image -> POST to /biometric/passport
    - Scan Fingerprint: simulated scan dialog -> POST to /biometric/fingerprint
    - Verify Passenger: aggregate biometric inputs for a passenger ID -> POST /biometric/verify
    - Register Biometric: add new user templates to local DB -> POST /biometric/register
    - Manual Override: mark current luggage/passenger as manually cleared -> POST /actions/manual_override
    - View Audit Log: open logs panel (pull /logs) and show entries
    - Export Report: export current selection or logs as CSV/PDF via /export

- Backend (C)
  - Components:
    - main.c (daemon entrypoint): sets up listeners, handles incoming JSON messages, spawns worker threads for long tasks.
    - api.c / api.h: JSON parsing and request routing (use cJSON or jsmn for parsing). Endpoints: /graph, /node/run, /biometric/*, /logs, /export
    - agents.c / agents.h: agent implementations (simulated). Each agent provides a function signature like: AgentResult run_agent(AgentInput *in).
    - storage.c / storage.h: small wrapper around SQLite (libsqlite3) or a flatfile. Provide functions: db_init(), db_lookup_biometric(), db_insert_template(), db_write_log().
    - ipc.c / ipc.h: loopback TCP or UNIX socket communication layer. Simple request-response JSON.
    - utils.c / utils.h: helper functions for image hashing (pHash/simhash stub), string handling, logging.

  - Why C: the assignment requested backend purely in C. We keep third-party dependencies minimal: libsqlite3 and cJSON or jsmn. Use pthreads for concurrency.

  - Example (simplified) request flow:
    - Frontend -> POST JSON: { "action":"scan_face","payload": { "image_path":"/tmp/face.jpg", "session_id":"S123" }}
    - Backend (ipc) receives message, calls agents_run_face_verifier(image_path)
    - Face verifier computes a simple perceptual hash and compares with DB entries -> returns { match: true, id: "P007", score: 0.87 }
    - Backend logs the event and returns { status: "ok", match: true, id: "P007", score: 0.87 }

Mini LangChain graph (logical)

- Nodes:
  - DocumentRetriever (index / policies / flight manifests)
  - QA Agent (answers questions about rules and baggage restrictions)
  - Biometric Agent (handles face/passport/fingerprint verification)
  - Decision Node (policy evaluation combining QA + Biometric)
  - Logger (audit and export)

Mermaid-style diagram (for the report / visuals)

```mermaid
flowchart LR
  DR[Document Retriever]\nstyle DR fill:#DDEEFF,stroke:#333,stroke-width:1px
  QA[QA Agent]\nstyle QA fill:#FFDDAA,stroke:#333,stroke-width:1px
  BIO[Biometric Agent]\nstyle BIO fill:#DFFFD6,stroke:#333,stroke-width:1px
  DEC[Decision Node]\nstyle DEC fill:#FFDDDD,stroke:#333,stroke-width:1px
  LOG[Logger/Audit]

  DR --> QA
  QA --> DEC
  BIO --> DEC
  DEC --> LOG
```

(Frontend will render a prettier version with cytoscape.js and icons.)

UI mockups (ASCII / labelled)

[Toolbar] [Biometric Buttons]

  +-----------------------------+    +-----------------------+
  | Graph Canvas                |    | Agent Details / Log   |
  |  (nodes + edges)            |    |  - Node Name          |
  |                             |    |  - Inputs / Outputs   |
  +-----------------------------+    +-----------------------+
  [Status: Backend: Connected | Last: Verify OK]

Biometric button list (with descriptions)

- Scan Face: capture or upload face image for matching against DB templates. Quick response.
- Scan Passport MRZ: OCR the MRZ zone and match MRZ to passenger records.
- Scan Fingerprint: open fingerprint modal and simulate template capture/match.
- Verify Passenger: run combined biometric match and return decision.
- Register Biometric: create a new biometric template for a passenger ID.
- Manual Override: allow agent or operator to mark luggage as cleared despite low confidence.
- View Audit Log: open detailed event history related to current luggage/passenger.
- Export Report: create a CSV/PDF summary of activities for a shift.

Data model (simplified)

- passengers (id TEXT PRIMARY KEY, name TEXT, passport TEXT, registered_at TIMESTAMP)
- biometrics (id INTEGER PRIMARY KEY AUTOINCREMENT, passenger_id TEXT, type TEXT, template BLOB, created_at TIMESTAMP)
- logs (id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TIMESTAMP, event_type TEXT, details TEXT)
- luggage (id TEXT PRIMARY KEY, passenger_id TEXT, status TEXT, created_at TIMESTAMP)

Security and privacy notes (educational/demo)

- For a real deployment: never store raw biometric images unencrypted; store templates, use secure enclave/TPM, apply encryption at rest and in transit, apply access controls and lawful collection practices.
- This demo stores data locally and uses simulated matching — it's purely educational.

How we built the backend in C (implementation notes)

- Build system: Makefile using gcc. Example targets: all, clean, install. Static linking optional.
- Dependencies: libsqlite3, cJSON (or jsmn), pthreads. Keep implementations small and well-commented.
- Image hashing: use a tiny pHash-like stub or call out to a simple hashing routine implemented in C (resize to small greyscale, compute DCT stub) — for the demo, a simple average hash is adequate.
- IPC: simple HTTP-like JSON over loopback TCP or a custom JSON-RPC via unix socket.
- Agent orchestration: agents.c contains a small API to register agents and run them synchronously for demo simplicity.

How we built the frontend and assets (implementation notes)

- Use Electron + Node. Structure:
  - src/main.js (Electron main process) — launch window, manage native menus.
  - src/renderer/index.html + index.js — render graph, buttons, and panels.
  - src/assets/ — icons, sample images (add the screenshot here as assets/screenshot.png)
  - src/graph.js — cytoscape initialization and node wiring.
  - src/ipcClient.js — small client that sends JSON to the C backend via TCP socket.

- Graph visuals: cytoscape with custom styles per node type. Hover/click handlers request node details from backend.
- Diagram export: frontend can convert canvas to PNG using existing libraries (cytoscape.js export) and call /export for logs.

Demo mode / limitations

- For the module submission, keep the demo fast, local and deterministic. Use canned biometric templates and passengers in the DB so test cases run predictably.
- ML models are simulated. If you want to plug a real LLM for the QA agent, use an external API key and call the remote service from a small Python microservice, or implement a client in C that calls the remote REST API. For this assignment the behavior can be mocked in C.

Files to add to the repo (suggested)

- docs/mini_langchain_app_spec.md (this file — created now)
- backend/
  - Makefile
  - src/main.c
  - src/api.c/h
  - src/agents.c/h
  - src/storage.c/h
  - src/ipc.c/h
- frontend/
  - package.json
  - src/main.js
  - src/renderer/index.html
  - src/renderer/index.js
  - src/renderer/graph.js
  - src/assets/screenshot.png (placeholder)

Commit message

"docs: add mini LangChain graph app feature spec and demo plan (biometric Mauritian airport)"

Notes and next steps

- I added this specification file to the repository so you can include it in your module submission. Next I can:
  - Create the folder scaffolding (backend and frontend) and add starter files (Makefile, main.c, package.json, Electron starter) in the repository.
  - Or implement a minimal working backend C daemon and a minimal Electron frontend that connects to it.

If you want, I will now push the starter scaffolding (simple Makefile, main.c stub, simple Electron boilerplate) into the repo under the paths listed above. Tell me if you want me to proceed and I will add the files.