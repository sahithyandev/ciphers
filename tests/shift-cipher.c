// Unit + CLI tests for src/shift-cipher.c, run via `make test`.
// The source is included directly (compiled with -DUNIT_TEST, which omits
// its main() so this file's main() is the only one), giving access to both
// the pure functions and the CLI entry point (cipher_main) in one binary.
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "../src/shift-cipher.c"

// getopt's parser state persists across calls; reset before each cipher_main
// invocation (BSD/Darwin getopt uses optreset for this).
static int run(int argc, char *argv[]) {
    optreset = 1;
    optind = 1;
    return cipher_main(argc, argv);
}

int main(void) {
    // shift_cipher / shift_decipher
    assert(shift_cipher('a', 3) == 'd');
    assert(shift_cipher('x', 3) == 'a');           // wrap forward
    assert(shift_cipher('a', -3) == 'x');           // negative shift
    assert(shift_cipher('a', 29) == 'd');            // shift > 26
    assert(shift_cipher('A', 1) == 'B');             // case preserved
    assert(shift_decipher('d', 3) == 'a');

    for (int k = -30; k <= 30; k++) {
        for (char c = 'a'; c <= 'z'; c++) {
            assert(shift_decipher(shift_cipher(c, k), k) == c);
        }
    }

    // cipher_main: argv[i] must be writable, so use arrays not literals.
    char prog[] = "shift-cipher";
    char key3[] = "3";
    char msgEnc[] = "Attack at dawn";
    char msgDec[] = "Dwwdfn dw gdzq";

    {
        char *argv[] = {prog, key3, msgEnc};
        assert(run(3, argv) == 0);
        assert(strcmp(msgEnc, "Dwwdfn dw gdzq") == 0);
    }
    {
        char dflag[] = "-d";
        char *argv[] = {prog, dflag, key3, msgDec};
        assert(run(4, argv) == 0);
        assert(strcmp(msgDec, "Attack at dawn") == 0);
    }
    {
        // too few args
        char *argv[] = {prog, key3};
        assert(run(2, argv) == 1);
    }
    {
        // too many args
        char msg[] = "hi";
        char extra[] = "there";
        char *argv[] = {prog, key3, msg, extra};
        assert(run(4, argv) == 1);
    }
    {
        // unknown flag
        char bad[] = "-z";
        char msg[] = "hi";
        char *argv[] = {prog, bad, key3, msg};
        assert(run(4, argv) == 1);
    }
    return 0;
}
