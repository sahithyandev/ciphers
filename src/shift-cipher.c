// Docs: docs/shift-cipher.md
#include <stdio.h>
#include <stdlib.h>
#include "../utils/cli.c"

char shift_cipher(char c, int shift) {
    char base;
    if (c >= 'a' && c <= 'z') {
        base = 'a';
    } else if (c >= 'A' && c <= 'Z') {
        base = 'A';
    } else {
        return c;
    }

    // Normalize shift into [0, 26): one mod for the common case, plus a
    // conditional fix-up for negative shifts instead of the ((x % n) + n) % n
    // double-mod idiom.
    shift %= 26;
    if (shift < 0) {
        shift += 26;
    }

    int offset = c - base + shift;
    if (offset >= 26) {
        offset -= 26;
    }
    return (char)(base + offset);
}

char shift_decipher(char c, int shift) {
    return shift_cipher(c, -shift);
}

int cipher_main(int argc, char *argv[]) {
    int decrypt;
    char *key_str, *message;
    if (parse_args(argc, argv, &decrypt, &key_str, &message) != 0) {
        return 1;
    }

    // Fold the decrypt flag into the shift once, up front, so the loop body
    // below is a single shift_cipher call per character.
    int shift = atoi(key_str);
    if (decrypt) {
        shift = -shift;
    }

    for (int i = 0; message[i] != '\0'; i++) {
        message[i] = shift_cipher(message[i], shift);
    }
    printf("%s\n", message);

    return 0;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}
#endif
