// Docs: docs/feistel-cipher.md
#include "../utils/cli.c"

// The whole cipher: transform message in place. Delete the (void)decrypt
// and take a "const char *key" param too if this cipher needs one.
void feistel_cipher(char *message, int decrypt) {
    (void)message;
    (void)decrypt;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return run_keyless_cipher(argc, argv, feistel_cipher);
}
#endif
