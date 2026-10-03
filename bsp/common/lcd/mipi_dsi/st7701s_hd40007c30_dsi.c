#include "st7701s_hd40007c30_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_ST7701S_HD40007C30)

/*
 * HD40007C30-V5 (ST7701S driver IC), 480x480, 2-lane MIPI DSI.
 *
 * The init sequence comes from the vendor script
 * ST7701S_HSD3.95IPS(HSD040BPN1)480x480_V1.0.txt. That script is written in
 * WriteComm/WriteData form (one command byte followed by its parameters), which
 * maps directly onto auto-typed DCS writes here -- no generic-long-write quirks
 * like ST7102_YH494 has.
 *
 * ST7701S uses a Command2 bank scheme: 0xFF 0x77 0x01 0x00 0x00 <bank> selects
 * BK0 (0x10) / BK1 (0x11) / BK3 (0x13), and <bank> 0x00 leaves Command2 so the
 * standard DCS registers (0x11, 0x29, 0x36, 0x3A) are reachable again. The bank
 * switches are kept in place and commented so the table stays diffable against
 * the vendor script.
 *
 * Two deviations from the vendor script:
 *   - 0x3A is sent as 0x77 (24bpp) instead of the vendor 0x66 (18bpp), to match
 *     the RGB888 DSI link this driver configures. The vendor script itself has
 *     0x3A=0x77 present but commented out in the BK0 section, so both values are
 *     sanctioned by the panel vendor.
 *   - The trailing bare `WriteComm(0x2A)` is dropped. 0x2A is column-address-set
 *     and needs four parameters; with none it is a vendor typo, and in DSI video
 *     mode the DPI scans the framebuffer continuously so no address window is
 *     needed at all.
 *
 * Panel reset is driven by the LCD framework (LCD_RESET_* in lcd_conf_user.h)
 * before this init runs.
 */

/* op codes in the init table */
#define ST7701S_HD40007C30_OP_CMD   0 /* send payload[0]=cmd + rest=data via mipi_dsi_v2 */
#define ST7701S_HD40007C30_OP_DELAY 1 /* delay .len milliseconds */

struct st7701s_hd40007c30_instr {
    uint8_t op;          /* ST7701S_HD40007C30_OP_xxx */
    uint8_t len;         /* payload length (for CMD) or delay ms (for DELAY) */
    uint8_t payload[17]; /* payload[0] is the command byte; longest is 0xED (1+16) */
};

#define ST7701S_HD40007C30_CMD(...)                                    \
    {                                                                  \
        .op = ST7701S_HD40007C30_OP_CMD,                               \
        .len = sizeof((uint8_t[]){ __VA_ARGS__ }),                     \
        .payload = { __VA_ARGS__ },                                    \
    }

#define ST7701S_HD40007C30_DELAY(ms)                                   \
    {                                                                  \
        .op = ST7701S_HD40007C30_OP_DELAY,                             \
        .len = (ms),                                                   \
    }

/* Command2 bank select: 0xFF 0x77 0x01 0x00 0x00 <bank> */
#define ST7701S_HD40007C30_BANK(bank) ST7701S_HD40007C30_CMD(0xFF, 0x77, 0x01, 0x00, 0x00, (bank))

/* HD40007C30-V5 panel timing (board-provided):
 *   PCLK 19 MHz, HSA 2 / HBP 44 / HFP 46, VSA 5 / VBP 16 / VFP 50
 *   htotal 480 + 2 + 44 + 46 = 572, vtotal 480 + 5 + 16 + 50 = 551
 *   572 * 551 = 315172 clk/frame -> 60.3 Hz at 19 MHz
 *
 * The display clock tree only offers WIFIPLL 96/160/240 MHz divided by 1..16, so
 * 19 MHz is not exact: 96 / 5 = 19.2 MHz is the closest tap and lands the frame
 * rate at 60.9 Hz. Blanking is left exactly as specified rather than stretched to
 * trim that 1.5%.
 *
 * RGB888 over 2 lanes needs 19.2 MHz * 24 / 2 = 230 Mbps/lane, so the 400 MHz HS
 * bit clock has ample headroom. */
