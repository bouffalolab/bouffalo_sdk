/*
 * Copyright (c) 2025 Bouffalolab
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include "src/headers/mlog.h"
#include "https_wrapper.h"

#include <lwip/errno.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include "FreeRTOS.h"
#include "task.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/debug.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

/* Certificate bundle support */
#ifdef CONFIG_X509_CERTIFICATE_BUNDLE
#include "bl_crt_bundle.h"
#endif

#if defined(MBEDTLS_THREADING_ALT)
#include "mbedtls/threading.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif

#define MBEDTLS_DEBUG_LEVEL 3
#define CONNECT_TCP_DNS_RETRY 3
#define CONNECT_TCP_DNS_RETRY_DELAY_MS 500

#if defined(MBEDTLS_THREADING_ALT)
static bool s_mbedtls_threading_alt_ready = false;
#endif

typedef struct {
    mbedtls_net_context net;
    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt owncert;
    mbedtls_ssl_config conf;
    mbedtls_ssl_context ssl;
    mbedtls_pk_context pkey;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    int ssl_inited;
} ssl_param_t;

static int ssl_data_send(void *ssl, const unsigned char *buffer, size_t length);
static int ssl_data_recv(void *ssl, unsigned char *buffer, size_t length);

static void tls_err_to_string(int err, char *buf, size_t buf_len)
{
    if (!buf || buf_len == 0) {
        return;
    }
#if defined(MBEDTLS_ERROR_C)
    mbedtls_strerror(err, buf, buf_len);
#else
    snprintf(buf, buf_len, "err=%d", err);
#endif
}

#if defined(MBEDTLS_THREADING_ALT)
static void ssl_mutex_init(mbedtls_threading_mutex_t *mutex)
{
    mutex->is_valid = aos_mutex_new(&mutex->mutex) == 0;
}

static void ssl_mutex_free(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
        return;

    aos_mutex_free(&mutex->mutex);
    mutex->is_valid = 0;
}

static int ssl_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
        return( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );

    if (aos_mutex_lock( &mutex->mutex, AOS_WAIT_FOREVER) != 0 )
        return(MBEDTLS_ERR_THREADING_MUTEX_ERROR);

    return 0;
}

static int ssl_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
    if( mutex == NULL || ! mutex->is_valid )
        return( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );

    if( aos_mutex_unlock( &mutex->mutex ) != 0 )
        return( MBEDTLS_ERR_THREADING_MUTEX_ERROR );

    return( 0 );
}
#endif

#if defined(MBEDTLS_DEBUG_C)
static void ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    (void)ctx;
    (void) level;

    LOG_DBG("%s\r", str);
    return;
}
#endif

static int net_would_block(const mbedtls_net_context *ctx)
{
    int err = errno;

    // Never return 'WOULD BLOCK' on a non-blocking socket
    if( ( fcntl( ctx->fd, F_GETFL, 0) & O_NONBLOCK ) != O_NONBLOCK ) {
        errno = err;
        return( 0 );
    }

    switch( errno = err )
    {
#if defined EAGAIN
        case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#endif
            return( 1 );
    }
    return( 0 );
}

static int ssl_data_recv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        LOG_DBG("[MBEDTLS] invalid socket fd\r\n");
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    ret = (int)recv(fd, buf, len, 0);
    if (ret < 0) {
        if (errno == EPIPE || errno == ECONNRESET) {
            LOG_DBG("[MBEDTLS] net reset - errno: %d\r\n", errno);
            return (MBEDTLS_ERR_NET_CONN_RESET);
        }

        if (errno == EINTR) {
            return (MBEDTLS_ERR_SSL_WANT_READ);
        }

        if( net_would_block(ctx) != 0 ) {
            return (MBEDTLS_ERR_SSL_WANT_READ);
        }
        LOG_DBG("[MBEDTLS] ssl recv failed - errno: %d\r\n", errno);
        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }
    return ret;
}

static int ssl_data_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    ret = (int)send(fd, buf, len, 0);
    if (ret < 0) {
        LOG_DBG("[MBEDTLS] ssl send failed - errno: %d\r\n", errno);
        return (MBEDTLS_ERR_NET_SEND_FAILED);
    }
    return ret;
}

