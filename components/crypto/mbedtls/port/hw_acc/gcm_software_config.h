#ifndef GCM_SOFTWARE_CONFIG_H
#define GCM_SOFTWARE_CONFIG_H

/* Private backend: only GCM and its software block ciphers are enabled. */
#define MBEDTLS_AES_C
#define MBEDTLS_GCM_C
/* ROM tables avoid lazy mutable-table initialization across contexts. */
#define MBEDTLS_AES_ROM_TABLES
#if defined(CONFIG_MBEDTLS_AES_FEWER_TABLES)
#define MBEDTLS_AES_FEWER_TABLES
#endif
#if defined(CONFIG_MBEDTLS_GCM_SOFTWARE_LARGE_TABLE)
#define MBEDTLS_GCM_LARGE_TABLE
#endif
#if defined(CONFIG_MBEDTLS_CIPHER_CAMELLIA_ENABLED)
#define MBEDTLS_CAMELLIA_C
#endif
#endif
