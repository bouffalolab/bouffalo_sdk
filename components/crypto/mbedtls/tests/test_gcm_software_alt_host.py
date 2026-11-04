#!/usr/bin/env python3
"""Compile the production GCM ALT on the host and compare it with OpenSSL.

Requires a C compiler and Python's cryptography package. The SDK and upstream
Mbed TLS source trees are only read; outputs are placed in a temporary directory.
"""
import concurrent.futures
import csv
import ctypes as c
import os
from pathlib import Path
import random
import subprocess
import tempfile

from cryptography.hazmat.primitives.ciphers.aead import AESGCM

ROOT = Path(__file__).resolve().parents[1]
PORT = ROOT / 'port' / 'hw_acc'
UPSTREAM = ROOT / 'mbedtls_v3'


class Context(c.Structure):
    _fields_ = [('backend', c.c_void_p)]


def build(directory, large_table, camellia=False, platform_memory=False):
    public_config = directory / 'public_config.h'
    public_config.write_text('#define MBEDTLS_AES_C\n#define MBEDTLS_GCM_C\n#define MBEDTLS_GCM_ALT\n' +
                             ('#define MBEDTLS_CAMELLIA_C\n' if camellia else '') +
                             ('#define MBEDTLS_PLATFORM_C\n#define MBEDTLS_PLATFORM_MEMORY\n' if platform_memory else ''))
    compiler = os.environ.get('CC', 'cc')
    flags = [compiler, '-fPIC', '-O3', '-Wall', '-Wextra', '-Werror',
             '-I' + str(PORT), '-I' + str(UPSTREAM / 'include'), '-I' + str(UPSTREAM / 'library')]
    if camellia:
        flags.append('-DCONFIG_MBEDTLS_CIPHER_CAMELLIA_ENABLED=1')
    private = ['-DMBEDTLS_SELF_TEST', '-include', str(PORT / 'gcm_software_namespace.h')]
    if large_table:
        private.append('-DCONFIG_MBEDTLS_GCM_SOFTWARE_LARGE_TABLE=1')
    public = ['-DMBEDTLS_CONFIG_FILE="' + str(public_config) + '"']
    objects = []
    sources = [(UPSTREAM / 'library' / name, private) for name in ('aes.c', 'gcm.c', 'block_cipher.c', 'camellia.c')]
    sources += [(PORT / 'gcm_software_backend.c', private), (PORT / 'gcm_software_alt.c', public)]
    sources += [(UPSTREAM / 'library' / name, public) for name in ('platform_util.c', 'constant_time.c')]
    if platform_memory:
        sources.append((UPSTREAM / 'library' / 'platform.c', public))
    for number, (source, options) in enumerate(sources):
        obj = directory / f'{number}.o'
        subprocess.run(flags + options + ['-c', str(source), '-o', str(obj)], check=True)
        objects.append(str(obj))
    library = directory / 'libgcm_alt.so'
    subprocess.run([compiler, '-shared', '-Wl,--no-undefined', *objects, '-o', str(library)], check=True)
    return library


def setup_api(path):
    lib = c.CDLL(str(path))
    ptr, size, integer, uint = c.c_void_p, c.c_size_t, c.c_int, c.c_uint
    signatures = {
        'mbedtls_gcm_init': [ptr], 'mbedtls_gcm_setkey': [ptr, integer, ptr, uint],
        'mbedtls_gcm_starts': [ptr, integer, ptr, size], 'mbedtls_gcm_update_ad': [ptr, ptr, size],
        'mbedtls_gcm_update': [ptr, ptr, size, ptr, size, ptr],
        'mbedtls_gcm_finish': [ptr, ptr, size, ptr, ptr, size],
        'mbedtls_gcm_crypt_and_tag': [ptr, integer, size, ptr, size, ptr, size, ptr, ptr, size, ptr],
        'mbedtls_gcm_auth_decrypt': [ptr, size, ptr, size, ptr, size, ptr, size, ptr, ptr],
        'mbedtls_gcm_free': [ptr],
    }
    for name, arguments in signatures.items():
        function = getattr(lib, name)
        function.argtypes = arguments
        function.restype = None if name.endswith(('_init', '_free')) else integer
    return lib


def buffer(value):
    return c.create_string_buffer(value, max(1, len(value)))


