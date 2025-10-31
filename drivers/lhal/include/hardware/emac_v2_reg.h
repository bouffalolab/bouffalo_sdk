/**
  ******************************************************************************
  * @file    emac_v2_reg.h
  * @version V1.0
  * @date    2024-05-01
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2024 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __HARDWARE_EMAC_V2_H__
#define __HARDWARE_EMAC_V2_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/
#define EMAC_V2_CONFIG_OFFSET              (0x0)
#define EMAC_V2_FRAME_FILTER_OFFSET        (0x4)
#define EMAC_V2_HASH_HIGH_OFFSET           (0x8)
#define EMAC_V2_HASH_LOW_OFFSET            (0xC)
#define EMAC_V2_GMII_ADDR_OFFSET           (0x10)
#define EMAC_V2_GMII_DATA_OFFSET           (0x14)
#define EMAC_V2_FLOW_CONTROL_OFFSET        (0x18)
#define EMAC_V2_VLAN_OFFSET                (0x1C)
#define EMAC_V2_VERSION_OFFSET             (0x20)
#define EMAC_V2_WAKEUP_ADDR_OFFSET         (0x28)
#define EMAC_V2_PMT_CTRL_STATUS_OFFSET     (0x2C)
#define EMAC_V2_LPI_CTRL_STS_OFFSET        (0x30)
#define EMAC_V2_LPI_TIMER_CTRL_OFFSET      (0x34)
#define EMAC_V2_INTERRUPT_STATUS_OFFSET    (0x38)
#define EMAC_V2_INTERRUPT_MASK_OFFSET      (0x3C)
#define EMAC_V2_MAC_ADDR0_HIGH_OFFSET      (0x40)
#define EMAC_V2_MAC_ADDR0_LOW_OFFSET       (0x44)
#define EMAC_V2_MAC_ADDR1_HIGH_OFFSET      (0x48)
#define EMAC_V2_MAC_ADDR1_LOW_OFFSET       (0x4C)

#define EMAC_V2_MAC_ADDR15_HIGH_OFFSET     (0xB8)
#define EMAC_V2_MAC_ADDR15_LOW_OFFSET      (0xBC)

#define EMAC_V2_TS_CTRL_OFFSET             (0x700)
#define EMAC_V2_TS_SUB_SEC_INCR_OFFSET     (0x704)
#define EMAC_V2_TS_HIGH_OFFSET             (0x708)
#define EMAC_V2_TS_LOW_OFFSET              (0x70C)

#define EMAC_V2_TS_HIGH_UPDATE_OFFSET      (0x710)
#define EMAC_V2_TS_LOW_UPDATE_OFFSET       (0x714)
#define EMAC_V2_TS_ADDEND_OFFSET           (0x718)
#define EMAC_V2_TS_TARGET_TIME_HIGH_OFFSET (0x71C)
#define EMAC_V2_TS_TARGET_TIME_LOW_OFFSET  (0x720)
#define EMAC_V2_TS_HIGH_WORD_OFFSET        (0x724)
#define EMAC_V2_TS_STATUS_OFFSET           (0x728)
#ifdef EMAC_V2_AVB_SUPPORT
#define EMAC_V2_AV_MAC_CTRL_OFFSET (0x738)
#endif

#define EMAC_V2_DMA_BUS_MODE_OFFSET         (0x1000) /* CSR0 - Bus Mode Register                          */
#define EMAC_V2_DMA_TX_POLL_DEMAND_OFFSET   (0x1004) /* CSR1 - Transmit Poll Demand Register              */
#define EMAC_V2_DMA_RX_POLL_DEMAND_OFFSET   (0x1008) /* CSR2 - Receive Poll Demand Register               */
#define EMAC_V2_DMA_RX_BASE_ADDR_OFFSET     (0x100C) /* CSR3 - Receive Descriptor list base address       */
#define EMAC_V2_DMA_TX_BASE_ADDR_OFFSET     (0x1010) /* CSR4 - Transmit Descriptor list base address      */
#define EMAC_V2_DMA_STATUS_OFFSET           (0x1014) /* CSR5 - Dma status Register                        */
#define EMAC_V2_DMA_CONTROL_OFFSET          (0x1018) /* CSR6 - Dma Operation Mode Register                */
#define EMAC_V2_DMA_INTERRUPT_ENABLE_OFFSET (0x101C) /* CSR7 - Interrupt enable                           */
#define EMAC_V2_DMA_MISSED_FR_OFFSET        (0x1020) /* CSR8 - Missed Frame & Buffer overflow Counter     */
#define EMAC_V2_DMA_TX_CURR_DESC_OFFSET     (0x1048) /*      - Current host Tx Desc Register              */
#define EMAC_V2_DMA_RX_CURR_DESC_OFFSET     (0x104C) /*      - Current host Rx Desc Register              */
#define EMAC_V2_DMA_TX_CURR_ADDR_OFFSET     (0x1050) /* CSR20 - Current host transmit buffer address      */
#define EMAC_V2_DMA_RX_CURR_ADDR_OFFSET     (0x1054) /* CSR21 - Current host receive buffer address       */
#ifdef EMAC_V2_AVB_SUPPORT
#define EMAC_V2_DMA_HW_FEATURE_OFFSET        (0x1058) /* Hardware Feature Register                         */
#define EMAC_V2_DMA_SLOT_FN_CTRL_STS_OFFSET  (0x1030) /* Slot function control and status register         */
#define EMAC_V2_DMA_CHANNEL_CTRL_OFFSET      (0x1060) /* Channel Control register only for Channel1 and Channel2 */
#define EMAC_V2_DMA_CHANNEL_AV_STS_OFFSET    (0x1064) /* Channel Status register only for Channel1 and  Channel2 */
#define EMAC_V2_DMA_IDLE_SLOPE_CREDIT_OFFSET (0x1068) /* Idle slope credit register                              */
#define EMAC_V2_DMA_SEND_SLOPE_CREDIT_OFFSET (0x106C) /* Send slope credit register                              */
#define EMAC_V2_DMA_HIGH_CREDIT_OFFSET       (0x1070) /* High Credit register                                    */
#define EMAC_V2_DMA_LOW_CREDIT_OFFSET        (0x1074) /* Lo Credit Register                                      */
#endif

