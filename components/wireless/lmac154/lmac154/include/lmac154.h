#ifndef __LMAC154_H__
#define __LMAC154_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define VERSION_LMAC154_MAJOR 1
#define VERSION_LMAC154_MINOR 6
#define VERSION_LMAC154_PATCH 15

// #define VERSION_LMAC154_SRC_EXTRA_INFO "customer-1"

typedef void (*lmac154_isr_t)(void);

typedef enum {
    LMAC154_CHANNEL_NONE = -1,
    LMAC154_CHANNEL_11 = 0,
    LMAC154_CHANNEL_12,
    LMAC154_CHANNEL_13,
    LMAC154_CHANNEL_14,
    LMAC154_CHANNEL_15,
    LMAC154_CHANNEL_16,
    LMAC154_CHANNEL_17,
    LMAC154_CHANNEL_18,
    LMAC154_CHANNEL_19,
    LMAC154_CHANNEL_20,
    LMAC154_CHANNEL_21,
    LMAC154_CHANNEL_22,
    LMAC154_CHANNEL_23,
    LMAC154_CHANNEL_24,
    LMAC154_CHANNEL_25,
    LMAC154_CHANNEL_26,
}lmac154_channel_t;

typedef enum {
    LMAC154_TX_POWER_0dBm  = 0,
    LMAC154_TX_POWER_1dBm  = 1,
    LMAC154_TX_POWER_2dBm  = 2,
    LMAC154_TX_POWER_3dBm  = 3,
    LMAC154_TX_POWER_4dBm  = 4,
    LMAC154_TX_POWER_5dBm  = 5,
    LMAC154_TX_POWER_6dBm  = 6,
    LMAC154_TX_POWER_7dBm  = 7,
    LMAC154_TX_POWER_8dBm  = 8,
    LMAC154_TX_POWER_9dBm  = 9,
    LMAC154_TX_POWER_10dBm = 10,
    LMAC154_TX_POWER_11dBm = 11,
    LMAC154_TX_POWER_12dBm = 12,
    LMAC154_TX_POWER_13dBm = 13,
    LMAC154_TX_POWER_14dBm = 14,
}lmac154_tx_power_t;

typedef enum {
    LMAC154_DATA_RATE_250K = 0,
    LMAC154_DATA_RATE_500K = 1,
    LMAC154_DATA_RATE_1M   = 2,
    LMAC154_DATA_RATE_2M   = 3,
}lmac154_data_rate_t;

typedef enum {
    LMAC154_CCA_MODE_ED        = 0,
    LMAC154_CCA_MODE_CS        = 1,
    LMAC154_CCA_MODE_ED_AND_CS = 2,
    LMAC154_CCA_MODE_ED_OR_CS  = 3,
}lmac154_cca_mode_t;

typedef enum {
    LMAC154_FRAME_TYPE_BEACON = 0x01,
    LMAC154_FRAME_TYPE_DATA   = 0x02,
    LMAC154_FRAME_TYPE_ACK    = 0x04,
    LMAC154_FRAME_TYPE_CMD    = 0x08,
    LMAC154_FRAME_TYPE_MPP    = 0x10,
}lmac154_frame_type_t;

typedef enum {
    LMAC154_RF_PWR_OFF        = 0,
    LMAC154_RF_STATE_RX       = 1,
    LMAC154_RF_STATE_RX_DOING = 2,
    LMAC154_RF_STATE_TX       = 3,
    LMAC154_RF_STATE_IDLE     = 4,
}lmac154_rf_state_t;

typedef enum {
    LMAC154_AES_MODE_ENCRYPT = 0,
    LMAC154_AES_MODE_DECRYPT = 1,
}lmac154_aes_mode_t;

typedef enum {
    LMAC154_AES_MIC_LEN_4  = 0,
    LMAC154_AES_MIC_LEN_8  = 1,
    LMAC154_AES_MIC_LEN_16 = 2,
    LMAC154_AES_MIC_LEN_12 = 3,
}lmac154_aes_mic_len_t;

typedef enum {
    LMAC154_AES_STATUS_SUCCESS         = 0,
    LMAC154_AES_STATUS_INVALID_MODE    = -1,
    LMAC154_AES_STATUS_INVALID_MIC_LEN = -2,
    LMAC154_AES_STATUS_REENTER         = -3,
    LMAC154_AES_STATUS_TIMEOUT         = -4,
}lmac154_aes_status_t;

typedef enum {
    LMAC154_FPT_STATUS_SUCCESS          = 0,
    LMAC154_FPT_STATUS_NO_RESOURCE      = -1,
    LMAC154_FPT_STATUS_ADDR_NOT_FOUND   = -2,
    LMAC154_FPT_STATUS_EMPTY            = -3,
    LMAC154_FPT_STATUS_INVALID_PARAM    = -4,
    LMAC154_FPT_STATUS_INVALID_OPT      = -5
}lmac154_fpt_status_t;


typedef enum {
    LMAC154_TX_STATUS_TX_FINISHED = 0,
    LMAC154_TX_STATUS_CSMA_FAILED = 1,
    LMAC154_TX_STATUS_TX_ABORTED  = 2,
    LMAC154_TX_STATUS_HW_ERROR    = 3,
    LMAC154_TX_STATUS_DELAY_ERROR = 4,
}lmac154_tx_status_t;

typedef enum {
    LMAC154_EVENT_TIME_RX_START = 0,
    LMAC154_EVENT_TIME_RX_END   = 1,
    LMAC154_EVENT_TIME_TX_END   = 4,
}lmac154_eventTimeType_t;

