// Tests for src/substitution-cipher.c, run via `make test`. The source is
// included directly (compiled with -DUNIT_TEST, which omits its main() so
// this file's main() is the only one), giving direct access to parse_key
// and substitution_cipher without going through argv.
#include <ctype.h>
#include <string.h>
#include "test.h"
#include "../src/substitution-cipher.c"

int main(void) {
    TEST_INIT();

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
        CHECK(strcmp(out, ALPHABETS) == 0);
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
        CHECK(strcmp(out, ALPHABETS) == 0);
    }

    // substitution_cipher: argv[i] must be writable, so use arrays not
    // literals. Key "b" expands (via parse_key) to
    // "bacdefghijklmnopqrstuvwxyz", which swaps only a<->b and leaves every
    // other letter fixed - easy to verify by hand.
    char keyB[] = "b";
    {
        char msg[] = "Attack at dawn";
        substitution_cipher(msg, keyB, 0);
        CHECK(strcmp(msg, "Bttbck bt dbwn") == 0);
    }
    {
        char msg[] = "Bttbck bt dbwn";
        substitution_cipher(msg, keyB, 1);
        CHECK(strcmp(msg, "Attack at dawn") == 0);
    }
    {
        // explicit mappings for zebra's key, both directions, case preserved
        char key[] = "zebra";
        char a[] = "a", e[] = "e", z[] = "z", A[] = "A", brace[] = "{";
        substitution_cipher(a, key, 0); CHECK(strcmp(a, "z") == 0);
        substitution_cipher(e, key, 0); CHECK(strcmp(e, "a") == 0);
        substitution_cipher(z, key, 0); CHECK(strcmp(z, "y") == 0);
        substitution_cipher(A, key, 0); CHECK(strcmp(A, "Z") == 0); // case preserved
        substitution_cipher(brace, key, 0); CHECK(strcmp(brace, "{") == 0); // non-letter passthrough
    }
    {
        // round trip across the full alphabet, both cases, using zebra's key
        char key[] = "zebra";
        for (char c = 'a'; c <= 'z'; c++) {
            char msg[2] = {c, '\0'};
            substitution_cipher(msg, key, 0);
            substitution_cipher(msg, key, 1);
            CHECK(msg[0] == c);
        }
        for (char c = 'A'; c <= 'Z'; c++) {
            char msg[2] = {c, '\0'};
            substitution_cipher(msg, key, 0);
            substitution_cipher(msg, key, 1);
            CHECK(msg[0] == c);
        }
    }
    {
        // raw key longer than the alphabet, with a duplicate letter and
        // non-letter characters: parse_key trims it down to a valid
        // 26-letter key instead of overflowing or being rejected
        char key[] = "abcdefghijklmnopqrstuvwxyz3z!"; // 29 chars, dupes 'z'
        char msg[] = "hi";
        substitution_cipher(msg, key, 0);
        CHECK(strcmp(msg, "hi") == 0); // sanitized key == identity alphabet
    }
    {
        // non-letter char above 'z' in ASCII passes through untouched
        char msg[] = "a{Z";
        substitution_cipher(msg, keyB, 0);
        CHECK(strcmp(msg, "b{Z") == 0);
    }

    return TEST_SUMMARY();
}