/* 0x0 : EMAC_V2_CONFIG */
#define EMAC_V2_RX_ENABLE_SHIFT                  (2U)
#define EMAC_V2_RX_ENABLE_MASK                   (0x1 << EMAC_V2_RX_ENABLE_SHIFT)
#define EMAC_V2_TX_ENABLE_SHIFT                  (3U)
#define EMAC_V2_TX_ENABLE_MASK                   (0x1 << EMAC_V2_TX_ENABLE_SHIFT)
#define EMAC_V2_DEFERRAL_CHECK_SHIFT             (4U)
#define EMAC_V2_DEFERRAL_CHECK_MASK              (0x1 << EMAC_V2_DEFERRAL_CHECK_SHIFT)
#define EMAC_V2_BACKOFF_LIMIT_SHIFT              (5U)
#define EMAC_V2_BACKOFF_LIMIT_MASK               (0x3 << EMAC_V2_BACKOFF_LIMIT_SHIFT)
#define EMAC_V2_PAD_CRC_STRIP_SHIFT              (7U)
#define EMAC_V2_PAD_CRC_STRIP_MASK               (0x1 << EMAC_V2_PAD_CRC_STRIP_SHIFT)
#define EMAC_V2_LINK_UP_SHIFT                    (8U)
#define EMAC_V2_LINK_UP_MASK                     (0x1 << EMAC_V2_LINK_UP_SHIFT)
#define EMAC_V2_RETRY_DISABLE_SHIFT              (9U)
#define EMAC_V2_RETRY_DISABLE_MASK               (0x1 << EMAC_V2_RETRY_DISABLE_SHIFT)
#define EMAC_V2_RX_IPC_OFFLOAD_SHIFT             (10U)
#define EMAC_V2_RX_IPC_OFFLOAD_MASK              (0x1 << EMAC_V2_RX_IPC_OFFLOAD_SHIFT)
#define EMAC_V2_DUPLEX_SHIFT                     (11U)
#define EMAC_V2_DUPLEX_MASK                      (0x1 << EMAC_V2_DUPLEX_SHIFT)
#define EMAC_V2_LOOPBACK_SHIFT                   (12U)
#define EMAC_V2_LOOPBACK_MASK                    (0x1 << EMAC_V2_LOOPBACK_SHIFT)
#define EMAC_V2_RX_OWN_DISABLE_SHIFT             (13U)
#define EMAC_V2_RX_OWN_DISABLE_MASK              (0x1 << EMAC_V2_RX_OWN_DISABLE_SHIFT)
#define EMAC_V2_FE_100M_SHIFT                    (14U)
#define EMAC_V2_FE_100M_MASK                     (0x1 << EMAC_V2_FE_100M_SHIFT)
#define EMAC_V2_MII_GMII_SELECT_SHIFT            (15U)
#define EMAC_V2_MII_GMII_SELECT_MASK             (0x1 << EMAC_V2_MII_GMII_SELECT_SHIFT)
#define EMAC_V2_DISABLE_CRS_SHIFT                (16U)
#define EMAC_V2_DISABLE_CRS_MASK                 (0x1 << EMAC_V2_DISABLE_CRS_SHIFT)
#define EMAC_V2_INTER_FRAME_GAP_SHIFT            (17U)
#define EMAC_V2_INTER_FRAME_GAP_MASK             (0x7 << EMAC_V2_INTER_FRAME_GAP_SHIFT)
#define EMAC_V2_JUMBO_FRAME_SHIFT                (20U)
#define EMAC_V2_JUMBO_FRAME_MASK                 (0x1 << EMAC_V2_JUMBO_FRAME_SHIFT)
#define EMAC_V2_FRAME_BURST_SHIFT                (21U)
#define EMAC_V2_FRAME_BURST_MASK                 (0x1 << EMAC_V2_FRAME_BURST_SHIFT)
#define EMAC_V2_JABBER_TIMER_DISABLE_SHIFT       (22U)
#define EMAC_V2_JABBER_TIMER_DISABLE_MASK        (0x1 << EMAC_V2_JABBER_TIMER_DISABLE_SHIFT)
#define EMAC_V2_WATCHDOG_DISABLE_SHIFT           (23U)
#define EMAC_V2_WATCHDOG_DISABLE_MASK            (0x1 << EMAC_V2_WATCHDOG_DISABLE_SHIFT)

/* 0x4 : EMAC_V2_FRAME_FILTER */
#define EMAC_V2_PROMISCUOUS_MODE_SHIFT           (0U)
#define EMAC_V2_PROMISCUOUS_MODE_MASK            (0x1 << EMAC_V2_PROMISCUOUS_MODE_SHIFT)
#define EMAC_V2_UCAST_HASH_FILTER_SHIFT          (1U)
#define EMAC_V2_UCAST_HASH_FILTER_MASK           (0x1 << EMAC_V2_UCAST_HASH_FILTER_SHIFT)
#define EMAC_V2_MCAST_HASH_FILTER_SHIFT          (2U)
#define EMAC_V2_MCAST_HASH_FILTER_MASK           (0x1 << EMAC_V2_MCAST_HASH_FILTER_SHIFT)
#define EMAC_V2_DEST_ADDR_FILTER_INV_SHIFT       (3U)
#define EMAC_V2_DEST_ADDR_FILTER_INV_MASK        (0x1 << EMAC_V2_DEST_ADDR_FILTER_INV_SHIFT)
#define EMAC_V2_MCAST_FILTER_OFF_SHIFT           (4U)
#define EMAC_V2_MCAST_FILTER_OFF_MASK            (0x1 << EMAC_V2_MCAST_FILTER_OFF_SHIFT)
#define EMAC_V2_BCAST_DISABLE_SHIFT              (5U)
#define EMAC_V2_BCAST_DISABLE_MASK               (0x1 << EMAC_V2_BCAST_DISABLE_SHIFT)
#define EMAC_V2_PASS_CTRL_SHIFT                  (6U)
#define EMAC_V2_PASS_CTRL_MASK                   (0x3 << EMAC_V2_PASS_CTRL_SHIFT)
#define EMAC_V2_SRC_INV_ARRD_FILTER_SHIFT        (8U)
#define EMAC_V2_SRC_INV_ARRD_FILTER_MASK         (0x1 << EMAC_V2_SRC_INV_ARRD_FILTER_SHIFT)
#define EMAC_V2_SRC_ARRD_FILTER_SHIFT            (9U)
#define EMAC_V2_SRC_ARRD_FILTER_MASK             (0x1 << EMAC_V2_SRC_ARRD_FILTER_SHIFT)
#define EMAC_V2_HASH_PERFECT_FILTER_SHIFT        (10U)
#define EMAC_V2_HASH_PERFECT_FILTER_MASK         (0x1 << EMAC_V2_HASH_PERFECT_FILTER_SHIFT)
#define EMAC_V2_FILTER_OFF_SHIFT                 (31U)
#define EMAC_V2_FILTER_OFF_MASK                  (0x1 << EMAC_V2_FILTER_OFF_SHIFT)

