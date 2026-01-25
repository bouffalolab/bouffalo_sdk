/**
 ****************************************************************************************
 *
 * @file fhost_rx_def.h
 *
 * @brief Definitions of the fully hosted RX task.
 *
 *
 ****************************************************************************************
 */

#ifndef _FHOST_RX_DEF_H_
#define _FHOST_RX_DEF_H_

/**
 ****************************************************************************************
 * @addtogroup FHOST_RX
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "fhost.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// Number of elements in the RX descriptor queue
#define FHOST_RX_QUEUE_DESC_ELT_CNT (FHOST_RX_BUF_CNT * 2)
/// Size of a RX buffer
#define FHOST_RX_BUF_SIZE           (RX_MAX_AMSDU_SUBFRAME_LEN + 1)

/// Structure containing the information about the payload that should be resend
struct tx_info
{
    /// Network stack buffer element
    net_al_tx_t net_buf;
};

/// FHOST RX environment structure
struct fhost_rx_buf_tag
{
    /// Network stack buffer element - Must be the first field in the structure because of the implicit cast
    /// by net_al when calling @ref fhost_rx_buf_push (via free_fn) to free a net_al_rx_t!!!
    // net_al_rx_t net_buf;
    uint8_t fnet_buf[64];
    /// Structure containing the information to resend on Tx
    struct tx_info info_tx;
    /// Number of reference to Rx buffer
    uint8_t ref;
    /// Structure containing the information about the received payload - Payload must be just after !!!
    struct rx_info info;
    /// Payload buffer space - must be after rx_info  !!!
    uint32_t payload[CO_ALIGN4_HI(FHOST_RX_BUF_SIZE)/sizeof(uint32_t)];

};
/// @}

#endif // _FHOST_RX_DEF_H_

