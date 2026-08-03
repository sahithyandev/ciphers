// Docs: docs/shift-cipher.md
#include <stdio.h>
#include <stdlib.h>
#include "../utils/cli.c"

void shift_cipher(char *message, const char *key, int decrypt) {
    // Fold the decrypt flag into the shift once, up front, so the loop body
    // below is a single add per character.
    int shift = atoi(key);
    if (decrypt) {
        shift = -shift;
    }

    // Normalize shift into [0, 26): one mod for the common case, plus a
    // conditional fix-up for negative shifts instead of the ((x % n) + n) % n
    // double-mod idiom.
    shift %= 26;
    if (shift < 0) {
        shift += 26;
    }

    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i], base;
        if (c >= 'a' && c <= 'z') {
            base = 'a';
        } else if (c >= 'A' && c <= 'Z') {
            base = 'A';
        } else {
            continue;
        }

        int offset = c - base + shift;
        if (offset >= 26) {
            offset -= 26;
        }
        message[i] = (char)(base + offset);
    }
}

#ifndef NO_MAIN
int main(int argc, char *argv[]) {
    return run_cipher(argc, argv, shift_cipher);
}
#endif
