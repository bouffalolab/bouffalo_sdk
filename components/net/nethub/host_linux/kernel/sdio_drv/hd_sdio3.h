
/**
 * @file hd_sdio3.h
 * @brief Host Driver SDIO3 hardware interface header definitions
 * @details This file contains SDIO3 specific configuration macros, custom register
 *          definitions, and hardware abstraction layer interfaces
 * @author mlwang
 * @date 2025-07-17
 * @version 1.0
 */

#ifndef __HD_SDIO3_H__
#define __HD_SDIO3_H__

#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

/** @defgroup SDIO3_CONFIG SDIO3 Configuration Macros
 * @{
 */

/**
 * @brief Enable/disable reading upload length register
 * @details When enabled, reads actual data length from device register.
 *          When disabled, uses fixed buffer size. Reading length register
 *          may be inefficient for performance-critical applications.
 */
#define SDIO3_HOST_UPLD_RD_LEN_EN             (1)

/**
 * @brief Enable/disable writing download length register
 * @details When enabled, writes data length to device before transfer.
 */
#define SDIO3_HOST_DNLD_WR_LEN_EN             (0)

/** SDIO3 function 1 block size */
#define SDIO3_HOST_FN1_BLOCK_SIZE             (512)
/** SDIO3 function 2 block size */
#define SDIO3_HOST_FN2_BLOCK_SIZE             (512)

/** Function 1 maximum host upload buffer size */
#define HD_SDIO3_HOST_FN1_UPLD_SIZE_MAX       (3 * 1024)
/** Function 2 maximum host upload buffer size */
#define HD_SDIO3_HOST_FN2_UPLD_SIZE_MAX       (3 * 1024)

/** Function 1 maximum CMD53 byte mode transfer size */
#define SDIO3_FN1_CMD53_BYTE_MOD_SIZE_MAX     ((SDIO3_HOST_FN1_BLOCK_SIZE > 512) ? 512 : SDIO3_HOST_FN1_BLOCK_SIZE)
/** Function 2 maximum CMD53 byte mode transfer size */
#define SDIO3_FN2_CMD53_BYTE_MOD_SIZE_MAX     ((SDIO3_HOST_FN2_BLOCK_SIZE > 512) ? 512 : SDIO3_HOST_FN2_BLOCK_SIZE)

/**
 * @brief Get maximum CMD53 byte mode transfer size for specific function
 * @param fn Function number (1 or 2)
 * @return Maximum transfer size
 */
#define SDIO3_CMD53_BYTE_MOD_SIZE_MAX(fn)     ((fn == 1) ? SDIO3_FN1_CMD53_BYTE_MOD_SIZE_MAX : SDIO3_FN2_CMD53_BYTE_MOD_SIZE_MAX)

/** @} */

/** @defgroup SDIO3_CUSTOM_REGISTERS SDIO3 Custom Register Definitions
 * @brief SDIO3 custom register functionality definitions
 * @details These registers provide extended functionality beyond standard SDIO.
 *          Total 512 bytes: 0-127 reserved, func1: 128-255, func2: 256-383, 384-511 user area
 * @{
 */

/** Base address for SDIO3 custom registers */
#define SDIO3_HOST_CUSTOM_REG_ADDR            (0x6000)

/** Total size of custom register area */
#define SDIO3_CUSTOM_REG_SIZE                 (512)

/**
 * @brief Get function-specific custom register offset
 * @param fn Function number (1 or 2)
 * @return Base offset for function registers (func1: 128-255, func2: 256-383)
 */
#define SDIO3_CUSTOM_REG_FUNC_OFFSET(fn)      (128 * fn)

/** @defgroup SDIO3_FUNC_QUEUE_REGS Function Queue Control Registers
 * @brief Download/upload queue information registers
 * @{
 */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE   (0)  /**< Download max size (2 bytes) */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN   (2)  /**< Download queue in/push pointer (1 byte) */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT  (4)  /**< Download queue out/pop pointer (1 byte) */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_MAX_SIZE   (8)  /**< Upload max size (2 bytes) */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN   (10) /**< Upload queue in/push pointer (1 byte) */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT  (12) /**< Upload queue out/pop pointer (1 byte) */
#define SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH (14) /**< Queue maximum depth (1 byte) */
/** @} */

/** @defgroup SDIO3_FUNC_INFO_REGS Function Information Registers
 * @brief Function configuration and status registers
 * @{
 */
#define SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE      (32) /**< Block size (2 bytes) */
#define SDIO3_CUSTOM_REG_FUNC_CARD_READY      (34) /**< Card ready status (1 byte) */
#define SDIO3_CUSTOM_REG_FUNC_HOST_READY      (35) /**< Host ready status (1 byte) */
#define SDIO3_CUSTOM_REG_FUNC_STA_FLAG        (36) /**< SDIO system status (1 byte) */
/** @} */

/** @defgroup SDIO3_FUNC_QUEUE_ELEM Function Queue Element Registers
 * @brief Queue element storage registers
 * @{
 */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE      (64) /**< Download queue elements (64-95, 2*16 bytes) */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE      (96) /**< Upload queue elements (96-127, 2*16 bytes) */
/** @} */

/** User-defined custom register area offset */
#define SDIO3_CUSTOM_REG_USER_OFFSET          (384) /**< User area (384-511, 128 bytes) */

/** @} */

/** @defgroup SDIO2_STA_FLAG sdio system status and mode type.
  * @{
  */
#define SDIO3_STA_FLAG_SDIO_BOOT              (1 << 7)
#define SDIO3_STA_FLAG_APP_RUN                (1 << 6)
#define SDIO3_STA_FLAG_RD_LEN_COMPRESS_SUP    (1 << 5)
/**
  * @}
  */

/* sdio3 */
extern struct sdio_driver hd_sdio3;

#endif
