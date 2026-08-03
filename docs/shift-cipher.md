# Shift Cipher

Also known as the Caesar cipher. Shifts each letter by a fixed number of
positions in the alphabet, wrapping around from `z` to `a`. Non-alphabetic
characters are left unchanged. Case is preserved.

Source: [`src/shift-cipher.c`](../src/shift-cipher.c)

## Usage

```
bin/shift-cipher [-d] <key> <message>
```

- `<key>` — integer shift amount (negative or >26 values wrap correctly)
- `<message>` — text to transform
- `-d` — decrypt instead of encrypt

## Examples

```
$ bin/shift-cipher 3 "Attack at dawn"
Dwwdfn dw gdzq

$ bin/shift-cipher -d 3 "Dwwdfn dw gdzq"
Attack at dawn
```

## Weaknesses

Only 26 possible keys, so it's trivially broken by brute force or frequency
analysis. See [statistical-shift-attack](statistical-shift-attack.md).
