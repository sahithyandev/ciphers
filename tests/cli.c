// Unit tests for utils/cli.c, run via `make test`.
#include <string.h>
#include "test.h"
#include "../utils/cli.c"

int main(void) {
    TEST_INIT();

    // success: encrypt (no flag)
    {
        char prog[] = "prog", key[] = "3", msg[] = "hi";
        char *argv[] = {prog, key, msg};
        int decrypt; char *k, *m;
        CHECK(parse_args(3, argv, &decrypt, &k, &m) == 0);
        CHECK(decrypt == 0);
        CHECK(strcmp(k, "3") == 0);
        CHECK(strcmp(m, "hi") == 0);
    }
    // success: -d sets decrypt and shifts key/message over
    {
        char prog[] = "prog", dflag[] = "-d", key[] = "3", msg[] = "hi";
        char *argv[] = {prog, dflag, key, msg};
        int decrypt; char *k, *m;
        CHECK(parse_args(4, argv, &decrypt, &k, &m) == 0);
        CHECK(decrypt == 1);
        CHECK(strcmp(k, "3") == 0);
        CHECK(strcmp(m, "hi") == 0);
    }
    // unknown flag
    {
        char prog[] = "prog", bad[] = "-z", key[] = "3", msg[] = "hi";
        char *argv[] = {prog, bad, key, msg};
        int decrypt; char *k, *m;
        CHECK(parse_args(4, argv, &decrypt, &k, &m) == 1);
    }
    // too few args
    {
        char prog[] = "prog", key[] = "3";
        char *argv[] = {prog, key};
        int decrypt; char *k, *m;
        CHECK(parse_args(2, argv, &decrypt, &k, &m) == 1);
    }
    // too many args
    {
        char prog[] = "prog", key[] = "3", msg[] = "hi", extra[] = "there";
        char *argv[] = {prog, key, msg, extra};
        int decrypt; char *k, *m;
        CHECK(parse_args(4, argv, &decrypt, &k, &m) == 1);
    }
    // argc == 0: argi < argc is false, short-circuiting both flag checks
    {
        char prog[] = "prog";
        char *argv[] = {prog};
        int decrypt; char *k, *m;
        CHECK(parse_args(0, argv, &decrypt, &k, &m) == 1);
    }

    // parse_args_no_key: same flag handling, but a single positional arg
    {
        char prog[] = "prog", msg[] = "hi";
        char *argv[] = {prog, msg};
        int decrypt; char *m;
        CHECK(parse_args_no_key(2, argv, &decrypt, &m) == 0);
        CHECK(decrypt == 0);
        CHECK(strcmp(m, "hi") == 0);
    }
    {
        char prog[] = "prog", dflag[] = "-d", msg[] = "hi";
        char *argv[] = {prog, dflag, msg};
        int decrypt; char *m;
        CHECK(parse_args_no_key(3, argv, &decrypt, &m) == 0);
        CHECK(decrypt == 1);
        CHECK(strcmp(m, "hi") == 0);
    }
    {
        // unknown flag
        char prog[] = "prog", bad[] = "-z", msg[] = "hi";
        char *argv[] = {prog, bad, msg};
        int decrypt; char *m;
        CHECK(parse_args_no_key(3, argv, &decrypt, &m) == 1);
    }
    {
        // too many args
        char prog[] = "prog", msg[] = "hi", extra[] = "there";
        char *argv[] = {prog, msg, extra};
        int decrypt; char *m;
        CHECK(parse_args_no_key(3, argv, &decrypt, &m) == 1);
    }
    {
        // too few args
        char prog[] = "prog";
        char *argv[] = {prog};
        int decrypt; char *m;
        CHECK(parse_args_no_key(1, argv, &decrypt, &m) == 1);
    }

    return TEST_SUMMARY();
}
