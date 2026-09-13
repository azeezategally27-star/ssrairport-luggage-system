CC=gcc
CFLAGS=-Wall -O2
SRC=src/main.c src/agents.c src/storage.c
OBJ=$(SRC:.c=.o)
BIN=bin/ssrairportd

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN) $(OBJ)
