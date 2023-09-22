/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __WEB_H__
#define __WEB_H__

#include "dict.h"
#include "web_url.h"

__BEGIN_DECLS__

#define WEB_USER_AGENT        "CSKY/YOC"
#define WEB_TIMEOUT_DEFAULT   (6*1000)   // ms

typedef struct web_session {
    int                             fd;
    int                             method;

    int                             hdr_size_max;
    int                             body_size_max;
    int                             redirect_cnt_max;

    int                             code;
    char                            *phrase;

    /* used for req/resp */
    dict_t                          hdrs;

    web_url_t                       *url;
    void                            *tls; // for https if needed
#define WSESSION_IS_HTTPS(s) (s && s->url && s->url->is_https)
} wsession_t;

enum {
    WEB_METHOD_UNKNOWN,
    WEB_METHOD_GET,
    WEB_METHOD_POST,
};

/* set/get the max size of HEADER and BODY */
enum {
    WEB_SET_HDR_SIZE_MAX     = 1000,
    WEB_SET_BODY_SIZE_MAX,
    WEB_SET_REDIRECT_CNT_MAX,

    WEB_GET_HDR_SIZE_MAX     = 1100,
    WEB_GET_BODY_SIZE_MAX,
    WEB_GET_REDIRECT_CNT_MAX,
};

#define WEB_HDR_SIZE_MAX_DEFAULT        (1024*2)
#define WEB_BODY_SIZE_MAX_DEFAULT       (1024*8)
#define WEB_REDIRECT_CNT_MAX_DEFAULT    (5)

/**
 * @brief  create a web session
 * @return NULL on err
 */
wsession_t* wsession_create();

/**
 * @brief  open the session by url(parse the url, create fd and connect)
 * @param  [in] session
 * @param  [in] url
 * @param  [in] timeout_ms
 * @return 0/-1
 */
int wsession_open(wsession_t *session, const char *url, int timeout_ms);

/**
 * @brief  write data to session
 * @param  [in] session
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on error
 */
int wsession_write(wsession_t *session, const char *buf, size_t count, int timeout_ms);

/**
 * @brief  read data from session
 * @param  [in] session
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on error
 */
int wsession_read(wsession_t *session, char *buf, size_t count, int timeout_ms);

/**
 * @brief  add header field
 * @param  [in] session
 * @return 0/-1
 */
#define wsession_hdrs_add(session, k, v)      dict_add(&session->hdrs, k, v)

/**
 * @brief  add header field(value is int)
 * @param  [in] session
 * @return 0/-1
 */
#define wsession_hdrs_add_int(session, k, v)      dict_add_int(&session->hdrs, k, v)

/**
 * @brief  send hdr of get/post request
 * @param  [in] session
 * @param  [in] method     : WEB_METHOD_GET/WEB_METHOD_POST
 * @param  [in] timeout_ms
 * @return 0/-1
 */
int wsession_send_hdr(wsession_t *session, int method, int timeout_ms);

/**
 * @brief  read and parse the header of response
 * @param  [in] session
 * @param  [in] timeout_ms
 * @return 0/-1
 */
int wsession_read_resp_hdr(wsession_t *session, int timeout_ms);

/**
 * @brief  close the websession and release resource related
 * @param  [in] session
 * @return 0/-1
 */
int wsession_close(wsession_t *session);

/**
 * @brief  destroy a web session
 * @param  [in] session
 * @return 0/-1
 */
int wsession_destroy(wsession_t *session);

/**
 * @brief  open the url, send requst header and read/parse the resp header
 * @param  [in] session
 * @param  [in] url
 * @param  [in] redirect : the max times of redirect(3xx)
 * @return 0/-1
 */
int wsession_get(wsession_t *session, const char *url, int redirect);

/**
 * @brief  open the url, send requst header and read/parse the resp header
 * @param  [in] session
 * @param  [in] url
 * @param  [in] redirect : the max times of redirect(3xx)
 * @param  [in] range_s  : start pos of the range request
 * @param  [in] range_e  : end pos of the range request. -1 means the end default
 * @return 0/-1
 */
int wsession_get_range(wsession_t *session, const char *url, int redirect, int range_s, int range_e);

__END_DECLS__

#endif /* __WEB_H__ */