typedef enum {
    LMAC154_RX_REJECT_CRC_ERR            = 0x00000001, // will reject packets that CRC check error
    LMAC154_RX_REJECT_FRAME_TYPE_RVSD    = 0x00000002, // will reject packets that the value of frame type filed is reserved in 802.15.4 spec
    LMAC154_RX_REJECT_FRAME_VER_RVSD     = 0x00000004, // will reject packets that the value of frame version filed is reserved in 802.15.4 spec
    LMAC154_RX_REJECT_DST_ADDR_M_RVSD    = 0x00000008, // will reject packets that the value of dest address mode filed is reserved in 802.15.4 spec
    LMAC154_RX_REJECT_SRC_ADDR_M_RVSD    = 0x00000010, // will reject packets that the value of src address mode filed is reserved in 802.15.4 spec
    LMAC154_RX_REJECT_DST_PANID_MISMATCH = 0x00000020, // will reject packets that the value of dest PANID does not match with local device
    LMAC154_RX_REJECT_DST_ADDR_MISMATCH  = 0x00000040, // will reject packets that the value of dest address does not match with local device
    LMAC154_RX_REJECT_SRC_PANID_MISMATCH = 0x00000080, // will reject packets that the value of src PANID does not match with local device

    LMAC154_RX_REJECT_NONE               = 0x00000000, // will not reject all packets above
    LMAC154_RX_REJECT_ALL                = 0x000000ff, // will reject all packets above
}lmac154_rx_reject_policy_t;

typedef enum {
    LMAC154_RX_ACCEPT_CRC_ERR            = 0x00000001, // able to receive packets that CRC check error
    LMAC154_RX_ACCEPT_FRAME_TYPE_RVSD    = 0x00000002, // able to receive packets that the value of frame type filed is reserved in 802.15.4 spec 
    LMAC154_RX_ACCEPT_FRAME_VER_RVSD     = 0x00000004, // able to receive packets that the value of frame version filed is reserved in 802.15.4 spec 
    LMAC154_RX_ACCEPT_DST_ADDR_M_RVSD    = 0x00000008, // able to receive packets that the value of dest address mode filed is reserved in 802.15.4 spec 
    LMAC154_RX_ACCEPT_SRC_ADDR_M_RVSD    = 0x00000010, // able to receive packets that the value of src address mode filed is reserved in 802.15.4 spec 
    LMAC154_RX_ACCEPT_DST_PANID_MISMATCH = 0x00000020, // able to receive packets that the value of dest PANID does not match with local device
    LMAC154_RX_ACCEPT_DST_ADDR_MISMATCH  = 0x00000040, // able to receive packets that the value of dest address does not match with local device
    LMAC154_RX_ACCEPT_SRC_PANID_MISMATCH = 0x00000080, // able to receive packets that the value of src PANID does not match with local device

    LMAC154_RX_ACCEPT_NONE               = 0x00000000, // not able to receive all packets above
    LMAC154_RX_ACCEPT_ALL                = 0x000000ff, // able to receive all packets above 
}lmac154_rx_accept_policy_t;

typedef enum {
    lmac154_addr_info_none               = 0x00000000,
    lmac154_addr_info_panid              = 0x00000001,
    lmac154_addr_info_short_addr         = 0x00000002,
    lmac154_addr_info_ext_addr           = 0x00000004,
    lmac154_addr_info_panid_short_addr   = lmac154_addr_info_panid | lmac154_addr_info_short_addr,
    lmac154_addr_info_panid_ext_addr     = lmac154_addr_info_panid | lmac154_addr_info_ext_addr,
} lmac154_addr_info_t;


#define LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK       (7)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_DATA       (1)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_ACK        (2)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_CMD        (3)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_MPP        (5)

#define LMAC154_FRAME_SECURITY_MASK                 (1 << 3)
#define LMAC154_FRAME_FRAME_PENDING_MASK            (1 << 4)
#define LMAC154_FRAME_ACK_REQUEST_MASK              (1 << 5)
#define LMAC154_FRAME_PANID_COMPRESSION             (1 << 6)
#define LMAC154_FRAME_SEQ_SUPRESSION_MASK           (1 << 8)
#define LMAC154_FRAME_IE_MASK                       (1 << 9)
#define LMAC154_FRAME_ADDR_DEST_NONE                (0 << 10)
#define LMAC154_FRAME_ADDR_DEST_SHORT               (2 << 10)
#define LMAC154_FRAME_ADDR_DEST_EXT                 (3 << 10)
#define LMAC154_FRAME_ADDR_DEST_MASK                (3 << 10)
#define LMAC154_FRAME_VERSION_MASK                  (3 << 12)
#define LMAC154_FRAME_VERSION_2015                  (2 << 12)
#define LMAC154_FRAME_ADDR_SRC_NONE                 (0 << 14)
#define LMAC154_FRAME_ADDR_SRC_SHORT                (2 << 14)
#define LMAC154_FRAME_ADDR_SRC_EXT                  (3 << 14)
#define LMAC154_FRAME_ADDR_SRC_MASK                 (3 << 14)

#define LMAC154_FRAME_MPP_LONG_FRAME_BIT            (1 << 3)
#define LMAC154_FRAME_MPP_SECURITY_BIT              (1 << 9)
#define LMAC154_FRAME_MPP_ACK_REQ                   (1 << 14)

#define LMAC154_FRAME_CMD_DATA_REQUEST              (4)
#define LMAC154_FRAME_IS_CMD(x)                     (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_CMD)
#define LMAC154_FRAME_IS_DATA(x)                     (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_DATA)
#define LMAC154_FRAME_IS_ACK(x)                     (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_ACK)

#define LMAC154_FRAME_IS_ACK_REQ(x)                 (((x) & LMAC154_FRAME_ACK_REQUEST_MASK) == LMAC154_FRAME_ACK_REQUEST_MASK)
#define LMAC154_FRAME_IS_PANID_COMPRESSED(x)        (((x) & LMAC154_FRAME_PANID_COMPRESSION) == LMAC154_FRAME_PANID_COMPRESSION)


#define LMAC154_FRAME_IS_FRAME_2015(x)              (((x) & LMAC154_FRAME_VERSION_MASK) == LMAC154_FRAME_VERSION_2015)

#define LMAC154_FRAME_IS_FRAME_PENDED(x)            ((x) & LMAC154_FRAME_FRAME_PENDING_MASK)
#define LMAC154_FRAME_IS_NORMAL_SECURITY(x)         ((x & LMAC154_FRAME_SECURITY_MASK) == LMAC154_FRAME_SECURITY_MASK)

#define LMAC154_FRAME_IS_MPP(x)                     ((x & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_MPP)
#define LMAC154_FRAME_IS_MPP_LONG_FRAME(x)          ((x & LMAC154_FRAME_MPP_LONG_FRAME_BIT) == LMAC154_FRAME_MPP_LONG_FRAME_BIT)
#define LMAC154_FRAME_IS_MPP_SECURITY(x)            (LMAC154_FRAME_IS_MPP_LONG_FRAME(x) && ((x) & LMAC154_FRAME_MPP_SECURITY_BIT) == LMAC154_FRAME_MPP_SECURITY_BIT)

#define LMAC154_FRAME_IS_MPP_ACK_REQ(x)             (LMAC154_FRAME_IS_MPP(x) && LMAC154_FRAME_IS_MPP_LONG_FRAME(x) && ((x) & LMAC154_FRAME_MPP_ACK_REQ) == LMAC154_FRAME_MPP_ACK_REQ)
#define LMAC154_FRAME_IS_ENH_ACK_REQ(x)             (LMAC154_FRAME_IS_MPP_ACK_REQ(x) || (LMAC154_FRAME_IS_ACK_REQ(x) && LMAC154_FRAME_IS_FRAME_2015(x)))
#define LMAC154_FRAME_IS_IMM_ACK_REQ(x)             (LMAC154_FRAME_IS_ACK_REQ(x) && !LMAC154_FRAME_IS_FRAME_2015(x))

#define LMAC154_FRAME_IS_SECURITY_ENABLED(x)        (LMAC154_FRAME_IS_MPP(x) ?                                                          \
                                                        (LMAC154_FRAME_IS_MPP_SECURITY(x) : LMAC154_FRAME_IS_NORMAL_SECURITY(x))

#define LMAC154_FRAME_IS_ACK(x)                     (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_ACK)
#define LMAC154_FRAME_IS_IMM_ACK(x)                 (LMAC154_FRAME_IS_ACK(x) && !LMAC154_FRAME_IS_FRAME_2015(x))
#define LMAC154_FRAME_IS_ENH_ACK(x)                 (LMAC154_FRAME_IS_ACK(x) && LMAC154_FRAME_IS_FRAME_2015(x))
#define LMAC154_FRAME_IS_IE_EXIST(x)                (((x) & LMAC154_FRAME_IE_MASK) && LMAC154_FRAME_IS_FRAME_2015(x))

#define LMAC154_FRAME_GET_SRC_ADDR_TYPE(x)          ((x) & LMAC154_FRAME_ADDR_SRC_MASK)
#define LMAC154_FRAME_GET_DST_ADDR_TYPE(x)          ((x) & LMAC154_FRAME_ADDR_DEST_MASK)


#define LMAC154_FRAME_AUX_KEY_ID_MODE_MASK          (3 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_0             (0 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_1             (1 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_2             (2 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_3             (3 << 3)

#define LMAC154_LIFS                    40
#define LMAC154_SIFS                    12
#define LMAC154_AIFS                    LMAC154_SIFS
#define LMAC154_SIFS_MAX_FRAME_SIZE     18
#define LMAC154_TURNAROUND              12
#define LMAC154_PKT_MAX_LEN             127
#define LMAC154_PREAMBLE_LEN            8

#define LMAC154_US_PER_SYMBOL_BITS      4
#define LMAC154_US_PER_SYMBOL           (1 << LMAC154_US_PER_SYMBOL_BITS)
#define LMAC154_US_PER_SYMBOL_MASK      (LMAC154_US_PER_SYMBOL - 1)

/****************************************************************************//**
 * @brief  Initialize the hardware module
 *         Call this function first
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_init(void);


/****************************************************************************//**
 * @brief  Enable MAC 15.4 feature for 2015 version
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enable2015Feature(void);

/****************************************************************************//**
 * @brief  Disable MAC 15.4 feature for 2015 version
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disable2015Feature(void);

/****************************************************************************//**
 * @brief  Enable second stack for dual stack
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enable2ndStack(void);


/****************************************************************************//**
 * @brief  Disable second stack for dual stack, which is disable by default
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disable2ndStack(void);


/****************************************************************************//**
 * @brief  Check whether the second stack is enabled
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint32_t lmac154_is2ndStackEnabled(void);


/****************************************************************************//**
 * @brief  Get whether lmac154 is disabled
 *
 * @param  None
 *
 * @return 1 for disabled, 0 for enabled
 *
*******************************************************************************/
uint32_t lmac154_isDisabled(void);


/****************************************************************************//**
 * @brief  Monitor the hardware state
 *         Call this function periodically (1ms recommended) if needed
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_monitor(void);


/****************************************************************************//**
 * @brief  Get the lmac154 interrupt handler for interrupt registration of M154_IRQn
 *
 * @param  None
 *
 * @return The lmac154 interrupt handler
 *
*******************************************************************************/
lmac154_isr_t lmac154_getInterruptHandler(void);
lmac154_isr_t lmac154_get2015InterruptHandler(void);

