#ifndef _BFLB_CKS_H
#define _BFLB_CKS_H

#include "bflb_core.h"

/** @defgroup CKS_ENDIAN cks endian definition
  * @{
  */
#define CKS_LITTLE_ENDIAN 0
#define CKS_BIG_ENDIAN    1
/**
  * @}
  */

#ifdef __cplusplus
extern "C" {
#endif

void bflb_cks_reset(struct bflb_device_s *dev);
void bflb_cks_set_endian(struct bflb_device_s *dev, uint8_t endian);
uint16_t bflb_cks_compute(struct bflb_device_s *dev, uint8_t *data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