/* 0x8 : EMAC_V2_HASH_HIGH */

/* 0xC : EMAC_V2_HASH_LOW */

/* 0x10 : EMAC_V2_GMII_Addr */
#define EMAC_V2_GMII_BUSY_SHIFT                  (0U)
#define EMAC_V2_GMII_BUSY_MASK                   (0x1 << EMAC_V2_GMII_BUSY_SHIFT)
#define EMAC_V2_GMII_WRITE_SHIFT                 (1U)
#define EMAC_V2_GMII_WRITE_MASK                  (0x1 << EMAC_V2_GMII_WRITE_SHIFT)
#define EMAC_V2_GMII_CSR_CLK_SHIFT               (2U)
#define EMAC_V2_GMII_CSR_CLK_MASK                (0x7 << EMAC_V2_GMII_CSR_CLK_SHIFT)
#define EMAC_V2_GMII_REG_SHIFT                   (6U)
#define EMAC_V2_GMII_REG_MASK                    (0x1F << EMAC_V2_GMII_REG_SHIFT)
#define EMAC_V2_GMII_ADDR_SHIFT                  (11U)
#define EMAC_V2_GMII_ADDR_MASK                   (0x1F << EMAC_V2_GMII_ADDR_SHIFT)

/* 0x14 : EMAC_V2_GMII_DATA */

/* 0x18 : EMAC_V2_FLOW_CONTROL */
#define EMAC_V2_SEND_PAUSE_FRAME_SHIFT           (0U)
#define EMAC_V2_SEND_PAUSE_FRAME_MASK            (0x1 << EMAC_V2_SEND_PAUSE_FRAME_SHIFT)
#define EMAC_V2_APPLY_BACK_PRESSURE_SHIFT        (0U)
#define EMAC_V2_APPLY_BACK_PRESSURE_MASK         (0x1 << EMAC_V2_APPLY_BACK_PRESSURE_SHIFT)
#define EMAC_V2_TX_FLOW_CONTROL_ENABLE_SHIFT     (1U)
#define EMAC_V2_TX_FLOW_CONTROL_ENABLE_MASK      (0x1 << EMAC_V2_TX_FLOW_CONTROL_ENABLE_SHIFT)
#define EMAC_V2_RX_FLOW_CONTROL_ENABLE_SHIFT     (2U)
#define EMAC_V2_RX_FLOW_CONTROL_ENABLE_MASK      (0x1 << EMAC_V2_RX_FLOW_CONTROL_ENABLE_SHIFT)
#define EMAC_V2_UCAST_PAUSE_FRAME_ON_SHIFT       (3U)
#define EMAC_V2_UCAST_PAUSE_FRAME_ON_MASK        (0x1 << EMAC_V2_UCAST_PAUSE_FRAME_ON_SHIFT)
#define EMAC_V2_PAUSE_LOW_THRESH_SHIFT           (4U)
#define EMAC_V2_PAUSE_LOW_THRESH_MASK            (0x3 << EMAC_V2_PAUSE_LOW_THRESH_SHIFT)
#define EMAC_V2_PAUSE_TIME_SHIFT                 (16U)
#define EMAC_V2_PAUSE_TIME_MASK                  (0xFFFF << EMAC_V2_PAUSE_TIME_SHIFT)

/* 0x38 : EMAC_V2_INT_STATUS */
#define EMAC_V2_RGMII_LINK_INT_STS_SHIFT         (0U)
#define EMAC_V2_RGMII_LINK_INT_STS_MASK          (0x1 << EMAC_V2_RGMII_LINK_INT_STS_SHIFT)
#define EMAC_V2_PCS_LINK_INT_STS_SHIFT           (1U)
#define EMAC_V2_PCS_LINK_INT_STS_MASK            (0x1 << EMAC_V2_PCS_LINK_INT_STS_SHIFT)
#define EMAC_V2_PCS_AN_CMPL_INT_STS_SHIFT        (2U)
#define EMAC_V2_PCS_AN_CMPL_INT_STS_MASK         (0x1 << EMAC_V2_PCS_AN_CMPL_INT_STS_SHIFT)
#define EMAC_V2_PCS_PMT_INT_STS_SHIFT            (3U)
#define EMAC_V2_PCS_PMT_INT_STS_MASK             (0x1 << EMAC_V2_PCS_PMT_INT_STS_SHIFT)
#define EMAC_V2_MMC_INT_STS_SHIFT                (4U)
#define EMAC_V2_MMC_INT_STS_MASK                 (0x1 << EMAC_V2_MMC_INT_STS_SHIFT)
#define EMAC_V2_MMC_RX_INT_STS_SHIFT             (5U)
#define EMAC_V2_MMC_RX_INT_STS_MASK              (0x1 << EMAC_V2_MMC_RX_INT_STS_SHIFT)
#define EMAC_V2_MMC_TX_INT_STS_SHIFT             (6U)
#define EMAC_V2_MMC_TX_INT_STS_MASK              (0x1 << EMAC_V2_MMC_TX_INT_STS_SHIFT)
#define EMAC_V2_MMC_RX_CKS_OFFLOAD_INT_STS_SHIFT (7U)
#define EMAC_V2_MMC_RX_CKS_OFFLOAD_INT_STS_MASK  (0x1 << EMAC_V2_MMC_RX_CKS_OFFLOAD_INT_STS_SHIFT)
#define EMAC_V2_TS_INT_STS_SHIFT                 (9U)
#define EMAC_V2_TS_INT_STS_MASK                  (0x1 << EMAC_V2_TS_INT_STS_SHIFT)

