CC = gcc
CFLAGS = -Wall -g `sdl2-config --cflags` -I./src
LDFLAGS = `sdl2-config --libs` -lGL -lm -lpthread

# sources (modern renderer and optional onnx wrapper are included)
SRC = src/main.c src/renderer.c src/xray_simulator.c src/ai_stub.c src/sensors.c src/logging.c src/onnx_wrapper.c
OBJ = $(SRC:.c=.o)
BIN = bin/xray_demo

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin demo/xray_evidence
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
