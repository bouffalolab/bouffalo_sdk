#include "bflb_core.h"
#include "bflb_dsi.h"
#include "hardware/dsi_reg.h"

#define DSI_TX_TIMEOUT_COUNT (320 * 1000)
#define DSI_PHY_ADDR_BASE (0x20000000 + 0x700)

/* small busy-wait helper to avoid tight spin when polling */
static inline void dsi_delay_short(void)
{
    __ASM volatile("nop; nop; nop; nop");
}

static inline uint32_t dsi_base(const struct bflb_device_s *dev)
{
    return (uint32_t)dev->reg_base;
}

void bflb_dsi_init(struct bflb_device_s *dev, const bflb_dsi_config_t *cfg)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val;
    uint32_t lanes_mask = 0;

    reg_val = getreg32(reg_base + DSI_CONFIG_OFFSET);
    reg_val &= ~DSI_CR_LANE_NUM_MSK;
    reg_val |= ((uint32_t)cfg->lane_num << DSI_CR_LANE_NUM_POS) & DSI_CR_LANE_NUM_MSK;
    reg_val &= ~DSI_CR_HSTX_MODE_MSK;
    reg_val |= ((uint32_t)cfg->sync_type << DSI_CR_HSTX_MODE_POS) & DSI_CR_HSTX_MODE_MSK;
    reg_val &= ~DSI_CR_LANE_MUX_SEL_MSK;
    reg_val |= ((uint32_t)cfg->lane_order << DSI_CR_LANE_MUX_SEL_POS) & DSI_CR_LANE_MUX_SEL_MSK;
    reg_val &= ~DSI_CR_DT_MSK;
    reg_val |= ((uint32_t)cfg->data_type << DSI_CR_DT_POS) & DSI_CR_DT_MSK;
    reg_val &= ~DSI_CR_VC_MSK;
    reg_val |= ((uint32_t)cfg->virtual_chan << DSI_CR_VC_POS) & DSI_CR_VC_MSK;
    reg_val &= ~DSI_CR_HSTX_VFP_MSK;
    reg_val |= ((uint32_t)cfg->vfp << DSI_CR_HSTX_VFP_POS) & DSI_CR_HSTX_VFP_MSK;
    reg_val &= ~DSI_CR_HSTX_VSA_MSK;
    reg_val |= ((uint32_t)cfg->vsa << DSI_CR_HSTX_VSA_POS) & DSI_CR_HSTX_VSA_MSK;
    putreg32(reg_val, reg_base + DSI_CONFIG_OFFSET);

    /* Auto-enable lanes based on lane_num configuration (matching std driver behavior) */
    lanes_mask = BFLB_DSI_LANE_CLOCK; /* Always enable clock lane */

    if (cfg->lane_num == BFLB_DSI_LANES_1) {
        lanes_mask |= BFLB_DSI_LANE_DATA0;
    } else if (cfg->lane_num == BFLB_DSI_LANES_2) {
        lanes_mask |= BFLB_DSI_LANE_DATA0 | BFLB_DSI_LANE_DATA1;
    } else if (cfg->lane_num == BFLB_DSI_LANES_4) {
        lanes_mask |= BFLB_DSI_LANE_DATA0 | BFLB_DSI_LANE_DATA1 | BFLB_DSI_LANE_DATA2 | BFLB_DSI_LANE_DATA3;
    }

    bflb_dsi_phy_enable_lanes(dev, lanes_mask);
}

void bflb_dsi_deinit(struct bflb_device_s *dev)
{
    (void)dev;
}

void bflb_dsi_int_mask(struct bflb_device_s *dev, uint32_t int_mask, bool mask)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_INT_MASK_OFFSET);
    if (mask) {
        reg_val |= int_mask;
    } else {
        reg_val &= ~int_mask;
    }
    putreg32(reg_val, reg_base + DSI_INT_MASK_OFFSET);
}

void bflb_dsi_int_clear(struct bflb_device_s *dev, uint32_t int_flag)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_INT_CLEAR_OFFSET);
    reg_val |= int_flag;
    putreg32(reg_val, reg_base + DSI_INT_CLEAR_OFFSET);
}

void bflb_dsi_int_enable(struct bflb_device_s *dev, uint32_t int_type)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_INT_ENABLE_OFFSET);
    reg_val |= int_type;
    putreg32(reg_val, reg_base + DSI_INT_ENABLE_OFFSET);
}

uint32_t bflb_dsi_int_get(struct bflb_device_s *dev)
{
    uint32_t reg_base = dsi_base(dev);
    return getreg32(reg_base + DSI_INT_STATUS_OFFSET);
}

