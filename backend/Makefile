CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g

BUILD_DIR = build
CLI_BIN = $(BUILD_DIR)/farmacia_cli
SHARED_LIB = $(BUILD_DIR)/libfarmacia.so

all: $(CLI_BIN) $(SHARED_LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLI_BIN): src/main.c src/farmacia.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(SHARED_LIB): src/farmacia.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean