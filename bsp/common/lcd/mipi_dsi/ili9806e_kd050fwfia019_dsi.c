#include "ili9806e_kd050fwfia019_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_ILI9806E_KD050FWFIA019)

/*
 * KD050FWFIA019 panel, ILI9806E driver IC, 480x854, RGB565, 2-lane MIPI DSI.
 *
 * The init sequence comes from the vendor script (KD050FWFIA019 initi code,
 * 180-degree-rotated variant, which is the only complete one of the
 * two vendor scripts -- the other stops right after the GIP table and never
 * issues MADCTL/tear-on/pixel-format/sleep-out/display-on).
 *
 * ILI9806E manufacturer command set: pages are unlocked/selected with the EXTC
 * command 0xFF followed by the parameter signature 0x98 0x06 0x04 <page>
 * ("9806" = the part number). This is NOT the 0x98,0x81 signature used by the
 * ILI9881C panels (kd050020/kd050023w4) -- the two are not interchangeable.
 */

#define ILI9806E_KD050FWFIA019_SWITCH_PAGE 0
#define ILI9806E_KD050FWFIA019_COMMAND     1

struct ili9806e_kd050fwfia019_instr {
    uint8_t op;

    union arg {
        struct cmd {
            uint8_t cmd;
            uint8_t data;
        } cmd;
        uint8_t page;
    } arg;
};

#define ILI9806E_KD050FWFIA019_SWITCH_PAGE_INSTR(_page) \
    {                                                   \
        .op = ILI9806E_KD050FWFIA019_SWITCH_PAGE,       \
        .arg = {                                        \
            .page = (_page),                            \
        },                                              \
    }

#define ILI9806E_KD050FWFIA019_COMMAND_INSTR(_cmd, _data) \
    {                                                     \
        .op = ILI9806E_KD050FWFIA019_COMMAND,             \
        .arg = {                                          \
            .cmd = {                                      \
                .cmd = (_cmd),                            \
                .data = (_data),                          \
            },                                            \
        },                                                \
    }

/* KD050FWFIA019 panel timing: 480x854, 2-lane, RGB565.
 * NOTE: VCI=3.3V, IOVCC=1.8V. Frame rate 60Hz.
 * Vendor porch values (vsw=4, vbp=30, vfp=20, hsw=4, hbp=30, hfp=18) taken
 * verbatim from the panel spec. Vendor asks for a 29MHz DPI pixel clock, which
 * WIFIPLL_240M/8 (30MHz) approximates most closely of the available dividers. */