void bflb_dsi_set_line_buffer_threshold(struct bflb_device_s *dev, uint32_t frame_width,
                                        uint32_t dvp_tsrc_clock, uint32_t dsi_clock,
                                        uint32_t data_type, uint32_t lane_number)
{
    uint32_t regval;
    uint32_t regbase = dsi_base(dev);
    int64_t threshold = dvp_tsrc_clock;
    uint32_t pixel_byte;

    /* Frame Width should not exceed 1280 and should be a multiple of 4 */
    if (frame_width > 1280 || (frame_width % 4) != 0) {
        return;
    }

    /* Determine bytes per pixel based on data type */
    if (data_type < BFLB_DSI_DATA_RGB666) {
        pixel_byte = 2;
    } else {
        pixel_byte = 3;
    }

    /* Calculate threshold */
    threshold = frame_width - threshold * frame_width * pixel_byte / (dsi_clock / 8) / (1 << lane_number);

    if (threshold < 6) {
        threshold = 6;
    }

    regval = getreg32(regbase + DSI_HSTX_CONFIG_OFFSET);
    regval = (regval & ~DSI_CR_HSTX_PC_MSK) | (frame_width << DSI_CR_HSTX_PC_POS);
    regval = (regval & ~DSI_CR_HSTX_OUT_TH_MSK) | (threshold << DSI_CR_HSTX_OUT_TH_POS);
    putreg32(regval, regbase + DSI_HSTX_CONFIG_OFFSET);
}

void bflb_dsi_set_vsa_vfp(struct bflb_device_s *dev, uint8_t vsa, uint8_t vfp)
{
    uint32_t regval;
    uint32_t regbase = dsi_base(dev);;

    regval = getreg32(regbase + DSI_CONFIG_OFFSET);
    regval = (regval & ~DSI_CR_HSTX_VFP_MSK) | (vfp << DSI_CR_HSTX_VFP_POS);
    regval = (regval & ~DSI_CR_HSTX_VSA_MSK) | (vsa << DSI_CR_HSTX_VSA_POS);
    putreg32(regval, regbase + DSI_CONFIG_OFFSET);
}

void bflb_dsi_phy_reset(struct bflb_device_s *dev)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);
    reg_val &= ~DSI_DPHY_RESET_N_MSK;
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);
    reg_val |= (1U << DSI_DPHY_RESET_N_POS);
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);
}

void bflb_dsi_phy_config(struct bflb_device_s *dev, const bflb_dsi_dphy_config_t *phy)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val;

    reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_7_OFFSET);
    reg_val &= ~DSI_REG_TIME_CK_ZERO_MSK;
    reg_val |= ((uint32_t)phy->time_clk_zero << DSI_REG_TIME_CK_ZERO_POS) & DSI_REG_TIME_CK_ZERO_MSK;
    reg_val &= ~DSI_REG_TIME_CK_TRAIL_MSK;
    reg_val |= ((uint32_t)phy->time_clk_trail << DSI_REG_TIME_CK_TRAIL_POS) & DSI_REG_TIME_CK_TRAIL_MSK;
    reg_val &= ~DSI_REG_TIME_CK_EXIT_MSK;
    reg_val |= ((uint32_t)phy->time_clk_exit << DSI_REG_TIME_CK_EXIT_POS) & DSI_REG_TIME_CK_EXIT_MSK;
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_7_OFFSET);

    reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_9_OFFSET);
    reg_val &= ~DSI_REG_TIME_HS_ZERO_MSK;
    reg_val |= ((uint32_t)phy->time_data_zero << DSI_REG_TIME_HS_ZERO_POS) & DSI_REG_TIME_HS_ZERO_MSK;
    reg_val &= ~DSI_REG_TIME_HS_TRAIL_MSK;
    reg_val |= ((uint32_t)phy->time_data_trail << DSI_REG_TIME_HS_TRAIL_POS) & DSI_REG_TIME_HS_TRAIL_MSK;
    reg_val &= ~DSI_REG_TIME_HS_EXIT_MSK;
    reg_val |= ((uint32_t)phy->time_data_exit << DSI_REG_TIME_HS_EXIT_POS) & DSI_REG_TIME_HS_EXIT_MSK;
    reg_val &= ~DSI_REG_TIME_HS_PREP_MSK;
    reg_val |= ((uint32_t)phy->time_data_prepare << DSI_REG_TIME_HS_PREP_POS) & DSI_REG_TIME_HS_PREP_MSK;
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_9_OFFSET);

    reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_10_OFFSET);
    reg_val &= ~DSI_REG_TIME_TA_GO_MSK;
    reg_val |= ((uint32_t)phy->time_ta_go << DSI_REG_TIME_TA_GO_POS) & DSI_REG_TIME_TA_GO_MSK;
    reg_val &= ~DSI_REG_TIME_TA_GET_MSK;
    reg_val |= ((uint32_t)phy->time_ta_get << DSI_REG_TIME_TA_GET_POS) & DSI_REG_TIME_TA_GET_MSK;
    reg_val &= ~DSI_REG_TIME_REQRDY_MSK;
    reg_val |= ((uint32_t)phy->time_req_ready << DSI_REG_TIME_REQRDY_POS) & DSI_REG_TIME_REQRDY_MSK;
    reg_val &= ~DSI_REG_TIME_LPX_MSK;
    reg_val |= ((uint32_t)phy->time_lpx << DSI_REG_TIME_LPX_POS) & DSI_REG_TIME_LPX_MSK;
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_10_OFFSET);

    reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_11_OFFSET);
    reg_val &= ~DSI_REG_TIME_WAKEUP_MSK;
    reg_val |= ((uint32_t)phy->time_wakeup << DSI_REG_TIME_WAKEUP_POS) & DSI_REG_TIME_WAKEUP_MSK;
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_11_OFFSET);
}

