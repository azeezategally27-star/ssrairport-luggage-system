# SSRAirport 3D Ledger Demo

This folder contains a lightweight Electron + Three.js demo of a local blockchain-style ledger for luggage events. It's intentionally simple and cute for presentation.

Run locally:

- cd frontend_3d
- npm install
- npm run start

Notes:
- The ledger runs entirely in the renderer process using the Web Crypto API for SHA-256 hashes.
- Use the buttons to add blocks, tamper and validate the chain.
- The app exports ledger.json for inclusion in reports.
