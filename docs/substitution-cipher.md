# Substitution Cipher

Each letter is substituted with another letter according to a fixed key. Non-alphabetic characters are left unchanged. Case is preserved.

Source: [`src/substitution-cipher.c`](../src/substitution-cipher.c)

## Usage

```
bin/substitution-cipher [-d] <key> <message>
```

- `<key>` — substitution key. Case-insensitive; non-alphabetic characters are
  dropped and duplicate letters are collapsed to their first occurrence.
  Whatever's left is filled out with the remaining letters of the alphabet,
  in order (e.g. `zebra` becomes `zebracdfghijklmnopqstuvwxy`).
- `<message>` — text to transform
- `-d` — decrypt instead of encrypt

## Examples

```
$ bin/substitution-cipher zebra "Attack at dawn"
Zsszbi zs rzvl

$ bin/substitution-cipher -d zebra "Zsszbi zs rzvl"
Attack at dawn
```

## Weaknesses

A monoalphabetic substitution has 26! possible keys, so it resists brute
force, but each letter always maps to the same output letter. That makes it
vulnerable to frequency analysis: in English text `e` and `t` are the most
common letters, common short words (`the`, `and`) are recognizable by
pattern, and repeated letters/doubles leak structure. Given enough
ciphertext it can usually be solved by hand.
