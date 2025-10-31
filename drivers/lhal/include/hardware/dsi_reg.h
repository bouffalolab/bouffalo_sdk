#ifndef __DSI_REG_H__
#define __DSI_REG_H__

/* 0x0 : dsi_config */
#define DSI_CONFIG_OFFSET        (0x0)
#define DSI_CR_HSTX_EN           DSI_CR_HSTX_EN
#define DSI_CR_HSTX_EN_POS       (0U)
#define DSI_CR_HSTX_EN_LEN       (1U)
#define DSI_CR_HSTX_EN_MSK       (((1U << DSI_CR_HSTX_EN_LEN) - 1) << DSI_CR_HSTX_EN_POS)
#define DSI_CR_HSTX_EN_UMSK      (~(((1U << DSI_CR_HSTX_EN_LEN) - 1) << DSI_CR_HSTX_EN_POS))
#define DSI_CR_LANE_NUM          DSI_CR_LANE_NUM
#define DSI_CR_LANE_NUM_POS      (1U)
#define DSI_CR_LANE_NUM_LEN      (2U)
#define DSI_CR_LANE_NUM_MSK      (((1U << DSI_CR_LANE_NUM_LEN) - 1) << DSI_CR_LANE_NUM_POS)
#define DSI_CR_LANE_NUM_UMSK     (~(((1U << DSI_CR_LANE_NUM_LEN) - 1) << DSI_CR_LANE_NUM_POS))
#define DSI_CR_HSTX_MODE         DSI_CR_HSTX_MODE
#define DSI_CR_HSTX_MODE_POS     (4U)
#define DSI_CR_HSTX_MODE_LEN     (1U)
#define DSI_CR_HSTX_MODE_MSK     (((1U << DSI_CR_HSTX_MODE_LEN) - 1) << DSI_CR_HSTX_MODE_POS)
#define DSI_CR_HSTX_MODE_UMSK    (~(((1U << DSI_CR_HSTX_MODE_LEN) - 1) << DSI_CR_HSTX_MODE_POS))
#define DSI_CR_LSE_PKT_EN        DSI_CR_LSE_PKT_EN
#define DSI_CR_LSE_PKT_EN_POS    (5U)
#define DSI_CR_LSE_PKT_EN_LEN    (1U)
#define DSI_CR_LSE_PKT_EN_MSK    (((1U << DSI_CR_LSE_PKT_EN_LEN) - 1) << DSI_CR_LSE_PKT_EN_POS)
#define DSI_CR_LSE_PKT_EN_UMSK   (~(((1U << DSI_CR_LSE_PKT_EN_LEN) - 1) << DSI_CR_LSE_PKT_EN_POS))
#define DSI_CR_LANE_MUX_SEL      DSI_CR_LANE_MUX_SEL
#define DSI_CR_LANE_MUX_SEL_POS  (6U)
#define DSI_CR_LANE_MUX_SEL_LEN  (2U)
#define DSI_CR_LANE_MUX_SEL_MSK  (((1U << DSI_CR_LANE_MUX_SEL_LEN) - 1) << DSI_CR_LANE_MUX_SEL_POS)
#define DSI_CR_LANE_MUX_SEL_UMSK (~(((1U << DSI_CR_LANE_MUX_SEL_LEN) - 1) << DSI_CR_LANE_MUX_SEL_POS))
#define DSI_CR_DT                DSI_CR_DT
#define DSI_CR_DT_POS            (8U)
#define DSI_CR_DT_LEN            (6U)
#define DSI_CR_DT_MSK            (((1U << DSI_CR_DT_LEN) - 1) << DSI_CR_DT_POS)
#define DSI_CR_DT_UMSK           (~(((1U << DSI_CR_DT_LEN) - 1) << DSI_CR_DT_POS))
#define DSI_CR_VC                DSI_CR_VC
#define DSI_CR_VC_POS            (14U)
#define DSI_CR_VC_LEN            (2U)
#define DSI_CR_VC_MSK            (((1U << DSI_CR_VC_LEN) - 1) << DSI_CR_VC_POS)
#define DSI_CR_VC_UMSK           (~(((1U << DSI_CR_VC_LEN) - 1) << DSI_CR_VC_POS))
#define DSI_CR_HSTX_VFP          DSI_CR_HSTX_VFP
#define DSI_CR_HSTX_VFP_POS      (16U)
#define DSI_CR_HSTX_VFP_LEN      (8U)
#define DSI_CR_HSTX_VFP_MSK      (((1U << DSI_CR_HSTX_VFP_LEN) - 1) << DSI_CR_HSTX_VFP_POS)
#define DSI_CR_HSTX_VFP_UMSK     (~(((1U << DSI_CR_HSTX_VFP_LEN) - 1) << DSI_CR_HSTX_VFP_POS))
#define DSI_CR_HSTX_VSA          DSI_CR_HSTX_VSA
#define DSI_CR_HSTX_VSA_POS      (24U)
#define DSI_CR_HSTX_VSA_LEN      (8U)
#define DSI_CR_HSTX_VSA_MSK      (((1U << DSI_CR_HSTX_VSA_LEN) - 1) << DSI_CR_HSTX_VSA_POS)
#define DSI_CR_HSTX_VSA_UMSK     (~(((1U << DSI_CR_HSTX_VSA_LEN) - 1) << DSI_CR_HSTX_VSA_POS))

/* 0x4 : dsi_esc_config */
#define DSI_ESC_CONFIG_OFFSET        (0x4)
#define DSI_CR_ESC_TX_EN             DSI_CR_ESC_TX_EN
#define DSI_CR_ESC_TX_EN_POS         (0U)
#define DSI_CR_ESC_TX_EN_LEN         (1U)
#define DSI_CR_ESC_TX_EN_MSK         (((1U << DSI_CR_ESC_TX_EN_LEN) - 1) << DSI_CR_ESC_TX_EN_POS)
#define DSI_CR_ESC_TX_EN_UMSK        (~(((1U << DSI_CR_ESC_TX_EN_LEN) - 1) << DSI_CR_ESC_TX_EN_POS))
#define DSI_CR_ESC_TX_MODE           DSI_CR_ESC_TX_MODE
#define DSI_CR_ESC_TX_MODE_POS       (1U)
#define DSI_CR_ESC_TX_MODE_LEN       (2U)
#define DSI_CR_ESC_TX_MODE_MSK       (((1U << DSI_CR_ESC_TX_MODE_LEN) - 1) << DSI_CR_ESC_TX_MODE_POS)
#define DSI_CR_ESC_TX_MODE_UMSK      (~(((1U << DSI_CR_ESC_TX_MODE_LEN) - 1) << DSI_CR_ESC_TX_MODE_POS))
#define DSI_CR_ESC_TX_ULPS_EXIT      DSI_CR_ESC_TX_ULPS_EXIT
#define DSI_CR_ESC_TX_ULPS_EXIT_POS  (3U)
#define DSI_CR_ESC_TX_ULPS_EXIT_LEN  (1U)
#define DSI_CR_ESC_TX_ULPS_EXIT_MSK  (((1U << DSI_CR_ESC_TX_ULPS_EXIT_LEN) - 1) << DSI_CR_ESC_TX_ULPS_EXIT_POS)
#define DSI_CR_ESC_TX_ULPS_EXIT_UMSK (~(((1U << DSI_CR_ESC_TX_ULPS_EXIT_LEN) - 1) << DSI_CR_ESC_TX_ULPS_EXIT_POS))
#define DSI_CR_ESC_TX_TRIG           DSI_CR_ESC_TX_TRIG
#define DSI_CR_ESC_TX_TRIG_POS       (4U)
#define DSI_CR_ESC_TX_TRIG_LEN       (4U)
#define DSI_CR_ESC_TX_TRIG_MSK       (((1U << DSI_CR_ESC_TX_TRIG_LEN) - 1) << DSI_CR_ESC_TX_TRIG_POS)
#define DSI_CR_ESC_TX_TRIG_UMSK      (~(((1U << DSI_CR_ESC_TX_TRIG_LEN) - 1) << DSI_CR_ESC_TX_TRIG_POS))
#define DSI_CR_ESC_RX_EN             DSI_CR_ESC_RX_EN
#define DSI_CR_ESC_RX_EN_POS         (16U)
#define DSI_CR_ESC_RX_EN_LEN         (1U)
#define DSI_CR_ESC_RX_EN_MSK         (((1U << DSI_CR_ESC_RX_EN_LEN) - 1) << DSI_CR_ESC_RX_EN_POS)
#define DSI_CR_ESC_RX_EN_UMSK        (~(((1U << DSI_CR_ESC_RX_EN_LEN) - 1) << DSI_CR_ESC_RX_EN_POS))
#define DSI_ST_ESC_RX_LEN            DSI_ST_ESC_RX_LEN
#define DSI_ST_ESC_RX_LEN_POS        (24U)
#define DSI_ST_ESC_RX_LEN_LEN        (8U)
#define DSI_ST_ESC_RX_LEN_MSK        (((1U << DSI_ST_ESC_RX_LEN_LEN) - 1) << DSI_ST_ESC_RX_LEN_POS)
#define DSI_ST_ESC_RX_LEN_UMSK       (~(((1U << DSI_ST_ESC_RX_LEN_LEN) - 1) << DSI_ST_ESC_RX_LEN_POS))

/* 0x8 : dsi_lpdt_tx_config */
#define DSI_LPDT_TX_CONFIG_OFFSET (0x8)
#define DSI_CR_LPDT_PDLEN         DSI_CR_LPDT_PDLEN
#define DSI_CR_LPDT_PDLEN_POS     (0U)
#define DSI_CR_LPDT_PDLEN_LEN     (8U)
#define DSI_CR_LPDT_PDLEN_MSK     (((1U << DSI_CR_LPDT_PDLEN_LEN) - 1) << DSI_CR_LPDT_PDLEN_POS)
#define DSI_CR_LPDT_PDLEN_UMSK    (~(((1U << DSI_CR_LPDT_PDLEN_LEN) - 1) << DSI_CR_LPDT_PDLEN_POS))
#define DSI_CR_LPDT_VC            DSI_CR_LPDT_VC
#define DSI_CR_LPDT_VC_POS        (8U)
#define DSI_CR_LPDT_VC_LEN        (2U)
#define DSI_CR_LPDT_VC_MSK        (((1U << DSI_CR_LPDT_VC_LEN) - 1) << DSI_CR_LPDT_VC_POS)
#define DSI_CR_LPDT_VC_UMSK       (~(((1U << DSI_CR_LPDT_VC_LEN) - 1) << DSI_CR_LPDT_VC_POS))
#define DSI_CR_LPDT_DI            DSI_CR_LPDT_DI
#define DSI_CR_LPDT_DI_POS        (10U)
#define DSI_CR_LPDT_DI_LEN        (6U)
#define DSI_CR_LPDT_DI_MSK        (((1U << DSI_CR_LPDT_DI_LEN) - 1) << DSI_CR_LPDT_DI_POS)
#define DSI_CR_LPDT_DI_UMSK       (~(((1U << DSI_CR_LPDT_DI_LEN) - 1) << DSI_CR_LPDT_DI_POS))
#define DSI_CR_LPDT_WORD0         DSI_CR_LPDT_WORD0
#define DSI_CR_LPDT_WORD0_POS     (16U)
#define DSI_CR_LPDT_WORD0_LEN     (8U)
#define DSI_CR_LPDT_WORD0_MSK     (((1U << DSI_CR_LPDT_WORD0_LEN) - 1) << DSI_CR_LPDT_WORD0_POS)
#define DSI_CR_LPDT_WORD0_UMSK    (~(((1U << DSI_CR_LPDT_WORD0_LEN) - 1) << DSI_CR_LPDT_WORD0_POS))
#define DSI_CR_LPDT_WORD1         DSI_CR_LPDT_WORD1
#define DSI_CR_LPDT_WORD1_POS     (24U)
#define DSI_CR_LPDT_WORD1_LEN     (8U)
#define DSI_CR_LPDT_WORD1_MSK     (((1U << DSI_CR_LPDT_WORD1_LEN) - 1) << DSI_CR_LPDT_WORD1_POS)
#define DSI_CR_LPDT_WORD1_UMSK    (~(((1U << DSI_CR_LPDT_WORD1_LEN) - 1) << DSI_CR_LPDT_WORD1_POS))

/* 0xC : dsi_hstx_config */
#define DSI_HSTX_CONFIG_OFFSET  (0xC)
#define DSI_CR_HSTX_PC          DSI_CR_HSTX_PC
#define DSI_CR_HSTX_PC_POS      (0U)
#define DSI_CR_HSTX_PC_LEN      (11U)
#define DSI_CR_HSTX_PC_MSK      (((1U << DSI_CR_HSTX_PC_LEN) - 1) << DSI_CR_HSTX_PC_POS)
#define DSI_CR_HSTX_PC_UMSK     (~(((1U << DSI_CR_HSTX_PC_LEN) - 1) << DSI_CR_HSTX_PC_POS))
#define DSI_CR_HSTX_OUT_TH      DSI_CR_HSTX_OUT_TH
#define DSI_CR_HSTX_OUT_TH_POS  (16U)
#define DSI_CR_HSTX_OUT_TH_LEN  (11U)
#define DSI_CR_HSTX_OUT_TH_MSK  (((1U << DSI_CR_HSTX_OUT_TH_LEN) - 1) << DSI_CR_HSTX_OUT_TH_POS)
#define DSI_CR_HSTX_OUT_TH_UMSK (~(((1U << DSI_CR_HSTX_OUT_TH_LEN) - 1) << DSI_CR_HSTX_OUT_TH_POS))

