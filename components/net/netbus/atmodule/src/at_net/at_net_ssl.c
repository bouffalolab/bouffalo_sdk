/**
  ******************************************************************************
  * @file    at_net_ssl.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <lwip/errno.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

//#include <hosal_rng.h>

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h" 
#else 
#include MBEDTLS_CONFIG_FILE 
#endif

#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/net.h"
#include "at_net_ssl.h"

#define AT_NET_SSL_PRINTF printf

#if defined(MBEDTLS_THREADING_ALT)
#include "mbedtls/threading.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif

#define MBEDTLS_DEBUG_LEVEL 3

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

static int ssl_random(void *prng, unsigned char *output, size_t output_len)
{
    (void)prng;
    bflb_trng_readlen(output, output_len);
    return 0;
}

#if defined(MBEDTLS_DEBUG_C)
static void ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    (void)ctx;
    (void) level;

    AT_NET_SSL_PRINTF("%s\r", str);
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

static int ssl_recv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] invalid socket fd\r\n");
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    ret = (int)read(fd, buf, len);
    if (ret < 0) {
        if (errno == EPIPE || errno == ECONNRESET) {
            AT_NET_SSL_PRINTF("[MBEDTLS] net reset - errno: %d\r\n", errno);
            return (MBEDTLS_ERR_NET_CONN_RESET);
        }

        if (errno == EINTR) {
            return (MBEDTLS_ERR_SSL_WANT_READ);
        }

        if( net_would_block(ctx) != 0 ) {
            return (MBEDTLS_ERR_SSL_WANT_READ);
        }
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl recv failed - errno: %d\r\n", errno);
        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }
    return ret;
}

static int ssl_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    ret = (int)send(fd, buf, len, 0);
    if (ret < 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl send failed - errno: %d\r\n", errno);
        return (MBEDTLS_ERR_NET_SEND_FAILED);
    }
    return ret;
}

void *mbedtls_ssl_connect(int fd, const ssl_conn_param_t *param)
{
    int ret;
    uint32_t result;
    ssl_param_t *ssl_param = NULL;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
#endif

#if defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_set_alt(ssl_mutex_init,
        ssl_mutex_free,
        ssl_mutex_lock,
        ssl_mutex_unlock);
#endif

    ssl_param = (ssl_param_t *)pvPortMalloc(sizeof(ssl_param_t));
    if (NULL == ssl_param) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: malloc(%d) fail\r\n", sizeof(ssl_param_t));
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
    if (param->ca_cert && param->ca_cert_len > 0)
        mbedtls_x509_crt_init(&ssl_param->ca_cert);
    if(param->own_cert && param->own_cert_len > 0)
        mbedtls_x509_crt_init(&ssl_param->owncert);
    if(param->private_cert && param->private_cert_len > 0)
        mbedtls_pk_init(&ssl_param->pkey);

    /*
    * Initialize certificates
    */
    AT_NET_SSL_PRINTF("[MBEDTLS] Loading the CA root certificate ... \r\n");
    if (param->ca_cert && param->ca_cert_len > 0)  {
        AT_NET_SSL_PRINTF("[MBEDTLS] Loading the rsa\r\n");
        ret = mbedtls_x509_crt_parse(&ssl_param->ca_cert, (unsigned char *)param->ca_cert, (size_t)param->ca_cert_len);
        if (ret < 0) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: root parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        } 
    }

    if (param->own_cert && param->own_cert_len > 0) {
        ret = mbedtls_x509_crt_parse(&ssl_param->owncert, (unsigned char *)param->own_cert, (size_t)param->own_cert_len);
        if (ret < 0) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: x509 parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    if (param->private_cert && param->private_cert_len > 0) {
        ret = mbedtls_pk_parse_key(&ssl_param->pkey, (unsigned char *)param->private_cert, param->private_cert_len, NULL, 0);
        if (ret != 0) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: x509 parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    /*
    * setup stuff
    */
    AT_NET_SSL_PRINTF("[MBEDTLS] Setting up the SSL/TLS structure ... \r\n");
    ret = mbedtls_ssl_config_defaults(&ssl_param->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: set ssl config failed - %d\r\n", ret);
        goto ERROR;
    }

    if (param->ca_cert && param->ca_cert_len > 0) {
        mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&ssl_param->conf, &ssl_param->ca_cert, NULL);
    } else {
        mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_NONE);
    }

    if (param->own_cert && param->own_cert_len > 0 && param->private_cert && param->private_cert_len > 0) {
        mbedtls_ssl_conf_own_cert(&ssl_param->conf, &ssl_param->owncert, &ssl_param->pkey);
    }

    mbedtls_ssl_conf_rng(&ssl_param->conf, ssl_random, NULL);
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(&ssl_param->conf, ssl_debug, NULL);
#endif
    
    if (param->alpn_num && param->alpn) {
        mbedtls_ssl_conf_alpn_protocols(&ssl_param->conf, param->alpn);
    }
    
    if (param->psk_len && param->psk && param->pskhint_len && param->pskhint) {
        mbedtls_ssl_conf_psk(&ssl_param->conf, param->psk, param->psk_len, param->pskhint, param->pskhint_len);
    }

    ret = mbedtls_ssl_setup(&ssl_param->ssl, &ssl_param->conf);
    if (ret != 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: mbedtls_ssl_setup returned - %d\r\n", ret);
        goto ERROR;
    }

    if (param->sni) {
        mbedtls_ssl_set_hostname(&ssl_param->ssl, param->sni);
    }
    mbedtls_ssl_set_bio(&ssl_param->ssl, &ssl_param->net, ssl_send, ssl_recv, NULL);

    /*
    * handshake
    */
    AT_NET_SSL_PRINTF("[MBEDTLS] Performing the SSL/TLS handshake ... \r\n");
    while ((ret = mbedtls_ssl_handshake(&ssl_param->ssl)) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: mbedtls_ssl_handshake returned -0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    /*
    * verify the server certificate
    */
    AT_NET_SSL_PRINTF("[MBEDTLS] ...... Verifying peer X.509 certificate ... \r\n");
    result = mbedtls_ssl_get_verify_result(&ssl_param->ssl);
    if (result != 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect: verify result not confirmed - %d\r\n", result);
        goto ERROR;
    }

    AT_NET_SSL_PRINTF("[MBEDTLS] ssl connect ok\r\n");
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

        vPortFree(ssl_param);
        ssl_param = NULL;
    }
    return NULL;
}

