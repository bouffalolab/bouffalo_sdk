#ifndef _BFLB_GMAC_H
#define _BFLB_GMAC_H

#include "bflb_core.h"
#include "hardware/gmac_reg.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup GMAC
  * @{
  */

#define GMAC_DO_FLUSH_DATA           (1)
#define GMAC_IPC_OFFLOAD             (1)

/** @defgroup GMAC_CMD gmac feature control cmd definition
  * @{
  */
#define GMAC_CMD_NO_PREAMBLE_MODE    (0x01)
#define GMAC_CMD_EN_PROMISCUOUS      (0x02)
#define GMAC_CMD_FRAME_GAP_CHECK     (0x03)
#define GMAC_CMD_FULL_DUPLEX         (0x04)
#define GMAC_CMD_EN_TX_CRC_FIELD     (0x05)
#define GMAC_CMD_RECV_HUGE_FRAMES    (0x06)
#define GMAC_CMD_EN_AUTO_PADDING     (0x07)
#define GMAC_CMD_RECV_SMALL_FRAME    (0x08)
#define GMAC_CMD_SET_PHY_ADDRESS     (0x09)
#define GMAC_CMD_SET_MAC_ADDRESS     (0x0A)
#define GMAC_CMD_SET_PACKET_GAP      (0x0B)
#define GMAC_CMD_SET_MIN_FRAME       (0x0C)
#define GMAC_CMD_SET_MAX_FRAME       (0x0D)
#define GMAC_CMD_SET_MAXRET          (0x0E)
#define GMAC_CMD_SET_COLLVALID       (0x0F)
#define GMAC_CMD_SET_SPEED_MODE      (0x10)
#define GMAC_CMD_SET_SMI_DIV         (0x11)
/**
  * @}
  */

/** @defgroup PHY_STATE phy state definition
  * @{
  */
#define PHY_STATE_DOWN               (0) /* PHY is not usable */
#define PHY_STATE_READY              (1) /* PHY is OK, wait for controller */
#define PHY_STATE_UP                 (2) /* Network is ready for TX/RX */
#define PHY_STATE_RUNNING            (3) /* working */
#define PHY_STATE_NOLINK             (4) /* no cable connected */
#define PHY_STATE_STOPPED            (5) /* PHY has been stopped */
#define PHY_STATE_TESTING            (6) /* in test mode */
/**
  * @}
  */

/* GMAC PACKET */
#define GMAC_NORMAL_PACKET           (uint32_t)(0)
#define GMAC_FRAGMENT_PACKET         (uint32_t)(0x01)
#define GMAC_NOCOPY_PACKET           (uint32_t)(0x02)

/* ETH packet size */
/* ETH     | Header | Extra | VLAN tag | Payload   | CRC | */
/* Size    | 14     | 2     | 4        | 46 ~ 1500 | 4   | */
#define ETH_MAX_PACKET_SIZE          ((uint32_t)1524U) /*!< ETH_HEADER + ETH_EXTRA + ETH_VLAN_TAG + ETH_MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER_SZIE              ((uint32_t)14U)   /*!< 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC_SIZE                 ((uint32_t)4U)    /*!< Ethernet CRC */
#define ETH_EXTRA_SIZE               ((uint32_t)2U)    /*!< Extra bytes in some cases */
#define ETH_VLAN_TAG_SIZE            ((uint32_t)4U)    /*!< optional 802.1q VLAN Tag */
#define ETH_MIN_ETH_PAYLOAD_SIZE     ((uint32_t)46U)   /*!< Minimum Ethernet payload size */
#define ETH_MAX_ETH_PAYLOAD_SIZE     ((uint32_t)1500U) /*!< Maximum Ethernet payload size */
#define ETH_JUMBO_FRAME_PAYLOAD_SIZE ((uint32_t)9000U) /*!< Jumbo frame payload size */

/* ETH tx & rx buffer size */
#ifndef ETH_TX_BUFFER_SIZE
#define ETH_TX_BUFFER_SIZE (ETH_MAX_PACKET_SIZE)
#endif
#ifndef ETH_RX_BUFFER_SIZE
#define ETH_RX_BUFFER_SIZE (ETH_MAX_PACKET_SIZE)
#endif

#define GAMC_HALFDUPLEX    0
#define GAMC_FULLDUPLEX    1
#define GMAC_PASS_CONTROL0 0
#define GMAC_PASS_CONTROL1 1
#define GMAC_PASS_CONTROL2 2
#define GMAC_PASS_CONTROL3 3
#define GMAC_SPEED_10      10
#define GMAC_SPEED_100     100
#define GMAC_SPEED_1000    1000

/**
 * @brief GMAC configuration structure
 *
 * @param mac_addr  GMAC mac addr
 * @param mii_clk_div mii clock div
 * @param duplex_mode  full or half duplex mode
 * @param speed 10,100 or 1000
 * @param min_frame_len min frame len
 * @param max_frame_len max frame len
 *
 */
struct bflb_gmac_config_s {
    uint8_t mac_addr[6];
    bool clk_internal_mode;
    uint8_t mii_clk_div;
    uint8_t duplex_mode;
    uint16_t speed;
    uint16_t min_frame_len;
    uint16_t max_frame_len;
};

/**
 * @brief GMAC DMA description structure
 *
 * @param mac_addr  GMAC mac addr
 * @param mii_clk_div mii clock div
 * @param duplex_mode  full or half duplex mode
 * @param speed 10,100 or 1000
 * @param min_frame_len min frame len
 * @param max_frame_len max frame len
 *
 */
struct bflb_gmac_dma_desc_s {
    uint32_t status;  /* Status  */
    uint32_t length;  /* Buffer 1  and Buffer 2 length */
    uint32_t buffer1; /* Network Buffer 1 pointer (DMA-able) */
    uint32_t buffer2; /* Network Buffer 2 pointer or next descriptor pointer (DMA-able)in chain structure  */
    /* This data below is used only by driver */
    uint32_t extstatus;     /* Extended status of a Rx Descriptor  */
    uint32_t reserved1;     /* Reserved word */
    uint32_t timestamplow;  /* Lower 32 bits of the 64 bit timestamp value */
    uint32_t timestamphigh; /* Higher 32 bits of the 64 bit timestamp value  */
    uint32_t data1;         /* This holds virtual address of buffer1, not used by DMA  */
    uint32_t data2;         /* This holds virtual address of buffer2, not used by DMA  */
};

/*
**********DMA descriptor struct ****************************
DMA_Rx_Base_Addr     = 0x000C,
DMA_Rx_Base_Addr is the pointer to the first Rx Descriptors. 
DMA_Tx_Base_Addr     = 0x0010,
DMA_Tx_Base_Addr is the pointer to the first Tx Descriptors. 
the Descriptor format in Little endian with a 32 bit Data bus is as shown below
---------------------------------------------------------------------------------
RDES0   |OWN (31)| Status                                                        |
---------------------------------------------------------------------------------
RDES1   | Ctrl | Res | Byte Count Buffer 2 | Ctrl | Res | Byte Count Buffer 1    |
---------------------------------------------------------------------------------
RDES2   |  Buffer 1 Address                                                      |
---------------------------------------------------------------------------------
RDES3   |  Buffer 2 Address / Next Descriptor Address                            |
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
TDES0   |OWN (31)| Ctrl | Res | Ctrl | Res | Status                              |
---------------------------------------------------------------------------------
TDES1   | Res | Byte Count Buffer 2 | Res |         Byte Count Buffer 1          |
---------------------------------------------------------------------------------
TDES2   |  Buffer 1 Address                                                      |
---------------------------------------------------------------------------------
TDES3   |  Buffer 2 Address / Next Descriptor Address                            |
---------------------------------------------------------------------------------
*/

/* status word of DMA descriptor */
#define GMAC_DESC_OWN_BY_DMA           0x80000000 /* (OWN)Descriptor is owned by DMA engine              31      RW   */
#define GMAC_DESC_DA_FILTER_FAIL       0x40000000 /* (AFM)Rx - DA Filter Fail for the rx frame           30           */
#define GMAC_DESC_FRAME_LEN_MASK       0x3FFF0000 /* (FL)Receive descriptor frame length                 29:16        */
#define GMAC_DESC_FRAME_LEN_SHIFT      16
#define GMAC_DESC_ERROR                0x00008000 /* (ES)Error summary bit  - OR of the follo. bits:     15           */
                                                  /*  DE || OE || IPC || LC || RWT || RE || CE */
#define GMAC_DESC_RX_TRUNCATED         0x00004000 /* (DE)Rx - no more descriptors for receive frame      14           */
#define GMAC_DESC_SA_FILTER_FAIL       0x00002000 /* (SAF)Rx - SA Filter Fail for the received frame     13           */
#define GMAC_DESC_RX_LEN_ERROR         0x00001000 /* (LE)Rx - frm size not matching with len field       12           */
#define GMAC_DESC_RX_DAMAGED           0x00000800 /* (OE)Rx - frm was damaged due to buffer overflow     11           */
#define GMAC_DESC_RX_VLAN_TAG          0x00000400 /* (VLAN)Rx - received frame is a VLAN frame           10           */
#define GMAC_DESC_RX_FIRST             0x00000200 /* (FS)Rx - first descriptor of the frame              9            */
#define GMAC_DESC_RX_LAST              0x00000100 /* (LS)Rx - last descriptor of the frame               8            */
//#define GMAC_DESC_RX_LONG_FRAME   0x00000080    /* (Giant Frame)Rx - frame is longer than 1518/1522    7            */
#define GMAC_DESC_RX_TS_AVAIL          0x00000080 /* Time stamp available                                7            */
#define GMAC_DESC_RX_COLLISION         0x00000040 /* (LC)Rx - late collision occurred during reception   6            */
#define GMAC_DESC_RX_FRAME_ETHER       0x00000020 /* (FT)Rx - Frame type - Ethernet, otherwise 802.3     5            */
#define GMAC_DESC_RX_WATCHDOG          0x00000010 /* (RWT)Rx - watchdog timer expired during reception   4            */
#define GMAC_DESC_RX_MII_ERROR         0x00000008 /* (RE)Rx - error reported by MII interface            3            */
#define GMAC_DESC_RX_DRIBBLING         0x00000004 /* (DE)Rx - frame contains non int multiple of 8 bits  2            */
#define GMAC_DESC_RX_CRC               0x00000002 /* (CE)Rx - CRC error                                  1            */
#define GMAC_DESC_RX_EXT_STS           0x00000001 /* Extended Status Available (RDES4)                   0            */

#define GMAC_DESC_RX_CHK_Bit0          0x00000001 /*()  Rx - Rx Payload Checksum Error                   0            */
#define GMAC_DESC_RX_CHK_Bit7          0x00000080 /* (IPC CS ERROR)Rx - Ipv4 header checksum error       7            */
#define GMAC_DESC_RX_CHK_Bit5          0x00000020 /* (FT)Rx - Frame type - Ethernet, otherwise 802.3     5            */

#define GMAC_DESC_TX_INT_ENABLE        0x40000000 /* (IC)Tx - interrupt on completion                    30                 */
#define GMAC_DESC_TX_LAST              0x20000000 /* (LS)Tx - Last segment of the frame                  29                 */
#define GMAC_DESC_TX_FIRST             0x10000000 /* (FS)Tx - First segment of the frame                 28                 */
#define GMAC_DESC_TX_DISABLE_CRC       0x08000000 /* (DC)Tx - Add CRC disabled (first segment only)      27                 */
#define GMAC_DESC_TX_DISABLE_PADD      0x04000000 /* (DP)disable padding, added by - reyaz               26                 */
#define GMAC_DESC_TX_TS_ENABLE         0x02000000 /* Enable TX timestamp update when tx done             25                 */
#define GMAC_DESC_TX_CIS_MASK          0x00c00000 /* Tx checksum offloading control mask                 23:22              */
#define GMAC_DESC_TX_CIS_BYPASS        0x00000000 /* Checksum bypass                                                        */
#define GMAC_DESC_TX_CIS_IPV4_HDR_CS   0x00400000 /* IPv4 header checksum                                                   */
#define GMAC_DESC_TX_CIS_TCP_ONLY_CS   0x00800000 /* TCP/UDP/ICMP checksum. Pseudo header checksum is assumed to be present */
#define GMAC_DESC_TX_CIS_TCP_PSEUDO_CS 0x00c00000 /* TCP/UDP/ICMP checksum fully in hardware including pseudo header        */
#define GMAC_DESC_TX_END_OF_RING       0x00200000 /* (TER)End of descriptors ring                        21                 */
#define GMAC_DESC_TX_DESC_CHAIN        0x00100000 /* (TCH)Second buffer address is chain address         20                 */
#define GMAC_DESC_TX_IPV4_CHK_ERROR    0x00010000 /* (IHE) Tx Ip header error                            16                 */
#define GMAC_DESC_TX_TIMEOUT           0x00004000 /* (JT)Tx - Transmit jabber timeout                    14                 */
#define GMAC_DESC_TX_FRAME_FLUSHED     0x00002000 /* (FF)Tx - DMA/MTL flushed the frame due to SW flush  13                 */
#define GMAC_DESC_TX_PAY_CHK_ERROR     0x00001000 /* (PCE) Tx Payload checksum Error                     12                 */
#define GMAC_DESC_TX_LOST_CARRIER      0x00000800 /* (LC)Tx - carrier lost during tramsmission           11                 */
#define GMAC_DESC_TX_NO_CARRIER        0x00000400 /* (NC)Tx - no carrier signal from the tranceiver      10                 */
#define GMAC_DESC_TX_LATE_COLLISION    0x00000200 /* (LC)Tx - transmission aborted due to collision      9                  */
#define GMAC_DESC_TX_EXC_COLLISIONS    0x00000100 /* (EC)Tx - transmission aborted after 16 collisions   8                  */
#define GMAC_DESC_TX_VLAN_FRAME        0x00000080 /* (VF)Tx - VLAN-type frame                            7                  */
#define GMAC_DESC_TX_COLL_MASK         0x00000078 /* (CC)Tx - Collision count                            6:3                */
#define GMAC_DESC_TX_COLL_SHIFT        3
#define GMAC_DESC_TX_EXC_DEFERRAL      0x00000004 /* (ED)Tx - excessive deferral                         2                  */
#define GMAC_DESC_TX_UNDERFLOW         0x00000002 /* (UF)Tx - late data arrival from the memory          1                  */
#define GMAC_DESC_TX_DEFERRED          0x00000001 /* (DB)Tx - frame transmision deferred                 0                  */

/* length word of DMA descriptor 
---------------------------------------------------------------------------------
RDES1/TDES1  | Control Bits | Byte Count Buffer 2 | Byte Count Buffer 1          |
---------------------------------------------------------------------------------
*/
#define GMAC_DESC_RX_DIS_INT_COMPL     0x80000000 /* (Disable Rx int on completion)                     31                  */
#define GMAC_DESC_RX_END_OF_RING       0x00008000 /* (TER)End of descriptors ring                       15                  */
#define GMAC_DESC_RX_DESC_CHAIN        0x00004000 /* (TCH)Second buffer address is chain address        14                  */
#define GMAC_DESC_SIZE2_MASK           0x1FFF0000 /* (TBS2) Buffer 2 size                               28:16               */
#define GMAC_DESC_SIZE2_SHIFT          16
#define GMAC_DESC_SIZE1_MASK           0x00001FFF /* (TBS1) Buffer 1 size                               12:0                */
#define GMAC_DESC_SIZE1_SHIFT          0

/* extended Status word of DMA descriptor */
#ifdef GMAC_AVB_SUPPORT
#define GMAC_EDESC_RX_VLAN_PRIO_VAL    0x001C0000 /* Gives the VLAN Priority Value                                                      20:18 */
#define GMAC_EDESC_RX_VLAN_PRIO_SH_VAL 18         /* Gives the shift value to get priority value in LS bits                                   */
#define GMAC_EDESC_RX_AV_TAG_PKT_RX    0x00020000 /* Indicates AV tagged Packet is received                                             17    */
#define GMAC_EDESC_RX_AV_PKT_RX        0x00010000 /* Indicates AV Packet received                                                       16    */
#endif

#define GMAC_EDESC_RX_PTP_AVAIL          0x00004000 /* PTP snapshot available                                                           14    */
#define GMAC_EDESC_RX_PTP_VER            0x00002000 /* When set indicates IEEE1584 Version 2 (else Ver1)                                13    */
#define GMAC_EDESC_RX_PTP_FRAME_TYPE     0x00001000 /* PTP frame type Indicates PTP sent over ethernet                                  12    */
#define GMAC_EDESC_RX_PTP_MESSAGE_TYPE   0x00000F00 /* Message Type                                                                     11:8  */
#define GMAC_EDESC_RX_PTP_NO             0x00000000 /* 0000 => No PTP message received                                                        */
#define GMAC_EDESC_RX_PTP_SYNC           0x00000100 /* 0001 => Sync (all clock types) received                                                */
#define GMAC_EDESC_RX_PTP_FOLLOW_UP      0x00000200 /* 0010 => Follow_Up (all clock types) received                                           */
#define GMAC_EDESC_RX_PTP_DELAY_REQ      0x00000300 /* 0011 => Delay_Req (all clock types) received                                           */
#define GMAC_EDESC_RX_PTP_DELAY_RESP     0x00000400 /* 0100 => Delay_Resp (all clock types) received                                          */
#define GMAC_EDESC_RX_PTP_PDELAY_REQ     0x00000500 /* 0101 => Pdelay_Req (in P to P tras clk)  or Announce in Ord and Bound clk              */
#define GMAC_EDESC_RX_PTP_PDELAY_RESP    0x00000600 /* 0110 => Pdealy_Resp(in P to P trans clk) or Management in Ord and Bound clk            */
#define GMAC_EDESC_RX_PTP_PDELAY_RESP_FP 0x00000700 /* 0111 => Pdealy_Resp_Follow_Up (in P to P trans clk) or Signaling in Ord and Bound clk  */
#define GMAC_EDESC_RX_PTP_IPV6           0x00000080 /* Received Packet is  in IPV6 Packet                                               7     */
#define GMAC_EDESC_RX_PTP_IPV4           0x00000040 /* Received Packet is  in IPV4 Packet                                               6     */
#define GMAC_EDESC_RX_CHKSUM_BYPASS      0x00000020 /* When set indicates checksum offload engine is bypassed                           5     */
#define GMAC_EDESC_RX_IP_PAYLOAD_ERROR   0x00000010 /* When set indicates 16bit IP payload CS is in error                               4     */
#define GMAC_EDESC_RX_IP_HEADER_ERROR    0x00000008 /* When set indicates 16bit IPV4 header CS is in                                    3     */
                                                    /* error or IP datagram version is not consistent with Ethernet type value                */
#define GMAC_EDESC_RX_IP_PAYLOAD_TYPE    0x00000007 /* Indicate the type of payload encapsulated in IPdatagram processed by COE (Rx)    2:0   */
#define GMAC_EDESC_RX_IP_PAYLOAD_UNKNOWN 0x00000000 /* Unknown or didnot process IP payload                                                   */
#define GMAC_EDESC_RX_IP_PAYLOAD_UDP     0x00000001 /* UDP                                                                                    */
#define GMAC_EDESC_RX_IP_PAYLOAD_TCP     0x00000002 /* TCP                                                                                    */
#define GMAC_EDESC_RX_IP_PAYLOAD_ICMP    0x00000003 /* ICMP                                                                                   */

#define GMAC_DMA_INT_API_RX_NORMAL       0x01 /* normal receiver interrupt */
#define GMAC_DMA_INT_API_RX_ABNORMAL     0x02 /* abnormal receiver interrupt */
#define GMAC_DMA_INT_API_RX_STOPPED      0x04 /* receiver stopped */
#define GMAC_DMA_INT_API_TX_NORMAL       0x08 /* normal transmitter interrupt */
#define GMAC_DMA_INT_API_TX_ABNORMAL     0x10 /* abnormal transmitter interrupt */
#define GMAC_DMA_INT_API_TX_STOPPED      0x20 /* transmitter stopped */
#define GMAC_DMA_INT_API_ERROR           0x80 /* Dma engine error */
#ifdef AVB_SUPPORT
#define GMAC_DMA_INT_API_SLOT_COUNTER 0x40 /* Dma SlotCounter interrupt mask for Channel1 and Channel2*/
#endif

#define bflb_gmac_is_desc_owned_by_dma(desc)              ((desc->status & GMAC_DESC_OWN_BY_DMA) == GMAC_DESC_OWN_BY_DMA)
#define bflb_gmac_get_rx_desc_frame_length(status)        ((status & GMAC_DESC_FRAME_LEN_MASK) >> GMAC_DESC_FRAME_LEN_SHIFT)
#define bflb_gmac_is_desc_valid(status)                   ((status & GMAC_DESC_ERROR) == 0)
#define bflb_gmac_is_desc_empty(desc)                     (((desc->length & GMAC_DESC_SIZE1_MASK) == 0) && ((desc->length & GMAC_DESC_SIZE2_MASK) == 0))
#define bflb_gmac_is_rx_desc_valid(status)                ((status & GMAC_DESC_ERROR) == 0) && ((status & GMAC_DESC_RX_FIRST) == GMAC_DESC_RX_FIRST) && ((status & GMAC_DESC_RX_LAST) == GMAC_DESC_RX_LAST)
#define bflb_gmac_is_tx_aborted(status)                   (((status & GMAC_DESC_TX_LATE_COLLISION) == GMAC_DESC_TX_LATE_COLLISION) | ((status & GMAC_DESC_TX_EXC_COLLISIONS) == GMAC_DESC_TX_EXC_COLLISIONS))
#define bflb_gmac_is_tx_carrier_error(status)             (((status & GMAC_DESC_TX_LOST_CARRIER) == GMAC_DESC_TX_LOST_CARRIER) | ((status & GMAC_DESC_TX_NO_CARRIER) == GMAC_DESC_TX_NO_CARRIER))
#define bflb_gmac_get_tx_collision_count(status)          ((status & GMAC_DESC_TX_COLL_MASK) >> GMAC_DESC_TX_COLL_SHIFT)
#define bflb_gmac_is_exc_tx_collisions(status)            ((status & GMAC_DESC_TX_EXC_COLLISIONS) == GMAC_DESC_TX_EXC_COLLISIONS)
#define bflb_gmac_is_rx_frame_damaged(status)             (((status & GMAC_DESC_RX_DAMAGED) == GMAC_DESC_RX_DAMAGED) | ((status & GMAC_DESC_RX_COLLISION) == GMAC_DESC_RX_COLLISION))
#define bflb_gmac_is_rx_frame_collision(status)           ((status & GMAC_DESC_RX_COLLISION) == GMAC_DESC_RX_COLLISION)
#define bflb_gmac_is_rx_crc(status)                       ((status & GMAC_DESC_RX_CRC) == GMAC_DESC_RX_CRC)
#define bflb_gmac_is_frame_dribbling_errors(status)       ((status & GMAC_DESC_RX_DRIBBLING) == GMAC_DESC_RX_DRIBBLING)
#define bflb_gmac_is_rx_frame_length_errors(status)       ((status & GMAC_DESC_RX_LEN_ERROR) == GMAC_DESC_RX_LEN_ERROR)
#define bflb_gmac_is_last_rx_desc(desc)                   (((desc->length & GMAC_DESC_RX_END_OF_RING) == GMAC_DESC_RX_END_OF_RING))
#define bflb_gmac_is_last_tx_desc(desc)                   (((desc->status & GMAC_DESC_TX_END_OF_RING) == GMAC_DESC_TX_END_OF_RING))
#define bflb_gmac_tx_checksum_offload_ipv4hdr(desc)       (desc->status = ((desc->status & (~GMAC_DESC_TX_CIS_MASK)) | GMAC_DESC_TX_CIS_IPV4_HDR_CS))
#define bflb_gmac_tx_checksum_offload_bypass(desc)        (desc->status = (desc->status & (~GMAC_DESC_TX_CIS_MASK)))
#define bflb_gmac_tx_checksum_offload_tcponly(desc)       (desc->status = ((desc->status & (~GMAC_DESC_TX_CIS_MASK)) | GMAC_DESC_TX_CIS_TCP_ONLY_CS))
#define bflb_gmac_tx_checksum_offload_tcp_pseudo(desc)    (desc->status = ((desc->status & (~GMAC_DESC_TX_CIS_MASK)) | GMAC_DESC_TX_CIS_TCP_PSEUDO_CS))
#define bflb_gmac_take_desc_ownership(desc)               (desc->status &= ~GMAC_DESC_OWN_BY_DMA)
#define bflb_gmac_is_ext_status(status)                   ((status & GMAC_DESC_RX_EXT_STS) != 0)
#define bflb_gmac_is_tx_payload_checksum_error(status)    ((status & GMAC_DESC_TX_PAY_CHK_ERROR) == GMAC_DESC_TX_PAY_CHK_ERROR)
#define bflb_gmac_is_tx_ipv4header_checksum_error(status) ((status & GMAC_DESC_TX_IPV4_CHK_ERROR) == GMAC_DESC_TX_IPV4_CHK_ERROR)
#define bflb_gmac_es_is_ip_header_error(ext_status)       ((ext_status & GMAC_EDESC_RX_IP_HEADER_ERROR) == GMAC_EDESC_RX_IP_HEADER_ERROR)
#define bflb_gmac_es_is_rx_checksum_bypassed(ext_status)  ((ext_status & GMAC_EDESC_RX_CHKSUM_BYPASS) == GMAC_EDESC_RX_CHKSUM_BYPASS)
#define bflb_gmac_es_is_ip_payload_error(ext_status)      ((ext_status & GMAC_EDESC_RX_IP_PAYLOAD_ERROR) == GMAC_EDESC_RX_IP_PAYLOAD_ERROR)

#define GMAC_INT_ENABLE_CFG                               (GMAC_DMA_IE_NORMAL | GMAC_DMA_IE_ABNORMAL | GMAC_DMA_IE_BUS_ERROR | \
                             GMAC_DMA_IE_RX_NOBUFFER | GMAC_DMA_IE_RX_COMPLETED | GMAC_DMA_IE_RX_STOPPED |                     \
                             GMAC_DMA_IE_TX_UNDERFLOW | GMAC_DMA_IE_TX_COMPLETED | GMAC_DMA_IE_TX_STOPPED)