/* 0x10 : dsi_int_status */
#define DSI_INT_STATUS_OFFSET (0x10)
#define DSI_INT_STATUS        DSI_INT_STATUS
#define DSI_INT_STATUS_POS    (0U)
#define DSI_INT_STATUS_LEN    (14U)
#define DSI_INT_STATUS_MSK    (((1U << DSI_INT_STATUS_LEN) - 1) << DSI_INT_STATUS_POS)
#define DSI_INT_STATUS_UMSK   (~(((1U << DSI_INT_STATUS_LEN) - 1) << DSI_INT_STATUS_POS))

/* 0x14 : dsi_int_mask */
#define DSI_INT_MASK_OFFSET (0x14)
#define DSI_INT_MASK        DSI_INT_MASK
#define DSI_INT_MASK_POS    (0U)
#define DSI_INT_MASK_LEN    (14U)
#define DSI_INT_MASK_MSK    (((1U << DSI_INT_MASK_LEN) - 1) << DSI_INT_MASK_POS)
#define DSI_INT_MASK_UMSK   (~(((1U << DSI_INT_MASK_LEN) - 1) << DSI_INT_MASK_POS))

/* 0x18 : dsi_int_clear */
#define DSI_INT_CLEAR_OFFSET (0x18)
#define DSI_INT_CLEAR        DSI_INT_CLEAR
#define DSI_INT_CLEAR_POS    (0U)
#define DSI_INT_CLEAR_LEN    (14U)
#define DSI_INT_CLEAR_MSK    (((1U << DSI_INT_CLEAR_LEN) - 1) << DSI_INT_CLEAR_POS)
#define DSI_INT_CLEAR_UMSK   (~(((1U << DSI_INT_CLEAR_LEN) - 1) << DSI_INT_CLEAR_POS))

/* 0x1C : dsi_int_enable */
#define DSI_INT_ENABLE_OFFSET (0x1C)
#define DSI_INT_ENABLE        DSI_INT_ENABLE
#define DSI_INT_ENABLE_POS    (0U)
#define DSI_INT_ENABLE_LEN    (14U)
#define DSI_INT_ENABLE_MSK    (((1U << DSI_INT_ENABLE_LEN) - 1) << DSI_INT_ENABLE_POS)
#define DSI_INT_ENABLE_UMSK   (~(((1U << DSI_INT_ENABLE_LEN) - 1) << DSI_INT_ENABLE_POS))

/* 0x20 : dsi_frame_num */
#define DSI_FRAME_NUM_OFFSET  (0x20)
#define DSI_CR_FRAME_NUM      DSI_CR_FRAME_NUM
#define DSI_CR_FRAME_NUM_POS  (0U)
#define DSI_CR_FRAME_NUM_LEN  (16U)
#define DSI_CR_FRAME_NUM_MSK  (((1U << DSI_CR_FRAME_NUM_LEN) - 1) << DSI_CR_FRAME_NUM_POS)
#define DSI_CR_FRAME_NUM_UMSK (~(((1U << DSI_CR_FRAME_NUM_LEN) - 1) << DSI_CR_FRAME_NUM_POS))
#define DSI_CR_FRAME_UPD      DSI_CR_FRAME_UPD
#define DSI_CR_FRAME_UPD_POS  (31U)
#define DSI_CR_FRAME_UPD_LEN  (1U)
#define DSI_CR_FRAME_UPD_MSK  (((1U << DSI_CR_FRAME_UPD_LEN) - 1) << DSI_CR_FRAME_UPD_POS)
#define DSI_CR_FRAME_UPD_UMSK (~(((1U << DSI_CR_FRAME_UPD_LEN) - 1) << DSI_CR_FRAME_UPD_POS))

/* 0x60 : dsi_fifo_config_0 */
#define DSI_FIFO_CONFIG_0_OFFSET   (0x60)
#define DSI_DMA_TX_EN              DSI_DMA_TX_EN
#define DSI_DMA_TX_EN_POS          (0U)
#define DSI_DMA_TX_EN_LEN          (1U)
#define DSI_DMA_TX_EN_MSK          (((1U << DSI_DMA_TX_EN_LEN) - 1) << DSI_DMA_TX_EN_POS)
#define DSI_DMA_TX_EN_UMSK         (~(((1U << DSI_DMA_TX_EN_LEN) - 1) << DSI_DMA_TX_EN_POS))
#define DSI_DMA_RX_EN              DSI_DMA_RX_EN
#define DSI_DMA_RX_EN_POS          (1U)
#define DSI_DMA_RX_EN_LEN          (1U)
#define DSI_DMA_RX_EN_MSK          (((1U << DSI_DMA_RX_EN_LEN) - 1) << DSI_DMA_RX_EN_POS)
#define DSI_DMA_RX_EN_UMSK         (~(((1U << DSI_DMA_RX_EN_LEN) - 1) << DSI_DMA_RX_EN_POS))
#define DSI_TX_FIFO_CLR            DSI_TX_FIFO_CLR
#define DSI_TX_FIFO_CLR_POS        (2U)
#define DSI_TX_FIFO_CLR_LEN        (1U)
#define DSI_TX_FIFO_CLR_MSK        (((1U << DSI_TX_FIFO_CLR_LEN) - 1) << DSI_TX_FIFO_CLR_POS)
#define DSI_TX_FIFO_CLR_UMSK       (~(((1U << DSI_TX_FIFO_CLR_LEN) - 1) << DSI_TX_FIFO_CLR_POS))
#define DSI_RX_FIFO_CLR            DSI_RX_FIFO_CLR
#define DSI_RX_FIFO_CLR_POS        (3U)
#define DSI_RX_FIFO_CLR_LEN        (1U)
#define DSI_RX_FIFO_CLR_MSK        (((1U << DSI_RX_FIFO_CLR_LEN) - 1) << DSI_RX_FIFO_CLR_POS)
#define DSI_RX_FIFO_CLR_UMSK       (~(((1U << DSI_RX_FIFO_CLR_LEN) - 1) << DSI_RX_FIFO_CLR_POS))
#define DSI_TX_FIFO_OVERFLOW       DSI_TX_FIFO_OVERFLOW
#define DSI_TX_FIFO_OVERFLOW_POS   (4U)
#define DSI_TX_FIFO_OVERFLOW_LEN   (1U)
#define DSI_TX_FIFO_OVERFLOW_MSK   (((1U << DSI_TX_FIFO_OVERFLOW_LEN) - 1) << DSI_TX_FIFO_OVERFLOW_POS)
#define DSI_TX_FIFO_OVERFLOW_UMSK  (~(((1U << DSI_TX_FIFO_OVERFLOW_LEN) - 1) << DSI_TX_FIFO_OVERFLOW_POS))
#define DSI_TX_FIFO_UNDERFLOW      DSI_TX_FIFO_UNDERFLOW
#define DSI_TX_FIFO_UNDERFLOW_POS  (5U)
#define DSI_TX_FIFO_UNDERFLOW_LEN  (1U)
#define DSI_TX_FIFO_UNDERFLOW_MSK  (((1U << DSI_TX_FIFO_UNDERFLOW_LEN) - 1) << DSI_TX_FIFO_UNDERFLOW_POS)
#define DSI_TX_FIFO_UNDERFLOW_UMSK (~(((1U << DSI_TX_FIFO_UNDERFLOW_LEN) - 1) << DSI_TX_FIFO_UNDERFLOW_POS))
#define DSI_RX_FIFO_OVERFLOW       DSI_RX_FIFO_OVERFLOW
#define DSI_RX_FIFO_OVERFLOW_POS   (6U)
#define DSI_RX_FIFO_OVERFLOW_LEN   (1U)
#define DSI_RX_FIFO_OVERFLOW_MSK   (((1U << DSI_RX_FIFO_OVERFLOW_LEN) - 1) << DSI_RX_FIFO_OVERFLOW_POS)
#define DSI_RX_FIFO_OVERFLOW_UMSK  (~(((1U << DSI_RX_FIFO_OVERFLOW_LEN) - 1) << DSI_RX_FIFO_OVERFLOW_POS))
#define DSI_RX_FIFO_UNDERFLOW      DSI_RX_FIFO_UNDERFLOW
#define DSI_RX_FIFO_UNDERFLOW_POS  (7U)
#define DSI_RX_FIFO_UNDERFLOW_LEN  (1U)
#define DSI_RX_FIFO_UNDERFLOW_MSK  (((1U << DSI_RX_FIFO_UNDERFLOW_LEN) - 1) << DSI_RX_FIFO_UNDERFLOW_POS)
#define DSI_RX_FIFO_UNDERFLOW_UMSK (~(((1U << DSI_RX_FIFO_UNDERFLOW_LEN) - 1) << DSI_RX_FIFO_UNDERFLOW_POS))

/* 0x64 : dsi_fifo_config_1 */
#define DSI_FIFO_CONFIG_1_OFFSET (0x64)
#define DSI_TX_FIFO_CNT          DSI_TX_FIFO_CNT
#define DSI_TX_FIFO_CNT_POS      (0U)
#define DSI_TX_FIFO_CNT_LEN      (3U)
#define DSI_TX_FIFO_CNT_MSK      (((1U << DSI_TX_FIFO_CNT_LEN) - 1) << DSI_TX_FIFO_CNT_POS)
#define DSI_TX_FIFO_CNT_UMSK     (~(((1U << DSI_TX_FIFO_CNT_LEN) - 1) << DSI_TX_FIFO_CNT_POS))
#define DSI_RX_FIFO_CNT          DSI_RX_FIFO_CNT
#define DSI_RX_FIFO_CNT_POS      (8U)
#define DSI_RX_FIFO_CNT_LEN      (3U)
#define DSI_RX_FIFO_CNT_MSK      (((1U << DSI_RX_FIFO_CNT_LEN) - 1) << DSI_RX_FIFO_CNT_POS)
#define DSI_RX_FIFO_CNT_UMSK     (~(((1U << DSI_RX_FIFO_CNT_LEN) - 1) << DSI_RX_FIFO_CNT_POS))
#define DSI_TX_FIFO_TH           DSI_TX_FIFO_TH
#define DSI_TX_FIFO_TH_POS       (16U)
#define DSI_TX_FIFO_TH_LEN       (2U)
#define DSI_TX_FIFO_TH_MSK       (((1U << DSI_TX_FIFO_TH_LEN) - 1) << DSI_TX_FIFO_TH_POS)
#define DSI_TX_FIFO_TH_UMSK      (~(((1U << DSI_TX_FIFO_TH_LEN) - 1) << DSI_TX_FIFO_TH_POS))
#define DSI_RX_FIFO_TH           DSI_RX_FIFO_TH
#define DSI_RX_FIFO_TH_POS       (24U)
#define DSI_RX_FIFO_TH_LEN       (2U)
#define DSI_RX_FIFO_TH_MSK       (((1U << DSI_RX_FIFO_TH_LEN) - 1) << DSI_RX_FIFO_TH_POS)
#define DSI_RX_FIFO_TH_UMSK      (~(((1U << DSI_RX_FIFO_TH_LEN) - 1) << DSI_RX_FIFO_TH_POS))

/* 0x68 : dsi_fifo_wdata */
#define DSI_FIFO_WDATA_OFFSET (0x68)
#define DSI_FIFO_WDATA        DSI_FIFO_WDATA
#define DSI_FIFO_WDATA_POS    (0U)
#define DSI_FIFO_WDATA_LEN    (32U)
#define DSI_FIFO_WDATA_MSK    (((1U << DSI_FIFO_WDATA_LEN) - 1) << DSI_FIFO_WDATA_POS)
#define DSI_FIFO_WDATA_UMSK   (~(((1U << DSI_FIFO_WDATA_LEN) - 1) << DSI_FIFO_WDATA_POS))

/* 0x6C : dsi_fifo_rdata */
#define DSI_FIFO_RDATA_OFFSET (0x6C)
#define DSI_FIFO_RDATA        DSI_FIFO_RDATA
#define DSI_FIFO_RDATA_POS    (0U)
#define DSI_FIFO_RDATA_LEN    (32U)
#define DSI_FIFO_RDATA_MSK    (((1U << DSI_FIFO_RDATA_LEN) - 1) << DSI_FIFO_RDATA_POS)
#define DSI_FIFO_RDATA_UMSK   (~(((1U << DSI_FIFO_RDATA_LEN) - 1) << DSI_FIFO_RDATA_POS))

/* 0xFC : dummy_reg */
#define DSI_DUMMY_REG_OFFSET (0xFC)
#define DSI_DUMMY_REG        DSI_DUMMY_REG
#define DSI_DUMMY_REG_POS    (0U)
#define DSI_DUMMY_REG_LEN    (32U)
#define DSI_DUMMY_REG_MSK    (((1U << DSI_DUMMY_REG_LEN) - 1) << DSI_DUMMY_REG_POS)
#define DSI_DUMMY_REG_UMSK   (~(((1U << DSI_DUMMY_REG_LEN) - 1) << DSI_DUMMY_REG_POS))

