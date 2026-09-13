# Flight Security Assistant Demo

This demo implements an Advanced Security Assistant for counterfeit & threat detection at Mauritius airport.

Build & run

Dependencies: SDL2, SDL2_ttf, pthreads

On Debian/Ubuntu:
  sudo apt install libsdl2-dev libsdl2-ttf-dev build-essential

Then:
  cd flight_security_app
  make
  ./bin/flight_security_demo

Controls

- ESC: Quit
- H: print last logs in console
- The app also runs a TCP API on port 43000; use a simple telnet or nc to issue LIST_EVENTS, GET_EVENT idx, HOLD id commands.

Notes

- The AI model is a toy deterministic model embedded in ai_detector.c. A trainer script (not included) would compute weights for production.
- The UI simulates sensor feeds and animates baggage on a conveyor; threat hotspots are visualized as red overlay markers.

Ethics: This demo uses simulated data and is for educational purposes only.
