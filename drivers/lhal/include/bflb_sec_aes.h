#ifndef _BFLB_SEC_AES_H
#define _BFLB_SEC_AES_H

#include "bflb_core.h"

#define AES_MODE_ECB 0
#define AES_MODE_CTR 1
#define AES_MODE_CBC 2
#define AES_MODE_XTS 3

#ifdef __cplusplus
extern "C" {
#endif

void bflb_aes_init(struct bflb_device_s *dev);
void bflb_aes_deinit(struct bflb_device_s *dev);
void bflb_aes_set_mode(struct bflb_device_s *dev, uint8_t mode);
void bflb_aes_setkey(struct bflb_device_s *dev, const uint8_t *key, uint16_t keybits);
int bflb_aes_encrypt(struct bflb_device_s *dev,
                     const uint8_t *input,
                     const uint8_t *iv,
                     uint8_t *output,
                     uint32_t len);
int bflb_aes_decrypt(struct bflb_device_s *dev,
                     const uint8_t *input,
                     const uint8_t *iv,
                     uint8_t *output,
                     uint32_t len);

#ifdef __cplusplus
}
#endif

#endif