#include "axs15231e_jx371_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_AXS15231E_JX371)

/*
 * JX 3.71+15231B panel (AXS15231E driver IC), 258x960 portrait, RGB565, 1-lane MIPI DSI.
 *
 * The init sequence comes from the vendor script
 * (JX 3.71+15231B_MIPI_IIC_D01_V01_20240118.txt). Every register write in that
 * script is issued as DataType(0x29) -- a *generic* long write -- even the 4-byte
 * 0xC9 payload, so the table carries an explicit data type per entry rather than
 * letting the length pick the DCS packet type (same approach as st7102_yh494).
 * Only the trailing sleep-out / display-on pair is a DCS short write (0x05).
 *
 * The vendor script is written for forward scan. For reverse scan the panel note
 * asks for two changes: 0xD0 param 18: 0x40 -> 0xC0, and 0xD5 param 14: -> 0xF3.
 *
 * The DSI link here carries RGB565; the vendor script has no 0x3A, the host pixel
 * format is selected by the 0xA0/0xA2 test/timing registers instead, so nothing in
 * the table needs patching for 16bpp.
 */

/* op codes in the init table */
#define AXS15231E_JX371_OP_CMD   0 /* send payload with .data_type via mipi_dsi_v2 */
#define AXS15231E_JX371_OP_DELAY 1 /* delay .len milliseconds */

/* MIPI data types used by this panel */
#define AXS15231E_JX371_DT_DCS_SHORT    DSI_V2_DCS_SHORT_WRITE    /* 0x05 */
#define AXS15231E_JX371_DT_GENERIC_LONG DSI_V2_GENERIC_LONG_WRITE /* 0x29 */

struct axs15231e_jx371_instr {
    uint8_t op;        /* AXS15231E_JX371_OP_xxx */
    uint8_t data_type; /* AXS15231E_JX371_DT_xxx (for AXS15231E_JX371_OP_CMD) */
    uint8_t len;       /* payload length (for CMD) or delay ms (for DELAY) */
    uint8_t payload[32]; /* payload[0] is the command byte; longest is 0xA2 (1+31) */
};

#define AXS15231E_JX371_DCS_SHORT(...) { .op = AXS15231E_JX371_OP_CMD, .data_type = AXS15231E_JX371_DT_DCS_SHORT,    .len = sizeof((uint8_t[]){__VA_ARGS__}), .payload = { __VA_ARGS__ } }
#define AXS15231E_JX371_GEN_LONG(...)  { .op = AXS15231E_JX371_OP_CMD, .data_type = AXS15231E_JX371_DT_GENERIC_LONG, .len = sizeof((uint8_t[]){__VA_ARGS__}), .payload = { __VA_ARGS__ } }
#define AXS15231E_JX371_DELAY(ms)      { .op = AXS15231E_JX371_OP_DELAY, .len = (ms) }

/* JX 3.71+15231B panel timing: 258x960 portrait, 1-lane, RGB565.
 *
 * Porch values are the vendor loadHxx / loadVxx settings verbatim: htotal =
 * 258+10+60+50 = 378, vtotal = 960+25+60+150 = 1195. At 60fps that is a
 * ~27.1MHz pixel clock, matching the script's loadDCLKSet(28). 160MHz / 6 =
 * 26.67MHz gives ~59fps.
 *
 * The script's loadHSCLK(661) is the aggregate link rate; RGB565 over 1 lane
 * needs 26.67M x 16 / 1 = ~427Mbps, so the 500M DSI PLL gives safe headroom.
 */
