# Substitution Cipher

Each letter is substituted with another letter according to a fixed key. Non-alphabetic characters are left unchanged. Case is preserved.

Source: [`src/substitution-cipher.c`](../src/substitution-cipher.c)

## Usage

```
bin/substitution-cipher [-d] <key> <message>
```

- `<key>` — substitution key. A string of 26 or less unique characters. Duplicates are removed. Non-alphabetic characters are ignored. Filled with the alphabet in order for remaining.
- `<message>` — text to transform
- `-d` — decrypt instead of encrypt

## Examples

```
$ bin/substitution-cipher zebra "Attack at dawn"

$ bin/substitution-cipher -d zebra "Dwwdfn dw gdzq"
```
