#ifndef _BFLB_EFUSE_H
#define _BFLB_EFUSE_H

#include "bflb_core.h"

/**
 *  @brief Efuse device information type definition
 */
typedef struct
{
    uint8_t package;
    uint8_t flash_info;
#if defined(BL602)
    uint8_t ext_info;
    uint8_t mcu_info;
#else
    uint8_t psram_info;
#endif
#if defined(BL702) || defined(BL702L)
    uint8_t sf_swap_cfg;
#else
    uint8_t version;
#endif
#if defined(BL616)
    uint16_t process_corner;
#endif
    const char *package_name;
    const char *flash_info_name;
#if defined(BL602)
#else
    const char *psram_info_name;
#endif
#if defined(BL616)
    char process_corner_name[16];
#endif
} bflb_efuse_device_info_type;

#ifdef __cplusplus
extern "C" {
#endif

void bflb_efuse_get_chipid(uint8_t chipid[8]);

uint8_t bflb_efuse_is_mac_address_slot_empty(uint8_t slot, uint8_t reload);
int bflb_efuse_write_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t program);
int bflb_efuse_read_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t reload);

float bflb_efuse_get_adc_trim(void);
uint32_t bflb_efuse_get_adc_tsen_trim(void);

void bflb_efuse_get_device_info(bflb_efuse_device_info_type *device_info);
void bflb_efuse_read_secure_boot(uint8_t *sign, uint8_t *aes);

#ifdef __cplusplus
}
#endif

#endif