static void *ssl_connect(int fd, const http_wrapper_ssl_param_t *param)
{
    int ret;
    uint32_t result;
    ssl_param_t *ssl_param = NULL;
    static const char *drbg_pers = "https_wrapper";

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
#endif

#if defined(MBEDTLS_THREADING_ALT)
    if (!s_mbedtls_threading_alt_ready) {
        mbedtls_threading_set_alt(ssl_mutex_init,
            ssl_mutex_free,
            ssl_mutex_lock,
            ssl_mutex_unlock);
        s_mbedtls_threading_alt_ready = true;
    }
#endif

    ssl_param = (ssl_param_t *)malloc(sizeof(ssl_param_t));
    if (NULL == ssl_param) {
        LOG_DBG("[MBEDTLS] ssl connect: malloc(%d) fail\r\n", sizeof(ssl_param_t));
        return NULL;
    }

    memset(ssl_param, 0, sizeof(ssl_param_t));
    ssl_param->ssl_inited = 1;

    /*
    * Initialize the connection
    */
    ssl_param->net.fd = fd;
    mbedtls_ssl_config_init(&ssl_param->conf);
    mbedtls_ssl_init(&ssl_param->ssl);
    mbedtls_entropy_init(&ssl_param->entropy);
    mbedtls_ctr_drbg_init(&ssl_param->ctr_drbg);
    if (param->ca_cert && param->ca_cert_len > 0)
        mbedtls_x509_crt_init(&ssl_param->ca_cert);
    if(param->own_cert && param->own_cert_len > 0)
        mbedtls_x509_crt_init(&ssl_param->owncert);
    if(param->private_cert && param->private_cert_len > 0)
        mbedtls_pk_init(&ssl_param->pkey);

    ret = mbedtls_ctr_drbg_seed(&ssl_param->ctr_drbg,
                                mbedtls_entropy_func,
                                &ssl_param->entropy,
                                (const unsigned char *)drbg_pers,
                                strlen(drbg_pers));
    if (ret != 0) {
        char err_buf[96];
        tls_err_to_string(ret, err_buf, sizeof(err_buf));
        LOG_ERR("[MBEDTLS] ssl connect: ctr_drbg_seed failed ret=0x%x (%s)\r\n",
                -ret, err_buf);
        goto ERROR;
    }

    /*
    * Initialize certificates
    */
    LOG_DBG("[MBEDTLS] Loading the CA root certificate ... \r\n");
    if (param->ca_cert && param->ca_cert_len > 0)  {
        LOG_DBG("[MBEDTLS] Loading the rsa\r\n");
        ret = mbedtls_x509_crt_parse(&ssl_param->ca_cert, (unsigned char *)param->ca_cert, (size_t)param->ca_cert_len);
        if (ret < 0) {
            LOG_DBG("[MBEDTLS] ssl connect: root parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    if (param->own_cert && param->own_cert_len > 0) {
        ret = mbedtls_x509_crt_parse(&ssl_param->owncert, (unsigned char *)param->own_cert, (size_t)param->own_cert_len);
        if (ret < 0) {
            LOG_DBG("[MBEDTLS] ssl connect: x509 parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    if (param->private_cert && param->private_cert_len > 0) {
#ifdef CONFIG_MBEDTLS_V2
        ret = mbedtls_pk_parse_key(&ssl_param->pkey, (unsigned char *)param->private_cert, param->private_cert_len, NULL, 0);
#else
        ret = mbedtls_pk_parse_key(&ssl_param->pkey, (unsigned char *)param->private_cert, param->private_cert_len, NULL, 0, NULL, NULL);
#endif
        if (ret != 0) {
            LOG_DBG("[MBEDTLS] ssl connect: x509 parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    /*
    * setup stuff
    */
    LOG_DBG("[MBEDTLS] Setting up the SSL/TLS structure ... \r\n");
    ret = mbedtls_ssl_config_defaults(&ssl_param->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        LOG_ERR("[MBEDTLS] ssl connect: set ssl config failed - %d\r\n", ret);
        goto ERROR;
    }

    if (param->ca_cert && param->ca_cert_len > 0) {
        mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&ssl_param->conf, &ssl_param->ca_cert, NULL);
    } else {
#ifdef CONFIG_X509_CERTIFICATE_BUNDLE
        /* Use certificate bundle from firmware */
        LOG_DBG("[MBEDTLS] Using certificate bundle from firmware\r\n");
        if (bl_crt_bundle_attach(&ssl_param->conf) == 0) {
            mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
            LOG_DBG("[MBEDTLS] Certificate bundle attached successfully\r\n");
        } else {
            LOG_DBG("[MBEDTLS] Failed to attach certificate bundle, falling back to no verification\r\n");
            mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_NONE);
        }
#else
        mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_NONE);
#endif
    }

    if (param->own_cert && param->own_cert_len > 0 && param->private_cert && param->private_cert_len > 0) {
        mbedtls_ssl_conf_own_cert(&ssl_param->conf, &ssl_param->owncert, &ssl_param->pkey);
    }

    mbedtls_ssl_conf_rng(&ssl_param->conf, mbedtls_ctr_drbg_random, &ssl_param->ctr_drbg);
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(&ssl_param->conf, ssl_debug, NULL);
#endif

    if (param->alpn_num && param->alpn) {
        mbedtls_ssl_conf_alpn_protocols(&ssl_param->conf, param->alpn);
    }

    if (param->psk_len && param->psk && param->pskhint_len && param->pskhint) {
        mbedtls_ssl_conf_psk(&ssl_param->conf,
                             (const unsigned char *)param->psk,
                             param->psk_len,
                             (const unsigned char *)param->pskhint,
                             param->pskhint_len);
    }

    ret = mbedtls_ssl_setup(&ssl_param->ssl, &ssl_param->conf);
    if (ret != 0) {
        LOG_DBG("[MBEDTLS] ssl connect: mbedtls_ssl_setup returned - %d\r\n", ret);
        goto ERROR;
    }

    if (param->sni && param->sni[0] != '\0') {
        ret = mbedtls_ssl_set_hostname(&ssl_param->ssl, param->sni);
    } else {
        ret = mbedtls_ssl_set_hostname(&ssl_param->ssl, NULL);
    }
    if (ret != 0) {
        char err_buf[96];
        tls_err_to_string(ret, err_buf, sizeof(err_buf));
        LOG_ERR("[MBEDTLS] ssl connect: set hostname failed ret=0x%x (%s), sni=%s\r\n",
                -ret, err_buf, param->sni);
        goto ERROR;
    }
    mbedtls_ssl_set_bio(&ssl_param->ssl, &ssl_param->net, ssl_data_send, ssl_data_recv, NULL);

    /*
    * handshake
    */
    LOG_DBG("[MBEDTLS] Performing the SSL/TLS handshake ... \r\n");
    while ((ret = mbedtls_ssl_handshake(&ssl_param->ssl)) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            char err_buf[96];
            tls_err_to_string(ret, err_buf, sizeof(err_buf));
            LOG_ERR("[MBEDTLS] ssl connect: mbedtls_ssl_handshake returned -0x%x (%s), sni=%s\r\n",
                    -ret, err_buf, param->sni ? param->sni : "(null)");
            goto ERROR;
        }
    }

    /*
    * verify the server certificate
    */
    LOG_DBG("[MBEDTLS] ...... Verifying peer X.509 certificate ... \r\n");
    result = mbedtls_ssl_get_verify_result(&ssl_param->ssl);
    if (result != 0) {
        LOG_ERR("[MBEDTLS] ssl connect: verify result not confirmed - %d\r\n", result);
        goto ERROR;
    }

    LOG_DBG("[MBEDTLS] ssl connect ok\r\n");
    return (void *)ssl_param;

ERROR:
    if (ssl_param != NULL) {
        if (param->ca_cert && param->ca_cert_len > 0)
            mbedtls_x509_crt_free(&ssl_param->ca_cert);
        if(param->own_cert && param->own_cert_len > 0)
            mbedtls_x509_crt_free(&ssl_param->owncert);
        if(param->private_cert && param->private_cert_len > 0)
            mbedtls_pk_free(&ssl_param->pkey);
        mbedtls_ssl_free(&ssl_param->ssl);
        mbedtls_ssl_config_free(&ssl_param->conf);
        mbedtls_ctr_drbg_free(&ssl_param->ctr_drbg);
        mbedtls_entropy_free(&ssl_param->entropy);

        free(ssl_param);
        ssl_param = NULL;
    }
    return NULL;
}

static int mbedtls_ssl_send(void *ssl, const unsigned char *buffer, size_t length)
{
    int ret;
    int total_len = 0;
    int retry = 0;
    ssl_param_t *ssl_param;

    if (ssl == NULL || buffer == NULL || length <= 0) {
        LOG_DBG("[MBEDTLS] mbedtls_send: invalid input args\r\n");
        return -1;
    }

    LOG_DBG("[MBEDTLS] ...... > Send to server\r\n");

    ssl_param = (ssl_param_t *)ssl;
    if (!ssl_param->ssl_inited) {
        LOG_DBG("[MBEDTLS] mbedtls_send: uninitialized\r\n");
        return -1;
    }

    do {
        ret = mbedtls_ssl_write(&ssl_param->ssl, (const unsigned char *)buffer, (size_t)(length - total_len));
        if (ret > 0) {
            total_len += ret;
            buffer += ret;
        }
        else if (ret == 0) {
            /* EOF */
            break;
        }
        else {
            if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                retry ++;
                vTaskDelay(10);
                continue;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                /*
                * indicate that write already complete,
                * if call read again, it will return 0(EOF))
                */
                break;
            }

            LOG_DBG("[MBEDTLS] mbedtls_send: mbedtls_ssl_write failed - 0x%x\r\n", -ret);
            return -1;
        }
    } while (total_len < length && retry < 10);

    LOG_DBG("[MBEDTLS] %d bytes sent retry %d\r\n", ret, retry);
    return ret < 0 ? -1 : total_len;
}

static int mbedtls_ssl_recv(void *ssl, unsigned char *buffer, size_t length)
{
    int ret;
    int total_len = 0;
    ssl_param_t *ssl_param;

    if (ssl == NULL || buffer == NULL || length <= 0) {
        LOG_DBG("[MBEDTLS] mbedtls_recv: invalid input args\r\n");
        return -1;
    }
    else {
        ssl_param = (ssl_param_t *)ssl;
        if (!ssl_param->ssl_inited) {
            LOG_DBG("[MBEDTLS] mbedtls_recv: uninitialized\r\n");
            return -1;
        }
    }

    LOG_DBG("[MBEDTLS] ...... < Read from server\r\n");

    mbedtls_ssl_conf_read_timeout(&ssl_param->conf, 10000);
    do {
        ret = mbedtls_ssl_read(&ssl_param->ssl, (unsigned char *)buffer, (size_t)length);
        if (ret > 0) {
            total_len = ret;
            break;
        }
        else if (ret == 0) {
            /* EOF */
            return 0;
        }
        else {
            if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
                return -EAGAIN;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                /*
                * indicate that read already complete,
                * if call read again, it will return 0(EOF))
                */
                break;
            }

            LOG_DBG("[MBEDTLS] mbedtls_recv: mbedtls_ssl_read failed - 0x%x\r\n", -ret);
            return -1;
        }
    } while (0);

    LOG_DBG("[MBEDTLS] %d bytes read\r\n", total_len);
    return total_len;
}

static int ssl_close(void *ssl)
{
    ssl_param_t *ssl_param;

    if (ssl == NULL) {
        return 0;
    }

    ssl_param = (ssl_param_t *)ssl;
    if (!ssl_param->ssl_inited) {
        LOG_DBG("[MBEDTLS] mbedtls_close: uninitialized\r\n");
        return -1;
    }

    mbedtls_ssl_close_notify(&ssl_param->ssl);

#ifdef CONFIG_X509_CERTIFICATE_BUNDLE
    /* Detach certificate bundle if it was used
     * Note: We check if ca_cert was initialized to determine if bundle was used
     * This is because bundle is used when ca_cert is NOT provided by user
     */
    mbedtls_x509_crt_free(&ssl_param->owncert);
    mbedtls_pk_free(&ssl_param->pkey);
    mbedtls_ssl_free(&ssl_param->ssl);
    mbedtls_ssl_config_free(&ssl_param->conf);
    mbedtls_ctr_drbg_free(&ssl_param->ctr_drbg);
    mbedtls_entropy_free(&ssl_param->entropy);

    /* Detach certificate bundle callback */
    bl_crt_bundle_detach(&ssl_param->conf);
#else
    mbedtls_x509_crt_free(&ssl_param->ca_cert);
    mbedtls_x509_crt_free(&ssl_param->owncert);
    mbedtls_pk_free(&ssl_param->pkey);
    mbedtls_ssl_free(&ssl_param->ssl);
    mbedtls_ssl_config_free(&ssl_param->conf);
    mbedtls_ctr_drbg_free(&ssl_param->ctr_drbg);
    mbedtls_entropy_free(&ssl_param->entropy);
#endif

    free(ssl_param);
    return 0;
}


static int connect_tcp(const char *host_name, uint16_t port)
{
    int sockfd = -1;
    struct addrinfo hints = {0};
    char port_name[10];

    hints.ai_family = AF_INET; /* Prefer IPv4 to avoid IPv6 stack incompatibilities */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int rv = -1;
    int dns_try;
    struct addrinfo *p;
    struct addrinfo *servinfo = NULL;

    snprintf(port_name, sizeof(port_name), "%d", port);
    /* get address information, with retry for transient DNS failures */
    for (dns_try = 1; dns_try <= CONNECT_TCP_DNS_RETRY; dns_try++) {
        rv = getaddrinfo(host_name, port_name, &hints, &servinfo);
        if (rv == 0 && servinfo) {
            break;
        }
        LOG_ERR("Failed to open socket (getaddrinfo): host=%s port=%s ret=%d try=%d/%d\r\n",
                host_name, port_name, rv, dns_try, CONNECT_TCP_DNS_RETRY);
        if (dns_try < CONNECT_TCP_DNS_RETRY) {
            vTaskDelay(pdMS_TO_TICKS(CONNECT_TCP_DNS_RETRY_DELAY_MS));
        }
    }
    if (rv != 0 || !servinfo) {
        return -1;
    }

    /* open the first possible socket */
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) continue;

        /* connect to server */
        rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if(rv == -1) {
            close(sockfd);
            sockfd = -1;
            continue;
        }
        break;
    }

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1) {
        int iMode = 1;
        ioctlsocket(sockfd, FIONBIO, &iMode);
    } else {
        LOG_ERR("TCP connect failed: host=%s port=%s\r\n", host_name, port_name);
    }

    return sockfd;
}

