# Autonomous Agents feature

This directory contains a Phase-1 prototype for an autonomous agents system for the Mauritian airport project.

Structure
- autonomous/backend/ - C backend simulation and TCP control server
  - src/ - C sources
  - Makefile - builds bin/autonomous_server
- autonomous/frontend/ - Python + Pygame frontend that connects to the backend and visualizes agents
- autonomous/demo_scenarios/ - example scenario files

How to build the backend (Linux)
1. cd autonomous/backend
2. make
3. ./bin/autonomous_server

How to run the frontend (Python)
1. pip install pygame
2. python frontend/frontend.py --connect 127.0.0.1:9292

Behavior
- The backend simulates a small set of agents running simple state machines and exposes a tiny text protocol on TCP port 9292.
- The frontend polls GET /agents to retrieve agent JSON and renders them. You can click agents to see status and (in future phases) send commands.

Notes
- This is a Phase-1 baseline; Phase-2 will add pathfinding, richer sensors, more agent types, UI actions, and incident orchestration.
