#ifndef LIBPEER_MBEDTLS_CONFIG_H
#define LIBPEER_MBEDTLS_CONFIG_H

/* libpeer generates a local certificate and negotiates the use_srtp extension. */
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_SSL_DTLS_SRTP

/* The SDK generic config defines COMPAT even when Connection ID is disabled. */
#if !defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
#undef MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT
#endif

#endif
