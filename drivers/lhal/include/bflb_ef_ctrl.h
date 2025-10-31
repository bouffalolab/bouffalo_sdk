#ifndef _BFLB_EF_CTRL_H
#define _BFLB_EF_CTRL_H

#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup EF_CTRL
  * @{
  */
#if defined(BL616L) || defined(BL616D)
#define EF_CTRL_SF_AES_NONE (0) /*!< No AES */
#define EF_CTRL_SF_AES_128  (1) /*!< AES 128 */
#define EF_CTRL_SF_AES_192  (2) /*!< AES 192 */
#define EF_CTRL_SF_AES_256  (3) /*!< AES 256 */
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
} bflb_ef_ctrl_com_trim_cfg_t;

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
} bflb_ef_ctrl_com_trim_t;

/**
 *  @brief Efuse ctrl para type definition
 */
typedef struct
{
    uint16_t pd_1st;      /*!< stable */
    uint16_t pd_cs_s;     /*!< >500ns */
    uint16_t cs;          /*!< >6.6ns */
    uint16_t rd_adr;      /*!< >6.3ns */
    uint16_t rd_dat;      /*!< >199ns */
    uint16_t rd_dmy;      /*!< >14.9ns */
    uint16_t pd_cs_h;     /*!< >1ns */
    uint16_t ps_cs;       /*!< >50ns */
    uint16_t wr_adr;      /*!< >6.3ns */
    uint16_t pp;          /*!< >11-13us */
    uint16_t pi;          /*!< >14.9ns */
} bflb_ef_ctrl_para_t;


/**
 * @brief Get efuse control common trim list.
 *
 * @param [in] trim_list pointer to save trim list
 * @return trim list length
 */
uint32_t bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg_t **trim_list);

/**
 * @brief
 *
 * @param [in] dev device handle
 * @return int
 */
int bflb_ef_ctrl_autoload_done(struct bflb_device_s *dev);

/**
 * @brief
 *
 * @param [in] para parameter for efuse program or read
 * @return int
 */
int bflb_ef_ctrl_set_para(bflb_ef_ctrl_para_t *para);

/**
 * @brief
 *
 * @param [in] dev device handle
 * @param [in] offset
 * @param [in] pword
 * @param [in] count
 * @param [in] program
 */
void bflb_ef_ctrl_write_direct(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t program);

/**
 * @brief
 *
 * @param [in] dev device handle
 * @param [in] offset
 * @param [in] pword
 * @param [in] count
 * @param [in] reload
 */
void bflb_ef_ctrl_read_direct(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t reload);

/**
 * @brief
 *
 * @param [in] dev device handle
 * @param [in] name
 * @param [in] trim
 * @param [in] reload
 */
void bflb_ef_ctrl_read_common_trim(struct bflb_device_s *dev, char *name, bflb_ef_ctrl_com_trim_t *trim, uint8_t reload);

/**
 * @brief
 *
 * @param [in] dev device handle
 * @param [in] name
 * @param [in] value
 * @param [in] program
 */
void bflb_ef_ctrl_write_common_trim(struct bflb_device_s *dev, char *name, uint32_t value, uint8_t program);

/**
 * @brief
 *
 * @param [in] val
 * @param [in] start
 * @param [in] len
 * @return uint8_t
 */
uint8_t bflb_ef_ctrl_is_all_bits_zero(uint32_t val, uint8_t start, uint8_t len);

/**
 * @brief
 *
 * @param [in] val
 * @return uint32_t
 */
uint32_t bflb_ef_ctrl_get_byte_zero_cnt(uint8_t val);

/**
 * @brief
 *
 * @param [in] val
 * @param [in] len
 * @return uint8_t
 */
uint8_t bflb_ef_ctrl_get_trim_parity(uint32_t val, uint8_t len);


/**
 * @brief  Check efuse busy status
 *
 * @param dev  ef control device pointer
 *
 * @return 1 for busy 0 for not
 *
 */
int bflb_ef_ctrl_busy(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif
