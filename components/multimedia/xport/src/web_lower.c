/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */
#ifndef CONFIG_CPU_C906

#ifndef EAI_AGAIN
#define EAI_AGAIN 206
#endif

#include "avutil/av_config.h"

#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#else
#include <lwip/netdb.h>
#endif
#include <arpa/inet.h>

// #include "avutil/common.h"
// #include "avutil/web.h"
// #include "avutil/dync_buf.h"
// #include "avutil/socket_rw.h"

#include "xutils/debug.h"
#include "avutil/av_errno.h"
#include "avutil/web.h"

#ifdef CONFIG_USING_TLS
#include "mbedtls/base64.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#endif

#define TAG                   "WEB"
#ifndef __linux__
// #define close                 lwip_close
// #define fcntl                 lwip_fcntl
#endif

extern int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
//extern int web_upper_http_open(wsession_t *session, web_url_t *wurl, int timeout_ms);
//extern void av_errno_set(int err_no);
extern int sock_writen(int fd, const char *buf, size_t count, int timeout_ms);
extern int sock_readn(int fd, char *buf, size_t count, int timeout_ms);

#ifdef CONFIG_USING_TLS
struct mtls_session {
    uint8_t                     init;
    mbedtls_net_context         nctx;
    mbedtls_entropy_context     entropy;
    mbedtls_ctr_drbg_context    ctr_drbg;
    mbedtls_ssl_context         ssl;
    mbedtls_ssl_config          conf;
    mbedtls_x509_crt            cacert;
};
#endif

static void _wsession_init(wsession_t *session)
{
#ifdef CONFIG_USING_TLS
    struct mtls_session *mtls = session->tls;
#endif

    memset(session, 0, sizeof(wsession_t));
    session->fd               = -1;
    session->code             = -1;
    session->method           = WEB_METHOD_UNKNOWN;
    session->hdr_size_max     = WEB_HDR_SIZE_MAX_DEFAULT;
    session->body_size_max    = WEB_BODY_SIZE_MAX_DEFAULT;
    session->redirect_cnt_max = WEB_REDIRECT_CNT_MAX_DEFAULT;
#ifdef CONFIG_USING_TLS
    session->tls              = mtls;
#endif
}

#ifdef CONFIG_USING_TLS
static void _mtls_session_init(struct mtls_session *mtls)
{
    if (mtls && (!mtls->init)) {
        mbedtls_net_init(&mtls->nctx);
        mbedtls_ssl_init(&mtls->ssl);
        mbedtls_ssl_config_init(&mtls->conf);
        mbedtls_x509_crt_init(&mtls->cacert );
        mbedtls_ctr_drbg_init(&mtls->ctr_drbg);
        mbedtls_entropy_init(&mtls->entropy);
        mtls->init = 1;
    }
}

static void _mtls_session_deinit(struct mtls_session *mtls)
{
    if (mtls && mtls->init) {
        mbedtls_net_free(&mtls->nctx);
        mbedtls_x509_crt_free(&mtls->cacert);
        mbedtls_ssl_free(&mtls->ssl);
        mbedtls_ssl_config_free(&mtls->conf);
        mbedtls_ctr_drbg_free(&mtls->ctr_drbg);
        mbedtls_entropy_free(&mtls->entropy);
        mtls->init = 0;
    }
}


static int _mtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int fd = ((mbedtls_net_context*)ctx)->fd;

    return sock_writen(fd, (const char*)buf, len, 3000);
}

static int _mtls_net_reado(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    int fd = ((mbedtls_net_context*)ctx)->fd;

    return sock_readn(fd, (char*)buf, len, timeout);
}

