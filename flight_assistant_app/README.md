# Flight Assistant Panel Demo (C / SDL2)

This demo app is a simplified Flight Assistant Panel implemented in C using SDL2 for rendering. It demonstrates a number of advanced features in a single native application with simulated live data (no network or database required for the demo). See docs/flights_assistant_spec.md for the full spec.

Build & run

Dependencies: SDL2, SDL2_ttf, pthreads

On Debian/Ubuntu:
  sudo apt install libsdl2-dev libsdl2-ttf-dev build-essential

Then:
  cd flight_assistant_app
  make
  ./bin/flight_assistant_demo

Controls:
- R: Reschedule selected flight (demo chooses the first flight)
- A: Ask AI assistant for suggestion for the first flight
- ESC: Quit

Notes

- This demo uses a simple simulated feed to change baggage progress over time and demonstrates UI updates and logging. In a full app the AI assistant would be a separate trained model and the app would connect to live feeds via an API or message broker.
