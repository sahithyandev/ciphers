// Tests for src/vigenere-cipher.c, run via `make test`. The source is
// included directly (compiled with -DUNIT_TEST, which omits its main() so
// this file's main() is the only one), giving direct access to
// vigenere_cipher without going through argv.
#include <string.h>
#include "test.h"
#include "../src/vigenere-cipher.c"

int main(void) {
    TEST_INIT();

    {
        char msg[] = "Attack at dawn";
        vigenere_cipher(msg, "lemon", 0);
        CHECK(strcmp(msg, "Lxfopv ef rnhr") == 0);
    }
    {
        char msg[] = "Lxfopv ef rnhr";
        vigenere_cipher(msg, "lemon", 1);
        CHECK(strcmp(msg, "Attack at dawn") == 0);
    }
    {
        // non-letters don't consume a key position
        char msg[] = "a, a!";
        vigenere_cipher(msg, "ab", 0);
        CHECK(strcmp(msg, "a, b!") == 0);
    }
    {
        // uppercase key letter, case preserved, wrap forward
        char msg[] = "aZ";
        vigenere_cipher(msg, "B", 0);
        CHECK(strcmp(msg, "bA") == 0);
    }
    {
        char msg[] = "z";
        vigenere_cipher(msg, "b", 0);
        CHECK(strcmp(msg, "a") == 0); // wrap forward
    }

    // exhaustive round trip over every key letter x message letter
    for (char k = 'a'; k <= 'z'; k++) {
        for (char c = 'a'; c <= 'z'; c++) {
            char key[] = {k, '\0'};
            char msg[] = {c, '\0'};
            vigenere_cipher(msg, key, 0);
            vigenere_cipher(msg, key, 1);
            CHECK(msg[0] == c);
        }
    }

    return TEST_SUMMARY();
}