#define GMAC_RX_LEN_LT600                0 /* Bit(5:7:0)=>0 IEEE 802.3 type frame Length field is Lessthan 0x0600            */
#define GMAC_RX_IPHDR_PAYLOAD_CHK_BYPASS 1 /* Bit(5:7:0)=>1 Payload & Ip header checksum bypassed (unsuppported payload)     */
#define GMAC_RX_IPHDR_PAYLOAD_RES        2 /* Bit(5:7:0)=>2 Reserved                                                         */
#define GMAC_RX_CHK_BYPASS               3 /* Bit(5:7:0)=>3 Neither IPv4 nor IPV6. So checksum bypassed                      */
#define GMAC_RX_NO_CHK_ERROR             4 /* Bit(5:7:0)=>4 No IPv4/IPv6 Checksum error detected                             */
#define GMAC_RX_PAYLOAD_CHK_ERROR        5 /* Bit(5:7:0)=>5 Payload checksum error detected for Ipv4/Ipv6 frames             */
#define GMAC_RX_IPHDR_CHK_ERROR          6 /* Bit(5:7:0)=>6 Ip header checksum error detected for Ipv4 frames                */
#define GMAC_RX_IPHDR_PAYLOAD_CHK_ERROR  7 /* Bit(5:7:0)=>7 Payload & Ip header checksum error detected for Ipv4/Ipv6 frames */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief bflb gmac enable the watchdog timer on the receiver.
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_wd_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable the watchdog timer on the receiver.
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_wd_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enables the Jabber frame support. 
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_jabber_enable(struct bflb_device_s *dev);
/**
 * @brief bflb gmac disables the Jabber frame support. 
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_jabber_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable frame burst. 
 * Enables Frame bursting (Only in Half Duplex Mode). 
 * When enabled, GMAC allows frame bursting in GMII Half Duplex mode.
 * Reserved in 10/100 and Full-Duplex configurations.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_frame_burst_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable frame burst. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_frame_burst_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable jumbo frame. 
 * Enable Jumbo frame support. 
 * When Enabled GMAC supports jumbo frames of 9018/9022(VLAN tagged).
 * Giant frame error is not reported in receive frame status.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_jumbo_frame_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable frame burst. 
 * When Disabled GMAC does not supports jumbo frames.
 * Giant frame error is reported in receive frame status.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_jumbo_frame_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable carrier sense. 
 * When Disabled GMAC ignores CRS signal during frame transmission
 * in half duplex mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_crs_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac select gmii interface. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_gmii_select(struct bflb_device_s *dev);

/**
 * @brief bflb gmac select mii interface. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_mii_select(struct bflb_device_s *dev);

/**
 * @brief bflb gmac select mii interface. 
 * 
 * @param dev
 *
 * @param speed
 * 
 * @return 0 for success
 *
 */
