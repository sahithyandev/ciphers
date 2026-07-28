// Unit + CLI tests for src/shift-cipher.c, run via `make test`.
// The source is included directly (compiled with -DUNIT_TEST, which omits
// its main() so this file's main() is the only one), giving access to both
// the pure functions and the CLI entry point (cipher_main) in one binary.
#include <string.h>
#include "test.h"
#include "../src/shift-cipher.c"

static int run(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}

int main(void) {
    TEST_INIT(); // silence cipher_main's own stdout/stderr output

    // shift_cipher / shift_decipher
    CHECK(shift_cipher('a', 3) == 'd');
    CHECK(shift_cipher('x', 3) == 'a');           // wrap forward
    CHECK(shift_cipher('a', -3) == 'x');           // negative shift
    CHECK(shift_cipher('a', 29) == 'd');            // shift > 26
    CHECK(shift_cipher('A', 1) == 'B');             // case preserved
    CHECK(shift_cipher('{', 3) == '{');             // c >= 'a' but > 'z': falls to 'A' base
    CHECK(shift_decipher('d', 3) == 'a');

    for (int k = -30; k <= 30; k++) {
        for (char c = 'a'; c <= 'z'; c++) {
            CHECK(shift_decipher(shift_cipher(c, k), k) == c);
        }
    }

    // cipher_main: argv[i] must be writable, so use arrays not literals.
    char prog[] = "shift-cipher";
    char key3[] = "3";
    char msgEnc[] = "Attack at dawn";
    char msgDec[] = "Dwwdfn dw gdzq";

    {
        char *argv[] = {prog, key3, msgEnc};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msgEnc, "Dwwdfn dw gdzq") == 0);
    }
    {
        char dflag[] = "-d";
        char *argv[] = {prog, dflag, key3, msgDec};
        CHECK(run(4, argv) == 0);
        CHECK(strcmp(msgDec, "Attack at dawn") == 0);
    }
    {
        // too few args
        char *argv[] = {prog, key3};
        CHECK(run(2, argv) == 1);
    }
    {
        // argc == 0: argi < argc is false, short-circuiting both flag checks
        char *argv[] = {prog};
        CHECK(run(0, argv) == 1);
    }
    {
        // too many args
        char msg[] = "hi";
        char extra[] = "there";
        char *argv[] = {prog, key3, msg, extra};
        CHECK(run(4, argv) == 1);
    }
    {
        // unknown flag
        char bad[] = "-z";
        char msg[] = "hi";
        char *argv[] = {prog, bad, key3, msg};
        CHECK(run(4, argv) == 1);
    }
    {
        // non-letter char above 'z' in ASCII: exercises the is_alpha
        // short-circuit false branch that a plain space doesn't reach
        char key1[] = "1";
        char msg[] = "a{Z";
        char *argv[] = {prog, key1, msg};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msg, "b{A") == 0);
    }
    return TEST_SUMMARY();
}
