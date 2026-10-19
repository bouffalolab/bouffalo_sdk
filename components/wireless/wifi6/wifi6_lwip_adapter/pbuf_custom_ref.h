#ifndef __PBUF_CUSTOM_REF_H
#define __PBUF_CUSTOM_REF_H

#include "lwip/pbuf.h"

struct pbuf *pbuf_custom_ref_create(struct pbuf *orig, u16_t offset, u16_t len);

#endif
