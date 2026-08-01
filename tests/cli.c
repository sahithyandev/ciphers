// Unit tests for utils/cli.c, run via `make test`.
#include <ctype.h>
#include <string.h>
#include "test.h"
#include "../utils/cli.c"

// Stand-ins for a real cipher function: run_cipher/run_keyless_cipher only
// wire parse -> call -> print together, so any fn matching the signature
// exercises that wiring.
static int unreachable_called = 0;

static void uppercase(char *message, const char *key, int decrypt) {
    (void)key; (void)decrypt;
    for (int i = 0; message[i]; i++) {
        message[i] = (char)toupper((unsigned char)message[i]);
    }
}

static void unreachable(char *message, const char *key, int decrypt) {
    (void)message; (void)key; (void)decrypt;
    unreachable_called = 1;
}

static void reverse_flag(char *message, int decrypt) {
    if (decrypt) {
        message[0] = (char)toupper((unsigned char)message[0]);
    }
}

static void unreachable_keyless(char *message, int decrypt) {
    (void)message; (void)decrypt;
    unreachable_called = 1;
}

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

    // run_cipher / run_keyless_cipher: a trivial fn stands in for a real
    // cipher, since these two only wire parse -> call -> print together.
    {
        char prog[] = "prog", key[] = "k", msg[] = "hi";
        char *argv[] = {prog, key, msg};
        CHECK(run_cipher(3, argv, uppercase) == 0);
        CHECK(strcmp(msg, "HI") == 0);
    }
    {
        // bad usage: run_cipher returns 1 without calling fn
        char prog[] = "prog", key[] = "k";
        char *argv[] = {prog, key};
        unreachable_called = 0;
        CHECK(run_cipher(2, argv, unreachable) == 1);
        CHECK(unreachable_called == 0);
    }
    {
        char prog[] = "prog", dflag[] = "-d", msg[] = "hi";
        char *argv[] = {prog, dflag, msg};
        CHECK(run_keyless_cipher(3, argv, reverse_flag) == 0);
        CHECK(strcmp(msg, "Hi") == 0);
    }
    {
        // bad usage: run_keyless_cipher returns 1 without calling fn
        char prog[] = "prog", msg[] = "hi", extra[] = "there";
        char *argv[] = {prog, msg, extra};
        unreachable_called = 0;
        CHECK(run_keyless_cipher(3, argv, unreachable_keyless) == 1);
        CHECK(unreachable_called == 0);
    }

    return TEST_SUMMARY();
}
