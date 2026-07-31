CC = cc
CFLAGS = -O2 -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude
LLVM_PREFIX := $(if $(shell command -v xcrun 2>/dev/null),xcrun )

SRC_DIR = src
UTILS_DIR = utils
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

$(BIN_DIR)/tests/%: $(TEST_DIR)/%.c $(UTILS_DIR)/%.c | $(BIN_DIR)/tests
	$(CC) $(CFLAGS) -DUNIT_TEST $< -o $@

$(BIN_DIR) $(BIN_DIR)/tests:
	mkdir -p $@

GREEN = \033[32m
RED = \033[31m
RESET = \033[0m

test: $(TEST_BINS)
	@pass=0; total=0; \
	for t in $(TEST_BINS); do \
		total=$$((total+1)); \
		if ./$$t; then echo "$(GREEN)PASS$(RESET) $$t"; pass=$$((pass+1)); \
		else echo "$(RED)FAIL$(RESET) $$t"; fi; \
	done; \
	echo "$$pass/$$total passed"; \
	[ $$pass -eq $$total ]

coverage: CFLAGS += -fprofile-instr-generate -fcoverage-mapping
coverage: clean $(TEST_BINS)
	@for t in $(TEST_BINS); do LLVM_PROFILE_FILE=$$t.profraw ./$$t; done
	@$(LLVM_PREFIX)llvm-profdata merge -sparse $(BIN_DIR)/tests/*.profraw -o $(BIN_DIR)/tests/all.profdata
	@$(LLVM_PREFIX)llvm-cov report $(firstword $(TEST_BINS)) $(addprefix -object ,$(wordlist 2,$(words $(TEST_BINS)),$(TEST_BINS))) -instr-profile=$(BIN_DIR)/tests/all.profdata $(SRC_DIR)/ $(UTILS_DIR)/
	@echo "Detail: $(LLVM_PREFIX)llvm-cov show <bin> -instr-profile=$(BIN_DIR)/tests/all.profdata"

clean:
	rm -rf $(BIN_DIR)

DOCS_DIR = docs

new:
ifndef NAME
	$(error Usage: make new NAME=<cipher-name>)
endif
	@for f in $(SRC_DIR)/$(NAME)-cipher.c $(DOCS_DIR)/$(NAME)-cipher.md $(TEST_DIR)/$(NAME)-cipher.c; do \
		if [ -e $$f ]; then echo "$$f already exists"; exit 1; fi; \
	done
	@title=$$(echo $(NAME) | awk -F- '{for(i=1;i<=NF;i++)$$i=toupper(substr($$i,1,1)) substr($$i,2); print}' OFS=' '); \
	printf '// Docs: docs/$(NAME).md\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n\nint cipher_main(int argc, char *argv[]) {\n    int decrypt = 0;\n    int argi = 1;\n    if (argi < argc && strcmp(argv[argi], "-d") == 0) {\n        decrypt = 1;\n        argi++;\n    }\n    (void)decrypt;\n\n    if (argc - argi != 1) {\n        fprintf(stderr, "Usage: %%s [-d] <message>\\n", argv[0]);\n        return 1;\n    }\n\n    char *message = argv[argi];\n    printf("%%s\\n", message);\n\n    return 0;\n}\n\n#ifndef UNIT_TEST\nint main(int argc, char *argv[]) {\n    return cipher_main(argc, argv);\n}\n#endif\n' > $(SRC_DIR)/$(NAME)-cipher.c; \
	printf '# %s\n\nTODO: describe the cipher.\n\nSource: [`src/$(NAME)-cipher.c`](../src/$(NAME)-cipher.c)\n\n## Usage\n\n```\nbin/$(NAME) [-d] <args...>\n```\n\n## Examples\n\n```\n$$ bin/$(NAME) ...\n```\n\n## Weaknesses\n\nTODO.\n' "$$title" > $(DOCS_DIR)/$(NAME)-cipher.md; \
	printf '// Unit + CLI tests for src/$(NAME)-cipher.c, run via `make test`.\n#include <string.h>\n#include "test.h"\n#include "../src/$(NAME)-cipher.c"\n\nstatic int run(int argc, char *argv[]) {\n    return cipher_main(argc, argv);\n}\n\nint main(void) {\n    TEST_INIT();\n\n    // TODO: add tests\n\n    return TEST_SUMMARY();\n}\n' > $(TEST_DIR)/$(NAME)-cipher.c; \
	echo "created $(SRC_DIR)/$(NAME)-cipher.c $(DOCS_DIR)/$(NAME)-cipher.md $(TEST_DIR)/$(NAME)-cipher.c"

.PHONY: all clean test coverage new
