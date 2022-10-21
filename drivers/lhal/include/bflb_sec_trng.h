#ifndef _BFLB_SEC_TRNG_H
#define _BFLB_SEC_TRNG_H

#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

void bflb_trng_read(struct bflb_device_s *dev, uint8_t data[32]);

#ifdef __cplusplus
}
#endif

#endif