def exercise(lib, seed):
    rng = random.Random(seed)
    random_bytes = lambda n: bytes(rng.getrandbits(8) for _ in range(n))
    for index in range(80):
        key = random_bytes([16, 24, 32][index % 3])
        iv = random_bytes([8, 12, 16][index % 3])
        aad = random_bytes([0, 1, 15, 16, 17, 41][index % 6])
        plain = random_bytes([0, 1, 15, 16, 17, 255, 1200, 4097][index % 8])
        expected = AESGCM(key).encrypt(iv, plain, aad)
        k, nonce, associated, data = map(buffer, (key, iv, aad, plain))
        output, tag, ctx = c.create_string_buffer(max(1, len(plain))), c.create_string_buffer(16), Context()
        lib.mbedtls_gcm_init(c.byref(ctx))
        assert lib.mbedtls_gcm_setkey(c.byref(ctx), 2, k, len(key) * 8) == 0
        assert lib.mbedtls_gcm_crypt_and_tag(c.byref(ctx), 1, len(plain), nonce, len(iv), associated,
                                             len(aad), data, output, 16, tag) == 0
        assert output.raw[:len(plain)] + tag.raw == expected
        assert lib.mbedtls_gcm_starts(c.byref(ctx), 1, nonce, len(iv)) == 0
        split = len(aad) // 2
        assert lib.mbedtls_gcm_update_ad(c.byref(ctx), buffer(aad[:split]), split) == 0
        assert lib.mbedtls_gcm_update_ad(c.byref(ctx), buffer(aad[split:]), len(aad) - split) == 0
        combined, offset = b'', 0
        for count in (1, 15, 31, 63, len(plain)):
            part = plain[offset:offset + count]
            if not part:
                continue
            dst, used = c.create_string_buffer(len(part) + 16), c.c_size_t()
            assert lib.mbedtls_gcm_update(c.byref(ctx), buffer(part), len(part), dst, len(dst), c.byref(used)) == 0
            combined += dst.raw[:used.value]
            offset += len(part)
        used, dst, stream_tag = c.c_size_t(), c.create_string_buffer(16), c.create_string_buffer(16)
        assert lib.mbedtls_gcm_finish(c.byref(ctx), dst, len(dst), c.byref(used), stream_tag, 16) == 0
        assert combined + dst.raw[:used.value] + stream_tag.raw == expected
        ciphertext, auth = buffer(expected[:-16]), buffer(expected[-16:])
        assert lib.mbedtls_gcm_auth_decrypt(c.byref(ctx), len(plain), nonce, len(iv), associated,
                                            len(aad), auth, 16, ciphertext, output) == 0
        assert output.raw[:len(plain)] == plain
        invalid = bytearray(expected[-16:]); invalid[0] ^= 1
        assert lib.mbedtls_gcm_auth_decrypt(c.byref(ctx), len(plain), nonce, len(iv), associated,
                                            len(aad), buffer(bytes(invalid)), 16, ciphertext, output) == -0x12
        assert output.raw[:len(plain)] == bytes(len(plain))
        # Re-key the same context; a previous key must never survive the update.
        new_key = random_bytes(len(key)); new_expected = AESGCM(new_key).encrypt(iv, plain, aad)
        assert lib.mbedtls_gcm_setkey(c.byref(ctx), 2, buffer(new_key), len(new_key) * 8) == 0
        assert lib.mbedtls_gcm_crypt_and_tag(c.byref(ctx), 1, len(plain), nonce, len(iv), associated,
                                             len(aad), data, output, 16, tag) == 0
        assert output.raw[:len(plain)] + tag.raw == new_expected
        lib.mbedtls_gcm_free(c.byref(ctx))
        assert ctx.backend is None
    return 80


def exercise_camellia(lib):
    source = UPSTREAM / 'tests' / 'suites' / 'test_suite_gcm.camellia.data'
    checked = 0
    for row in csv.reader(source.read_text().splitlines(), delimiter=':'):
        if not row or row[0] != 'gcm_encrypt_and_tag' or row[-1] != '0':
            continue
        _, cipher, key, plain, iv, aad, ciphertext, tag_bits, tag, _ = row
        if cipher != 'MBEDTLS_CIPHER_ID_CAMELLIA':
            continue
        key, plain, iv, aad, ciphertext, tag = map(bytes.fromhex, (key, plain, iv, aad, ciphertext, tag))
        ctx, output, actual_tag = Context(), c.create_string_buffer(max(1, len(plain))), c.create_string_buffer(len(tag))
        lib.mbedtls_gcm_init(c.byref(ctx))
        assert lib.mbedtls_gcm_setkey(c.byref(ctx), 5, buffer(key), len(key) * 8) == 0
        assert lib.mbedtls_gcm_crypt_and_tag(c.byref(ctx), 1, len(plain), buffer(iv), len(iv), buffer(aad),
                                             len(aad), buffer(plain), output, int(tag_bits) // 8, actual_tag) == 0
        assert output.raw[:len(plain)] == ciphertext and actual_tag.raw == tag
        lib.mbedtls_gcm_free(c.byref(ctx))
        checked += 1
    assert checked > 0
    print(f'PASS: {checked} upstream Camellia-GCM vectors')


def exercise_allocation_failure(lib):
    calloc_type = c.CFUNCTYPE(c.c_void_p, c.c_size_t, c.c_size_t)
    free_type = c.CFUNCTYPE(None, c.c_void_p)
    attempts = []
    calloc = calloc_type(lambda count, size: attempts.append((count, size)) or None)
    free = free_type(lambda ptr: None)
    lib.mbedtls_platform_set_calloc_free.argtypes = [calloc_type, free_type]
    lib.mbedtls_platform_set_calloc_free.restype = c.c_int
    assert lib.mbedtls_platform_set_calloc_free(calloc, free) == 0
    ctx = Context()
    lib.mbedtls_gcm_init(c.byref(ctx))
    assert lib.mbedtls_gcm_setkey(c.byref(ctx), 2, buffer(bytes(16)), 128) == -0x6180
    assert len(attempts) == 1 and ctx.backend is None
    lib.mbedtls_gcm_free(c.byref(ctx))
    print('PASS: allocator failure returns CIPHER_ALLOC_FAILED and leaves no context')


def main():
    with tempfile.TemporaryDirectory(prefix='mbedtls-gcm-alt-test-') as temp:
        for large in (False, True):
            directory = Path(temp) / ('large' if large else 'small')
            directory.mkdir()
            lib = setup_api(build(directory, large))
            with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
                total = sum(pool.map(lambda seed: exercise(lib, seed), (1, 2, 3, 4)))
            print(f'PASS: {total} AES-GCM cases, {"large" if large else "small"} GHASH table')
        directory = Path(temp) / 'camellia'
        directory.mkdir()
        exercise_camellia(setup_api(build(directory, True, camellia=True)))
        directory = Path(temp) / 'allocation_failure'
        directory.mkdir()
        exercise_allocation_failure(setup_api(build(directory, True, platform_memory=True)))


if __name__ == '__main__':
    main()