/* 0x2C : dsi_ana_ctrl */
#define DSI_ANA_CTRL_OFFSET                                     (0x2C)
#define DSI_TEN_DSI                                             DSI_TEN_DSI
#define DSI_TEN_DSI_POS                                         (0U)
#define DSI_TEN_DSI_LEN                                         (1U)
#define DSI_TEN_DSI_MSK                                         (((1U<<DSI_TEN_DSI_LEN)-1)<<DSI_TEN_DSI_POS)
#define DSI_TEN_DSI_UMSK                                        (~(((1U<<DSI_TEN_DSI_LEN)-1)<<DSI_TEN_DSI_POS))
#define DSI_LPRX_BW_SEL                                         DSI_LPRX_BW_SEL
#define DSI_LPRX_BW_SEL_POS                                     (4U)
#define DSI_LPRX_BW_SEL_LEN                                     (4U)
#define DSI_LPRX_BW_SEL_MSK                                     (((1U<<DSI_LPRX_BW_SEL_LEN)-1)<<DSI_LPRX_BW_SEL_POS)
#define DSI_LPRX_BW_SEL_UMSK                                    (~(((1U<<DSI_LPRX_BW_SEL_LEN)-1)<<DSI_LPRX_BW_SEL_POS))
#define DSI_HSTX_BYTE_CLK_POL                                   DSI_HSTX_BYTE_CLK_POL
#define DSI_HSTX_BYTE_CLK_POL_POS                               (12U)
#define DSI_HSTX_BYTE_CLK_POL_LEN                               (1U)
#define DSI_HSTX_BYTE_CLK_POL_MSK                               (((1U<<DSI_HSTX_BYTE_CLK_POL_LEN)-1)<<DSI_HSTX_BYTE_CLK_POL_POS)
#define DSI_HSTX_BYTE_CLK_POL_UMSK                              (~(((1U<<DSI_HSTX_BYTE_CLK_POL_LEN)-1)<<DSI_HSTX_BYTE_CLK_POL_POS))
#define DSI_VREG400_VREF_SEL                                    DSI_VREG400_VREF_SEL
#define DSI_VREG400_VREF_SEL_POS                                (16U)
#define DSI_VREG400_VREF_SEL_LEN                                (3U)
#define DSI_VREG400_VREF_SEL_MSK                                (((1U<<DSI_VREG400_VREF_SEL_LEN)-1)<<DSI_VREG400_VREF_SEL_POS)
#define DSI_VREG400_VREF_SEL_UMSK                               (~(((1U<<DSI_VREG400_VREF_SEL_LEN)-1)<<DSI_VREG400_VREF_SEL_POS))
#define DSI_VREG400_EN_V_VREF                                   DSI_VREG400_EN_V_VREF
#define DSI_VREG400_EN_V_VREF_POS                               (20U)
#define DSI_VREG400_EN_V_VREF_LEN                               (1U)
#define DSI_VREG400_EN_V_VREF_MSK                               (((1U<<DSI_VREG400_EN_V_VREF_LEN)-1)<<DSI_VREG400_EN_V_VREF_POS)
#define DSI_VREG400_EN_V_VREF_UMSK                              (~(((1U<<DSI_VREG400_EN_V_VREF_LEN)-1)<<DSI_VREG400_EN_V_VREF_POS))
#define DSI_VREG400_EN_I_VREF                                   DSI_VREG400_EN_I_VREF
#define DSI_VREG400_EN_I_VREF_POS                               (24U)
#define DSI_VREG400_EN_I_VREF_LEN                               (1U)
#define DSI_VREG400_EN_I_VREF_MSK                               (((1U<<DSI_VREG400_EN_I_VREF_LEN)-1)<<DSI_VREG400_EN_I_VREF_POS)
#define DSI_VREG400_EN_I_VREF_UMSK                              (~(((1U<<DSI_VREG400_EN_I_VREF_LEN)-1)<<DSI_VREG400_EN_I_VREF_POS))
#define DSI_EN                                                  DSI_EN
#define DSI_EN_POS                                              (28U)
#define DSI_EN_LEN                                              (1U)
#define DSI_EN_MSK                                              (((1U<<DSI_EN_LEN)-1)<<DSI_EN_POS)
#define DSI_EN_UMSK                                             (~(((1U<<DSI_EN_LEN)-1)<<DSI_EN_POS))

/* 0x30 : dsi_dly_ctrl */
#define DSI_DLY_CTRL_OFFSET                                     (0x30)
#define DSI_HSTX_DATA_DLY_15_0                                 DSI_HSTX_DATA_DLY_15_0
#define DSI_HSTX_DATA_DLY_15_0_POS                             (0U)
#define DSI_HSTX_DATA_DLY_15_0_LEN                             (16U)
#define DSI_HSTX_DATA_DLY_15_0_MSK                             (((1U<<DSI_HSTX_DATA_DLY_15_0_LEN)-1)<<DSI_HSTX_DATA_DLY_15_0_POS)
#define DSI_HSTX_DATA_DLY_15_0_UMSK                            (~(((1U<<DSI_HSTX_DATA_DLY_15_0_LEN)-1)<<DSI_HSTX_DATA_DLY_15_0_POS))
#define DSI_HSTX_CLK_DLY                                        DSI_HSTX_CLK_DLY
#define DSI_HSTX_CLK_DLY_POS                                    (24U)
#define DSI_HSTX_CLK_DLY_LEN                                    (4U)
#define DSI_HSTX_CLK_DLY_MSK                                    (((1U<<DSI_HSTX_CLK_DLY_LEN)-1)<<DSI_HSTX_CLK_DLY_POS)
#define DSI_HSTX_CLK_DLY_UMSK                                   (~(((1U<<DSI_HSTX_CLK_DLY_LEN)-1)<<DSI_HSTX_CLK_DLY_POS))

/* 0x34 : dsi_reserve */
#define DSI_RESERVE_OFFSET                                      (0x34)
#define DSI_RESV1_7_0                                          DSI_RESV1_7_0
#define DSI_RESV1_7_0_POS                                      (0U)
#define DSI_RESV1_7_0_LEN                                      (8U)
#define DSI_RESV1_7_0_MSK                                      (((1U<<DSI_RESV1_7_0_LEN)-1)<<DSI_RESV1_7_0_POS)
#define DSI_RESV1_7_0_UMSK                                     (~(((1U<<DSI_RESV1_7_0_LEN)-1)<<DSI_RESV1_7_0_POS))
#define DSI_RESV0_7_0                                          DSI_RESV0_7_0
#define DSI_RESV0_7_0_POS                                      (16U)
#define DSI_RESV0_7_0_LEN                                      (8U)
#define DSI_RESV0_7_0_MSK                                      (((1U<<DSI_RESV0_7_0_LEN)-1)<<DSI_RESV0_7_0_POS)
#define DSI_RESV0_7_0_UMSK                                     (~(((1U<<DSI_RESV0_7_0_LEN)-1)<<DSI_RESV0_7_0_POS))

/* 0x38 : dphy_config_0 */
#define DSI_DPHY_CONFIG_0_OFFSET                                (0x38)
#define DSI_REG_DPHY_VREG400MV_TRIM                             DSI_REG_DPHY_VREG400MV_TRIM
#define DSI_REG_DPHY_VREG400MV_TRIM_POS                         (0U)
#define DSI_REG_DPHY_VREG400MV_TRIM_LEN                         (3U)
#define DSI_REG_DPHY_VREG400MV_TRIM_MSK                         (((1U<<DSI_REG_DPHY_VREG400MV_TRIM_LEN)-1)<<DSI_REG_DPHY_VREG400MV_TRIM_POS)
#define DSI_REG_DPHY_VREG400MV_TRIM_UMSK                        (~(((1U<<DSI_REG_DPHY_VREG400MV_TRIM_LEN)-1)<<DSI_REG_DPHY_VREG400MV_TRIM_POS))
#define DSI_REG_DPHY_BG_VREF_EN                                 DSI_REG_DPHY_BG_VREF_EN
#define DSI_REG_DPHY_BG_VREF_EN_POS                             (4U)
#define DSI_REG_DPHY_BG_VREF_EN_LEN                             (1U)
#define DSI_REG_DPHY_BG_VREF_EN_MSK                             (((1U<<DSI_REG_DPHY_BG_VREF_EN_LEN)-1)<<DSI_REG_DPHY_BG_VREF_EN_POS)
#define DSI_REG_DPHY_BG_VREF_EN_UMSK                            (~(((1U<<DSI_REG_DPHY_BG_VREF_EN_LEN)-1)<<DSI_REG_DPHY_BG_VREF_EN_POS))
#define DSI_REG_ANA_BG_VREF_EN                                  DSI_REG_ANA_BG_VREF_EN
#define DSI_REG_ANA_BG_VREF_EN_POS                              (8U)
#define DSI_REG_ANA_BG_VREF_EN_LEN                              (1U)
#define DSI_REG_ANA_BG_VREF_EN_MSK                              (((1U<<DSI_REG_ANA_BG_VREF_EN_LEN)-1)<<DSI_REG_ANA_BG_VREF_EN_POS)
#define DSI_REG_ANA_BG_VREF_EN_UMSK                             (~(((1U<<DSI_REG_ANA_BG_VREF_EN_LEN)-1)<<DSI_REG_ANA_BG_VREF_EN_POS))
#define DSI_REG_ANA_VREG400MV_TRIM                              DSI_REG_ANA_VREG400MV_TRIM
#define DSI_REG_ANA_VREG400MV_TRIM_POS                          (12U)
#define DSI_REG_ANA_VREG400MV_TRIM_LEN                          (3U)
#define DSI_REG_ANA_VREG400MV_TRIM_MSK                          (((1U<<DSI_REG_ANA_VREG400MV_TRIM_LEN)-1)<<DSI_REG_ANA_VREG400MV_TRIM_POS)
#define DSI_REG_ANA_VREG400MV_TRIM_UMSK                         (~(((1U<<DSI_REG_ANA_VREG400MV_TRIM_LEN)-1)<<DSI_REG_ANA_VREG400MV_TRIM_POS))
#define DSI_REG_ANA_VREF_EN                                     DSI_REG_ANA_VREF_EN
#define DSI_REG_ANA_VREF_EN_POS                                 (20U)
#define DSI_REG_ANA_VREF_EN_LEN                                 (1U)
#define DSI_REG_ANA_VREF_EN_MSK                                 (((1U<<DSI_REG_ANA_VREF_EN_LEN)-1)<<DSI_REG_ANA_VREF_EN_POS)
#define DSI_REG_ANA_VREF_EN_UMSK                                (~(((1U<<DSI_REG_ANA_VREF_EN_LEN)-1)<<DSI_REG_ANA_VREF_EN_POS))
#define DSI_REG_ANA_TEST_EN                                     DSI_REG_ANA_TEST_EN
#define DSI_REG_ANA_TEST_EN_POS                                 (24U)
#define DSI_REG_ANA_TEST_EN_LEN                                 (1U)
#define DSI_REG_ANA_TEST_EN_MSK                                 (((1U<<DSI_REG_ANA_TEST_EN_LEN)-1)<<DSI_REG_ANA_TEST_EN_POS)
#define DSI_REG_ANA_TEST_EN_UMSK                                (~(((1U<<DSI_REG_ANA_TEST_EN_LEN)-1)<<DSI_REG_ANA_TEST_EN_POS))

