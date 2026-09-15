#include "mipi_dsi_v2.h"
#include "bl618dg_clock.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_osd.h"
#include "bflb_dpi.h"
#include "bflb_irq.h"
#include "bflb_l1c.h"
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
    .lf_alpha_base = 1,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 1,
    .lf_beta_base = 0,
    .lf_beta_exp = 3,
    .lf_beta_fast = 0,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 2,
    .resv0_1_0 = 1,
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
    .lf_alpha_base = 1,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 1,
    .lf_beta_base = 0,
    .lf_beta_exp = 3,
    .lf_beta_fast = 0,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 2,
    .resv0_1_0 = 1,
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_500M[GLB_XTAL_MAX] = {
    { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { &dsipll500MCfg_40M, 0x32000 },
    { NULL, 0x0 }, { NULL, 0x0 },
};

/* 550M */
const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll550MCfg_40M = {
    .refdiv_ratio = 4,
    .vco_speed = 4,
    .vco_idac_extra = 2,
    .tdc_dly_sel = 1,
    .dtc_r_sel = 2,
    .lf_alpha_base = 1,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 1,
    .lf_beta_base = 0,
    .lf_beta_exp = 3,
    .lf_beta_fast = 0,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 2,
    .resv0_1_0 = 1,
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_550M[GLB_XTAL_MAX] = {
    { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { &dsipll550MCfg_40M, 0x37000 },
    { NULL, 0x0 }, { NULL, 0x0 },
};

/* 650M */
const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll650MCfg_40M = {
    .refdiv_ratio = 2,
    .vco_speed = 6,
    .vco_idac_extra = 2,
    .tdc_dly_sel = 0,
    .dtc_r_sel = 0,
    .lf_alpha_base = 1,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 1,
    .lf_beta_base = 0,
    .lf_beta_exp = 3,
    .lf_beta_fast = 0,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 2,
    .resv0_1_0 = 1,
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_650M[GLB_XTAL_MAX] = {
    { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { &dsipll650MCfg_40M, 0x20800 },
    { NULL, 0x0 }, { NULL, 0x0 },
};

/* 750M */
const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll750MCfg_40M = {
    .refdiv_ratio = 2,
    .vco_speed = 7,
    .vco_idac_extra = 2,
    .tdc_dly_sel = 0,
    .dtc_r_sel = 0,
    .lf_alpha_base = 1,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 1,
    .lf_beta_base = 0,
    .lf_beta_exp = 3,
    .lf_beta_fast = 0,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 2,
    .resv0_1_0 = 1,
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_750M[GLB_XTAL_MAX] = {
    { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { NULL, 0x0 }, { &dsipll750MCfg_40M, 0x25800 },
    { NULL, 0x0 }, { NULL, 0x0 },
};

/* 850M */
const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll850MCfg_40M = {
    .refdiv_ratio = 4,
    .vco_speed = 2,
    .vco_idac_extra = 2,
    .tdc_dly_sel = 1,
    .dtc_r_sel = 2,
    .lf_alpha_base = 1,
    .lf_alpha_exp = 2,
    .lf_alpha_fast = 1,
    .lf_beta_base = 0,
    .lf_beta_exp = 3,
    .lf_beta_fast = 0,
    .spd_gain = 2,
    .lms_ext_en = 0,
    .lms_ext_value = 32,
    .bitclk_div = 1,
    .resv0_1_0 = 1,
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
    uint32_t dpi_pixel_clock = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_DISPLAY);

    /* bflb_dsi_set_line_buffer_threshold() requires frame_width to be a multiple of 4;
     * round up cfg->width to meet that constraint (DPI/DSI timing still uses actual width). */
    uint32_t lb_width = (cfg->width + 3) & ~3u;

    if(dpi_pixel_clock>=60000000){
        bflb_dsi_set_line_buffer_threshold(dsi_dev, lb_width, dpi_pixel_clock+4000000, cfg->dsi_hs_clock,
                                       cfg->data_type, cfg->lane_num);
    }
    else{
        bflb_dsi_set_line_buffer_threshold(dsi_dev, lb_width, dpi_pixel_clock, cfg->dsi_hs_clock,
                                       cfg->data_type, cfg->lane_num);
    }
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

/* ---------- Frame-buffer switch (OSD0 blend + SEOF interrupt, shared by all v2 panels) ----------
 *
 * The LVGL canvas is the OSD0 blend overlay on top of the DPI background. Switching
 * the screen re-points OSD0 at a new canvas; the swap latches at the next frame
 * boundary (SEOF). The OSD interrupt fires every SEOF: the ISR invokes the SWAP
 * callback so the LVGL port (lv_port_disp_rgb.c) knows the swap completed.
 * OSD0 (not OSD1) is used so OSD1 stays free for another overlay. */

static struct bflb_device_s *dsi_v2_osd = NULL;
static void *dsi_v2_screen_using = NULL;
static void (*dsi_v2_swap_callback)(void) = NULL;
static void (*dsi_v2_cycle_callback)(void) = NULL;

/* For D-cache */
static uint32_t dsi_v2_osd_buf_size = 0;

/**
 * @brief Base (DPI background) layer swap, invoked from the OSD SEOF ISR.
 *
 * Weak no-op by default: an OSD0-only LVGL app needs nothing here (the base layer
 * just scans black). An app that drives a video background overrides this (see the
 * video pipeline in dpi_manager.c) to latch the next decoded YUV frame into the DPI
 * base layer at the frame boundary -- so the background switches between frames,
 * never mid-scanout. Keeping it weak means the driver no longer reaches into the
 * app's video state (the old dpi_show_* / dpi_busy_y externs are gone). */
__attribute__((weak)) void mipi_dsi_v2_osd0_base_layer_swap(void)
{
}

static void mipi_dsi_v2_osd0_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;

    bflb_osd_int_clear(dsi_v2_osd);

    /* SEOF crossed: swap the base (DPI background) layer -- weak no-op unless an
     * app overrides it to latch the next video frame between scanouts. */
    mipi_dsi_v2_osd0_base_layer_swap();

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
    bflb_irq_attach(osd->irq_num, mipi_dsi_v2_osd0_isr, NULL);
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

    /* [1] DPI background scan-out layer. framebuffer_addr=0: the video pipeline
     * switches in real YUV frames at runtime (via the app's base-layer swap). */
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

    bflb_osd_blend_enable(osd);

    /* [3] OSD SEOF interrupt -> frame callbacks (drives screen_switch reporting) */
    mipi_dsi_v2_osd_irq_init(osd);

    /* Canvas byte size for the cache write-back in screen_switch() (ARGB8888). */
    dsi_v2_osd_buf_size = (uint32_t)cfg->width * cfg->height * 4U;

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

    /* Need to clean D-cache. */
    if (dsi_v2_osd_buf_size != 0U) {
        bflb_l1c_dcache_clean_range(screen_buffer, dsi_v2_osd_buf_size);
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

/* ---------- RGB565 base framebuffer + OSD1 SEOF path ----------
 *
 * The camera preview uses DPI's RGB565 base framebuffer directly. OSD1 stays
 * transparent and exists only to generate a frame-boundary interrupt. Keep all
 * state separate from the OSD0/LVGL path above so existing DSI applications
 * cannot affect this mode. */

static struct bflb_device_s *dsi_v2_rgb565_dpi = NULL;
static struct bflb_device_s *dsi_v2_rgb565_osd = NULL;
static void *volatile dsi_v2_rgb565_screen_using = NULL;
static void *volatile dsi_v2_rgb565_screen_pending = NULL;
static uint32_t dsi_v2_rgb565_buf_size = 0;
static void (*dsi_v2_rgb565_swap_callback)(void) = NULL;
static void (*dsi_v2_rgb565_cycle_callback)(void) = NULL;

static void mipi_dsi_v2_rgb565_osd1_isr(int irq, void *arg)
{
    bool swapped = false;

    (void)irq;
    (void)arg;

    bflb_osd_int_clear(dsi_v2_rgb565_osd);

    if (dsi_v2_rgb565_screen_using != dsi_v2_rgb565_screen_pending) {
        bflb_dpi_framebuffer_switch(
            dsi_v2_rgb565_dpi, (uint32_t)(uintptr_t)dsi_v2_rgb565_screen_pending);
        dsi_v2_rgb565_screen_using = dsi_v2_rgb565_screen_pending;
        swapped = true;
    }

    if (dsi_v2_rgb565_cycle_callback != NULL) {
        dsi_v2_rgb565_cycle_callback();
    }
    if (swapped && dsi_v2_rgb565_swap_callback != NULL) {
        dsi_v2_rgb565_swap_callback();
    }
}

int mipi_dsi_v2_rgb565_display_init(const mipi_dsi_v2_timing_t *cfg, uint32_t framebuffer_addr,
                                    uint32_t osd_sync_buf)
{
    struct bflb_dpi_config_s dpi_config;
    struct bflb_osd_blend_config_s osd_blend_config;

    if (cfg == NULL || framebuffer_addr == 0 || osd_sync_buf == 0) {
        return -2;
    }

    dsi_v2_rgb565_dpi = bflb_device_get_by_name("dpi");
    dsi_v2_rgb565_osd = bflb_device_get_by_name("osd1");
    if (dsi_v2_rgb565_dpi == NULL || dsi_v2_rgb565_osd == NULL) {
        return -1;
    }

    dpi_config = (struct bflb_dpi_config_s){
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
        .test_pattern = DPI_TEST_PATTERN_NULL,
        .data_format = DPI_DATA_FORMAT_RGB565,
        .framebuffer_addr = framebuffer_addr,
        .uv_framebuffer_addr = 0,
    };
    bflb_dpi_init(dsi_v2_rgb565_dpi, &dpi_config);
    bflb_dpi_feature_control(dsi_v2_rgb565_dpi, DPI_CMD_SET_BURST, DPI_BURST_INCR8);

    osd_blend_config = (struct bflb_osd_blend_config_s){
        .blend_format = OSD_BLEND_FORMAT_ARGB8888,
        .order_a = 3,
        .order_rv = 2,
        .order_gy = 1,
        .order_bu = 0,
        .coor = {
            .start_x = 0,
            .start_y = 0,
            .end_x = 1,
            .end_y = 1,
        },
        .layer_buffer_addr = osd_sync_buf,
    };
    bflb_osd_blend_init(dsi_v2_rgb565_osd, &osd_blend_config);
    bflb_osd_blend_set_global_a(dsi_v2_rgb565_osd, true, 0);
    bflb_osd_blend_enable(dsi_v2_rgb565_osd);

    bflb_osd_int_clear(dsi_v2_rgb565_osd);
    bflb_irq_attach(dsi_v2_rgb565_osd->irq_num, mipi_dsi_v2_rgb565_osd1_isr, NULL);
    bflb_osd_int_mask(dsi_v2_rgb565_osd, false);
    bflb_irq_enable(dsi_v2_rgb565_osd->irq_num);

    dsi_v2_rgb565_screen_using = (void *)(uintptr_t)framebuffer_addr;
    dsi_v2_rgb565_screen_pending = (void *)(uintptr_t)framebuffer_addr;
    dsi_v2_rgb565_buf_size = cfg->width * cfg->height * sizeof(uint16_t);
    return 0;
}

int mipi_dsi_v2_rgb565_screen_switch(void *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }
    if (dsi_v2_rgb565_dpi == NULL || dsi_v2_rgb565_osd == NULL) {
        return -2;
    }

    /* The RGB565 framebuffer is half the size of the ARGB8888 OSD canvas. */
    if (dsi_v2_rgb565_buf_size != 0U) {
        bflb_l1c_dcache_clean_range(screen_buffer, dsi_v2_rgb565_buf_size);
    }

    dsi_v2_rgb565_screen_pending = screen_buffer;
    return 0;
}

void *mipi_dsi_v2_rgb565_get_screen_using(void)
{
    return dsi_v2_rgb565_screen_using;
}

int mipi_dsi_v2_rgb565_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == MIPI_DSI_V2_FRAME_INT_TYPE_SWAP) {
        dsi_v2_rgb565_swap_callback = callback;
    } else if (callback_type == MIPI_DSI_V2_FRAME_INT_TYPE_CYCLE) {
        dsi_v2_rgb565_cycle_callback = callback;
    }
    return 0;
}