int bflb_gmac_mii_speed_set(struct bflb_device_s *dev, uint32_t speed);

/**
 * @brief bflb gmac enable rx own frame. 
 * Enables Receive Own bit (Only in Half Duplex Mode). 
 * When enaled GMAC receives all the packets given by phy while transmitting.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_rx_own_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable rx own frame. 
 * Disables Receive Own bit (Only in Half Duplex Mode). 
 * When enaled GMAC disables the reception of frames when gmii_txen_o is asserted.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_rx_own_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable loopback mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_loopback_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable loopback mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_loopback_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable full duplex mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_full_duplex_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable half duplex mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_half_duplex_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable retry.
 * GMAC tries retransmission (Only in Half Duplex mode).
 * If collision occurs on the GMII/MII, GMAC attempt retries based on the 
 * back off limit configured. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_retry_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable retry.
 * GMAC tries only one transmission (Only in Half Duplex mode).
 * If collision occurs on the GMII/MII, GMAC will ignore the current frami
 * transmission and report a frame abort with excessive collision in tranmit frame status. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_retry_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac strips the Pad/FCS field of incoming frames.
 * This is true only if the length field value is less than or equal to
 * 1500 bytes. All received frames with length field greater than or equal to
 * 1501 bytes are passed to the application without stripping the Pad/FCS field. 
 *  
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_pad_crc_strip_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable strips the Pad/FCS field of incoming frames.
 * GMAC doesnot strips the Pad/FCS field of incoming frames.
 * GMAC will pass all the incoming frames to Host unmodified. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_pad_crc_strip_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac set back limits.
 * 
 * @param dev
 * 
 * @param value
 *
 * @return 0 for success
 *
 */
