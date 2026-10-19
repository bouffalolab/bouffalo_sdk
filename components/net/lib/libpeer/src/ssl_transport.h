#ifndef SSL_TRANSPORT_H_
#define SSL_TRANSPORT_H_

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <stdint.h>

#include "socket.h"

typedef struct NetworkContext NetworkContext_t;

struct NetworkContext {
  TcpSocket tcp_socket;
  mbedtls_ssl_context ssl;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_config conf;
  mbedtls_x509_crt cacert;
};

int ssl_transport_connect(NetworkContext_t* net_ctx,
                          const char* host,
                          uint16_t port,
                          const char* cacert);

void ssl_transport_disconnect(NetworkContext_t* net_ctx);

int32_t ssl_transport_recv(NetworkContext_t* net_ctx, void* buf, size_t len);

int32_t ssl_transport_send(NetworkContext_t* net_ctx, const void* buf, size_t len);

#endif  // SSL_TRANSPORT_H_
