#include "st7102_yh494_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_ST7102_YH494)

/*
 * YH-494BSAC002N1 (ST7102_YH494 driver IC), 480x960 portrait, RGB565, 2-lane MIPI DSI.
 *
 * The init sequence comes from the vendor script (ST7102_YH494_BOE_494_*.txt) and the
 * matching Linux st7701s_xiaomi cmd list. Unlike the ILI9881C (cmd,data) table,
 * ST7102_YH494 needs variable-length payloads AND a distinction between DCS writes
 * (0x05 short / 0x39 long) and *generic* long writes (0x29) — KEY unlock, gamma,
 * GIP, tear-on are all generic. So the table carries an explicit data type.
 */

/* op codes in the init table */
#define ST7102_YH494_OP_CMD   0 /* send payload with .data_type via mipi_dsi_v2 */
#define ST7102_YH494_OP_DELAY 1 /* delay .len milliseconds */

/* MIPI data types used by this panel */
#define ST7102_YH494_DT_DCS_SHORT   DSI_V2_DCS_SHORT_WRITE    /* 0x05 */
#define ST7102_YH494_DT_DCS_LONG    DSI_V2_DCS_LONG_WRITE     /* 0x39 */
#define ST7102_YH494_DT_GENERIC_LONG DSI_V2_GENERIC_LONG_WRITE /* 0x29 */

struct st7102_yh494_instr {
    uint8_t op;        /* ST7102_YH494_OP_xxx */
    uint8_t data_type; /* ST7102_YH494_DT_xxx (for ST7102_YH494_OP_CMD) */
    uint8_t len;       /* payload length (for CMD) or delay ms (for DELAY) */
    uint8_t payload[52];
};

#define ST7102_YH494_DCS_SHORT(...)  { .op = ST7102_YH494_OP_CMD, .data_type = ST7102_YH494_DT_DCS_SHORT,    .len = sizeof((uint8_t[]){__VA_ARGS__}), .payload = { __VA_ARGS__ } }
#define ST7102_YH494_DCS_LONG(...)   { .op = ST7102_YH494_OP_CMD, .data_type = ST7102_YH494_DT_DCS_LONG,     .len = sizeof((uint8_t[]){__VA_ARGS__}), .payload = { __VA_ARGS__ } }
#define ST7102_YH494_GEN_LONG(...)   { .op = ST7102_YH494_OP_CMD, .data_type = ST7102_YH494_DT_GENERIC_LONG, .len = sizeof((uint8_t[]){__VA_ARGS__}), .payload = { __VA_ARGS__ } }
#define ST7102_YH494_DELAY(ms)       { .op = ST7102_YH494_OP_DELAY, .len = (ms) }

/* YH-494 panel timing: 480x960 portrait, 2-lane, RGB565. Reuses the 800M DSI
 * PLL (HS ~400MHz, ample for 480x960@60 RGB565 ~344Mbps/lane). Timing values
 * from the Linux fb_videomode (st7701s_xiaomi). */
static const mipi_dsi_v2_timing_t st7102_yh494_timing = {
    .width      = 480,
    .height     = 960,
    .hsw        = 2,
    .hbp        = 40,
    .hfp        = 80,
    .vsw        = 2,
    .vbp        = 10,
    .vfp        = 215,
    .lane_num   = BFLB_DSI_LANES_2,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB565,
    .reset_pin  = GPIO_PIN_2,

    .pll_cfg         = &dsipllCfg_850M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_160M,//240 div4=66hz
    .display_clk_div = 4,
    .dsi_hs_clock    = 850*1000*1000,

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
        .time_ta_go        = 0xF,
        .time_wakeup       = 0x9c41,
    },
};

#define ST7102_YH494_BL_PIN GPIO_PIN_40 /* backlight enable (driven high, same board as KD050) */