int bflb_gmac_back_off_limit_set(struct bflb_device_s *dev, uint32_t value);

/**
 * @brief bflb gmac enable deferral check.
 * Enables the Deferral check in GMAC (Only in Half Duplex mode)
 * GMAC issues a Frame Abort Status, along with the excessive deferral error bit set in the 
 * transmit frame status when transmit state machine is deferred for more than
 *  - 24,288 bit times in 10/100Mbps mode
 *  - 155,680 bit times in 1000Mbps mode or Jumbo frame mode in 10/100Mbps operation.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_deferral_check_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable deferral check.
 * Disables the Deferral check in GMAC (Only in Half Duplex mode).
 * GMAC defers until the CRS signal goes inactive.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_deferral_check_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable the reception of frames on GMII/MII.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_rx_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable the reception of frames on GMII/MII.
 * GMAC receive state machine is disabled after completion of reception of current frame.
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_rx_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable the transmission of frames on GMII/MII.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_tx_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable the transmission of frames on GMII/MII.
 * GMAC transmit state machine is disabled after completion of transmission of current frame.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_tx_disable(struct bflb_device_s *dev);
/**
 * @brief bflb gmac enable reception of all the frames to application.
 * GMAC passes all the frames received to application irrespective of whether they
 * pass SA/DA address filtering or not.
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_frame_filter_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable reception of all the frames to application.
 * GMAC passes only those received frames to application which 
 * pass SA/DA address filtering.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_frame_filter_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac set hash table high.
 * Populates the Hash High register with the data supplied.
 * This function is called when the Hash filtering is to be enabled.
 * 
 * @param dev
 *
 * @param data
 * 
 * @return 0 for success
 *
 */
