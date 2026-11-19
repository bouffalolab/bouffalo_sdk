/**
 * @file display_dsi_bl618dg.c
 * @brief DSI host hardware access for BL618DG: DSI PLL, clocks, controller and D-PHY through the SDK drivers.
 *
 * Bring-up order: DSI PLL, escape clock and display clock, then controller and D-PHY. The panel is initialized
 * over LP packets after that; the HS video starts when the scanout asks for it.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bus/dsi/display_dsi_hw.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_clock.h"
#include "bflb_core.h"
#include "bflb_dsi.h"
#include "bl618dg_clock.h"

#define DSI_XTAL_HZ         40000000u
#define DSI_PCLK_BOOST_FROM 60000000u /**< From this pixel clock on the line buffer is sized with 4 MHz more */
#define DSI_PCLK_BOOST      4000000u

/*
 * DSI PLL of a 40 MHz crystal, one entry per step of display_bl618dg_dpu_dsipll_step(). The bit clock is the VCO
 * divided by bitclk_div; the loop filter settings are the same for all steps. sdmin = VCO / PFD * 2^11 with
 * PFD = 40 MHz / refdiv_ratio.
 */
#define DSIPLL_BASIC(_refdiv, _vco_speed, _tdc, _dtc, _bitclk_div) \
    {                                                              \
        .refdiv_ratio = (_refdiv),                                 \
        .vco_speed = (_vco_speed),                                 \
        .vco_idac_extra = 2,                                       \
        .tdc_dly_sel = (_tdc),                                     \
        .dtc_r_sel = (_dtc),                                       \
        .lf_alpha_base = 1,                                        \
        .lf_alpha_exp = 2,                                         \
        .lf_alpha_fast = 1,                                        \
        .lf_beta_base = 0,                                         \
        .lf_beta_exp = 3,                                          \
        .lf_beta_fast = 0,                                         \
        .spd_gain = 2,                                             \
        .lms_ext_en = 0,                                           \
        .lms_ext_value = 32,                                       \
        .bitclk_div = (_bitclk_div),                               \
        .resv0_1_0 = 1,                                            \
    }

static const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION pll_basic[DISPLAY_BL618DG_DPU_DSIPLL_STEPS] = {
    DSIPLL_BASIC(4, 2, 1, 2, 2), /* 400 Mbit/s */
    DSIPLL_BASIC(4, 3, 1, 2, 2), /* 500 */
    DSIPLL_BASIC(4, 4, 1, 2, 2), /* 550 */
    DSIPLL_BASIC(2, 6, 0, 0, 2), /* 650 */
    DSIPLL_BASIC(2, 7, 0, 0, 2), /* 750 */
    DSIPLL_BASIC(4, 2, 1, 2, 1), /* 850 */
};

static const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION pll_cfg[DISPLAY_BL618DG_DPU_DSIPLL_STEPS] = {
    { &pll_basic[0], 0x28000 }, { &pll_basic[1], 0x32000 }, { &pll_basic[2], 0x37000 },
    { &pll_basic[3], 0x20800 }, { &pll_basic[4], 0x25800 }, { &pll_basic[5], 0x2A800 },
};

/* D-PHY timing shared by the verified panels (400 to 850 Mbit/s) */
static const bflb_dsi_dphy_config_t dphy_default = {
    .time_clk_exit = 5,
    .time_clk_trail = 3,
    .time_clk_zero = 0xf,
    .time_data_exit = 5,
    .time_data_prepare = 1,
    .time_data_trail = 3,
    .time_data_zero = 6,
    .time_lpx = 3,
    .time_req_ready = 0,
    .time_ta_get = 0x13,
    .time_ta_go = 0xf,
    .time_wakeup = 0x9c41,
};

/** @brief DSI data type of the controller for a link format. */
static uint8_t dsi_data_type(display_dsi_format_t format)
{
    switch (format) {
        case DISPLAY_DSI_FMT_RGB565:
            return BFLB_DSI_DATA_RGB565;
        case DISPLAY_DSI_FMT_RGB666:
            return BFLB_DSI_DATA_RGB666;
        default:
            return BFLB_DSI_DATA_RGB888;
    }
}

/** @brief Lane count code of the controller. */
static uint8_t dsi_lane_num(uint8_t lanes)
{
    return lanes == 1 ? BFLB_DSI_LANES_1 : (lanes == 2 ? BFLB_DSI_LANES_2 : BFLB_DSI_LANES_4);
}

int display_dsi_hw_init(display_dsi_host_t *host)
{
    host->dsi = bflb_device_get_by_name(host->cfg.dev ? host->cfg.dev : "dsi");
    return host->dsi ? 0 : -ENODEV;
}

