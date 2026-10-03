#ifndef __MIPI_DSI_V2_H__
#define __MIPI_DSI_V2_H__

/*
 * mipi_dsi_v2 - low-level MIPI DSI helpers for BL618DG video-mode panels.
 *
 * This is a sibling of mipi_dsi.c (the Linux/DRM-style DCS compatibility HAL).
 * It carries the chip-specific pieces that the generic HAL deliberately does
 * not: the DSI PLL clock table, direct PHY analog register bring-up, a
 * GPIO-based panel reset, and a timing-aware controller init driven from a
 * per-panel timing descriptor.
 *
 * Ported from chiptest/bl616d/dsi/common/dsi_common.{c,h}; the MJPEG decoder
 * helpers that lived in dsi_common are intentionally left out of this module.
 */

#include <stdint.h>
#include "bflb_dsi.h"
#include "bflb_dpi.h"
#include "bflb_gpio.h"
#include "bl618dg_glb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MIPI DSI data types */
#define DSI_V2_DCS_SHORT_WRITE       0x05
#define DSI_V2_DCS_SHORT_WRITE_PARAM 0x15
#define DSI_V2_DCS_LONG_WRITE        0x39
#define DSI_V2_DCS_READ              0x06
#define DSI_V2_SET_MAX_RET_PKT_SIZE  0x37
#define DSI_V2_GENERIC_LONG_WRITE    0x29

/* MIPI DCS commands */
#define DSI_V2_DCS_SOFT_RESET        0x01
#define DSI_V2_DCS_ENTER_SLEEP_MODE  0x10
#define DSI_V2_DCS_EXIT_SLEEP_MODE   0x11
#define DSI_V2_DCS_SET_TEAR_ON       0x35
#define DSI_V2_DCS_SET_DISPLAY_ON    0x29

#define DSI_V2_TEAR_MODE_VBLANK      0

#define DSI_V2_CTRL_BASE             0x2004C000

/* Panel timing / lane / clock descriptor consumed by mipi_dsi_v2_*.
 *
 * Everything DSI-link-specific is driven from this struct so a new panel only
 * needs its own instance — nothing in mipi_dsi_v2.c is hard-coded to a lane
 * count, a D-PHY timing set, or a PLL frequency. */
typedef struct {
    /* display timing */
    uint16_t width;
    uint16_t height;
    uint16_t hsw;
    uint16_t hbp;
    uint16_t hfp;
    uint16_t vsw;
    uint16_t vbp;
    uint16_t vfp;

    /* lane / format */
    uint8_t  lane_num;   /* BFLB_DSI_LANES_x (1 / 2 / 4) */
    uint8_t  lane_order; /* BFLB_DSI_LANE_ORDER_xxx */
    uint8_t  data_type;  /* BFLB_DSI_DATA_xxx (e.g. BFLB_DSI_DATA_RGB565) */
    uint8_t  reset_pin;  /* GPIO pin used for the panel reset line, or external */

    /* PLL / clock tree (all panel-determined) */
    const GLB_DSIPLL_Cfg_Type *pll_cfg; /* resolved DSI PLL config for the board xtal */
    uint8_t  esc_clk_sel;               /* GLB_Set_DSI_ESC_CLK sel */
    uint8_t  esc_clk_div;               /* GLB_Set_DSI_ESC_CLK div */
    uint8_t  display_clk_sel;           /* GLB_Set_Display_CLK clkSel (GLB_DP_CLK_xxx) */
    uint8_t  display_clk_div;           /* GLB_Set_Display_CLK div; DPI pixel clock is derived from this at runtime */
    uint32_t dsi_hs_clock;              /* DSI HS bit clock (Hz), for line buffer threshold */

    /* D-PHY HS timing (panel/clock-rate specific) */
    bflb_dsi_dphy_config_t dphy;
} mipi_dsi_v2_timing_t;

#define MIPI_DSI_V2_RESET_EXTERNAL UINT8_MAX

/* DSI PLL clock table (BL618DG specific). 800 MHz VCO / bitclk_div 2 = 400 MHz HS
 * bit clock (200 MHz DDR clock lane, 400 Mbps/lane). A panel may point its
 * timing.pll_cfg at &dsipllCfg_400M[GLB_XTAL_xxx] or supply its own. */
extern const GLB_DSIPLL_CFG_BASIC_Type dsipll400MCfg_40M;
extern const GLB_DSIPLL_Cfg_Type dsipllCfg_400M[GLB_XTAL_MAX];