int bflb_gmac_hash_table_high_set(struct bflb_device_s *dev, uint32_t data);

/**
 * @brief bflb gmac set hash table low.
 * Populates the Hash Low register with the data supplied.
 * This function is called when the Hash filtering is to be enabled.
 * 
 * @param dev
 *
 * @param data
 *
 * @return 0 for success
 *
 */
int bflb_gmac_hash_table_low_set(struct bflb_device_s *dev, uint32_t data);

/**
 * @brief bflb gmac enable hash or perfect filter
 * Enables Hash or Perfect filter (only if Hash filter is enabled in H/W).
 * Only frames matching either perfect filtering or Hash Filtering as per HMC and HUC 
 * configuration are sent to application.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_hash_perfect_filter_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable hash filter only
 * Enables only Hash(only if Hash filter is enabled in H/W).
 * Only frames matching Hash Filtering as per HMC and HUC 
 * configuration are sent to application.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_hash_filter_only_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable src address filter
 * Enables Source address filtering.
 * When enabled source address filtering is performed. Only frames matching SA filtering are passed  to application with 
 * SAMatch bit of RxStatus is set. GMAC drops failed frames. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_src_addr_filter_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable src address filter
 * When disabled GMAC forwards the received frames with updated SAMatch bit in RxStatus. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_src_addr_filter_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac inverse dst address filter
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_dst_addr_filter_inverse(struct bflb_device_s *dev);

/**
 * @brief bflb gmac normal dst address filter
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_dst_addr_filter_normal(struct bflb_device_s *dev);

/**
 * @brief bflb gmac set forwarding of control frames.
 * When set forwards all the control frames (incl. unicast and multicast PAUSE frames).
 *
 * @param dev
 * 
 * @param val
 *
 * @return 0 for success
 *
 */
int bflb_gmac_pass_control_set(struct bflb_device_s *dev, uint32_t val);

/**
 * @brief bflb gmac enable Broadcast frames.
 * When enabled Address filtering module passes all incoming broadcast frames.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_broadcast_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable Broadcast frames.
 * When disabled Address filtering module filters all incoming broadcast frames.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_broadcast_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable Multicast frames.
  * When enabled all multicast frames are passed.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_multicast_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable Multicast frames.
 * When disabled multicast frame filtering depends on HMC bit.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_multicast_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable multicast hash filtering.
 * When enabled GMAC performs teh destination address filtering according to the hash table.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_multicast_hash_filter_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable multicast hash filtering.
 * When disabled GMAC performs perfect destination address filtering for multicast frames, it compares 
 * DA field with the value programmed in DA register.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_multicast_hash_filter_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable promiscous mode.
 * When enabled Address filter modules pass all incoming frames regardless of their Destination
 * and source addresses.
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_promisc_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable promiscous mode.
 * When called the GMAC falls back to normal operation from promiscous mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_promisc_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable unicast hash filtering.
 * When disabled GMAC performs perfect destination address filtering for unicast frames, it compares 
 * DA field with the value programmed in DA register.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_unicast_hash_filter_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable unicast hash filtering.
 * When disabled GMAC performs perfect destination address filtering for multicast frames, it compares 
 * DA field with the value programmed in DA register.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_unicast_hash_filter_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable detection of pause frames with stations unicast address.
 * When enabled GMAC detects the pause frames with stations unicast address in addition to the
 * detection of pause frames with unique multicast address.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_unicast_pause_frame_detect_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable detection of pause frames with stations unicast address.
 * When disabled GMAC only detects with the unique multicast address (802.3x).
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_unicast_pause_frame_detect_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable Rx flow control enable.
 * When Enabled GMAC will decode the rx pause frame and disable the tx for a specified time.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_rx_flow_control_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable Rx flow control disable.
 * When disabled GMAC will not decode pause frame.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_rx_flow_control_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac enable Tx flow control enable.
 * When Enabled 
 *  - In full duplex GMAC enables flow control operation to transmit pause frames.
 *  - In Half duplex GMAC enables the back pressure operation
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_tx_flow_control_enable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac disable Tx flow control disable.
 * When Disabled 
 *  - In full duplex GMAC will not transmit any pause frames.
 *  - In Half duplex GMAC disables the back pressure feature.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_tx_flow_control_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac activate Flowcontrol operation.
 * When Set
 *  - In full duplex GMAC initiates pause control frame.
 *  - In Half duplex GMAC initiates back pressure function.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_tx_flow_control_activate(struct bflb_device_s *dev);

/**
 * @brief bflb gmac stops Flowcontrol operation.called only in HALFDUPLEX
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_tx_flow_control_deactivate(struct bflb_device_s *dev);

/**
 * @brief bflb gmac set rx dma description
 *
 * @param dev
 * @param last_ring_desc
 * 
 * @return 0 for success
 *
 */