static const mipi_dsi_v2_timing_t axs15231e_jx371_timing = {
    .width      = AXS15231E_JX371_DSI_W,
    .height     = AXS15231E_JX371_DSI_H,
    .hsw        = 10,
    .hbp        = 60,
    .hfp        = 50,
    .vsw        = 25,
    .vbp        = 60,
    .vfp        = 150,
    .lane_num   = BFLB_DSI_LANES_1,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB565,
    .reset_pin  = GPIO_PIN_2,

    .pll_cfg         = &dsipllCfg_400M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_160M,
    .display_clk_div = 6,  // 160M/6 = 26.67MHz (vendor uses 28MHz)
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

/* Full init sequence (vendor JX 3.71+15231B_MIPI_IIC_D01_V01_20240118.txt).
 *
 * The power-rail / reset preamble at the top of the vendor script (En18V, EnVCI,
 * EnVSP/EnVSN, EnBLT and the commented-out reset pulse) is board-level: the LCD
 * framework pulses reset via LCD_RESET_* in lcd_conf_user.h before this runs, and
 * the rails / backlight are handled by the board's power sequencing.
 *
 * 0xBB is the vendor KEY register: it is unlocked with the 0x5A/0xA5 magic at the
 * top and re-locked with all-zero at the bottom. */
static const struct axs15231e_jx371_instr axs15231e_jx371_init[] = {
    /* KEY unlock */
    AXS15231E_JX371_GEN_LONG(0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A,
                             0xA5),

    /* Test setting -- vendor note: normally not to be changed */
    AXS15231E_JX371_GEN_LONG(0xA0, 0x00, 0x30, 0x00, 0x02, 0x00, 0x00, 0x08,
                             0x3F, 0x20, 0x05, 0x3F, 0x3F, 0x00, 0x00, 0x00,
                             0x00, 0x00),

    AXS15231E_JX371_GEN_LONG(0xA2, 0x20, 0x0A, 0x0A, 0x3C, 0x3C, 0x64, 0x32,
                             0xC0, 0x02, 0x39, 0x7F, 0x7F, 0x7F, 0x20, 0xF8,
                             0x10, 0x02, 0xFF, 0xFF, 0xF0, 0x90, 0x01, 0x32,
                             0xA0, 0x91, 0xC0, 0x20, 0x7F, 0xFF, 0x00, 0x04),

    /* param 18 (0x40) is 0xC0 for reverse scan */
    AXS15231E_JX371_GEN_LONG(0xD0, 0xC0, 0x02, 0x72, 0x24, 0x08, 0x05, 0x10,
                             0x10, 0x70, 0x21, 0xC2, 0x40, 0x20, 0x02, 0xAA,
                             0x03, 0x10, 0x12, 0x60, 0x14, 0x1E, 0x51, 0x15,
                             0x00, 0xAC, 0x00, 0x00, 0x03, 0x0D, 0x12),

    AXS15231E_JX371_GEN_LONG(0xA3, 0xA0, 0x06, 0xAA, 0x00, 0x08, 0x02, 0x0A,
                             0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                             0x04, 0x04, 0x04, 0x04, 0x00, 0x55, 0x55),

    AXS15231E_JX371_GEN_LONG(0xC1, 0x31, 0x04, 0x02, 0x02, 0x71, 0x05, 0x24,
                             0x55, 0x02, 0x00, 0x41, 0x01, 0x53, 0xFF, 0xFF,
                             0xFF, 0x4F, 0x52, 0x00, 0x4F, 0x52, 0x00, 0x45,
                             0x3B, 0x0B, 0x02, 0x0D, 0x00, 0xFF, 0x40),

    AXS15231E_JX371_GEN_LONG(0xC3, 0x00, 0x00, 0x00, 0x50, 0x03, 0x00, 0x00,
                             0x00, 0x01, 0x80, 0x01),

    AXS15231E_JX371_GEN_LONG(0xC4, 0x00, 0x24, 0x33, 0x80, 0x66, 0xEA, 0x64,
                             0x32, 0xC8, 0x32, 0x32, 0x32, 0x90, 0x32, 0x10,
                             0x06, 0xDC, 0xFA, 0x00, 0x00, 0x80, 0xFE, 0x10,
                             0x40, 0x00, 0x0A, 0x02, 0x44, 0x50),

    AXS15231E_JX371_GEN_LONG(0xC5, 0x18, 0x00, 0x00, 0x03, 0xFE, 0x18, 0x38,
                             0x40, 0x10, 0x10, 0x88, 0xDE, 0x0D, 0x08, 0x0F,
                             0x0F, 0x01, 0x18, 0x38, 0x40, 0x10, 0x10, 0x00),

    AXS15231E_JX371_GEN_LONG(0xC6, 0x05, 0x0A, 0x05, 0x0A, 0x00, 0xE0, 0x2E,
                             0x0B, 0x12, 0x22, 0x12, 0x22, 0x01, 0x00, 0x00,
                             0x02, 0x6A, 0x18, 0xC8, 0x22),

    AXS15231E_JX371_GEN_LONG(0xC7, 0x50, 0x32, 0x28, 0x00, 0xA2, 0x80, 0x8F,
                             0x00, 0x80, 0x00, 0x00, 0x11, 0x9C, 0x6F, 0xFF,
                             0x22, 0x01, 0x01, 0xB9, 0x0F),

    AXS15231E_JX371_GEN_LONG(0xC9, 0x33, 0x44, 0x44, 0x01),

    AXS15231E_JX371_GEN_LONG(0xCF, 0x2C, 0x1E, 0x88, 0x58, 0x13, 0x18, 0x56,
                             0x18, 0x1E, 0x68, 0xFF, 0x00, 0x68, 0x08, 0x22,
                             0xC4, 0x0C, 0x77, 0x22, 0x44, 0xAA, 0x55, 0x04,
                             0x04, 0x16, 0xA0, 0x08),

    /* GIP -- param 14 (0x53) is 0xF3 for reverse scan */
    AXS15231E_JX371_GEN_LONG(0xD5, 0x19, 0x24, 0x84, 0x80, 0x35, 0x03, 0xA2,
                             0x35, 0x08, 0xC4, 0xC4, 0x88, 0x04, 0x28, 0x53,
                             0x42, 0x26, 0x19, 0x03, 0x03, 0x86, 0x00, 0x00,
                             0x00, 0xC0, 0x53, 0xAB, 0x19, 0x29, 0x00),

    AXS15231E_JX371_GEN_LONG(0xD6, 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC,
                             0xFE, 0x95, 0x00, 0x01, 0x01, 0xC5, 0xC5, 0x88,
                             0x75, 0x36, 0x20, 0x03, 0x03, 0x03, 0x03, 0x10,
                             0x10, 0x00, 0x83, 0x51, 0x22, 0x01, 0x00),

    AXS15231E_JX371_GEN_LONG(0xD7, 0x00, 0x18, 0x1F, 0x08, 0x0A, 0x0C, 0x0E,
                             0x06, 0x1F, 0x1A, 0x19, 0x1F, 0x15, 0x24, 0x04,
                             0x00, 0x15, 0x2D, 0x1F),

    AXS15231E_JX371_GEN_LONG(0xD8, 0x01, 0x18, 0x1F, 0x09, 0x0B, 0x0D, 0x0F,
                             0x07, 0x1F, 0x1A, 0x19, 0x1F),

    AXS15231E_JX371_GEN_LONG(0xD9, 0x05, 0x1F, 0x18, 0x08, 0x0A, 0x0C, 0x0E,
                             0x01, 0x1F, 0x1A, 0x19, 0x1F),

    AXS15231E_JX371_GEN_LONG(0xDD, 0x04, 0x1F, 0x18, 0x09, 0x0B, 0x0D, 0x0F,
                             0x00, 0x1F, 0x1A, 0x19, 0x1F),

    AXS15231E_JX371_GEN_LONG(0xDF, 0x44, 0x73, 0x4B, 0x69, 0x00, 0x0A, 0x02,
                             0x90),

    /* Gamma 2.2 */
    AXS15231E_JX371_GEN_LONG(0xE0, 0x3B, 0x08, 0x13, 0x1A, 0x2B, 0x29, 0x35,
                             0x2F, 0x44, 0x27, 0x2F, 0x18, 0x35, 0x2E, 0x31,
                             0x0D, 0x0F),

    AXS15231E_JX371_GEN_LONG(0xE1, 0x35, 0x08, 0x13, 0x1A, 0x2B, 0x29, 0x35,
                             0x2F, 0x44, 0x27, 0x2F, 0x18, 0x35, 0x2E, 0x31,
                             0x0D, 0x0F),

    AXS15231E_JX371_GEN_LONG(0xE2, 0x19, 0x20, 0x0A, 0x13, 0x13, 0x06, 0x11,
                             0x25, 0xD4, 0x22, 0x0B, 0x13, 0x12, 0x2D, 0x32,
                             0x2F, 0x03),

    AXS15231E_JX371_GEN_LONG(0xE3, 0x38, 0x20, 0x0A, 0x13, 0x13, 0x06, 0x11,
                             0x25, 0xC4, 0x21, 0x0A, 0x12, 0x11, 0x2C, 0x32,
                             0x2F, 0x27),

    AXS15231E_JX371_GEN_LONG(0xE4, 0x19, 0x20, 0x0D, 0x14, 0x0D, 0x08, 0x12,
                             0x2A, 0xD4, 0x26, 0x0E, 0x15, 0x13, 0x34, 0x39,
                             0x2F, 0x03),

    AXS15231E_JX371_GEN_LONG(0xE5, 0x38, 0x20, 0x0D, 0x13, 0x0D, 0x07, 0x12,
                             0x29, 0xC4, 0x25, 0x0D, 0x15, 0x12, 0x33, 0x39,
                             0x2F, 0x27),

    /* KEY re-lock */
    AXS15231E_JX371_GEN_LONG(0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00),

    AXS15231E_JX371_DCS_SHORT(0x11), /* Exit Sleep */
    AXS15231E_JX371_DELAY(200),
    AXS15231E_JX371_DCS_SHORT(0x29), /* Display On */
    AXS15231E_JX371_DELAY(100),
};

#define AXS15231E_JX371_INIT_LEN (sizeof(axs15231e_jx371_init) / sizeof(axs15231e_jx371_init[0]))

static void axs15231e_jx371_run_init_table(void)
{
    for (unsigned int i = 0; i < AXS15231E_JX371_INIT_LEN; i++) {
        const struct axs15231e_jx371_instr *instr = &axs15231e_jx371_init[i];

        if (instr->op == AXS15231E_JX371_OP_DELAY) {
            bflb_mtimer_delay_ms(instr->len);
        } else {
            /* payload[0] is the command, rest is data. The explicit data_type is
             * passed through, so a 0x29 generic long write stays a long packet
             * even for the 4-byte 0xC9 payload. */
            mipi_dsi_v2_dcs_write_cmd(instr->data_type, instr->payload[0],
                                      (instr->len > 1) ? &instr->payload[1] : NULL,
                                      instr->len - 1);
        }
    }
}

static int axs15231e_jx371_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&axs15231e_jx371_timing);

    /* Panel register sequence (sent over LPDT in LP mode) */
    axs15231e_jx371_run_init_table();

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&axs15231e_jx371_timing);

    return 0;
}

