// Tests for src/shift-cipher.c, run via `make test`. The source is
// included directly (compiled with -DUNIT_TEST, which omits its main() so
// this file's main() is the only one), giving direct access to
// shift_cipher without going through argv.
#include <string.h>
#include "test.h"
#include "../src/shift-cipher.c"

int main(void) {
    TEST_INIT();

    // key "3": argv[i] must be writable, so use arrays not literals.
    char key3[] = "3";
    {
        char msg[] = "Attack at dawn";
        shift_cipher(msg, key3, 0);
        CHECK(strcmp(msg, "Dwwdfn dw gdzq") == 0);
    }
    {
        char msg[] = "Dwwdfn dw gdzq";
        shift_cipher(msg, key3, 1);
        CHECK(strcmp(msg, "Attack at dawn") == 0);
    }
    {
        // wrap forward, shift > 26, case preserved
        char msg[] = "xaA";
        char key29[] = "29"; // == 3 mod 26
        shift_cipher(msg, key29, 0);
        CHECK(strcmp(msg, "adD") == 0);
    }
    {
        // negative shift
        char msg[] = "a";
        char keyNeg3[] = "-3";
        shift_cipher(msg, keyNeg3, 0);
        CHECK(strcmp(msg, "x") == 0);
    }
    for (int k = -30; k <= 30; k++) {
        char key[8];
        snprintf(key, sizeof(key), "%d", k);
        for (char c = 'a'; c <= 'z'; c++) {
            char msg[2] = {c, '\0'};
            shift_cipher(msg, key, 0);
            shift_cipher(msg, key, 1);
            CHECK(msg[0] == c);
        }
    }
    {
        // non-letter char above 'z' in ASCII: exercises the is_alpha
        // short-circuit false branch that a plain space doesn't reach
        char key1[] = "1";
        char msg[] = "a{Z";
        shift_cipher(msg, key1, 0);
        CHECK(strcmp(msg, "b{A") == 0);
    }

    return TEST_SUMMARY();
}