int bflb_gmac_rx_dma_desc_init(struct bflb_gmac_dma_desc_s *desc, uint8_t last_ring_desc);

/**
 * @brief bflb gmac set tx dma description
 *
 * @param dev
 * @param last_ring_desc
 * 
 * @return 0 for success
 *
 */
int bflb_gmac_tx_dma_desc_init(struct bflb_gmac_dma_desc_s *desc, uint8_t last_ring_desc);

/**
 * @brief gmac buffer description init
 *
 * @param dev
 * @param tx_dma_desc
 * @param tx_buff
 * @param tx_dma_desc_cnt
 * @param rx_dma_desc
 * @param rx_buff
 * @param rx_dma_desc_cnt
 * 
 * @return 0 for success
 * 
 */
int bflb_gmac_dma_desc_init(struct bflb_device_s *dev,
                            struct bflb_gmac_dma_desc_s *tx_dma_desc, uint8_t *tx_buff, uint32_t tx_dma_desc_cnt,
                            struct bflb_gmac_dma_desc_s *rx_dma_desc, uint8_t *rx_buff, uint32_t rx_dma_desc_cnt);

/**
 * @brief This function tracks the tx descriptor the DMA just closed after the transmission of data from this descriptor is 
 * over. This returns the descriptor fields to the caller.
 * 
 * @param[in] dev pointer to gmac.
 * @param[out] status status field of the descriptor.
 * @param[out] buffer1 Dma-able buffer1 pointer.
 * @param[out] length1 length of buffer1 (Max is 2048).
 * @param[out] data1 virtual pointer for buffer1.
 * @param[out] buffer2 Dma-able buffer2 pointer.
 * @param[out] length2 length of buffer2 (Max is 2048).
 * @param[out] data2 virtual pointer for buffer2.
 * @param[out] ext_status extend status field of the descriptor.
 * @param[out] time_stamp_high time stamp high.
 * @param[out] time_stamp_low time stamp low.
 * 
 * @return returns present tx descriptor index on success. Negative value if error.
 */

int bflb_gmac_get_tx_qptr(struct bflb_device_s *dev, uint32_t *status, uint32_t *buffer1, uint32_t *length1,
                          uint32_t *data1, uint32_t *buffer2, uint32_t *length2, uint32_t *data2,
                          uint32_t *ext_status, uint32_t *time_stamp_high, uint32_t *time_stamp_low);

/**
 * @brief Once the driver has a packet ready to be transmitted, this function is called with the 
 * valid dma-able buffer addresses and their lengths. This function populates the descriptor
 * and make the DMA the owner for the descriptor. This function also controls whetther Checksum
 * offloading to be done in hardware or not.
 * 
 * @param[in] dev pointer to gmac.
 * @param[in] buffer1 Dma-able buffer1 pointer.
 * @param[in] length1 length of buffer1 (Max is 2048).
 * @param[in] data1 virtual pointer for buffer1.
 * @param[in] buffer2 Dma-able buffer2 pointer.
 * @param[in] length2 length of buffer2 (Max is 2048).
 * @param[in] data2 virtual pointer for buffer2.
 * @param[in] offload_needed indicating whether the checksum offloading in HW/SW.
 * 
 * @return returns present tx descriptor index on success. Negative value if error.
 */
int bflb_gmac_set_tx_qptr(struct bflb_device_s *dev, uint32_t buffer1, uint32_t length1, uint32_t data1,
                          uint32_t buffer2, uint32_t length2, uint32_t data2, uint32_t offload_needed);

/**
 * @brief Prepares the descriptor to receive packets.After successful return from this function the
 * descriptor is added to the receive descriptor pool/queue.
 * 
 * @param[in] dev pointer to gmac.
 * @param[in] buffer1 Dma-able buffer1 pointer.
 * @param[in] length1 length of buffer1 (Max is 2048).
 * @param[in] data1 virtual pointer for buffer1.
 * @param[in] buffer2 Dma-able buffer2 pointer.
 * @param[in] length2 length of buffer2 (Max is 2048).
 * @param[in] data2 virtual pointer for buffer2.
 * 
 * @return returns returns present rx descriptor index on success. Negative value if error.
 */
int bflb_gmac_set_rx_qptr(struct bflb_device_s *dev, uint32_t buffer1, uint32_t length1, uint32_t data1,
                          uint32_t buffer2, uint32_t length2, uint32_t data2);

/**
 * @brief When the DMA indicates that the data is received (interrupt is generated), this function should be
 * called to get the descriptor and hence the data buffers received. With successful return from this
 * function caller gets the descriptor fields for processing.
 * 
 * @param[in] dev pointer to gmac.
 * @param[out] buffer1 Dma-able buffer1 pointer.
 * @param[out] length1 length of buffer1 (Max is 2048).
 * @param[out] data1 virtual pointer for buffer1.
 * @param[out] buffer2 Dma-able buffer2 pointer.
 * @param[out] length2 length of buffer2 (Max is 2048).
 * @param[out] data2 virtual pointer for buffer2.
 * @param[out] ext_status extend status field of the descriptor.
 * @param[out] time_stamp_high time stamp high.
 * @param[out] time_stamp_low time stamp low.
 * 
 * @return returns returns present rx descriptor index on success. Negative value if error.
 */
int bflb_gmac_get_rx_qptr(struct bflb_device_s *dev, uint32_t *status, uint32_t *buffer1, uint32_t *length1,
                          uint32_t *data1, uint32_t *buffer2, uint32_t *length2, uint32_t *data2,
                          uint32_t *ext_status, uint32_t *time_stamp_high, uint32_t *time_stamp_low);

/**
 * @brief Get all the interrupts.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return interrupt status
 */
uint32_t bflb_gmac_get_interrupt_status(struct bflb_device_s *dev);

/**
 * @brief Clears all the pending interrupts.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] interrupts interrupts to clear.
 * 
 * @return 0 for success
 */
int bflb_gmac_clear_interrupt(struct bflb_device_s *dev, uint32_t interrupts);

/**
 * @brief Returns the all unmasked interrupt status after reading the DmaStatus register.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return the all unmasked interrupt status
 */
uint32_t bflb_gmac_get_interrupt_type(struct bflb_device_s *dev);

/**
 * @brief Get the interrupt mask.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return the interrupt mask.
 */

uint32_t bflb_gmac_get_interrupt_mask(struct bflb_device_s *dev);

/**
 * @brief Enables the DMA interrupt as specified by the bit mask.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] interrupts bit mask for interrupts to be enabled.
 * 
 * @return 0 for success
 */
int bflb_gmac_enable_interrupt(struct bflb_device_s *dev, uint32_t interrupts);

/**
 * @brief Disable all the interrupts.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_disable_interrupt_all(struct bflb_device_s *dev);

/**
 * @brief Disable interrupt according to the bitfield supplied.
 * Disables only those interrupts specified in the bit mask in second argument.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] interrupts bit mask for interrupts to be disabled.
 * 
 * @return 0 for success
 */
int bflb_gmac_disable_interrupt(struct bflb_device_s *dev, uint32_t interrupts);