https_wrapper_handle_t https_wrapper_connect(const char *host_name, uint16_t port, http_wrapper_ssl_param_t *param)
{
    int rv = 0;
    int sockfd;
    mbedtls_net_context net_ctx;
    ssl_param_t *ctx;

    sockfd = connect_tcp(host_name, port);
    if (sockfd < 0) {
        LOG_ERR("TCP connect fail:%d\r\n", sockfd);
        return NULL;
    }

    if (param == NULL) {
        ctx = (ssl_param_t *)calloc(sizeof(ssl_param_t), 1);
        if (NULL == ctx) {
            LOG_ERR("[MBEDTLS] ssl connect: malloc(%d) fail\r\n", sizeof(ssl_param_t));
            return NULL;
        }
        ctx->net.fd = sockfd;
        return (https_wrapper_handle_t)ctx;
    }

    net_ctx.fd = sockfd;
    rv = mbedtls_net_set_block(&net_ctx);
    if (rv != 0) {
        close(sockfd);
        return NULL;
    }

    LOG_DBG("TLS connect host=%s port=%u sni=%s ca=%s alpn_num=%d\r\n",
            host_name,
            port,
            param->sni ? param->sni : "(null)",
            (param->ca_cert && param->ca_cert_len > 0) ? "yes" : "no",
            param->alpn_num);

    ctx = ssl_connect(net_ctx.fd, param);

    if (ctx == NULL) {
        LOG_ERR("ssl_connect handle NULL, fd:%d\r\n", sockfd);
        close(sockfd);
    }

    rv = mbedtls_net_set_nonblock(&net_ctx);
    if (rv != 0) {
        ssl_close(ctx);
        close(sockfd);
    }

    return (https_wrapper_handle_t)ctx;
}

int https_wrapper_send(https_wrapper_handle_t https, const void *data, uint16_t size, int flags)
{
    ssl_param_t *ctx = (ssl_param_t *)https;
    if (ctx->ssl_inited) {
        return mbedtls_ssl_send(ctx, data, size);
    }
    return send(ctx->net.fd, data, size, flags);
}

int https_wrapper_recv(https_wrapper_handle_t https, uint8_t *data, uint32_t size, int flags)
{
    ssl_param_t *ctx = (ssl_param_t *)https;
    if (ctx->ssl_inited) {
        return mbedtls_ssl_recv(ctx, data, size);
    }
    return recv(ctx->net.fd, data, size, flags);
}

int https_wrapper_destroy(https_wrapper_handle_t https)
{
    ssl_param_t *ctx = (ssl_param_t *)https;
    int fd = ctx->net.fd;

    if (ctx->ssl_inited) {
        ssl_close(ctx);
    } else {
        free(ctx);
    }
    close(fd);
    return 0;
}

int https_wrapper_socketfd_get(https_wrapper_handle_t https)
{
    return ((ssl_param_t *)https)->net.fd;
}