static int _https_open(wsession_t *session, web_url_t *wurl, int timeout_ms)
{
    int rc, fd = -1;
    char port[16];
    struct timeval tv;
    const char *pers          = "mbedtls_pc";
    struct mtls_session *mtls = session->tls;

    _mtls_session_init(mtls);
    rc = mbedtls_ctr_drbg_seed(&mtls->ctr_drbg, mbedtls_entropy_func, &mtls->entropy,
                               (const unsigned char *) pers, strlen(pers));
    CHECK_RET_WITH_GOTO(rc == 0, err);

    snprintf(port, sizeof(port), "%d", wurl->port);
    LOGD(TAG, "tls: connect to server %s ,port is %s", wurl->host, port);

    rc = mbedtls_net_connect(&mtls->nctx, wurl->host, port, MBEDTLS_NET_PROTO_TCP);
    if (rc != 0) {
        LOGE(TAG, "failed ! mbedtls_net_connect returned -0x%x", -rc);
        av_errno_set(AV_ERRNO_CONNECT_FAILD);
        goto err;
    }

    fd         = mtls->nctx.fd;
    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, sizeof(tv));

    LOGD(TAG, "Setting up the SSL/TLS structure...a");
    rc = mbedtls_ssl_config_defaults(&mtls->conf,
                                     MBEDTLS_SSL_IS_CLIENT,
                                     MBEDTLS_SSL_TRANSPORT_STREAM,
                                     MBEDTLS_SSL_PRESET_DEFAULT);
    if (rc != 0) {
        LOGE(TAG, "failed ! mbedtls_ssl_config_defaults returned -0x%x", -rc);
        goto err;
    }

    mbedtls_ssl_conf_read_timeout(&mtls->conf, timeout_ms);
    mbedtls_ssl_conf_authmode(&mtls->conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&mtls->conf, mbedtls_ctr_drbg_random, &mtls->ctr_drbg);

    rc = mbedtls_ssl_setup(&mtls->ssl, &mtls->conf);
    CHECK_RET_WITH_GOTO(rc == 0, err);

    rc = mbedtls_ssl_set_hostname(&mtls->ssl, wurl->host);
    CHECK_RET_WITH_GOTO(rc == 0, err);

    mbedtls_ssl_set_bio(&mtls->ssl, &mtls->nctx, _mtls_net_send, NULL, _mtls_net_reado);

    LOGD(TAG, "now, start handshake...");
    rc = mbedtls_ssl_handshake(&mtls->ssl);
    if (rc != 0) {
        LOGE(TAG, "handshake error, rc = %d", rc);
        av_errno_set(AV_ERRNO_NETWORK_FAILD);
        goto err;
    }

    LOGD(TAG, " ***tls handshake ok***\n    [ Protocol is %s ]\n    [ Ciphersuite is %s ]\n",
         mbedtls_ssl_get_version(&mtls->ssl), mbedtls_ssl_get_ciphersuite(&mtls->ssl));
    rc = mbedtls_ssl_get_record_expansion(&mtls->ssl);
    LOGD(TAG, "    [ Record expansion is %d ]", rc);

    return 0;
err:
    _mtls_session_deinit(mtls);

    return -1;
}
#endif

int web_upper_http_open(wsession_t *session, web_url_t *wurl, int timeout_ms)
{
    struct addrinfo *res = NULL;
    const struct addrinfo hints = {
        .ai_socktype = SOCK_STREAM
    };
    fd_set fds;
    char port[16];
    struct sockaddr sa;
    struct timeval tv;
    int rc, flags, fd = -1, recnt = 0;

    snprintf(port, sizeof(port), "%d", wurl->port);
retry:
    rc = getaddrinfo(wurl->host, port, &hints, &res);
    if (rc != 0) {
        MSP_LOGE("getaddrinfo fail, may be retry again later. rc = %d, host = %s, port = %s\n", rc, wurl->host, port);
        if (rc == EAI_AGAIN && recnt++ < 2) {
            msp_msleep(20);
            goto retry;
        }
        av_errno_set(AV_ERRNO_DNS_FAILD);
        goto err;
    }

    memcpy(&sa, res->ai_addr, res->ai_addrlen);
    fd = socket(sa.sa_family, SOCK_STREAM, IPPROTO_TCP);
    CHECK_RET_WITH_GOTO(fd != -1, err);

    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, sizeof(tv));

    flags = fcntl(fd, F_GETFL, 0);
    rc = fcntl(fd, F_SETFL, O_NONBLOCK);
    CHECK_RET_WITH_GOTO(rc != -1, err);

    while (1) {
        rc = connect(fd, &sa, res->ai_addrlen);
        if (rc == 0) {
            break;
        } else {
            if (errno == EISCONN) {
                break;
            }
            if ((errno != EAGAIN) && (errno != EINPROGRESS)) {
                MSP_LOGE("wsession_open connect fail. errno = %d", errno);
                av_errno_set(AV_ERRNO_CONNECT_FAILD);
                goto err;
            }
        }

        while (1) {
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            rc = select(fd + 1, NULL, &fds, NULL, &tv);
            if (rc == -1) {
                if ((errno == EINTR) || (errno == EAGAIN)) {
                    msp_msleep(20);
                    continue;
                }
                MSP_LOGE("wsession_open select fail. errno = %d", errno);
                av_errno_set(AV_ERRNO_CONNECT_FAILD);
                goto err;
            } else if (rc == 1) {
                goto conn;
            } else if (rc == 0) {
                MSP_LOGE("wsession_open select timeout. errno = %d", errno);
                av_errno_set(AV_ERRNO_CONNECT_FAILD);
                goto err;
            }
        }
    }

conn:
    fcntl(session->fd, F_SETFL, flags);
    freeaddrinfo(res);

    session->fd  = fd;
    session->url = wurl;
    return 0;

err:
    if (res)
        freeaddrinfo(res);
    if (fd >= 0)
        close(fd);
    return -1;
}

int _dict_add_basic_auth(dict_t *c, const char *header, const char *user, const char *passwd)
{
#ifdef CONFIG_USING_TLS
    char *b64;
    char buf[256];
    size_t dlen, olen, slen;

    snprintf(buf, sizeof(buf), "%s:%s", user, passwd);
    slen = strlen(buf);
    dlen = slen * 3 / 2 + 4;
    b64  = msp_zalloc(dlen);
    CHECK_RET_WITH_RET(b64, -1);

    mbedtls_base64_encode((unsigned char*)b64, dlen, &olen, (unsigned char *)buf, slen);

    snprintf(buf, sizeof(buf), "Basic %s", b64);
    msp_free(b64);

    return dict_add(c, header, buf);
#else
    return -1;
#endif
}

