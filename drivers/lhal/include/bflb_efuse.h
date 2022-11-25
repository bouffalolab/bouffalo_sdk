#ifndef _BFLB_EFUSE_H
#define _BFLB_EFUSE_H

#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

float bflb_efuse_get_adc_trim(void);
uint32_t bflb_efuse_get_adc_tsen_trim(void);
void bflb_efuse_write_secure_boot();
void bflb_efuse_write_aes_key(uint8_t index, uint8_t *data, uint32_t len);
void bflb_efuse_read_aes_key(uint8_t index, uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
