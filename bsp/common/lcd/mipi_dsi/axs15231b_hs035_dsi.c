#include "axs15231b_hs035_dsi.h"
#include "mipi_dsi_v2.h"
#include "bflb_mtimer.h"

#if defined(LCD_DSI_AXS15231B_HS035)

/*
 * HS035VB35PGR panel (AXS15231B), 1-lane MIPI DSI, RGB565 link.
 *
 * The panel has firmware-side IC initialization, so there is no vendor register
 * table to send over DSI. The SoC provides RGB565 video timing,
 * plus the board reset/backlight sequence from lcd_conf_user.h.
 *
 * Panel active resolution is 172x640 portrait. The timing porch values come
 * from the panel note; the module firmware handles IC register initialization.
 */
static const mipi_dsi_v2_timing_t axs15231b_hs035_timing = {
    .width      = AXS15231B_HS035_DSI_W,
    .height     = AXS15231B_HS035_DSI_H,
    .hsw        = 10,
    .hbp        = 60,
    .hfp        = 60,
    .vsw        = 4,
    .vbp        = 120,
    .vfp        = 40,
    .lane_num   = BFLB_DSI_LANES_1,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type  = BFLB_DSI_DATA_RGB565,
    .reset_pin  = GPIO_PIN_2,

    .pll_cfg         = &dsipllCfg_850M[GLB_XTAL_40M],
    .esc_clk_sel     = 0,
    .esc_clk_div     = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_160M,
    .display_clk_div = 10, // 14.5Mhz 60fps
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

static int axs15231b_hs035_prepare(void)
{
    /* DSI PLL/clocks + controller/D-PHY. Panel registers are initialized by
     * firmware inside the module, so no panel-specific register table is sent. */
    mipi_dsi_v2_setup(&axs15231b_hs035_timing);

    /* Minimal DCS wake-up still matters even when the panel firmware owns IC
     * registers: leave sleep, select RGB565 payload format, then turn display on. */
    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_EXIT_SLEEP_MODE, NULL, 0);
    bflb_mtimer_delay_ms(120);

    {
        uint8_t fmt = 0x05;
        mipi_dsi_v2_dcs_write_cmd(0, 0x3A, &fmt, 1);
    }

    mipi_dsi_v2_dcs_write_cmd(0, DSI_V2_DCS_SET_DISPLAY_ON, NULL, 0);
    bflb_mtimer_delay_ms(20);

    /* Line buffer threshold + start HS (video) mode */
    mipi_dsi_v2_hs_mode_start(&axs15231b_hs035_timing);

    return 0;
}

int axs15231b_hs035_dsi_init(axs15231b_hs035_dsi_color_t *screen_buffer)
{
    int ret = axs15231b_hs035_prepare();
    if (ret != 0) {
        return ret;
    }
    /* DPI background + OSD0 overlay + OSD SEOF interrupt. screen_buffer is the
     * initial OSD canvas handed down by lcd_init(). */
    return mipi_dsi_v2_display_init(&axs15231b_hs035_timing, (uint32_t)screen_buffer);
}

int axs15231b_hs035_dsi_screen_switch(axs15231b_hs035_dsi_color_t *screen_buffer)
{
    return mipi_dsi_v2_screen_switch((void *)screen_buffer);
}

axs15231b_hs035_dsi_color_t *axs15231b_hs035_dsi_get_screen_using(void)
{
    return (axs15231b_hs035_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int axs15231b_hs035_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *axs15231b_hs035_dsi_get_timing(void)
{
    return &axs15231b_hs035_timing;
}

int display_prepare(void)
{
    return axs15231b_hs035_prepare();
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

#endif /* LCD_DSI_AXS15231B_HS035 */