/****************************************************************************//**
 * @brief  Get the version number
 *
 * @param  None
 *
 * @return The library version number
 *
*******************************************************************************/
uint32_t lmac154_getVersionNumber(void);

/****************************************************************************//**
 * @brief  Get the version number string
 *
 * @param  None
 *
 * @return The library version number string
 *
*******************************************************************************/
char * lmac154_getVersionString(void);

/****************************************************************************//**
 * @brief  Enable standard or enhanced rx promiscuous mode (default disabled)
 *         The MAC sublayer accepts all frames received from the PHY layer
 *         Able to receive frames without any IFS but rx sensitivity may be degraded if enhanced_mode = 1
 *         The MPDU is unavailable in lmac154_rxDoneEvent if ignore_mpdu = 1
 *
 * @param  enhanced_mode: 0: standard mode, 1: enhanced mode
 * @param  ignore_mpdu: 0: read mpdu from registers, 1: do not read mpdu from registers
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRxPromiscuousMode(uint8_t enhanced_mode, uint8_t ignore_mpdu);


/****************************************************************************//**
 * @brief  Disable standard or enhanced rx promiscuous mode (default disabled)
 *         The MAC sublayer only accepts frames that pass the address filtering
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRxPromiscuousMode(void);


/****************************************************************************//**
 * @brief  Get whether rx promiscuous mode is enabled
 *
 * @param  None
 *
 * @return 1 for enabled; 0 for disabled
 *
*******************************************************************************/
uint32_t lmac154_isRxPromiscuousModeEnabled(void);


/****************************************************************************//**
 * @brief  Enable RX (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRx(void);


/****************************************************************************//**
 * @brief  Disable RX (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRx(void);


/****************************************************************************//**
 * @brief  Set rx on/off state when idle state
 *
 * @param  isRxOnWhenIdle: true is rx on when idle; otherwhile is rx off when idle
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setRxStateWhenIdle(bool isRxOnWhenIdle);

/****************************************************************************//**
 * @brief  Get rx on/off state when idle state
 *
 * @param  None 
 *
 * @return true is rx on when idle; otherwhile is rx off when idle
 *
*******************************************************************************/
bool lmac154_isRxStateWhenIdle(void);

/****************************************************************************//**
 * @brief  Set the number of maximum retransmission times (default 0)
 *         Will perform retransmission in case ack is not received if num > 0
 *         Will raise lmac154_ackEvent(ack_received = 0) after maximum retransmission times
 *
 * @param  num: number of maximum retransmission times
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxRetry(uint32_t num);


/****************************************************************************//**
 * @brief  Trigger TX
 *
 * @param  DataPtr: pointer to data buffer
 * @param  length: data length in bytes
 * @param  csma: 0: without CSMA, 1: with CSMA
 *
 * @return None
 *
*******************************************************************************/
void lmac154_triggerTx(uint8_t *DataPtr, uint8_t length, uint8_t csma);


/****************************************************************************//**
 * @brief  Trigger TX with tx time specified
 *
 * @param  DataPtr: pointer to data buffer
 * @param  length: data length in bytes
 * @param  csma_cca: 0: without CSMA, 1: with CSMA/CCA
 * @param  baseTimeUs: the base time us for delay tx trigger
 * @param  delayUs: the delay time from baseTimeUs to tx trigger
 *
 * @return 0 is Success
 *
*******************************************************************************/
int lmac154_triggerTxDelay(uint8_t *DataPtr, uint32_t length, uint32_t csma_cca, 
    uint64_t baseTimeUs, uint32_t delayUs);

/****************************************************************************//**
 * @brief  File MPDU data after tx trigger requested when lmac154_triggerTx or 
 *         lmac154_triggerTxDelay without data specified.
 *
 * @param  DataPtr: pointer to data buffer
 * @param  length: data length in bytes
 *
 * @return 0 is Success
 *
*******************************************************************************/
uint32_t lmac154_postFillMPDU(uint8_t *DataPtr, uint32_t length);

/****************************************************************************//**
 * @brief  File MPDU data after tx trigger requested when lmac154_triggerTx or 
 *         lmac154_triggerTxDelay without data specified.
 *
 * @param  DataPtr: pointer to data buffer
 * @param  length: data length in bytes
 *
 * @return 1 is selected
 *
*******************************************************************************/
uint32_t lmac154_isTriggerTimeSelected(void);

/****************************************************************************//**
 * @brief  Run tx continuous modulation (PRBS15 pattern)
 *         Call lmac154_resetTx to stop
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_runTxCM(void);


/****************************************************************************//**
 * @brief  Run tx continuous wave (single tone)
 *         Call lmac154_resetTx to stop
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_runTxCW(void);


/****************************************************************************//**
 * @brief  Reset tx state machine
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_resetTx(void);

/****************************************************************************//**
 * @brief  Reset rx state machine
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_resetRx(void);

/****************************************************************************//**
 * @brief  Set the channel (default LMAC154_CHANNEL_11)
 *
 * @param  ch_ind: channel index ranging from LMAC154_CHANNEL_11 to LMAC154_CHANNEL_26
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setChannel(lmac154_channel_t ch_ind);


/****************************************************************************//**
 * @brief  Get the channel
 *
 * @param  None
 *
 * @return The channel index ranging from LMAC154_CHANNEL_11 to LMAC154_CHANNEL_26
 *
*******************************************************************************/
lmac154_channel_t lmac154_getChannel(void);


/****************************************************************************//**
 * @brief  Get RSSI in dBm
 *
 * @param  None
 *
 * @return RSSI in dBm
 *
*******************************************************************************/
int lmac154_getRSSI(void);


/****************************************************************************//**
 * @brief  Get LQI
 *
 * @param  None
 *
 * @return LQI
 *
*******************************************************************************/
int lmac154_getLQI(void);


