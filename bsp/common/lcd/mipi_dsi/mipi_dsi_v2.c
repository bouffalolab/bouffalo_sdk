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

/* ---------- Unified DPI base + OSD0 scan-out path ---------- */

static struct bflb_device_s *dsi_v2_dpi = NULL;
static struct bflb_device_s *dsi_v2_osd = NULL;
static void *volatile dsi_v2_screen_using = NULL;
static void *volatile dsi_v2_screen_pending = NULL;
static void (*dsi_v2_swap_callback)(void) = NULL;
static void (*dsi_v2_cycle_callback)(void) = NULL;
static uint8_t dsi_v2_base_format = DPI_DATA_FORMAT_Y_UV_PLANAR;
static uint32_t dsi_v2_buf_size = 0;
static bool dsi_v2_screen_is_base = false;
static uint32_t dsi_v2_sync_pixel __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));

__attribute__((weak)) void mipi_dsi_v2_osd0_base_layer_swap(void)
{
}

static void mipi_dsi_v2_osd0_isr(int irq, void *arg)
{
    bool swapped = false;

    (void)irq;
    (void)arg;

    bflb_osd_int_clear(dsi_v2_osd);

    if (dsi_v2_screen_using != dsi_v2_screen_pending) {
        dsi_v2_screen_using = dsi_v2_screen_pending;
        swapped = true;
    }

    mipi_dsi_v2_osd0_base_layer_swap();

    if (dsi_v2_cycle_callback != NULL) {
        dsi_v2_cycle_callback();
    }
    if (swapped && dsi_v2_swap_callback != NULL) {
        dsi_v2_swap_callback();
    }
}

void mipi_dsi_v2_osd_irq_init(struct bflb_device_s *osd)
{
    dsi_v2_osd = osd;
    bflb_osd_int_clear(osd);
    bflb_osd_int_mask(osd, false);
    bflb_irq_attach(osd->irq_num, mipi_dsi_v2_osd0_isr, NULL);
    bflb_irq_enable(osd->irq_num);
}

int mipi_dsi_v2_display_init(const mipi_dsi_v2_init_t *init_config)
{
    if (init_config == NULL || init_config->timing == NULL ||
        (init_config->base_format != DPI_DATA_FORMAT_RGB565 &&
         init_config->base_format != DPI_DATA_FORMAT_Y_UV_PLANAR) ||
        (init_config->osd_format != MIPI_DSI_V2_OSD_FORMAT_RGB565 &&
         init_config->osd_format != MIPI_DSI_V2_OSD_FORMAT_ARGB8888 &&
         init_config->osd_format != MIPI_DSI_V2_OSD_FORMAT_NONE)) {
        return -2;
    }

    const mipi_dsi_v2_timing_t *cfg = init_config->timing;
    struct bflb_device_s *dpi = bflb_device_get_by_name("dpi");
    struct bflb_device_s *osd = bflb_device_get_by_name("osd0");
    bool osd_enabled = init_config->osd_format != MIPI_DSI_V2_OSD_FORMAT_NONE;

    if (dpi == NULL || osd == NULL) {
        return -1;
    }
    if (osd_enabled && init_config->osd0_frame_buff == NULL) {
        return -2;
    }

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
        .test_pattern = DPI_TEST_PATTERN_NULL,
        .data_format = init_config->base_format,
        .framebuffer_addr = 0,
        .uv_framebuffer_addr = 0,
    };
    bflb_dpi_init(dpi, &dpi_config);
    bflb_dpi_feature_control(dpi, DPI_CMD_SET_BURST, DPI_BURST_INCR8);

    struct bflb_osd_blend_config_s osd_blend_config = {
        .blend_format = osd_enabled && init_config->osd_format == MIPI_DSI_V2_OSD_FORMAT_RGB565
                            ? OSD_BLEND_FORMAT_RGB565
                            : OSD_BLEND_FORMAT_ARGB8888,
        .order_a = 3,
        .order_rv = 2,
        .order_gy = 1,
        .order_bu = 0,
        .coor = {
            .start_x = 0,
            .start_y = 0,
            .end_x = osd_enabled ? cfg->width : 1,
            .end_y = osd_enabled ? cfg->height : 1,
        },
        .layer_buffer_addr = (uint32_t)(uintptr_t)(osd_enabled ? init_config->osd0_frame_buff : &dsi_v2_sync_pixel),
    };
    dsi_v2_sync_pixel = 0;
    bflb_l1c_dcache_clean_range(&dsi_v2_sync_pixel, sizeof(dsi_v2_sync_pixel));
    bflb_osd_blend_init(osd, &osd_blend_config);
    if (!osd_enabled) {
        bflb_osd_blend_set_global_a(osd, true, 0);
    }
    bflb_osd_blend_enable(osd);

    dsi_v2_dpi = dpi;
    dsi_v2_base_format = init_config->base_format;
    dsi_v2_screen_is_base = !osd_enabled;
    dsi_v2_buf_size = osd_enabled ? (uint32_t)cfg->width * cfg->height *
                       (init_config->osd_format == MIPI_DSI_V2_OSD_FORMAT_RGB565 ? 2U : 4U) :
                       (init_config->base_format == DPI_DATA_FORMAT_RGB565 ?
                        (uint32_t)cfg->width * cfg->height * 2U : 0U);
    dsi_v2_screen_using = osd_enabled ? init_config->osd0_frame_buff : init_config->base_frame_buff;
    dsi_v2_screen_pending = dsi_v2_screen_using;

    if (osd_enabled) {
        bflb_l1c_dcache_clean_range(init_config->osd0_frame_buff, dsi_v2_buf_size);
    }

    /* Keep the base address out of bflb_dpi_init(); apply an optional RGB565
     * base buffer through the hardware's frame-switch API after setup. */
    if (init_config->base_format == DPI_DATA_FORMAT_RGB565 &&
        init_config->base_frame_buff != NULL) {
        bflb_l1c_dcache_clean_range(init_config->base_frame_buff,
                                    (uint32_t)cfg->width * cfg->height * 2U);
        bflb_dpi_framebuffer_switch(dpi, (uint32_t)(uintptr_t)init_config->base_frame_buff);
    }

    mipi_dsi_v2_osd_irq_init(osd);
    return 0;
}

int mipi_dsi_v2_screen_switch(void *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }
    if (dsi_v2_osd == NULL || dsi_v2_dpi == NULL) {
        return -2;
    }
    if (dsi_v2_buf_size != 0U) {
        bflb_l1c_dcache_clean_range(screen_buffer, dsi_v2_buf_size);
    }

    uintptr_t flags = bflb_irq_save();
    if (dsi_v2_screen_is_base) {
        if (dsi_v2_base_format != DPI_DATA_FORMAT_RGB565) {
            bflb_irq_restore(flags);
            return -3;
        }
        bflb_dpi_framebuffer_switch(dsi_v2_dpi, (uint32_t)(uintptr_t)screen_buffer);
    } else {
        bflb_osd_blend_set_layer_buffer(dsi_v2_osd, (uint32_t)(uintptr_t)screen_buffer);
        bflb_osd_int_clear(dsi_v2_osd);
    }
    dsi_v2_screen_pending = screen_buffer;
    bflb_irq_restore(flags);
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
