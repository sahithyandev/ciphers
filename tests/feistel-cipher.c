// Tests for src/feistel-cipher.c, run via `make test`.
#include <string.h>
#include "test.h"
#include "../src/feistel-cipher.c"

int main(void) {
    TEST_INIT();

    {
        // matches encrypt_block(0xABCD, generate_subkeys(0x1234)) from the
        // reference Python, byte for byte
        char msg[] = {(char)0xAB, (char)0xCD, '\0'};
        feistel_cipher(msg, "0x1234", 0);
        CHECK((unsigned char)msg[0] == 0xBD && (unsigned char)msg[1] == 0xEF);
    }
    {
        char msg[] = "Hello, World!";
        char expect[] = "Hello, World!";
        feistel_cipher(msg, "0x1234", 0);
        feistel_cipher(msg, "0x1234", 1);
        CHECK(strcmp(msg, expect) == 0);
    }
    {
        // odd-length message exercises the trailing single-byte case
        char msg[] = "abcde";
        char expect[] = "abcde";
        feistel_cipher(msg, "42", 0);
        feistel_cipher(msg, "42", 1);
        CHECK(strcmp(msg, expect) == 0);
    }
    {
        char msg[] = "";
        feistel_cipher(msg, "1", 0);
        CHECK(strcmp(msg, "") == 0);
    }
    {
        unsigned char bytes[] = {0xBD, 0xEF, 0x00, 0x0A};
        char b64[9];
        base64_encode(bytes, 4, b64);
        CHECK(strcmp(b64, "ve8ACg==") == 0);
    }
    {
        unsigned char out[4] = {0};
        long len = base64_decode("ve8ACg==", out);
        CHECK(len == 4 && out[0] == 0xBD && out[1] == 0xEF && out[2] == 0x00 && out[3] == 0x0A);
    }
    {
        unsigned char out[4];
        CHECK(base64_decode("abc", out) == -1);   // length not a multiple of 4
        CHECK(base64_decode("!!!!", out) == -1);  // non-base64 characters
    }

    return TEST_SUMMARY();
}
