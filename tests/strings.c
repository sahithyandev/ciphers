// Unit tests for utils/strings.c, run via `make test`.
#include "test.h"
#include "../utils/strings.c"

int main(void) {
    TEST_INIT();

    CHECK(to_lower('A') == 'a');
    CHECK(to_lower('z') == 'z');           // already lowercase
    CHECK(to_lower('1') == '1');           // non-letter passthrough
    CHECK(to_upper('a') == 'A');
    CHECK(to_upper('Z') == 'Z');           // already uppercase
    CHECK(to_upper('1') == '1');           // non-letter passthrough

    return TEST_SUMMARY();
}
