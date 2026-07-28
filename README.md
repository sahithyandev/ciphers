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
make        # builds every cipher in src/ into bin/
make clean  # removes bin/
make test   # builds, then runs tests/test.sh
```

Each `src/<name>.c` becomes `bin/<name>` — no Makefile changes needed when adding a new cipher, just drop the file in `src/`.

## Ciphers

| Cipher         | Docs                                         |
| -------------- | -------------------------------------------- |
| Shift (Caesar) | [docs/shift-cipher.md](docs/shift-cipher.md) |
