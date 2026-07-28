#!/usr/bin/env bash
# Black-box tests for bin/shift-cipher: run it and check stdout/exit code.
set -u
cd "$(dirname "$0")/.."
BIN=bin/shift-cipher

pass=0
fail=0

check() {
    local desc="$1" expected="$2" actual="$3"
    if [ "$expected" = "$actual" ]; then
        pass=$((pass + 1))
    else
        fail=$((fail + 1))
        echo "FAIL: $desc"
        echo "  expected: $expected"
        echo "  actual:   $actual"
    fi
}

# shift-cipher
check "encrypt basic" "Dwwdfn dw gdzq" "$($BIN 3 'Attack at dawn')"
check "decrypt basic" "Attack at dawn" "$($BIN -d 3 'Dwwdfn dw gdzq')"
check "wrap forward" "abc" "$($BIN 3 'xyz')"
check "negative shift" "xyz" "$($BIN -- -3 'abc')"
check "shift > 26" "def" "$($BIN 29 'abc')"
check "case preserved" "BcA" "$($BIN 1 'AbZ')"
check "non-alpha unchanged" "Mjqqt, Btwqi! 123" "$($BIN 5 'Hello, World! 123')"

roundtrip=$($BIN -d 17 "$($BIN 17 'The Quick Brown Fox! 42')")
check "encrypt/decrypt roundtrip" "The Quick Brown Fox! 42" "$roundtrip"

$BIN >/dev/null 2>&1
check "missing args exits nonzero" "1" "$?"

echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
