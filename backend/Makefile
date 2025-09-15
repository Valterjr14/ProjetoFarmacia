CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g

LIBS = -lpq -lpthread

SRC = src/main.c src/farmacia.c src/civetweb.c
BIN = build/server

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(BIN)