void bflb_dsi_phy_reinit_data_lanes(struct bflb_device_s *dev, uint32_t lanes_mask)
{
    uint32_t regval;
    uint32_t regbase = DSI_PHY_ADDR_BASE;

    regval = getreg32(regbase + DSI_DPHY_CONFIG_14_OFFSET);

    /* Clear all force rx mode bits first */
    regval &= ~(DSI_DL0_FORCERXMODE_MSK | DSI_DL1_FORCERXMODE_MSK |
                DSI_DL2_FORCERXMODE_MSK | DSI_DL3_FORCERXMODE_MSK);

    /* Set force rx mode for specified lanes */
    if (lanes_mask & BFLB_DSI_LANE_DATA0) {
        regval |= DSI_DL0_FORCERXMODE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA1) {
        regval |= DSI_DL1_FORCERXMODE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA2) {
        regval |= DSI_DL2_FORCERXMODE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA3) {
        regval |= DSI_DL3_FORCERXMODE_MSK;
    }

    putreg32(regval, regbase + DSI_DPHY_CONFIG_14_OFFSET);
}

void bflb_dsi_phy_enable_lanes(struct bflb_device_s *dev, uint32_t lanes_mask)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);
    if (lanes_mask & BFLB_DSI_LANE_DATA0) {
        reg_val |= (1U << DSI_DL0_ENABLE_POS);
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA1) {
        reg_val |= (1U << DSI_DL1_ENABLE_POS);
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA2) {
        reg_val |= (1U << DSI_DL2_ENABLE_POS);
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA3) {
        reg_val |= (1U << DSI_DL3_ENABLE_POS);
    }
    if (lanes_mask & BFLB_DSI_LANE_CLOCK) {
        reg_val |= (1U << DSI_CL_ENABLE_POS);
    }
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);
}

void bflb_dsi_phy_disable_lanes(struct bflb_device_s *dev, uint32_t lanes_mask)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);
    if (lanes_mask & BFLB_DSI_LANE_DATA0) {
        reg_val &= ~DSI_DL0_ENABLE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA1) {
        reg_val &= ~DSI_DL1_ENABLE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA2) {
        reg_val &= ~DSI_DL2_ENABLE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA3) {
        reg_val &= ~DSI_DL3_ENABLE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_CLOCK) {
        reg_val &= ~DSI_CL_ENABLE_MSK;
    }
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);
}

void bflb_dsi_phy_stop_data_lanes(struct bflb_device_s *dev, uint32_t lanes_mask)
{
    uint32_t regval;
    uint32_t regbase = DSI_PHY_ADDR_BASE;

    regval = getreg32(regbase + DSI_DPHY_CONFIG_14_OFFSET);

    /* Clear all force tx stop mode bits first */
    regval &= ~(DSI_DL0_FORCETXSTOPMODE_MSK | DSI_DL1_FORCETXSTOPMODE_MSK |
                DSI_DL2_FORCETXSTOPMODE_MSK | DSI_DL3_FORCETXSTOPMODE_MSK);

    /* Set force tx stop mode for specified lanes */
    if (lanes_mask & BFLB_DSI_LANE_DATA0) {
        regval |= DSI_DL0_FORCETXSTOPMODE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA1) {
        regval |= DSI_DL1_FORCETXSTOPMODE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA2) {
        regval |= DSI_DL2_FORCETXSTOPMODE_MSK;
    }
    if (lanes_mask & BFLB_DSI_LANE_DATA3) {
        regval |= DSI_DL3_FORCETXSTOPMODE_MSK;
    }

    putreg32(regval, regbase + DSI_DPHY_CONFIG_14_OFFSET);
}

void bflb_dsi_phy_data_lane0_turnaround(struct bflb_device_s *dev)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);
    reg_val |= (1U << DSI_DL0_TURNESC_POS);
    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);
}