/****************************************************************************//**
 * @brief  Get SFD correlation
 *
 * @param  None
 *
 * @return SFD correlation
 *
*******************************************************************************/
uint8_t lmac154_getSFDCorrelation(void);


/****************************************************************************//**
 * @brief  Get the frequency offset in Hz
 *
 * @param  None
 *
 * @return The frequency offset in Hz
 *
*******************************************************************************/
int lmac154_getFrequencyOffset(void);

/****************************************************************************//**
 * @brief  Set country code for tx power adjustment
 *
 * @param  country_code: country code, for example: CN
 *
 * @return true, if country code is valid.
 *
*******************************************************************************/
bool lmac154_setCountryCode(const char * country_code);

/****************************************************************************//**
 * @brief  Set tx power (no default value)
 *
 * @param  power_dbm: tx power ranging from LMAC154_TX_POWER_0dBm to LMAC154_TX_POWER_14dBm
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxPower(lmac154_tx_power_t power_dbm);

/****************************************************************************//**
 * @brief  Set tx power (no default value)
 *
 * @param  power_dbm: tx power ranging from LMAC154_TX_POWER_0dBm to LMAC154_TX_POWER_14dBm
 *
 * @param  ch_ind: channel index ranging from LMAC154_CHANNEL_11 to LMAC154_CHANNEL_26
 *
 * @param country_code: country code, for example: CN
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxPowerWithPowerLimit(lmac154_tx_power_t power_dbm, lmac154_channel_t ch_ind, const char *country_code);

/****************************************************************************//**
 * @brief  Get tx power
 *
 * @param  None
 *
 * @return tx power ranging from LMAC154_TX_POWER_0dBm to LMAC154_TX_POWER_14dBm
 *
*******************************************************************************/
lmac154_tx_power_t lmac154_getTxPower(void);


/****************************************************************************//**
 * @brief  Set tx data rate (default LMAC154_DATA_RATE_250K)
 *         The maximum tx mpdu length is 127 bytes for 250kbps tx data rate, and 255 bytes for others
 *
 * @param  rate: LMAC154_DATA_RATE_250K, LMAC154_DATA_RATE_500K, LMAC154_DATA_RATE_1M, or LMAC154_DATA_RATE_2M
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxDataRate(lmac154_data_rate_t rate);


/****************************************************************************//**
 * @brief  Enable auto rx data rate (default disabled)
 *         The maximum rx mpdu length is 127 bytes for 250kbps rx data rate, and 255 bytes for others
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableAutoRxDataRate(void);


/****************************************************************************//**
 * @brief  Disable auto rx data rate (default disabled)
 *         The rx data rate is 250kbps, and the maximum rx mpdu length is 127 bytes
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableAutoRxDataRate(void);


/****************************************************************************//**
 * @brief  Get rx data rate
 *
 * @param  None
 *
 * @return LMAC154_DATA_RATE_250K, LMAC154_DATA_RATE_500K, LMAC154_DATA_RATE_1M, or LMAC154_DATA_RATE_2M
 *
*******************************************************************************/
lmac154_data_rate_t lmac154_getRxDataRate(void);


/****************************************************************************//**
 * @brief  Set CCA mode (default LMAC154_CCA_MODE_ED_AND_CS)
 *
 * @param  mode: LMAC154_CCA_MODE_ED, LMAC154_CCA_MODE_CS, LMAC154_CCA_MODE_ED_AND_CS, or LMAC154_CCA_MODE_ED_OR_CS
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setCCAMode(lmac154_cca_mode_t mode);


/****************************************************************************//**
 * @brief  Set CCA ED threshold (default -71dBm)
 *
 * @param  threshold: ED threshold ranging from -122 to 5 in dBm
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setEDThreshold(int threshold);


/****************************************************************************//**
 * @brief  Get CCA ED threshold
 *
 * @param  None
 *
 * @return ED threshold
 *
*******************************************************************************/
int lmac154_getEDThreshold(void);


/****************************************************************************//**
 * @brief  Run CCA
 *
 * @param  rssi: rssi output ranging from -122 to 5 in dBm
 *
 * @return CCA result: 0: channel idle, 1: channel busy
 *
*******************************************************************************/
uint8_t lmac154_runCCA(int *rssi);


/****************************************************************************//**
 * @brief  Set PAN ID (default 0x4321)
 *
 * @param  pid: PAN ID
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setPanId(uint16_t pid);
void lmac154_set2ndPanId(uint16_t pid);


/****************************************************************************//**
 * @brief  Get PAN ID
 *
 * @param  None
 *
 * @return PAN ID
 *
*******************************************************************************/
uint16_t lmac154_getPanId(void);
uint16_t lmac154_get2ndPanId(void);


/****************************************************************************//**
 * @brief  Set 16-bit short address (default 0x0000)
 *
 * @param  sadr: 16-bit short address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setShortAddr(uint16_t sadr);
void lmac154_set2ndShortAddr(uint16_t sadr);


/****************************************************************************//**
 * @brief  Get 16-bit short address
 *
 * @param  None 
 *
 * @return 16-bit short address
 *
*******************************************************************************/
uint16_t lmac154_getShortAddr(void);
uint16_t lmac154_get2ndShortAddr(void);


/****************************************************************************//**
 * @brief  Set 64-bit long address (default 0xACDE4800_00000002)
 *
 * @param  ladr: pointer to 64-bit long address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setLongAddr(uint8_t *ladr);
void lmac154_set2ndLongAddr(uint8_t *ladr);


/****************************************************************************//**
 * @brief  Get 64-bit long address
 *
 * @param  ladr: pointer to 64-bit long address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_getLongAddr(uint8_t *ladr);
void lmac154_get2ndLongAddr(uint8_t *ladr);


/****************************************************************************//**
 * @brief  Enable frame type filtering (default disabled)
 *         Will only receive specified frame types
 *
 * @param  frame_types: any combination of LMAC154_FRAME_TYPE_XXX (BEACON, DATA, ACK, or CMD), via bitwise OR ('|') operation
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableFrameTypeFiltering(uint8_t frame_types);


/****************************************************************************//**
 * @brief  Disable frame type filtering (default disabled)
 *         Will receive all valid frame types (BEACON, DATA, ACK, or CMD)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableFrameTypeFiltering(void);


/****************************************************************************//**
 * @brief  Set rx reject policy
 *
 * @param policy : enum lmac154_rx_reject_policy_t
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setRxRejectPolicy(lmac154_rx_reject_policy_t policy);


/****************************************************************************//**
 * @brief  Get rx reject policy
 *
 * @param None
 *
 * @return Rx reject policy
 *
*******************************************************************************/
lmac154_rx_reject_policy_t lmac154_getRxRejectPolicy(void);


/****************************************************************************//**
 * @brief  set rx accept policy
 *         
 *
 * @param policy : enum lmac154_rx_accept_policy_t
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setRxAcceptPolicy(lmac154_rx_accept_policy_t policy);


/****************************************************************************//**
 * @brief  get rx accept policy
 *         
 *
 * @param None
 *
 * @return lmac154_rx_accept_policy : enum lmac154_rx_accept_policy_t
 *
*******************************************************************************/
lmac154_rx_accept_policy_t lmac154_getRxAcceptPolicy(void);


/****************************************************************************//**
 * @brief  Set external PA
 *         Restriction: txpin % 5 != rxpin % 5
 *
 * @param  txpin: tx pin ranging from 0 to 31
 * @param  rxpin: rx pin ranging from 0 to 31
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setExtPA(uint8_t txpin, uint8_t rxpin);


/****************************************************************************//**
 * @brief  Get RF state
 *
 * @param  None
 *
 * @return LMAC154_RF_STATE_RX, LMAC154_RF_STATE_RX_DOING, LMAC154_RF_STATE_TX, or LMAC154_RF_STATE_IDLE
 *
*******************************************************************************/
lmac154_rf_state_t lmac154_getRFState(void);


/****************************************************************************//**
 * @brief  Enable the coexistence of zigbee and other wireless modules (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableCoex(void);


/****************************************************************************//**
 * @brief  Disable the coexistence of zigbee and other wireless modules (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableCoex(void);

/****************************************************************************//**
 * @brief  Enable TX abort interrupt
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableTxAbortInt(void);

/****************************************************************************//**
 * @brief  Disable TX abort interrupt
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableTxAbortInt(void);

/****************************************************************************//**
 * @brief  Enable auto transmission of ack frame by hardware (default enabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableHwAutoTxAck(void);


/****************************************************************************//**
 * @brief  Disable auto transmission of ack frame by hardware (default enabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableHwAutoTxAck(void);

/****************************************************************************//**
 * @brief  Get whether hardware auto transmission of ack frame is enabled
 *
 * @param  None
 *
 * @return ture, enabled
 *
*******************************************************************************/
bool lmac154_isHwAutoTxAckEnabled(void);

/****************************************************************************//**
 * @brief  Enable lmac154_ackEvent (default enabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableAckEvent(void);


/****************************************************************************//**
 * @brief  Disable lmac154_ackEvent (default enabled)
 *         Will raise lmac154_rxDoneEvent instead when ack is received
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableAckEvent(void);


/****************************************************************************//**
 * @brief  Enable lmac154_rxStartEvent (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRxStartEvent(void);


/****************************************************************************//**
 * @brief  Disable lmac154_rxStartEvent (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRxStartEvent(void);


/****************************************************************************//**
 * @brief  Enable MAC header received event
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRxMHREvent(void);


/****************************************************************************//**
 * @brief  Disable MAC header received event
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRxMHREvent(void);


/****************************************************************************//**
 * @brief  Enable MAC Security header received event
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRxSecMHREvent(void);


/****************************************************************************//**
 * @brief  Disable MAC Security header received event
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRxSecMHREvent(void);


/****************************************************************************//**
 * @brief  Enable Req enh-ack event
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableReqEnhAckEvent(void);


/****************************************************************************//**
 * @brief  Disable Req enh-ack event
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableReqEnhAckEvent(void);

/****************************************************************************//**
 * @brief  Get the receiving or received mpdu length in bytes (crc included)
 *
 * @param  None
 *
 * @return The receiving or received mpdu length
 *
*******************************************************************************/
uint8_t lmac154_getRxLength(void);


/****************************************************************************//**
 * @brief  Read rx data (crc not included)
 *
 * @param  buf: pointer to data buffer
 * @param  offset: data offset ranging from 0 to 252 in bytes
 * @param  len: data length ranging from 1 to 253-offset in bytes
 *
 * @return None
 *
*******************************************************************************/
void lmac154_readRxData(uint8_t *buf, uint8_t offset, uint8_t len);


/****************************************************************************//**
 * @brief  Read rx crc
 *
 * @param  crc: byte array to carry rx crc
 *
 * @return None
 *
*******************************************************************************/
void lmac154_readRxCrc(uint8_t crc[2]);


/****************************************************************************//**
 * @brief  Set the maximum wait time for ack frame (default 864us)
 *
 * @param  time_us: maximum wait time
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setAckWaitTime(uint16_t time_us);

/****************************************************************************//**
 * @brief  Get the maximum wait time for imm-ack frame
 *
 * @param  None
 *
 * @return time_us: maximum wait time
 *
*******************************************************************************/
uint16_t lmac154_getAckWaitTime(void);

