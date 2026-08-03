// Docs: docs/statistical-shift-attack.md
#include <stdio.h>
#include "../src/shift-cipher.c" // reuses shift_cipher; its main() is off via -DNO_MAIN

// Real letter frequencies of English characters (as percentages), A-Z, from:
// https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html
static const double ENGLISH_FREQ[26] = {
    8.12, 1.49, 2.71, 4.32, 12.02, 2.30, 2.03, 5.92, 7.31, 0.10, 0.69, 3.98,
    2.61, 6.95, 7.68, 1.82, 0.11, 6.02, 6.28, 9.10, 2.88, 1.11, 2.09, 0.17,
    2.11, 0.07,
};

// Breaks a shift-ciphered message by frequency analysis: the letter counts of
// a shift-by-k ciphertext are just the plaintext's counts rotated by k, so
// scoring every key is a rotation-and-compare over one histogram, no need to
// decipher 26 candidate strings. Deciphers message in place with the
// best-scoring key and returns that key.
int statistical_shift_attack(char *message) {
    int counts[26] = {0};
    int total = 0;

    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i];
        if (c >= 'a' && c <= 'z') {
            counts[c - 'a']++;
            total++;
        } else if (c >= 'A' && c <= 'Z') {
            counts[c - 'A']++;
            total++;
        }
    }

    if (total == 0) {
        return 0;
    }

    double freq[26];
    for (int i = 0; i < 26; i++) {
        freq[i] = counts[i] * 100.0 / total;
    }

    int best_key = 0;
    double best_error = -1;
    for (int key = 0; key < 26; key++) {
        double error = 0;
        for (int i = 0; i < 26; i++) {
            error += freq[(i + key) % 26] > ENGLISH_FREQ[i]
                ? freq[(i + key) % 26] - ENGLISH_FREQ[i]
                : ENGLISH_FREQ[i] - freq[(i + key) % 26];
        }
        if (best_error < 0 || error < best_error) {
            best_error = error;
            best_key = key;
        }
    }

    char key_str[8];
    snprintf(key_str, sizeof(key_str), "%d", best_key);
    shift_cipher(message, key_str, 1);
    return best_key;
}

#ifndef NO_ATTACK_MAIN
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <cipher text>\n", argv[0]);
        return 1;
    }

    char *message = argv[1];
    int key = statistical_shift_attack(message);
    printf("Key: %d\n", key);
    printf("%s\n", message);
    return 0;
}
#endif
