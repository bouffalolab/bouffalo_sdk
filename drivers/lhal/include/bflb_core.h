#ifndef _BFLB_CORE_H
#define _BFLB_CORE_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#if !defined(BL616D) || defined(CPU_MODEL_A0) || defined(CPU_LP)
#include <risc-v/csr.h>
#endif
#include <risc-v/riscv_arch.h>
#include <compiler/compiler_gcc.h>
#include <compiler/compiler_ld.h>
#include "bflb_name.h"
#include "bflb_common.h"
#include "bflb_mtimer.h"
#include "bflb_irq.h"
#include "bflb_l1c.h"

#ifdef CONFIG_LHAL_ROMAPI
#include "bflb_lhal_romdriver.h"
#endif

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup CORE
  * @{
  */

#if !defined(BL602) && !defined(BL702) && !defined(BL702L) &&  \
    !defined(BL616) && !defined(BL616L) && !defined(BL616D) && \
    !defined(BL606P) && !defined(BL808) && !defined(BL628)
#error please define a supported chip
#endif

#ifdef CONFIG_LHAL_PARAM_ASSERT
#define LHAL_PARAM_ASSERT(expr) ((expr) ? (void)0 : bflb_lhal_assert_func(__FILE__, __LINE__, __FUNCTION__, #expr))
#else
#define LHAL_PARAM_ASSERT(expr) ((void)0U)
#endif

#if defined(BL702)
#define BFLB_PSRAM_BASE 0x26000000
#elif defined(BL702L)
#define BFLB_PSRAM_BASE 0x24000000
#elif defined(BL616)
#define BFLB_PSRAM_BASE 0xA8000000
#elif defined(BL808)
#define BFLB_PSRAM_BASE 0x50000000
#elif defined(BL606P)
#define BFLB_PSRAM_BASE 0x54000000
#elif defined(BL616D) || defined(BL616L)
#define BFLB_PSRAM_BASE 0x88000000
#endif

#define BFLB_DEVICE_TYPE_ADC        0
#define BFLB_DEVICE_TYPE_DAC        1
#define BFLB_DEVICE_TYPE_AUDIOADC   2
#define BFLB_DEVICE_TYPE_AUDIODAC   3
#define BFLB_DEVICE_TYPE_GPIO       4
#define BFLB_DEVICE_TYPE_UART       5
#define BFLB_DEVICE_TYPE_SPI        6
#define BFLB_DEVICE_TYPE_I2C        7
#define BFLB_DEVICE_TYPE_DMA        8
#define BFLB_DEVICE_TYPE_I2S        9
#define BFLB_DEVICE_TYPE_IR         10
#define BFLB_DEVICE_TYPE_TIMER      11
#define BFLB_DEVICE_TYPE_PWM        12
#define BFLB_DEVICE_TYPE_CAMERA     14
#define BFLB_DEVICE_TYPE_FLASH      15
#define BFLB_DEVICE_TYPE_QSPI       16
#define BFLB_DEVICE_TYPE_SDH        17
#define BFLB_DEVICE_TYPE_SDU        18
#define BFLB_DEVICE_TYPE_ETH        19
#define BFLB_DEVICE_TYPE_RTC        20
#define BFLB_DEVICE_TYPE_CRC        21
#define BFLB_DEVICE_TYPE_RNG        22
#define BFLB_DEVICE_TYPE_MIPI       23
#define BFLB_DEVICE_TYPE_DPI        24
#define BFLB_DEVICE_TYPE_DSI        25
#define BFLB_DEVICE_TYPE_CSI        26
#define BFLB_DEVICE_TYPE_USB        27
#define BFLB_DEVICE_TYPE_SEC_AES    28
#define BFLB_DEVICE_TYPE_SEC_SHA    29
#define BFLB_DEVICE_TYPE_SEC_MD5    30
#define BFLB_DEVICE_TYPE_SEC_TRNG   31
#define BFLB_DEVICE_TYPE_SEC_PKA    32
#define BFLB_DEVICE_TYPE_CKS        33
#define BFLB_DEVICE_TYPE_MJPEG      34
#define BFLB_DEVICE_TYPE_KYS        35
#define BFLB_DEVICE_TYPE_DBI        36
#define BFLB_DEVICE_TYPE_WDT        37
#define BFLB_DEVICE_TYPE_EF_CTRL    38
#define BFLB_DEVICE_TYPE_SDIO2      39
#define BFLB_DEVICE_TYPE_SDIO3      40
#define BFLB_DEVICE_TYPE_PLFMDMA    41
#define BFLB_DEVICE_TYPE_WO         42
#define BFLB_DEVICE_TYPE_GMAC       43
#define BFLB_DEVICE_TYPE_IPC        44
#define BFLB_DEVICE_TYPE_MJDEC      45
#define BFLB_DEVICE_TYPE_PSRAM      46
#define BFLB_DEVICE_TYPE_TOUCH      47
#define BFLB_DEVICE_TYPE_CAN        48
#define BFLB_DEVICE_TYPE_PEC        49
#define BFLB_DEVICE_TYPE_EMAC_V2    50
#define BFLB_DEVICE_TYPE_DVP_RASTER 51
#define BFLB_DEVICE_TYPE_OSD        52
#define BFLB_DEVICE_TYPE_SEC_GMAC   53
#define BFLB_DEVICE_TYPE_TOUCH_V2   54
#define BFLB_DEVICE_TYPE_DMA2D      55

struct bflb_device_s {
    const char *name;
    uint32_t reg_base;
    uint8_t irq_num;
    uint8_t idx;
    uint8_t sub_idx;
    uint8_t dev_type;
    void *user_data;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get device handle by name.
 *
 * @param [in] name device name
 * @return device handle
 */
struct bflb_device_s *bflb_device_get_by_name(const char *name);

/**
 * @brief Set user data into device handle.
 *
 * @param [in] device device handle
 * @param [in] user_data pointer to user data
 */
void bflb_device_set_userdata(struct bflb_device_s *device, void *user_data);

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