#define EMAC_V2_TS_INT_STS                       0x00000200 /* set if int generated due to TS (Read Time Stamp Status Register to know details)*/
#define EMAC_V2_MMC_RX_CHKSUM_OFFLOAD            0x00000080 /* set if int generated in MMC RX CHECKSUM OFFLOAD int register                   */
#define EMAC_V2_MMC_TX_INT_STS                   0x00000040 /* set if int generated in MMC TX Int register      */
#define EMAC_V2_MMC_RX_INT_STS                   0x00000020 /* set if int generated in MMC RX Int register       */
#define EMAC_V2_MMC_INT_STS                      0x00000010 /* set if any of the above bit [7:5] is set      */
#define EMAC_V2_PMT_INT_STS                      0x00000008 /* set whenver magic pkt/wake-on-lan frame is received     */
#define EMAC_V2_PCS_AN_COMPLETE                  0x00000004 /* set when AN is complete in TBI/RTBI/SGMIII phy interface        */
#define EMAC_V2_PCS_LNK_STS_CHANGE               0x00000002 /* set if any lnk status change in TBI/RTBI/SGMII interface        */
#define EMAC_V2_RGMII_INT_STS                    0x00000001 /* set if any change in lnk status of RGMII interface     */
#define EMAC_V2_TS_INTMASK                       0x00000200 /* when set disables the time stamp interrupt generation            */
#define EMAC_V2_PMT_INTMASK                      0x00000008 /* when set Disables the assertion of PMT interrupt           */
#define EMAC_V2_PCS_AN_INT_MASK                  0x00000004 /* When set disables the assertion of PCS AN complete interrupt        */
#define EMAC_V2_PCS_LNK_STS_INT_MASK             0x00000002 /* when set disables the assertion of PCS lnk status change interrupt */
#define EMAC_V2_RGMII_INT_MASK                   0x00000001 /* when set disables the assertion of RGMII int    */

/* 0x38 : EMAC_V2_INT_STATUS */
#define EMAC_V2_RGMII_LINK_INT_STS_SHIFT         (0U)
#define EMAC_V2_RGMII_LINK_INT_STS_MASK          (0x1 << EMAC_V2_RGMII_LINK_INT_STS_SHIFT)
#define EMAC_V2_PCS_LINK_INT_STS_SHIFT           (1U)
#define EMAC_V2_PCS_LINK_INT_STS_MASK            (0x1 << EMAC_V2_PCS_LINK_INT_STS_SHIFT)
#define EMAC_V2_PCS_AN_CMPL_INT_STS_SHIFT        (2U)
#define EMAC_V2_PCS_AN_CMPL_INT_STS_MASK         (0x1 << EMAC_V2_PCS_AN_CMPL_INT_STS_SHIFT)
#define EMAC_V2_PCS_PMT_INT_STS_SHIFT            (3U)
#define EMAC_V2_PCS_PMT_INT_STS_MASK             (0x1 << EMAC_V2_PCS_PMT_INT_STS_SHIFT)
#define EMAC_V2_MMC_INT_STS_SHIFT                (4U)
#define EMAC_V2_MMC_INT_STS_MASK                 (0x1 << EMAC_V2_MMC_INT_STS_SHIFT)
#define EMAC_V2_MMC_RX_INT_STS_SHIFT             (5U)
#define EMAC_V2_MMC_RX_INT_STS_MASK              (0x1 << EMAC_V2_MMC_RX_INT_STS_SHIFT)
#define EMAC_V2_MMC_TX_INT_STS_SHIFT             (6U)
#define EMAC_V2_MMC_TX_INT_STS_MASK              (0x1 << EMAC_V2_MMC_TX_INT_STS_SHIFT)
#define EMAC_V2_MMC_RX_CKS_OFFLOAD_INT_STS_SHIFT (7U)
#define EMAC_V2_MMC_RX_CKS_OFFLOAD_INT_STS_MASK  (0x1 << EMAC_V2_MMC_RX_CKS_OFFLOAD_INT_STS_SHIFT)
#define EMAC_V2_TS_INT_STS_SHIFT                 (9U)
#define EMAC_V2_TS_INT_STS_MASK                  (0x1 << EMAC_V2_TS_INT_STS_SHIFT)

/* 0x3C : EMAC_V2_INT_MASK */
#define EMAC_V2_RGMII_LINK_INT_MASK_SHIFT        (0U)
#define EMAC_V2_RGMII_LINK_INT_MASK_MASK         (0x1 << EMAC_V2_RGMII_LINK_INT_MASK_SHIFT)
#define EMAC_V2_PCS_LINK_INT_MASK_SHIFT          (1U)
#define EMAC_V2_PCS_LINK_INT_MASK_MASK           (0x1 << EMAC_V2_PCS_LINK_INT_MASK_SHIFT)
#define EMAC_V2_PCS_AN_CMPL_INT_MASK_SHIFT       (2U)
#define EMAC_V2_PCS_AN_CMPL_INT_MASK_MASK        (0x1 << EMAC_V2_PCS_AN_CMPL_INT_MASK_SHIFT)
#define EMAC_V2_PCS_PMT_INT_MASK_SHIFT           (3U)
#define EMAC_V2_PCS_PMT_INT_MASK_MASK            (0x1 << EMAC_V2_PCS_PMT_INT_MASK_SHIFT)
#define EMAC_V2_TS_INT_MASK_SHIFT                (9U)
#define EMAC_V2_TS_INT_MASK_MASK                 (0x1 << EMAC_V2_TS_INT_MASK_SHIFT)

