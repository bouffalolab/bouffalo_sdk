/* Host-only transport boundary; production SCTP and buffer code is compiled unchanged. */
#ifndef RT_TEST_LIBPEER_STUBS_H
#define RT_TEST_LIBPEER_STUBS_H
#define DTLS_SRTP_H_
#define CONFIG_USE_USRSCTP 0
#define CONFIG_USE_LWIP 0
#include <arpa/inet.h>
#include <stdint.h>
#include <stddef.h>
typedef struct DtlsSrtp { int unused; } DtlsSrtp;
int dtls_srtp_write(DtlsSrtp *ctx, const uint8_t *data, size_t len);
#endif
