#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *alphabet = "abcdefghijklmnopqrstuvwxyz";

char to_lower(char c) {
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

char to_upper(char c) {
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

/*
 * Parses the key string and fills in any missing characters in place.
 * zebra -> zebracdfghijklmnopqstuvwxy
 */
void parse_key(char* key) {
    size_t alphabet_size = strlen(alphabet);
    size_t i = 0;

    while (i < alphabet_size) {
        char ch = alphabet[i];
        if (strchr(key, ch) == NULL) {
            char suffix[2] = {ch, '\0'};
            strcat(key, suffix);
        }
        i++;
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
    
    if (strlen(argv[argi]) > 26) {
        fprintf(stderr, "Key must be at most 26 characters\n");
        return 1;
    }
    char key[27];
    strcpy(key, argv[argi]);
    parse_key(key);
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
