CC=gcc
CFLAGS=-Wall -O2 `sdl2-config --cflags`
LDFLAGS=`sdl2-config --libs` -lSDL2_ttf -lpthread
SRC=src/main.c src/ui.c src/data.c src/ai_assistant.c src/animations.c
BIN=bin/flight_assistant_demo

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean
