#include "fl7707n_hd395003c30_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_FL7707N_HD395003C30)

/*
 * HD395003C30-V2 (FL7707N driver IC), 720x720, 2-lane MIPI DSI.
 *
 * The init sequence comes from the vendor script
 * 2_FL7707N_QV040YNQ-N80_IPS_Code_2Power_V5.5_20230810.txt, which is written as
 * DSI_CMD(<total length>, <cmd>) followed by DSI_PA(<param>) lines. The leading
 * length byte counts the command itself, so DSI_CMD(0x04, 0xB9) means 0xB9 plus
 * three parameters; the tables below carry the command in payload[0] the same way
 * the other panels in this directory do, and the lengths were cross-checked
 * against the DSI_PA counts.
 *
 * 0xB9 = F1 12 87 is the vendor unlock password; nothing after it takes effect
 * without it. 0xBA parameter 1 = 0x31 declares the 2-lane wiring to the IC
 * itself (0x32 / 0x33 would be 3 / 4 lane), so it must stay in step with
 * .lane_num below. 0xB8 parameter 1 = 0x26 selects the 2-power-mode variant this
 * script is named for.
 *
 * Everything is sent as auto-typed DCS (short-write / short-write-param /
 * long-write by payload length). Unlike JD9365TX_7KF82, the FL7707N script gives
 * no indication that its vendor registers need generic-long packets; if a board
 * turns out to require them, switch the run loop to force
 * DSI_V2_GENERIC_LONG_WRITE for the 0xB0..0xEF range the way jd9365tx_7kf82 does.
 *
 * Panel reset is driven by the LCD framework (LCD_RESET_* in lcd_conf_user.h)
 * before this init runs.
 */

/* op codes in the init table */
#define FL7707N_HD395003C30_OP_CMD   0 /* send payload[0]=cmd + rest=data via mipi_dsi_v2 */
#define FL7707N_HD395003C30_OP_DELAY 1 /* delay .len milliseconds */

struct fl7707n_hd395003c30_instr {
    uint8_t op;          /* FL7707N_HD395003C30_OP_xxx */
    uint8_t len;         /* payload length (for CMD) or delay ms (for DELAY) */
    uint8_t payload[64]; /* payload[0] is the command byte; longest is 0xE9 (1+63) */
};

#define FL7707N_HD395003C30_CMD(...)                                   \
    {                                                                  \
        .op = FL7707N_HD395003C30_OP_CMD,                              \
        .len = sizeof((uint8_t[]){ __VA_ARGS__ }),                     \
        .payload = { __VA_ARGS__ },                                    \
    }

#define FL7707N_HD395003C30_DELAY(ms)                                  \
    {                                                                  \
        .op = FL7707N_HD395003C30_OP_DELAY,                            \
        .len = (ms),                                                   \
    }

/* HD395003C30-V2 panel timing (board-provided):
 *   PCLK 38 MHz, HSA 2 / HBP 44 / HFP 46, VSA 5 / VBP 16 / VFP 50
 *   htotal 720 + 2 + 44 + 46 = 812, vtotal 720 + 5 + 16 + 50 = 791
 *   812 * 791 = 642292 clk/frame -> 59.2 Hz at 38 MHz
 *
 * The display clock tree only offers WIFIPLL 96/160/240 MHz divided by 1..16, so
 * 38 MHz is not available: 240 / 6 = 40 MHz is the closest tap and lands the frame
 * rate at 62.3 Hz. Blanking is left exactly as specified rather than stretched to
 * pull that back to 60 Hz.
 *
 * Note the vendor script's own VFP/VBP (0xB3 = 10/10 plus DE 28/28) are the
 * panel-internal RGB generator settings, not the DSI link blanking configured
 * here; the two are independent.
 *
 * RGB888 over 2 lanes needs 40 MHz * 24 / 2 = 480 Mbps/lane, so the 550 MHz HS bit
 * clock leaves ~14% headroom. */