/* 0x3C : dphy_config_1 */
#define DSI_DPHY_CONFIG_1_OFFSET                                (0x3C)
#define DSI_REG_ANA_HSTXEN                                      DSI_REG_ANA_HSTXEN
#define DSI_REG_ANA_HSTXEN_POS                                  (0U)
#define DSI_REG_ANA_HSTXEN_LEN                                  (4U)
#define DSI_REG_ANA_HSTXEN_MSK                                  (((1U<<DSI_REG_ANA_HSTXEN_LEN)-1)<<DSI_REG_ANA_HSTXEN_POS)
#define DSI_REG_ANA_HSTXEN_UMSK                                 (~(((1U<<DSI_REG_ANA_HSTXEN_LEN)-1)<<DSI_REG_ANA_HSTXEN_POS))
#define DSI_REG_ANA_HS_TRAIL_BYTE                               DSI_REG_ANA_HS_TRAIL_BYTE
#define DSI_REG_ANA_HS_TRAIL_BYTE_POS                           (4U)
#define DSI_REG_ANA_HS_TRAIL_BYTE_LEN                           (4U)
#define DSI_REG_ANA_HS_TRAIL_BYTE_MSK                           (((1U<<DSI_REG_ANA_HS_TRAIL_BYTE_LEN)-1)<<DSI_REG_ANA_HS_TRAIL_BYTE_POS)
#define DSI_REG_ANA_HS_TRAIL_BYTE_UMSK                          (~(((1U<<DSI_REG_ANA_HS_TRAIL_BYTE_LEN)-1)<<DSI_REG_ANA_HS_TRAIL_BYTE_POS))
#define DSI_REG_ANA_HS_SYNC_LD_BYTE                             DSI_REG_ANA_HS_SYNC_LD_BYTE
#define DSI_REG_ANA_HS_SYNC_LD_BYTE_POS                         (8U)
#define DSI_REG_ANA_HS_SYNC_LD_BYTE_LEN                         (4U)
#define DSI_REG_ANA_HS_SYNC_LD_BYTE_MSK                         (((1U<<DSI_REG_ANA_HS_SYNC_LD_BYTE_LEN)-1)<<DSI_REG_ANA_HS_SYNC_LD_BYTE_POS)
#define DSI_REG_ANA_HS_SYNC_LD_BYTE_UMSK                        (~(((1U<<DSI_REG_ANA_HS_SYNC_LD_BYTE_LEN)-1)<<DSI_REG_ANA_HS_SYNC_LD_BYTE_POS))
#define DSI_REG_ANA_HS_P2S_SEL_BYTE                             DSI_REG_ANA_HS_P2S_SEL_BYTE
#define DSI_REG_ANA_HS_P2S_SEL_BYTE_POS                         (12U)
#define DSI_REG_ANA_HS_P2S_SEL_BYTE_LEN                         (4U)
#define DSI_REG_ANA_HS_P2S_SEL_BYTE_MSK                         (((1U<<DSI_REG_ANA_HS_P2S_SEL_BYTE_LEN)-1)<<DSI_REG_ANA_HS_P2S_SEL_BYTE_POS)
#define DSI_REG_ANA_HS_P2S_SEL_BYTE_UMSK                        (~(((1U<<DSI_REG_ANA_HS_P2S_SEL_BYTE_LEN)-1)<<DSI_REG_ANA_HS_P2S_SEL_BYTE_POS))
#define DSI_REG_ANA_HS_TXEN_CLKLANE_ESC                         DSI_REG_ANA_HS_TXEN_CLKLANE_ESC
#define DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_POS                     (20U)
#define DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_LEN                     (1U)
#define DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_MSK                     (((1U<<DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_POS)
#define DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_UMSK                    (~(((1U<<DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_HS_TXEN_CLKLANE_ESC_POS))
#define DSI_REG_ANA_CLK_EN_ESC                                  DSI_REG_ANA_CLK_EN_ESC
#define DSI_REG_ANA_CLK_EN_ESC_POS                              (24U)
#define DSI_REG_ANA_CLK_EN_ESC_LEN                              (1U)
#define DSI_REG_ANA_CLK_EN_ESC_MSK                              (((1U<<DSI_REG_ANA_CLK_EN_ESC_LEN)-1)<<DSI_REG_ANA_CLK_EN_ESC_POS)
#define DSI_REG_ANA_CLK_EN_ESC_UMSK                             (~(((1U<<DSI_REG_ANA_CLK_EN_ESC_LEN)-1)<<DSI_REG_ANA_CLK_EN_ESC_POS))
#define DSI_REG_ANA_BYTE_REV                                    DSI_REG_ANA_BYTE_REV
#define DSI_REG_ANA_BYTE_REV_POS                                (28U)
#define DSI_REG_ANA_BYTE_REV_LEN                                (1U)
#define DSI_REG_ANA_BYTE_REV_MSK                                (((1U<<DSI_REG_ANA_BYTE_REV_LEN)-1)<<DSI_REG_ANA_BYTE_REV_POS)
#define DSI_REG_ANA_BYTE_REV_UMSK                               (~(((1U<<DSI_REG_ANA_BYTE_REV_LEN)-1)<<DSI_REG_ANA_BYTE_REV_POS))

/* 0x40 : dphy_config_2 */
#define DSI_DPHY_CONFIG_2_OFFSET                                (0x40)
#define DSI_REG_ANA_HS_DATA_OUT_BYTE                            DSI_REG_ANA_HS_DATA_OUT_BYTE
#define DSI_REG_ANA_HS_DATA_OUT_BYTE_POS                        (0U)
#define DSI_REG_ANA_HS_DATA_OUT_BYTE_LEN                        (32U)
#define DSI_REG_ANA_HS_DATA_OUT_BYTE_MSK                        (((1U<<DSI_REG_ANA_HS_DATA_OUT_BYTE_LEN)-1)<<DSI_REG_ANA_HS_DATA_OUT_BYTE_POS)
#define DSI_REG_ANA_HS_DATA_OUT_BYTE_UMSK                       (~(((1U<<DSI_REG_ANA_HS_DATA_OUT_BYTE_LEN)-1)<<DSI_REG_ANA_HS_DATA_OUT_BYTE_POS))

/* 0x44 : dphy_config_3 */
#define DSI_DPHY_CONFIG_3_OFFSET                                (0x44)
#define DSI_REG_EN_LPRX_AT_ULPS                                 DSI_REG_EN_LPRX_AT_ULPS
#define DSI_REG_EN_LPRX_AT_ULPS_POS                             (0U)
#define DSI_REG_EN_LPRX_AT_ULPS_LEN                             (1U)
#define DSI_REG_EN_LPRX_AT_ULPS_MSK                             (((1U<<DSI_REG_EN_LPRX_AT_ULPS_LEN)-1)<<DSI_REG_EN_LPRX_AT_ULPS_POS)
#define DSI_REG_EN_LPRX_AT_ULPS_UMSK                            (~(((1U<<DSI_REG_EN_LPRX_AT_ULPS_LEN)-1)<<DSI_REG_EN_LPRX_AT_ULPS_POS))
#define DSI_REG_ANA_LPTXP_DATA                                  DSI_REG_ANA_LPTXP_DATA
#define DSI_REG_ANA_LPTXP_DATA_POS                              (4U)
#define DSI_REG_ANA_LPTXP_DATA_LEN                              (4U)
#define DSI_REG_ANA_LPTXP_DATA_MSK                              (((1U<<DSI_REG_ANA_LPTXP_DATA_LEN)-1)<<DSI_REG_ANA_LPTXP_DATA_POS)
#define DSI_REG_ANA_LPTXP_DATA_UMSK                             (~(((1U<<DSI_REG_ANA_LPTXP_DATA_LEN)-1)<<DSI_REG_ANA_LPTXP_DATA_POS))
#define DSI_REG_ANA_LPTXN_DATA                                  DSI_REG_ANA_LPTXN_DATA
#define DSI_REG_ANA_LPTXN_DATA_POS                              (8U)
#define DSI_REG_ANA_LPTXN_DATA_LEN                              (4U)
#define DSI_REG_ANA_LPTXN_DATA_MSK                              (((1U<<DSI_REG_ANA_LPTXN_DATA_LEN)-1)<<DSI_REG_ANA_LPTXN_DATA_POS)
#define DSI_REG_ANA_LPTXN_DATA_UMSK                             (~(((1U<<DSI_REG_ANA_LPTXN_DATA_LEN)-1)<<DSI_REG_ANA_LPTXN_DATA_POS))
#define DSI_REG_ANA_LPTXEN                                      DSI_REG_ANA_LPTXEN
#define DSI_REG_ANA_LPTXEN_POS                                  (12U)
#define DSI_REG_ANA_LPTXEN_LEN                                  (4U)
#define DSI_REG_ANA_LPTXEN_MSK                                  (((1U<<DSI_REG_ANA_LPTXEN_LEN)-1)<<DSI_REG_ANA_LPTXEN_POS)
#define DSI_REG_ANA_LPTXEN_UMSK                                 (~(((1U<<DSI_REG_ANA_LPTXEN_LEN)-1)<<DSI_REG_ANA_LPTXEN_POS))
#define DSI_REG_ANA_LPRXEN                                      DSI_REG_ANA_LPRXEN
#define DSI_REG_ANA_LPRXEN_POS                                  (16U)
#define DSI_REG_ANA_LPRXEN_LEN                                  (4U)
#define DSI_REG_ANA_LPRXEN_MSK                                  (((1U<<DSI_REG_ANA_LPRXEN_LEN)-1)<<DSI_REG_ANA_LPRXEN_POS)
#define DSI_REG_ANA_LPRXEN_UMSK                                 (~(((1U<<DSI_REG_ANA_LPRXEN_LEN)-1)<<DSI_REG_ANA_LPRXEN_POS))
#define DSI_REG_ANA_LP_TXEN_CLKLANE_ESC                         DSI_REG_ANA_LP_TXEN_CLKLANE_ESC
#define DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_POS                     (24U)
#define DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_LEN                     (1U)
#define DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_MSK                     (((1U<<DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_POS)
#define DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_UMSK                    (~(((1U<<DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_LP_TXEN_CLKLANE_ESC_POS))
#define DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC                     DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC
#define DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_POS                 (28U)
#define DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_LEN                 (1U)
#define DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_MSK                 (((1U<<DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_POS)
#define DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_UMSK                (~(((1U<<DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_LP_DPTXDATA_CLKLANE_ESC_POS))
#define DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC                     DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC
#define DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_POS                 (29U)
#define DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_LEN                 (1U)
#define DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_MSK                 (((1U<<DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_POS)
#define DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_UMSK                (~(((1U<<DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_LEN)-1)<<DSI_REG_ANA_LP_DNTXDATA_CLKLANE_ESC_POS))

/* 0x48 : dphy_config_4 */
#define DSI_DPHY_CONFIG_4_OFFSET                                (0x48)
#define DSI_REG_IMP_PU_CODE                                     DSI_REG_IMP_PU_CODE
#define DSI_REG_IMP_PU_CODE_POS                                 (0U)
#define DSI_REG_IMP_PU_CODE_LEN                                 (5U)
#define DSI_REG_IMP_PU_CODE_MSK                                 (((1U<<DSI_REG_IMP_PU_CODE_LEN)-1)<<DSI_REG_IMP_PU_CODE_POS)
#define DSI_REG_IMP_PU_CODE_UMSK                                (~(((1U<<DSI_REG_IMP_PU_CODE_LEN)-1)<<DSI_REG_IMP_PU_CODE_POS))
#define DSI_REG_IMP_PD_CODE                                     DSI_REG_IMP_PD_CODE
#define DSI_REG_IMP_PD_CODE_POS                                 (8U)
#define DSI_REG_IMP_PD_CODE_LEN                                 (5U)
#define DSI_REG_IMP_PD_CODE_MSK                                 (((1U<<DSI_REG_IMP_PD_CODE_LEN)-1)<<DSI_REG_IMP_PD_CODE_POS)
#define DSI_REG_IMP_PD_CODE_UMSK                                (~(((1U<<DSI_REG_IMP_PD_CODE_LEN)-1)<<DSI_REG_IMP_PD_CODE_POS))
#define DSI_REG_FORCE_HIZ_LP                                    DSI_REG_FORCE_HIZ_LP
#define DSI_REG_FORCE_HIZ_LP_POS                                (16U)
#define DSI_REG_FORCE_HIZ_LP_LEN                                (4U)
#define DSI_REG_FORCE_HIZ_LP_MSK                                (((1U<<DSI_REG_FORCE_HIZ_LP_LEN)-1)<<DSI_REG_FORCE_HIZ_LP_POS)
#define DSI_REG_FORCE_HIZ_LP_UMSK                               (~(((1U<<DSI_REG_FORCE_HIZ_LP_LEN)-1)<<DSI_REG_FORCE_HIZ_LP_POS))
#define DSI_REG_FORCE_HIZ_HS                                    DSI_REG_FORCE_HIZ_HS
#define DSI_REG_FORCE_HIZ_HS_POS                                (20U)
#define DSI_REG_FORCE_HIZ_HS_LEN                                (4U)
#define DSI_REG_FORCE_HIZ_HS_MSK                                (((1U<<DSI_REG_FORCE_HIZ_HS_LEN)-1)<<DSI_REG_FORCE_HIZ_HS_POS)
#define DSI_REG_FORCE_HIZ_HS_UMSK                               (~(((1U<<DSI_REG_FORCE_HIZ_HS_LEN)-1)<<DSI_REG_FORCE_HIZ_HS_POS))
#define DSI_REG_FORCE_CLK_HIZ_LP                                DSI_REG_FORCE_CLK_HIZ_LP
#define DSI_REG_FORCE_CLK_HIZ_LP_POS                            (24U)
#define DSI_REG_FORCE_CLK_HIZ_LP_LEN                            (1U)
#define DSI_REG_FORCE_CLK_HIZ_LP_MSK                            (((1U<<DSI_REG_FORCE_CLK_HIZ_LP_LEN)-1)<<DSI_REG_FORCE_CLK_HIZ_LP_POS)
#define DSI_REG_FORCE_CLK_HIZ_LP_UMSK                           (~(((1U<<DSI_REG_FORCE_CLK_HIZ_LP_LEN)-1)<<DSI_REG_FORCE_CLK_HIZ_LP_POS))
#define DSI_REG_FORCE_CLK_HIZ_HS                                DSI_REG_FORCE_CLK_HIZ_HS
#define DSI_REG_FORCE_CLK_HIZ_HS_POS                            (28U)
#define DSI_REG_FORCE_CLK_HIZ_HS_LEN                            (1U)
#define DSI_REG_FORCE_CLK_HIZ_HS_MSK                            (((1U<<DSI_REG_FORCE_CLK_HIZ_HS_LEN)-1)<<DSI_REG_FORCE_CLK_HIZ_HS_POS)
#define DSI_REG_FORCE_CLK_HIZ_HS_UMSK                           (~(((1U<<DSI_REG_FORCE_CLK_HIZ_HS_LEN)-1)<<DSI_REG_FORCE_CLK_HIZ_HS_POS))

