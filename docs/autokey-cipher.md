# Autokey

Like [Vigenere](vigenere-cipher.md), but the key never repeats: after a
short primer keyword, the key stream continues with the plaintext itself
(`key = primer + plaintext`). Each letter of the message is shifted by the
alphabet position of the matching key letter (`a`=0, `b`=1, ... `z`=25).
Non-alphabetic characters are left unchanged and case is preserved.

Decryption runs the same process in reverse: the primer gives the first few
key letters, and each recovered plaintext letter is appended to the key so
it can decrypt the next one.

Source: [`src/autokey-cipher.c`](../src/autokey-cipher.c)

## Usage

```
bin/autokey-cipher [-d] <key> <message>
```

- `<key>` — primer keyword prepended to the message to form the key stream (letters only)
- `<message>` — text to transform
- `-d` — decrypt instead of encrypt

## Examples

```
$ bin/autokey-cipher queenly "Attack at dawn"
Qnxepv yt wtwp

$ bin/autokey-cipher -d queenly "Qnxepv yt wtwp"
Attack at dawn
```

## Weaknesses

Because the key never repeats, the usual attacks on Vigenere (Kasiski
examination, index of coincidence) don't work here.

But the key is made of plaintext, so it contains ordinary words. An attacker
can guess a likely plaintext fragment (a crib, e.g. "the"), slide it across the
ciphertext at every offset, and decrypt with it as if it were the key at that point.
Most offsets produce gibberish, but the correct offset produces a readable
fragment both in the plaintext and, shifted right by the primer's length,
in the key itself, which reveals more of the primer and cascades into
recovering the rest of the message. See
[autokey-crib-attack](autokey-crib-attack.md).
