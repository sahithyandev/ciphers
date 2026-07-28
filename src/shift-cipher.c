// Docs: docs/shift-cipher.md
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char shift_cipher(char c, int shift) {
    char base = (c >= 'a' && c <= 'z') ? 'a' : 'A';
    shift = ((shift % 26) + 26) % 26;
    return base + (c - base + shift) % 26;
}

char shift_decipher(char c, int shift) {
    return shift_cipher(c, -shift);
}

int main(int argc, char *argv[]) {
    int decrypt = 0;
    int opt;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        if (opt == 'd') decrypt = 1;
        else {
            fprintf(stderr, "Usage: %s [-d] <key> <message>\n", argv[0]);
            return 1;
        }
    }

    if (argc - optind != 2) {
        fprintf(stderr, "Usage: %s [-d] <key> <message>\n", argv[0]);
        return 1;
    }

    int key = atoi(argv[optind]);
    char *message = argv[optind + 1];
    char (*cipher_function)(char, int) = decrypt ? shift_decipher : shift_cipher;

    for (int i = 0; message[i] != '\0'; i++) {
        char ch = message[i];
        int is_alpha = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
        if (is_alpha) {
            message[i] = cipher_function(ch, key);
        }
    }
    printf("%s\n", message);
    
    return 0;
}
