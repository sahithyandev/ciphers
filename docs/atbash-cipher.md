# Atbash

Maps each letter to its mirror position in the alphabet (`a`<->`z`, `b`<->`y`,
and so on). Non-alphabetic characters are left unchanged. Case is preserved.
There's no key, and encryption and decryption are the same operation, so
`-d` is accepted but has no effect on the output.

Source: [`src/atbash-cipher.c`](../src/atbash-cipher.c)

## Usage

```
bin/atbash-cipher [-d] <message>
```

- `<message>` — text to transform
- `-d` — decrypt instead of encrypt (no-op; Atbash is its own inverse)

## Examples

```
$ bin/atbash-cipher "Attack at dawn"
Zggzxp zg wzdm

$ bin/atbash-cipher "Zggzxp zg wzdm"
Attack at dawn
```

## Weaknesses

There's no key at all, so there's nothing to brute force — anyone who
recognizes the pattern can read it instantly. It's also a monoalphabetic
substitution, so it's fully vulnerable to frequency analysis on top of that.
