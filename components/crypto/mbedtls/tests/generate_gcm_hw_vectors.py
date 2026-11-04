#!/usr/bin/env python3
"""Print the independently generated vector initializers for the board test.

Requires the Python cryptography package, which uses a separate crypto backend.
The board test compares SHA-256 of the complete ciphertext and the GCM tag.
"""

from hashlib import sha256

from cryptography.hazmat.primitives.ciphers.aead import AESGCM


LENGTHS = (0, 1, 15, 16, 17, 31, 32, 63, 64, 65, 127, 128, 255, 256,
           257, 511, 512, 513, 1152, 1196, 2049, 4097)


def hex_bytes(data):
    return ",".join(f"0x{value:02x}" for value in data)


for keylen in (16, 24, 32):
    key = bytes((index * 11 + keylen) & 255 for index in range(keylen))
    for index, length in enumerate(LENGTHS):
        ivlen = (8, 12, 16)[index % 3]
        aadlen = (0, 1, 13, 31)[index % 4]
        iv = bytes((i * 13 + index * 3 + keylen) & 255 for i in range(ivlen))
        aad = bytes((i * 17 + index * 7 + keylen) & 255 for i in range(aadlen))
        plain = bytes((i * 7 + index * 5 + keylen) & 255 for i in range(length))
        encrypted = AESGCM(key).encrypt(iv, plain, aad)
        digest = sha256(encrypted[:-16]).digest()
        print(f"{{{keylen},{length},{ivlen},{aadlen},{index},"
              f"{{{hex_bytes(digest)}}},{{{hex_bytes(encrypted[-16:])}}}}},")
