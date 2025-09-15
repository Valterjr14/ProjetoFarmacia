CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = src/main.c src/farmacia.c
OBJ = $(SRC:.c=.o)
BIN = build/farmacia

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f src/*.o $(BIN)