void bflb_dsi_phy_get_lanes_state(struct bflb_device_s *dev, uint32_t lane, uint32_t *state)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val = getreg32(reg_base + DSI_DPHY_RB_0_OFFSET);
    uint32_t stop_pos = 0, ulp_pos = 0;

    *state = BFLB_DSI_LANE_STATE_NORMAL;

    /* Map lane type to register bit positions */
    if (lane == BFLB_DSI_LANE_DATA0) {
        stop_pos = DSI_DL0_STOPSTATE_POS;
        ulp_pos = DSI_DL0_ULPSACTIVENOT_POS;
    } else if (lane == BFLB_DSI_LANE_DATA1) {
        stop_pos = DSI_DL1_STOPSTATE_POS;
        ulp_pos = DSI_DL1_ULPSACTIVENOT_POS;
    } else if (lane == BFLB_DSI_LANE_DATA2) {
        stop_pos = DSI_DL2_STOPSTATE_POS;
        ulp_pos = DSI_DL2_ULPSACTIVENOT_POS;
    } else if (lane == BFLB_DSI_LANE_DATA3) {
        stop_pos = DSI_DL3_STOPSTATE_POS;
        ulp_pos = DSI_DL3_ULPSACTIVENOT_POS;
    } else if (lane == BFLB_DSI_LANE_CLOCK) {
        stop_pos = DSI_CL_STOPSTATE_POS;
        ulp_pos = DSI_CL_ULPSACTIVENOT_POS;
    } else {
        return; /* Invalid lane */
    }

    /* Check ULP state first (ULPSACTIVENOT=0 means in ULP mode) */
    if (!(reg_val & (1U << ulp_pos))) {
        *state = BFLB_DSI_LANE_STATE_ULP;
    }
    /* Check stop state */
    else if (reg_val & (1U << stop_pos)) {
        *state = BFLB_DSI_LANE_STATE_STOP;
    }
    /* Check HS or bridge state */
    else {
        if (lane == BFLB_DSI_LANE_CLOCK) {
            /* For clock lane: check CL_TXREQUESTHS in CONFIG_14 register */
            uint32_t config14_reg = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);
            if (config14_reg & (1U << DSI_CL_TXREQUESTHS_POS)) {
                *state = BFLB_DSI_LANE_STATE_HS;
            } else {
                *state = BFLB_DSI_LANE_STATE_BRIDGE;
            }
        } else {
            /* For data lanes: check HSTX_EN */
            uint32_t config_reg = getreg32(reg_base + DSI_CONFIG_OFFSET);
            if (config_reg & (1U << DSI_CR_HSTX_EN_POS)) {
                *state = BFLB_DSI_LANE_STATE_HS;
            } else {
                *state = BFLB_DSI_LANE_STATE_BRIDGE;
            }
        }
    }
}

int bflb_dsi_phy_set_clock_lane(struct bflb_device_s *dev, uint32_t operations)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);

    if (operations & BFLB_DSI_CLOCK_LANE_ULP_ENTER) {
        reg_val |= (1U << DSI_CL_TXULPSCLK_POS);
    }

    if (operations & BFLB_DSI_CLOCK_LANE_ULP_EXIT) {
        reg_val |= (1U << DSI_CL_TXULPSEXIT_POS);
        putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);
        /* delay 1ms as per original driver */
        for (volatile uint32_t i = 0; i < 1000; i++)
            dsi_delay_short();
        reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);
        reg_val &= ~(1U << DSI_CL_TXULPSCLK_POS);
    }

    if (operations & BFLB_DSI_CLOCK_LANE_HS_REQ) {
        reg_val |= (1U << DSI_CL_TXREQUESTHS_POS);
    }

    if (operations & BFLB_DSI_CLOCK_LANE_HS_EXIT) {
        reg_val &= ~(1U << DSI_CL_TXREQUESTHS_POS);
    }

    putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);

    return 0;
}

void bflb_dsi_phy_hs_mode_start(struct bflb_device_s *dev)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val;
    uint32_t state;
    /* bring clock lane to high speed */
    bflb_dsi_phy_get_lanes_state(dev, BFLB_DSI_LANE_CLOCK, &state);
    if (state == BFLB_DSI_LANE_STATE_STOP) {
        bflb_dsi_phy_set_clock_lane(dev, BFLB_DSI_CLOCK_LANE_HS_REQ);
    }

    /* bring data lane to high speed */
    bflb_dsi_phy_get_lanes_state(dev, BFLB_DSI_LANE_DATA0, &state);
    if (state == BFLB_DSI_LANE_STATE_STOP) {
        reg_val = getreg32(reg_base + DSI_CONFIG_OFFSET);
        reg_val = (reg_val & ~DSI_CR_HSTX_EN_MSK) | (1U << DSI_CR_HSTX_EN_POS);
        putreg32(reg_val, reg_base + DSI_CONFIG_OFFSET);
    }
}

