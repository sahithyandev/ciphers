// Docs: docs/vigenere-cipher.md
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "../utils/cli.c"

void vigenere_cipher(char *message, const char *key, int decrypt) {
    unsigned long key_len = strlen(key);

    // Key index only advances on alphabetic message characters, so
    // punctuation/spaces don't throw off the key alignment.
    unsigned long ki = 0;
    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i], base;
        if (c >= 'a' && c <= 'z') {
            base = 'a';
        } else if (c >= 'A' && c <= 'Z') {
            base = 'A';
        } else {
            continue;
        }

        int shift = tolower((unsigned char)key[ki % key_len]) - 'a';
        if (decrypt) {
            shift = -shift;
        }
        ki++;

        int offset = (c - base + shift) % 26;
        if (offset < 0) {
            offset += 26;
        }
        message[i] = (char)(base + offset);
    }
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return run_cipher(argc, argv, vigenere_cipher);
}
#endif
