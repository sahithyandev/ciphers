// Unit + CLI tests for src/vigenere-cipher.c, run via `make test`.
#include <string.h>
#include "test.h"
#include "../src/vigenere-cipher.c"

static int run(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}

int main(void) {
    TEST_INIT();

    // vigenere_cipher
    CHECK(vigenere_cipher('a', 'l', 0) == 'l');
    CHECK(vigenere_cipher('A', 'l', 0) == 'L');            // case preserved
    CHECK(vigenere_cipher('z', 'b', 0) == 'a');             // wrap forward
    CHECK(vigenere_cipher('L', 'l', 1) == 'A');             // decrypt
    CHECK(vigenere_cipher('{', 'l', 0) == '{');             // non-letter passthrough
    CHECK(vigenere_cipher('a', 'Z', 0) == 'z');             // uppercase key letter

    for (char k = 'a'; k <= 'z'; k++) {
        for (char c = 'a'; c <= 'z'; c++) {
            CHECK(vigenere_cipher(vigenere_cipher(c, k, 0), k, 1) == c);
        }
    }

    // cipher_main: argv[i] must be writable, so use arrays not literals.
    char prog[] = "vigenere-cipher";
    char key[] = "lemon";
    char msgEnc[] = "Attack at dawn";
    char msgDec[] = "Lxfopv ef rnhr";

    {
        char *argv[] = {prog, key, msgEnc};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msgEnc, "Lxfopv ef rnhr") == 0);
    }
    {
        char dflag[] = "-d";
        char *argv[] = {prog, dflag, key, msgDec};
        CHECK(run(4, argv) == 0);
        CHECK(strcmp(msgDec, "Attack at dawn") == 0);
    }
    {
        // non-letters don't consume a key position
        char k[] = "ab";
        char msg[] = "a, a!";
        char *argv[] = {prog, k, msg};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msg, "a, b!") == 0);
    }
    {
        // too few args
        char *argv[] = {prog, key};
        CHECK(run(2, argv) == 1);
    }
    {
        // unknown flag
        char bad[] = "-z";
        char msg[] = "hi";
        char *argv[] = {prog, bad, key, msg};
        CHECK(run(4, argv) == 1);
    }

    return TEST_SUMMARY();
}
