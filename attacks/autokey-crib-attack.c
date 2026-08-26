// Docs: docs/autokey-crib-attack.md
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/autokey-cipher.c" // reuses autokey_cipher; its main() is off via -DNO_MAIN

// A small dictionary of common English words. This is the automated stand-in
// for a human eyeballing a decrypted fragment and recognizing a real word in
// it, i.e. the crib-dragging judgment call described in
// docs/autokey-cipher.md's Weaknesses section.
static const char *COMMON_WORDS[] = {
    "the", "of", "and", "a", "to", "in", "is", "you", "that", "it", "he",
    "was", "for", "on", "are", "as", "with", "his", "they", "at", "be",
    "this", "have", "from", "or", "one", "had", "by", "but", "not", "what",
    "all", "were", "we", "when", "your", "can", "there", "an", "each",
    "which", "she", "do", "how", "their", "if", "will", "up", "other",
    "about", "out", "many", "then", "them", "these", "so", "some", "her",
    "would", "make", "like", "him", "into", "time", "has", "look", "two",
    "more", "go", "see", "no", "way", "could", "people", "my", "than",
    "first", "water", "been", "call", "who", "its", "now", "find", "long",
    "down", "day", "did", "get", "come", "made", "may", "part", "over",
    "new", "sound", "take", "only", "little", "work", "know", "place",
    "year", "live", "me", "back", "give", "most", "very", "after", "thing",
    "our", "just", "name", "good", "man", "think", "say", "great", "where",
    "help", "through", "much", "before", "line", "right", "too", "mean",
    "old", "any", "same", "tell", "boy", "follow", "came", "want", "show",
    "also", "around", "form", "three", "small", "set", "put", "end",
    "does", "another", "well", "large", "must", "big", "even", "such",
    "because", "turn", "here", "why", "ask", "went", "men", "read", "need",
    "land", "different", "home", "us", "move", "try", "kind", "hand",
    "again", "change", "off", "play", "air", "away", "animal", "house",
    "point", "page", "letter", "mother", "answer", "found", "still",
    "learn", "should", "world", "near",
};
#define COMMON_WORDS_LEN (sizeof(COMMON_WORDS) / sizeof(COMMON_WORDS[0]))

// Brute-forcing primers calls word_score (and so is_common_word) on the
// order of a million times, so lookups go through a small hash set built
// once from COMMON_WORDS instead of a linear scan.
#define WORD_TABLE_SIZE 1024
static const char *word_table[WORD_TABLE_SIZE];
static int word_table_ready = 0;

static unsigned long hash_str(const char *s) {
    unsigned long h = 5381;
    for (; *s != '\0'; s++) {
        h = h * 33 + (unsigned char)*s;
    }
    return h;
}

static void build_word_table(void) {
    for (size_t i = 0; i < COMMON_WORDS_LEN; i++) {
        unsigned long slot = hash_str(COMMON_WORDS[i]) % WORD_TABLE_SIZE;
        while (word_table[slot] != NULL) {
            slot = (slot + 1) % WORD_TABLE_SIZE;
        }
        word_table[slot] = COMMON_WORDS[i];
    }
    word_table_ready = 1;
}

static int is_common_word(const char *word) {
    if (!word_table_ready) {
        build_word_table();
    }

    unsigned long slot = hash_str(word) % WORD_TABLE_SIZE;
    while (word_table[slot] != NULL) {
        if (strcmp(word_table[slot], word) == 0) {
            return 1;
        }
        slot = (slot + 1) % WORD_TABLE_SIZE;
    }
    return 0;
}

// Counts how many whitespace/punctuation-delimited words in `text` are in
// COMMON_WORDS. Higher is more plausibly English. Even one wrong letter
// breaks a word match, which is what makes this far more discriminating
// than a letter-frequency score: a decryption that's 90% right but wrong in
// a way that breaks most words scores close to zero, not close to correct.
static int word_score(const char *text) {
    int score = 0;
    char word[32];
    int wi = 0;

    for (int i = 0;; i++) {
        char c = text[i];
        if (c >= 'a' && c <= 'z') {
            if (wi < (int)sizeof(word) - 1) {
                word[wi++] = c;
            }
        } else if (c >= 'A' && c <= 'Z') {
            if (wi < (int)sizeof(word) - 1) {
                word[wi++] = (char)tolower((unsigned char)c);
            }
        } else {
            if (wi > 0) {
                word[wi] = '\0';
                score += is_common_word(word);
                wi = 0;
            }
            if (c == '\0') {
                break;
            }
        }
    }

    return score;
}

// Longest primer this attack will try to recover. Brute-forcing a primer is
// 26^length candidates, so this is the largest length that still finishes in
// a reasonable time (26^4 is under half a million).
#define MAX_PRIMER_LEN 4

// Tries every primer of `len` letters (recursing one primer position at a
// time), decrypting `ciphertext` into `scratch` with each and keeping
// whichever contains the most recognizable words.
static void brute_force_primer(const char *ciphertext, char *primer, int len, int pos,
                                char *scratch, int *best_score, char *best_primer) {
    if (pos == len) {
        strcpy(scratch, ciphertext);
        autokey_cipher(scratch, primer, 1);
        int score = word_score(scratch);
        if (score > *best_score) {
            *best_score = score;
            memcpy(best_primer, primer, (size_t)len + 1);
        }
        return;
    }

    for (char c = 'a'; c <= 'z'; c++) {
        primer[pos] = c;
        brute_force_primer(ciphertext, primer, len, pos + 1, scratch, best_score, best_primer);
    }
}

// Breaks an autokey-ciphered message without knowing the primer: since the
// key stream is just the primer followed by the plaintext, a correctly
// guessed primer decrypts the whole message into recognizable English words,
// while any wrong guess decrypts almost none of them. So this brute-forces
// every primer up to MAX_PRIMER_LEN letters, scores each decryption by its
// count of recognizable words, and keeps the best-scoring one. Deciphers
// message in place with the recovered primer and copies the primer
// (NUL-terminated) into key_out, which must have room for at least
// MAX_PRIMER_LEN + 1 bytes.
void autokey_crib_attack(char *message, char *key_out) {
    unsigned long len = strlen(message);
    char *scratch = malloc(len + 1);
    char primer[MAX_PRIMER_LEN + 1];

    int best_score = -1;
    for (int primer_len = 1; primer_len <= MAX_PRIMER_LEN; primer_len++) {
        primer[primer_len] = '\0';
        brute_force_primer(message, primer, primer_len, 0, scratch, &best_score, key_out);
    }

    free(scratch);
    autokey_cipher(message, key_out, 1);
}

#ifndef NO_ATTACK_MAIN
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <cipher text>\n", argv[0]);
        return 1;
    }

    char *message = argv[1];
    char key[MAX_PRIMER_LEN + 1];
    autokey_crib_attack(message, key);
    printf("Key: %s\n", key);
    printf("%s\n", message);
    return 0;
}
#endif
