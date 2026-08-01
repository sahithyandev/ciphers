# ciphers

Cipher implementations in C, one program per cipher.

## Structure

```
src/       one .c file per cipher, each with its own main()
utils/     shared helper .c files, #include'd directly by ciphers that need them
include/   shared headers, if any
bin/       build output (gitignored)
```

## Build

```
make          # builds every cipher in src/ into bin/
make clean    # removes bin/
make test     # builds and runs tests/*.c
make coverage # runs tests/*.c under clang source coverage, prints a report
```

Each `src/<name>.c` becomes `bin/<name>` — no Makefile changes needed when adding a new cipher, just drop the file in `src/`.

Tests live in `tests/<name>.c`, one per cipher, and `#include` the matching
`src/<name>.c` directly so they can call its functions and its `main`
(compiled as `cipher_main`) without a header split.

## Ciphers

The ciphers in the `ciphers/` directory are (ordered alphabetically):

| Cipher         | Docs                                                       |
| -------------- | ---------------------------------------------------------- |
| Atbash         | [docs/atbash-cipher.md](docs/atbash-cipher.md)             |
| Shift (Caesar) | [docs/shift-cipher.md](docs/shift-cipher.md)               |
| Substitution   | [docs/substitution-cipher.md](docs/substitution-cipher.md) |
| Vigenere       | [docs/vigenere-cipher.md](docs/vigenere-cipher.md)         |

## Adding a new cipher

1. Write `src/<name>.c`. Give it a `cipher_main(int argc, char *argv[])`
   function instead of `main`, and wrap `main` itself in `#ifndef UNIT_TEST`
   so tests can include the file and call `cipher_main` directly without it
   colliding with their own `main`.
2. Take every input as a CLI argument — the key, the message, everything.
   Never prompt for input interactively (no `scanf`/`fgets` on stdin), since
   the automated checks run the binary non-interactively.
3. Support a `-d` flag as the first argument to run in decrypt mode instead
   of encrypt. One program should handle both directions. Don't write a separate
   binary for decryption.
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
