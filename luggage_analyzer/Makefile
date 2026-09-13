CC = gcc
CFLAGS = -Wall -g `sdl2-config --cflags` -I./src
LDFLAGS = `sdl2-config --libs` -lGL -lm

SRC = src/main.c src/renderer.c src/analysis.c
OBJ = $(SRC:.c=.o)
BIN = bin/luggage_analyzer

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin demo/claims
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