int bflb_dsi_phy_hs_mode_stop(struct bflb_device_s *dev)
{
    uint32_t reg_base = DSI_PHY_ADDR_BASE;
    uint32_t reg_val;
    uint32_t state;
    uint32_t timeout = DSI_TX_TIMEOUT_COUNT;

    bflb_dsi_phy_get_lanes_state(dev, BFLB_DSI_LANE_DATA0, &state);
    if (state != BFLB_DSI_LANE_STATE_STOP) {
        /* stop data lane HS */
        bflb_dsi_phy_set_clock_lane(dev, BFLB_DSI_CLOCK_LANE_HS_EXIT);
        do {
            bflb_dsi_phy_get_lanes_state(dev, BFLB_DSI_LANE_DATA0, &state);
            if (state == BFLB_DSI_LANE_STATE_STOP) {
                break;
            }
            if (--timeout == 0) {
                return -ETIMEDOUT;
            }
            dsi_delay_short();

        } while (1);
    }

    bflb_dsi_phy_get_lanes_state(dev, BFLB_DSI_LANE_CLOCK, &state);
    if (state != BFLB_DSI_LANE_STATE_STOP) {
        /* exit clock lane HS */
        reg_val = getreg32(reg_base + DSI_DPHY_CONFIG_14_OFFSET);
        reg_val &= ~DSI_CL_TXREQUESTHS_MSK;
        putreg32(reg_val, reg_base + DSI_DPHY_CONFIG_14_OFFSET);
        do {
            bflb_dsi_phy_get_lanes_state(dev, BFLB_DSI_LANE_CLOCK, &state);
            if (state == BFLB_DSI_LANE_STATE_STOP) {
                break;
            }
            if (--timeout == 0) {
                return -ETIMEDOUT;
            }
            dsi_delay_short();

        } while (1);
    }
    return 0;
}

int bflb_dsi_fifo_config(struct bflb_device_s *dev, const bflb_dsi_fifo_config_t *fifo_cfg)
{
    uint32_t regval;
    uint32_t regbase = dsi_base(dev);;

    /* Configure FIFO thresholds */
    regval = getreg32(regbase + DSI_FIFO_CONFIG_1_OFFSET);
    regval = (regval & ~DSI_TX_FIFO_TH_MSK) | (fifo_cfg->tx_fifo_dma_threshold << DSI_TX_FIFO_TH_POS);
    regval = (regval & ~DSI_RX_FIFO_TH_MSK) | (fifo_cfg->rx_fifo_dma_threshold << DSI_RX_FIFO_TH_POS);
    putreg32(regval, regbase + DSI_FIFO_CONFIG_1_OFFSET);

    /* Configure FIFO DMA enables */
    regval = getreg32(regbase + DSI_FIFO_CONFIG_0_OFFSET);

    if (fifo_cfg->tx_fifo_dma_enable) {
        regval |= DSI_DMA_TX_EN_MSK;
    } else {
        regval &= ~DSI_DMA_TX_EN_MSK;
    }

    if (fifo_cfg->rx_fifo_dma_enable) {
        regval |= DSI_DMA_RX_EN_MSK;
    } else {
        regval &= ~DSI_DMA_RX_EN_MSK;
    }

    putreg32(regval, regbase + DSI_FIFO_CONFIG_0_OFFSET);

    return 0;
}

int bflb_dsi_lpdt_start_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_ESC_CONFIG_OFFSET);
    reg_val &= ~DSI_CR_ESC_RX_EN_MSK;
    putreg32(reg_val, reg_base + DSI_ESC_CONFIG_OFFSET);
    reg_val |= (1U << DSI_CR_ESC_TX_EN_POS);
    putreg32(reg_val, reg_base + DSI_ESC_CONFIG_OFFSET);
    return 0;
}

int bflb_dsi_wait_tx_done(struct bflb_device_s *dev)
{
    uint32_t int_status;
    uint32_t timeout = DSI_TX_TIMEOUT_COUNT;

    /* wait for Tx finished - following std driver logic */
    do {
        int_status = bflb_dsi_int_get(dev);
        if (int_status & BFLB_DSI_INT_ESCAPE_TX_END) {
            bflb_dsi_int_clear(dev, BFLB_DSI_INT_ESCAPE_TX_END);
            return 0;
        }
        if (--timeout == 0) {
            return -ETIMEDOUT;
        }
        dsi_delay_short();
    } while (1);
}

int bflb_dsi_lpdt_start_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_ESC_CONFIG_OFFSET);
    reg_val &= ~DSI_CR_ESC_RX_EN_MSK;
    putreg32(reg_val, reg_base + DSI_ESC_CONFIG_OFFSET);
    reg_val |= (1U << DSI_CR_ESC_RX_EN_POS);
    putreg32(reg_val, reg_base + DSI_ESC_CONFIG_OFFSET);
    return 0;
}

uint16_t bflb_dsi_lpdt_get_rx_length(struct bflb_device_s *dev)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_ESC_CONFIG_OFFSET);
    return (uint16_t)((reg_val & DSI_ST_ESC_RX_LEN_MSK) >> DSI_ST_ESC_RX_LEN_POS);
}

