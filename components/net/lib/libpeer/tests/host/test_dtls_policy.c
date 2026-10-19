/* Exercise the actual certificate callback, including flags that must survive. */
#include "dtls_srtp.c"
#include <assert.h>

void ports_sleep_ms(int milliseconds) { usleep(milliseconds * 1000); }

int main(void)
{
    DtlsSrtp transport = {0};
    assert(dtls_srtp_init(&transport, DTLS_SRTP_ROLE_CLIENT, NULL) == 0);
    strcpy(transport.remote_fingerprint, transport.local_fingerprint);
    const uint32_t exempt = MBEDTLS_X509_BADCERT_NOT_TRUSTED |
        MBEDTLS_X509_BADCERT_CN_MISMATCH | MBEDTLS_X509_BADCERT_EXPIRED |
        MBEDTLS_X509_BADCERT_FUTURE | MBEDTLS_X509_BADCERT_BAD_MD |
        MBEDTLS_X509_BADCERT_BAD_KEY;
    uint32_t flags = exempt;
    assert(dtls_srtp_cert_verify(&transport, &transport.cert, 0, &flags) == 0);
    assert(!flags && transport.remote_fingerprint_ready);
    flags = UINT32_MAX;
    assert(dtls_srtp_cert_verify(&transport, &transport.cert, 0, &flags) == 0);
    assert(flags == (UINT32_MAX & ~exempt));
    const uint32_t fatal[] = {MBEDTLS_X509_BADCERT_REVOKED,
        MBEDTLS_X509_BADCERT_MISSING, MBEDTLS_X509_BADCERT_OTHER,
        MBEDTLS_X509_BADCERT_KEY_USAGE, MBEDTLS_X509_BADCERT_EXT_KEY_USAGE,
        MBEDTLS_X509_BADCERT_BAD_PK, UINT32_C(0x80000000)};
    for (unsigned i = 0; i < sizeof(fatal) / sizeof(fatal[0]); ++i) {
        flags = fatal[i] | exempt;
        assert(dtls_srtp_cert_verify(&transport, &transport.cert, 0, &flags) == 0);
        assert(flags == fatal[i]); /* REQUIRED makes these handshake failures. */
    }
    flags = exempt;
    transport.remote_fingerprint[0] = '?';
    assert(dtls_srtp_cert_verify(&transport, &transport.cert, 0, &flags) < 0);
    assert(flags == exempt);
    assert(dtls_srtp_cert_verify(NULL, &transport.cert, 0, &flags) < 0);
    dtls_srtp_deinit(&transport);
    puts("DTLS pin verification exempts only documented flags and retains fatal/unknown errors");
    return 0;
}
