#ifndef _BFLB_EF_CTRL_H
#define _BFLB_EF_CTRL_H

#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 *  @brief Efuse common trim config definition
 */
typedef struct
{
    char *name;           /*!< trim anme */
    uint16_t en_addr;     /*!< enable address */
    uint16_t parity_addr; /*!< parity address */
    uint16_t value_addr;  /*!< value address */
    uint16_t value_len;   /*!< value length */
} bflb_ef_ctrl_com_trim_cfg;

/**
 *  @brief Efuse common trim type definition
 */
typedef struct
{
    uint8_t en;     /*!< Enable status */
    uint8_t parity; /*!< Trim parity */
    uint8_t empty;  /*!< Trim empty */
    uint8_t len;    /*!< Trim value len in bit */
    uint32_t value; /*!< Trim value */
} bflb_ef_ctrl_com_trim_type;

uint32_t bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg **trim_list);
int bflb_ef_ctrl_autoload_done(struct bflb_device_s *dev);
void bflb_ef_ctrl_write_direct(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t program);
void bflb_ef_ctrl_read_direct(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t reload);
void bflb_ef_ctrl_read_common_trim(struct bflb_device_s *dev, char *name, bflb_ef_ctrl_com_trim_type *trim, uint8_t reload);
void bflb_ef_ctrl_write_common_trim(struct bflb_device_s *dev, char *name, uint32_t value, uint8_t program);
uint8_t bflb_ef_ctrl_is_all_bits_zero(uint32_t val, uint8_t start, uint8_t len);
uint32_t bflb_ef_ctrl_get_byte_zero_cnt(uint8_t val);
uint8_t bflb_ef_ctrl_get_trim_parity(uint32_t val, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
