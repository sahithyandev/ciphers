# Statistical Shift Attack

Breaks a [shift cipher](shift-cipher.md) by frequency analysis. English text has
a distinctive letter frequency distribution (E and T are common, Q and Z are
rare), and shifting the alphabet just rotates that distribution. So instead of
brute-forcing all 26 keys and eyeballing the output, this counts the letters in
the ciphertext, tries rotating that histogram back by each of the 26 possible
keys, and picks whichever rotation lines up best with real English letter
frequencies.

Source: [`attacks/statistical-shift-attack.c`](../attacks/statistical-shift-attack.c)

## Usage

```
bin/statistical-shift-attack <cipher text>
```

- `<cipher text>` — a shift-enciphered message, ideally at least a sentence or
  two long

Prints the recovered key, then the deciphered message.

## Examples

```
$ bin/statistical-shift-attack WELMXLCEREKMPI
Key: 4
SAHITHYANAGILE
```

## Limitations

Needs enough ciphertext for its letter histogram to resemble English — a
handful of characters (e.g. `"Hello, World!"`) don't carry enough signal and
can pick the wrong key. Also assumes the underlying plaintext is English;
other languages or non-prose text (code, random strings) have different
frequency profiles and won't be recovered reliably.
