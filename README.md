Airport luggage system — SSR Airport demo

This repository contains a set of C-based tools and demo frontends for a
bio-mimetic, agent-driven baggage routing and monitoring system for the
Mauritian SSR Airport (demo / academic). It includes:

- A multi-agent C simulator (msgbus + agents) — airport_sim
- A biometric ncurses UI demo — biometric_app
- A simple TCP backend for remote enroll/verify — backend_server
- An ncurses frontend client that talks to the backend — frontend_client

Build
-----
Dependencies: gcc (C11), pthreads, ncurses

On Debian/Ubuntu:
  sudo apt-get install build-essential libncurses-dev

Build everything:
  make

Run
---
  ./backend_server       # run biometric backend (port 5555)
  ./frontend_client      # ncurses frontend client
  ./airport_sim data/flights.csv   # run simulator
  ./biometric_app        # run in-process ncurses UI

License: demo code for academic use
