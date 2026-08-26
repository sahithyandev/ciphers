// Docs: docs/autokey-cipher.md
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/cli.c"

// Like Vigenere, but the key stream is the primer followed by the message's
// own letters (plaintext when encrypting, recovered plaintext when
// decrypting) instead of a repeating keyword.
void autokey_cipher(char *message, const char *key, int decrypt) {
    unsigned long key_len = strlen(key);
    unsigned long msg_len = strlen(message);

    // Letters seen so far, lowercased, extending the key stream past the
    // primer. At most one per message byte.
    char *stream = malloc(msg_len);
    unsigned long stream_len = 0;

    unsigned long ki = 0;
    for (unsigned long i = 0; message[i] != '\0'; i++) {
        char c = message[i], base;
        if (c >= 'a' && c <= 'z') {
            base = 'a';
        } else if (c >= 'A' && c <= 'Z') {
            base = 'A';
        } else {
            continue;
        }

        int shift = (ki < key_len ? tolower((unsigned char)key[ki])
                                   : stream[ki - key_len]) - 'a';
        ki++;

        if (decrypt) {
            int offset = (c - base - shift) % 26;
            if (offset < 0) {
                offset += 26;
            }
            message[i] = (char)(base + offset);
            stream[stream_len++] = (char)tolower((unsigned char)message[i]);
        } else {
            stream[stream_len++] = (char)tolower((unsigned char)c);
            int offset = (c - base + shift) % 26;
            message[i] = (char)(base + offset);
        }
    }

    free(stream);
}

#ifndef NO_MAIN
int main(int argc, char *argv[]) {
    return run_cipher(argc, argv, autokey_cipher);
}
#endif
