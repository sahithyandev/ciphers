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

    // parse_key: sanitizes raw (lowercases, drops non-letters, keeps only
    // the first occurrence of each letter) then fills in the remaining
    // letters of the alphabet in order.
    {
        char out[27];
        parse_key("zebra", out);
        CHECK(strcmp(out, "zebracdfghijklmnopqstuvwxy") == 0);
    }
    {
        // empty raw: falls back to the alphabet in its natural order
        char out[27];
        parse_key("", out);
        CHECK(strcmp(out, alphabet) == 0);
    }
    {
        // already-complete key: nothing left to append
        char out[27];
        parse_key("bacdefghijklmnopqrstuvwxyz", out);
        CHECK(strcmp(out, "bacdefghijklmnopqrstuvwxyz") == 0);
    }
    {
        // mixed case, duplicates, and non-letters are sanitized before
        // the remaining letters get filled in
        char out[27];
        parse_key("zEbra!!zz", out);
        CHECK(strcmp(out, "zebracdfghijklmnopqstuvwxy") == 0);
    }
    {
        // '{' sits just above 'z' in ASCII: exercises the ch > 'z' branch,
        // which digits/punctuation below 'a' never reach
        char out[27];
        parse_key("z{ebra", out);
        CHECK(strcmp(out, "zebracdfghijklmnopqstuvwxy") == 0);
    }
    {
        // every letter duplicated: dedupes down to the plain alphabet
        char out[27];
        parse_key("aabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz", out);
        CHECK(strcmp(out, alphabet) == 0);
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
            char C = toupper((unsigned char)c);
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
        // raw key longer than the alphabet, with a duplicate letter and
        // non-letter characters: parse_key trims it down to a valid
        // 26-letter key instead of overflowing or being rejected
        char key[] = "abcdefghijklmnopqrstuvwxyz3z!"; // 29 chars, dupes 'z'
        char msg[] = "hi";
        char *argv[] = {prog, key, msg};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msg, "hi") == 0); // sanitized key == identity alphabet
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
