#ifndef _BFLB_EFUSE_H
#define _BFLB_EFUSE_H

#include "bflb_core.h"
#include "bflb_ef_ctrl.h"

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

/**
 * @brief  Efuse read chipid
 *
 * @param  chipid[8]: chip id buffer
 *
 * @return 0 or -1
 *
 */
int bflb_efuse_get_chipid(uint8_t chipid[8]);

/**
 * @brief  Efuse read device info
 *
 * @param  device_info: device info pointer
 *
 * @return 0 or -1
 *
 */
void bflb_efuse_get_device_info(bflb_efuse_device_info_type *device_info);

/**
 * @brief  Efuse read optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  reload: Whether reload
 *
 * @return 0 or -1
 *
 */
int bflb_efuse_read_mac_address(uint8_t mac[6], uint8_t reload);

/**
 * @brief  Whether MAC address slot is empty
 *
 * @param  slot: MAC address slot
 * @param  reload: whether  reload to check
 *
 * @return 0 for all slots full,1 for others
 *
 */
uint8_t bflb_efuse_is_mac_address_slot_empty(uint8_t slot, uint8_t reload);

/**
 * @brief  Efuse read optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  reload: Whether reload
 *
 * @return 0 or -1
 */
int bflb_efuse_read_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t reload);

/**
 * @brief  Efuse read adc trim
 *
 * @param  None
 *
 * @return coe
 *
 */
float bflb_efuse_get_adc_trim(void);

/**
 * @brief  Efuse read adc tsen trim
 *
 * @param  None
 *
 * @return int
 *
 */
uint32_t bflb_efuse_get_adc_tsen_trim(void);

/**
 * @brief  Efuse read secure boot
 *
 * @param  sign: sign type
 * @param  aes: aes type
 *
 * @return None
 *
 */
void bflb_efuse_read_secure_boot(uint8_t *sign, uint8_t *aes);

/**
 * @brief  Efuse write secure boot AES
 *
 * @param  aes_type: aes type
 * @param  xts_mode: xts type
 *
 * @return 0 for success
 *
 */
int bflb_efuse_enable_aes(uint8_t aes_type, uint8_t xts_mode);

/**
 * @brief  Efuse write AES key lock
 *
 * @param  aes_type: aes type
 * @param  xts_mode: xts type
 *
 * @return 0 for success
 *
 */
int bflb_efuse_rw_lock_aes_key(uint8_t key_index, uint8_t rd_lock, uint8_t wr_lock);

/**
 * @brief  Efuse write dbg key lock
 *
 * @param  rd_lock: read lock
 * @param  wr_lock: write lock
 *
 * @return 0 for success
 *
 */
int bflb_efuse_rw_lock_dbg_key(uint8_t rd_lock, uint8_t wr_lock);

/**
 * @brief  Efuse write public key hash lock
 *
 * @param  pkhash_len: public key length
 *
 * @return 0 for success
 *
 */
int bflb_efuse_write_lock_pk_hash(uint32_t pkhash_len);

/**
 * @brief  Efuse write usb pid vid lock
 *
 * @param  None
 *
 * @return 0 for success
 *
 */
int bflb_efuse_write_lock_usb_pid_vid(void);

#ifdef __cplusplus
}
#endif

#endif
