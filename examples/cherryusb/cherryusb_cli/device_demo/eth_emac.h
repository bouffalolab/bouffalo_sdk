/**
 * @file eth_emac.h
 * @brief EMAC and Ethernet PHY interface used by the USB network demos.
 */

#ifndef _ETH_EMAC_H_
#define _ETH_EMAC_H_

#include "bflb_core.h"
#include "bflb_emac.h"

/** @name EMAC frame-buffer configuration
 * @{ */
/** @brief Reserved bytes before each frame for protocol encapsulation. */
#define EMAC_BUF_HEAD_SIZE (64)

/** @brief Maximum size of an Ethernet transmit frame. */
#define EMAC_TX_BUFF_SIZE  (14 + 1500 + 4)
/** @brief Number of transmit buffers in the free-buffer pool. */
#define EMAC_TX_BUFF_CNT   (6)

/** @brief Maximum size of an Ethernet receive frame. */
#define EMAC_RX_BUFF_SIZE  (14 + 1500 + 4)
/** @brief Number of receive buffers supplied to the EMAC. */
#define EMAC_RX_BUFF_CNT   (10)
/** @} */

/** @name Default numeric MAC address octets
 * @{ */
#define MAC_ADDR_NUM_0     (0x18)
#define MAC_ADDR_NUM_1     (0xB9)
#define MAC_ADDR_NUM_2     (0x05)
#define MAC_ADDR_NUM_3     (0x12)
#define MAC_ADDR_NUM_4     (0x34)
#define MAC_ADDR_NUM_5     (0x56)
/** @} */

/** @name Default MAC address hexadecimal characters
 *
 * Used by the CDC ECM string descriptor without colon separators.
 * @{ */
#define MAC_ADDR_ASCII_00  ('1')
#define MAC_ADDR_ASCII_01  ('8')
#define MAC_ADDR_ASCII_10  ('B')
#define MAC_ADDR_ASCII_11  ('9')
#define MAC_ADDR_ASCII_20  ('0')
#define MAC_ADDR_ASCII_21  ('5')
#define MAC_ADDR_ASCII_30  ('1')
#define MAC_ADDR_ASCII_31  ('2')
#define MAC_ADDR_ASCII_40  ('3')
#define MAC_ADDR_ASCII_41  ('4')
#define MAC_ADDR_ASCII_50  ('5')
#define MAC_ADDR_ASCII_51  ('6')
/** @} */

/**
 * @brief EMAC event callback type.
 * @param[in] irq_event EMAC interrupt event identifier.
 * @note The callback is invoked from the EMAC interrupt callback context.
 */
typedef void (*eth_emac_event_cb_t)(uint32_t irq_event);

/** @brief Cumulative EMAC transmit and receive statistics. */
struct emac_debug_info_s {
    volatile uint32_t tx_push_cnt;    /**< Transmit descriptors submitted. */
    volatile uint32_t tx_success_cnt; /**< Frames transmitted successfully. */
    volatile uint32_t tx_error_cnt;   /**< Transmit completions with errors. */
    volatile uint64_t tx_total_size;  /**< Total successfully transmitted bytes. */

    volatile uint32_t rx_push_cnt;    /**< Receive descriptors returned to hardware. */
    volatile uint32_t rx_success_cnt; /**< Frames received successfully. */
    volatile uint32_t rx_error_cnt;   /**< Receive frames dropped with errors. */
    volatile uint32_t rx_busy_cnt;    /**< Receive-busy interrupt count. */
    volatile uint64_t rx_total_size;  /**< Total successfully received bytes. */
};

/** @brief Initialize the EMAC, PHY, descriptor queues, and frame buffers.
 * @retval 0 Initialization succeeded.
 * @retval -1 An EMAC or MDIO device was unavailable.
 * @retval -2 No supported PHY was found.
 * @retval -3 PHY initialization failed.
 */
extern int eth_emac_init(void);
/** @brief Disable and release the EMAC resources.
 * @retval 0 Deinitialization completed.
 */
extern int eth_emac_deinit(void);
/** @brief Reset descriptor queues and restart EMAC transmit and receive paths.
 * @pre `eth_emac_init()` completed successfully.
 */
extern void eth_emac_restart(void);
/** @brief Register the callback notified for completed frame events.
 * @param[in] cb Callback to register, or NULL to disable notification.
 */
extern void eth_emac_event_cb_register(eth_emac_event_cb_t cb);
/** @brief Poll the PHY and apply current speed and duplex settings.
 * @retval true The PHY link is currently up.
 * @retval false The PHY link is down or unavailable.
 */
extern bool eth_link_state_update(void);

/** @brief Acquire a transmit buffer from the EMAC pool.
 * @param[out] trans_desc Descriptor receiving the acquired buffer address.
 * @param[in] timeout FreeRTOS queue wait time in ticks.
 * @retval 0 A transmit buffer was acquired.
 * @retval -1 No buffer became available before the timeout.
 */
extern int eth_emac_tx_buff_get(struct bflb_emac_trans_desc_s *trans_desc, uint32_t timeout);
/** @brief Submit a filled transmit descriptor to the EMAC.
 * @param[in,out] trans_desc Descriptor and frame length to submit.
 * @retval 0 The descriptor was queued.
 * @retval -1 The EMAC rejected the descriptor.
 * @note Ownership transfers to the EMAC until completion returns the buffer to the pool.
 */
extern int eth_emac_tx_buff_push(struct bflb_emac_trans_desc_s *trans_desc);
/** @brief Acquire a completed receive descriptor.
 * @param[out] trans_desc Descriptor receiving the frame address and length.
 * @param[in] timeout FreeRTOS queue wait time in ticks.
 * @retval 0 A received frame was acquired.
 * @retval -1 No frame became available before the timeout.
 * @note The caller owns the descriptor until `eth_emac_rx_data_free()`.
 */
extern int eth_emac_rx_data_get(struct bflb_emac_trans_desc_s *trans_desc, uint32_t timeout);
/** @brief Return a consumed receive descriptor to the EMAC.
 * @param[in,out] trans_desc Receive descriptor to recycle.
 * @retval 0 The descriptor was returned.
 * @retval -1 The EMAC rejected the descriptor.
 */
extern int eth_emac_rx_data_free(struct bflb_emac_trans_desc_s *trans_desc);

/** @brief Print cumulative EMAC statistics and descriptor availability. */
void eth_emac_info_dump(void);

/** @brief Shared cumulative EMAC statistics. */
extern struct emac_debug_info_s emac_debug_info;

#endif