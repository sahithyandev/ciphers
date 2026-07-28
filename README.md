# ciphers

Cipher implementations in C, one program per cipher.

## Structure

```
src/       one .c file per cipher, each with its own main()
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

| Cipher         | Docs                                         |
| -------------- | -------------------------------------------- |
| Shift (Caesar) | [docs/shift-cipher.md](docs/shift-cipher.md) |
