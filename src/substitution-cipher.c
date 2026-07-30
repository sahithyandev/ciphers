#include <stdio.h>
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
    int seen[26] = {0}; // seen[i]: has 'a'+i already been placed in out?

    for (size_t i = 0; raw[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)raw[i];
        // Lowercase via bit 0x20: turns 'A'-'Z' into 'a'-'z' and leaves
        // 'a'-'z' unchanged. Safe here because the range check below only
        // accepts the result if it lands in 'a'-'z'.
        ch |= 0x20;
        if (ch < 'a' || ch > 'z' || seen[ch - 'a']) {
            continue;
        }
        seen[ch - 'a'] = 1;
        out[len++] = (char)ch;
    }

    for (size_t i = 0; i < ALPHABET_LEN; i++) {
        if (!seen[i]) {
            out[len++] = alphabet[i];
        }
    }
    out[len] = '\0';
}

// Applies a 26-letter forward map (indexed 'a'-'z') to c, preserving case;
// non-letters pass through untouched. No search: direct array index.
static char map_char(char c, const char *map) {
    if (c >= 'a' && c <= 'z') {
        return map[c - 'a'];
    }
    if (c >= 'A' && c <= 'Z') {
        return (char)(map[c - 'A'] - 'a' + 'A');
    }
    return c;
}

char substitute_cipher(char c, char *key) {
    return map_char(c, key);
}

char substitute_decipher(char c, char *key) {
    // Invert key (key[i] maps 'a'+i -> key[i]) so a plain forward map_char
    // can be reused instead of a second search-based code path.
    char inverse[ALPHABET_LEN + 1];
    for (size_t i = 0; i < ALPHABET_LEN; i++) {
        inverse[key[i] - 'a'] = (char)('a' + i);
    }
    inverse[ALPHABET_LEN] = '\0';
    return map_char(c, inverse);
}

int cipher_main(int argc, char *argv[]) {
    int decrypt;
    char *key_str, *message;
    if (parse_args(argc, argv, &decrypt, &key_str, &message) != 0) {
        return 1;
    }

    char key[ALPHABET_LEN + 1];
    parse_key(key_str, key);

    // Build the map once up front (inverted for decryption) so the message
    // loop below is a single map_char call per character.
    char inverse[ALPHABET_LEN + 1];
    const char *map = key;
    if (decrypt) {
        for (size_t i = 0; i < ALPHABET_LEN; i++) {
            inverse[key[i] - 'a'] = (char)('a' + i);
        }
        inverse[ALPHABET_LEN] = '\0';
        map = inverse;
    }

    for (int i = 0; message[i] != '\0'; i++) {
        message[i] = map_char(message[i], map);
    }
    printf("%s\n", message);

    return 0;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return cipher_main(argc, argv);
}
#endif