/* 0x4C : dphy_config_5 */
#define DSI_DPHY_CONFIG_5_OFFSET                                (0x4C)
#define DSI_REG_TESTBUS_SEL_LO                                  DSI_REG_TESTBUS_SEL_LO
#define DSI_REG_TESTBUS_SEL_LO_POS                              (0U)
#define DSI_REG_TESTBUS_SEL_LO_LEN                              (4U)
#define DSI_REG_TESTBUS_SEL_LO_MSK                              (((1U<<DSI_REG_TESTBUS_SEL_LO_LEN)-1)<<DSI_REG_TESTBUS_SEL_LO_POS)
#define DSI_REG_TESTBUS_SEL_LO_UMSK                             (~(((1U<<DSI_REG_TESTBUS_SEL_LO_LEN)-1)<<DSI_REG_TESTBUS_SEL_LO_POS))
#define DSI_REG_TESTBUS_SEL_HI                                  DSI_REG_TESTBUS_SEL_HI
#define DSI_REG_TESTBUS_SEL_HI_POS                              (4U)
#define DSI_REG_TESTBUS_SEL_HI_LEN                              (4U)
#define DSI_REG_TESTBUS_SEL_HI_MSK                              (((1U<<DSI_REG_TESTBUS_SEL_HI_LEN)-1)<<DSI_REG_TESTBUS_SEL_HI_POS)
#define DSI_REG_TESTBUS_SEL_HI_UMSK                             (~(((1U<<DSI_REG_TESTBUS_SEL_HI_LEN)-1)<<DSI_REG_TESTBUS_SEL_HI_POS))
#define DSI_REG_TESTBUS_HI8BSEL_8BMODE                          DSI_REG_TESTBUS_HI8BSEL_8BMODE
#define DSI_REG_TESTBUS_HI8BSEL_8BMODE_POS                      (12U)
#define DSI_REG_TESTBUS_HI8BSEL_8BMODE_LEN                      (1U)
#define DSI_REG_TESTBUS_HI8BSEL_8BMODE_MSK                      (((1U<<DSI_REG_TESTBUS_HI8BSEL_8BMODE_LEN)-1)<<DSI_REG_TESTBUS_HI8BSEL_8BMODE_POS)
#define DSI_REG_TESTBUS_HI8BSEL_8BMODE_UMSK                     (~(((1U<<DSI_REG_TESTBUS_HI8BSEL_8BMODE_LEN)-1)<<DSI_REG_TESTBUS_HI8BSEL_8BMODE_POS))
#define DSI_REG_PT_PRBS_OR_JITT                                 DSI_REG_PT_PRBS_OR_JITT
#define DSI_REG_PT_PRBS_OR_JITT_POS                             (16U)
#define DSI_REG_PT_PRBS_OR_JITT_LEN                             (1U)
#define DSI_REG_PT_PRBS_OR_JITT_MSK                             (((1U<<DSI_REG_PT_PRBS_OR_JITT_LEN)-1)<<DSI_REG_PT_PRBS_OR_JITT_POS)
#define DSI_REG_PT_PRBS_OR_JITT_UMSK                            (~(((1U<<DSI_REG_PT_PRBS_OR_JITT_LEN)-1)<<DSI_REG_PT_PRBS_OR_JITT_POS))
#define DSI_REG_PT_LP_MODE                                      DSI_REG_PT_LP_MODE
#define DSI_REG_PT_LP_MODE_POS                                  (20U)
#define DSI_REG_PT_LP_MODE_LEN                                  (1U)
#define DSI_REG_PT_LP_MODE_MSK                                  (((1U<<DSI_REG_PT_LP_MODE_LEN)-1)<<DSI_REG_PT_LP_MODE_POS)
#define DSI_REG_PT_LP_MODE_UMSK                                 (~(((1U<<DSI_REG_PT_LP_MODE_LEN)-1)<<DSI_REG_PT_LP_MODE_POS))
#define DSI_REG_PT_EN                                           DSI_REG_PT_EN
#define DSI_REG_PT_EN_POS                                       (24U)
#define DSI_REG_PT_EN_LEN                                       (1U)
#define DSI_REG_PT_EN_MSK                                       (((1U<<DSI_REG_PT_EN_LEN)-1)<<DSI_REG_PT_EN_POS)
#define DSI_REG_PT_EN_UMSK                                      (~(((1U<<DSI_REG_PT_EN_LEN)-1)<<DSI_REG_PT_EN_POS))

/* 0x50 : dphy_config_6 */
#define DSI_DPHY_CONFIG_6_OFFSET                                (0x50)
#define DSI_REG_PT_FREE_REP_PAT                                 DSI_REG_PT_FREE_REP_PAT
#define DSI_REG_PT_FREE_REP_PAT_POS                             (0U)
#define DSI_REG_PT_FREE_REP_PAT_LEN                             (32U)
#define DSI_REG_PT_FREE_REP_PAT_MSK                             (((1U<<DSI_REG_PT_FREE_REP_PAT_LEN)-1)<<DSI_REG_PT_FREE_REP_PAT_POS)
#define DSI_REG_PT_FREE_REP_PAT_UMSK                            (~(((1U<<DSI_REG_PT_FREE_REP_PAT_LEN)-1)<<DSI_REG_PT_FREE_REP_PAT_POS))

/* 0x54 : dphy_config_7 */
#define DSI_DPHY_CONFIG_7_OFFSET                                (0x54)
#define DSI_REG_TIME_CK_ZERO                                    DSI_REG_TIME_CK_ZERO
#define DSI_REG_TIME_CK_ZERO_POS                                (0U)
#define DSI_REG_TIME_CK_ZERO_LEN                                (8U)
#define DSI_REG_TIME_CK_ZERO_MSK                                (((1U<<DSI_REG_TIME_CK_ZERO_LEN)-1)<<DSI_REG_TIME_CK_ZERO_POS)
#define DSI_REG_TIME_CK_ZERO_UMSK                               (~(((1U<<DSI_REG_TIME_CK_ZERO_LEN)-1)<<DSI_REG_TIME_CK_ZERO_POS))
#define DSI_REG_TIME_CK_TRAIL                                   DSI_REG_TIME_CK_TRAIL
#define DSI_REG_TIME_CK_TRAIL_POS                               (8U)
#define DSI_REG_TIME_CK_TRAIL_LEN                               (8U)
#define DSI_REG_TIME_CK_TRAIL_MSK                               (((1U<<DSI_REG_TIME_CK_TRAIL_LEN)-1)<<DSI_REG_TIME_CK_TRAIL_POS)
#define DSI_REG_TIME_CK_TRAIL_UMSK                              (~(((1U<<DSI_REG_TIME_CK_TRAIL_LEN)-1)<<DSI_REG_TIME_CK_TRAIL_POS))
#define DSI_REG_TIME_CK_EXIT                                    DSI_REG_TIME_CK_EXIT
#define DSI_REG_TIME_CK_EXIT_POS                                (16U)
#define DSI_REG_TIME_CK_EXIT_LEN                                (8U)
#define DSI_REG_TIME_CK_EXIT_MSK                                (((1U<<DSI_REG_TIME_CK_EXIT_LEN)-1)<<DSI_REG_TIME_CK_EXIT_POS)
#define DSI_REG_TIME_CK_EXIT_UMSK                               (~(((1U<<DSI_REG_TIME_CK_EXIT_LEN)-1)<<DSI_REG_TIME_CK_EXIT_POS))

/* 0x58 : dphy_config_9 */
#define DSI_DPHY_CONFIG_9_OFFSET                                (0x58)
#define DSI_REG_TIME_HS_ZERO                                    DSI_REG_TIME_HS_ZERO
#define DSI_REG_TIME_HS_ZERO_POS                                (0U)
#define DSI_REG_TIME_HS_ZERO_LEN                                (8U)
#define DSI_REG_TIME_HS_ZERO_MSK                                (((1U<<DSI_REG_TIME_HS_ZERO_LEN)-1)<<DSI_REG_TIME_HS_ZERO_POS)
#define DSI_REG_TIME_HS_ZERO_UMSK                               (~(((1U<<DSI_REG_TIME_HS_ZERO_LEN)-1)<<DSI_REG_TIME_HS_ZERO_POS))
#define DSI_REG_TIME_HS_TRAIL                                   DSI_REG_TIME_HS_TRAIL
#define DSI_REG_TIME_HS_TRAIL_POS                               (8U)
#define DSI_REG_TIME_HS_TRAIL_LEN                               (8U)
#define DSI_REG_TIME_HS_TRAIL_MSK                               (((1U<<DSI_REG_TIME_HS_TRAIL_LEN)-1)<<DSI_REG_TIME_HS_TRAIL_POS)
#define DSI_REG_TIME_HS_TRAIL_UMSK                              (~(((1U<<DSI_REG_TIME_HS_TRAIL_LEN)-1)<<DSI_REG_TIME_HS_TRAIL_POS))
#define DSI_REG_TIME_HS_PREP                                    DSI_REG_TIME_HS_PREP
#define DSI_REG_TIME_HS_PREP_POS                                (16U)
#define DSI_REG_TIME_HS_PREP_LEN                                (8U)
#define DSI_REG_TIME_HS_PREP_MSK                                (((1U<<DSI_REG_TIME_HS_PREP_LEN)-1)<<DSI_REG_TIME_HS_PREP_POS)
#define DSI_REG_TIME_HS_PREP_UMSK                               (~(((1U<<DSI_REG_TIME_HS_PREP_LEN)-1)<<DSI_REG_TIME_HS_PREP_POS))
#define DSI_REG_TIME_HS_EXIT                                    DSI_REG_TIME_HS_EXIT
#define DSI_REG_TIME_HS_EXIT_POS                                (24U)
#define DSI_REG_TIME_HS_EXIT_LEN                                (8U)
#define DSI_REG_TIME_HS_EXIT_MSK                                (((1U<<DSI_REG_TIME_HS_EXIT_LEN)-1)<<DSI_REG_TIME_HS_EXIT_POS)
#define DSI_REG_TIME_HS_EXIT_UMSK                               (~(((1U<<DSI_REG_TIME_HS_EXIT_LEN)-1)<<DSI_REG_TIME_HS_EXIT_POS))

/* 0x5C : dphy_config_10 */
#define DSI_DPHY_CONFIG_10_OFFSET                               (0x5C)
#define DSI_REG_TIME_TA_GO                                      DSI_REG_TIME_TA_GO
#define DSI_REG_TIME_TA_GO_POS                                  (0U)
#define DSI_REG_TIME_TA_GO_LEN                                  (8U)
#define DSI_REG_TIME_TA_GO_MSK                                  (((1U<<DSI_REG_TIME_TA_GO_LEN)-1)<<DSI_REG_TIME_TA_GO_POS)
#define DSI_REG_TIME_TA_GO_UMSK                                 (~(((1U<<DSI_REG_TIME_TA_GO_LEN)-1)<<DSI_REG_TIME_TA_GO_POS))
#define DSI_REG_TIME_TA_GET                                     DSI_REG_TIME_TA_GET
#define DSI_REG_TIME_TA_GET_POS                                 (8U)
#define DSI_REG_TIME_TA_GET_LEN                                 (8U)
#define DSI_REG_TIME_TA_GET_MSK                                 (((1U<<DSI_REG_TIME_TA_GET_LEN)-1)<<DSI_REG_TIME_TA_GET_POS)
#define DSI_REG_TIME_TA_GET_UMSK                                (~(((1U<<DSI_REG_TIME_TA_GET_LEN)-1)<<DSI_REG_TIME_TA_GET_POS))
#define DSI_REG_TIME_LPX                                        DSI_REG_TIME_LPX
#define DSI_REG_TIME_LPX_POS                                    (16U)
#define DSI_REG_TIME_LPX_LEN                                    (8U)
#define DSI_REG_TIME_LPX_MSK                                    (((1U<<DSI_REG_TIME_LPX_LEN)-1)<<DSI_REG_TIME_LPX_POS)
#define DSI_REG_TIME_LPX_UMSK                                   (~(((1U<<DSI_REG_TIME_LPX_LEN)-1)<<DSI_REG_TIME_LPX_POS))

/* 0x60 : dphy_config_11 */
#define DSI_DPHY_CONFIG_11_OFFSET                               (0x60)
#define DSI_REG_TIME_WAKEUP                                     DSI_REG_TIME_WAKEUP
#define DSI_REG_TIME_WAKEUP_POS                                 (0U)
#define DSI_REG_TIME_WAKEUP_LEN                                 (16U)
#define DSI_REG_TIME_WAKEUP_MSK                                 (((1U<<DSI_REG_TIME_WAKEUP_LEN)-1)<<DSI_REG_TIME_WAKEUP_POS)
#define DSI_REG_TIME_WAKEUP_UMSK                                (~(((1U<<DSI_REG_TIME_WAKEUP_LEN)-1)<<DSI_REG_TIME_WAKEUP_POS))
#define DSI_REG_TIME_REQRDY                                     DSI_REG_TIME_REQRDY
#define DSI_REG_TIME_REQRDY_POS                                 (16U)
#define DSI_REG_TIME_REQRDY_LEN                                 (8U)
#define DSI_REG_TIME_REQRDY_MSK                                 (((1U<<DSI_REG_TIME_REQRDY_LEN)-1)<<DSI_REG_TIME_REQRDY_POS)
#define DSI_REG_TIME_REQRDY_UMSK                                (~(((1U<<DSI_REG_TIME_REQRDY_LEN)-1)<<DSI_REG_TIME_REQRDY_POS))

