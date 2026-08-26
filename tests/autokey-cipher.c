// Tests for src/autokey-cipher.c, run via `make test`. The source is
// included directly (compiled with -DUNIT_TEST, which omits its main() so
// this file's main() is the only one), giving direct access to
// autokey_cipher without going through argv.
#include <string.h>
#include "test.h"
#include "../src/autokey-cipher.c"

int main(void) {
    TEST_INIT();

    {
        char msg[] = "attack at dawn";
        autokey_cipher(msg, "queenly", 0);
        CHECK(strcmp(msg, "qnxepv yt wtwp") == 0);
    }
    {
        char msg[] = "qnxepv yt wtwp";
        autokey_cipher(msg, "queenly", 1);
        CHECK(strcmp(msg, "attack at dawn") == 0);
    }
    {
        // non-letters don't consume a key/stream position
        char msg[] = "a, a!";
        autokey_cipher(msg, "ab", 0);
        CHECK(strcmp(msg, "a, b!") == 0);
    }
    {
        // uppercase key and message letters, case preserved; second letter's
        // key comes from the first plaintext letter ('a', shift 0)
        char msg[] = "aZ";
        autokey_cipher(msg, "B", 0);
        CHECK(strcmp(msg, "bZ") == 0);
    }
    {
        char msg[] = "z";
        autokey_cipher(msg, "b", 0);
        CHECK(strcmp(msg, "a") == 0); // wrap forward
    }
    {
        // primer shorter than message: key stream continues with plaintext
        char msg[] = "meetatthefountain";
        autokey_cipher(msg, "kilt", 0);
        CHECK(strcmp(msg, "wmpmmxxaeyhbryoca") == 0);
    }
    {
        char msg[] = "wmpmmxxaeyhbryoca";
        autokey_cipher(msg, "kilt", 1);
        CHECK(strcmp(msg, "meetatthefountain") == 0);
    }
    {
        // empty message
        char msg[] = "";
        autokey_cipher(msg, "key", 0);
        CHECK(strcmp(msg, "") == 0);
    }
    {
        // decrypt wrap-forward path (offset would go negative)
        char msg[] = "a";
        autokey_cipher(msg, "b", 1);
        CHECK(strcmp(msg, "z") == 0);
    }

    // exhaustive round trip over every key letter x message letter
    for (char k = 'a'; k <= 'z'; k++) {
        for (char c = 'a'; c <= 'z'; c++) {
            char key[] = {k, '\0'};
            char msg[] = {c, '\0'};
            autokey_cipher(msg, key, 0);
            autokey_cipher(msg, key, 1);
            CHECK(msg[0] == c);
        }
    }

    return TEST_SUMMARY();
}
