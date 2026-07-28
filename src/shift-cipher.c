// Docs: docs/shift-cipher.md
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char shift_cipher(char c, int shift) {
    char base;
    if (c >= 'a' && c <= 'z') {
        base = 'a';
    } else if (c >= 'A' && c <= 'Z') {
        base = 'A';
    } else {
        return c;
    }
    shift = ((shift % 26) + 26) % 26;
    return base + (c - base + shift) % 26;
}

char shift_decipher(char c, int shift) {
    return shift_cipher(c, -shift);
}

int cipher_main(int argc, char *argv[]) {
    int decrypt = 0;
    int argi = 1;
    if (argi < argc && strcmp(argv[argi], "-d") == 0) {
        decrypt = 1;
        argi++;
    } else if (argi < argc && argv[argi][0] == '-') {
        fprintf(stderr, "Usage: %s [-d] <key> <message>\n", argv[0]);
        return 1;
    }

    if (argc - argi != 2) {
        fprintf(stderr, "Usage: %s [-d] <key> <message>\n", argv[0]);
        return 1;
    }

    int key = atoi(argv[argi]);
    char *message = argv[argi + 1];
    char (*cipher_function)(char, int) = decrypt ? shift_decipher : shift_cipher;

    for (int i = 0; message[i] != '\0'; i++) {
        char ch = message[i];
        message[i] = cipher_function(ch, key);
    }
    printf("%s\n", message);

    return 0;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}
#endif