/* 0x700 : EMAC_V2_TS_CTRL */
#define EMAC_V2_TS_ENA_SHIFT                     (0U)
#define EMAC_V2_TS_ENA_MASK                      (0x1 << EMAC_V2_TS_ENA_SHIFT)
#define EMAC_V2_TS_CF_UPDT_SHIFT                 (1U)
#define EMAC_V2_TS_CF_UPDT_MASK                  (0x1 << EMAC_V2_TS_CF_UPDT_SHIFT)
#define EMAC_V2_TS_INIT_SHIFT                    (2U)
#define EMAC_V2_TS_INIT_MASK                     (0x1 << EMAC_V2_TS_INIT_SHIFT)
#define EMAC_V2_TS_UPDT_SHIFT                    (3U)
#define EMAC_V2_TS_UPDT_MASK                     (0x1 << EMAC_V2_TS_UPDT_SHIFT)
#define EMAC_V2_TS_TRIG_SHIFT                    (4U)
#define EMAC_V2_TS_TRIG_MASK                     (0x1 << EMAC_V2_TS_TRIG_SHIFT)
#define EMAC_V2_TS_ADD_REG_SHIFT                 (5U)
#define EMAC_V2_TS_ADD_REG_MASK                  (0x1 << EMAC_V2_TS_ADD_REG_SHIFT)
#define EMAC_V2_TS_EN_ALL_SHIFT                  (8U)
#define EMAC_V2_TS_EN_ALL_MASK                   (0x1 << EMAC_V2_TS_EN_ALL_SHIFT)
#define EMAC_V2_TS_CTRL_SSR_SHIFT                (9U)
#define EMAC_V2_TS_CTRL_SSR_MASK                 (0x1 << EMAC_V2_TS_CTRL_SSR_SHIFT)
#define EMAC_V2_TS_V2_ENA_SHIFT                  (10U)
#define EMAC_V2_TS_V2_ENA_MASK                   (0x1 << EMAC_V2_TS_V2_ENA_SHIFT)
#define EMAC_V2_TS_IP_ENA_SHIFT                  (11U)
#define EMAC_V2_TS_IP_ENA_MASK                   (0x1 << EMAC_V2_TS_IP_ENA_SHIFT)
#define EMAC_V2_TS_IPV6_ENA_SHIFT                (12U)
#define EMAC_V2_TS_IPV6_ENA_MASK                 (0x1 << EMAC_V2_TS_IPV6_ENA_SHIFT)
#define EMAC_V2_TS_IPV4_ENA_SHIFT                (13U)
#define EMAC_V2_TS_IPV4_ENA_MASK                 (0x1 << EMAC_V2_TS_IPV4_ENA_SHIFT)
#define EMAC_V2_TS_EVNT_ENA_SHIFT                (14U)
#define EMAC_V2_TS_EVNT_ENA_MASK                 (0x1 << EMAC_V2_TS_EVNT_ENA_SHIFT)
#define EMAC_V2_TS_MSTR_ENA_SHIFT                (15U)
#define EMAC_V2_TS_MSTR_ENA_MASK                 (0x1 << EMAC_V2_TS_MSTR_ENA_SHIFT)
#define EMAC_V2_TS_CLK_TYPE_SHIFT                (16U)
#define EMAC_V2_TS_CLK_TYPE_MASK                 (0x3 << EMAC_V2_TS_CLK_TYPE_SHIFT)
#define EMAC_V2_TS_EN_MAC_ADDR_SHIFT             (18U)
#define EMAC_V2_TS_EN_MAC_ADDR_MASK              (0x1 << EMAC_V2_TS_EN_MAC_ADDR_SHIFT)

/* 0x704 : EMAC_V2_TS_SUB_SEC_INCR */
#define EMAC_V2_SS_INC_MASK                      (0xFF)

/* 0x708 : EMAC_V2_TS_HIGH */
#define EMAC_V2_TS_HIGH_OFFSET                   (0x708)

/* 0x70C : EMAC_V2_TS_LOW */
#define EMAC_V2_TS_LOW_OFFSET                    (0x70C)
#define EMAC_V2_TS_SIGN_SHIF                     (31U)
#define EMAC_V2_TS_SIGN_MASK                     (0x1 << EMAC_V2_TS_SIGN_SHIF)
#define EMAC_V2_TS_SIGN                          EMAC_V2_TS_SIGN_MASK
#define EMAC_V2_TS_NEGATIVE                      EMAC_V2_TS_SIGN_MASK
#define EMAC_V2_TS_POSITIVE                      (0x0)

/* 0x724:    EMAC_V2_TS_HIGH_WORD */
#define EMAC_V2_TS_HIGH_WORD_SHIFT               (0U)
#define EMAC_V2_TS_HIGH_WORD_MASK                (0xFFFF << EMAC_V2_TS_HIGH_WORD_SHIFT)

/* 0x728:    EMAC_V2_TS_STATUS */
#define EMAC_V2_TS_SECONDS_OVERFLOW_SHIFT        (0U)
#define EMAC_V2_TS_SECONDS_OVERFLOW_MASK         (0x1 << EMAC_V2_TS_SECONDS_OVERFLOW_SHIFT)
#define EMAC_V2_TS_TARG_TIME_REACHED_SHIFT       (1U)
#define EMAC_V2_TS_TARG_TIME_REACHED_MASK        (0x1 << EMAC_V2_TS_TARG_TIME_REACHED_SHIFT)

/* 0x738:    EMAC_V2_AV_MAC_CTRL */
#define EMAC_V2_AV_TYPE_SHIFT                    (0U)
#define EMAC_V2_AV_TYPE_MASK                     (0xFFFF << EMAC_V2_AV_TYPE_SHIFT)
#define EMAC_V2_AV_PRIO_SHIFT                    (16U)
#define EMAC_V2_AV_PRIO_MASK                     (0x7 << EMAC_V2_AV_PRIO_SHIFT)
#define EMAC_V2_PTP_CH_SHIFT                     (19U)
#define EMAC_V2_PTP_CH_MASK                      (0x3 << EMAC_V2_PTP_CH_SHIFT)
#define EMAC_V2_AV_CTRL_CH_SHIFT                 (24U)
#define EMAC_V2_AV_CTRL_CH_MASK                  (0x3 << EMAC_V2_AV_CTRL_CH_SHIFT)

/*EMAC_V2_DMA_BUS_MODE = 0x1000,    CSR0 - Bus Mode */
#ifdef EMAC_V2_AVB_SUPPORT
#define EMAC_V2_DMA_CHANNEL_PRIO_WT_MASK 0x30000000 /* Channel priority weight mask                     29:28    RW       0       */
#define EMAC_V2_DMA_CHANNEL_PRIO1        0x00000000 /* Channel priority weight 1                        29:28    RW       0       */
#define EMAC_V2_DMA_CHANNEL_PRIO2        0x10000000 /* Channel priority weight 2                        29:28    RW       0       */
#define EMAC_V2_DMA_CHANNEL_PRIO3        0x20000000 /* Channel priority weight 3                        29:28    RW       0       */
#define EMAC_V2_DMA_CHANNEL_PRIO4        0x30000000 /* Channel priority weight 4                        29:28    RW       0       */
#define EMAC_V2_DMA_TX_RX_PRIO           0x08000000 /* When set indicates Tx DMA has more priority       27      RW        0       */
#define EMAC_V2_DMA_PRIO_RATIO11         0x00000000 /* (PR)TX:RX DMA priority ratio 1:1                15:14   RW        00      */
#define EMAC_V2_DMA_PRIO_RATIO21         0x00004000 /* (PR)TX:RX DMA priority ratio 2:1                                          */
#define EMAC_V2_DMA_PRIO_RATIO31         0x00008000 /* (PR)TX:RX DMA priority ratio 3:1                                          */
#define EMAC_V2_DMA_PRIO_RATIO41         0x0000C000 /* (PR)TX:RX DMA priority ratio 4:1                                          */
#define EMAC_V2_DMA_ARBITRATION          0x00000002 /* DMA Arbitration decides whether strict prio or RR  1      RW       0       */
#define EMAC_V2_DMA_ARBITRATION_STRICT   0x00000002 /* DMA Arbitration decides whether strict prio or RR  1      RW       0       */
#define EMAC_V2_DMA_ARBITRATION_RR       0x00000000 /* DMA Arbitration decides whether strict prio or RR  0      RW       0       */
#endif
#define EMAC_V2_DMA_FIXED_BURST_ENABLE  0x00010000 /* (FB)Fixed Burst SINGLE, INCR4, INCR8 or INCR16   16     RW                */
#define EMAC_V2_DMA_FIXED_BURST_DISABLE 0x00000000 /*             SINGLE, INCR                                          0       */
#define EMAC_V2_DMA_PRIO_RATIO11        0x00000000 /* (PR)TX:RX DMA priority ratio 1:1                15:14   RW        00      */
#define EMAC_V2_DMA_PRIO_RATIO21        0x00004000 /* (PR)TX:RX DMA priority ratio 2:1                                          */
#define EMAC_V2_DMA_PRIO_RATIO31        0x00008000 /* (PR)TX:RX DMA priority ratio 3:1                                          */
#define EMAC_V2_DMA_PRIO_RATIO41        0x0000C000 /* (PR)TX:RX DMA priority ratio 4:1                                          */

