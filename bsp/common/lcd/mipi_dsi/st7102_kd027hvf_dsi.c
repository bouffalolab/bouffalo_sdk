#include "st7102_kd027hvf_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_ST7102_KD027HVF)

/*
 * KD027HVFID009 (ST7102 driver IC), 320x320, RGB565, 1-lane MIPI DSI.
 *
 * The init sequence comes from the vendor script (KD027HVFID009 MIPI.txt). It is
 * a plain write_command/write_data (DCS) list -- unlike ST7102_YH494 there are no
 * generic-long-write requirements, so the table sends everything as auto-typed DCS
 * (short-write / short-write-param / long-write picked by payload length).
 *
 * VCC=3.3V, IOVCC=1.8V. The vendor 0x3A=0x70 selects a 24-bit host format, but the
 * DSI link here carries RGB565, so 0x3A is sent as 0x55 (16bpp) to match.
 */

/* op codes in the init table */
#define ST7102_KD027HVF_OP_CMD   0 /* send payload[0]=cmd + rest=data via mipi_dsi_v2 */
#define ST7102_KD027HVF_OP_DELAY 1 /* delay .len milliseconds */

struct st7102_kd027hvf_instr {
    uint8_t op;         /* ST7102_KD027HVF_OP_xxx */
    uint8_t len;        /* payload length (for CMD) or delay ms (for DELAY) */
    uint8_t payload[16]; /* payload[0] is the command byte */
};

#define ST7102_KD027HVF_CMD(...)   { .op = ST7102_KD027HVF_OP_CMD, .len = sizeof((uint8_t[]){__VA_ARGS__}), .payload = { __VA_ARGS__ } }
#define ST7102_KD027HVF_DELAY(ms)  { .op = ST7102_KD027HVF_OP_DELAY, .len = (ms) }

/* KD027HVFID009 panel timing: 320x320, 1-lane, RGB565. Pixel clock ~7MHz
 * (htotal 336 * vtotal 347 * 60 ~= 6.99MHz), from the vendor note RGB_CLOCK=7M.
 * 160MHz / 23 ~= 6.96MHz DPI pixel clock. Single-lane RGB565 link is ~112Mbps,
 * well within the 400M DSI PLL. */