int bflb_dsi_send_trigger_command(struct bflb_device_s *dev, uint8_t cmd)
{
    uint32_t regval;
    uint32_t regbase = dsi_base(dev);;

    /* Stop HS mode before escape mode operations */
    bflb_dsi_phy_hs_mode_stop(dev);

    /* Configure escape mode tx mode to trigger command (1) */
    regval = getreg32(regbase + DSI_ESC_CONFIG_OFFSET);
    regval = (regval & ~DSI_CR_ESC_TX_MODE_MSK) | (1 << DSI_CR_ESC_TX_MODE_POS);
    regval = (regval & ~DSI_CR_ESC_TX_TRIG_MSK) | (cmd << DSI_CR_ESC_TX_TRIG_POS);
    putreg32(regval, regbase + DSI_ESC_CONFIG_OFFSET);

    /* Enable escape mode tx */
    regval = getreg32(regbase + DSI_ESC_CONFIG_OFFSET);
    regval |= DSI_CR_ESC_TX_EN_MSK;
    putreg32(regval, regbase + DSI_ESC_CONFIG_OFFSET);

    return bflb_dsi_wait_tx_done(dev);
}

int bflb_dsi_send_ulps_command(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t regbase = dsi_base(dev);;

    /* Stop HS mode before ULPS command */
    bflb_dsi_phy_hs_mode_stop(dev);

    /* Configure escape mode tx mode to ULPS (2) */
    regval = getreg32(regbase + DSI_ESC_CONFIG_OFFSET);
    regval = (regval & DSI_CR_ESC_TX_MODE_UMSK) | (2 << DSI_CR_ESC_TX_MODE_POS);
    putreg32(regval, regbase + DSI_ESC_CONFIG_OFFSET);

    /* Enable escape mode tx */
    regval = getreg32(regbase + DSI_ESC_CONFIG_OFFSET);
    regval |= DSI_CR_ESC_TX_EN_MSK;
    putreg32(regval, regbase + DSI_ESC_CONFIG_OFFSET);

    return bflb_dsi_wait_tx_done(dev);
}

int bflb_dsi_exit_ulps(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t regbase = dsi_base(dev);;

    /* Configure ULPS exit */
    regval = getreg32(regbase + DSI_ESC_CONFIG_OFFSET);
    regval = (regval & DSI_CR_ESC_TX_MODE_UMSK) | (1 << DSI_CR_ESC_TX_ULPS_EXIT_POS);
    putreg32(regval, regbase + DSI_ESC_CONFIG_OFFSET);

    /* Wait 2ms for ULPS exit as required by MIPI spec */
    bflb_mtimer_delay_ms(2);

    /* Configure escape mode tx mode to mark/space (3) */
    regval = getreg32(regbase + DSI_ESC_CONFIG_OFFSET);
    regval = (regval & DSI_CR_ESC_TX_MODE_UMSK) | (3 << DSI_CR_ESC_TX_MODE_POS);
    putreg32(regval, regbase + DSI_ESC_CONFIG_OFFSET);

    /* Enable escape mode tx */
    regval = getreg32(regbase + DSI_ESC_CONFIG_OFFSET);
    regval |= DSI_CR_ESC_TX_EN_MSK;
    putreg32(regval, regbase + DSI_ESC_CONFIG_OFFSET);

    return bflb_dsi_wait_tx_done(dev);
}

/* Additional API functions following std driver pattern */
int bflb_dsi_lpdt_config_short_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val;

    /* ensure HS stopped before escape - following std driver */
    bflb_dsi_phy_hs_mode_stop(dev);

    reg_val = getreg32(reg_base + DSI_ESC_CONFIG_OFFSET);
    reg_val &= ~DSI_CR_ESC_TX_MODE_MSK;
    reg_val |= (0U << DSI_CR_ESC_TX_MODE_POS); /* escape tx mode = 0 for LPDT */
    putreg32(reg_val, reg_base + DSI_ESC_CONFIG_OFFSET);

    /* configure short packet */
    reg_val = getreg32(reg_base + DSI_LPDT_TX_CONFIG_OFFSET);
    reg_val &= ~DSI_CR_LPDT_PDLEN_MSK;
    reg_val |= (0U << DSI_CR_LPDT_PDLEN_POS); /* payload length = 0 for short packet */
    reg_val &= ~DSI_CR_LPDT_VC_MSK;
    reg_val |= ((uint32_t)msg->virtual_chan << DSI_CR_LPDT_VC_POS) & DSI_CR_LPDT_VC_MSK;
    reg_val &= ~DSI_CR_LPDT_DI_MSK;
    reg_val |= ((uint32_t)msg->data_type << DSI_CR_LPDT_DI_POS) & DSI_CR_LPDT_DI_MSK;
    reg_val &= ~DSI_CR_LPDT_WORD0_MSK;
    reg_val |= ((uint32_t)msg->tx_buf[0] << DSI_CR_LPDT_WORD0_POS) & DSI_CR_LPDT_WORD0_MSK;
    reg_val &= ~DSI_CR_LPDT_WORD1_MSK;
    reg_val |= ((uint32_t)msg->tx_buf[1] << DSI_CR_LPDT_WORD1_POS) & DSI_CR_LPDT_WORD1_MSK;
    putreg32(reg_val, reg_base + DSI_LPDT_TX_CONFIG_OFFSET);

    return 0;
}

