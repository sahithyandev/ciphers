CC = cc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

SRC_DIR = src
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
BINS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRCS))

all: $(BINS)

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

test: all
	@for t in tests/*.sh; do bash "$$t" || exit 1; done

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean test
