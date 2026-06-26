#include "mipi_dsi_v2.h"
#include "bl618dg_clock.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_osd.h"
#include "bflb_dpi.h"
#include "bflb_irq.h"
#include <stdio.h>
#include <string.h>

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "semphr.h"
#endif

/* ---------- DSI PLL configuration (BL618DG) ----------
 *
 * Frequency chain for this config (40 MHz XTAL):
 *   - bitclk_div         = 2
 *   - bit clock          = 400 MHz   (per-lane HS bit rate)
 *   - DDR clock lane     = bitclk / 2       = 200 MHz   (CLK_N / CLK_P frequency,
 *                                                        MIPI clock lane is DDR)
 *   - per-lane data rate = 400 Mbps         (= bit clock) */

const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll400MCfg_40M = {
    .refdiv_ratio = 4,
    .vco_speed = 2,
    .vco_idac_extra = 2,
    .tdc_dly_sel = 1,
    .dtc_r_sel = 2,
    .lf_alpha_base = 0,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 3,
    .lf_beta_base = 3,
    .lf_beta_exp = 1,
    .lf_beta_fast = 1,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 2, 
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_400M[GLB_XTAL_MAX] = {
    { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { &dsipll400MCfg_40M, 0x28000 },
    { NULL, 0x0 }, { NULL, 0x0 },
};

/* ---------- DSI PLL: 500 MHz HS bit clock (1000 MHz VCO) ----------
 *
 * Frequency chain for this config (40 MHz XTAL):
 *   - bitclk_div         = 2
 *   - bit clock          = 500 MHz   (per-lane HS bit rate)
 *   - DDR clock lane     = bitclk / 2       = 250 MHz
 *   - per-lane data rate = 500 Mbps         (= bit clock)
 *
 * Same loop-filter/IDAC trims as the 400M config; only vco_speed is bumped
 * (2 -> 3) for the higher 1 GHz VCO. sdmin = vco/PFD * 2^11:
 *   40 MHz XTAL: PFD = 40/refdiv(4) = 10 MHz -> 1000/10 * 2^11 = 0x32000 */
const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll500MCfg_40M = {
    .refdiv_ratio = 4,
    .vco_speed = 3,
    .vco_idac_extra = 2,
    .tdc_dly_sel = 1,
    .dtc_r_sel = 2,
    .lf_alpha_base = 0,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 3,
    .lf_beta_base = 3,
    .lf_beta_exp = 1,
    .lf_beta_fast = 1,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 2,
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_500M[GLB_XTAL_MAX] = {
    { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { &dsipll500MCfg_40M, 0x32000 },
    { NULL, 0x0 }, { NULL, 0x0 },
};

const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll850MCfg_40M = {
    .refdiv_ratio = 4,
    .vco_speed = 2,
    .vco_idac_extra = 2,
    .tdc_dly_sel = 1,
    .dtc_r_sel = 2,
    .lf_alpha_base = 0,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 3,
    .lf_beta_base = 3,
    .lf_beta_exp = 1,
    .lf_beta_fast = 1,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 1,
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_850M[GLB_XTAL_MAX] = {
    { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { &dsipll850MCfg_40M, 0x2A800 },
    { NULL, 0x0 }, { NULL, 0x0 },
};


/* ---------- DSI device handle (single DSI controller on BL618DG) ---------- */

/* One DSI controller drives one panel, so cache the handle (resolved in setup). */
static struct bflb_device_s *dsi_dev = NULL;

/* ---------- DSI bring-up: PLL + display clocks + controller + D-PHY ----------
 *
 * Power on the DSI PLL, enable ESC/display clocks, init the DSI controller and
 * D-PHY. Call once before the panel's register init; HS mode starts separately
 * after that init. */
void mipi_dsi_v2_setup(const mipi_dsi_v2_timing_t *cfg)
{
    dsi_dev = bflb_device_get_by_name("dsi");

    /* DSI PLL + ESC/display clocks (all panel-determined from cfg) */
    GLB_Power_Off_DSIPLL();
    GLB_DSIPLL_Ref_Clk_Sel(GLB_DSIPLL_REFCLK_XTAL_SOC);
    GLB_Power_On_DSIPLL(cfg->pll_cfg, 1);
    GLB_Set_DSI_ESC_CLK(1, cfg->esc_clk_sel, cfg->esc_clk_div);
    GLB_Set_Display_CLK(ENABLE, cfg->display_clk_sel, cfg->display_clk_div);

    /* DSI controller + D-PHY */
    bflb_dsi_config_t dsi_cfg = {
        .virtual_chan = 0,
        .lane_num = cfg->lane_num,
        .lane_order = cfg->lane_order,
        .data_type = cfg->data_type,
        .sync_type = BFLB_DSI_HS_SYNC_EVENT_MODE,
        .vsa = cfg->vsw,
        .vfp = cfg->vfp,
    };

    bflb_dsi_init(dsi_dev, &dsi_cfg);
    bflb_dsi_phy_reset(dsi_dev);
    bflb_dsi_phy_enable(dsi_dev);
    bflb_dsi_phy_config(dsi_dev, &cfg->dphy);

    /* data-lane enable mask (clock lane always included) */
    uint32_t lane_mask = BFLB_DSI_LANE_CLOCK;
    if (cfg->lane_num == BFLB_DSI_LANES_1) {
        lane_mask |= BFLB_DSI_LANE_DATA0;
    } else if (cfg->lane_num == BFLB_DSI_LANES_2) {
        lane_mask |= BFLB_DSI_LANE_DATA0 | BFLB_DSI_LANE_DATA1;
    } else { /* BFLB_DSI_LANES_4 */
        lane_mask |= BFLB_DSI_LANE_DATA0 | BFLB_DSI_LANE_DATA1 | BFLB_DSI_LANE_DATA2 | BFLB_DSI_LANE_DATA3;
    }
    bflb_dsi_phy_enable_lanes(dsi_dev, lane_mask);
}

/* ---------- DSI line buffer threshold + HS (video) mode ---------- */

void mipi_dsi_v2_hs_mode_start(const mipi_dsi_v2_timing_t *cfg)
{
    /* DPI pixel clock is derived from the display clock configured in setup()
     * (display_clk_sel / display_clk_div), so read it back instead of carrying
     * a hand-copied constant that can drift out of sync with the divider. */
    uint32_t dpi_pixel_clock = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_DISPLAY);//63000000 86000000
    bflb_dsi_set_line_buffer_threshold(dsi_dev, cfg->width, dpi_pixel_clock, cfg->dsi_hs_clock,
                                       cfg->data_type, cfg->lane_num);//dpi_pixel_clock
    // bflb_dsi_set_line_buffer_threshold(dsi_dev, cfg->width, 63000000, cfg->dsi_hs_clock,
    //                                    cfg->data_type, cfg->lane_num);//dpi_pixel_clock
    bflb_dsi_phy_hs_mode_start(dsi_dev);
}

/* ---------- DSI HS mode stop + controller/PLL teardown ---------- */

int mipi_dsi_v2_hs_mode_stop(void)
{
    return bflb_dsi_phy_hs_mode_stop(dsi_dev);
}

void mipi_dsi_v2_deinit(void)
{
    bflb_dsi_phy_hs_mode_stop(dsi_dev);
    bflb_dsi_deinit(dsi_dev);
    GLB_Power_Off_DSIPLL();
}

/* ---------- DCS / generic write ---------- */

int mipi_dsi_v2_dcs_write_cmd(uint8_t data_type, uint8_t cmd, const uint8_t *data, uint16_t len)
{
    struct bflb_device_s *dsi = dsi_dev;
    uint8_t buf[1 + DSI_V2_DCS_WRITE_MAX_LEN];

    if (len > 0 && data == NULL) {
        return -1;
    }
    if (len > DSI_V2_DCS_WRITE_MAX_LEN) {
        return -2;
    }

    /* data_type == 0: auto-pick the packet type from payload length (0 -> short,
     * 1 -> short param, >1 -> long). Nonzero: caller forces a type (e.g. generic
     * long write 0x29, which some vendor panels require even for short payloads). */
    if (data_type == 0) {
        data_type = (len == 0) ? DSI_V2_DCS_SHORT_WRITE :
                    (len == 1) ? DSI_V2_DCS_SHORT_WRITE_PARAM :
                                 DSI_V2_DCS_LONG_WRITE;
    }

    buf[0] = cmd;
    if (len > 0) {
        memcpy(&buf[1], data, len);
    }

    bflb_dsi_lpdt_msg_t msg = {
        .virtual_chan = 0,
        .data_type = data_type,
        .tx_len = 1 + len,
        .tx_buf = buf,
    };

    if (data_type == DSI_V2_DCS_SHORT_WRITE || data_type == DSI_V2_DCS_SHORT_WRITE_PARAM) {
        bflb_dsi_lpdt_send_short_packet(dsi, &msg);
    } else {
        bflb_dsi_lpdt_send_long_packet(dsi, &msg);
    }
    return 0;
}

/* ---------- DCS read ---------- */

int mipi_dsi_v2_dcs_read(uint8_t cmd, uint8_t *buf, uint16_t len)
{
    struct bflb_device_s *dsi = dsi_dev;
    uint8_t tx_buf[2];
    int ret;

    volatile uint32_t *phy_base = (volatile uint32_t *)DSI_PLL_BASE;
    volatile uint32_t *dsi_ctrl = (volatile uint32_t *)DSI_V2_CTRL_BASE;

    /* Ensure PHY analog RX is fully enabled for LPDT read */
    phy_base[0x38 / 4] |= (1U << 4) | (1U << 8) | (1U << 20); /* BG_VREF + ANA_VREF */
    phy_base[0x3C / 4] |=
        0xF | (0xF << 4) | (0xF << 8) | (0xF << 12) | (1U << 20) | (1U << 24); /* HSTXEN + byte ctrl + ESC clk */
    phy_base[0x44 / 4] |= (0xF << 12) | (0xF << 16) | (1U << 24);              /* LPTXEN + LPRXEN + CLK LP TX */
    phy_base[0x2C / 4] |= (0xF << 4) | (1U << 20);                             /* LPRX_BW_SEL + VREG400_EN_V_VREF */

    /* Clear FORCERXMODE + TURNESC from any previous attempt */
    phy_base[0x6C / 4] &= ~((1U << 16) | (1U << 24));
    /* Ensure D0 LPTXEN is set for TX phase */
    phy_base[0x44 / 4] |= (1U << 12);
    /* Clear ESC_RX_EN and ESC_TX_EN */
    dsi_ctrl[0x04 / 4] &= ~((1U << 16) | (1U << 0));

    /* Clear RX FIFO and all interrupts */
    dsi_ctrl[0x60 / 4] |= (1U << 3);
    dsi_ctrl[0x60 / 4] &= ~(1U << 3);
    dsi_ctrl[0x18 / 4] = 0x3FFF;

    uint8_t max_ret[2] = { len & 0xff, (len >> 8) & 0xff };
    bflb_dsi_lpdt_msg_t set_max_msg = {
        .virtual_chan = 0,
        .data_type = DSI_V2_SET_MAX_RET_PKT_SIZE,
        .tx_len = 2,
        .tx_buf = max_ret,
    };
    ret = bflb_dsi_lpdt_send_short_packet(dsi, &set_max_msg);
    if (ret) {
        printf("  set_max_ret failed: %d\r\n", ret);
        return ret;
    }

    tx_buf[0] = cmd;
    tx_buf[1] = 0;
    bflb_dsi_lpdt_msg_t read_msg = {
        .virtual_chan = 0,
        .data_type = DSI_V2_DCS_READ,
        .tx_len = 1,
        .tx_buf = tx_buf,
        .rx_buf = buf,
        .rx_len = len,
    };

    ret = bflb_dsi_lpdt_send_short_packet(dsi, &read_msg);
    if (ret) {
        printf("  send read cmd failed: %d\r\n", ret);
        return ret;
    }

    ret = bflb_dsi_lpdt_recv(dsi, &read_msg);
    if (ret) {
        printf("  recv failed: %d, CFG14=0x%08x RB0=0x%08x\r\n", ret, phy_base[0x6C / 4], phy_base[0x70 / 4]);
        phy_base[0x6C / 4] &= ~((1U << 16) | (1U << 24));
        phy_base[0x44 / 4] |= (1U << 12);
        dsi_ctrl[0x04 / 4] &= ~((1U << 16) | (1U << 0));
        return ret;
    }

    printf("  RX ok: len=%u, data:", read_msg.rx_len);
    for (uint16_t i = 0; i < read_msg.rx_len && i < 16; i++) {
        printf(" %02x", buf[i]);
    }
    printf("\r\n");
    return 0;
}

/* ---------- Frame-buffer switch (OSD0 blend + SEOF interrupt, shared by all v2 panels) ----------
 *
 * The LVGL canvas is the OSD0 blend overlay on top of the DPI background. Switching
 * the screen re-points OSD0 at a new canvas; the swap latches at the next frame
 * boundary (SEOF). The OSD interrupt fires every SEOF: the ISR invokes the SWAP
 * callback so the LVGL port (lv_port_disp_rgb.c) knows the swap completed.
 * OSD0 (not OSD1) is used so OSD1 stays free for another overlay. */

static struct bflb_device_s *dsi_v2_osd = NULL;
static struct bflb_device_s *dsi_v2_dpi = NULL;
static void *dsi_v2_screen_using = NULL;
static void (*dsi_v2_swap_callback)(void) = NULL;
static void (*dsi_v2_cycle_callback)(void) = NULL;

/* Pending video (DPI background) frame, latched at the next SEOF (between frames,
 * so the background never tears mid-scanout). Owned by the app's dpi_manager
 * (defined there, extern here); it holds the *display* address (the panel-sized
 * planar buffer the DPI scans out) and raises dpi_show_pending when the frame is
 * ready. The ISR latches it and clears the flag. */
extern volatile uint32_t dpi_show_bufaddr0;  /* Y plane to scan out */
extern volatile uint32_t dpi_show_bufaddr1;  /* UV plane to scan out */
extern volatile uint8_t dpi_show_pending;
/* Triple-buffer in-flight tracking (defined in the app's dpi_manager, extern here).
 * The swap latches one frame late, so two buffers are hardware-owned at once: [0]
 * scanning now, [1] latched to scan next. The SEOF ISR rotates this set each latch
 * so the decode task can tell which Y buffers it must not overwrite. */
extern volatile uint32_t dpi_busy_y[2];

static void mipi_dsi_v2_osd_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;

    bflb_osd_int_clear(dsi_v2_osd);

    /* SEOF crossed: latch the pending video frame here so the DPI background
     * switches between frames, not mid-scanout. */
    if (dpi_show_pending && dsi_v2_dpi != NULL) {
        bflb_dpi_framebuffer_planar_switch(dsi_v2_dpi, dpi_show_bufaddr0, dpi_show_bufaddr1);
        /* Rotate the in-flight set: what was "next" ([1]) is now scanning ([0]),
         * and the just-latched buffer becomes "next". The decode task reads this to
         * stay off buffers the scanner still owns. */
        dpi_busy_y[0] = dpi_busy_y[1];
        dpi_busy_y[1] = dpi_show_bufaddr0;
        dpi_show_pending = 0;
    }

    if (dsi_v2_cycle_callback != NULL) {
        dsi_v2_cycle_callback();
    }
    if (dsi_v2_swap_callback != NULL) {
        dsi_v2_swap_callback();
    }
}

/* Attach + enable the OSD SEOF interrupt. Call once after the OSD blend layer is
 * up (the dpi_manager owns that). */
void mipi_dsi_v2_osd_irq_init(struct bflb_device_s *osd)
{
    dsi_v2_osd = osd;
    bflb_osd_int_clear(osd);
    bflb_osd_int_mask(osd, false);
    bflb_irq_attach(osd->irq_num, mipi_dsi_v2_osd_isr, NULL);
    bflb_irq_enable(osd->irq_num);
}

/* Bring up the scan-out side (the panel link itself was already up via
 * mipi_dsi_v2_setup()/hs_mode_start() in the panel's _dsi_init()):
 *   [1] DPI background scan-out layer (YUV planar, FRAMEBUFFER_WITH_OSD)
 *   [2] OSD0 ARGB8888 blend overlay (the LVGL canvas) -- must follow [1]
 *   [3] OSD SEOF interrupt that drives the frame callbacks
 * osd_buf is the initial OSD canvas. The DPI background framebuffer is left at 0;
 * the video pipeline points it at real YUV frames at runtime. */
int mipi_dsi_v2_display_init(const mipi_dsi_v2_timing_t *cfg, uint32_t osd_buf)
{
    struct bflb_device_s *dpi = bflb_device_get_by_name("dpi");
    struct bflb_device_s *osd = bflb_device_get_by_name("osd0");

    if (dpi == NULL || osd == NULL) {
        return -1;
    }
    dsi_v2_dpi = dpi;

    /* [1] DPI background scan-out layer. framebuffer_addr=0: the video pipeline
     * switches in real YUV frames at runtime. */
    struct bflb_dpi_config_s dpi_config = {
        .width = cfg->width,
        .height = cfg->height,
        .hsw = cfg->hsw,
        .hbp = cfg->hbp,
        .hfp = cfg->hfp,
        .vsw = cfg->vsw,
        .vbp = cfg->vbp,
        .vfp = cfg->vfp,
        .interface = DPI_INTERFACE_24_PIN,
        .input_sel = DPI_INPUT_SEL_FRAMEBUFFER_WITH_OSD,
        // .input_sel = DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD, // completly shut down the layer 
        .test_pattern = DPI_TEST_PATTERN_NULL,
        .data_format = DPI_DATA_FORMAT_Y_UV_PLANAR,
        .framebuffer_addr = 0,
        .uv_framebuffer_addr = 0,
    };
    bflb_dpi_init(dpi, &dpi_config);

    bflb_dpi_feature_control(dpi, DPI_CMD_SET_BURST, DPI_BURST_INCR8);

    /* [2] OSD0 full-screen ARGB8888 overlay (the LVGL canvas) */
    struct bflb_osd_blend_config_s osd_blend_config = {
        .blend_format = OSD_BLEND_FORMAT_ARGB8888,
        .order_a = 3,
        .order_rv = 2,
        .order_gy = 1,
        .order_bu = 0,
        .coor = {
            .start_x = 0,
            .start_y = 0,
            .end_x = cfg->width,
            .end_y = cfg->height,
        },
        .layer_buffer_addr = osd_buf,
    };
    bflb_osd_blend_init(osd, &osd_blend_config);

    (*(volatile uint32_t *)0x2004A4FC) &= ~(0x7 << 12);
    (*(volatile uint32_t *)0x2004A4FC) |= (0x4 << 12);

    bflb_osd_blend_enable(osd);

    /* [3] OSD SEOF interrupt -> frame callbacks (drives screen_switch reporting) */
    mipi_dsi_v2_osd_irq_init(osd);

    dsi_v2_screen_using = (void *)osd_buf;
    return 0;
}

int mipi_dsi_v2_screen_switch(void *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }
    if (dsi_v2_osd == NULL) {
        return -2;
    }

    /* Re-point the OSD blend layer at the new canvas. Non-blocking: the swap
     * latches at the next SEOF; the SWAP callback tells the caller it happened. */
    bflb_osd_blend_set_layer_buffer(dsi_v2_osd, (uint32_t)screen_buffer);
    // bflb_osd_blend_set_layer_buffer(dsi_v2_osd, 0); // shut down the osd
    dsi_v2_screen_using = screen_buffer;

    return 0;
}

void *mipi_dsi_v2_get_screen_using(void)
{
    return dsi_v2_screen_using;
}

int mipi_dsi_v2_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == MIPI_DSI_V2_FRAME_INT_TYPE_SWAP) {
        dsi_v2_swap_callback = callback;
    } else if (callback_type == MIPI_DSI_V2_FRAME_INT_TYPE_CYCLE) {
        dsi_v2_cycle_callback = callback;
    }
    return 0;
}