int bflb_dsi_lpdt_send_short_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg)
{
    /* Following std driver DSI_LPDT_Send_Short_Packet logic */
    int ret;

    /* Config short packet - call the API function */
    ret = bflb_dsi_lpdt_config_short_packet(dev, msg);
    if (ret != 0) {
        return ret;
    }

    /* Start TX - call the API function */
    ret = bflb_dsi_lpdt_start_tx(dev);
    if (ret != 0) {
        return ret;
    }

    /* Wait for completion - call the API function */
    return bflb_dsi_wait_tx_done(dev);
}

int bflb_dsi_lpdt_config_long_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val;

    /* ensure HS stopped before escape - following std driver */
    bflb_dsi_phy_hs_mode_stop(dev);

    reg_val = getreg32(reg_base + DSI_ESC_CONFIG_OFFSET);
    reg_val &= ~DSI_CR_ESC_TX_MODE_MSK;
    reg_val |= (0U << DSI_CR_ESC_TX_MODE_POS); /* escape tx mode = 0 for LPDT */
    putreg32(reg_val, reg_base + DSI_ESC_CONFIG_OFFSET);

    /* configure long packet */
    reg_val = getreg32(reg_base + DSI_LPDT_TX_CONFIG_OFFSET);
    reg_val &= ~DSI_CR_LPDT_VC_MSK;
    reg_val |= ((uint32_t)msg->virtual_chan << DSI_CR_LPDT_VC_POS) & DSI_CR_LPDT_VC_MSK;
    reg_val &= ~DSI_CR_LPDT_DI_MSK;
    reg_val |= ((uint32_t)msg->data_type << DSI_CR_LPDT_DI_POS) & DSI_CR_LPDT_DI_MSK;
    reg_val &= ~DSI_CR_LPDT_WORD0_MSK;
    reg_val |= ((uint32_t)(msg->tx_len & 0xff) << DSI_CR_LPDT_WORD0_POS) & DSI_CR_LPDT_WORD0_MSK;
    reg_val &= ~DSI_CR_LPDT_WORD1_MSK;
    reg_val |= ((uint32_t)((msg->tx_len >> 8) & 0xff) << DSI_CR_LPDT_WORD1_POS) & DSI_CR_LPDT_WORD1_MSK;
    reg_val &= ~DSI_CR_LPDT_PDLEN_MSK;
    reg_val |= ((uint32_t)msg->tx_len << DSI_CR_LPDT_PDLEN_POS) & DSI_CR_LPDT_PDLEN_MSK;
    putreg32(reg_val, reg_base + DSI_LPDT_TX_CONFIG_OFFSET);

    return 0;
}

uint8_t bflb_dsi_get_tx_fifo_count(struct bflb_device_s *dev)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_FIFO_CONFIG_1_OFFSET);
    return (uint8_t)((reg_val & DSI_TX_FIFO_CNT_MSK) >> DSI_TX_FIFO_CNT_POS);
}

uint8_t bflb_dsi_get_rx_fifo_count(struct bflb_device_s *dev)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t reg_val = getreg32(reg_base + DSI_FIFO_CONFIG_1_OFFSET);
    return (uint8_t)((reg_val & DSI_RX_FIFO_CNT_MSK) >> DSI_RX_FIFO_CNT_POS);
}

/* FIFO access functions following std driver pattern */
uint8_t bflb_dsi_full_fill_fifo(struct bflb_device_s *dev, const uint8_t *data, uint16_t len)
{
    uint32_t reg_base = dsi_base(dev);
    const uint8_t *data_end = data + len;
    uint8_t filled = 0;

    /* Fill FIFO with data while space is available */
    while (data < data_end) {
        if (bflb_dsi_get_tx_fifo_count(dev) > 0) {
            uint32_t word = 0;
            uint32_t remaining = data_end - data;
            uint32_t to_write = remaining >= 4 ? 4 : remaining;

            /* Pack data into word */
            for (uint32_t i = 0; i < to_write; i++) {
                word |= ((uint32_t)data[i]) << (i * 8);
            }

            putreg32(word, reg_base + DSI_FIFO_WDATA_OFFSET);
            data += 4; /* Always advance by 4 as per std driver */
            filled += 4;
        } else {
            return filled; /* FIFO full */
        }
    }

    return filled;
}

int bflb_dsi_feed_fifo(struct bflb_device_s *dev, const uint8_t *data, uint16_t len)
{
    uint32_t reg_base = dsi_base(dev);
    const uint8_t *data_end = data + len;
    const uint8_t *data_start = data;
    uint32_t timeout = DSI_TX_TIMEOUT_COUNT;

    /* Feed data with timeout */
    while (data < data_end) {
        if (bflb_dsi_get_tx_fifo_count(dev) > 0) {
            uint32_t word = 0;
            uint32_t remaining = data_end - data;
            uint32_t to_write = remaining >= 4 ? 4 : remaining;

            /* Pack data into word */
            for (uint32_t i = 0; i < to_write; i++) {
                word |= ((uint32_t)data[i]) << (i * 8);
            }

            putreg32(word, reg_base + DSI_FIFO_WDATA_OFFSET);
            timeout = DSI_TX_TIMEOUT_COUNT;
            data += 4; /* Always advance by 4 as per std driver */
        } else {
            timeout--;
            if (timeout == 0) {
                /* Return bytes actually sent before timeout */
                return (int)(data - data_start);
            }
            dsi_delay_short();
        }
    }

    return (int)(data - data_start); /* Return total bytes sent */
}

