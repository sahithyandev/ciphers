# Feistel

A toy Feistel-network block cipher: it encrypts a block by splitting it into
two halves and running several identical rounds that mix one half into the
other with a round key, instead of transforming the whole block directly
like the other ciphers in this repo. Decryption reuses the exact same round
logic as encryption, just with the round keys applied in reverse order.

## How it works

The message is split into 2-byte blocks. Each block's first byte is the left
half, the second byte is the right half. If the message has an odd number of
bytes, the leftover last byte is handled separately, below.

The 4 round subkeys come from repeatedly rotating the 16-bit key left by one
bit and keeping its low byte:

```
key = rotate_left_1(key); subkey[i] = low_byte(key)   # i = 0..3
```

Subkey 0 comes from rotating the key once, subkey 1 from rotating it twice,
and so on.

Each block runs through 4 rounds, one per subkey, and each round mixes the
right half into the left half and swaps them:

```
left, right = right, left XOR right XOR subkey[i]
```

After round 3, the resulting `(left, right)` pair is the ciphertext block;
unlike some Feistel ciphers, there's no final swap. An odd trailing byte has
no second half to Feistel it against, so it's just XORed with subkey 0
instead.

Decrypting runs the same 4 rounds on the ciphertext block, but with the
subkeys in reverse order (subkey 3 first, subkey 0 last) and the halves
swapped going in and coming out. That reverse-order-and-swap trick is what
makes any Feistel network decryptable no matter what the round function
does: `left XOR right XOR subkey` happens to be invertible on its own, but
the same scheme works just as well with a round function that isn't
invertible at all, which is the actual point of the Feistel construction.

Since the encrypted bytes are arbitrary binary, not valid text, the CLI
prints and reads them as base64 rather than raw bytes.

Source: [`src/feistel-cipher.c`](../src/feistel-cipher.c)

## Usage

```
bin/feistel-cipher [-d] <key> <message>
```

- `<key>` — a 16-bit integer (decimal or `0x` hex)
- `<message>` — text to encrypt, or base64 ciphertext to decrypt
- `-d` — decrypt instead of encrypt

## Examples

```
$ bin/feistel-cipher 0x1234 "Hello, World!"
FaQciVzKJ/4ClBSBSQ==

$ bin/feistel-cipher -d 0x1234 "FaQciVzKJ/4ClBSBSQ=="
Hello, World!
```

## Weaknesses

Only 4 rounds and an 8-bit effective subkey per round (derived from a single
16-bit key via a simple bit rotation, not an independent key schedule) make
this trivially breakable by brute force or known-plaintext attacks. A real
Feistel cipher (e.g. DES) uses many more rounds and a non-linear round
function.
