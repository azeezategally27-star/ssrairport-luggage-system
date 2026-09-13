CC=gcc
CFLAGS=-Wall -O2 `sdl2-config --cflags`
LDFLAGS=`sdl2-config --libs` -lSDL2_ttf -lpthread
SRC=src/main.c src/ui.c src/data.c src/ai_detector.c src/api.c
BIN=bin/flight_security_demo

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean
