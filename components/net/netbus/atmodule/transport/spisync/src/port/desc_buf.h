#ifndef __DESC_BUF_H__
#define __DESC_BUF_H__

#include <stdint.h>

typedef struct {
    void *hdl;
} desc_msg_t;

#define DESC_FOREVER 0xFFFFFFFF
#define DESC_BUF_DEBUG_ENABLE    (0)

int desc_init(desc_msg_t *q, uint32_t items);
int desc_deinit(desc_msg_t *q);
int desc_pop(desc_msg_t *q, void *buf, uint32_t *len_p, uint32_t timeout_ms);// 0-noblock DESC_FOREVER-block
int desc_push_toback(desc_msg_t *q, void *buf, uint32_t len, uint32_t timeout_ms);
int desc_push_tofront(desc_msg_t *q, void *buf, uint32_t len, uint32_t timeout_ms);//vip
int desc_peek(desc_msg_t *q, uint32_t *len_p, uint32_t timeout_ms);// 0-noblock DESC_FOREVER-block
int desc_deinit(desc_msg_t *q);

#endif

