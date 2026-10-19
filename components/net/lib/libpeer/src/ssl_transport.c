#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ssl.h"

#include <sys/select.h>
#include "config.h"
#include "ports.h"
#include "ssl_transport.h"
#include "utils.h"

static int ssl_transport_mbedtls_recv_timeout(void* ctx, unsigned char* buf, size_t len, uint32_t timeout) {
  int ret;
  fd_set read_fds;
  struct timeval tv;
  tv.tv_sec = timeout / 1000;
  tv.tv_usec = (timeout % 1000) * 1000;

  FD_ZERO(&read_fds);
  FD_SET(((TcpSocket*)ctx)->fd, &read_fds);

  ret = select(((TcpSocket*)ctx)->fd + 1, &read_fds, NULL, NULL, &tv);
  if (ret < 0) {
    if (errno == EINTR) {
      return MBEDTLS_ERR_SSL_WANT_READ;
    }
    return -1;
  } else if (ret == 0) {
    return MBEDTLS_ERR_SSL_TIMEOUT;
  } else if (FD_ISSET(((TcpSocket*)ctx)->fd, &read_fds)) {
    return tcp_socket_recv((TcpSocket*)ctx, buf, len);
  }

  return MBEDTLS_ERR_SSL_WANT_READ;
}

static int ssl_transport_mbedlts_send(void* ctx, const uint8_t* buf, size_t len) {
  return tcp_socket_send((TcpSocket*)ctx, buf, len);
}

int ssl_transport_connect(NetworkContext_t* net_ctx,
                          const char* host,
                          uint16_t port,
                          const char* cacert) {
  const char* pers = "ssl_client";
  int ret = -1;
  Address resolved_addr;

  if (!net_ctx || !host || host[0] == '\0' || port == 0) {
    return -1;
  }

  memset(net_ctx, 0, sizeof(*net_ctx));
  net_ctx->tcp_socket.fd = -1;
  mbedtls_ssl_init(&net_ctx->ssl);
  mbedtls_ssl_config_init(&net_ctx->conf);
  mbedtls_x509_crt_init(&net_ctx->cacert);
  mbedtls_ctr_drbg_init(&net_ctx->ctr_drbg);
  mbedtls_entropy_init(&net_ctx->entropy);

  if ((ret = mbedtls_ctr_drbg_seed(&net_ctx->ctr_drbg, mbedtls_entropy_func, &net_ctx->entropy,
                                   (const unsigned char*)pers, strlen(pers))) != 0) {
    LOGE("ssl random seed error: -0x%x", (unsigned int)-ret);
    goto fail;
  }

  if ((ret = mbedtls_ssl_config_defaults(&net_ctx->conf,
                                         MBEDTLS_SSL_IS_CLIENT,
                                         MBEDTLS_SSL_TRANSPORT_STREAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    LOGE("ssl config error: -0x%x", (unsigned int)-ret);
    goto fail;
  }

  if (cacert && cacert[0] != '\0') {
    ret = mbedtls_x509_crt_parse(&net_ctx->cacert,
                                 (const unsigned char*)cacert,
                                 strlen(cacert) + 1);
    if (ret != 0) {
      LOGE("ssl CA parse error: -0x%x", (unsigned int)-ret);
      goto fail;
    }
    mbedtls_ssl_conf_ca_chain(&net_ctx->conf, &net_ctx->cacert, NULL);
    mbedtls_ssl_conf_authmode(&net_ctx->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  } else {
    /* Preserve legacy callers, but make the insecure mode visible. */
    mbedtls_ssl_conf_authmode(&net_ctx->conf, MBEDTLS_SSL_VERIFY_NONE);
    LOGW("TLS peer verification disabled for %s", host);
  }

  mbedtls_ssl_conf_rng(&net_ctx->conf, mbedtls_ctr_drbg_random, &net_ctx->ctr_drbg);

  if ((ret = mbedtls_ssl_setup(&net_ctx->ssl, &net_ctx->conf)) != 0) {
    LOGE("ssl setup error: -0x%x", (unsigned int)-ret);
    goto fail;
  }

  if ((ret = mbedtls_ssl_set_hostname(&net_ctx->ssl, host)) != 0) {
    LOGE("ssl set hostname error: -0x%x", (unsigned int)-ret);
    goto fail;
  }

  memset(&resolved_addr, 0, sizeof(resolved_addr));
  if (ports_resolve_addr(host, &resolved_addr) < 0) {
    LOGE("Failed to resolve TLS host %s", host);
    goto fail;
  }
  addr_set_port(&resolved_addr, port);
  if (tcp_socket_open(&net_ctx->tcp_socket, resolved_addr.family) < 0) {
    goto fail;
  }
  ret = tcp_socket_connect(&net_ctx->tcp_socket, &resolved_addr);
  if (ret < 0) {
    goto fail;
  }

  mbedtls_ssl_conf_read_timeout(&net_ctx->conf, CONFIG_TLS_READ_TIMEOUT);
  mbedtls_ssl_set_bio(&net_ctx->ssl, &net_ctx->tcp_socket,
                      ssl_transport_mbedlts_send, NULL, ssl_transport_mbedtls_recv_timeout);

  LOGI("start to handshake");

  while ((ret = mbedtls_ssl_handshake(&net_ctx->ssl)) != 0) {
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
      LOGE("ssl handshake error: -0x%x", (unsigned int)-ret);
      goto fail;
    }
  }

  LOGI("handshake success");
  return 0;

fail:
  ssl_transport_disconnect(net_ctx);
  return -1;
}

void ssl_transport_disconnect(NetworkContext_t* net_ctx) {
  if (!net_ctx) {
    return;
  }

  if (net_ctx->tcp_socket.fd >= 0) {
    tcp_socket_close(&net_ctx->tcp_socket);
    net_ctx->tcp_socket.fd = -1;
  }
  mbedtls_ssl_config_free(&net_ctx->conf);
  mbedtls_x509_crt_free(&net_ctx->cacert);
  mbedtls_ctr_drbg_free(&net_ctx->ctr_drbg);
  mbedtls_entropy_free(&net_ctx->entropy);
  mbedtls_ssl_free(&net_ctx->ssl);
}

int32_t ssl_transport_recv(NetworkContext_t* net_ctx, void* buf, size_t len) {
  int ret;
  ret = mbedtls_ssl_read(&net_ctx->ssl, buf, len);

  return ret;
}

int32_t ssl_transport_send(NetworkContext_t* net_ctx, const void* buf, size_t len) {
  int ret;

  while ((ret = mbedtls_ssl_write(&net_ctx->ssl, buf, len)) <= 0) {
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
      LOGE("ssl write error: -0x%x", (unsigned int)-ret);
      return ret;
    }
  }

  return ret;
}
