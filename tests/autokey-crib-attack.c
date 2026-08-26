// Tests for attacks/autokey-crib-attack.c, run via `make test`.
#include <string.h>
#include "test.h"
#include "../attacks/autokey-crib-attack.c"

int main(void) {
    TEST_INIT();

    {
        // Round trip: encrypt with a known primer, recover both the primer
        // and the original message via the attack.
        char msg[] = "the quick brown fox jumps over the lazy dog while the sun "
                      "sets slowly behind the distant mountains and the evening "
                      "breeze carries the scent of pine trees through the quiet "
                      "valley where travelers often stop to rest beside the old "
                      "stone bridge that crosses the gentle river flowing toward "
                      "the sea";
        char expect[sizeof(msg)];
        strcpy(expect, msg);

        autokey_cipher(msg, "kilt", 0);
        char key[MAX_PRIMER_LEN + 1];
        autokey_crib_attack(msg, key);
        CHECK(strcmp(key, "kilt") == 0);
        CHECK(strcmp(msg, expect) == 0);
    }
    {
        // A 2-letter primer, well within the brute-force bound.
        char msg[] = "the quick brown fox jumps over the lazy dog while the sun "
                      "sets slowly behind the distant mountains and the evening "
                      "breeze carries the scent of pine trees through the quiet "
                      "valley where travelers often stop to rest beside the old "
                      "stone bridge that crosses the gentle river flowing toward "
                      "the sea";
        char expect[sizeof(msg)];
        strcpy(expect, msg);

        autokey_cipher(msg, "ab", 0);
        char key[MAX_PRIMER_LEN + 1];
        autokey_crib_attack(msg, key);
        CHECK(strcmp(key, "ab") == 0);
        CHECK(strcmp(msg, expect) == 0);
    }
    {
        // The doc's example.
        char msg[] = "wmpm mx xae yhbryoca geie gle fek whzqw ufvhhv jb zlf pbcii "
                      "ecie poi fnu wwnf srw lhr ebyht azqy lprok lzmemik fwam";
        char key[MAX_PRIMER_LEN + 1];
        autokey_crib_attack(msg, key);
        CHECK(strcmp(key, "kilt") == 0);
        CHECK(strcmp(msg, "meet at the fountain near the old stone bridge by the "
                           "river when the sun sets and the birds sing their "
                           "evening song") == 0);
    }
    {
        // No letters at all: must not divide by zero or crash.
        char msg[] = "1234 !?.,";
        char expect[] = "1234 !?.,";
        char key[MAX_PRIMER_LEN + 1];
        autokey_crib_attack(msg, key);
        CHECK(strcmp(msg, expect) == 0);
    }
    {
        // Empty string.
        char msg[] = "";
        char key[MAX_PRIMER_LEN + 1];
        autokey_crib_attack(msg, key);
        CHECK(strcmp(msg, "") == 0);
    }
    {
        // word_score: known words count ("the", "on", "it"), unknown words
        // ("cat", "sat") don't.
        CHECK(word_score("the cat sat on it") == 3);
        CHECK(word_score("zzz qxqx") == 0);
        CHECK(word_score("") == 0);
        CHECK(word_score("the, the! the?") == 3);
    }

    return TEST_SUMMARY();
}
