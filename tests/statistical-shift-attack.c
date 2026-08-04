// Tests for attacks/statistical-shift-attack.c, run via `make test`.
#include <string.h>
#include "test.h"
#include "../attacks/statistical-shift-attack.c"

int main(void) {
    TEST_INIT();

    {
        // Round trip: encrypt with a known key, recover both the key and
        // the original message via the attack.
        char key7[] = "7";
        char msg[] = "The quick brown fox jumps over the lazy dog and runs away";
        shift_cipher(msg, key7, 0);
        int key = statistical_shift_attack(msg);
        CHECK(key == 7);
        CHECK(strcmp(msg, "The quick brown fox jumps over the lazy dog and runs away") == 0);
    }
    {
        // The doc/Python sample.
        char msg[] = "WELMXLCEREKMPI";
        int key = statistical_shift_attack(msg);
        CHECK(key == 4);
        CHECK(strcmp(msg, "SAHITHYANAGILE") == 0);
    }
    {
        // Already plaintext (key 0): frequency analysis should leave it alone.
        char msg[] = "The five boxing wizards jump quickly over the lazy sleeping dog";
        int key = statistical_shift_attack(msg);
        CHECK(key == 0);
    }
    {
        // No letters at all: must not divide by zero or crash.
        char msg[] = "1234 !?.,";
        int key = statistical_shift_attack(msg);
        CHECK(key == 0);
        CHECK(strcmp(msg, "1234 !?.,") == 0);
    }
    {
        // Empty string.
        char msg[] = "";
        int key = statistical_shift_attack(msg);
        CHECK(key == 0);
    }
    {
        // '[' and '{' sit just past 'Z' and 'z' in ASCII: exercises the
        // is-letter short-circuit false branch that a plain space doesn't reach.
        char key9[] = "9";
        char msg[] = "The quick {brown} fox [jumps] over the lazy dog and runs away";
        char expect[] = "The quick {brown} fox [jumps] over the lazy dog and runs away";
        shift_cipher(msg, key9, 0);
        int key = statistical_shift_attack(msg);
        CHECK(key == 9);
        CHECK(strcmp(msg, expect) == 0);
    }
    {
        // Case and non-letters are preserved through the recovered plaintext.
        // (Needs a longer message than "Hello, World!" for the letter
        // histogram to reliably favor the right key.)
        char key13[] = "13";
        char msg[] = "The Quick Brown Fox jumps over the lazy Dog, said the fox: 42!";
        shift_cipher(msg, key13, 0);
        int key = statistical_shift_attack(msg);
        CHECK(key == 13);
        CHECK(strcmp(msg, "The Quick Brown Fox jumps over the lazy Dog, said the fox: 42!") == 0);
    }

    return TEST_SUMMARY();
}