/**
 * @brief Enable the DMA Reception.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_enable_dma_rx(struct bflb_device_s *dev);

/**
 * @brief Enable the DMA Transmission.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_enable_dma_tx(struct bflb_device_s *dev);

/**
 * @brief Resumes the DMA Transmission.
 * the DmaTxPollDemand is written. (the data writeen could be anything).
 * This forces the DMA to resume transmission.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_resume_dma_tx(struct bflb_device_s *dev);

/**
 * @brief Resumes the DMA Reception.
 * the DmaRxPollDemand is written. (the data writeen could be anything).
 * This forces the DMA to resume reception.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_resume_dma_rx(struct bflb_device_s *dev);

/**
 * @brief Take ownership of all the rx Descriptors.
 * This function is called when there is fatal error in DMA transmission.
 * When called it takes the ownership of all the tx descriptor in tx descriptor pool/queue from DMA.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_take_desc_ownership_rx(struct bflb_device_s *dev);

/**
 * @brief Take ownership of all the tx Descriptors.
 * This function is called when there is fatal error in DMA transmission.
 * When called it takes the ownership of all the tx descriptor in tx descriptor pool/queue from DMA.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_take_desc_ownership_tx(struct bflb_device_s *dev);

/**
 * @brief Disable the DMA for Transmission.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_disable_dma_tx(struct bflb_device_s *dev);

/**
 * @brief Disable the DMA for Reception.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_disable_dma_rx(struct bflb_device_s *dev);

/**
 * @brief Decodes the Rx Descriptor status to various checksum error conditions.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] u32 status field of the corresponding descriptor.
 * 
 * @return returns decoded enum (u32) indicating the status.
 */
uint32_t bflb_gmac_is_rx_checksum_error(struct bflb_device_s *dev, uint32_t status);

/**
 * @brief Enables the assertion of PMT interrupt.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_pmt_int_enable(struct bflb_device_s *dev);

/**
 * @brief This function enables the timestamping. This enables the timestamping for transmit and receive frames.
 * When disabled timestamp is not added to tx and receive frames and timestamp generator is suspended.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_enable(struct bflb_device_s *dev);

/**
 * @brief This function disables the timestamping. 
 * When disabled timestamp is not added to tx and receive frames and timestamp generator is suspended.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_disable(struct bflb_device_s *dev);

/**
 * @brief Enable the interrupt to get timestamping interrupt. 
  * This enables the host to get the interrupt when (1) system time is greater or equal to the 
  * target time high and low register or (2) there is a overflow in th esecond register.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_int_enable(struct bflb_device_s *dev);

/**
 * @brief Disable the interrupt to get timestamping interrupt. 
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_int_disable(struct bflb_device_s *dev);

/**
 * @brief Enable MAC address for PTP frame filtering. 
 * When enabled, uses MAC address (apart from MAC address 0) to filter the PTP frames when
 * PTP is sent directly over Ethernet.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_mac_addr_filt_enable(struct bflb_device_s *dev);

/**
 * @brief Disables MAC address for PTP frame filtering. 
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_mac_addr_filt_disable(struct bflb_device_s *dev);

/**
 * @brief Selet the type of clock mode for PTP. 
 * Please note to use one of the follwoing as the clk_type argument.
 *     00=> Ordinary clock
 *     01=> Boundary clock
 *     10=> End-to-End transparent clock
 *     11=> P-to-P transparent clock
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] clk_type value representing one of the above clk value
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_set_clk_type(struct bflb_device_s *dev, uint32_t clk_type);

/**
 * @brief Enable Snapshot for messages relevant to Master. 
 * When enabled, snapshot is taken for messages relevant to master mode only, else snapshot is taken for messages relevant 
 * to slave node. 
 * Valid only for Ordinary clock and Boundary clock
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_master_enable(struct bflb_device_s *dev);

/**
 * @brief Disable Snapshot for messages relevant to Master. 
 * When disabled, snapshot is taken for messages relevant 
 * to slave node. 
 * Valid only for Ordinary clock and Boundary clock
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_master_disable(struct bflb_device_s *dev);

/**
 * @brief Enable Snapshot for Event messages. 
 * When enabled, snapshot is taken for event messages only (SYNC, Delay_Req, Pdelay_Req or Pdelay_Resp)
 * When disabled, snapshot is taken for all messages except Announce, Management and Signaling.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_event_enable(struct bflb_device_s *dev);

/**
 * @brief Disable Snapshot for Event messages. 
 * When disabled, snapshot is taken for all messages except Announce, Management and Signaling.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_event_disable(struct bflb_device_s *dev);

/**
 * @brief Enable time stamp snapshot for IPV4 frames. 
 * When enabled, time stamp snapshot is taken for IPV4 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_ipv4_enable(struct bflb_device_s *dev);

/**
 * @brief Disable time stamp snapshot for IPV4 frames. 
 * When disabled, time stamp snapshot is not taken for IPV4 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_ipv4_disable(struct bflb_device_s *dev);

/**
 * @brief Enable time stamp snapshot for IPV6 frames. 
 * When enabled, time stamp snapshot is taken for IPV6 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_ipv6_enable(struct bflb_device_s *dev);

/**
 * @brief Disable time stamp snapshot for IPV6 frames. 
 * When disabled, time stamp snapshot is not taken for IPV6 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_ipv6_disable(struct bflb_device_s *dev);

/**
 * @brief Enable time stamp snapshot for PTP over Ethernet frames. 
 * When enabled, time stamp snapshot is taken for PTP over Ethernet frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_ptp_over_ethernet_enable(struct bflb_device_s *dev);

/**
 * @brief Disable time stamp snapshot for PTP over Ethernet frames. 
 * When disabled, time stamp snapshot is not taken for PTP over Ethernet frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_ptp_over_ethernet_disable(struct bflb_device_s *dev);

/**
 * @brief Snoop PTP packet for version 2 format 
 * When set the PTP packets are snooped using the version 2 format.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_pkt_snoop_ver2(struct bflb_device_s *dev);

/**
 * @brief Snoop PTP packet for version 1 format 
 * When set the PTP packets are snooped using the version 1 format.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_pkt_snoop_ver1(struct bflb_device_s *dev);

/**
 * @brief Timestamp digital rollover 
 * When set the timestamp low register rolls over after 0x3B9A_C9FF value.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_digital_rollover_enable(struct bflb_device_s *dev);

/**
 * @brief Timestamp binary rollover 
 * When set the timestamp low register rolls over after 0x7FFF_FFFF value.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_binary_rollover_enable(struct bflb_device_s *dev);

/**
 * @brief Enable Time Stamp for All frames 
 * When set the timestamp snap shot is enabled for all frames received by the core.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_all_frames_enable(struct bflb_device_s *dev);

/**
 * @brief Disable Time Stamp for All frames 
 * When reset the timestamp snap shot is not enabled for all frames received by the core.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_all_frames_disable(struct bflb_device_s *dev);

/**
 * @brief Addend Register Update 
 * This function loads the contents of Time stamp addend register with the supplied 32 value.
 * This is reserved function when only coarse correction option is selected
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] addend_value 32 bit addend value.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_addend_update(struct bflb_device_s *dev, uint32_t addend_value);

/**
 * @brief time stamp Update 
 * This function updates (adds/subtracts) with the value specified in the Timestamp High Update and
 * Timestamp Low Update register.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] high_value Timestamp High Update value
 * @param[in] low_value Timestamp Low Update value
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_timestamp_update(struct bflb_device_s *dev, uint32_t high_value, uint32_t low_value);

/**
 * @brief time stamp Initialize 
 * This function Loads/Initializes h the value specified in the Timestamp High Update and
 * Timestamp Low Update register.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] high_value Timestamp High Update value
 * @param[in] low_value Timestamp Low Update value
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_timestamp_init(struct bflb_device_s *dev, uint32_t high_value, uint32_t low_value);

/**
 * @brief Time Stamp Update Coarse 
  * When reset the timestamp update is done using coarse method.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_coarse_update(struct bflb_device_s *dev);

/**
 * @brief Time Stamp Update Fine 
  * When reset the timestamp update is done using Fine method.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_fine_update(struct bflb_device_s *dev);

/**
 * @brief Load the Sub Second Increment value in to Sub Second increment register 
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] sub_sec_inc_value 32 bit addend value.
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_subsecond_init(struct bflb_device_s *dev, uint32_t sub_sec_inc_value);

/**
 * @brief Reads the time stamp contents in to the respective pointers 
 * These registers are readonly.
 * This function returns the 48 bit time stamp assuming Version 2 timestamp with higher word is selected.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] higher_sec_val pointer to hold 16 higher bit second register contents
 * 
 * @param[in] sec_val pointer to hold 32 bit second register contents
 * 
 * @param[in] sub_sec_val pointer to hold 32 bit subnanosecond register contents
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_read_timestamp(struct bflb_device_s *dev,
                                uint16_t *higher_sec_val, uint32_t *sec_val, uint32_t *sub_sec_val);

/**
 * @brief Loads the time stamp higher sec value from the value supplied 
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] higher_sec_val 16 higher bit second register contents
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_load_timestamp_higher_val(struct bflb_device_s *dev, uint16_t higher_sec_val);

/**
 * @brief Loads the time stamp higher sec value from the value supplied 
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] higher_sec_val pointer to hold 16 higher bit second register contents
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_read_timestamp_higher_val(struct bflb_device_s *dev, uint16_t *higher_sec_val);

/**
 * @brief Loads the time stamp higher sec value from the value supplied 
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] sec_val target Timestamp High value
 * 
 * @param[in] sub_sec_val target Timestamp Low value
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_load_target_timestamp(struct bflb_device_s *dev, uint32_t sec_val, uint32_t sub_sec_val);

/**
 * @brief Reads the Target time stamp registers 
 * This function Loads the target time stamp registers with the values proviced
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @param[in] sec_val pointer of target Timestamp High value
 * 
 * @param[in] sub_sec_val pointer of target Timestamp Low value
 * 
 * @return 0 for success
 */
