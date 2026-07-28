// Minimal check macro shared by test binaries: unlike assert(), a failed
// CHECK is reported and execution continues so the rest of the cases still run.
#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <unistd.h>

static FILE *test_out;
static int checks_run = 0, checks_passed = 0;

// Call once at the top of main(): saves the real stderr for our own
// reporting, then silences stdout/stderr so the program under test stays quiet.
#define TEST_INIT() do { \
    test_out = fdopen(dup(STDERR_FILENO), "w"); \
    freopen("/dev/null", "w", stdout); \
    freopen("/dev/null", "w", stderr); \
} while (0)

#define CHECK(cond) do { \
    checks_run++; \
    if (cond) checks_passed++; \
    else fprintf(test_out, "  \033[31mFAIL\033[0m %s:%d: %s\n", __FILE__, __LINE__, #cond); \
} while (0)

#define TEST_SUMMARY() (fprintf(test_out, "%d/%d checks passed\n", checks_passed, checks_run), \
    checks_passed == checks_run ? 0 : 1)

#endif