#define EMAC_V2_DMA_BURST_LEN_X8        0x01000000 /* When set mutiplies the PBL by 8                  24      RW        0      */
#define EMAC_V2_DMA_BURST_LEN_256       0x01002000 /*(DMABurstLengthx8 | DMABurstLength32) = 256      [24]:13:8                 */
#define EMAC_V2_DMA_BURST_LEN_128       0x01001000 /*(DMABurstLengthx8 | DMABurstLength16) = 128      [24]:13:8                 */
#define EMAC_V2_DMA_BURST_LEN_64        0x01000800 /*(DMABurstLengthx8 | DMABurstLength8) = 64        [24]:13:8                 */
#define EMAC_V2_DMA_BURST_LEN_32        0x00002000 /* (PBL) programmable DMA burst length = 32        13:8    RW                */
#define EMAC_V2_DMA_BURST_LEN_16        0x00001000 /* DMA burst length = 16                                                     */
#define EMAC_V2_DMA_BURST_LEN_8         0x00000800 /* DMA burst length = 8                                                      */
#define EMAC_V2_DMA_BURST_LEN_4         0x00000400 /* DMA burst length = 4                                                      */
#define EMAC_V2_DMA_BURST_LEN_2         0x00000200 /* DMA burst length = 2                                                      */
#define EMAC_V2_DMA_BURST_LEN_1         0x00000100 /* DMA burst length = 1                                                      */
#define EMAC_V2_DMA_BURST_LEN_0         0x00000000 /* DMA burst length = 0                                               0x00   */

#define EMAC_V2_DMA_DESCRIPTOR_8_WORDS  0x00000080 /* Enh Descriptor works  1=> 8 word descriptor      7                  0    */
#define EMAC_V2_DMA_DESCRIPTOR_4_WORDS  0x00000000 /* Enh Descriptor works  0=> 4 word descriptor      7                  0    */
#define EMAC_V2_DMA_DESCRIPTOR_SKIP_16  0x00000040 /* (DSL)Descriptor skip length (no.of dwords)       6:2     RW               */
#define EMAC_V2_DMA_DESCRIPTOR_SKIP_8   0x00000020 /* between two unchained descriptors                                         */
#define EMAC_V2_DMA_DESCRIPTOR_SKIP_4   0x00000010 /*                                                                           */
#define EMAC_V2_DMA_DESCRIPTOR_SKIP_2   0x00000008 /*                                                                           */
#define EMAC_V2_DMA_DESCRIPTOR_SKIP_1   0x00000004 /*                                                                           */
#define EMAC_V2_DMA_DESCRIPTOR_SKIP_0   0x00000000 /*                                                                    0x00   */
#define EMAC_V2_DMA_ARBIT_RR            0x00000000 /* (DA) DMA RR arbitration                            1     RW         0     */
#define EMAC_V2_DMA_ARBIT_PR            0x00000002 /* Rx has priority over Tx                                                   */
#define EMAC_V2_DMA_RESET_ON            0x00000001 /* (SWR)Software Reset DMA engine                     0     RW               */
#define EMAC_V2_DMA_RESET_OFF           0x00000000 /*                                                    0                      */

/*EMAC_V2_DMA_TX_POLL_DEMAND = 0x1004,    CSR1 - Transmit Poll Demand Register        */

/*EMAC_V2_DMA_RX_POLL_DEMAND = 0x1008,    CSR2 - Receive Poll Demand Register         */

/*EMAC_V2_DMA_RX_BASE_ADDR = 0x100C,  CSR3 - Receive Descriptor list base address       */

/*EMAC_V2_DMA_TX_BASE_ADDR = 0x1010,  CSR4 - Transmit Descriptor list base address      */

