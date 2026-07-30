// Docs: docs/shift-cipher.md
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utils/cli.c"

char shift_cipher(char c, int shift) {
    if (!isalpha((unsigned char)c)) {
        return c;
    }
    char base = isupper((unsigned char)c) ? 'A' : 'a';
    shift = ((shift % 26) + 26) % 26;
    return base + (c - base + shift) % 26;
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

    int key = atoi(key_str);

    for (int i = 0; message[i] != '\0'; i++) {
        char ch = message[i];
        message[i] = decrypt ? shift_decipher(ch, key) : shift_cipher(ch, key);
    }
    printf("%s\n", message);

    return 0;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}
#endif
