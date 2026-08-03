// Docs: docs/feistel-cipher.md
#include <stdlib.h>
#include <string.h>
#include "../utils/cli.c"

#define ROUNDS 4
#define HALF_MASK 0xFF

// Rotates the 16-bit key left by one bit per round and keeps the low byte as
// that round's subkey.
static void generate_subkeys(unsigned key, unsigned char subkeys[ROUNDS]) {
    unsigned current = key & 0xFFFF;
    for (int i = 0; i < ROUNDS; i++) {
        current = ((current << 1) | (current >> 15)) & 0xFFFF;
        subkeys[i] = (unsigned char)(current & HALF_MASK);
    }
}

// One Feistel round: (left, right) -> (right, left ^ right ^ subkey).
static void round_step(unsigned *left, unsigned *right, unsigned char subkey) {
    unsigned new_right = *left ^ (*right ^ subkey);
    *left = *right;
    *right = new_right;
}

// Encrypts/decrypts a 16-bit block. There's no swap after the last round, so
// decryption swaps the halves, runs the same rounds with subkeys reversed,
// then swaps back.
static unsigned feistel_block(unsigned block, const unsigned char subkeys[ROUNDS], int decrypt) {
    unsigned left = decrypt ? block & HALF_MASK : block >> 8;
    unsigned right = decrypt ? block >> 8 : block & HALF_MASK;

    for (int i = 0; i < ROUNDS; i++) {
        round_step(&left, &right, subkeys[decrypt ? ROUNDS - 1 - i : i]);
    }

    return decrypt ? (right << 8) | left : (left << 8) | right;
}

// Transforms message in place, two bytes per block. An odd trailing byte
// (no partner to form a block with) is XORed with the first subkey instead.
void feistel_cipher(char *message, const char *key, int decrypt) {
    unsigned char subkeys[ROUNDS];
    generate_subkeys((unsigned)strtoul(key, NULL, 0), subkeys);

    unsigned char *bytes = (unsigned char *)message;
    size_t len = strlen(message);
    size_t i;

    for (i = 0; i + 1 < len; i += 2) {
        unsigned block = ((unsigned)bytes[i] << 8) | bytes[i + 1];
        unsigned result = feistel_block(block, subkeys, decrypt);
        bytes[i] = (unsigned char)(result >> 8);
        bytes[i + 1] = (unsigned char)(result & 0xFF);
    }
    if (i < len) {
        bytes[i] ^= subkeys[0];
    }
}

static const char B64_CHARS[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Writes standard base64 (with '=' padding) into out, sized 4*((len+2)/3)+1
// by the caller.
static void base64_encode(const unsigned char *bytes, size_t len, char *out) {
    size_t i, o = 0;
    for (i = 0; i + 2 < len; i += 3) {
        unsigned v = ((unsigned)bytes[i] << 16) | ((unsigned)bytes[i + 1] << 8) | bytes[i + 2];
        out[o++] = B64_CHARS[(v >> 18) & 0x3F];
        out[o++] = B64_CHARS[(v >> 12) & 0x3F];
        out[o++] = B64_CHARS[(v >> 6) & 0x3F];
        out[o++] = B64_CHARS[v & 0x3F];
    }

    size_t rem = len - i;
    if (rem == 1) {
        unsigned v = (unsigned)bytes[i] << 16;
        out[o++] = B64_CHARS[(v >> 18) & 0x3F];
        out[o++] = B64_CHARS[(v >> 12) & 0x3F];
        out[o++] = '=';
        out[o++] = '=';
    } else if (rem == 2) {
        unsigned v = ((unsigned)bytes[i] << 16) | ((unsigned)bytes[i + 1] << 8);
        out[o++] = B64_CHARS[(v >> 18) & 0x3F];
        out[o++] = B64_CHARS[(v >> 12) & 0x3F];
        out[o++] = B64_CHARS[(v >> 6) & 0x3F];
        out[o++] = '=';
    }
    out[o] = '\0';
}

static int base64_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

// Decodes a base64 string into out (sized 3*(strlen(b64)/4) by the caller).
// Returns the decoded length, or -1 if b64 is malformed.
static long base64_decode(const char *b64, unsigned char *out) {
    size_t len = strlen(b64);
    if (len % 4 != 0) return -1;

    int pad = 0;
    if (len > 0 && b64[len - 1] == '=') pad = 1;
    if (len > 0 && b64[len - 2] == '=') pad = 2;

    size_t o = 0;
    for (size_t i = 0; i < len; i += 4) {
        int group_pad = (i + 4 == len) ? pad : 0;

        int v[4];
        for (int j = 0; j < 4; j++) {
            char c = b64[i + j];
            if (group_pad && j >= 4 - group_pad) {
                if (c != '=') return -1;
                v[j] = 0;
            } else {
                v[j] = base64_value(c);
                if (v[j] < 0) return -1;
            }
        }

        unsigned val = ((unsigned)v[0] << 18) | ((unsigned)v[1] << 12) | ((unsigned)v[2] << 6) | (unsigned)v[3];
        out[o++] = (unsigned char)(val >> 16);
        if (group_pad < 2) out[o++] = (unsigned char)(val >> 8);
        if (group_pad < 1) out[o++] = (unsigned char)val;
    }
    return (long)o;
}

#ifndef UNIT_TEST
// Ciphertext is arbitrary bytes, so it's printed/read as base64 rather than
// mutating the message argv in place like the other ciphers do.
int main(int argc, char *argv[]) {
    int decrypt;
    char *key, *message;
    if (parse_args(argc, argv, &decrypt, &key, &message) != 0) {
        return 1;
    }

    if (!decrypt) {
        size_t len = strlen(message);
        char *b64 = malloc(4 * ((len + 2) / 3) + 1);
        feistel_cipher(message, key, 0);
        base64_encode((unsigned char *)message, len, b64);
        printf("%s\n", b64);
        free(b64);
    } else {
        unsigned char *buf = malloc(3 * (strlen(message) / 4) + 1);
        long len = base64_decode(message, buf);
        if (len < 0) {
            fprintf(stderr, "%s: ciphertext must be base64\n", argv[0]);
            free(buf);
            return 1;
        }
        buf[len] = '\0';
        feistel_cipher((char *)buf, key, 1);
        printf("%s\n", buf);
        free(buf);
    }
    return 0;
}
#endif
