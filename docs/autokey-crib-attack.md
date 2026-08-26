# Autokey Crib Attack

Breaks an [autokey cipher](autokey-cipher.md) by recovering its primer. The
[Weaknesses](autokey-cipher.md#weaknesses) section describes crib dragging: a
human guesses a plaintext fragment, slides it across the ciphertext, and
looks for a decryption that reads like real English. This automates that
judgment call — instead of a human eyeballing fragments, it brute-forces
every short primer, decrypts the whole message with each, and scores the
result by how many of its words are in a small dictionary of common English
words. A correct primer decrypts into mostly real words; a wrong one decrypts
into near-gibberish that matches almost none, so the highest-scoring primer
wins.

Source: [`attacks/autokey-crib-attack.c`](../attacks/autokey-crib-attack.c)

## Usage

```
bin/autokey-crib-attack <cipher text>
```

- `<cipher text>` — an autokey-enciphered message, long enough to contain
  several dictionary words

Prints the recovered primer, then the deciphered message.

## Examples

```
$ bin/autokey-crib-attack "wmpm mx xae yhbryoca geie gle fek whzqw ufvhhv jb zlf pbcii ecie poi fnu wwnf srw lhr ebyht azqy lprok lzmemik fwam"
Key: kilt
meet at the fountain near the old stone bridge by the river when the sun sets and the birds sing their evening song
```

## Limitations

Brute-forcing a primer of length N is 26^N candidates, so this only searches
primers up to 4 letters — anything longer (like the `queenly` example in
`autokey-cipher.md`) is out of reach. It also needs enough ciphertext for the
word-count score to reliably favor the real primer: a short message like
`"attack at dawn"` doesn't contain enough dictionary words for the correct
decryption to stand out from the ~450,000 wrong ones it's compared against,
and can recover the wrong primer. Non-English plaintext, or English made of
uncommon words, won't score well against the built-in dictionary either.
