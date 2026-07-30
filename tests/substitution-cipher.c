// Unit + CLI tests for src/substitution-cipher.c, run via `make test`.
// The source is included directly (compiled with -DUNIT_TEST, which omits
// its main() so this file's main() is the only one), giving access to both
// the pure functions and the CLI entry point (cipher_main) in one binary.
#include <string.h>
#include "test.h"
#include "../src/substitution-cipher.c"

static int run(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}

int main(void) {
    TEST_INIT(); // silence cipher_main's own stdout/stderr output

    // parse_key: fills in missing letters in alphabetical order, keeping
    // the caller-supplied prefix. Buffers must be sized for the full
    // 26-letter alphabet plus the terminator.
    {
        char key[27] = "zebra";
        parse_key(key);
        CHECK(strcmp(key, "zebracdfghijklmnopqstuvwxy") == 0);
    }
    {
        // empty key: falls back to the alphabet in its natural order
        char key[27] = "";
        parse_key(key);
        CHECK(strcmp(key, alphabet) == 0);
    }
    {
        // already-complete key: nothing left to append
        char key[27] = "bacdefghijklmnopqrstuvwxyz";
        parse_key(key);
        CHECK(strcmp(key, "bacdefghijklmnopqrstuvwxyz") == 0);
    }

    // substitute_cipher / substitute_decipher, using the full key that
    // parse_key("zebra") produces.
    {
        char key[] = "zebracdfghijklmnopqstuvwxy";
        CHECK(substitute_cipher('a', key) == 'z');
        CHECK(substitute_cipher('e', key) == 'a');
        CHECK(substitute_cipher('z', key) == 'y');
        CHECK(substitute_cipher('A', key) == 'Z');      // case preserved
        CHECK(substitute_cipher('{', key) == '{');      // non-letter passthrough

        CHECK(substitute_decipher('z', key) == 'a');
        CHECK(substitute_decipher('a', key) == 'e');
        CHECK(substitute_decipher('Z', key) == 'A');    // case preserved
        CHECK(substitute_decipher('{', key) == '{');    // non-letter passthrough

        for (char c = 'a'; c <= 'z'; c++) {
            CHECK(substitute_decipher(substitute_cipher(c, key), key) == c);
            char C = to_upper(c);
            CHECK(substitute_decipher(substitute_cipher(C, key), key) == C);
        }
    }

    // cipher_main: argv[i] must be writable, so use arrays not literals.
    // Key "b" expands (via parse_key) to "bacdefghijklmnopqrstuvwxyz",
    // which swaps only a<->b and leaves every other letter fixed - easy
    // to verify by hand.
    char prog[] = "substitution-cipher";
    char keyB[] = "b";
    char msgEnc[] = "Attack at dawn";
    char msgDec[] = "Bttbck bt dbwn";

    {
        char *argv[] = {prog, keyB, msgEnc};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msgEnc, "Bttbck bt dbwn") == 0);
    }
    {
        char dflag[] = "-d";
        char *argv[] = {prog, dflag, keyB, msgDec};
        CHECK(run(4, argv) == 0);
        CHECK(strcmp(msgDec, "Attack at dawn") == 0);
    }
    {
        // too few args
        char key[] = "b";
        char *argv[] = {prog, key};
        CHECK(run(2, argv) == 1);
    }
    {
        // argc == 0: argi < argc is false, short-circuiting both flag checks
        char *argv[] = {prog};
        CHECK(run(0, argv) == 1);
    }
    {
        // too many args
        char key[] = "b";
        char msg[] = "hi";
        char extra[] = "there";
        char *argv[] = {prog, key, msg, extra};
        CHECK(run(4, argv) == 1);
    }
    {
        // unknown flag
        char bad[] = "-z";
        char key[] = "b";
        char msg[] = "hi";
        char *argv[] = {prog, bad, key, msg};
        CHECK(run(4, argv) == 1);
    }
    {
        // key longer than the alphabet is rejected
        char key[] = "abcdefghijklmnopqrstuvwxyzz"; // 27 chars
        char msg[] = "hi";
        char *argv[] = {prog, key, msg};
        CHECK(run(3, argv) == 1);
    }
    {
        // non-letter char above 'z' in ASCII passes through untouched
        char key[] = "b";
        char msg[] = "a{Z";
        char *argv[] = {prog, key, msg};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msg, "b{Z") == 0);
    }
    return TEST_SUMMARY();
}