/*EMAC_V2_DMA_STATUS = 0x1014,  CSR5 - DMA status Register                        */
/*Bit 28 27 and 26 indicate whether the interrupt due to PMT EMAC_V2MMC or EMAC_V2 LINE Remaining bits are DMA interrupts*/
#ifdef EMAC_V2_AVB_SUPPORT
//#define EMAC_V2_DMA_SLOT_COUNTER_INTR      0x40000000    /* For Ch1 and Ch2 AVB slot interrupt status          31     RW       0       */
#endif
#ifdef EMAC_V2_LPI_SUPPORT
//#define EMAC_V2_LPI_INTR                   0x40000000    /* GMC LPI interrupt                                  31     RO       0       */
#endif
#define EMAC_V2_PMT_INTR                 0x20000000 /* (GPI)Gmac subsystem interrupt                      28     RO       0       */
//#define EMAC_V2_MMC_INTR                   0x08000000    /* (GMI)Gmac MMC subsystem interrupt                  27     RO       0       */
//#define EMAC_V2_LINE_INTF_INTR             0x04000000    /* Line interface interrupt                           26     RO       0       */
#define EMAC_V2_DMA_ERROR_BIT2           0x02000000 /* (EB)Error bits 0-data buffer, 1-desc. access       25     RO       0       */
#define EMAC_V2_DMA_ERROR_BIT1           0x01000000 /* (EB)Error bits 0-write trnsf, 1-read transfr       24     RO       0       */
#define EMAC_V2_DMA_ERROR_BIT0           0x00800000 /* (EB)Error bits 0-Rx DMA, 1-Tx DMA                  23     RO       0       */
#define EMAC_V2_DMA_TX_STATE             0x00700000 /* (TS)Transmit process state                         22:20  RO               */
#define EMAC_V2_DMA_TX_STOPPED           0x00000000 /* Stopped - Reset or Stop Tx Command issued                         000      */
#define EMAC_V2_DMA_TX_FETCHING          0x00100000 /* Running - fetching the Tx descriptor                                       */
#define EMAC_V2_DMA_TX_WAITING           0x00200000 /* Running - waiting for status                                               */
#define EMAC_V2_DMA_TX_READING           0x00300000 /* Running - reading the data from host memory                                */
#define EMAC_V2_DMA_TX_SUSPENDED         0x00600000 /* Suspended - Tx Descriptor unavailabe                                       */
#define EMAC_V2_DMA_TX_CLOSING           0x00700000 /* Running - closing Rx descriptor                                            */
#define EMAC_V2_DMA_RX_STATE             0x000E0000 /* (RS)Receive process state                         19:17  RO                */
#define EMAC_V2_DMA_RX_STOPPED           0x00000000 /* Stopped - Reset or Stop Rx Command issued                         000      */
#define EMAC_V2_DMA_RX_FETCHING          0x00020000 /* Running - fetching the Rx descriptor                                       */
#define EMAC_V2_DMA_RX_WAITING           0x00060000 /* Running - waiting for packet                                               */
#define EMAC_V2_DMA_RX_SUSPENDED         0x00080000 /* Suspended - Rx Descriptor unavailable                                      */
#define EMAC_V2_DMA_RX_CLOSING           0x000A0000 /* Running - closing descriptor                                               */
#define EMAC_V2_DMA_RX_QUEUING           0x000E0000 /* Running - queuing the recieve frame into host memory                       */
#define EMAC_V2_DMA_INT_NORMAL           0x00010000 /* (NIS)Normal interrupt summary                     16     RW        0       */
#define EMAC_V2_DMA_INT_ABNORMAL         0x00008000 /* (AIS)Abnormal interrupt summary                   15     RW        0       */
#define EMAC_V2_DMA_INT_EARLY_RX         0x00004000 /* Early receive interrupt (Normal)       RW        0       */
#define EMAC_V2_DMA_INT_BUS_ERROR        0x00002000 /* Fatal bus error (Abnormal)             RW        0       */
#define EMAC_V2_DMA_INT_EARLY_TX         0x00000400 /* Early transmit interrupt (Abnormal)    RW        0       */
#define EMAC_V2_DMA_INT_RX_WDOG_TO       0x00000200 /* Receive Watchdog Timeout (Abnormal)    RW        0       */
#define EMAC_V2_DMA_INT_RX_STOPPED       0x00000100 /* Receive process stopped (Abnormal)     RW        0       */
#define EMAC_V2_DMA_INT_RX_NOBUFFER      0x00000080 /* Receive buffer unavailable (Abnormal)  RW        0       */
#define EMAC_V2_DMA_INT_RX_COMPLETED     0x00000040 /* Completion of frame reception (Normal) RW        0       */
#define EMAC_V2_DMA_INT_TX_UNDERFLOW     0x00000020 /* Transmit underflow (Abnormal)          RW        0       */
#define EMAC_V2_DMA_INT_RCV_OVERFLOW     0x00000010 /* Receive Buffer overflow interrupt      RW        0       */
#define EMAC_V2_DMA_INT_TX_JABBER_TO     0x00000008 /* Transmit Jabber Timeout (Abnormal)     RW        0       */
#define EMAC_V2_DMA_INT_TX_NOBUFFER      0x00000004 /* Transmit buffer unavailable (Normal)   RW        0       */
#define EMAC_V2_DMA_INT_TX_STOPPED       0x00000002 /* Transmit process stopped (Abnormal)    RW        0       */
#define EMAC_V2_DMA_INT_TX_COMPLETED     0x00000001 /* Transmit completed (Normal)            RW        0       */