/* 1000 MHz VCO / bitclk_div 2 = 500 MHz HS bit clock (250 MHz DDR clock lane,
 * 500 Mbps/lane). Same chain as the 400M table, just a higher VCO. */
extern const GLB_DSIPLL_CFG_BASIC_Type dsipll500MCfg_40M;
extern const GLB_DSIPLL_Cfg_Type dsipllCfg_500M[GLB_XTAL_MAX];

extern const GLB_DSIPLL_CFG_BASIC_Type dsipll550MCfg_40M;
extern const GLB_DSIPLL_Cfg_Type dsipllCfg_550M[GLB_XTAL_MAX];

extern const GLB_DSIPLL_CFG_BASIC_Type dsipll650MCfg_40M;
extern const GLB_DSIPLL_Cfg_Type dsipllCfg_650M[GLB_XTAL_MAX];

extern const GLB_DSIPLL_CFG_BASIC_Type dsipll750MCfg_40M;
extern const GLB_DSIPLL_Cfg_Type dsipllCfg_750M[GLB_XTAL_MAX];

extern const GLB_DSIPLL_CFG_BASIC_Type dsipll850MCfg_40M;
extern const GLB_DSIPLL_Cfg_Type dsipllCfg_850M[GLB_XTAL_MAX];

/* Panel reset is driven by the LCD framework (LCD_RESET_* in lcd_conf_user.h),
 * which pulses the reset GPIO at the start of lcd_init() before this setup.
 * Configure LCD_RESET_PIN / LCD_RESET_ACTIVE_LEVEL / LCD_RESET_HOLD_MS /
 * LCD_RESET_DELAY per panel there. */

/* One-shot DSI bring-up: power on the DSI PLL, enable ESC/display clocks, then
 * init the DSI controller and D-PHY (reset, enable, D-PHY timing, lane enable),
 * all from the panel's timing descriptor. Call once before the panel register
 * init sequence; start HS (video) mode separately after that. Caches the DSI
 * device handle internally, so the other calls below take no device argument. */
void mipi_dsi_v2_setup(const mipi_dsi_v2_timing_t *cfg);

/* Configure the DSI line-buffer threshold for the panel and start HS (video)
 * mode. Call after the panel register init sequence completes. */
void mipi_dsi_v2_hs_mode_start(const mipi_dsi_v2_timing_t *cfg);

/* Stop HS (video) mode. Returns the underlying bflb_dsi_phy_hs_mode_stop()
 * status (0 on success). Use before re-init / sleep / panel swap. */
int mipi_dsi_v2_hs_mode_stop(void);

/* Tear down the DSI controller and power off the DSI PLL. Inverse of
 * mipi_dsi_v2_setup(). */
void mipi_dsi_v2_deinit(void);

/* Largest DCS payload mipi_dsi_v2_dcs_write_cmd() can stage on the stack
 * (1 command byte + up to MAX-1 data bytes). */
#define DSI_V2_DCS_WRITE_MAX_LEN 63

/* DCS / generic write. The wire payload is always 'cmd' followed by 'len' data
 * bytes. 'data_type' selects how it is sent:
 *   - data_type == 0: auto-pick the DCS packet type by length
 *       (len==0 short-write, len==1 short-write-param, len>1 long-write).
 *   - data_type != 0: force this explicit type as a long packet (e.g.
 *       DSI_V2_GENERIC_LONG_WRITE 0x29, which some vendor panels require even
 *       for short payloads -- no length-based downgrade).
 * Returns 0 on success, -1 on bad args, -2 if len exceeds DSI_V2_DCS_WRITE_MAX_LEN. */
int mipi_dsi_v2_dcs_write_cmd(uint8_t data_type, uint8_t cmd, const uint8_t *data, uint16_t len);

/* ---------- Frame-buffer switch (shared by every DSI v2 panel) ----------
 *
 * A DSI v2 panel runs in HS video mode: the DPI controller continuously scans a
 * framebuffer out over the DSI link, and the LVGL canvas is composited on top by
 * the OSD0 blend layer (set up by the application's dpi_manager). "Switching the
 * screen" means re-pointing the OSD0 blend layer at the freshly rendered canvas;
 * the swap latches at the next frame boundary (SEOF). An OSD interrupt fires on
 * every SEOF. CYCLE is reported on every frame; SWAP is reported only after a
 * submitted canvas crosses a subsequent frame boundary. get_screen_using()
 * reports that completed canvas, so the LVGL port can safely reuse the old one.
 *
 * This is the DSI sibling of bl_mipi_dpi_v2_screen_switch(): the panel-specific
 * <panel>_dsi_screen_switch / _get_screen_using / _frame_callback_register
 * wrappers all forward here, exactly as the DPI panels forward to
 * lcd_mipi_dpi_* (see standard_dpi.c).
 *
 * The OSD canvas is the caller's buffer; screen_switch() cleans its D-cache
 * before programming the new address. */

