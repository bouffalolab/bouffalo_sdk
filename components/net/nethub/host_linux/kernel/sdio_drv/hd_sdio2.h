
/**
 * @file hd_sdio2.h
 * @brief Host Driver SDIO2 hardware interface header definitions
 * @details This file contains SDIO2 specific register definitions, configuration
 *          macros, and hardware abstraction layer interfaces
 * @author mlwang
 * @date 2025-07-17
 * @version 1.0
 */

#ifndef __HD_SDIO2_H__
#define __HD_SDIO2_H__

#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

/** @defgroup SDIO2_CONFIG SDIO2 Configuration Macros
 * @{
 */

/**
 * @brief Enable/disable reading upload length register
 * @details When enabled (1), read actual data length from device register.
 *          When disabled (0), use fixed dnld_max size for upload operations.
 *          For long data transfers, reading length register may be inefficient.
 */
#define SDIO2_HOST_UPLD_RD_LEN_EN          (1)

/** Block size for SDIO2 transfers (must be power of 2) */
#define SDIO2_HOST_BLOCK_SIZE              (512)

/** Maximum number of ports for normal operation (must be power of 2) */
#define SDIO2_HOST_PORT_NUM_MAX            (16)
/** Maximum number of ports for bootrom operation (must be 1) */
#define SDIO2_HOST_BOOTROM_PORT_NUM_MAX    (1)

/** Maximum host upload buffer size */
#define SDIO2_HOST_UPLD_SIZE_MAX           (1024 * 2)
/** Default maximum slave download buffer size (used when handshake fails) */
#define SDIO2_HOST_DNLD_SIZE_MAX_DEFAULT   (1024 * 2)
/** Size granularity for buffer size negotiation during handshake */
#define SDIO2_SIZE_CONSULT_MULTIPLE        (64)

/** Maximum size for CMD53 byte mode transfers */
#define SDIO2_CMD53_BYTE_MOD_SIZE_MAX      ((SDIO2_HOST_BLOCK_SIZE > 512) ? 512 : SDIO2_HOST_BLOCK_SIZE)

/** @} */

/** @defgroup SDIO2_REGISTERS SDIO2 Slave Function Register Offsets
 * @brief Register offset definitions for SDIO2 slave function
 * @{
 */
#define SDIO2_HOST_TO_CARD_EVENT_OFFSET    (0x00) /**< Host to card event register */
#define SDIO2_HOST_INT_CAUSE_OFFSET        (0x01) /**< Host interrupt cause register */
#define SDIO2_HOST_INT_MASK_OFFSET         (0x02) /**< Host interrupt mask register */
#define SDIO2_HOST_INT_STATUS_OFFSET       (0x03) /**< Host interrupt status register */
#define SDIO2_RD_BIT_MAP_OFFSET            (0x04) /**< Read bitmap register */
#define SDIO2_WR_BIT_MAP_OFFSET            (0x06) /**< Write bitmap register */
#define SDIO2_RD_LEN_OFFSET                (0x08) /**< Read length register */
#define SDIO2_HOST_TRANS_STATUS_OFFSET     (0x28) /**< Host transfer status register */
#define SDIO2_CARD_TO_HOST_EVENT_OFFSET    (0x30) /**< Card to host event register */
#define SDIO2_CARD_INT_MASK_OFFSET         (0x34) /**< Card interrupt mask register */
#define SDIO2_CARD_INT_STATUS_OFFSET       (0x38) /**< Card interrupt status register */
#define SDIO2_CARD_INT_MODE_OFFSET         (0x3C) /**< Card interrupt mode register */
#define SDIO2_SQ_READ_BASE_OFFSET          (0x40) /**< Sequence read base register */
#define SDIO2_SQ_WRITE_BASE_OFFSET         (0x44) /**< Sequence write base register */
#define SDIO2_READ_INDEX_OFFSET            (0x48) /**< Read index register */
#define SDIO2_WRITE_INDEX_OFFSET           (0x49) /**< Write index register */
#define SDIO2_DNLD_QUEUE_WRPTR_OFFSET      (0x4A) /**< Download queue write pointer */
#define SDIO2_UPLD_QUEUE_WRPTR_OFFSET      (0x4B) /**< Upload queue write pointer */
#define SDIO2_DNLD_QUEUE_OFFSET            (0x4C) /**< Download queue register */
#define SDIO2_UPLD_QUEUE_OFFSET            (0x54) /**< Upload queue register */
#define SDIO2_CHIP_VERSION_OFFSET          (0x5C) /**< Chip version register */
#define SDIO2_IP_VERSION0_OFFSET           (0x5E) /**< IP version 0 register */
#define SDIO2_IP_VERSION1_OFFSET           (0x5F) /**< IP version 1 register */
#define SDIO2_SCRATCH0_0_OFFSET            (0x60) /**< Scratch register 0_0 (host ready/upld max size) */
#define SDIO2_SCRATCH0_1_OFFSET            (0x61) /**< Scratch register 0_1 (card ready/dnld max size) */
#define SDIO2_SCRATCH0_2_OFFSET            (0x62) /**< Scratch register 0_2 (sdio status flag) */
#define SDIO2_SCRATCH0_3_OFFSET            (0x63) /**< Scratch register 0_3 (dnld/upld queue out pointer ) */
#define SDIO2_SCRATCH1_0_OFFSET            (0x64) /**< Scratch register 1_0 (reserved) */
#define SDIO2_SCRATCH1_1_OFFSET            (0x65) /**< Scratch register 1_1 (reserved) */
#define SDIO2_SCRATCH1_2_OFFSET            (0x66) /**< Scratch register 1_2 (reserved) */
#define SDIO2_SCRATCH1_3_OFFSET            (0x67) /**< Scratch register 1_3 (reserved) */
#define SDIO2_OCR0_OFFSET                  (0x68) /**< OCR register 0 */
#define SDIO2_OCR1_OFFSET                  (0x69) /**< OCR register 1 */
#define SDIO2_OCR2_OFFSET                  (0x6A) /**< OCR register 2 */
#define SDIO2_CONFIG_OFFSET                (0x6B) /**< Configuration register */
#define SDIO2_CONFIG2_OFFSET               (0x6C) /**< Configuration register 2 */
#define SDIO2_DEBUG_OFFSET                 (0x70) /**< Debug register */
#define SDIO2_DMA_ADDR_OFFSET              (0x74) /**< DMA address register */
#define SDIO2_IO_PORT_OFFSET               (0x78) /**< IO port register */