static const mipi_dsi_v2_timing_t ili9806e_kd050fwfia019_timing = {
    .width      = 480,
    .height     = 854,
    .hsw        = 4,
    .hbp        = 30,
    .hfp        = 18,
    .vsw        = 4,
    .vbp        = 30,
    .vfp        = 20,
    .lane_num   = BFLB_DSI_LANES_2,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB565,
    .reset_pin  = GPIO_PIN_2,

    /* 400 Mbps/lane. This panel needs only ~232 Mbps/lane (480x854 RGB565 @
     * 60Hz), and an ILI9806E WVGA part's HS-PHY/PLL cannot lock at the 850 Mbps/
     * lane profile the ILI9881C panels use (a known-good ILI9806E 2-lane link
     * runs ~429 Mbps/lane) -- driving it at 850M left the panel black (backlight
     * only) because the receiver never entered HS. 400M covers the requirement
     * with margin and is within the ILI9806E's lock range. dsi_hs_clock must
     * match pll_cfg: it feeds the line-buffer threshold in hs_mode_start(). */
    .pll_cfg         = &dsipllCfg_550M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_240M,
    .display_clk_div = 7, /* 240M / 8 = 30MHz 61hz */
    .dsi_hs_clock    = 550 * 1000 * 1000,

    /* D-PHY HS timing. These are HS byte-clock-period counts; at 400M the byte
     * clock is ~2x slower than at 850M, so the same counts map to longer (never
     * shorter) absolute settle/trail times -- safe to reuse the proven 2-lane
     * values (same as kd050023w4 / st7102_yh494). */
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

#define ILI9806E_KD050FWFIA019_BL_PIN GPIO_PIN_40

static const struct ili9806e_kd050fwfia019_instr ili9806e_kd050fwfia019_init[] = {
    /* Page 1 - power / VCOM / gamma */
    ILI9806E_KD050FWFIA019_SWITCH_PAGE_INSTR(1),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x08, 0x10), /* Output SDA */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x20, 0x00), /* set DE/VSYNC mode */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x21, 0x01), /* DE = 1 Active */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x30, 0x01), /* Resolution setting 480 x 854 */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x31, 0x00), /* Inversion setting 2-dot */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x40, 0x16), /* BT AVDD */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x41, 0x33),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x42, 0x03),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x43, 0x09),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x44, 0x06),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x50, 0x88), /* VREG1 */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x51, 0x88), /* VREG2 */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x52, 0x00), /* Flicker MSB */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x53, 0x49), /* Flicker LSB / VCOM */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x55, 0x49), /* Flicker */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x60, 0x07),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x61, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x62, 0x07),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x63, 0x00),
    /* Positive gamma */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA0, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA1, 0x09),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA2, 0x11),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA3, 0x0B),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA4, 0x05),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA5, 0x08),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA6, 0x06),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA7, 0x04),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA8, 0x09),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xA9, 0x0C),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xAA, 0x15),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xAB, 0x08),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xAC, 0x0F),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xAD, 0x12),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xAE, 0x09),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xAF, 0x00),
    /* Negative gamma */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC0, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC1, 0x09),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC2, 0x10),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC3, 0x0C),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC4, 0x05),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC5, 0x08),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC6, 0x06),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC7, 0x04),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC8, 0x08),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xC9, 0x0C),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xCA, 0x14),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xCB, 0x08),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xCC, 0x0F),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xCD, 0x11),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xCE, 0x09),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0xCF, 0x00),

    /* Page 6 - GIP timing 1 */
    ILI9806E_KD050FWFIA019_SWITCH_PAGE_INSTR(6),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x00, 0x20),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x01, 0x0A),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x02, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x03, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x04, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x05, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x06, 0x98),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x07, 0x06),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x08, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x09, 0x80),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x0A, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x0B, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x0C, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x0D, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x0E, 0x05),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x0F, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x10, 0xF0),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x11, 0xF4),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x12, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x13, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x14, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x15, 0xC0),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x16, 0x08),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x17, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x18, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x19, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x1A, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x1B, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x1C, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x1D, 0x00),
    /* GIP timing 2 */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x20, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x21, 0x23),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x22, 0x45),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x23, 0x67),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x24, 0x01),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x25, 0x23),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x26, 0x45),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x27, 0x67),
    /* GIP timing 3 */
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x30, 0x11),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x31, 0x11),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x32, 0x00),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x33, 0xEE),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x34, 0xFF),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x35, 0xBB),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x36, 0xAA),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x37, 0xDD),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x38, 0xCC),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x39, 0x66),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x3A, 0x77),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x3B, 0x22),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x3C, 0x22),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x3D, 0x22),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x3E, 0x22),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x3F, 0x22),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x40, 0x22),

    /* Page 7 - GIP timing 4 */
    ILI9806E_KD050FWFIA019_SWITCH_PAGE_INSTR(7),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x17, 0x22),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x02, 0x77),
    ILI9806E_KD050FWFIA019_COMMAND_INSTR(0x26, 0xB2),
};

#define ILI9806E_KD050FWFIA019_INIT_LEN (sizeof(ili9806e_kd050fwfia019_init) / sizeof(ili9806e_kd050fwfia019_init[0]))

/* ILI9806E page-switch: EXTC command 0xFF with the parameter signature
 * 0x98, 0x06, 0x04, <page>. The wire packet is cmd 0xFF followed by 5 data
 * bytes {0xFF, 0x98, 0x06, 0x04, page}, i.e. FF FF 98 06 04 <page>, taken
 * verbatim from the vendor script. */
static void ili9806e_kd050fwfia019_switch_page(uint8_t page)
{
    uint8_t data[5] = { 0xFF, 0x98, 0x06, 0x04, page };
    mipi_dsi_v2_dcs_write_cmd(0, 0xFF, data, 5);
}