int bflb_dsi_read_fifo(struct bflb_device_s *dev, uint8_t *data, uint16_t maxlen)
{
    uint32_t reg_base = dsi_base(dev);
    uint32_t len = 0;
    uint8_t *data_end = data + maxlen;

    /* Read data from RX FIFO */
    while (data < data_end && bflb_dsi_get_rx_fifo_count(dev) > 0) {
        uint32_t word = getreg32(reg_base + DSI_FIFO_RDATA_OFFSET);
        uint32_t remaining = data_end - data;
        uint32_t to_copy = remaining >= 4 ? 4 : remaining;

        for (uint32_t i = 0; i < to_copy; i++) {
            data[i] = (uint8_t)(word >> (i * 8));
        }
        data += to_copy;
        len += to_copy;
    }

    return len;
}

int dsi_write_fifo(struct bflb_device_s *dev, const uint8_t *buf, uint16_t len)
{
    uint32_t reg_base = dsi_base(dev);
    uint16_t i = 0;
    uint32_t w = 0;
    while (i < len) {
        if (bflb_dsi_get_tx_fifo_count(dev) == 0) {
            continue;
        }
        w = 0;
        w |= (i < len ? buf[i] : 0);
        i++;
        w |= (i < len ? ((uint32_t)buf[i] << 8) : 0);
        i++;
        w |= (i < len ? ((uint32_t)buf[i] << 16) : 0);
        i++;
        w |= (i < len ? ((uint32_t)buf[i] << 24) : 0);
        i++;
        putreg32(w, reg_base + DSI_FIFO_WDATA_OFFSET);
    }
    return 0;
}

int bflb_dsi_lpdt_send_long_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg)
{
    /* Following std driver DSI_LPDT_Send_Long_Packet logic */
    int ret;
    uint8_t filled = 0;

    /* Config long packet - call the API function */
    ret = bflb_dsi_lpdt_config_long_packet(dev, msg);
    if (ret != 0) {
        return ret;
    }

    /* Fill FIFO - call the API function */
    filled = bflb_dsi_full_fill_fifo(dev, msg->tx_buf, msg->tx_len);

    /* Start TX - call the API function */
    ret = bflb_dsi_lpdt_start_tx(dev);
    if (ret != 0) {
        return ret;
    }

    /* Continue feeding FIFO if needed - call the API function */
    while (filled < msg->tx_len) {
        int bytes_sent = bflb_dsi_feed_fifo(dev, msg->tx_buf + filled, msg->tx_len - filled);
        if (bytes_sent <= 0) {
            return -EIO; /* Feed failed or no progress */
        }
        filled += bytes_sent; /* Update filled count with actual bytes fed */
    }

    /* Wait for completion - call the API function */
    return bflb_dsi_wait_tx_done(dev);
}

int bflb_dsi_lpdt_recv(struct bflb_device_s *dev, bflb_dsi_lpdt_msg_t *msg)
{
    uint32_t state;
    uint32_t int_status;
    uint32_t timeout = DSI_TX_TIMEOUT_COUNT;

    /* wait for bus stop - following std driver DSI_LPDT_Recv_Packet logic */
    do {
        bflb_dsi_phy_get_lanes_state(dev, BFLB_DSI_LANE_DATA0, &state);
        if (--timeout == 0) {
            return -ETIMEDOUT;
        }
        dsi_delay_short();
    } while (state != BFLB_DSI_LANE_STATE_STOP);

    /* turn around - call the API function */
    bflb_dsi_phy_data_lane0_turnaround(dev);

    /* start rx - call the API function */
    bflb_dsi_lpdt_start_rx(dev);

    /* wait for rx finished - following std driver logic */
    timeout = DSI_TX_TIMEOUT_COUNT;
    do {
        int_status = bflb_dsi_int_get(dev);
        if (int_status & BFLB_DSI_INT_LPDT_RX_END) {
            bflb_dsi_int_clear(dev, BFLB_DSI_INT_LPDT_RX_END);
            break;
        }
        if (--timeout == 0) {
            return -ETIMEDOUT;
        }
        dsi_delay_short();
    } while (1);

    /* get RX length - call the API function */
    msg->rx_len = bflb_dsi_lpdt_get_rx_length(dev);

    /* read FIFO data - call the API function */
    if (msg->rx_buf) {
        bflb_dsi_read_fifo(dev, msg->rx_buf, msg->rx_len);
    }
    return 0;
}