/*EMAC_V2_DMA_CONTROL = 0x1018,  CSR6 - DMA Operation Mode Register                */
#define EMAC_V2_DMA_DISABLE_DROP_TCP_CS  0x04000000 /* (DT) Dis. drop. of tcp/ip CS error frames        26      RW        0       */
#define EMAC_V2_DMA_STORE_AND_FORWARD    0x00200000 /* (SF)Store and forward                            21      RW        0       */
#define EMAC_V2_DMA_FLUSH_TX_FIFO        0x00100000 /* (FTF)Tx FIFO controller is reset to default      20      RW        0       */
#define EMAC_V2_DMA_TX_THRESH_CTRL       0x0001C000 /* (TTC)Controls thre Threh of MTL tx Fifo          16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_16    0x0001C000 /* (TTC)Controls thre Threh of MTL tx Fifo 16       16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_24    0x00018000 /* (TTC)Controls thre Threh of MTL tx Fifo 24       16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_32    0x00014000 /* (TTC)Controls thre Threh of MTL tx Fifo 32       16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_40    0x00010000 /* (TTC)Controls thre Threh of MTL tx Fifo 40       16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_256   0x0000c000 /* (TTC)Controls thre Threh of MTL tx Fifo 256      16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_192   0x00008000 /* (TTC)Controls thre Threh of MTL tx Fifo 192      16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_128   0x00004000 /* (TTC)Controls thre Threh of MTL tx Fifo 128      16:14   RW                */
#define EMAC_V2_DMA_TX_THRESH_CTRL_64    0x00000000 /* (TTC)Controls thre Threh of MTL tx Fifo 64       16:14   RW        000     */
#define EMAC_V2_DMA_TX_START             0x00002000 /* (ST)Start/Stop transmission                      13      RW        0       */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT    0x00401800 /* (RFD)Rx flow control deact. threhold             [22]:12:11   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT_1K 0x00000000 /* (RFD)Rx flow control deact. threhold (1kbytes)   [22]:12:11   RW        00       */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT_2K 0x00000800 /* (RFD)Rx flow control deact. threhold (2kbytes)   [22]:12:11   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT_3K 0x00001000 /* (RFD)Rx flow control deact. threhold (3kbytes)   [22]:12:11   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT_4K 0x00001800 /* (RFD)Rx flow control deact. threhold (4kbytes)   [22]:12:11   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT_5K 0x00400000 /* (RFD)Rx flow control deact. threhold (4kbytes)   [22]:12:11   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT_6K 0x00400800 /* (RFD)Rx flow control deact. threhold (4kbytes)   [22]:12:11   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_DEACT_7K 0x00401000 /* (RFD)Rx flow control deact. threhold (4kbytes)   [22]:12:11   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT      0x00800600 /* (RFA)Rx flow control Act. threhold              [23]:10:09   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT_1K   0x00000000 /* (RFA)Rx flow control Act. threhold (1kbytes)    [23]:10:09   RW        00       */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT_2K   0x00000200 /* (RFA)Rx flow control Act. threhold (2kbytes)    [23]:10:09   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT_3K   0x00000400 /* (RFA)Rx flow control Act. threhold (3kbytes)    [23]:10:09   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT_4K   0x00000300 /* (RFA)Rx flow control Act. threhold (4kbytes)    [23]:10:09   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT_5K   0x00800000 /* (RFA)Rx flow control Act. threhold (5kbytes)    [23]:10:09   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT_6K   0x00800200 /* (RFA)Rx flow control Act. threhold (6kbytes)    [23]:10:09   RW                 */
#define EMAC_V2_DMA_RX_FLOWCTRL_ACT_7K   0x00800400 /* (RFA)Rx flow control Act. threhold (7kbytes)    [23]:10:09   RW                 */
#define EMAC_V2_DMA_RX_THRESH_CTRL       0x00000018 /* (RTC)Controls thre Threh of MTL rx Fifo          4:3   RW                */
#define EMAC_V2_DMA_RX_THRESH_CTRL_64    0x00000000 /* (RTC)Controls thre Threh of MTL tx Fifo 64       4:3   RW                */
#define EMAC_V2_DMA_RX_THRESH_CTRL_32    0x00000008 /* (RTC)Controls thre Threh of MTL tx Fifo 32       4:3   RW                */
#define EMAC_V2_DMA_RX_THRESH_CTRL_96    0x00000010 /* (RTC)Controls thre Threh of MTL tx Fifo 96       4:3   RW                */
#define EMAC_V2_DMA_RX_THRESH_CTRL_128   0x00000018 /* (RTC)Controls thre Threh of MTL tx Fifo 128      4:3   RW                */
#define EMAC_V2_DMA_EN_HW_FLOWCTRL       0x00000100 /* (EFC)Enable HW flow control                      8       RW                 */
#define EMAC_V2_DMA_DIS_HW_FLOWCTRL      0x00000000 /* Disable HW flow control                                            0        */
#define EMAC_V2_DMA_FWD_ERROR_FRAMES     0x00000080 /* (FEF)Forward error frames                        7       RW        0       */
#define EMAC_V2_DMA_FWD_UNDERSZ_FRAMES   0x00000040 /* (FUF)Forward undersize frames                    6       RW        0       */
#define EMAC_V2_DMA_TX_SECOND_FRAME      0x00000004 /* (OSF)Operate on second frame                     4       RW        0       */
#define EMAC_V2_DMA_RX_START             0x00000002 /* (SR)Start/Stop reception                         1       RW        0       */

/*EMAC_V2_DMA_INTERRUPT = 0x101C,  CSR7 - Interrupt enable                           */
#define EMAC_V2_DMA_IE_NORMAL            EMAC_V2_DMA_INT_NORMAL       /* Normal interrupt enable                 RW        0       */
#define EMAC_V2_DMA_IE_ABNORMAL          EMAC_V2_DMA_INT_ABNORMAL     /* Abnormal interrupt enable               RW        0       */
#define EMAC_V2_DMA_IE_EARLY_RX          EMAC_V2_DMA_INT_EARLY_RX     /* Early receive interrupt enable          RW        0       */
#define EMAC_V2_DMA_IE_BUS_ERROR         EMAC_V2_DMA_INT_BUS_ERROR    /* Fatal bus error enable                  RW        0       */
#define EMAC_V2_DMA_IE_EARLY_TX          EMAC_V2_DMA_INT_EARLY_TX     /* Early transmit interrupt enable         RW        0       */
#define EMAC_V2_DMA_IE_RX_WDOG_TO        EMAC_V2_DMA_INT_RX_WDOG_TO   /* Receive Watchdog Timeout enable         RW        0       */
#define EMAC_V2_DMA_IE_RX_STOPPED        EMAC_V2_DMA_INT_RX_STOPPED   /* Receive process stopped enable          RW        0       */
#define EMAC_V2_DMA_IE_RX_NOBUFFER       EMAC_V2_DMA_INT_RX_NOBUFFER  /* Receive buffer unavailable enable       RW        0       */
#define EMAC_V2_DMA_IE_RX_COMPLETED      EMAC_V2_DMA_INT_RX_COMPLETED /* Completion of frame reception enable    RW        0       */
#define EMAC_V2_DMA_IE_TX_UNDERFLOW      EMAC_V2_DMA_INT_TX_UNDERFLOW /* Transmit underflow enable               RW        0       */
#define EMAC_V2_DMA_IE_RX_OVERFLOW       EMAC_V2_DMA_INT_RCV_OVERFLOW /* Receive Buffer overflow interrupt       RW        0       */
#define EMAC_V2_DMA_IE_TX_JABBER_TO      EMAC_V2_DMA_INT_TX_JABBER_TO /* Transmit Jabber Timeout enable          RW        0       */
#define EMAC_V2_DMA_IE_TX_NOBUFFER       EMAC_V2_DMA_INT_TX_NOBUFFER  /* Transmit buffer unavailable enable      RW        0       */
#define EMAC_V2_DMA_IE_TX_STOPPED        EMAC_V2_DMA_INT_TX_STOPPED   /* Transmit process stopped enable         RW        0       */
#define EMAC_V2_DMA_IE_TX_COMPLETED      EMAC_V2_DMA_INT_TX_COMPLETED /* Transmit completed enable               RW        0       */

/*EMAC_V2_DMA_MISSED_FR = 0x1020,  CSR8 - Missed Frame & Buffer overflow Counter     */
/*EMAC_V2_DMA_TX_CURR_DESC = 0x1048,  Current host Tx Desc Register              */
/*EMAC_V2_DMA_RX_CURR_DESC = 0x104C,  Current host Rx Desc Register              */
/*EMAC_V2_DMA_TX_CURR_ADDR = 0x1050,  CSR20 - Current host transmit buffer address      */
/*EMAC_V2_DMA_RX_CURR_ADDR = 0x1054,  CSR21 - Current host receive buffer address       */

#endif /* __HARDWARE_EMAC_V2_H__ */