# Vigenere

It is pronounced as "vee-zhuh-NAIR".

A polyalphabetic substitution cipher. Each letter of the message is shifted
by the alphabet position of the corresponding letter in a repeating keyword
(`a`=0, `b`=1, ... `z`=25), instead of by a fixed amount like the shift
cipher. Non-alphabetic characters are left unchanged. Case is preserved.

Source: [`src/vigenere-cipher.c`](../src/vigenere-cipher.c)

## Usage

```
bin/vigenere-cipher [-d] <key> <message>
```

- `<key>` — keyword to repeat over the message (letters only)
- `<message>` — text to transform
- `-d` — decrypt instead of encrypt

## Examples

```
$ bin/vigenere-cipher lemon "Attack at dawn"
Lxfopv ef rnhr

$ bin/vigenere-cipher -d lemon "Lxfopv ef rnhr"
Attack at dawn
```

## Weaknesses

Because the same key repeats, the ciphertext can be split into N interleaved
shift-cipher streams once the key length is found (e.g. via Kasiski
examination or index of coincidence), and each stream is then vulnerable to
ordinary frequency analysis. Short, repeated, or guessable keys make this
easy; a key as long as the message (used only once) removes the weakness
entirely.