int axs15231e_jx371_dsi_init(axs15231e_jx371_dsi_color_t *screen_buffer)
{

#if (AXS15231E_JX371_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) || \
    (AXS15231E_JX371_FB_MODE == AXS15231E_JX371_FB_MODE_RGB565)
    if (screen_buffer == NULL) {
        return -1;
    }
#endif

    int ret = axs15231e_jx371_prepare();
    if (ret != 0) {
        return ret;
    }

    const mipi_dsi_v2_init_t init_config = {
        .timing = &axs15231e_jx371_timing,
        .base_frame_buff = (AXS15231E_JX371_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE &&
                            AXS15231E_JX371_FB_MODE == AXS15231E_JX371_FB_MODE_RGB565) ? screen_buffer : NULL,
        .osd0_frame_buff = (AXS15231E_JX371_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) ? screen_buffer : NULL,
        .base_format = (AXS15231E_JX371_FB_MODE == AXS15231E_JX371_FB_MODE_RGB565) ? DPI_DATA_FORMAT_RGB565 : DPI_DATA_FORMAT_Y_UV_PLANAR,
        .osd_format = AXS15231E_JX371_OSD0_FORMAT,
    };
    return mipi_dsi_v2_display_init(&init_config);
}

int axs15231e_jx371_dsi_screen_switch(axs15231e_jx371_dsi_color_t *screen_buffer)
{
    return mipi_dsi_v2_screen_switch(screen_buffer);
}

axs15231e_jx371_dsi_color_t *axs15231e_jx371_dsi_get_screen_using(void)
{
    return (axs15231e_jx371_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int axs15231e_jx371_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *axs15231e_jx371_dsi_get_timing(void)
{
    return &axs15231e_jx371_timing;
}

int display_prepare(void)
{
    return axs15231e_jx371_prepare();
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

#endif /* LCD_DSI_AXS15231E_JX371 */
