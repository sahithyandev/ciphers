// Docs: docs/atbash-cipher.md
#include <stdio.h>
#include "../utils/cli.c"

int cipher_main(int argc, char *argv[]) {
    int decrypt;
    char *message;
    if (parse_args_no_key(argc, argv, &decrypt, &message) != 0) {
        return 1;
    }
    (void)decrypt; // atbash is its own inverse; -d is a documented no-op

    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i];
        if (c >= 'a' && c <= 'z') {
            message[i] = 'z' - (c - 'a');
        } else if (c >= 'A' && c <= 'Z') {
            message[i] = 'Z' - (c - 'A');
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
