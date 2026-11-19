#include "st7102_ydp395bt008_dsi.h"
#include "bflb_mtimer.h"
#include <stdio.h>

#if defined(LCD_DSI_ST7102_YDP395BT008)

/* op codes in the init table */
#define ST7102_YDP395BT008_OP_CMD   0 /* send payload[0]=cmd + rest=data via mipi_dsi_v2 */
#define ST7102_YDP395BT008_OP_DELAY 1 /* delay .len milliseconds */

struct st7102_ydp395bt008_instr {
    uint8_t op;          /* ST7102_YDP395BT008_OP_xxx */
    uint8_t len;         /* payload length (for CMD) or delay ms (for DELAY) */
    uint8_t payload[52]; /* command byte followed by vendor register data */
};

#define ST7102_YDP395BT008_CMD(...)                  \
    {                                              \
        .op = ST7102_YDP395BT008_OP_CMD,             \
        .len = sizeof((uint8_t[]){ __VA_ARGS__ }),   \
        .payload = { __VA_ARGS__ },                 \
    }

#define ST7102_YDP395BT008_DELAY(ms)                \
    {                                              \
        .op = ST7102_YDP395BT008_OP_DELAY,           \
        .len = (ms),                               \
    }

/* OSPTEK module timing: 30 MHz / (742 x 692) = 58.4 Hz.
 * The SDK 650 Mbps PLL preset replaces the vendor ESP32-P4 620 Mbps rate. */
static const mipi_dsi_v2_timing_t panel_timing = {
    .width = ST7102_YDP395BT008_DSI_W,
    .height = ST7102_YDP395BT008_DSI_H,
    .hsw = 2,
    .hbp = 140,
    .hfp = 120,
    .vsw = 4,
    .vbp = 8,
    .vfp = 200,
    .lane_num = BFLB_DSI_LANES_2,
    .lane_order = BFLB_DSI_LANE_ORDER_3210,
    .data_type = BFLB_DSI_DATA_RGB888,
    .reset_pin = GPIO_PIN_18,
    .pll_cfg = &dsipllCfg_650M[GLB_XTAL_40M],
    .esc_clk_sel = 0,
    .esc_clk_div = 0,
    .display_clk_sel = GLB_DP_CLK_WIFIPLL_240M,
    .display_clk_div = 7, /* 240 MHz / (7 + 1) = 30 MHz */
    .dsi_hs_clock = 650000000,
    .dphy = {
        .time_clk_exit = 5,
        .time_clk_trail = 3,
        .time_clk_zero = 0xf,
        .time_data_exit = 5,
        .time_data_prepare = 1,
        .time_data_trail = 3,
        .time_data_zero = 6,
        .time_lpx = 3,
        .time_req_ready = 0,
        .time_ta_get = 0x13,
        .time_ta_go = 0xf,
        .time_wakeup = 0x9c41,
    },
};