int display_dsi_hw_open(display_dsi_host_t *host, const display_video_mode_t *video, uint8_t vc)
{
    const display_dsi_link_t *link = &video->dsi;
    display_bl618dg_dpu_clk_t clk;
    int step = display_bl618dg_dpu_dsipll_step(link->lane_mbps);
    uint32_t lane_mask = BFLB_DSI_LANE_CLOCK;

    if (step < 0 || Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XTAL) != DSI_XTAL_HZ) {
        return -ENOTSUP; /* the PLL steps are for a 40 MHz crystal */
    }
    if (display_bl618dg_dpu_clock_select(Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK), video->clock_hz, &clk)) {
        return -ENOTSUP;
    }

    GLB_Power_Off_DSIPLL();
    GLB_DSIPLL_Ref_Clk_Sel(GLB_DSIPLL_REFCLK_XTAL_SOC);
    GLB_Power_On_DSIPLL(&pll_cfg[step], 1);
    GLB_Set_DSI_ESC_CLK(1, 0, 0);
    GLB_Set_Display_CLK(ENABLE, (uint8_t)clk.src, clk.div);

    bflb_dsi_config_t cfg = {
        .virtual_chan = vc,
        .lane_num = dsi_lane_num(link->lanes),
        .lane_order = (uint8_t)host->cfg.lane_order,
        .data_type = dsi_data_type(link->format),
        .sync_type = BFLB_DSI_HS_SYNC_EVENT_MODE,
        .vsa = (uint8_t)video->vsw,
        .vfp = (uint8_t)video->vfp,
    };

    bflb_dsi_init(host->dsi, &cfg);
    bflb_dsi_phy_reset(host->dsi);
    bflb_dsi_phy_enable(host->dsi);
    if (link->phy) {
        const bflb_dsi_dphy_config_t phy = {
            .time_clk_zero = link->phy->clk_zero,
            .time_clk_trail = link->phy->clk_trail,
            .time_clk_exit = link->phy->clk_exit,
            .time_data_zero = link->phy->data_zero,
            .time_data_trail = link->phy->data_trail,
            .time_data_exit = link->phy->data_exit,
            .time_data_prepare = link->phy->data_prepare,
            .time_ta_go = link->phy->ta_go,
            .time_ta_get = link->phy->ta_get,
            .time_req_ready = link->phy->req_ready,
            .time_lpx = link->phy->lpx,
            .time_wakeup = link->phy->wakeup,
        };

        bflb_dsi_phy_config(host->dsi, &phy);
    } else {
        bflb_dsi_phy_config(host->dsi, &dphy_default);
    }

    /* The clock lane is always on; data lanes are enabled from lane 0 up */
    for (uint8_t i = 0; i < link->lanes; i++) {
        lane_mask |= BFLB_DSI_LANE_DATA0 << i;
    }
    bflb_dsi_phy_enable_lanes(host->dsi, lane_mask);
    return 0;
}

void display_dsi_hw_close(display_dsi_host_t *host)
{
    bflb_dsi_phy_hs_mode_stop(host->dsi);
    bflb_dsi_deinit(host->dsi);
    GLB_Power_Off_DSIPLL();
}

int display_dsi_hw_send(display_dsi_host_t *host, uint8_t type, bool is_long, uint8_t vc, const uint8_t *payload,
                        uint16_t len)
{
    const bflb_dsi_lpdt_msg_t msg = {
        .virtual_chan = vc,
        .data_type = type,
        .tx_len = len,
        .tx_buf = payload,
    };

    return is_long ? bflb_dsi_lpdt_send_long_packet(host->dsi, &msg) : bflb_dsi_lpdt_send_short_packet(host->dsi, &msg);
}

void display_dsi_hw_hs_start(display_dsi_host_t *host, const display_video_mode_t *video)
{
    /* The pixel clock is read back from the display clock that was set up when the link was opened */
    uint32_t pixel_clock = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_DISPLAY);
    /* The line buffer calculation needs the width as a multiple of 4; the timing uses the real width */
    uint32_t lb_width = ((uint32_t)video->width + 3u) & ~3u;

    if (pixel_clock >= DSI_PCLK_BOOST_FROM) {
        pixel_clock += DSI_PCLK_BOOST;
    }
    bflb_dsi_set_line_buffer_threshold(host->dsi, lb_width, pixel_clock, (uint32_t)video->dsi.lane_mbps * 1000000u,
                                       dsi_data_type(video->dsi.format), dsi_lane_num(video->dsi.lanes));
    bflb_dsi_phy_hs_mode_start(host->dsi);
}

int display_dsi_hw_hs_stop(display_dsi_host_t *host)
{
    return bflb_dsi_phy_hs_mode_stop(host->dsi);
}
