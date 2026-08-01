# ciphers

Cipher implementations in C, one program per cipher.

## Structure

```
src/       one .c file per cipher, each with its own main()
utils/     shared helper .c files, #include'd directly by ciphers that need them
include/   shared headers, if any
bin/       build output (gitignored)
```

Each cipher's whole logic lives in one function named after the file, e.g.
`vigenere_cipher(char *message, const char *key, int decrypt)` in
`src/vigenere-cipher.c`. It transforms `message` in place. Everything else —
arg parsing, the `-d` flag, printing the result — is boilerplate handled by
`run_cipher`/`run_keyless_cipher` in `utils/cli.c`, so a reader can open any
cipher file and see nothing but the cipher itself.

## Build

```
make          # builds every cipher in src/ into bin/
make clean    # removes bin/
make test     # builds and runs tests/*.c
make coverage # runs tests/*.c under clang source coverage, prints a report
```

Each `src/<name>.c` becomes `bin/<name>` — no Makefile changes needed when adding a new cipher, just drop the file in `src/`.

Tests live in `tests/<name>.c`, one per cipher, and `#include` the matching
`src/<name>.c` directly so they can call its cipher function without a header
split. `main` itself is untested (and untestable-by-design): it's five lines
of boilerplate wrapped in `#ifndef UNIT_TEST`.

## Ciphers

The ciphers in the `ciphers/` directory are (ordered alphabetically):

| Cipher         | Docs                                                       |
| -------------- | ---------------------------------------------------------- |
| Atbash         | [docs/atbash-cipher.md](docs/atbash-cipher.md)             |
| Shift (Caesar) | [docs/shift-cipher.md](docs/shift-cipher.md)               |
| Substitution   | [docs/substitution-cipher.md](docs/substitution-cipher.md) |
| Vigenere       | [docs/vigenere-cipher.md](docs/vigenere-cipher.md)         |

## Adding a new cipher

1. Write `src/<name>.c`. Give it one function named after the cipher —
   `void <name>_cipher(char *message, const char *key, int decrypt)`, or
   `void <name>_cipher(char *message, int decrypt)` if there's no key — that
   transforms `message` in place. This function *is* the cipher; don't put
   any other logic in the file.
2. Wrap `main` in `#ifndef UNIT_TEST` (so tests can include the file and call
   the cipher function directly without a colliding `main`) and have it
   return `run_cipher(argc, argv, <name>_cipher)` (or `run_keyless_cipher` for
   the keyless case) from `utils/cli.c`. Copy it verbatim — it's boilerplate,
   not part of the cipher.
3. `run_cipher`/`run_keyless_cipher` already take every input as a CLI
   argument and support a leading `-d` flag to decrypt instead of encrypt —
   one program handles both directions, no separate binary for decryption,
   and no interactive `scanf`/`fgets` on stdin.
4. Add `docs/<name>.md` documenting the cipher: what it does, its usage
   (`bin/<name> [-d] <args...>`), a couple of example runs, and its known
   weaknesses.
5. Add `tests/<name>.c` with tests covering the cipher's functions.
6. Run `make coverage` and keep line coverage at 50% or above.

No Makefile changes are needed — `make` picks up any `.c` file dropped into
`src/` automatically.

### Automated checks

Every push and pull request runs two checks:

- **Docs and tests must exist.**  
  For every cipher source file, there must be a matching doc file and a matching test file.
- **Tests must pass and hit 50% coverage.**  
  `make test` must succeed, and `make coverage` must report at least 50% line coverage for every file.