void *mbedtls_ssl_accept(int fd, const char *ca_cert, int ca_cert_len, 
					 const char *srv_cert, int srv_cert_len, const char *private_cert, int private_cert_len)
{
    int ret;
    ssl_param_t *ssl_param = NULL;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
#endif

#if defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_set_alt(ssl_mutex_init,
        ssl_mutex_free,
        ssl_mutex_lock,
        ssl_mutex_unlock);
#endif

    ssl_param = (ssl_param_t *)pvPortMalloc(sizeof(ssl_param_t));
    if (NULL == ssl_param) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl server: malloc(%d) fail\r\n", sizeof(ssl_param_t));
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
    if(ca_cert && ca_cert_len > 0)
        mbedtls_x509_crt_init(&ssl_param->ca_cert);
    if(srv_cert && srv_cert_len > 0)
        mbedtls_x509_crt_init(&ssl_param->owncert);
    if(private_cert && private_cert_len > 0)
        mbedtls_pk_init(&ssl_param->pkey);

    /*
    * Initialize certificates
    */
    AT_NET_SSL_PRINTF("[MBEDTLS] Loading the CA root certificate ... \r\n");
    if (ca_cert && ca_cert_len > 0)  {
        AT_NET_SSL_PRINTF("[MBEDTLS] Loading the rsa\r\n");
        ret = mbedtls_x509_crt_parse(&ssl_param->ca_cert, (unsigned char *)ca_cert, (size_t)ca_cert_len);
        if (ret < 0) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl accept: root parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        } 
    }

    if (srv_cert && srv_cert_len > 0) {
        ret = mbedtls_x509_crt_parse(&ssl_param->owncert, (unsigned char *)srv_cert, (size_t)srv_cert_len);
        if (ret < 0) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl accept: x509 parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    if (private_cert && private_cert_len > 0) {
        ret = mbedtls_pk_parse_key(&ssl_param->pkey, (unsigned char *)private_cert, private_cert_len, NULL, 0);
        if (ret != 0) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl accept: x509 parse failed- 0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    /*
    * setup stuff
    */
    AT_NET_SSL_PRINTF("[MBEDTLS] Setting up the SSL/TLS structure ... \r\n");
    ret = mbedtls_ssl_config_defaults(&ssl_param->conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl accept: set ssl config failed - %d\r\n", ret);
        goto ERROR;
    }

    if (ca_cert && ca_cert_len > 0) {
        mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&ssl_param->conf, &ssl_param->ca_cert, NULL);
    } else {
        mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_NONE);
    }
    if (srv_cert && srv_cert_len > 0 && private_cert && private_cert_len > 0)
        mbedtls_ssl_conf_own_cert(&ssl_param->conf, &ssl_param->owncert, &ssl_param->pkey);

    mbedtls_ssl_conf_rng(&ssl_param->conf, ssl_random, NULL);
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(&ssl_param->conf, ssl_debug, NULL);
#endif

    ret = mbedtls_ssl_setup(&ssl_param->ssl, &ssl_param->conf);
    if (ret != 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] ssl accept: mbedtls_ssl_setup returned - %d\r\n", ret);
        goto ERROR;
    }
    mbedtls_ssl_set_bio(&ssl_param->ssl, &ssl_param->net, ssl_send, ssl_recv, NULL);

    /*
    * handshake
    */
    AT_NET_SSL_PRINTF("[MBEDTLS] Performing the SSL/TLS handshake ... \r\n");
    while ((ret = mbedtls_ssl_handshake(&ssl_param->ssl)) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            AT_NET_SSL_PRINTF("[MBEDTLS] ssl accept: mbedtls_ssl_handshake returned -0x%x\r\n", -ret);
            goto ERROR;
        }
    }

    AT_NET_SSL_PRINTF("[MBEDTLS] ssl accept ok\r\n");
    return (void *)ssl_param;

