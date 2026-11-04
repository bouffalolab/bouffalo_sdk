# Software GCM with hardware AES

`CONFIG_MBEDTLS_GCM_SOFTWARE_AES` selects the Mbed TLS v3 `MBEDTLS_GCM_ALT`
interface on non-PSA builds with hardware AES enabled. It has no chip-name
restriction. The SDK compiles the checked-out, unmodified
upstream software AES, GCM and block-cipher sources as separate objects with a
private namespace. It does not include upstream `.c` files in the software GCM
adapter or intercept the upstream cipher implementation. Ordinary AES calls
continue through `MBEDTLS_AES_ALT`; SHA and PKA have independent options.

The public GCM context owns one dynamically allocated private upstream context.
Freeing it calls the upstream GCM cleanup and releases the allocation. The AES
hardware context is unchanged. Enable a full rebuild when switching the option:
the public GCM context layout changes. GCM key material must be available to
software, and software lookup tables require a product-specific side-channel
assessment. The backend uses ROM AES tables to avoid concurrent lazy table
initialization.

`CONFIG_MBEDTLS_GCM_SOFTWARE_LARGE_TABLE` defaults to enabled with this option:
it increases the GHASH table to approximately 4 KiB per context for speed.
Disable it to use the upstream smaller table. Camellia is supported when enabled
through SDK Kconfig. Configurations enabling ARIA, or enabling Camellia only via
custom user headers, are rejected at compile time rather than silently changing
cipher support. PSA, Mbed TLS v2 and hardware-only GCM keys are not supported by
this optional backend. The experimental hardware GCM prefetch option remains a
separate mutually exclusive implementation. A successful build does not
establish correctness or performance on a board; validate each target.

Run `python3 components/crypto/mbedtls/tests/test_gcm_software_alt_host.py` from
the SDK root. It requires a host C compiler and Python `cryptography`, compares
AES-GCM with an independent OpenSSL-backed implementation for both GHASH table
sizes, exercises concurrent contexts and rekeying, and runs upstream Camellia
vectors and verifies allocation failure. In `local_mjpeg_stream`,
`CONFIG_LOCAL_MJPEG_GCM_HW_TEST` adds `gcm_verify`, `gcm_concurrent` and
`gcm_bench`; `CONFIG_MBEDTLS_SELF_TEST` additionally enables `crypto_selftest`.
Do not enable test CLI commands in a production firmware.
