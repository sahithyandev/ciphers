// Unit + CLI tests for src/atbash-cipher.c, run via `make test`.
// The source is included directly (compiled with -DUNIT_TEST, which omits
// its main() so this file's main() is the only one), giving access to the
// CLI entry point (cipher_main) in one binary. There's no separate pure
// mapping function to unit test directly (unlike shift/substitution) since
// atbash has no key to thread through one.
#include <string.h>
#include "test.h"
#include "../src/atbash-cipher.c"

static int run(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}

int main(void) {
    TEST_INIT(); // silence cipher_main's own stdout/stderr output

    // cipher_main: argv[i] must be writable, so use arrays not literals.
    char prog[] = "atbash-cipher";

    {
        char msg[] = "Attack at dawn";
        char *argv[] = {prog, msg};
        CHECK(run(2, argv) == 0);
        CHECK(strcmp(msg, "Zggzxp zg wzdm") == 0);
    }
    {
        // -d is documented as a no-op: atbash is its own inverse, so
        // encrypting the ciphertext again recovers the plaintext.
        char dflag[] = "-d";
        char msg[] = "Zggzxp zg wzdm";
        char *argv[] = {prog, dflag, msg};
        CHECK(run(3, argv) == 0);
        CHECK(strcmp(msg, "Attack at dawn") == 0);
    }
    {
        // same input, with and without -d, must produce the same output
        char msgA[] = "Attack at dawn";
        char dflag[] = "-d";
        char msgB[] = "Attack at dawn";
        char *argvA[] = {prog, msgA};
        char *argvB[] = {prog, dflag, msgB};
        CHECK(run(2, argvA) == 0);
        CHECK(run(3, argvB) == 0);
        CHECK(strcmp(msgA, msgB) == 0);
    }
    {
        // full alphabet, both cases: mirrors around the middle
        char msg[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        char *argv[] = {prog, msg};
        CHECK(run(2, argv) == 0);
        CHECK(strcmp(msg, "zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA") == 0);
    }
    {
        // non-letter char above 'z' in ASCII passes through untouched,
        // same boundary case exercised in the sibling ciphers' tests
        char msg[] = "a{Z";
        char *argv[] = {prog, msg};
        CHECK(run(2, argv) == 0);
        CHECK(strcmp(msg, "z{A") == 0);
    }
    {
        // too few args
        char *argv[] = {prog};
        CHECK(run(1, argv) == 1);
    }
    {
        // -d with no message
        char dflag[] = "-d";
        char *argv[] = {prog, dflag};
        CHECK(run(2, argv) == 1);
    }
    {
        // too many args
        char msg[] = "hi";
        char extra[] = "there";
        char *argv[] = {prog, msg, extra};
        CHECK(run(3, argv) == 1);
    }
    {
        // argc == 0: argi < argc is false, short-circuiting the flag check
        char *argv[] = {prog};
        CHECK(run(0, argv) == 1);
    }
    {
        // unknown flag, rejected by parse_args_no_key
        char bad[] = "-z";
        char *argv[] = {prog, bad};
        CHECK(run(2, argv) == 1);
    }
    return TEST_SUMMARY();
}