/* YDP395BT008-V1 vendor initialization. Keep command order and delays intact. */
static const struct st7102_ydp395bt008_instr st7102_ydp395bt008_init[] = {
    /* OSPTEK YDP395BT008-V1 sequence, adapted from the ESP32-P4 example.
     * Copyright 2026 OSPTEK; CC BY 4.0: https://creativecommons.org/licenses/by/4.0/
     * https://gitee.com/osptek/tft-3.95-480x480-mipi-st7102
     * Source revision: f786bbb0ca24250df567dcc9707ca3cf8020a98e
     * Adaptation: DCS writes/delays expressed using this driver's table macros.
     * RGB888 setup mirrors esp_lcd_st7102_mipi.c before the module table. */
    ST7102_YDP395BT008_CMD(0xF0, 0x00),
#if ST7102_YDP395BT008_RGB_ORDER_MODE
    ST7102_YDP395BT008_CMD(0x36, 0x08),
#else
    ST7102_YDP395BT008_CMD(0x36, 0x00),
#endif
    ST7102_YDP395BT008_CMD(0x3A, 0x77), /* RGB888 on the DSI link */

    ST7102_YDP395BT008_CMD(0x99, 0x71, 0x02, 0xa2),
    ST7102_YDP395BT008_CMD(0x99, 0x71, 0x02, 0xa3),
    ST7102_YDP395BT008_CMD(0x99, 0x71, 0x02, 0xa4),
    ST7102_YDP395BT008_CMD(0xB0, 0x22, 0x61, 0x1E, 0x61, 0x2F, 0x39, 0x39),
    ST7102_YDP395BT008_CMD(0xB7, 0x46, 0x46),
    ST7102_YDP395BT008_CMD(0xBF, 0x50, 0x50),
    ST7102_YDP395BT008_CMD(0xD7, 0x00, 0x0C, 0x80, 0x08, 0xF0, 0xF0),
    ST7102_YDP395BT008_CMD(0xA3, 0x40, 0x03, 0x80, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00,
                             0x04, 0x6F, 0x6F, 0x00, 0x1A, 0x00, 0x45, 0x05, 0x00, 0x00,
                             0x00, 0x00, 0x46, 0x00, 0x00, 0x12, 0x20, 0x52, 0x00, 0x05,
                             0x00, 0x00, 0xFF),
    ST7102_YDP395BT008_CMD(0xA6, 0x08, 0x00, 0x24, 0x55, 0x35, 0x00, 0x76, 0x40, 0x58,
                             0x58, 0x00, 0x24, 0x55, 0x00, 0x00, 0x40, 0x40, 0x58, 0x58,
                             0x02, 0xAC, 0x51, 0x00, 0xCC, 0x40, 0x40, 0x58, 0x58, 0x00,
                             0xAC, 0x11, 0x00, 0x00, 0x40, 0x40, 0x58, 0x58, 0x00, 0x00,
                             0x06, 0x00, 0x00, 0x00, 0x00),
    ST7102_YDP395BT008_CMD(0xA7, 0x19, 0x19, 0x00, 0x64, 0x40, 0x07, 0x16, 0x40, 0x00,
                             0x44, 0x43, 0x58, 0x58, 0x00, 0x64, 0x40, 0x25, 0x34, 0x00,
                             0x00, 0x42, 0x41, 0x58, 0x58, 0x00, 0x64, 0x40, 0x4B, 0x5A,
                             0x00, 0x00, 0x42, 0x41, 0x58, 0x58, 0x00, 0x24, 0x40, 0x69,
                             0x78, 0x00, 0x00, 0x40, 0x40, 0x58, 0x58, 0x00, 0x44),
    ST7102_YDP395BT008_CMD(0xAC, 0x00, 0x1C, 0x04, 0x1A, 0x19, 0x1B, 0x1B, 0x18, 0x06,
                             0x13, 0x19, 0x11, 0x1B, 0x08, 0x18, 0x0A, 0x01, 0x1C, 0x04,
                             0x1A, 0x19, 0x01, 0x1B, 0x18, 0x06, 0x12, 0x19, 0x10, 0x1B,
                             0x09, 0x18, 0x0B, 0xBF, 0xAA, 0xBF, 0xAA, 0x00),
    ST7102_YDP395BT008_CMD(0xAD, 0xCC, 0x40, 0x46, 0x11, 0x04, 0x6F, 0x6F),
    ST7102_YDP395BT008_CMD(0xE8, 0x30, 0x07, 0x05, 0x74, 0x74, 0x9C, 0x00, 0xE2, 0x04,
                             0x00, 0x00, 0x00, 0x00, 0xEF),
    ST7102_YDP395BT008_CMD(0x75, 0x03, 0x04),
    ST7102_YDP395BT008_CMD(0xE7, 0x8B, 0x3C, 0x00, 0x0C, 0xF0, 0x5D, 0x00, 0x5D, 0x00,
                             0x5D, 0x00, 0x5D, 0x00, 0xFF, 0x00, 0x08, 0x7B, 0x00, 0x00,
                             0xC8, 0x6A, 0x5A, 0x08, 0x1A, 0x3C, 0x00, 0xA1, 0x01, 0x8C,
                             0x01, 0x7F, 0xF0, 0x22),
    ST7102_YDP395BT008_CMD(0xE9, 0x3C, 0x7F, 0x08, 0x07, 0x1A, 0x7A, 0x22, 0x1A, 0x33),
    ST7102_YDP395BT008_CMD(0xC8, 0x00, 0x00, 0x15, 0x26, 0x44, 0x00, 0x78, 0x03, 0xBE,
                             0x06, 0x11, 0x1C, 0x09, 0x8A, 0x03, 0x21, 0xD4, 0x01, 0x11,
                             0x0F, 0x22, 0x4A, 0x0F, 0x8F, 0x0A, 0x32, 0xF0, 0x0A, 0x41,
                             0x0D, 0xF3, 0x80, 0x0D, 0xAE, 0xC5, 0x03, 0xC4),
    ST7102_YDP395BT008_CMD(0xC9, 0x00, 0x00, 0x15, 0x26, 0x44, 0x00, 0x78, 0x03, 0xBE,
                             0x06, 0x11, 0x1C, 0x09, 0x8A, 0x03, 0x21, 0xD4, 0x01, 0x11,
                             0x0F, 0x22, 0x4A, 0x0F, 0x8F, 0x0A, 0x32, 0xF0, 0x0A, 0x41,
                             0x0D, 0xF3, 0x80, 0x0D, 0xAE, 0xC5, 0x03, 0xC4),
    ST7102_YDP395BT008_CMD(0x11),
    ST7102_YDP395BT008_DELAY(120),
    ST7102_YDP395BT008_CMD(0x29),
    ST7102_YDP395BT008_DELAY(20),
};

#define ST7102_YDP395BT008_INIT_LEN \
    (sizeof(st7102_ydp395bt008_init) / sizeof(st7102_ydp395bt008_init[0]))