/**
 * @brief  create a web session
 * @return NULL on err
 */
wsession_t* wsession_create()
{
#ifdef CONFIG_USING_TLS
    wsession_t *session    = NULL;
    struct mtls_session *mtls = NULL;

    session = msp_zalloc(sizeof(wsession_t));
    mtls    = msp_zalloc(sizeof(struct mtls_session));
    CHECK_RET_WITH_GOTO(session && mtls, err);

    session->tls = mtls;
    _wsession_init(session);

    return session;
err:
    msp_free(mtls);
    msp_free(session);
    return NULL;
#else
    wsession_t *session;

    session = msp_zalloc(sizeof(wsession_t));
    if (session) {
        _wsession_init(session);
    }

    return session;
#endif
}


/**
 * @brief  open the session by url(parse the url, create fd and connect)
 * @param  [in] session
 * @param  [in] url
 * @param  [in] timeout_ms
 * @return 0/-1
 */
int wsession_open(wsession_t *session, const char *url, int timeout_ms)
{
    int rc;
    web_url_t *wurl;

    if (!(session && url && (timeout_ms > 0))) {
        return -1;
    }

    wurl = web_url_new(url);
    CHECK_RET_WITH_GOTO(wurl, err);

    rc = dict_init(&session->hdrs, 10);
    CHECK_RET_WITH_GOTO(rc == 0, err);

#ifdef CONFIG_USING_TLS
    if (URL_IS_HTTPS(wurl))
        rc = _https_open(session, wurl, timeout_ms);
    else
        rc = web_upper_http_open(session, wurl, timeout_ms);
#else
    rc = web_upper_http_open(session, wurl, timeout_ms);
#endif
    CHECK_RET_WITH_GOTO(rc == 0, err);
    session->url = wurl;

    return 0;
err:
    dict_uninit(&session->hdrs);
    web_url_free(wurl);
    return -1;
}

/**
 * @brief  write data to session
 * @param  [in] session
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on error
 */
int wsession_write(wsession_t *session, const char *buf, size_t count, int timeout_ms)
{
    if (!(session && buf && count && (timeout_ms > 0))) {
        return -1;
    }

#ifdef CONFIG_USING_TLS
    int rc;
    struct mtls_session *mtls = session->tls;

    UNUSED(timeout_ms);
    if (URL_IS_HTTPS(session->url))
        rc = mbedtls_ssl_write(&mtls->ssl, (const unsigned char *)buf, count);
    else
        rc = sock_writen(session->fd, buf, count, timeout_ms);

    return rc;
#else
    return sock_writen(session->fd, buf, count, timeout_ms);
#endif
}

/**
 * @brief  read data from session
 * @param  [in] session
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on error
 */
int wsession_read(wsession_t *session, char *buf, size_t count, int timeout_ms)
{
    if (!(session && buf && count && (timeout_ms > 0))) {
        return -1;
    }

#ifdef CONFIG_USING_TLS
    int rc;
    struct mtls_session *mtls = session->tls;

    if (URL_IS_HTTPS(session->url)) {
        mbedtls_ssl_conf_read_timeout(&mtls->conf, timeout_ms);
        count = count >= MBEDTLS_SSL_MAX_CONTENT_LEN ? MBEDTLS_SSL_MAX_CONTENT_LEN / 2 : count;
        rc = mbedtls_ssl_read(&mtls->ssl, (unsigned char *)buf, count);
        //printf("====>>>count = %10d, rc = %10d, %d\n", count, rc, MBEDTLS_SSL_MAX_CONTENT_LEN);
        if (rc < 0) {
            LOGI(TAG, "ssl read may be eof: rc = 0x%x.", -rc);
            /* FIXME: patch for mbedtls. may be read eof normaly */
            rc = (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) ? 0 : -1;
        }
    } else {
        rc = sock_readn(session->fd, buf, count, timeout_ms);
    }

    return rc;
#else
    return sock_readn(session->fd, buf, count, timeout_ms);
#endif
}

/**
 * @brief  close the websession and release resource related
 * @param  [in] session
 * @return 0/-1
 */
int wsession_close(wsession_t *session)
{
    if (session) {
        dict_uninit(&session->hdrs);
        web_url_free(session->url);
        msp_free(session->phrase);

#ifdef CONFIG_USING_TLS
        _mtls_session_deinit((struct mtls_session*)session->tls);
#endif

        if (session->fd >= 0)
            close(session->fd);

        _wsession_init(session);
        return 0;
    }

    return -1;
}

/**
 * @brief  destroy a web session
 * @param  [in] session
 * @return 0/-1
 */
int wsession_destroy(wsession_t *session)
{
    if (session) {
        wsession_close(session);
#ifdef CONFIG_USING_TLS
        msp_free(session->tls);
#endif
        msp_free(session);
        return 0;
    }

    return -1;
}

#endif
#endif