/* Full init sequence (ST7102_YH494_BOE_494 vendor script == Linux st7701s_xiaomi list). */
static const struct st7102_yh494_instr st7102_yh494_init[] = {
    ST7102_YH494_DCS_SHORT(0x10), /* sleep in */
    ST7102_YH494_DCS_SHORT(0x28), /* display off */

    /* KEY unlock */
    ST7102_YH494_GEN_LONG(0x99, 0x71, 0x02, 0xa2),
    ST7102_YH494_GEN_LONG(0x99, 0x71, 0x02, 0xa3),
    ST7102_YH494_GEN_LONG(0x99, 0x71, 0x02, 0xa4),

    ST7102_YH494_DCS_LONG(0x78, 0x21),
    ST7102_YH494_DCS_LONG(0x79, 0xCF),

    /* Power: VGH/VGL */
    ST7102_YH494_DCS_LONG(0xB0, 0x22, 0x57, 0x1E, 0x61, 0x2F, 0x57, 0x61),
    /* Source */
    ST7102_YH494_DCS_LONG(0xB7, 0x64, 0x64),
    /* VCOM */
    ST7102_YH494_DCS_LONG(0xBF, 0x6e, 0x6e),

    /* Gamma (generic long) */
    ST7102_YH494_GEN_LONG(0xC8, 0x00, 0x00, 0x13, 0x23, 0x3E, 0x00, 0x6A, 0x03, 0xB0, 0x06, 0x11, 0x0F, 0x07, 0x85,
                    0x03, 0x21, 0xD5, 0x01, 0x18, 0x00, 0x22, 0x56, 0x0F, 0x98, 0x0A, 0x32, 0xF8, 0x0D, 0x48,
                    0x0F, 0xF3, 0x80, 0x0F, 0xAC, 0xC1, 0x03, 0xC4),
    ST7102_YH494_GEN_LONG(0xC9, 0x00, 0x00, 0x13, 0x23, 0x3E, 0x00, 0x6A, 0x03, 0xB0, 0x06, 0x11, 0x0F, 0x07, 0x85,
                    0x03, 0x21, 0xD5, 0x01, 0x18, 0x00, 0x22, 0x56, 0x0F, 0x98, 0x0A, 0x32, 0xF8, 0x0D, 0x48,
                    0x0F, 0xF3, 0x80, 0x0F, 0xAC, 0xC1, 0x03, 0xC4),

    /* GIP */
    ST7102_YH494_DCS_LONG(0xD7, 0x10, 0x0C, 0x77, 0x19, 0xE0, 0xE0),
    ST7102_YH494_DCS_LONG(0xA3, 0x40, 0x03, 0x80, 0xCF, 0x44, 0x00, 0x00, 0x00, 0x00, 0x04, 0x6F, 0x6F, 0x00, 0x1A,
                    0x00, 0x45, 0x05, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x02, 0x20, 0x52, 0x00, 0x05,
                    0x00, 0x00, 0xFF),
    ST7102_YH494_DCS_LONG(0xA6, 0x02, 0x00, 0x24, 0x55, 0x35, 0x00, 0x38, 0x00, 0x5E, 0x5E, 0x00, 0x24, 0x55, 0x36,
                    0x00, 0x37, 0x00, 0x5E, 0x5E, 0x02, 0xAC, 0x51, 0x3A, 0x00, 0x00, 0x00, 0x5E, 0x5E, 0x00,
                    0xAC, 0x11, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x5E, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00),
    ST7102_YH494_DCS_LONG(0xA7, 0x19, 0x19, 0x00, 0x64, 0x40, 0x07, 0x16, 0x40, 0x00, 0x04, 0x03, 0x5E, 0x5E, 0x00,
                    0x64, 0x40, 0x25, 0x34, 0x00, 0x00, 0x02, 0x01, 0x5E, 0x5E, 0x00, 0x64, 0x40, 0x4B, 0x5A,
                    0x00, 0x00, 0x02, 0x01, 0x5E, 0x5E, 0x00, 0x24, 0x40, 0x69, 0x78, 0x00, 0x00, 0x00, 0x00,
                    0x5E, 0x5E, 0x00, 0x44),
    ST7102_YH494_DCS_LONG(0xAC, 0x08, 0x0A, 0x11, 0x00, 0x13, 0x03, 0x1B, 0x18, 0x06, 0x1A, 0x19, 0x1B, 0x1B, 0x1B,
                    0x18, 0x1B, 0x09, 0x0B, 0x10, 0x02, 0x12, 0x01, 0x1B, 0x18, 0x06, 0x1A, 0x19, 0x1B, 0x1B,
                    0x1B, 0x18, 0x1B, 0xFF, 0x67, 0xFF, 0x67, 0x00),
    ST7102_YH494_DCS_LONG(0xAD, 0xCC, 0x40, 0x46, 0x11, 0x04, 0x6F, 0x6F),
    ST7102_YH494_DCS_LONG(0xE8, 0x30, 0x07, 0x00, 0x7A, 0x7A, 0x9C, 0x00, 0xE2, 0x04, 0x00, 0x00, 0x00, 0x00, 0xEF),

    ST7102_YH494_GEN_LONG(0x75, 0x03, 0x04),
    ST7102_YH494_DCS_LONG(0xE7, 0x8B, 0x3C, 0x00, 0x0C, 0xF0, 0x5D, 0x00, 0x5D, 0x00, 0x5D, 0x00, 0x5D, 0x00, 0xFF,
                    0x00, 0x08, 0x7B, 0x00, 0x00, 0xC8, 0x6A, 0x5A, 0x08, 0x1A, 0x3C, 0x00, 0x91, 0x01, 0xCC,
                    0x01, 0x7F, 0xF0, 0x22),
    ST7102_YH494_DCS_LONG(0xE9, 0x4C, 0x7F, 0x08, 0x07, 0x1A, 0x7A, 0x22, 0x1A, 0x33),

    ST7102_YH494_DCS_SHORT(0x11), /* sleep out */
    ST7102_YH494_DELAY(120),
    ST7102_YH494_DCS_SHORT(0x29), /* display on */

    ST7102_YH494_GEN_LONG(0x35, 0x00), /* tear on (V-blank) */
};

