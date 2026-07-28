CC = cc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

SRC_DIR = src
BIN_DIR = bin

TEST_DIR = tests

SRCS = $(wildcard $(SRC_DIR)/*.c)
BINS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRCS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/tests/%,$(TEST_SRCS))

all: $(BINS)

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/tests/%: $(TEST_DIR)/%.c $(SRC_DIR)/%.c | $(BIN_DIR)/tests
	$(CC) $(CFLAGS) -DUNIT_TEST $< -o $@

$(BIN_DIR) $(BIN_DIR)/tests:
	mkdir -p $@

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do ./$$t || exit 1; done

coverage: CFLAGS += -fprofile-instr-generate -fcoverage-mapping
coverage: clean $(TEST_BINS)
	@for t in $(TEST_BINS); do LLVM_PROFILE_FILE=$$t.profraw ./$$t; done
	@xcrun llvm-profdata merge -sparse $(BIN_DIR)/tests/*.profraw -o $(BIN_DIR)/tests/all.profdata
	@xcrun llvm-cov report $(TEST_BINS) -instr-profile=$(BIN_DIR)/tests/all.profdata $(SRC_DIR)/
	@echo "Detail: xcrun llvm-cov show <bin> -instr-profile=$(BIN_DIR)/tests/all.profdata"

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean test coverage
