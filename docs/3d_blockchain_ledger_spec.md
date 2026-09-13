# 3D Blockchain Ledger — Product Feature Specification (Pink-themed luggage ledger for SSRAirport)

Overview

This document describes a demo 3D desktop/web application that visualises a simplified blockchain ledger integrated into an airport luggage ecosystem. The goal is a cute, light pink-themed, highly interactive 3D demo that demonstrates how luggage events can be recorded as blocks on a ledger and inspected visually. The demo is intended for a Software Engineering module presentation — it's educational and not production-ready.

Goals

- Provide a small interactive 3D scene where each blockchain block is a stylised cube connected by links.
- Allow operations: add block (record luggage event), inspect block details, validate chain (visual indicator), export ledger (JSON), and simulate consensus (animated confirmation).
- Integrated with the luggage concept: blocks store passenger ID, luggage ID, event type (check-in, scan, loaded, cleared), timestamp, and a simple hash linking to previous block.
- Pink/light visual theme with cute UI elements and a sidebar for controls and logs.
- Frontend-first demo implemented with Three.js and vanilla JS inside an Electron wrapper (so it's a desktop demo like the previous feature). No real distributed blockchain — ledger is local and deterministic for demo.

Target users

- Course evaluators and classmates.
- Stakeholders evaluating visual design and integration ideas for luggage tracking.

High-level user flow

1. Start the Electron app (frontend_3d). It loads a 3D scene showing the blockchain as a horizontal chain of cubes.
2. The left panel provides biometric and luggage controls (add luggage event, create passenger, export ledger).
3. When "Add Block" is pressed, a dialog collects simple data (passenger ID, luggage ID, event type). A new block is created with a hash computed from its contents and previous block hash; the block is animated into the chain.
4. Clicking on any cube shows its details in the right panel: full payload, hash, previous hash, and a simple verification button. Verified blocks highlight green; tampered blocks highlight red.
5. A "Simulate Tamper" button allows demonstration of chain integrity failure (changing block data breaks hashes downstream) to show why block chaining matters.
6. Users can export the ledger as JSON for reporting.

Detailed architecture

- frontend_3d/
  - Electron wrapper to run the app on desktop
  - index.html, styles.css: layout and pink theme
  - app.js: Three.js scene setup, block/chain logic, UI bindings
  - assets/: small cute icons and a recreated screenshot image (diagram) for the report

- Ledger model (in JS)
  - Block: { index, timestamp, passenger_id, luggage_id, event, data, prev_hash, hash }
  - Chain: array of Blocks; functions: addBlock(payload), computeHash(block), isValidChain()
  - For hash: use a simple SHA-256 (Web Crypto or a small JS function) to compute deterministic hash for demo.

UI and controls (left panel)

- New Block: opens modal to input passenger ID, luggage ID, and event type (dropdown: Check-in, Security Scan, Loaded, Cleared)
- Add Random Event: create a canned event for demo speed
- Simulate Tamper: pick a block and modify its content (demo only)
- Validate Chain: run validation algorithm and visually mark each block
- Export Ledger: download ledger.json
- Theme toggle: show/hide pink accent or switch to light mode (demo)

3D Visual design details

- Blocks are rounded cubes (use a small bevel effect via geometry subdivision or textured materials) with pastel pink material.
- Edges between blocks are thin glowing rods (light pink) with soft bloom-like shader (simple emissive material) to show connectivity.
- Camera: orbit controls so the user can rotate, pan and zoom. Desktop pointer interactivity: hover shows tooltip, click selects.
- Animation: when adding a block, it spawns from a small puff and slides into position; validation triggers color flashes.

Recreated screenshot (assets/screenshot_3d.svg)

- Included is an SVG recreation of a cute pink-themed interface showing a 3D chain (stylised) and UI panels. Use it in reports and slides.

How the blockchain simulation works (technical)

- Each block's hash is computed as SHA-256 over: index | timestamp | passenger_id | luggage_id | event | prev_hash
- Adding a block sets prev_hash to the previous head's hash and computes the new hash.
- validateChain() iterates and recomputes hashes; if any mismatch is found, that block is flagged and all downstream blocks can be marked invalid.
- Tampering simply mutates a block's passenger_id or event to demonstrate detection.

Why JS + Three.js

- Three.js is lightweight, well-supported, and easy to prototype 3D UI interactions. For a demo with cute visuals it's ideal.
- Electron wraps the app into a desktop package so you can present it as an app rather than a webpage.

Privacy & Security notes (educational)

- This demo stores passenger_id and luggage_id locally and does not transmit data. For real systems, use strong access controls and privacy-preserving identifiers.
- The blockchain here is a local append-only ledger for demonstration, not a resilient distributed system.

Files to add

- docs/3d_blockchain_ledger_spec.md (this file)
- frontend_3d/
  - package.json
  - main.js (Electron entry)
  - src/index.html
  - src/styles.css
  - src/app.js
  - src/ledger.js (simple chain model)
  - src/assets/screenshot_3d.svg
  - README.md (run instructions)

Commit message suggestion

"feat(demo): add 3D blockchain ledger demo (pink-themed)"

Next steps

I will now push a starter implementation into the repository under frontend_3d/ plus this specification file. The implementation includes a working Three.js scene (simple blocks), UI controls for adding blocks, tampering, validation, and export. It's designed for demo use and to be easy to understand and extend. If you'd like, I can later add more polish (better shading, shadows, or WebGL post-processing effects) or a small C backend to persist ledger to disk.
