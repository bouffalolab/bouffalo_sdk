#include "ek79007_wks70wsv114_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_EK79007_WKS70WSV114)

/*
 * WKS70WSV114 (EK79007 driver IC), 1024x600 landscape, RGB888 link, MIPI DSI.
 *
 * EK79007 is a DSI-to-RGB bridge, not a frame-memory panel controller: after
 * reset it only takes the 0x80..0x86 / 0xB1 / 0xB2 vendor configuration
 * registers over LPDT, then converts the incoming DSI video stream to parallel
 * RGB for the glass. There is no 0x11 sleep-out, no 0x29 display-on and no 0x3A
 * pixel-format register -- the link format is whatever the DSI packetizer sends
 * (BFLB_DSI_DATA_RGB888 below), so this init table is much shorter than the
 * DCS-based panels in this directory.
 *
 * Init values come from the vendor scripts EK79007_Initial_Code_2lane /
 * EK79007_Initial_Code_4lane. The two differ only in the trailing 0xB2 lane
 * register, so both tables live here and the active one is picked from
 * EK79007_WKS70WSV114_LANE_NUM (lcd_conf.h / lcd_conf_user.h).
 *
 * Panel reset is driven by the LCD framework (LCD_RESET_* in lcd_conf_user.h).
 * The vendor sequence is 5 ms high / 10 ms low / 120 ms high, which maps to
 * LCD_RESET_ACTIVE_LEVEL 0, LCD_RESET_HOLD_MS 10, LCD_RESET_DELAY 120.
 */

struct ek79007_wks70wsv114_instr {
    uint8_t cmd;
    uint8_t data;
};

#define EK79007_WKS70WSV114_COMMAND_INSTR(_cmd, _data) \
    {                                                  \
        .cmd = (_cmd),                                 \
        .data = (_data),                               \
    }

/* WKS70WSV114 panel timing, from the panel datasheet:
 *   DCLK 51.2 MHz, 1 horizontal line 1344 clk, HSYNC blanking (HSA+HBP) 160, HFP 160
 *                  1 vertical frame  635 line, VSYNC blanking (VSA+VBP)  23, VFP  12
 * The datasheet folds the sync pulse into the "blanking" figure while this
 * framework takes hsw/hbp separately, so the pulse widths are split out of the
 * blanking totals (hsw + hbp = 160, vsw + vbp = 23) and stay inside the
 * datasheet limits (HSA 1..140, VSA 1..20). The line/frame totals -- and with
 * them the panel's own clock budget -- are unchanged.
 *
 * The display clock tree cannot produce 51.2 MHz exactly (only WIFIPLL
 * 160/240/96 MHz divided by 1..16), so it runs at 160/3 = 53.33 MHz: 1344 * 635
 * = 853440 clk/frame gives ~62.5 Hz. For an exact 60 Hz at that pixel clock,
 * stretch hfp to 216 (1400 clk/line, 889000 clk/frame).
 *
 * RGB888 over 2 lanes needs 53.33 MHz * 24 / 2 = 640 Mbps/lane, so the 850 MHz
 * HS bit clock leaves ~30% headroom; the same PLL covers the 4-lane wiring at
 * 320 Mbps/lane. */
static const mipi_dsi_v2_timing_t ek79007_wks70wsv114_timing = {
    .width      = EK79007_WKS70WSV114_DSI_W,
    .height     = EK79007_WKS70WSV114_DSI_H,
    .hsw        = 20,
    .hbp        = 140,
    .hfp        = 160,
    .vsw        = 2,
    .vbp        = 21,
    .vfp        = 12,
#if (EK79007_WKS70WSV114_LANE_NUM == 4)
    .lane_num   = BFLB_DSI_LANES_4,
#else
    .lane_num   = BFLB_DSI_LANES_2,
#endif
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB888,
    .reset_pin  = MIPI_DSI_V2_RESET_EXTERNAL,

    /* pll */
    .pll_cfg         = &dsipllCfg_850M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_160M,
    .display_clk_div = 2, /* 160 MHz / (2 + 1) = 53.33 MHz pixel clock */
    .dsi_hs_clock    = 850 * 1000 * 1000,

    .dphy = {
        .time_clk_exit     = 5,
        .time_clk_trail    = 3,
        .time_clk_zero     = 0xf,
        .time_data_exit    = 5,
        .time_data_prepare = 1,
        .time_data_trail   = 3,
        .time_data_zero    = 6,
        .time_lpx          = 3,
        .time_req_ready    = 0,
        .time_ta_get       = 0x13,
        .time_ta_go        = 0xf,
        .time_wakeup       = 0x9c41,
    },
};

/* Vendor init sequence. 0x80..0x86 are the internal power/timing registers and
 * 0xB1 the RGB output mode; 0xB2 selects 2-lane MIPI and must be omitted on
 * 4-lane wiring (the IC defaults to 4 lanes). */
static const struct ek79007_wks70wsv114_instr ek79007_wks70wsv114_init[] = {
    EK79007_WKS70WSV114_COMMAND_INSTR(0x80, 0x8B),
    EK79007_WKS70WSV114_COMMAND_INSTR(0x81, 0x78),
    EK79007_WKS70WSV114_COMMAND_INSTR(0x82, 0x84),
    EK79007_WKS70WSV114_COMMAND_INSTR(0x83, 0x88),
    EK79007_WKS70WSV114_COMMAND_INSTR(0x84, 0xA8),
    EK79007_WKS70WSV114_COMMAND_INSTR(0x85, 0xE3),
    EK79007_WKS70WSV114_COMMAND_INSTR(0x86, 0x88),
    EK79007_WKS70WSV114_COMMAND_INSTR(0xB1, 0x04),
#if (EK79007_WKS70WSV114_LANE_NUM == 2)
    EK79007_WKS70WSV114_COMMAND_INSTR(0xB2, 0x10), /* 2 lane MIPI */
#endif
};