static const mipi_dsi_v2_timing_t fl7707n_hd395003c30_timing = {
    .width      = FL7707N_HD395003C30_DSI_W,
    .height     = FL7707N_HD395003C30_DSI_H,
    .hsw        = 2,
    .hbp        = 44,
    .hfp        = 46,
    .vsw        = 5,
    .vbp        = 16,
    .vfp        = 50,
    .lane_num   = BFLB_DSI_LANES_2, /* must stay in step with 0xBA param 1 = 0x31 */
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB888,
    .reset_pin  = GPIO_PIN_2,

    /* pll */
    .pll_cfg         = &dsipllCfg_550M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_240M,
    .display_clk_div = 5, /* 240 MHz / (5 + 1) = 40 MHz pixel clock */
    .dsi_hs_clock    = 550 * 1000 * 1000,

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

#define FL7707N_HD395003C30_BL_PIN GPIO_PIN_40 /* backlight enable (driven high, same board as KD050) */

/* Full init sequence (vendor 2_FL7707N_QV040YNQ-N80_IPS_Code_2Power_V5.5). */
static const struct fl7707n_hd395003c30_instr fl7707n_hd395003c30_init[] = {
    FL7707N_HD395003C30_CMD(0xB9, 0xF1, 0x12, 0x87), /* SETEXTC: vendor unlock password */
    FL7707N_HD395003C30_CMD(0xB2, 0xB4, 0x03, 0x70), /* Set Display */

    /* Set RGB: VBP/VFP/DE_BP/DE_FP of the panel-internal RGB generator */
    FL7707N_HD395003C30_CMD(0xB3, 0x10, 0x10, 0x28, 0x28, 0x03, 0xFF, 0x00, 0x00,
                            0x00, 0x00),

    FL7707N_HD395003C30_CMD(0xB4, 0x80),             /* Set Panel Inversion */
    FL7707N_HD395003C30_CMD(0xB5, 0x0A, 0x0A),       /* Set BGP: vref / nvref */
    FL7707N_HD395003C30_CMD(0xB6, 0x8D, 0x8D),       /* Set VCOM: F_VCOM / B_VCOM */
    FL7707N_HD395003C30_CMD(0xB8, 0x26, 0x22, 0xF0, 0x13), /* 0x26 = 2-power mode */

    /* Set DSI: param 1 = 0x31 selects 2-lane (0x32 = 3-lane, 0x33 = 4-lane);
     * param 18 = 0x91 is the vendor's ESD-hardened value (default 0x90). */
    FL7707N_HD395003C30_CMD(0xBA, 0x31, 0x81, 0x05, 0xF9, 0x0E, 0x0E, 0x20, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x25, 0x00,
                            0x91, 0x0A, 0x00, 0x00, 0x01, 0x4F, 0x01, 0x00, 0x00,
                            0x37),

    FL7707N_HD395003C30_CMD(0xBC, 0x47),                         /* Set VDC */
    FL7707N_HD395003C30_CMD(0xBF, 0x02, 0x10, 0x00, 0x80, 0x04), /* Set PCR */

    /* Set SCR */
    FL7707N_HD395003C30_CMD(0xC0, 0x73, 0x73, 0x50, 0x50, 0x00, 0x00, 0x12, 0x73,
                            0x00),

    /* Set POWER: VBTHS/VBTLS, VSPR, VSNR, VSP/VSN, APS */
    FL7707N_HD395003C30_CMD(0xC1, 0x36, 0x00, 0x32, 0x32, 0x77, 0xE1, 0x77, 0x77,
                            0xCC, 0xCC, 0xFF, 0xFF, 0x11, 0x11, 0x00, 0x00, 0x32),

    /* VOUT / detection-driven TE masking (param 9 = 0xED) */
    FL7707N_HD395003C30_CMD(0xC7, 0x10, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0xED, 0xC5, 0x00, 0xA5),

    FL7707N_HD395003C30_CMD(0xC8, 0x10, 0x40, 0x1E, 0x03),
    FL7707N_HD395003C30_CMD(0xCC, 0x0B), /* Set Panel: 0x0B forward, 0x07 backward */

    /* Set Gamma: 17 positive followed by 17 negative points */
    FL7707N_HD395003C30_CMD(0xE0, 0x00, 0x0A, 0x0F, 0x2A, 0x33, 0x3F, 0x44, 0x39,
                            0x06, 0x0C, 0x0E, 0x14, 0x15, 0x13, 0x15, 0x10, 0x18,
                            0x00, 0x0A, 0x0F, 0x2A, 0x33, 0x3F, 0x44, 0x39, 0x06,
                            0x0C, 0x0E, 0x14, 0x15, 0x13, 0x15, 0x10, 0x18),

    FL7707N_HD395003C30_CMD(0xE1, 0x11, 0x11, 0x91, 0x00, 0x00, 0x00, 0x00),

    /* Set EQ: param 13 = 0xC0 keeps the ESD detect function on */
    FL7707N_HD395003C30_CMD(0xE3, 0x07, 0x07, 0x0B, 0x0B, 0x0B, 0x0B, 0x00, 0x00,
                            0x00, 0x00, 0xFF, 0x04, 0xC0, 0x10),

    /* Set GIP */
    FL7707N_HD395003C30_CMD(0xE9, 0xC8, 0x10, 0x0A, 0x00, 0x00, 0x80, 0x81, 0x12,
                            0x31, 0x23, 0x4F, 0x86, 0xA0, 0x00, 0x47, 0x08, 0x00,
                            0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00,
                            0x00, 0x00, 0x98, 0x02, 0x8B, 0xAF, 0x46, 0x02, 0x88,
                            0x88, 0x88, 0x88, 0x88, 0x98, 0x13, 0x8B, 0xAF, 0x57,
                            0x13, 0x88, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00),

    /* Set GIP2 */
    FL7707N_HD395003C30_CMD(0xEA, 0x97, 0x0C, 0x09, 0x09, 0x09, 0x78, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x9F, 0x31, 0x8B, 0xA8, 0x31,
                            0x75, 0x88, 0x88, 0x88, 0x88, 0x88, 0x9F, 0x20, 0x8B,
                            0xA8, 0x20, 0x64, 0x88, 0x88, 0x88, 0x88, 0x88, 0x23,
                            0x00, 0x00, 0x02, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x40, 0x80, 0x81, 0x00, 0x00, 0x00, 0x00),

    FL7707N_HD395003C30_CMD(0xEF, 0xFF, 0xFF, 0x01),

    FL7707N_HD395003C30_CMD(0x11), /* Exit Sleep */
    FL7707N_HD395003C30_DELAY(250),
    FL7707N_HD395003C30_CMD(0x29), /* Display On */
    FL7707N_HD395003C30_DELAY(50),
};

#define FL7707N_HD395003C30_INIT_LEN \
    (sizeof(fl7707n_hd395003c30_init) / sizeof(fl7707n_hd395003c30_init[0]))

static void fl7707n_hd395003c30_run_init_table(void)
{
    for (unsigned int i = 0; i < FL7707N_HD395003C30_INIT_LEN; i++) {
        const struct fl7707n_hd395003c30_instr *instr = &fl7707n_hd395003c30_init[i];

        if (instr->op == FL7707N_HD395003C30_OP_DELAY) {
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

static int fl7707n_hd395003c30_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&fl7707n_hd395003c30_timing);

    /* Panel register sequence (sent over LPDT in LP mode) */
    fl7707n_hd395003c30_run_init_table();

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&fl7707n_hd395003c30_timing);

    return 0;
}

int fl7707n_hd395003c30_dsi_init(fl7707n_hd395003c30_dsi_color_t *screen_buffer)
{

#if (FL7707N_HD395003C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) || \
    (FL7707N_HD395003C30_FB_MODE == FL7707N_HD395003C30_FB_MODE_RGB565)
    if (screen_buffer == NULL) {
        return -1;
    }
#endif

    int ret = fl7707n_hd395003c30_prepare();
    if (ret != 0) {
        return ret;
    }

    const mipi_dsi_v2_init_t init_config = {
        .timing = &fl7707n_hd395003c30_timing,
        .base_frame_buff = (FL7707N_HD395003C30_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE &&
                            FL7707N_HD395003C30_FB_MODE == FL7707N_HD395003C30_FB_MODE_RGB565) ? screen_buffer : NULL,
        .osd0_frame_buff = (FL7707N_HD395003C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) ? screen_buffer : NULL,
        .base_format = (FL7707N_HD395003C30_FB_MODE == FL7707N_HD395003C30_FB_MODE_RGB565) ? DPI_DATA_FORMAT_RGB565 : DPI_DATA_FORMAT_Y_UV_PLANAR,
        .osd_format = FL7707N_HD395003C30_OSD0_FORMAT,
    };
    return mipi_dsi_v2_display_init(&init_config);
}

int fl7707n_hd395003c30_dsi_screen_switch(fl7707n_hd395003c30_dsi_color_t *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }

    return mipi_dsi_v2_screen_switch(screen_buffer);
}

fl7707n_hd395003c30_dsi_color_t *fl7707n_hd395003c30_dsi_get_screen_using(void)
{
    return (fl7707n_hd395003c30_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int fl7707n_hd395003c30_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *fl7707n_hd395003c30_dsi_get_timing(void)
{
    return &fl7707n_hd395003c30_timing;
}

int display_prepare(void)
{
    return fl7707n_hd395003c30_prepare();
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

#endif /* LCD_DSI_FL7707N_HD395003C30 */