/* Frame interrupt callback types (mirror the DPI framework's CYCLE/SWAP split). */
#define MIPI_DSI_V2_FRAME_INT_TYPE_CYCLE 0
#define MIPI_DSI_V2_FRAME_INT_TYPE_SWAP  1

#define MIPI_DSI_V2_OSD_FORMAT_RGB565   0
#define MIPI_DSI_V2_OSD_FORMAT_ARGB8888 1
#define MIPI_DSI_V2_OSD_FORMAT_NONE     2

typedef struct {
    const mipi_dsi_v2_timing_t *timing;
    void *base_frame_buff; /* Optional RGB565 base framebuffer. YUV base frames are
                              supplied later with bflb_dpi_framebuffer_planar_switch(). */
    void *osd0_frame_buff; /* OSD0 framebuffer, required unless OSD0_FORMAT_NONE. */
    uint8_t base_format; /* DPI base format: RGB565 or Y_UV_PLANAR */
    uint8_t osd_format;  /* OSD0 canvas format: RGB565, ARGB8888 or NONE */
} mipi_dsi_v2_init_t;

/* One-shot bring-up of the DSI scan-out side, called from the panel's _dsi_init()
 * after the panel link is up:
 *   [1] DPI background scan-out layer (RGB565 or YUV planar; the framebuffer
 *       address starts at 0 and the app switches real frames later through the
 *       bflb_dpi_framebuffer_switch/planar_switch APIs)
 *   [2] OSD0 RGB565/ARGB8888 blend overlay, or a transparent 1x1 OSD0 when NONE
 *   [3] OSD0 SEOF interrupt driving the frame callbacks
 * init_config supplies panel timing, independent base/OSD0 buffers, and both
 * formats. The base address is programmed after DPI setup through the DPI
 * framebuffer switch API; YUV base buffers are always supplied by the caller
 * with bflb_dpi_framebuffer_planar_switch().
 * Returns 0 on success, -1 if the DPI/OSD device is unavailable, -2 for invalid
 * configuration. The configuration is consumed during this call only. */
int mipi_dsi_v2_display_init(const mipi_dsi_v2_init_t *init_config);

/* Attach + enable the OSD SEOF interrupt that drives the frame callbacks. Called
 * by mipi_dsi_v2_display_init(); exposed for callers that set up the OSD blend
 * layer themselves. 'osd' is that OSD device (OSD0). */
void mipi_dsi_v2_osd_irq_init(struct bflb_device_s *osd);

/* Re-point the selected OSD0 or RGB565 base buffer at screen_buffer. Non-blocking:
 * the swap latches at the next SEOF and the registered SWAP callback fires from the OSD ISR.
 * Returns 0 on success, -1 on NULL buffer, -2 if the OSD IRQ was not initialized
 * (mipi_dsi_v2_osd_irq_init not called). */
int mipi_dsi_v2_screen_switch(void *screen_buffer);

/* The framebuffer most recently handed to mipi_dsi_v2_screen_switch(), or NULL
 * before the first switch. */
void *mipi_dsi_v2_get_screen_using(void);

/* Register a frame callback, invoked from the OSD SEOF ISR. SWAP and CYCLE are
 * both supported (mirrors the DPI framework). callback==NULL clears. Returns 0. */
int mipi_dsi_v2_frame_callback_register(uint32_t callback_type, void (*callback)(void));

/* Base (DPI background) layer swap, invoked from the OSD SEOF ISR on every frame
 * boundary. Weak no-op by default: OSD0-only LVGL apps need no override (the base
 * layer just scans black). An app driving a video background overrides this with a
 * strong definition to latch the next decoded YUV frame into the DPI base layer
 * between scanouts (see dpi_manager.c). Mirrors bl_mipi_dpi_v2_base_layer_swap(). */
void mipi_dsi_v2_osd0_base_layer_swap(void);

#ifdef __cplusplus
}
#endif

#endif /* __MIPI_DSI_V2_H__ */
