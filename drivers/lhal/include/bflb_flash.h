#ifndef _BFLB_FLASH_H
#define _BFLB_FLASH_H

#include "bflb_core.h"

#define FLASH_IOMODE_NIO 0 /*!< Normal IO mode define */
#define FLASH_IOMODE_DO  1 /*!< Dual Output mode define */
#define FLASH_IOMODE_QO  2 /*!< Quad Output mode define */
#define FLASH_IOMODE_DIO 3 /*!< Dual IO mode define */
#define FLASH_IOMODE_QIO 4 /*!< Quad IO mode define */

#define FLASH_AES_KEY_128BITS        0
#define FLASH_AES_KEY_192BITS        2
#define FLASH_AES_KEY_256BITS        1
#define FLASH_AES_KEY_DOUBLE_128BITS 3

#if defined(BL602) || defined(BL702) || defined(BL702L)
#define FLASH_XIP_BASE (0x23000000)
#elif defined(BL616) || defined(BL628)
#define FLASH_XIP_BASE (0xA0000000)
#elif defined(BL808) || defined(BL606P)
#define FLASH_XIP_BASE (0x58000000)
#endif

struct bflb_flash_aes_config_s {
    uint8_t region;
    uint8_t region_enable;
    uint8_t lock_enable;
    const uint8_t *key;
    uint8_t keybits;
    uint8_t *iv;
    uint32_t start_addr;
    uint32_t end_addr;
};

#ifdef __cplusplus
extern "C" {
#endif

int bflb_flash_init(void);
uint32_t bflb_flash_get_jedec_id(void);
void bflb_flash_get_cfg(uint8_t **cfg_addr, uint32_t *len);
void bflb_flash_set_iomode(uint8_t iomode);
uint32_t bflb_flash_get_image_offset(void);
int bflb_flash_erase(uint32_t addr, uint32_t len);
int bflb_flash_write(uint32_t addr, uint8_t *data, uint32_t len);
int bflb_flash_read(uint32_t addr, uint8_t *data, uint32_t len);

void bflb_flash_aes_init(struct bflb_flash_aes_config_s *config);
void bflb_flash_aes_enable(void);
void bflb_flash_aes_disable(void);

#ifdef __cplusplus
}
#endif

#endif