int bflb_gmac_ts_read_target_timestamp(struct bflb_device_s *dev, uint32_t *sec_val, uint32_t *sub_sec_val);

/**
 * @brief Enables the ip checksum offloading in receive path.
 * When set GMAC calculates 16 bit 1's complement of all received ethernet frame payload.
 * It also checks IPv4 Header checksum is correct. GMAC core appends the 16 bit checksum calculated
 * for payload of IP datagram and appends it to Ethernet frame transferred to the application.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_rx_chksum_offload_enable(struct bflb_device_s *dev);

/**
 * @brief Disable the ip checksum offloading in receive path.
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_rx_chksum_offload_disable(struct bflb_device_s *dev);

/**
 * @brief Instruct the DMA to drop the packets fails tcp ip checksum.
 * This is to instruct the receive DMA engine to drop the recevied packet if they 
 * fails the tcp/ip checksum in hardware. Valid only when full checksum offloading is enabled(type-2).
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_rx_tcpip_chksum_drop_enable(struct bflb_device_s *dev);

/**
 * @brief Instruct the DMA not to drop the packets even if it fails tcp ip checksum.
 * This is to instruct the receive DMA engine to allow the packets even if recevied packet
 * fails the tcp/ip checksum in hardware. Valid only when full checksum offloading is enabled(type-2).
 * 
 * @param[in] dev pointer to gmac.
 * 
 * @return 0 for success
 */
int bflb_gmac_rx_tcpip_chksum_drop_disable(struct bflb_device_s *dev);

/**
 * @brief bflb gmac set mdc clock div
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * 
 * @return int
 *
 */
int bflb_gmac_mdc_clk_div_set(struct bflb_device_s *dev, uint16_t div);

/**
 * @brief bflb gmac get verison
 *
 * @param dev pointer to gmac.
 * 
 * @param version pointer to version varial
 * 
 * @return int
 *
 */
int bflb_gmac_version_get(struct bflb_device_s *dev, uint32_t *version);

/**
 * @brief bflb gmac set mac address
 *
 * @param dev
 * @param mac_addr
 * 
 * @return 0 for success
 *
 */
int bflb_gmac_mac_addr_set(struct bflb_device_s *dev, const uint8_t mac_addr[6]);

/**
 * @brief bflb gmac reset.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_reset(struct bflb_device_s *dev);

/**
 * @brief bflb gmac DMA BUS Init.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_dma_bus_init(struct bflb_device_s *dev);

/**
 * @brief bflb gmac DMA control Init.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_gmac_dma_ctrl_init(struct bflb_device_s *dev);

/**
 * @brief bflb gmac init
 *
 * @param dev
 * 
 * @param config
 *
 * @return 0 for success
 * 
 */
int bflb_gmac_init(struct bflb_device_s *dev, const struct bflb_gmac_config_s *config);

/**
 * @brief bflb gmac deinit
 *
 * @param dev
 *
 */
void bflb_gmac_deinit(struct bflb_device_s *dev);

/**
 * @brief bflb gmac deinit
 *
 * @param dev
 *
 */
void bflb_gmac_start(struct bflb_device_s *dev);

/**
 * @brief bflb gmac handle tx done
 *
 * @param dev
 *
 * @return 0 for success
 * 
 */
int bflb_gmac_handle_transmit_over(struct bflb_device_s *dev);

/**
 * @brief
 *
 * @param flags
 * @param len
 * @param data_in
 * @return int
 */
int bflb_gmac_bd_tx_enqueue(struct bflb_device_s *dev, uint32_t flags, uint32_t len, const uint8_t *data_in);

/**
 * @brief
 * @param dev
 * @param flags
 * @param len
 * @param data_out
 * @return int
 */
#ifndef GMAC_RX_DATA_ZERO_COPY
int bflb_gmac_bd_rx_dequeue(struct bflb_device_s *dev, uint32_t flags, uint32_t *len, uint8_t *data_out);
#else
int bflb_gmac_bd_rx_dequeue(struct bflb_device_s *dev, uint32_t flags, uint32_t *len, uint8_t **data_out);
#endif

/**
 * @brief bflb gmac phy register read
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_gmac_phy_reg_read(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t *phy_reg_val);

/**
 * @brief bflb gmac phy register write
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_gmac_phy_reg_write(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t phy_reg_val);

/**
 * @brief bflb gmac feature control
 *
 * @param dev
 * @param cmd
 * @param arg
 * @return int
 *
 */
int bflb_gmac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

#if defined(BL628) || defined(BL616D)
/**
 * @brief bflb emac phy register read
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_emac_phy_reg_read(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t *phy_reg_val);
/**
 * @brief bflb emac phy register write
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_emac_phy_reg_write(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t phy_reg_val);

/**
 * @brief bflb emac feature control
 *
 * @param dev
 * @param cmd
 * @param arg
 * @return int
 *
 */
int bflb_emac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);
#endif

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