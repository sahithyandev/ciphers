// Docs: docs/vigenere-cipher.md
#include <ctype.h>
#include <stdio.h>
#include "../utils/cli.c"

char vigenere_cipher(char c, char key_char, int decrypt) {
    char base;
    if (c >= 'a' && c <= 'z') {
        base = 'a';
    } else if (c >= 'A' && c <= 'Z') {
        base = 'A';
    } else {
        return c;
    }

    int shift = tolower((unsigned char)key_char) - 'a';
    if (decrypt) {
        shift = -shift;
    }

    int offset = (c - base + shift) % 26;
    if (offset < 0) {
        offset += 26;
    }
    return (char)(base + offset);
}

int cipher_main(int argc, char *argv[]) {
    int decrypt;
    char *message;
    char *key;
    if (parse_args(argc, argv, &decrypt, &key, &message) != 0) {
        return 1;
    }

    unsigned long key_len = strlen(key);

    // Key index only advances on alphabetic message characters, so
    // punctuation/spaces don't throw off the key alignment.
    unsigned long ki = 0;
    for (int i = 0; message[i] != '\0'; i++) {
        char ch = message[i];
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            message[i] = vigenere_cipher(ch, key[ki % key_len], decrypt);
            ki++;
        }
    }

    printf("%s\n", message);

    return 0;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}
#endif
