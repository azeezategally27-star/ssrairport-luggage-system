CC = gcc
CFLAGS = -Wall -g -I./src
LDFLAGS = -lpthread

SRC = src/sim_server.c src/agent_core.c src/sensors.c src/scheduler.c
OBJ = $(SRC:.c=.o)
BIN = bin/autonomous_server

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin logs
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
