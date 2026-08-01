// Tests for src/atbash-cipher.c, run via `make test`. The source is
// included directly (compiled with -DUNIT_TEST, which omits its main() so
// this file's main() is the only one), giving direct access to
// atbash_cipher without going through argv.
#include <string.h>
#include "test.h"
#include "../src/atbash-cipher.c"

int main(void) {
    TEST_INIT();

    {
        char msg[] = "Attack at dawn";
        atbash_cipher(msg, 0);
        CHECK(strcmp(msg, "Zggzxp zg wzdm") == 0);
    }
    {
        // -d is documented as a no-op: atbash is its own inverse, so
        // encrypting the ciphertext again recovers the plaintext.
        char msg[] = "Zggzxp zg wzdm";
        atbash_cipher(msg, 1);
        CHECK(strcmp(msg, "Attack at dawn") == 0);
    }
    {
        // same input, with and without -d, must produce the same output
        char msgA[] = "Attack at dawn";
        char msgB[] = "Attack at dawn";
        atbash_cipher(msgA, 0);
        atbash_cipher(msgB, 1);
        CHECK(strcmp(msgA, msgB) == 0);
    }
    {
        // full alphabet, both cases: mirrors around the middle
        char msg[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        atbash_cipher(msg, 0);
        CHECK(strcmp(msg, "zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA") == 0);
    }
    {
        // non-letter char above 'z' in ASCII passes through untouched,
        // same boundary case exercised in the sibling ciphers' tests
        char msg[] = "a{Z";
        atbash_cipher(msg, 0);
        CHECK(strcmp(msg, "z{A") == 0);
    }

    return TEST_SUMMARY();
}