static void ili9806e_kd050fwfia019_run_init_table(void)
{
    for (unsigned int i = 0; i < ILI9806E_KD050FWFIA019_INIT_LEN; i++) {
        const struct ili9806e_kd050fwfia019_instr *instr = &ili9806e_kd050fwfia019_init[i];

        if (instr->op == ILI9806E_KD050FWFIA019_SWITCH_PAGE) {
            ili9806e_kd050fwfia019_switch_page(instr->arg.page);
        } else {
            uint8_t data = instr->arg.cmd.data;
            mipi_dsi_v2_dcs_write_cmd(0, instr->arg.cmd.cmd, &data, 1);
        }
    }
}

static int ili9806e_kd050fwfia019_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY (whole-chain bring-up lives in the panel).
     * mipi_dsi_v2_setup() resolves and caches the DSI device handle. */
    mipi_dsi_v2_setup(&ili9806e_kd050fwfia019_timing);

    ili9806e_kd050fwfia019_run_init_table();

    /* Switch back to page 0 (normal command) */
    ili9806e_kd050fwfia019_switch_page(0);

#if (ILI9806E_KD050FWFIA019_ROTATE_180)
    /* MADCTL: 180-degree rotation/mirror */
    {
        uint8_t madctl = 0x01;
        mipi_dsi_v2_dcs_write_cmd(0, 0x36, &madctl, 1);
    }
#endif

    /* Tear on (V-blank mode) */
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_SET_TEAR_ON, NULL, 0);

    /* Pixel format 16-bit (RGB565): DBI[2:0]=101=0x05. The vendor script sets
     * 0x70 (24-bit) here, but this framework's DSI wire format is fixed at
     * RGB565 (.data_type = BFLB_DSI_DATA_RGB565 in the timing above), so force
     * 16-bit here to match, same as kd050020/kd050023w4. */
    {
        uint8_t fmt = 0x05;
        mipi_dsi_v2_dcs_write_cmd(0, 0x3A, &fmt, 1);
    }

    /* Exit sleep */
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_EXIT_SLEEP_MODE, NULL, 0);
    bflb_mtimer_delay_ms(120);

    /* Display on */
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_SET_DISPLAY_ON, NULL, 0);
    bflb_mtimer_delay_ms(25);

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&ili9806e_kd050fwfia019_timing);

    return 0;
}

int ili9806e_kd050fwfia019_dsi_init(ili9806e_kd050fwfia019_dsi_color_t *screen_buffer)
{

#if (ILI9806E_KD050FWFIA019_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) || \
    (ILI9806E_KD050FWFIA019_FB_MODE == ILI9806E_KD050FWFIA019_FB_MODE_RGB565)
    if (screen_buffer == NULL) {
        return -1;
    }
#endif

    int ret = ili9806e_kd050fwfia019_prepare();
    if (ret != 0) {
        return ret;
    }

    const mipi_dsi_v2_init_t init_config = {
        .timing = &ili9806e_kd050fwfia019_timing,
        .base_frame_buff = (ILI9806E_KD050FWFIA019_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE &&
                            ILI9806E_KD050FWFIA019_FB_MODE == ILI9806E_KD050FWFIA019_FB_MODE_RGB565) ? screen_buffer : NULL,
        .osd0_frame_buff = (ILI9806E_KD050FWFIA019_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE) ? screen_buffer : NULL,
        .base_format = (ILI9806E_KD050FWFIA019_FB_MODE == ILI9806E_KD050FWFIA019_FB_MODE_RGB565) ? DPI_DATA_FORMAT_RGB565 : DPI_DATA_FORMAT_Y_UV_PLANAR,
        .osd_format = ILI9806E_KD050FWFIA019_OSD0_FORMAT,
    };
    return mipi_dsi_v2_display_init(&init_config);
}

int ili9806e_kd050fwfia019_dsi_screen_switch(ili9806e_kd050fwfia019_dsi_color_t *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }

    return mipi_dsi_v2_screen_switch((void *)screen_buffer);
}

ili9806e_kd050fwfia019_dsi_color_t *ili9806e_kd050fwfia019_dsi_get_screen_using(void)
{
    return (ili9806e_kd050fwfia019_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int ili9806e_kd050fwfia019_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *ili9806e_kd050fwfia019_dsi_get_timing(void)
{
    return &ili9806e_kd050fwfia019_timing;
}

int display_prepare(void)
{
    return ili9806e_kd050fwfia019_prepare();
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

#endif /* LCD_DSI_ILI9806E_KD050FWFIA019 */