ERROR:
    if (ssl_param != NULL) {
        mbedtls_ssl_close_notify(&ssl_param->ssl);
        if (ca_cert && ca_cert_len > 0) 
            mbedtls_x509_crt_free(&ssl_param->ca_cert);
        if(srv_cert && srv_cert_len > 0)
            mbedtls_x509_crt_free(&ssl_param->owncert);
        if(private_cert && private_cert_len > 0)
            mbedtls_pk_free(&ssl_param->pkey);
        mbedtls_ssl_free(&ssl_param->ssl);
        mbedtls_ssl_config_free(&ssl_param->conf);

        vPortFree(ssl_param);
        ssl_param = NULL;
    }
    return NULL;
}

int mbedtls_ssl_send(void *ssl, const char *buffer, int length)
{
    int ret;
    int total_len = 0;
    int retry = 0;
    ssl_param_t *ssl_param;

    if (ssl == NULL || buffer == NULL || length <= 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] mbedtls_send: invalid input args\r\n");
        return -1;
    }

    AT_NET_SSL_PRINTF("[MBEDTLS] ...... > Send to server\r\n");

    ssl_param = (ssl_param_t *)ssl;
    if (!ssl_param->ssl_inited) {
        AT_NET_SSL_PRINTF("[MBEDTLS] mbedtls_send: uninitialized\r\n");
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

            AT_NET_SSL_PRINTF("[MBEDTLS] mbedtls_send: mbedtls_ssl_write failed - 0x%x\r\n", -ret);
            return -1;
        }
    } while (total_len < length && retry < 10);

    AT_NET_SSL_PRINTF("[MBEDTLS] %d bytes sent retry %d\r\n", ret, retry);
    return ret < 0 ? -1 : total_len;
}

int mbedtls_ssl_recv(void *ssl, char *buffer, int length)
{
    int ret;
    int total_len = 0;
    ssl_param_t *ssl_param;

    if (ssl == NULL || buffer == NULL || length <= 0) {
        AT_NET_SSL_PRINTF("[MBEDTLS] mbedtls_recv: invalid input args\r\n");
        return -1;
    }
    else {
        ssl_param = (ssl_param_t *)ssl;
        if (!ssl_param->ssl_inited) {
            AT_NET_SSL_PRINTF("[MBEDTLS] mbedtls_recv: uninitialized\r\n");
            return -1;
        }
    }

    AT_NET_SSL_PRINTF("[MBEDTLS] ...... < Read from server\r\n");

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

            AT_NET_SSL_PRINTF("[MBEDTLS] mbedtls_recv: mbedtls_ssl_read failed - 0x%x\r\n", -ret);
            return -1;
        }
    } while (0);

    AT_NET_SSL_PRINTF("[MBEDTLS] %d bytes read\r\n", total_len);
    return total_len;
}

int mbedtls_ssl_close(void *ssl)
{
    ssl_param_t *ssl_param;

    if (ssl == NULL) {
        return 0;
    }

    ssl_param = (ssl_param_t *)ssl;
    if (!ssl_param->ssl_inited) {
        AT_NET_SSL_PRINTF("[MBEDTLS] mbedtls_close: uninitialized\r\n");
        return -1;
    }

    mbedtls_ssl_close_notify(&ssl_param->ssl);
    mbedtls_x509_crt_free(&ssl_param->ca_cert);
    mbedtls_x509_crt_free(&ssl_param->owncert);
    mbedtls_pk_free(&ssl_param->pkey);
    mbedtls_ssl_free(&ssl_param->ssl);
    mbedtls_ssl_config_free(&ssl_param->conf);
#if defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_free_alt();
#endif

    vPortFree(ssl_param);
    return 0;
}