static const mipi_dsi_v2_timing_t st7102_kd027hvf_timing = {
    .width      = ST7102_KD027HVF_DSI_W,
    .height     = ST7102_KD027HVF_DSI_H,
    .hsw        = 2,
    .hbp        = 4,
    .hfp        = 10,
    .vsw        = 2,
    .vbp        = 8,
    .vfp        = 17,
    .lane_num   = BFLB_DSI_LANES_1,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB565,
    .reset_pin  = GPIO_PIN_2,

    .pll_cfg         = &dsipllCfg_400M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_96M,
    .display_clk_div = 12, // 55fps
    .dsi_hs_clock    = 400 * 1000 * 1000,

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

#define ST7102_KD027HVF_BL_PIN GPIO_PIN_40 /* backlight enable (driven high, same board as KD050) */

/* Full init sequence (vendor KD027HVFID009 MIPI.txt). The reset pulse at the top
 * of the vendor script is driven by the LCD framework (LCD_RESET_*) before this
 * runs, and the trailing 0x2C (write memory start) is dropped -- in DSI video mode
 * the DPI scans the framebuffer continuously, no host memory write is needed. */
static const struct st7102_kd027hvf_instr st7102_kd027hvf_init[] = {
    ST7102_KD027HVF_CMD(0xF0, 0xC3), /* Command Set Control (unlock) */
    ST7102_KD027HVF_CMD(0xF0, 0x96), /* Command Set Control (unlock) */

    ST7102_KD027HVF_CMD(0x36, 0x48), /* Memory Data Access Control */
    ST7102_KD027HVF_CMD(0x3A, 0x55), /* Interface Pixel Format: RGB565 (vendor 0x70 was 24-bit) */

    ST7102_KD027HVF_CMD(0xB4, 0x01), /* 1-dot Inversion */
    ST7102_KD027HVF_CMD(0xB1, 0x80, 0x10), /* FRMCTR1: FRS/DIVA, RTNA */

    ST7102_KD027HVF_CMD(0xC0, 0x80, 0x64), /* Power Control 1: AVDD/AVCL, VGH=15V VGL=-10V */
    ST7102_KD027HVF_CMD(0xC1, 0x10),       /* Power Control 2: VOP=4.3V */
    ST7102_KD027HVF_CMD(0xC2, 0xA7),       /* Power Control 3 */
    ST7102_KD027HVF_CMD(0xC5, 0x15),       /* VCOM Control */

    ST7102_KD027HVF_CMD(0xE8, 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33), /* Display Output Ctrl Adjust */

    ST7102_KD027HVF_CMD(0xE0, 0xF0, 0x06, 0x0B, 0x07, 0x06, 0x05, 0x2E, 0x33,
                        0x47, 0x3A, 0x17, 0x16, 0x2E, 0x31), /* GAMMA */
    ST7102_KD027HVF_CMD(0xE1, 0xF0, 0x09, 0x0D, 0x09, 0x08, 0x23, 0x2E, 0x33,
                        0x46, 0x38, 0x13, 0x13, 0x2C, 0x32), /* GAMMA */

    ST7102_KD027HVF_CMD(0xF0, 0x3C), /* Command Set Control (relock) */
    ST7102_KD027HVF_CMD(0xF0, 0x69), /* Command Set Control (relock) */

    ST7102_KD027HVF_CMD(0x21), /* Enter invert mode */

    ST7102_KD027HVF_CMD(0x11), /* Exit Sleep */
    ST7102_KD027HVF_DELAY(120),
    ST7102_KD027HVF_CMD(0x29), /* Display On */
    ST7102_KD027HVF_DELAY(50),
};

#define ST7102_KD027HVF_INIT_LEN (sizeof(st7102_kd027hvf_init) / sizeof(st7102_kd027hvf_init[0]))

static void st7102_kd027hvf_run_init_table(void)
{
    for (unsigned int i = 0; i < ST7102_KD027HVF_INIT_LEN; i++) {
        const struct st7102_kd027hvf_instr *instr = &st7102_kd027hvf_init[i];

        if (instr->op == ST7102_KD027HVF_OP_DELAY) {
            bflb_mtimer_delay_ms(instr->len);
        } else {
            /* payload[0] is the command, rest is data. data_type 0 lets the DCS
             * packet type be auto-selected by payload length. */
            mipi_dsi_v2_dcs_write_cmd(0, instr->payload[0],
                                      (instr->len > 1) ? &instr->payload[1] : NULL,
                                      instr->len - 1);
        }
    }
}

static int st7102_kd027hvf_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&st7102_kd027hvf_timing);

    /* Panel register sequence (sent over LPDT in LP mode) */
    st7102_kd027hvf_run_init_table();

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&st7102_kd027hvf_timing);

    return 0;
}

int st7102_kd027hvf_dsi_init(st7102_kd027hvf_dsi_color_t *screen_buffer)
{

#if (ST7102_KD027HVF_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) || \
    (ST7102_KD027HVF_FB_MODE == ST7102_KD027HVF_FB_MODE_RGB565)
    if (screen_buffer == NULL) {
        return -1;
    }
#endif

    int ret = st7102_kd027hvf_prepare();
    if (ret != 0) {
        return ret;
    }

    const mipi_dsi_v2_init_t init_config = {
        .timing = &st7102_kd027hvf_timing,
        .base_frame_buff = (ST7102_KD027HVF_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE &&
                            ST7102_KD027HVF_FB_MODE == ST7102_KD027HVF_FB_MODE_RGB565) ? screen_buffer : NULL,
        .osd0_frame_buff = (ST7102_KD027HVF_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) ? screen_buffer : NULL,
        .base_format = (ST7102_KD027HVF_FB_MODE == ST7102_KD027HVF_FB_MODE_RGB565) ? DPI_DATA_FORMAT_RGB565 : DPI_DATA_FORMAT_Y_UV_PLANAR,
        .osd_format = ST7102_KD027HVF_OSD0_FORMAT,
    };
    return mipi_dsi_v2_display_init(&init_config);
}

int st7102_kd027hvf_dsi_screen_switch(st7102_kd027hvf_dsi_color_t *screen_buffer)
{
    return mipi_dsi_v2_screen_switch(screen_buffer);
}

st7102_kd027hvf_dsi_color_t *st7102_kd027hvf_dsi_get_screen_using(void)
{
    return (st7102_kd027hvf_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int st7102_kd027hvf_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *st7102_kd027hvf_dsi_get_timing(void)
{
    return &st7102_kd027hvf_timing;
}

int display_prepare(void)
{
    return st7102_kd027hvf_prepare();
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

#endif /* LCD_DSI_ST7102_KD027HVF */
