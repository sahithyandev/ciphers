#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/strings.c"

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
        char ch = to_lower(raw[i]);
        if (ch < 'a' || ch > 'z' || strchr(out, ch) != NULL) {
            continue;
        }
        out[len++] = ch;
        out[len] = '\0';
    }

    for (size_t i = 0; i < strlen(alphabet); i++) {
        char ch = alphabet[i];
        if (strchr(out, ch) == NULL) {
            size_t out_len = strlen(out);
            out[out_len] = ch;
            out[out_len + 1] = '\0';
        }
    }
}

char substitute_cipher(char c, char* key) {
    if (c >= 'a' && c <= 'z') {
        return key[strchr(alphabet, c) - alphabet];
    }
    if (c >= 'A' && c <= 'Z') {
        return to_upper(
            key[strchr(alphabet, to_lower(c)) - alphabet]
        );
    }
    return c;
}

char substitute_decipher(char c, char* key) {
    if (c >= 'a' && c <= 'z') {
        return alphabet[strchr(key, c) - key];
    }
    if (c >= 'A' && c <= 'Z') {
        return to_upper(
            alphabet[strchr(key, to_lower(c)) - key]
        );
    }
    return c;
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
    
    char key[ALPHABET_LEN + 1];
    parse_key(argv[argi], key);
    char *message = argv[argi + 1];
    char (*cipher_function)(char, char*) = decrypt ? substitute_decipher : substitute_cipher;

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