/**
 * @brief Get upload length register offset for specific port
 * @param n Port number
 * @return Register offset
 */
#define SDIO2_PORTn_UPLD_LEN_OFFSET(n)     (SDIO2_RD_LEN_OFFSET + 2 * n)

/** @} */

/** @defgroup SDIO2_STATUS_FLAGS SDIO2 System Status and Mode Flags
 * @brief Status flags for SDIO system operation modes
 * @{
 */
#define SDIO2_STA_FLAG_NOT_SDIO_BOOT       (1 << 7) /**< Not in SDIO boot mode */
#define SDIO2_STA_FLAG_APP_RUN             (1 << 6) /**< Application running flag */
#define SDIO2_STA_FLAG_RD_LEN_COMPRESS_SUP (1 << 5) /**< Read length compression support */
/** @} */

/** @defgroup SDIO2_H2C_EVENTS Host to Card Event Bits (0x00)
 * @{
 */
#define SDIO2_H2C_EVENT_PWR_DN             (1 << 0) /**< Power down event */
#define SDIO2_H2C_EVENT_PWR_UP             (1 << 1) /**< Power up event */
#define SDIO2_H2C_EVENT_FIN_HOST           (1 << 3) /**< Finish host event */
/** @} */

/** @defgroup SDIO2_HOST_INT_RSR Host Interrupt RSR Bits (0x01)
 * @{
 */
#define SDIO2_HOST_INT_RSR_UPLD            (1 << 0) /**< Upload interrupt */
#define SDIO2_HOST_INT_RSR_DNLD            (1 << 1) /**< Download interrupt */
#define SDIO2_HOST_INT_RSR_UNDERFLOW       (1 << 2) /**< Buffer underflow interrupt */
#define SDIO2_HOST_INT_RSR_OVERFLOW        (1 << 3) /**< Buffer overflow interrupt */
/** @} */

/** @defgroup SDIO2_HOST_INT_ENABLE Host Interrupt Enable Bits (0x02)
 * @{
 */
#define SDIO2_HOST_INT_UMSK_UPLD           (1 << 0) /**< Enable upload interrupt */
#define SDIO2_HOST_INT_UMSK_DNLD           (1 << 1) /**< Enable download interrupt */
#define SDIO2_HOST_INT_UMSK_UNDERFLOW      (1 << 2) /**< Enable underflow interrupt */
#define SDIO2_HOST_INT_UMSK_OVERFLOW       (1 << 3) /**< Enable overflow interrupt */
/** @} */

/** @defgroup SDIO2_HOST_INT_STATUS Host Interrupt Status Bits (0x03)
 * @{
 */
