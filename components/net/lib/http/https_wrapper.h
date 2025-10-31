/*
 * Copyright (c) 2025 Bouffalolab
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HTTPS_WRAPPER_H
#define HTTPS_WRAPPER_H

#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    const char *ca_cert;
    int ca_cert_len;
    const char *own_cert;
    int own_cert_len;
    const char *private_cert;
    int private_cert_len;

    const char **alpn;
    int alpn_num;

    const char *psk;
    int psk_len;
    const char *pskhint;
    int pskhint_len;

    char *sni;
} http_wrapper_ssl_param_t;

typedef void *https_wrapper_handle_t;

https_wrapper_handle_t https_wrapper_connect(const char *host_name, uint16_t port, http_wrapper_ssl_param_t *param);

int https_wrapper_destroy(https_wrapper_handle_t https);

int https_wrapper_send(https_wrapper_handle_t https, const void *data, uint16_t size, int flags);

int https_wrapper_recv(https_wrapper_handle_t https, uint8_t *data, uint32_t size, int flags);

int https_wrapper_socketfd_get(https_wrapper_handle_t https);

#endif  /* HTTPS_WRAPPER_H */
