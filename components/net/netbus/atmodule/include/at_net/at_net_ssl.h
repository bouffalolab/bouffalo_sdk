/**
  ******************************************************************************
  * @file    at_net_ssl.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_NET_SSL_H
#define AT_NET_SSL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/config.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/net.h"

typedef struct {
    int ssl_inited;
    mbedtls_net_context net;
    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt owncert;
    mbedtls_ssl_config conf;
    mbedtls_ssl_context ssl;
    mbedtls_pk_context pkey;
} ssl_param_t;

typedef struct {
    char *ca_cert;
    int ca_cert_len;
    char *own_cert;
    int own_cert_len;
    char *private_cert;
    int private_cert_len;

    char **alpn;
    int alpn_num;

    char *psk;
    int psk_len;
    char *pskhint;
    int pskhint_len;

    char *sni;
} ssl_conn_param_t;

void *mbedtls_ssl_accept(int fd, const char *ca_cert, int ca_cert_len, 
					 const char *srv_cert, int srv_cert_len, const char *private_cert, int private_cert_len);
void *mbedtls_ssl_connect(int fd, const ssl_conn_param_t *param);
int mbedtls_ssl_send(void *ssl, const char *buffer, int length);

int mbedtls_ssl_recv(void *ssl, char *buffer, int length);

int mbedtls_ssl_close(void *ssl);

#ifdef __cplusplus
}
#endif

#endif/* AT_NET_SSL_H */