#define SDIO2_HOST_INT_STA_UPLD            (1 << 0) /**< Upload interrupt status */
#define SDIO2_HOST_INT_STA_DNLD            (1 << 1) /**< Download interrupt status */
#define SDIO2_HOST_INT_STA_UNDERFLOW       (1 << 2) /**< Underflow interrupt status */
#define SDIO2_HOST_INT_STA_OVERFLOW        (1 << 3) /**< Overflow interrupt status */
/** @} */

/** @defgroup SDIO2_HOST_TRANS_STATUS Host Transfer Status Bits (0x28)
 * @{
 */
#define SDIO2_HOST_TRAN_STA_DNLD_RESTART   (1 << 0) /**< Download restart status */
#define SDIO2_HOST_TRAN_STA_UPLD_RESTART   (1 << 1) /**< Upload restart status */
#define SDIO2_HOST_TRAN_STA_DNLD_CRC_ERR   (1 << 2) /**< Download CRC error status */
/** @} */

/** @defgroup SDIO2_C2H_EVENTS Card to Host Event Bits (0x30)
 * @{
 */
#define SDIO2_C2H_INT_EVENT_DNLD_RDY       (1 << 0) /**< Download ready event */
#define SDIO2_C2H_INT_EVENT_UPLD_RDY       (1 << 1) /**< Upload ready event */
#define SDIO2_C2H_INT_EVENT_CIS_RDY        (1 << 2) /**< CIS ready event */
#define SDIO2_C2H_INT_EVENT_IO_RDY         (1 << 3) /**< IO ready event */
/** @} */

/** @defgroup SDIO2_CARD_INT_ENABLE Card Interrupt Enable Bits (0x34)
 * @{
 */
#define SDIO2_CARD_INT_UMSK_DNLD           (1 << 0) /**< Enable card download interrupt */
#define SDIO2_CARD_INT_UMSK_UPLD           (1 << 1) /**< Enable card upload interrupt */
#define SDIO2_CARD_INT_UMSK_ABORT          (1 << 2) /**< Enable card abort interrupt */
#define SDIO2_CARD_INT_UMSK_PWR_DN         (1 << 3) /**< Enable card power down interrupt */
#define SDIO2_CARD_INT_UMSK_PWR_UP         (1 << 4) /**< Enable card power up interrupt */
/** @} */

/** @defgroup SDIO2_CARD_INT_STATUS Card Interrupt Status Bits (0x38)
 * @{
 */
#define SDIO2_CARD_INT_STA_DNLD            (1 << 0) /**< Card download interrupt status */
#define SDIO2_CARD_INT_STA_UPLD            (1 << 1) /**< Card upload interrupt status */
#define SDIO2_CARD_INT_STA_ABORT           (1 << 2) /**< Card abort interrupt status */
#define SDIO2_CARD_INT_STA_PWR_DN          (1 << 3) /**< Card power down interrupt status */
#define SDIO2_CARD_INT_STA_PWR_UP          (1 << 4) /**< Card power up interrupt status */
/** @} */

/** @defgroup SDIO2_CARD_INT_RSR Card Interrupt RSR Bits (0x3C)
 * @{
 */
#define SDIO2_CARD_INT_RSR_DNLD            (1 << 0) /**< Card download interrupt RSR */
#define SDIO2_CARD_INT_RSR_UPLD            (1 << 1) /**< Card upload interrupt RSR */
#define SDIO2_CARD_INT_RSR_ABORT           (1 << 2) /**< Card abort interrupt RSR */
#define SDIO2_CARD_INT_RSR_PWR_DN          (1 << 3) /**< Card power down interrupt RSR */
#define SDIO2_CARD_INT_RSR_PWR_UP          (1 << 4) /**< Card power up interrupt RSR */
#define SDIO2_CARD_INT_RSR_UNDERFLOW       (1 << 5) /**< Card underflow interrupt RSR */
#define SDIO2_CARD_INT_RSR_OVERFLOW        (1 << 6) /**< Card overflow interrupt RSR */
/** @} */

/** @defgroup SDIO2_SCRATCH_REGS Scratch Register Bit Definitions
 * @{
 */
#define SDIO2_SCRATCH_HOST_READY_MASK      (1 << 0) /**< Host ready mask in scratch register 0_0 */
#define SDIO2_SCRATCH_SLAVE_READY_MASK     (1 << 1) /**< Slave ready mask in scratch register 0_0 */
/** @} */

/** @brief SDIO2 driver structure */
extern struct sdio_driver hd_sdio2;

#endif
