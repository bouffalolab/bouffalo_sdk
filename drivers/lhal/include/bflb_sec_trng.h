#ifndef _BFLB_SEC_TRNG_H
#define _BFLB_SEC_TRNG_H

#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

int bflb_trng_read(struct bflb_device_s *dev, uint8_t data[32]);
int bflb_trng_readlen(uint8_t *data, uint32_t len);
long random(void);

void bflb_group0_request_trng_access(struct bflb_device_s *dev);
void bflb_group0_release_trng_access(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

#endif