/* 0x64 : dphy_config_12 */
#define DSI_DPHY_CONFIG_12_OFFSET                               (0x64)
#define DSI_REG_TRIG3_CODE                                      DSI_REG_TRIG3_CODE
#define DSI_REG_TRIG3_CODE_POS                                  (0U)
#define DSI_REG_TRIG3_CODE_LEN                                  (8U)
#define DSI_REG_TRIG3_CODE_MSK                                  (((1U<<DSI_REG_TRIG3_CODE_LEN)-1)<<DSI_REG_TRIG3_CODE_POS)
#define DSI_REG_TRIG3_CODE_UMSK                                 (~(((1U<<DSI_REG_TRIG3_CODE_LEN)-1)<<DSI_REG_TRIG3_CODE_POS))
#define DSI_REG_TRIG2_CODE                                      DSI_REG_TRIG2_CODE
#define DSI_REG_TRIG2_CODE_POS                                  (8U)
#define DSI_REG_TRIG2_CODE_LEN                                  (8U)
#define DSI_REG_TRIG2_CODE_MSK                                  (((1U<<DSI_REG_TRIG2_CODE_LEN)-1)<<DSI_REG_TRIG2_CODE_POS)
#define DSI_REG_TRIG2_CODE_UMSK                                 (~(((1U<<DSI_REG_TRIG2_CODE_LEN)-1)<<DSI_REG_TRIG2_CODE_POS))
#define DSI_REG_TRIG1_CODE                                      DSI_REG_TRIG1_CODE
#define DSI_REG_TRIG1_CODE_POS                                  (16U)
#define DSI_REG_TRIG1_CODE_LEN                                  (8U)
#define DSI_REG_TRIG1_CODE_MSK                                  (((1U<<DSI_REG_TRIG1_CODE_LEN)-1)<<DSI_REG_TRIG1_CODE_POS)
#define DSI_REG_TRIG1_CODE_UMSK                                 (~(((1U<<DSI_REG_TRIG1_CODE_LEN)-1)<<DSI_REG_TRIG1_CODE_POS))
#define DSI_REG_TRIG0_CODE                                      DSI_REG_TRIG0_CODE
#define DSI_REG_TRIG0_CODE_POS                                  (24U)
#define DSI_REG_TRIG0_CODE_LEN                                  (8U)
#define DSI_REG_TRIG0_CODE_MSK                                  (((1U<<DSI_REG_TRIG0_CODE_LEN)-1)<<DSI_REG_TRIG0_CODE_POS)
#define DSI_REG_TRIG0_CODE_UMSK                                 (~(((1U<<DSI_REG_TRIG0_CODE_LEN)-1)<<DSI_REG_TRIG0_CODE_POS))

/* 0x68 : dphy_config_13 */
#define DSI_DPHY_CONFIG_13_OFFSET                               (0x68)
#define DSI_REG_LPDT_CODE                                       DSI_REG_LPDT_CODE
#define DSI_REG_LPDT_CODE_POS                                   (0U)
#define DSI_REG_LPDT_CODE_LEN                                   (8U)
#define DSI_REG_LPDT_CODE_MSK                                   (((1U<<DSI_REG_LPDT_CODE_LEN)-1)<<DSI_REG_LPDT_CODE_POS)
#define DSI_REG_LPDT_CODE_UMSK                                  (~(((1U<<DSI_REG_LPDT_CODE_LEN)-1)<<DSI_REG_LPDT_CODE_POS))
#define DSI_REG_ULPS_CODE                                       DSI_REG_ULPS_CODE
#define DSI_REG_ULPS_CODE_POS                                   (16U)
#define DSI_REG_ULPS_CODE_LEN                                   (8U)
#define DSI_REG_ULPS_CODE_MSK                                   (((1U<<DSI_REG_ULPS_CODE_LEN)-1)<<DSI_REG_ULPS_CODE_POS)
#define DSI_REG_ULPS_CODE_UMSK                                  (~(((1U<<DSI_REG_ULPS_CODE_LEN)-1)<<DSI_REG_ULPS_CODE_POS))

/* 0x6C : dphy_config_14 */
#define DSI_DPHY_CONFIG_14_OFFSET                               (0x6C)
#define DSI_CL_ENABLE                                           DSI_CL_ENABLE
#define DSI_CL_ENABLE_POS                                       (0U)
#define DSI_CL_ENABLE_LEN                                       (1U)
#define DSI_CL_ENABLE_MSK                                       (((1U<<DSI_CL_ENABLE_LEN)-1)<<DSI_CL_ENABLE_POS)
#define DSI_CL_ENABLE_UMSK                                      (~(((1U<<DSI_CL_ENABLE_LEN)-1)<<DSI_CL_ENABLE_POS))
#define DSI_CL_TXREQUESTHS                                      DSI_CL_TXREQUESTHS
#define DSI_CL_TXREQUESTHS_POS                                  (1U)
#define DSI_CL_TXREQUESTHS_LEN                                  (1U)
#define DSI_CL_TXREQUESTHS_MSK                                  (((1U<<DSI_CL_TXREQUESTHS_LEN)-1)<<DSI_CL_TXREQUESTHS_POS)
#define DSI_CL_TXREQUESTHS_UMSK                                 (~(((1U<<DSI_CL_TXREQUESTHS_LEN)-1)<<DSI_CL_TXREQUESTHS_POS))
#define DSI_CL_TXULPSCLK                                        DSI_CL_TXULPSCLK
#define DSI_CL_TXULPSCLK_POS                                    (2U)
#define DSI_CL_TXULPSCLK_LEN                                    (1U)
#define DSI_CL_TXULPSCLK_MSK                                    (((1U<<DSI_CL_TXULPSCLK_LEN)-1)<<DSI_CL_TXULPSCLK_POS)
#define DSI_CL_TXULPSCLK_UMSK                                   (~(((1U<<DSI_CL_TXULPSCLK_LEN)-1)<<DSI_CL_TXULPSCLK_POS))
#define DSI_CL_TXULPSEXIT                                       DSI_CL_TXULPSEXIT
#define DSI_CL_TXULPSEXIT_POS                                   (3U)
#define DSI_CL_TXULPSEXIT_LEN                                   (1U)
#define DSI_CL_TXULPSEXIT_MSK                                   (((1U<<DSI_CL_TXULPSEXIT_LEN)-1)<<DSI_CL_TXULPSEXIT_POS)
#define DSI_CL_TXULPSEXIT_UMSK                                  (~(((1U<<DSI_CL_TXULPSEXIT_LEN)-1)<<DSI_CL_TXULPSEXIT_POS))
#define DSI_DL0_ENABLE                                          DSI_DL0_ENABLE
#define DSI_DL0_ENABLE_POS                                      (8U)
#define DSI_DL0_ENABLE_LEN                                      (1U)
#define DSI_DL0_ENABLE_MSK                                      (((1U<<DSI_DL0_ENABLE_LEN)-1)<<DSI_DL0_ENABLE_POS)
#define DSI_DL0_ENABLE_UMSK                                     (~(((1U<<DSI_DL0_ENABLE_LEN)-1)<<DSI_DL0_ENABLE_POS))
#define DSI_DL1_ENABLE                                          DSI_DL1_ENABLE
#define DSI_DL1_ENABLE_POS                                      (9U)
#define DSI_DL1_ENABLE_LEN                                      (1U)
#define DSI_DL1_ENABLE_MSK                                      (((1U<<DSI_DL1_ENABLE_LEN)-1)<<DSI_DL1_ENABLE_POS)
#define DSI_DL1_ENABLE_UMSK                                     (~(((1U<<DSI_DL1_ENABLE_LEN)-1)<<DSI_DL1_ENABLE_POS))
#define DSI_DL2_ENABLE                                          DSI_DL2_ENABLE
#define DSI_DL2_ENABLE_POS                                      (10U)
#define DSI_DL2_ENABLE_LEN                                      (1U)
#define DSI_DL2_ENABLE_MSK                                      (((1U<<DSI_DL2_ENABLE_LEN)-1)<<DSI_DL2_ENABLE_POS)
#define DSI_DL2_ENABLE_UMSK                                     (~(((1U<<DSI_DL2_ENABLE_LEN)-1)<<DSI_DL2_ENABLE_POS))
#define DSI_DL3_ENABLE                                          DSI_DL3_ENABLE
#define DSI_DL3_ENABLE_POS                                      (11U)
#define DSI_DL3_ENABLE_LEN                                      (1U)
#define DSI_DL3_ENABLE_MSK                                      (((1U<<DSI_DL3_ENABLE_LEN)-1)<<DSI_DL3_ENABLE_POS)
#define DSI_DL3_ENABLE_UMSK                                     (~(((1U<<DSI_DL3_ENABLE_LEN)-1)<<DSI_DL3_ENABLE_POS))
#define DSI_DL0_FORCERXMODE                                     DSI_DL0_FORCERXMODE
#define DSI_DL0_FORCERXMODE_POS                                 (16U)
#define DSI_DL0_FORCERXMODE_LEN                                 (1U)
#define DSI_DL0_FORCERXMODE_MSK                                 (((1U<<DSI_DL0_FORCERXMODE_LEN)-1)<<DSI_DL0_FORCERXMODE_POS)
#define DSI_DL0_FORCERXMODE_UMSK                                (~(((1U<<DSI_DL0_FORCERXMODE_LEN)-1)<<DSI_DL0_FORCERXMODE_POS))
#define DSI_DL1_FORCERXMODE                                     DSI_DL1_FORCERXMODE
#define DSI_DL1_FORCERXMODE_POS                                 (17U)
#define DSI_DL1_FORCERXMODE_LEN                                 (1U)
#define DSI_DL1_FORCERXMODE_MSK                                 (((1U<<DSI_DL1_FORCERXMODE_LEN)-1)<<DSI_DL1_FORCERXMODE_POS)
#define DSI_DL1_FORCERXMODE_UMSK                                (~(((1U<<DSI_DL1_FORCERXMODE_LEN)-1)<<DSI_DL1_FORCERXMODE_POS))
#define DSI_DL2_FORCERXMODE                                     DSI_DL2_FORCERXMODE
#define DSI_DL2_FORCERXMODE_POS                                 (18U)
#define DSI_DL2_FORCERXMODE_LEN                                 (1U)
#define DSI_DL2_FORCERXMODE_MSK                                 (((1U<<DSI_DL2_FORCERXMODE_LEN)-1)<<DSI_DL2_FORCERXMODE_POS)
#define DSI_DL2_FORCERXMODE_UMSK                                (~(((1U<<DSI_DL2_FORCERXMODE_LEN)-1)<<DSI_DL2_FORCERXMODE_POS))
#define DSI_DL3_FORCERXMODE                                     DSI_DL3_FORCERXMODE
#define DSI_DL3_FORCERXMODE_POS                                 (19U)
#define DSI_DL3_FORCERXMODE_LEN                                 (1U)
#define DSI_DL3_FORCERXMODE_MSK                                 (((1U<<DSI_DL3_FORCERXMODE_LEN)-1)<<DSI_DL3_FORCERXMODE_POS)
#define DSI_DL3_FORCERXMODE_UMSK                                (~(((1U<<DSI_DL3_FORCERXMODE_LEN)-1)<<DSI_DL3_FORCERXMODE_POS))
#define DSI_DL0_FORCETXSTOPMODE                                 DSI_DL0_FORCETXSTOPMODE
#define DSI_DL0_FORCETXSTOPMODE_POS                             (20U)
#define DSI_DL0_FORCETXSTOPMODE_LEN                             (1U)
#define DSI_DL0_FORCETXSTOPMODE_MSK                             (((1U<<DSI_DL0_FORCETXSTOPMODE_LEN)-1)<<DSI_DL0_FORCETXSTOPMODE_POS)
#define DSI_DL0_FORCETXSTOPMODE_UMSK                            (~(((1U<<DSI_DL0_FORCETXSTOPMODE_LEN)-1)<<DSI_DL0_FORCETXSTOPMODE_POS))
#define DSI_DL1_FORCETXSTOPMODE                                 DSI_DL1_FORCETXSTOPMODE
#define DSI_DL1_FORCETXSTOPMODE_POS                             (21U)
#define DSI_DL1_FORCETXSTOPMODE_LEN                             (1U)
#define DSI_DL1_FORCETXSTOPMODE_MSK                             (((1U<<DSI_DL1_FORCETXSTOPMODE_LEN)-1)<<DSI_DL1_FORCETXSTOPMODE_POS)
#define DSI_DL1_FORCETXSTOPMODE_UMSK                            (~(((1U<<DSI_DL1_FORCETXSTOPMODE_LEN)-1)<<DSI_DL1_FORCETXSTOPMODE_POS))
#define DSI_DL2_FORCETXSTOPMODE                                 DSI_DL2_FORCETXSTOPMODE
#define DSI_DL2_FORCETXSTOPMODE_POS                             (22U)
#define DSI_DL2_FORCETXSTOPMODE_LEN                             (1U)
#define DSI_DL2_FORCETXSTOPMODE_MSK                             (((1U<<DSI_DL2_FORCETXSTOPMODE_LEN)-1)<<DSI_DL2_FORCETXSTOPMODE_POS)
#define DSI_DL2_FORCETXSTOPMODE_UMSK                            (~(((1U<<DSI_DL2_FORCETXSTOPMODE_LEN)-1)<<DSI_DL2_FORCETXSTOPMODE_POS))
#define DSI_DL3_FORCETXSTOPMODE                                 DSI_DL3_FORCETXSTOPMODE
#define DSI_DL3_FORCETXSTOPMODE_POS                             (23U)
#define DSI_DL3_FORCETXSTOPMODE_LEN                             (1U)
#define DSI_DL3_FORCETXSTOPMODE_MSK                             (((1U<<DSI_DL3_FORCETXSTOPMODE_LEN)-1)<<DSI_DL3_FORCETXSTOPMODE_POS)
#define DSI_DL3_FORCETXSTOPMODE_UMSK                            (~(((1U<<DSI_DL3_FORCETXSTOPMODE_LEN)-1)<<DSI_DL3_FORCETXSTOPMODE_POS))
#define DSI_DL0_TURNESC                                         DSI_DL0_TURNESC
#define DSI_DL0_TURNESC_POS                                     (24U)
#define DSI_DL0_TURNESC_LEN                                     (1U)
#define DSI_DL0_TURNESC_MSK                                     (((1U<<DSI_DL0_TURNESC_LEN)-1)<<DSI_DL0_TURNESC_POS)
#define DSI_DL0_TURNESC_UMSK                                    (~(((1U<<DSI_DL0_TURNESC_LEN)-1)<<DSI_DL0_TURNESC_POS))
#define DSI_DL1_TURNESC                                         DSI_DL1_TURNESC
#define DSI_DL1_TURNESC_POS                                     (25U)
#define DSI_DL1_TURNESC_LEN                                     (1U)
#define DSI_DL1_TURNESC_MSK                                     (((1U<<DSI_DL1_TURNESC_LEN)-1)<<DSI_DL1_TURNESC_POS)
#define DSI_DL1_TURNESC_UMSK                                    (~(((1U<<DSI_DL1_TURNESC_LEN)-1)<<DSI_DL1_TURNESC_POS))
#define DSI_DL2_TURNESC                                         DSI_DL2_TURNESC
#define DSI_DL2_TURNESC_POS                                     (26U)
#define DSI_DL2_TURNESC_LEN                                     (1U)
#define DSI_DL2_TURNESC_MSK                                     (((1U<<DSI_DL2_TURNESC_LEN)-1)<<DSI_DL2_TURNESC_POS)
#define DSI_DL2_TURNESC_UMSK                                    (~(((1U<<DSI_DL2_TURNESC_LEN)-1)<<DSI_DL2_TURNESC_POS))
#define DSI_DL3_TURNESC                                         DSI_DL3_TURNESC
#define DSI_DL3_TURNESC_POS                                     (27U)
#define DSI_DL3_TURNESC_LEN                                     (1U)
#define DSI_DL3_TURNESC_MSK                                     (((1U<<DSI_DL3_TURNESC_LEN)-1)<<DSI_DL3_TURNESC_POS)
#define DSI_DL3_TURNESC_UMSK                                    (~(((1U<<DSI_DL3_TURNESC_LEN)-1)<<DSI_DL3_TURNESC_POS))
#define DSI_DPHY_RESET_N                                        DSI_DPHY_RESET_N
#define DSI_DPHY_RESET_N_POS                                    (28U)
#define DSI_DPHY_RESET_N_LEN                                    (1U)
#define DSI_DPHY_RESET_N_MSK                                    (((1U<<DSI_DPHY_RESET_N_LEN)-1)<<DSI_DPHY_RESET_N_POS)
#define DSI_DPHY_RESET_N_UMSK                                   (~(((1U<<DSI_DPHY_RESET_N_LEN)-1)<<DSI_DPHY_RESET_N_POS))