/****************************************************************************//**
 * @brief  Set the maximum wait time for enh-ack ack frame (default 1500us)
 *
 * @param  time_us: maximum wait time
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setEnhAckWaitTime(uint16_t time_us);

/****************************************************************************//**
 * @brief  Get the maximum wait time for enh-ack ack frame
 *
 * @param  None
 *
 * @return time_us: maximum wait time
 *
*******************************************************************************/
uint16_t lmac154_getEnhAckWaitTime(void);

/****************************************************************************//**
 * @brief  Set the maximum and minimum CSMA-CA backoff exponent
 *
 * @param  max_be: maximum BE ranging from 3 to 8, default 5
 * @param  min_be: minimum BE ranging from 0 to max_be, default 3
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setBE(uint8_t max_be, uint8_t min_be);

/****************************************************************************//**
 * @brief  Set maximum CSMA backoff
 *
 * @param  maxBackoff: maximum CSMA backoff
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setMaxCsmaBackoff(uint8_t maxBackoff);

/****************************************************************************//**
 * @brief  Set tx-rx transition time
 *
 * @param  time: us
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxRxTransTime(uint8_t timeInUs);

/****************************************************************************//**
 * @brief  Get event timestamps
 *
 * @param  type: event timestamp type
 *
 * @return timestamp
 *
*******************************************************************************/
uint64_t lmac154_getEventTimeUs(lmac154_eventTimeType_t type);

void lmac154_fptSetMaxNum(uint8_t shortAddrNum, uint8_t longAddrNum);

/****************************************************************************//**
 * @brief  Run AES CCM
 *
 * @param  mode: LMAC154_AES_MODE_ENCRYPT or LMAC154_AES_MODE_DECRYPT
 * @param  a_data: pointer to additional authentication data buffer
 * @param  a_len: length of additional authentication data in bytes
 * @param  nonce: pointer to 13-byte nonce buffer
 * @param  key: pointer to 4-word key buffer
 * @param  mic_len: LMAC154_MIC_LEN_4, LMAC154_MIC_LEN_8, LMAC154_MIC_LEN_12, or LMAC154_MIC_LEN_16
 * @param  mic: pointer to mic buffer
 * @param  input_data: pointer to input data buffer
 * @param  output_data: pointer to output data buffer
 * @param  len: length of input data in bytes
 *
 * @return The error code
 *
*******************************************************************************/
lmac154_aes_status_t lmac154_runAESCCM(lmac154_aes_mode_t mode, 
                                       const uint8_t *a_data, 
                                       uint8_t a_len, 
                                       const uint8_t *nonce, 
                                       const uint32_t *key, 
                                       lmac154_aes_mic_len_t mic_len, 
                                       uint32_t *mic, 
                                       const uint32_t *input_data, 
                                       uint32_t *output_data, 
                                       uint8_t len);




// hardware frame pending table (256 bytes)
// shared by short addresses and long addresses (supports up to 128 short addresses or 32 long addresses or mixed)
// used by hardware to set the frame pending bit of hw auto tx ack
// if the address is not found in the table (e.g. not set or removed), the frame pending bit of hw auto tx ack is 1


/****************************************************************************//**
 * @brief  Add (or update) the key-value pair {sadr: pending} to (in) the frame pending table
 *
 * @param  sadr: 16-bit short address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_NO_RESOURCE
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptSetShortAddrPending(uint16_t sadr, uint8_t pending);


/****************************************************************************//**
 * @brief  Add (or update) the key-value pair {ladr: pending} to (in) the frame pending table
 *
 * @param  ladr: pointer to 64-bit long address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_NO_RESOURCE
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptSetLongAddrPending(uint8_t *ladr, uint8_t pending);


/****************************************************************************//**
 * @brief  Get the corresponding frame pending bit of the short address in the frame pending table
 *
 * @param  sadr: 16-bit short address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptGetShortAddrPending(uint16_t sadr, uint8_t *pending);


/****************************************************************************//**
 * @brief  Get the corresponding frame pending bit of the long address in the frame pending table
 *
 * @param  ladr: pointer to 64-bit long address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptGetLongAddrPending(uint8_t *ladr, uint8_t *pending);


/****************************************************************************//**
 * @brief  Remove the key-value pair {sadr: pending} from the frame pending table
 *
 * @param  sadr: 16-bit short address
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptRemoveShortAddr(uint16_t sadr);


/****************************************************************************//**
 * @brief  Remove the key-value pair {ladr: pending} from the frame pending table
 *
 * @param  ladr: pointer to 64-bit long address
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptRemoveLongAddr(uint8_t *ladr);


/****************************************************************************//**
 * @brief  Get short address list in the frame pending table
 *
 * @param  list: pointer to list buffer
 * @param  entry_num: number of entries
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fpt_GetShortAddrList(void *list, uint8_t *entry_num);


/****************************************************************************//**
 * @brief  Get long address list in the frame pending table
 *
 * @param  list: pointer to list buffer
 * @param  entry_num: number of entries
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptGetLongAddrList(void *list, uint8_t *entry_num);


/****************************************************************************//**
 * @brief  Set frame pending bit according to frame pending table, or force to set to 1
 *
 * @param  force: 0: set according to frame pending table (default), 1: force to set to 1
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptForcePending(uint8_t force);


/****************************************************************************//**
 * @brief  Clear the frame pending table
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptClear(void);


/****************************************************************************//**
 * @brief  Print the frame pending table using specified print function
 *
 * @param  print_func: user specified print function
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptDump(int print_func(const char *fmt, ...));


// functions below are callback functions running in the interrupt context
// should be implemented by user if needed


/****************************************************************************//**
 * @brief  Tx Done Event
 *         Will be raised after calling lmac154_triggerTx
 *
 * @param  tx_status: LMAC154_TX_STATUS_XXX (TX_FINISHED, CSMA_FAILED, TX_ABORTED, or HW_ERROR)
 *
 * @return None
 *
*******************************************************************************/
void lmac154_txDoneEvent(lmac154_tx_status_t tx_status);


