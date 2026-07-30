#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/cli.c"

#define ALPHABETS "abcdefghijklmnopqrstuvwxyz"
#define ALPHABET_LEN (sizeof(ALPHABETS) - 1)

const char *alphabet = ALPHABETS;

/*
 * Builds a full 26-letter substitution key in out (a 27-byte buffer) from
 * raw: keeps raw's letters (lowercased, deduped, non-letters dropped) as a
 * prefix, then fills in the remaining letters of the alphabet in order.
 * zEbra!!zz -> zebracdfghijklmnopqstuvwxy
 */
void parse_key(const char *raw, char *out) {
    size_t len = 0;
    out[0] = '\0';
    for (size_t i = 0; raw[i] != '\0'; i++) {
        char ch = tolower((unsigned char)raw[i]);
        if (!islower((unsigned char)ch) || strchr(out, ch) != NULL) {
            continue;
        }
        out[len++] = ch;
        out[len] = '\0';
    }

    for (size_t i = 0; i < ALPHABET_LEN; i++) {
        char ch = alphabet[i];
        if (strchr(out, ch) == NULL) {
            out[len++] = ch;
            out[len] = '\0';
        }
    }
}

// Maps c through the from->to letter mapping, preserving case;
// non-letters pass through untouched.
static char substitute(char c, const char *from, const char *to) {
    if (!isalpha((unsigned char)c)) {
        return c;
    }
    const char *p = strchr(from, tolower((unsigned char)c));
    char out = to[p - from];
    return isupper((unsigned char)c) ? toupper((unsigned char)out) : out;
}

char substitute_cipher(char c, char *key) {
    return substitute(c, alphabet, key);
}

char substitute_decipher(char c, char *key) {
    return substitute(c, key, alphabet);
}

int cipher_main(int argc, char *argv[]) {
    int decrypt;
    char *key_str, *message;
    if (parse_args(argc, argv, &decrypt, &key_str, &message) != 0) {
        return 1;
    }

    char key[ALPHABET_LEN + 1];
    parse_key(key_str, key);

    for (int i = 0; message[i] != '\0'; i++) {
        char ch = message[i];
        message[i] = decrypt ? substitute_decipher(ch, key) : substitute_cipher(ch, key);
    }
    printf("%s\n", message);

    return 0;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}
#endif
