// Docs: docs/atbash-cipher.md
#include <stdio.h>
#include "../utils/cli.c"

void atbash_cipher(char *message, int decrypt) {
    (void)decrypt; // atbash is its own inverse; -d is a documented no-op

    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i];
        if (c >= 'a' && c <= 'z') {
            message[i] = 'z' - (c - 'a');
        } else if (c >= 'A' && c <= 'Z') {
            message[i] = 'Z' - (c - 'A');
        }
    }
}

#ifndef NO_MAIN
int main(int argc, char *argv[]) {
    return run_keyless_cipher(argc, argv, atbash_cipher);
}
#endif
