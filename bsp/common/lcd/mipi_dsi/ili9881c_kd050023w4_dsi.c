#include "ili9881c_kd050023w4_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_ILI9881C_KD050023W4)

#define ILI9881C_KD050023W4_SWITCH_PAGE 0
#define ILI9881C_KD050023W4_COMMAND     1

struct ili9881c_kd050023w4_instr {
    uint8_t op;

    union arg {
        struct cmd {
            uint8_t cmd;
            uint8_t data;
        } cmd;
        uint8_t page;
    } arg;
};

#define ILI9881C_KD050023W4_SWITCH_PAGE_INSTR(_page) \
    {                                              \
        .op = ILI9881C_KD050023W4_SWITCH_PAGE,       \
        .arg = {                                   \
            .page = (_page),                       \
        },                                         \
    }

#define ILI9881C_KD050023W4_COMMAND_INSTR(_cmd, _data) \
    {                                                \
        .op = ILI9881C_KD050023W4_COMMAND,             \
        .arg = {                                     \
            .cmd = {                                 \
                .cmd = (_cmd),                       \
                .data = (_data),                     \
            },                                       \
        },                                           \
    }

/* KD050HDFIA023-W4 panel timing: 720x1280, 2-lane, RGB565.
 * Porch values reuse the proven 720p (020) numbers; the vendor init script has no
 * porch parameters. */
static const mipi_dsi_v2_timing_t ili9881c_kd050023w4_timing = {
    .width      = 720,
    .height     = 1280,
    .hsw        = 6,
    .hbp        = 15,
    .hfp        = 46,
    .vsw        = 6,
    .vbp        = 3,
    .vfp        = 0,
    .lane_num   = BFLB_DSI_LANES_2,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB565,
    .reset_pin  = GPIO_PIN_2,

    .pll_cfg         = &dsipllCfg_850M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_240M,
    .display_clk_div = 3,
    .dsi_hs_clock    = 850 * 1000 * 1000,

    // .pll_cfg         = &dsipllCfg_400M[GLB_XTAL_40M],
    // .esc_clk_sel     = 0,
    // .esc_clk_div     = 0,
    // .display_clk_sel = GLB_DP_CLK_WIFIPLL_160M,
    // .display_clk_div = 4,
    // .dsi_hs_clock    = 400 * 1000 * 1000,

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

#define ILI9881C_KD050023W4_BL_PIN GPIO_PIN_40

static const struct ili9881c_kd050023w4_instr ili9881c_kd050023w4_init[] = {
    /* Page 3 - GIP 1 */
    ILI9881C_KD050023W4_SWITCH_PAGE_INSTR(3),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x01, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x02, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x03, 0x72),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x04, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x05, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x06, 0x09),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x07, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x08, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x09, 0x01),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x0a, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x0b, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x0c, 0x01),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x0d, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x0e, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x0f, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x10, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x11, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x12, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x13, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x14, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x15, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x16, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x17, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x18, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x19, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x1a, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x1b, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x1c, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x1d, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x1e, 0x40),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x1f, 0x80),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x20, 0x05),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x21, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x22, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x23, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x24, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x25, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x26, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x27, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x28, 0x33),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x29, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x2a, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x2b, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x2c, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x2d, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x2e, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x2f, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x30, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x31, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x32, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x33, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x34, 0x04),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x35, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x36, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x37, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x38, 0x3C),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x39, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x3a, 0x40),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x3b, 0x40),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x3c, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x3d, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x3e, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x3f, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x40, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x41, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x42, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x43, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x44, 0x00),
    /* GIP 2 */
    ILI9881C_KD050023W4_COMMAND_INSTR(0x50, 0x01),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x51, 0x23),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x52, 0x45),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x53, 0x67),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x54, 0x89),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x55, 0xab),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x56, 0x01),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x57, 0x23),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x58, 0x45),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x59, 0x67),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x5a, 0x89),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x5b, 0xab),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x5c, 0xcd),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x5d, 0xef),
    /* GIP 3 */
    ILI9881C_KD050023W4_COMMAND_INSTR(0x5e, 0x11),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x5f, 0x01),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x60, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x61, 0x15),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x62, 0x14),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x63, 0x0E),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x64, 0x0F),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x65, 0x0C),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x66, 0x0D),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x67, 0x06),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x68, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x69, 0x07),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6a, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6b, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6c, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6d, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6e, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6f, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x70, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x71, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x72, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x73, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x74, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x75, 0x01),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x76, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x77, 0x14),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x78, 0x15),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x79, 0x0E),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x7a, 0x0F),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x7b, 0x0C),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x7c, 0x0D),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x7d, 0x06),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x7e, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x7f, 0x07),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x80, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x81, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x83, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x84, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x85, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x86, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x87, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x88, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x89, 0x02),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x8A, 0x02),

    /* Page 4 - power / ESD */
    ILI9881C_KD050023W4_SWITCH_PAGE_INSTR(4),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6C, 0x15),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6E, 0x2A),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x6F, 0x33),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x3A, 0x94),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x8D, 0x15),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x87, 0xBA),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x26, 0x76),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xB2, 0xD1),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xB5, 0x06),

    /* Page 1 - display timing + gamma */
    ILI9881C_KD050023W4_SWITCH_PAGE_INSTR(1),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x22, 0x0A),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x31, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x53, 0xA5),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x55, 0xA2),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x50, 0xB7),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x51, 0xB7),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x60, 0x22),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x61, 0x00),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x62, 0x19),
    ILI9881C_KD050023W4_COMMAND_INSTR(0x63, 0x10),
    /* Positive gamma */
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA0, 0x08),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA1, 0x17),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA2, 0x1E),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA3, 0x0E),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA4, 0x13),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA5, 0x24),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA6, 0x1B),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA7, 0x1B),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA8, 0x53),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xA9, 0x1B),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xAA, 0x28),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xAB, 0x45),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xAC, 0x1A),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xAD, 0x1A),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xAE, 0x50),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xAF, 0x21),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xB0, 0x2C),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xB1, 0x3B),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xB2, 0x63),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xB3, 0x39),
    /* Negative gamma */
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC0, 0x08),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC1, 0x0C),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC2, 0x17),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC3, 0x0F),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC4, 0x0B),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC5, 0x1C),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC6, 0x10),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC7, 0x16),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC8, 0x5B),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xC9, 0x1A),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xCA, 0x26),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xCB, 0x55),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xCC, 0x1D),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xCD, 0x1E),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xCE, 0x52),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xCF, 0x26),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xD0, 0x29),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xD1, 0x45),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xD2, 0x63),
    ILI9881C_KD050023W4_COMMAND_INSTR(0xD3, 0x39),
};

