/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ICORE_MSG_H__
#define __ICORE_MSG_H__

#include "avutil/common.h"

__BEGIN_DECLS__

typedef int msgid_t;

#define ICORE_ALIGNMENT  (16)
#define ICORE_ALIGN(addr)      (AV_ALIGN((addr), ICORE_ALIGNMENT))
#define ICORE_ALIGN_BUFZ(size) (AV_ALIGN_BUFZ((size), ICORE_ALIGNMENT * 2))

typedef struct icore_resp {
    int         code;       // return code
    size_t      resp_size;  // if have
    char        *resp;      // if have
} icore_resp_t;

typedef struct icore_msg {
    char        *orip;      // origin pointer of this message self for align
    msgid_t      id;
    size_t       size;
    icore_resp_t ret;
    char         msg_body[0];
} icore_msg_t;

#define ICORE_MSG_SIZE sizeof(icore_msg_t)

#define icore_get_msg(msg, type)  \
	(msg ? (((msg)->size == sizeof(type) ? (type*)((char*)(msg) + ICORE_MSG_SIZE) : NULL)) : NULL)

#define icore_resp_get(msg)  \
	(msg ? &(((icore_msg_t*)(msg))->ret) : NULL)

/**
 * @brief  new one icore message by msdid
 * @param  [in] id
 * @param  [in] size : size of the msg actualy
 * @return NULL on error
 */
icore_msg_t *icore_msg_new(msgid_t id, size_t size);

/**
 * @brief  free res by icore_msg_new
 * @param  [in] msg
 * @return
 */
void icore_msg_free(icore_msg_t *msg);


__END_DECLS__

#endif /* __ICORE_MSG_H__ */