static int st7102_ydp395bt008_run_init_table(void)
{
    for (unsigned int i = 0; i < ST7102_YDP395BT008_INIT_LEN; i++) {
        const struct st7102_ydp395bt008_instr *instr = &st7102_ydp395bt008_init[i];

        if (instr->op == ST7102_YDP395BT008_OP_DELAY) {
            bflb_mtimer_delay_ms(instr->len);
        } else {
            /* Let the common helper choose the DCS packet type by length. */
            int ret = mipi_dsi_v2_dcs_write_cmd(0, instr->payload[0],
                                               (instr->len > 1) ? &instr->payload[1] : NULL,
                                               instr->len - 1);
            if (ret < 0) {
                printf("ST7102: init[%u] cmd=0x%02x send failed: %d\r\n",
                       i, instr->payload[0], ret);
                return ret;
            }
        }
    }
    return 0;
}

static int panel_start_video(void)
{
    struct bflb_device_s *dsi = bflb_device_get_by_name("dsi");
    uint32_t data_state = 0;
    uint32_t clock_state = 0;
    uint64_t start_us = bflb_mtimer_get_time_us();

    if (dsi == NULL) {
        return -1;
    }

    /* LHAL hs_mode_start() checks STOP once and silently skips enabling TX
     * when the last LP command is still in BRIDGE. Wait for LP completion. */
    do {
        bflb_dsi_phy_get_lanes_state(dsi, BFLB_DSI_LANE_DATA0, &data_state);
        bflb_dsi_phy_get_lanes_state(dsi, BFLB_DSI_LANE_CLOCK, &clock_state);
        if (data_state == BFLB_DSI_LANE_STATE_STOP &&
            clock_state == BFLB_DSI_LANE_STATE_STOP) {
            break;
        }
        if (bflb_mtimer_get_time_us() - start_us >= 10000U) {
            printf("ST7102: HS start timed out waiting for STOP: D0=0x%lx CLK=0x%lx\r\n",
                   (unsigned long)data_state, (unsigned long)clock_state);
            return -1;
        }
        bflb_mtimer_delay_us(10);
    } while (1);

    mipi_dsi_v2_hs_mode_start(&panel_timing);
    /* BL618DG LHAL enables HS TX using bit 0 of the DSI configuration register.
     * Read the controller at its device address, rather than inferring TX from
     * local scan interrupts or the LHAL data-lane HS state helper. */
    uint32_t config = getreg32(dsi->reg_base);
    if ((config & 1U) == 0) {
        printf("ST7102: HS TX did not enable, DSI config=0x%08lx\r\n", (unsigned long)config);
        return -1;
    }
    return 0;
}

static int panel_prepare(void)
{
    if (bflb_device_get_by_name("dsi") == NULL) {
        return -1;
    }

#ifdef ST7102_YDP395BT008_TOUCH_RESET_PIN
    /* The vendor identifies ST7123 as TDDI: release touch reset before LCD
     * commands even when the application does not use the touch interface. */
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    if (gpio == NULL) {
        return -1;
    }
    bflb_gpio_init(gpio, ST7102_YDP395BT008_TOUCH_RESET_PIN,
                   GPIO_OUTPUT | GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_set(gpio, ST7102_YDP395BT008_TOUCH_RESET_PIN);
    bflb_mtimer_delay_ms(10);
#endif
    mipi_dsi_v2_setup(&panel_timing);
    int ret = st7102_ydp395bt008_run_init_table();
    if (ret < 0) {
        return ret;
    }
    return panel_start_video();
}

int st7102_ydp395bt008_dsi_init(st7102_ydp395bt008_dsi_color_t *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }

    int ret = panel_prepare();
    if (ret < 0) {
        return ret;
    }

    const mipi_dsi_v2_init_t init_config = {
        .timing = &panel_timing,
#if ST7102_YDP395BT008_FB_MODE == ST7102_YDP395BT008_FB_MODE_RGB565
        /* RGB565 is scanned from the DPI base layer. The common helper
         * provides the transparent OSD layer used for frame interrupts. */
        .base_frame_buff = screen_buffer,
        .base_format = DPI_DATA_FORMAT_RGB565,
        .osd_format = MIPI_DSI_V2_OSD_FORMAT_NONE,
#else
        /* ARGB8888 is composited by OSD0; DSI output remains RGB888. */
        .osd0_frame_buff = screen_buffer,
        .base_format = DPI_DATA_FORMAT_Y_UV_PLANAR,
        .osd_format = MIPI_DSI_V2_OSD_FORMAT_ARGB8888,
#endif
    };
    return mipi_dsi_v2_display_init(&init_config);
}

int st7102_ydp395bt008_dsi_screen_switch(st7102_ydp395bt008_dsi_color_t *screen_buffer)
{
    return mipi_dsi_v2_screen_switch(screen_buffer);
}

st7102_ydp395bt008_dsi_color_t *st7102_ydp395bt008_dsi_get_screen_using(void)
{
    return (st7102_ydp395bt008_dsi_color_t *)mipi_dsi_v2_get_screen_using();
}

int st7102_ydp395bt008_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return mipi_dsi_v2_frame_callback_register(callback_type, callback);
}

const mipi_dsi_v2_timing_t *st7102_ydp395bt008_dsi_get_timing(void)
{
    return &panel_timing;
}

#endif /* LCD_DSI_ST7102_YDP395BT008 */