static const mipi_dsi_v2_timing_t st7701s_hd40007c30_timing = {
    .width      = ST7701S_HD40007C30_DSI_W,
    .height     = ST7701S_HD40007C30_DSI_H,
    .hsw        = 2,
    .hbp        = 44,
    .hfp        = 46,
    .vsw        = 5,
    .vbp        = 16,
    .vfp        = 50,
    .lane_num   = BFLB_DSI_LANES_2,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB888,
    .reset_pin  = GPIO_PIN_2,

    /* pll */
    .pll_cfg         = &dsipllCfg_400M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_96M,
    .display_clk_div = 4, /* 96 MHz / (4 + 1) = 19.2 MHz pixel clock */
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

#define ST7701S_HD40007C30_BL_PIN GPIO_PIN_40 /* backlight enable (driven high, same board as KD050) */

/* Full init sequence (vendor ST7701S_HSD3.95IPS(HSD040BPN1)480x480_V1.0.txt).
 * Commands the vendor left commented out (0xCD, the BK0 0x3A, 0x21) are omitted
 * here as well, except where noted in the file header. */
static const struct st7701s_hd40007c30_instr st7701s_hd40007c30_init[] = {
    ST7701S_HD40007C30_BANK(0x13),   /* Command2 BK3 */
    ST7701S_HD40007C30_CMD(0xEF, 0x08),

    ST7701S_HD40007C30_BANK(0x10),         /* Command2 BK0 */
    ST7701S_HD40007C30_CMD(0xC0, 0x3B, 0x00), /* Display Line Setting: (0x3B+1)*8 = 480 */
    ST7701S_HD40007C30_CMD(0xC1, 0x0D, 0x02), /* Porch Control: VBP 13, VFP 2 (panel-internal) */
    ST7701S_HD40007C30_CMD(0xC2, 0x21, 0x08), /* Inversion selection (2-dot) & Frame Rate Control */

    /* Positive / negative gamma */
    ST7701S_HD40007C30_CMD(0xB0, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08,
                           0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18),
    ST7701S_HD40007C30_CMD(0xB1, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08,
                           0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18),

    ST7701S_HD40007C30_BANK(0x11),   /* Command2 BK1 */
    ST7701S_HD40007C30_CMD(0xB0, 0x60), /* VOP amplitude */
    ST7701S_HD40007C30_CMD(0xB1, 0x30), /* VCOM amplitude */
    ST7701S_HD40007C30_CMD(0xB2, 0x87), /* VGH voltage */
    ST7701S_HD40007C30_CMD(0xB3, 0x80), /* TEST command */
    ST7701S_HD40007C30_CMD(0xB5, 0x49), /* VGL voltage */
    ST7701S_HD40007C30_CMD(0xB7, 0x85), /* Power control 1 */
    ST7701S_HD40007C30_CMD(0xB8, 0x21), /* Power control 2 */
    ST7701S_HD40007C30_CMD(0xC1, 0x78), /* Source pre_drive timing set 1 */
    ST7701S_HD40007C30_CMD(0xC2, 0x78), /* Source pre_drive timing set 2 */
    ST7701S_HD40007C30_DELAY(20),

    /* GIP / gate timing */
    ST7701S_HD40007C30_CMD(0xE0, 0x00, 0x1B, 0x02),
    ST7701S_HD40007C30_CMD(0xE1, 0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00,
                           0x00, 0x44, 0x44),
    ST7701S_HD40007C30_CMD(0xE2, 0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00,
                           0xEC, 0xA0, 0x00, 0x00),
    ST7701S_HD40007C30_CMD(0xE3, 0x00, 0x00, 0x11, 0x11),
    ST7701S_HD40007C30_CMD(0xE4, 0x44, 0x44),
    ST7701S_HD40007C30_CMD(0xE5, 0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0,
                           0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0),
    ST7701S_HD40007C30_CMD(0xE6, 0x00, 0x00, 0x11, 0x11),
    ST7701S_HD40007C30_CMD(0xE7, 0x44, 0x44),
    ST7701S_HD40007C30_CMD(0xE8, 0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0,
                           0x0D, 0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0),
    ST7701S_HD40007C30_CMD(0xEB, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40),
    ST7701S_HD40007C30_CMD(0xEC, 0x3C, 0x00),
    ST7701S_HD40007C30_CMD(0xED, 0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF,
                           0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA),
    ST7701S_HD40007C30_CMD(0xEF, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F),

    ST7701S_HD40007C30_BANK(0x00),   /* leave Command2 */

    /* Vendor sleep-out wrapper: BK3 0xE8 gates the source output around the
     * 0x11 exit-sleep so the panel does not glitch while power ramps. */
    ST7701S_HD40007C30_BANK(0x13),
    ST7701S_HD40007C30_CMD(0xE8, 0x00, 0x0E),
    ST7701S_HD40007C30_BANK(0x00),
    ST7701S_HD40007C30_CMD(0x11), /* Exit Sleep */
    ST7701S_HD40007C30_DELAY(120),
    ST7701S_HD40007C30_BANK(0x13),
    ST7701S_HD40007C30_CMD(0xE8, 0x00, 0x0C),
    ST7701S_HD40007C30_DELAY(10),
    ST7701S_HD40007C30_CMD(0xE8, 0x00, 0x00),
    ST7701S_HD40007C30_BANK(0x00),

#if (ST7701S_HD40007C30_RGB_ORDER_MODE)
    ST7701S_HD40007C30_CMD(0x36, 0x08), /* Memory Data Access Control: B-G-R */
#else
    ST7701S_HD40007C30_CMD(0x36, 0x00), /* Memory Data Access Control: R-G-B */
#endif
    ST7701S_HD40007C30_CMD(0x3A, 0x77), /* Interface Pixel Format: 24bpp (vendor 0x66 was 18bpp) */

    ST7701S_HD40007C30_CMD(0x29), /* Display On */
    ST7701S_HD40007C30_DELAY(20),
};

#define ST7701S_HD40007C30_INIT_LEN \
    (sizeof(st7701s_hd40007c30_init) / sizeof(st7701s_hd40007c30_init[0]))

static void st7701s_hd40007c30_run_init_table(void)
{
    for (unsigned int i = 0; i < ST7701S_HD40007C30_INIT_LEN; i++) {
        const struct st7701s_hd40007c30_instr *instr = &st7701s_hd40007c30_init[i];

        if (instr->op == ST7701S_HD40007C30_OP_DELAY) {
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

static int st7701s_hd40007c30_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&st7701s_hd40007c30_timing);

    /* Panel register sequence (sent over LPDT in LP mode) */
    st7701s_hd40007c30_run_init_table();

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&st7701s_hd40007c30_timing);

    return 0;
}

int st7701s_hd40007c30_dsi_init(st7701s_hd40007c30_dsi_color_t *screen_buffer)
{

#if (ST7701S_HD40007C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) || \
    (ST7701S_HD40007C30_FB_MODE == ST7701S_HD40007C30_FB_MODE_RGB565)
    if (screen_buffer == NULL) {
        return -1;
    }
#endif

    int ret = st7701s_hd40007c30_prepare();
    if (ret != 0) {
        return ret;
    }

    const mipi_dsi_v2_init_t init_config = {
        .timing = &st7701s_hd40007c30_timing,
        .base_frame_buff = (ST7701S_HD40007C30_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE &&
                            ST7701S_HD40007C30_FB_MODE == ST7701S_HD40007C30_FB_MODE_RGB565) ? screen_buffer : NULL,
        .osd0_frame_buff = (ST7701S_HD40007C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) ? screen_buffer : NULL,
        .base_format = (ST7701S_HD40007C30_FB_MODE == ST7701S_HD40007C30_FB_MODE_RGB565) ? DPI_DATA_FORMAT_RGB565 : DPI_DATA_FORMAT_Y_UV_PLANAR,
        .osd_format = ST7701S_HD40007C30_OSD0_FORMAT,
    };
    return mipi_dsi_v2_display_init(&init_config);
}

int st7701s_hd40007c30_dsi_screen_switch(st7701s_hd40007c30_dsi_color_t *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }

    return mipi_dsi_v2_screen_switch(screen_buffer);
}

st7701s_hd40007c30_dsi_color_t *st7701s_hd40007c30_dsi_get_screen_using(void)
{
    return (st7701s_hd40007c30_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int st7701s_hd40007c30_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *st7701s_hd40007c30_dsi_get_timing(void)
{
    return &st7701s_hd40007c30_timing;
}

int display_prepare(void)
{
    return st7701s_hd40007c30_prepare();
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

#endif /* LCD_DSI_ST7701S_HD40007C30 */