#define EK79007_WKS70WSV114_INIT_LEN \
    (sizeof(ek79007_wks70wsv114_init) / sizeof(ek79007_wks70wsv114_init[0]))

static void ek79007_wks70wsv114_run_init_table(void)
{
    for (unsigned int i = 0; i < EK79007_WKS70WSV114_INIT_LEN; i++) {
        const struct ek79007_wks70wsv114_instr *instr = &ek79007_wks70wsv114_init[i];
        uint8_t data = instr->data;

        /* data_type 0: one payload byte, so this goes out as a DCS short write
         * with parameter (0x15), matching the vendor CMD/INDEX register pair. */
        mipi_dsi_v2_dcs_write_cmd(0, instr->cmd, &data, 1);
    }
}

static int ek79007_wks70wsv114_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&ek79007_wks70wsv114_timing);

    /* Vendor register sequence (sent over LPDT in LP mode) */
    ek79007_wks70wsv114_run_init_table();

    /* The bridge needs its internal PLL/regulators settled before the first
     * video line arrives; the vendor sequence has no polling handshake. */
    bflb_mtimer_delay_ms(20);

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&ek79007_wks70wsv114_timing);

    return 0;
}

int ek79007_wks70wsv114_dsi_init(ek79007_wks70wsv114_dsi_color_t *screen_buffer)
{
#if (EK79007_WKS70WSV114_FB_MODE == EK79007_WKS70WSV114_FB_MODE_RGB565)
    static uint32_t osd1_sync_pixel __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
#endif

    if (screen_buffer == NULL) {
        return -1;
    }

    int ret = ek79007_wks70wsv114_prepare();
    if (ret != 0) {
        return ret;
    }

#if (EK79007_WKS70WSV114_FB_MODE == EK79007_WKS70WSV114_FB_MODE_RGB565)
    osd1_sync_pixel = 0;
    bflb_l1c_dcache_clean_range(screen_buffer,
                                EK79007_WKS70WSV114_DSI_W * EK79007_WKS70WSV114_DSI_H *
                                    sizeof(*screen_buffer));
    bflb_l1c_dcache_clean_range(&osd1_sync_pixel, sizeof(osd1_sync_pixel));

    /* DPI expands the RGB565 framebuffer to the panel's RGB888 DSI stream. OSD1
     * stays transparent and supplies the frame-boundary interrupt for LVGL swaps. */
    return mipi_dsi_v2_rgb565_display_init(&ek79007_wks70wsv114_timing,
                                            (uint32_t)(uintptr_t)screen_buffer,
                                            (uint32_t)(uintptr_t)&osd1_sync_pixel);
#else
    /* YUV video scans out on the DPI base layer while LVGL uses the full-screen
     * ARGB8888 OSD0 layer. */
    return mipi_dsi_v2_display_init(&ek79007_wks70wsv114_timing,
                                    (uint32_t)(uintptr_t)screen_buffer);
#endif
}

int ek79007_wks70wsv114_dsi_screen_switch(ek79007_wks70wsv114_dsi_color_t *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }

#if (EK79007_WKS70WSV114_FB_MODE == EK79007_WKS70WSV114_FB_MODE_RGB565)
    return mipi_dsi_v2_rgb565_screen_switch(screen_buffer);
#else
    return mipi_dsi_v2_screen_switch(screen_buffer);
#endif
}

ek79007_wks70wsv114_dsi_color_t *ek79007_wks70wsv114_dsi_get_screen_using(void)
{
#if (EK79007_WKS70WSV114_FB_MODE == EK79007_WKS70WSV114_FB_MODE_RGB565)
    return (ek79007_wks70wsv114_dsi_color_t *)mipi_dsi_v2_rgb565_get_screen_using();
#else
    return (ek79007_wks70wsv114_dsi_color_t *)mipi_dsi_v2_get_screen_using();
#endif
}

int ek79007_wks70wsv114_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
#if (EK79007_WKS70WSV114_FB_MODE == EK79007_WKS70WSV114_FB_MODE_RGB565)
    return mipi_dsi_v2_rgb565_frame_callback_register(callback_type, callback);
#else
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
#endif
}

const mipi_dsi_v2_timing_t *ek79007_wks70wsv114_dsi_get_timing(void)
{
    return &ek79007_wks70wsv114_timing;
}

int display_prepare(void)
{
    return ek79007_wks70wsv114_prepare();
}

/* EK79007 has no DCS display-on/off or sleep registers, so the panel is "on"
 * exactly while the DSI link is streaming video. These hooks therefore start and
 * stop HS mode instead of sending 0x29 / 0x28 / 0x10 like the DCS panels do. */
int display_enable(void)
{
    mipi_dsi_v2_hs_mode_start(&ek79007_wks70wsv114_timing);
    return 0;
}

int display_disable(void)
{
    return mipi_dsi_v2_hs_mode_stop();
}

int display_unprepare(void)
{
    mipi_dsi_v2_deinit();
    return 0;
}

#endif /* LCD_DSI_EK79007_WKS70WSV114 */