/* 0x70 : dphy_rb_0 */
#define DSI_DPHY_RB_0_OFFSET                                    (0x70)
#define DSI_DL0_RXULPSESC                                       DSI_DL0_RXULPSESC
#define DSI_DL0_RXULPSESC_POS                                   (0U)
#define DSI_DL0_RXULPSESC_LEN                                   (1U)
#define DSI_DL0_RXULPSESC_MSK                                   (((1U<<DSI_DL0_RXULPSESC_LEN)-1)<<DSI_DL0_RXULPSESC_POS)
#define DSI_DL0_RXULPSESC_UMSK                                  (~(((1U<<DSI_DL0_RXULPSESC_LEN)-1)<<DSI_DL0_RXULPSESC_POS))
#define DSI_DL1_RXULPSESC                                       DSI_DL1_RXULPSESC
#define DSI_DL1_RXULPSESC_POS                                   (1U)
#define DSI_DL1_RXULPSESC_LEN                                   (1U)
#define DSI_DL1_RXULPSESC_MSK                                   (((1U<<DSI_DL1_RXULPSESC_LEN)-1)<<DSI_DL1_RXULPSESC_POS)
#define DSI_DL1_RXULPSESC_UMSK                                  (~(((1U<<DSI_DL1_RXULPSESC_LEN)-1)<<DSI_DL1_RXULPSESC_POS))
#define DSI_DL2_RXULPSESC                                       DSI_DL2_RXULPSESC
#define DSI_DL2_RXULPSESC_POS                                   (2U)
#define DSI_DL2_RXULPSESC_LEN                                   (1U)
#define DSI_DL2_RXULPSESC_MSK                                   (((1U<<DSI_DL2_RXULPSESC_LEN)-1)<<DSI_DL2_RXULPSESC_POS)
#define DSI_DL2_RXULPSESC_UMSK                                  (~(((1U<<DSI_DL2_RXULPSESC_LEN)-1)<<DSI_DL2_RXULPSESC_POS))
#define DSI_DL3_RXULPSESC                                       DSI_DL3_RXULPSESC
#define DSI_DL3_RXULPSESC_POS                                   (3U)
#define DSI_DL3_RXULPSESC_LEN                                   (1U)
#define DSI_DL3_RXULPSESC_MSK                                   (((1U<<DSI_DL3_RXULPSESC_LEN)-1)<<DSI_DL3_RXULPSESC_POS)
#define DSI_DL3_RXULPSESC_UMSK                                  (~(((1U<<DSI_DL3_RXULPSESC_LEN)-1)<<DSI_DL3_RXULPSESC_POS))
#define DSI_CL_STOPSTATE                                        DSI_CL_STOPSTATE
#define DSI_CL_STOPSTATE_POS                                    (8U)
#define DSI_CL_STOPSTATE_LEN                                    (1U)
#define DSI_CL_STOPSTATE_MSK                                    (((1U<<DSI_CL_STOPSTATE_LEN)-1)<<DSI_CL_STOPSTATE_POS)
#define DSI_CL_STOPSTATE_UMSK                                   (~(((1U<<DSI_CL_STOPSTATE_LEN)-1)<<DSI_CL_STOPSTATE_POS))
#define DSI_DL0_STOPSTATE                                       DSI_DL0_STOPSTATE
#define DSI_DL0_STOPSTATE_POS                                   (17U)
#define DSI_DL0_STOPSTATE_LEN                                   (1U)
#define DSI_DL0_STOPSTATE_MSK                                   (((1U<<DSI_DL0_STOPSTATE_LEN)-1)<<DSI_DL0_STOPSTATE_POS)
#define DSI_DL0_STOPSTATE_UMSK                                  (~(((1U<<DSI_DL0_STOPSTATE_LEN)-1)<<DSI_DL0_STOPSTATE_POS))
#define DSI_DL1_STOPSTATE                                       DSI_DL1_STOPSTATE
#define DSI_DL1_STOPSTATE_POS                                   (18U)
#define DSI_DL1_STOPSTATE_LEN                                   (1U)
#define DSI_DL1_STOPSTATE_MSK                                   (((1U<<DSI_DL1_STOPSTATE_LEN)-1)<<DSI_DL1_STOPSTATE_POS)
#define DSI_DL1_STOPSTATE_UMSK                                  (~(((1U<<DSI_DL1_STOPSTATE_LEN)-1)<<DSI_DL1_STOPSTATE_POS))
#define DSI_DL2_STOPSTATE                                       DSI_DL2_STOPSTATE
#define DSI_DL2_STOPSTATE_POS                                   (19U)
#define DSI_DL2_STOPSTATE_LEN                                   (1U)
#define DSI_DL2_STOPSTATE_MSK                                   (((1U<<DSI_DL2_STOPSTATE_LEN)-1)<<DSI_DL2_STOPSTATE_POS)
#define DSI_DL2_STOPSTATE_UMSK                                  (~(((1U<<DSI_DL2_STOPSTATE_LEN)-1)<<DSI_DL2_STOPSTATE_POS))
#define DSI_DL3_STOPSTATE                                       DSI_DL3_STOPSTATE
#define DSI_DL3_STOPSTATE_POS                                   (20U)
#define DSI_DL3_STOPSTATE_LEN                                   (1U)
#define DSI_DL3_STOPSTATE_MSK                                   (((1U<<DSI_DL3_STOPSTATE_LEN)-1)<<DSI_DL3_STOPSTATE_POS)
#define DSI_DL3_STOPSTATE_UMSK                                  (~(((1U<<DSI_DL3_STOPSTATE_LEN)-1)<<DSI_DL3_STOPSTATE_POS))
#define DSI_CL_ULPSACTIVENOT                                    DSI_CL_ULPSACTIVENOT
#define DSI_CL_ULPSACTIVENOT_POS                                (24U)
#define DSI_CL_ULPSACTIVENOT_LEN                                (1U)
#define DSI_CL_ULPSACTIVENOT_MSK                                (((1U<<DSI_CL_ULPSACTIVENOT_LEN)-1)<<DSI_CL_ULPSACTIVENOT_POS)
#define DSI_CL_ULPSACTIVENOT_UMSK                               (~(((1U<<DSI_CL_ULPSACTIVENOT_LEN)-1)<<DSI_CL_ULPSACTIVENOT_POS))
#define DSI_DL0_ULPSACTIVENOT                                   DSI_DL0_ULPSACTIVENOT
#define DSI_DL0_ULPSACTIVENOT_POS                               (25U)
#define DSI_DL0_ULPSACTIVENOT_LEN                               (1U)
#define DSI_DL0_ULPSACTIVENOT_MSK                               (((1U<<DSI_DL0_ULPSACTIVENOT_LEN)-1)<<DSI_DL0_ULPSACTIVENOT_POS)
#define DSI_DL0_ULPSACTIVENOT_UMSK                              (~(((1U<<DSI_DL0_ULPSACTIVENOT_LEN)-1)<<DSI_DL0_ULPSACTIVENOT_POS))
#define DSI_DL1_ULPSACTIVENOT                                   DSI_DL1_ULPSACTIVENOT
#define DSI_DL1_ULPSACTIVENOT_POS                               (26U)
#define DSI_DL1_ULPSACTIVENOT_LEN                               (1U)
#define DSI_DL1_ULPSACTIVENOT_MSK                               (((1U<<DSI_DL1_ULPSACTIVENOT_LEN)-1)<<DSI_DL1_ULPSACTIVENOT_POS)
#define DSI_DL1_ULPSACTIVENOT_UMSK                              (~(((1U<<DSI_DL1_ULPSACTIVENOT_LEN)-1)<<DSI_DL1_ULPSACTIVENOT_POS))
#define DSI_DL2_ULPSACTIVENOT                                   DSI_DL2_ULPSACTIVENOT
#define DSI_DL2_ULPSACTIVENOT_POS                               (27U)
#define DSI_DL2_ULPSACTIVENOT_LEN                               (1U)
#define DSI_DL2_ULPSACTIVENOT_MSK                               (((1U<<DSI_DL2_ULPSACTIVENOT_LEN)-1)<<DSI_DL2_ULPSACTIVENOT_POS)
#define DSI_DL2_ULPSACTIVENOT_UMSK                              (~(((1U<<DSI_DL2_ULPSACTIVENOT_LEN)-1)<<DSI_DL2_ULPSACTIVENOT_POS))
#define DSI_DL3_ULPSACTIVENOT                                   DSI_DL3_ULPSACTIVENOT
#define DSI_DL3_ULPSACTIVENOT_POS                               (28U)
#define DSI_DL3_ULPSACTIVENOT_LEN                               (1U)
#define DSI_DL3_ULPSACTIVENOT_MSK                               (((1U<<DSI_DL3_ULPSACTIVENOT_LEN)-1)<<DSI_DL3_ULPSACTIVENOT_POS)
#define DSI_DL3_ULPSACTIVENOT_UMSK                              (~(((1U<<DSI_DL3_ULPSACTIVENOT_LEN)-1)<<DSI_DL3_ULPSACTIVENOT_POS))