#define ST7102_YH494_INIT_LEN (sizeof(st7102_yh494_init) / sizeof(st7102_yh494_init[0]))

static void st7102_yh494_run_init_table(void)
{
    for (unsigned int i = 0; i < ST7102_YH494_INIT_LEN; i++) {
        const struct st7102_yh494_instr *instr = &st7102_yh494_init[i];

        if (instr->op == ST7102_YH494_OP_DELAY) {
            bflb_mtimer_delay_ms(instr->len);
        } else {
            /* payload[0] is the command, rest is data. For a generic long write
             * the vendor type is forced; otherwise data_type 0 lets the DCS
             * packet type be auto-selected by length. */
            uint8_t dt = (instr->data_type == ST7102_YH494_DT_GENERIC_LONG) ? instr->data_type : 0;
            mipi_dsi_v2_dcs_write_cmd(dt, instr->payload[0],
                                      (instr->len > 1) ? &instr->payload[1] : NULL,
                                      instr->len - 1);
        }
    }
}

static int st7102_yh494_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&st7102_yh494_timing);

    /* Panel register sequence (sent over LPDT in LP mode) */
    st7102_yh494_run_init_table();

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&st7102_yh494_timing);

    return 0;
}

int st7102_yh494_dsi_init(st7102_yh494_dsi_color_t *screen_buffer)
{
    int ret = st7102_yh494_prepare();
    if (ret != 0) {
        return ret;
    }
    /* DPI background + OSD0 overlay + OSD SEOF interrupt. screen_buffer is the
     * initial OSD canvas handed down by lcd_init(). */
    return mipi_dsi_v2_display_init(&st7102_yh494_timing, (uint32_t)screen_buffer);
}

int st7102_yh494_dsi_screen_switch(st7102_yh494_dsi_color_t *screen_buffer)
{
    return mipi_dsi_v2_screen_switch((void *)screen_buffer);
}

st7102_yh494_dsi_color_t *st7102_yh494_dsi_get_screen_using(void)
{
    return (st7102_yh494_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int st7102_yh494_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *st7102_yh494_dsi_get_timing(void)
{
    return &st7102_yh494_timing;
}

int display_prepare(void)
{
    return st7102_yh494_prepare();
}

int display_enable(void)
{
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_SET_DISPLAY_ON, NULL, 0);
    return 0;
}

int display_disable(void)
{
    mipi_dsi_v2_dcs_write_cmd(0, 0x28, NULL, 0); /* DCS set_display_off */
    return 0;
}

int display_unprepare(void)
{
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_ENTER_SLEEP_MODE, NULL, 0);
    return 0;
}

#endif /* LCD_DSI_ST7102_YH494 */