/****************************************************************************//**
 * @brief  Ack Event
 *         Will be raised after calling lmac154_triggerTx(AR bit = 1)
 *
 * @param  ack_received: 0: no ack, 1: ack received
 * @param  frame_pending: frame pending bit in ack frame
 * @param  seq_num: sequence number in ack frame
 *
 * @return None
 *
*******************************************************************************/
void lmac154_ackEvent(uint8_t ack_received, uint8_t frame_pending, uint8_t seq_num);


/****************************************************************************//**
 * @brief  Ack Event with ack frame
 *         Will be raised after calling lmac154_triggerTx(AR bit = 1)
 *
 * @param  ack_received: 0: no ack, 1: ack received
 * @param  rx_buf: pointer to the ack frame buffer
 * @param  len: ACK frame length
 *
 * @return None
 *
*******************************************************************************/
void lmac154_ackFrameEvent(uint8_t ack_received, uint8_t *rx_buf, uint8_t len);


/****************************************************************************//**
 * @brief  Rx Done Event
 *         Will be raised when a frame is received
 *
 * @param  rx_buf: pointer to rx buffer
 * @param  rx_len: received mpdu length in bytes (crc included)
 * @param  crc_fail: 0: crc pass, 1: crc fail
 *
 * @return None
 *
*******************************************************************************/
void lmac154_rxDoneEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t crc_fail);

/****************************************************************************//**
 * @brief  Rx Start Event
 *         Will be raised when rx on MAC sublayer starts
 *         lmac154_getRxLength is available because it is obtained from PHY layer
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_rxStartEvent(void);


/****************************************************************************//**
 * @brief  Hardware auto tx ack done event
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_hwAutoTxAckDoneEvent(void);


/****************************************************************************//**
 * @brief  A 2015 version unicast packet received and enh-ack is required to respond
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_reqEnhAckEvent(void);


/****************************************************************************//**
 * @brief  Rx Mac header Event
 *         Will be raised when a frame is received
 *
 * @param  rx_buf: pointer to rx buffer
 * @param  rx_len: received length when event occured
 * @param  pkt_len: the total lenght of receiving packet
 *
 * @return None
 *
*******************************************************************************/
void lmac154_rxMhrEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t pkt_len);


/****************************************************************************//**
 * @brief  Rx Mac header Event
 *         Will be raised when a frame is received
 *
 * @param  rx_buf: pointer to rx buffer
 * @param  rx_len: received length when event occured
 * @param  pkt_len: the total lenght of receiving packet
 *
 * @return None
 *
*******************************************************************************/
void lmac154_rxSecMhrEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t pkt_len);

#if CONFIG_LMAC154_LOG
void lmac154_log_init(void);
void lmac154_log(const char *format, ...);
void lmac154_logs_output(void);
#endif

/****************************************************************************//**
 * @brief  Parse IEEE 802.15.4 frame to get memory address of destination PAN ID 
 *         and address
 *
 * @param  a_pkt: pointer to IEEE 802.15.4 frame
 * @param  a_panid: pointer to the destination PAN ID in input frame, if present.
 * @param  a_sa: pointer to destination short address in input frame, if present.
 * @param  a_xa: pointer to destination extended address in input frame, 
 *               if present.
 * @return None
*******************************************************************************/
void lmac154_parseDestAddress(uint8_t * a_pkt, uint16_t ** a_panid, 
                              uint16_t ** a_sa, uint8_t ** a_xa);

/****************************************************************************//**
 * @brief  Parse IEEE 802.15.4 frame to get memory address of source PANID 
 *         and address
 *
 * @param  a_pkt: pointer to IEEE 802.15.4 frame
 * @param  a_panid: pointer to the source PAN ID in input frame, if present.
 * @param  a_sa: pointer to source short address in input frame, if present.
 * @param  a_xa: pointer to source extended address in input frame, if present.
 * 
 * @return None
*******************************************************************************/
void lmac154_parseSrcAddress(uint8_t * a_pkt, uint16_t **a_panid, 
                             uint16_t ** a_sa, uint8_t ** a_xa);

/****************************************************************************//**
 * @brief  Parse IEEE 802.15.4 frame to get destination PAN ID and address
 *
 * @param  a_pkt: pointer to IEEE 802.15.4 frame
 * @param  a_panid: pointer to save destination PAN ID from input frame
 * @param  a_sa: pointer to save destination short address from input frame, 
 *              if present.
 * @param  a_xa: pointer to save destination extended address from input frame, 
 *              if present.
 * 
 * @return lmac154_addr_info_t to indicate information of source address
*******************************************************************************/
lmac154_addr_info_t lmac154_getDestAddress(uint8_t * a_pkt, uint16_t * a_panid, 
                                           uint16_t * a_sa, uint8_t * a_xa);

/****************************************************************************//**
 * @brief  Parse IEEE 802.15.4 frame to get source PAN ID and address
 *
 * @param  a_pkt: pointer to IEEE 802.15.4 frame
 * @param  a_panid: pointer to save source PAN ID from input frame
 * @param  a_sa: pointer to save source short address from input frame, 
 *              if present.
 * @param  a_xa: pointer to save source extended address from input frame, 
 *              if present.
 * 
 * @return lmac154_addr_info_t to indicate information of source address
*******************************************************************************/
lmac154_addr_info_t lmac154_getSrcAddress(uint8_t * a_pkt, uint16_t * a_panid, 
                                          uint16_t * a_sa, uint8_t * a_xa);

#endif