/* 0x74 : dphy_rb_1 */
#define DSI_DPHY_RB_1_OFFSET                                    (0x74)
#define DSI_DL0_ERRCONTROL                                      DSI_DL0_ERRCONTROL
#define DSI_DL0_ERRCONTROL_POS                                  (0U)
#define DSI_DL0_ERRCONTROL_LEN                                  (1U)
#define DSI_DL0_ERRCONTROL_MSK                                  (((1U<<DSI_DL0_ERRCONTROL_LEN)-1)<<DSI_DL0_ERRCONTROL_POS)
#define DSI_DL0_ERRCONTROL_UMSK                                 (~(((1U<<DSI_DL0_ERRCONTROL_LEN)-1)<<DSI_DL0_ERRCONTROL_POS))
#define DSI_DL1_ERRCONTROL                                      DSI_DL1_ERRCONTROL
#define DSI_DL1_ERRCONTROL_POS                                  (1U)
#define DSI_DL1_ERRCONTROL_LEN                                  (1U)
#define DSI_DL1_ERRCONTROL_MSK                                  (((1U<<DSI_DL1_ERRCONTROL_LEN)-1)<<DSI_DL1_ERRCONTROL_POS)
#define DSI_DL1_ERRCONTROL_UMSK                                 (~(((1U<<DSI_DL1_ERRCONTROL_LEN)-1)<<DSI_DL1_ERRCONTROL_POS))
#define DSI_DL2_ERRCONTROL                                      DSI_DL2_ERRCONTROL
#define DSI_DL2_ERRCONTROL_POS                                  (2U)
#define DSI_DL2_ERRCONTROL_LEN                                  (1U)
#define DSI_DL2_ERRCONTROL_MSK                                  (((1U<<DSI_DL2_ERRCONTROL_LEN)-1)<<DSI_DL2_ERRCONTROL_POS)
#define DSI_DL2_ERRCONTROL_UMSK                                 (~(((1U<<DSI_DL2_ERRCONTROL_LEN)-1)<<DSI_DL2_ERRCONTROL_POS))
#define DSI_DL3_ERRCONTROL                                      DSI_DL3_ERRCONTROL
#define DSI_DL3_ERRCONTROL_POS                                  (3U)
#define DSI_DL3_ERRCONTROL_LEN                                  (1U)
#define DSI_DL3_ERRCONTROL_MSK                                  (((1U<<DSI_DL3_ERRCONTROL_LEN)-1)<<DSI_DL3_ERRCONTROL_POS)
#define DSI_DL3_ERRCONTROL_UMSK                                 (~(((1U<<DSI_DL3_ERRCONTROL_LEN)-1)<<DSI_DL3_ERRCONTROL_POS))
#define DSI_DL0_ERRSYNCESC                                      DSI_DL0_ERRSYNCESC
#define DSI_DL0_ERRSYNCESC_POS                                  (4U)
#define DSI_DL0_ERRSYNCESC_LEN                                  (1U)
#define DSI_DL0_ERRSYNCESC_MSK                                  (((1U<<DSI_DL0_ERRSYNCESC_LEN)-1)<<DSI_DL0_ERRSYNCESC_POS)
#define DSI_DL0_ERRSYNCESC_UMSK                                 (~(((1U<<DSI_DL0_ERRSYNCESC_LEN)-1)<<DSI_DL0_ERRSYNCESC_POS))
#define DSI_DL1_ERRSYNCESC                                      DSI_DL1_ERRSYNCESC
#define DSI_DL1_ERRSYNCESC_POS                                  (5U)
#define DSI_DL1_ERRSYNCESC_LEN                                  (1U)
#define DSI_DL1_ERRSYNCESC_MSK                                  (((1U<<DSI_DL1_ERRSYNCESC_LEN)-1)<<DSI_DL1_ERRSYNCESC_POS)
#define DSI_DL1_ERRSYNCESC_UMSK                                 (~(((1U<<DSI_DL1_ERRSYNCESC_LEN)-1)<<DSI_DL1_ERRSYNCESC_POS))
#define DSI_DL2_ERRSYNCESC                                      DSI_DL2_ERRSYNCESC
#define DSI_DL2_ERRSYNCESC_POS                                  (6U)
#define DSI_DL2_ERRSYNCESC_LEN                                  (1U)
#define DSI_DL2_ERRSYNCESC_MSK                                  (((1U<<DSI_DL2_ERRSYNCESC_LEN)-1)<<DSI_DL2_ERRSYNCESC_POS)
#define DSI_DL2_ERRSYNCESC_UMSK                                 (~(((1U<<DSI_DL2_ERRSYNCESC_LEN)-1)<<DSI_DL2_ERRSYNCESC_POS))
#define DSI_DL3_ERRSYNCESC                                      DSI_DL3_ERRSYNCESC
#define DSI_DL3_ERRSYNCESC_POS                                  (7U)
#define DSI_DL3_ERRSYNCESC_LEN                                  (1U)
#define DSI_DL3_ERRSYNCESC_MSK                                  (((1U<<DSI_DL3_ERRSYNCESC_LEN)-1)<<DSI_DL3_ERRSYNCESC_POS)
#define DSI_DL3_ERRSYNCESC_UMSK                                 (~(((1U<<DSI_DL3_ERRSYNCESC_LEN)-1)<<DSI_DL3_ERRSYNCESC_POS))
#define DSI_DL0_ERRESC                                          DSI_DL0_ERRESC
#define DSI_DL0_ERRESC_POS                                      (8U)
#define DSI_DL0_ERRESC_LEN                                      (1U)
#define DSI_DL0_ERRESC_MSK                                      (((1U<<DSI_DL0_ERRESC_LEN)-1)<<DSI_DL0_ERRESC_POS)
#define DSI_DL0_ERRESC_UMSK                                     (~(((1U<<DSI_DL0_ERRESC_LEN)-1)<<DSI_DL0_ERRESC_POS))
#define DSI_DL1_ERRESC                                          DSI_DL1_ERRESC
#define DSI_DL1_ERRESC_POS                                      (9U)
#define DSI_DL1_ERRESC_LEN                                      (1U)
#define DSI_DL1_ERRESC_MSK                                      (((1U<<DSI_DL1_ERRESC_LEN)-1)<<DSI_DL1_ERRESC_POS)
#define DSI_DL1_ERRESC_UMSK                                     (~(((1U<<DSI_DL1_ERRESC_LEN)-1)<<DSI_DL1_ERRESC_POS))
#define DSI_DL2_ERRESC                                          DSI_DL2_ERRESC
#define DSI_DL2_ERRESC_POS                                      (10U)
#define DSI_DL2_ERRESC_LEN                                      (1U)
#define DSI_DL2_ERRESC_MSK                                      (((1U<<DSI_DL2_ERRESC_LEN)-1)<<DSI_DL2_ERRESC_POS)
#define DSI_DL2_ERRESC_UMSK                                     (~(((1U<<DSI_DL2_ERRESC_LEN)-1)<<DSI_DL2_ERRESC_POS))
#define DSI_DL3_ERRESC                                          DSI_DL3_ERRESC
#define DSI_DL3_ERRESC_POS                                      (11U)
#define DSI_DL3_ERRESC_LEN                                      (1U)
#define DSI_DL3_ERRESC_MSK                                      (((1U<<DSI_DL3_ERRESC_LEN)-1)<<DSI_DL3_ERRESC_POS)
#define DSI_DL3_ERRESC_UMSK                                     (~(((1U<<DSI_DL3_ERRESC_LEN)-1)<<DSI_DL3_ERRESC_POS))
#define DSI_DL0_RXTRIGGERESC_3_0                               DSI_DL0_RXTRIGGERESC_3_0
#define DSI_DL0_RXTRIGGERESC_3_0_POS                           (16U)
#define DSI_DL0_RXTRIGGERESC_3_0_LEN                           (4U)
#define DSI_DL0_RXTRIGGERESC_3_0_MSK                           (((1U<<DSI_DL0_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL0_RXTRIGGERESC_3_0_POS)
#define DSI_DL0_RXTRIGGERESC_3_0_UMSK                          (~(((1U<<DSI_DL0_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL0_RXTRIGGERESC_3_0_POS))
#define DSI_DL1_RXTRIGGERESC_3_0                               DSI_DL1_RXTRIGGERESC_3_0
#define DSI_DL1_RXTRIGGERESC_3_0_POS                           (20U)
#define DSI_DL1_RXTRIGGERESC_3_0_LEN                           (4U)
#define DSI_DL1_RXTRIGGERESC_3_0_MSK                           (((1U<<DSI_DL1_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL1_RXTRIGGERESC_3_0_POS)
#define DSI_DL1_RXTRIGGERESC_3_0_UMSK                          (~(((1U<<DSI_DL1_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL1_RXTRIGGERESC_3_0_POS))
#define DSI_DL2_RXTRIGGERESC_3_0                               DSI_DL2_RXTRIGGERESC_3_0
#define DSI_DL2_RXTRIGGERESC_3_0_POS                           (24U)
#define DSI_DL2_RXTRIGGERESC_3_0_LEN                           (4U)
#define DSI_DL2_RXTRIGGERESC_3_0_MSK                           (((1U<<DSI_DL2_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL2_RXTRIGGERESC_3_0_POS)
#define DSI_DL2_RXTRIGGERESC_3_0_UMSK                          (~(((1U<<DSI_DL2_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL2_RXTRIGGERESC_3_0_POS))
#define DSI_DL3_RXTRIGGERESC_3_0                               DSI_DL3_RXTRIGGERESC_3_0
#define DSI_DL3_RXTRIGGERESC_3_0_POS                           (28U)
#define DSI_DL3_RXTRIGGERESC_3_0_LEN                           (4U)
#define DSI_DL3_RXTRIGGERESC_3_0_MSK                           (((1U<<DSI_DL3_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL3_RXTRIGGERESC_3_0_POS)
#define DSI_DL3_RXTRIGGERESC_3_0_UMSK                          (~(((1U<<DSI_DL3_RXTRIGGERESC_3_0_LEN)-1)<<DSI_DL3_RXTRIGGERESC_3_0_POS))

/* 0x78 : dphy_rb_2 */
#define DSI_DPHY_RB_2_OFFSET                                    (0x78)
#define DSI_REG_RD_DIG_TEST_BUS                                 DSI_REG_RD_DIG_TEST_BUS
#define DSI_REG_RD_DIG_TEST_BUS_POS                             (0U)
#define DSI_REG_RD_DIG_TEST_BUS_LEN                             (16U)
#define DSI_REG_RD_DIG_TEST_BUS_MSK                             (((1U<<DSI_REG_RD_DIG_TEST_BUS_LEN)-1)<<DSI_REG_RD_DIG_TEST_BUS_POS)
#define DSI_REG_RD_DIG_TEST_BUS_UMSK                            (~(((1U<<DSI_REG_RD_DIG_TEST_BUS_LEN)-1)<<DSI_REG_RD_DIG_TEST_BUS_POS))
#define DSI_DPHY_RX_CLK_ULPS_N                                  DSI_DPHY_RX_CLK_ULPS_N
#define DSI_DPHY_RX_CLK_ULPS_N_POS                              (16U)
#define DSI_DPHY_RX_CLK_ULPS_N_LEN                              (1U)
#define DSI_DPHY_RX_CLK_ULPS_N_MSK                              (((1U<<DSI_DPHY_RX_CLK_ULPS_N_LEN)-1)<<DSI_DPHY_RX_CLK_ULPS_N_POS)
#define DSI_DPHY_RX_CLK_ULPS_N_UMSK                             (~(((1U<<DSI_DPHY_RX_CLK_ULPS_N_LEN)-1)<<DSI_DPHY_RX_CLK_ULPS_N_POS))
#define DSI_DPHY_LANE_DIR_3_0                                  DSI_DPHY_LANE_DIR_3_0
#define DSI_DPHY_LANE_DIR_3_0_POS                              (20U)
#define DSI_DPHY_LANE_DIR_3_0_LEN                              (4U)
#define DSI_DPHY_LANE_DIR_3_0_MSK                              (((1U<<DSI_DPHY_LANE_DIR_3_0_LEN)-1)<<DSI_DPHY_LANE_DIR_3_0_POS)
#define DSI_DPHY_LANE_DIR_3_0_UMSK                             (~(((1U<<DSI_DPHY_LANE_DIR_3_0_LEN)-1)<<DSI_DPHY_LANE_DIR_3_0_POS))
#define DSI_DPHY_ERR_CONT_LP1_3_0                              DSI_DPHY_ERR_CONT_LP1_3_0
#define DSI_DPHY_ERR_CONT_LP1_3_0_POS                          (24U)
#define DSI_DPHY_ERR_CONT_LP1_3_0_LEN                          (4U)
#define DSI_DPHY_ERR_CONT_LP1_3_0_MSK                          (((1U<<DSI_DPHY_ERR_CONT_LP1_3_0_LEN)-1)<<DSI_DPHY_ERR_CONT_LP1_3_0_POS)
#define DSI_DPHY_ERR_CONT_LP1_3_0_UMSK                         (~(((1U<<DSI_DPHY_ERR_CONT_LP1_3_0_LEN)-1)<<DSI_DPHY_ERR_CONT_LP1_3_0_POS))
#define DSI_DPHY_ERR_CONT_LP0_3_0                              DSI_DPHY_ERR_CONT_LP0_3_0
#define DSI_DPHY_ERR_CONT_LP0_3_0_POS                          (28U)
#define DSI_DPHY_ERR_CONT_LP0_3_0_LEN                          (4U)
#define DSI_DPHY_ERR_CONT_LP0_3_0_MSK                          (((1U<<DSI_DPHY_ERR_CONT_LP0_3_0_LEN)-1)<<DSI_DPHY_ERR_CONT_LP0_3_0_POS)
#define DSI_DPHY_ERR_CONT_LP0_3_0_UMSK                         (~(((1U<<DSI_DPHY_ERR_CONT_LP0_3_0_LEN)-1)<<DSI_DPHY_ERR_CONT_LP0_3_0_POS))

#endif /* __DSI_REG_H__ */