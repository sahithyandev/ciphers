// Docs: docs/substitution-cipher.md
#include <stdio.h>
#include "../utils/cli.c"

#define ALPHABETS "abcdefghijklmnopqrstuvwxyz"
#define ALPHABET_LEN (sizeof(ALPHABETS) - 1)

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
            out[len++] = ALPHABETS[i];
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

void substitution_cipher(char *message, const char *key_str, int decrypt) {
    char key[ALPHABET_LEN + 1];
    parse_key(key_str, key);

    // Build the map once up front (inverted for decryption) so the message
    // loop below is a single map_char call per character.
    const char *map = key;
    char inverse[ALPHABET_LEN + 1];
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
}

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
    return run_cipher(argc, argv, substitution_cipher);
}
#endif
