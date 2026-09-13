CC = gcc
CFLAGS = -Wall -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lGL -lm -lpthread

SRC = src/main.c src/renderer.c src/xray_simulator.c src/ai_stub.c src/sensors.c src/logging.c
OBJ = $(SRC:.c=.o)
BIN = bin/xray_demo

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin demo/xray_evidence
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