#define ILI9881C_KD050023W4_INIT_LEN (sizeof(ili9881c_kd050023w4_init) / sizeof(ili9881c_kd050023w4_init[0]))

static void ili9881c_kd050023w4_switch_page(uint8_t page)
{
    uint8_t buf[4] = { 0xFF, 0x98, 0x81, page };
    mipi_dsi_v2_dcs_write_cmd(0, buf[0], &buf[1], 3);
}

static void ili9881c_kd050023w4_run_init_table(void)
{
    for (unsigned int i = 0; i < ILI9881C_KD050023W4_INIT_LEN; i++) {
        const struct ili9881c_kd050023w4_instr *instr = &ili9881c_kd050023w4_init[i];

        if (instr->op == ILI9881C_KD050023W4_SWITCH_PAGE) {
            ili9881c_kd050023w4_switch_page(instr->arg.page);
        } else {
            uint8_t data = instr->arg.cmd.data;
            mipi_dsi_v2_dcs_write_cmd(0, instr->arg.cmd.cmd, &data, 1);
        }
    }
}

static int ili9881c_kd050023w4_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&ili9881c_kd050023w4_timing);

    ili9881c_kd050023w4_run_init_table();

    /* Switch back to page 0 */
    ili9881c_kd050023w4_switch_page(0);

    /* Tear on (V-blank mode) */
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_SET_TEAR_ON, NULL, 0);

    /* Pixel format 16-bit (RGB565): DBI[2:0]=101=0x05 */
    {
        uint8_t fmt = 0x05;
        mipi_dsi_v2_dcs_write_cmd(0, 0x3A, &fmt, 1);
    }

    /* Exit sleep */
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_EXIT_SLEEP_MODE, NULL, 0);
    bflb_mtimer_delay_ms(120);

    /* Display on */
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_SET_DISPLAY_ON, NULL, 0);

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&ili9881c_kd050023w4_timing);

    return 0;
}

int ili9881c_kd050023w4_dsi_init(ili9881c_kd050023w4_dsi_color_t *screen_buffer)
{
    int ret = ili9881c_kd050023w4_prepare();
    if (ret != 0) {
        return ret;
    }
    /* DPI background + OSD0 overlay + OSD SEOF interrupt. screen_buffer is the
     * initial OSD canvas handed down by lcd_init(). */
    return mipi_dsi_v2_display_init(&ili9881c_kd050023w4_timing, (uint32_t)screen_buffer);
}

int ili9881c_kd050023w4_dsi_screen_switch(ili9881c_kd050023w4_dsi_color_t *screen_buffer)
{
    return mipi_dsi_v2_screen_switch((void *)screen_buffer);
}

ili9881c_kd050023w4_dsi_color_t *ili9881c_kd050023w4_dsi_get_screen_using(void)
{
    return (ili9881c_kd050023w4_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int ili9881c_kd050023w4_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *ili9881c_kd050023w4_dsi_get_timing(void)
{
    return &ili9881c_kd050023w4_timing;
}

int display_prepare(void)
{
    return ili9881c_kd050023w4_prepare();
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

#endif /* LCD_DSI_ILI9881C_KD050023W4 */
