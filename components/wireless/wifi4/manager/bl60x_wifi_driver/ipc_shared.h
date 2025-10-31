
/**
 ****************************************************************************************
 *
 * @file ipc_shared.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef _IPC_SHARED_H_
#define _IPC_SHARED_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <bl60x_fw_api.h>
#include "ipc_compat.h"
#include "lmac_types.h"
#include "lmac_mac.h"
#include "utils_list.h"

/*
 * DEFINES AND MACROS
 ****************************************************************************************
 */
#define CO_BIT(pos) (1U<<(pos)) 
#define NX_TXQ_CNT          4
#define CONFIG_USER_MAX     1

#define IPC_TXQUEUE_CNT     NX_TXQ_CNT
#define NX_TXDESC_CNT0      CFG_TXDESC
#define NX_TXDESC_CNT1      CFG_TXDESC
#define NX_TXDESC_CNT2      CFG_TXDESC
#define NX_TXDESC_CNT3      CFG_TXDESC
#define NX_TXDESC_CNT4      CFG_TXDESC
/*
 * Number of Host buffers available for Data Rx handling (through DMA)
 */
#define IPC_RXBUF_CNT       2

/*
 * Number of shared descriptors available for Data RX handling
 */
#define IPC_RXDESC_CNT      2

/*
 * Number of Host buffers available for Radar events handling (through DMA)
 */
#define IPC_RADARBUF_CNT       4

/*
 * RX Data buffers size (in bytes)
 */
#define IPC_RXBUF_SIZE 2048

/*
 * Number of Host buffers available for Emb->App MSGs sending (through DMA)
 */
#define IPC_MSGE2A_BUF_CNT       8

/*
 * Number of Host buffers available for Debug Messages sending (through DMA)
 */
#define IPC_DBGBUF_CNT       4

/*
 * Length used in MSGs structures
 */
#define IPC_A2E_MSG_BUF_SIZE    127 // size in 4-byte words
#define IPC_E2A_MSG_PARAM_SIZE   (257 - 4 - 8)// size in 4-byte words
/*
 * Debug messages buffers size (in bytes)
 */
#define IPC_DBG_PARAM_SIZE       256

/*
 * Define used for Rx hostbuf validity.
 * This value should appear only when hostbuf was used for a Reception.
 */
#define RX_DMA_OVER_PATTERN 0xAAAAAA00

/*
 * Define used for MSG buffers validity.
 * This value will be written only when a MSG buffer is used for sending from Emb to App.
 */
#define IPC_MSGE2A_VALID_PATTERN 0xADDEDE2A

/*
 * Define used for Debug messages buffers validity.
 * This value will be written only when a DBG buffer is used for sending from Emb to App.
 */
#define IPC_DBG_VALID_PATTERN 0x000CACA0

/*
 *  Length of the receive vectors, in bytes
 */
#define DMA_HDR_PHYVECT_LEN    36

/*
 * Maximum number of payload addresses and lengths present in the descriptor
 */
#define NX_TX_PAYLOAD_MAX      6

/*
 ****************************************************************************************
 */
// c.f LMAC/src/tx/tx_swdesc.h
/// Descriptor filled by the Host
struct hostdesc
{
    /// allocated pbuf
    uint32_t pbuf_addr;
    /// Pointer to packet payload
    uint32_t packet_addr;
    /// Size of the payload
    uint16_t packet_len;

    /// Address of the status descriptor in host memory (used for confirmation upload)
    uint32_t status_addr;
    /// Destination Address
    struct mac_addr eth_dest_addr;
    /// Source Address
    struct mac_addr eth_src_addr;
    /// Ethernet Type
    uint16_t ethertype;
    /// Buffer containing the PN to be used for this packet
    uint16_t pn[4];
    /// Sequence Number used for transmission of this MPDU
    uint16_t sn;
    /// Timestamp of first transmission of this MPDU
    uint16_t timestamp;
    /// Packet TID (0xFF if not a QoS frame)
    uint8_t tid;
    /// Interface Id
    uint8_t vif_idx;
    /// VIF type
    uint8_t vif_type; // 0: STA, 1: AP, [2-FF]:Other
    /// Station Id (0xFF if station is unknown)
    uint8_t staid;
    /// TX flags
    uint16_t flags;
    uint32_t pbuf_chained_ptr[4]; //max 4 chained pbuf for one output ethernet packet
    uint32_t pbuf_chained_len[4]; //max 4 chained pbuf for one output ethernet packet
    /// Time when TX desc is postponed 
    uint32_t postpone_time;
};

/// structure of a list element header
struct co_list_hdr
{
    /// Pointer to the next element in the list
    struct co_list_hdr *next;
};

/// structure of a list
struct co_list
{
    /// pointer to first element of the list
    struct co_list_hdr *first;
    /// pointer to the last element
    struct co_list_hdr *last;
};

/// upper part of struct txdesc
struct txdesc_upper
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// Information provided by Host
    struct hostdesc host;
};

struct txdesc_host
{
    struct utils_list_hdr list_hdr;

    void *host_id;

    uint32_t ready;

    uint32_t pad_txdesc[208/4];

    uint32_t pad_buf[400/4];
};

/// Structure containing the information about the PHY channel that is used
struct phy_channel_info
{
    /// PHY channel information 1
    uint32_t info1;
    /// PHY channel information 2
    uint32_t info2;
};

/**
 ****************************************************************************************
 *  @defgroup IPC_MISC IPC Misc
 *  @ingroup IPC
 *  @brief IPC miscellaneous functions
 ****************************************************************************************
 */
/// Message structure for MSGs from Emb to App
struct ipc_e2a_msg
{
    ke_msg_id_t id;                ///< Message id.
    ke_task_id_t dummy_dest_id;                ///<
    ke_task_id_t dummy_src_id;                ///<
    uint32_t param_len;         ///< Parameter embedded struct length.
    uint32_t param[IPC_E2A_MSG_PARAM_SIZE];  ///< Parameter embedded struct. Must be word-aligned.
    uint32_t pattern;           ///< Used to stamp a valid MSG buffer
};

/// Message structure for MSGs from App to Emb.
/// Actually a sub-structure will be used when filling the messages.
struct ipc_a2e_msg
{
    uint32_t dummy_word;                // used to cope with kernel message structure
    uint32_t msg[IPC_A2E_MSG_BUF_SIZE]; // body of the msg
};

// Indexes are defined in the MIB shared structure

#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
struct txbuf_host
{
    uint32_t flag;
    uint32_t buf[1600/4];
};
#endif

struct ipc_shared_env_tag
{
    volatile struct ipc_a2e_msg msg_a2e_buf; // room for MSG to be sent from App to Emb

    /// Host buffer address for the TX payload descriptor pattern
    volatile uint32_t  pattern_addr;

#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
    /// Array of TX buffer
    struct txbuf_host txbuf[NX_TXDESC_CNT0];
#endif

    /// Array of TX descriptors for the BK queue
    volatile struct txdesc_host txdesc0[NX_TXDESC_CNT0];

    /// List of free txdesc
    struct utils_list list_free;
    /// List of ongoing txdesc
    struct utils_list list_ongoing;
    /// List of cfm txdesc
    struct utils_list list_cfm;
};

extern struct ipc_shared_env_tag ipc_shared_env;


/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */

/// IPC TX descriptor interrupt mask
#define IPC_IRQ_A2E_TXDESC          0xFF00

#define IPC_IRQ_A2E_TXDESC_FIRSTBIT (8)
#define IPC_IRQ_A2E_RXBUF_BACK      CO_BIT(5)
#define IPC_IRQ_A2E_RXDESC_BACK     CO_BIT(4)

#define IPC_IRQ_A2E_MSG             CO_BIT(1)
#define IPC_IRQ_A2E_DBG             CO_BIT(0)

#define IPC_IRQ_A2E_ALL             (IPC_IRQ_A2E_TXDESC|IPC_IRQ_A2E_MSG|IPC_IRQ_A2E_DBG)

// IRQs from emb to app
#define IPC_IRQ_E2A_TXCFM_POS   7

#define IPC_IRQ_E2A_TXCFM       ((1 << NX_TXQ_CNT) - 1 ) << IPC_IRQ_E2A_TXCFM_POS

#define IPC_IRQ_E2A_RADAR       CO_BIT(6)
#define IPC_IRQ_E2A_TBTT_SEC    CO_BIT(5)
#define IPC_IRQ_E2A_TBTT_PRIM   CO_BIT(4)
#define IPC_IRQ_E2A_RXDESC      CO_BIT(3)
#define IPC_IRQ_E2A_MSG_ACK     CO_BIT(2)
#define IPC_IRQ_E2A_MSG         CO_BIT(1)
#define IPC_IRQ_E2A_DBG         CO_BIT(0)

#define IPC_IRQ_E2A_ALL         ( IPC_IRQ_E2A_TXCFM         \
                                | IPC_IRQ_E2A_RXDESC        \
                                | IPC_IRQ_E2A_MSG_ACK       \
                                | IPC_IRQ_E2A_MSG           \
                                | IPC_IRQ_E2A_DBG           \
                                | IPC_IRQ_E2A_TBTT_PRIM     \
                                | IPC_IRQ_E2A_TBTT_SEC      \
                                | IPC_IRQ_E2A_RADAR)
#endif // _IPC_SHARED_H_
