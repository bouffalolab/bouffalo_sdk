/**
 * @file display_sim_v1_scanout.h
 * @brief BL616 parallel RGB scanout (SIM v1): RGB565 frames sent by DMA.
 *
 * The SIM engine needs a function table at linker symbol __edata_etext_final;
 * without it configuration returns -ENOTSUP.
 *
 * Two fixed groups of DMA nodes alternate between the active and pending frames.
 * A flip builds and cleans the idle group before publishing it. The DMA frame-end
 * interrupt switches groups and reports FIRST / RETIRED / LAST, or VSYNC when no
 * operation completes. This logical boundary is not a measurement of the VSYNC pin.
 *
 * Config opens the engine without a frame or DMA start. FIRST starts group 0;
 * NULL stops renewal at frame end and reports LAST after close. A later FIRST
 * reopens the engine. Disabled SIM need not produce frame events.
 * Frames are tightly packed RGB565, with a byte size divisible by four.
 * The engine takes an integer frame rate derived from the module clock and timing.
 * BL616 has one SIM engine, so only one owner may use it.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SIM_V1_SCANOUT_H
#define DISPLAY_SIM_V1_SCANOUT_H

#include "scanout/display_scanout_driver.h"
#include "bflb_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup DISPLAY
 * @{ */
/** @defgroup DISPLAY_SCANOUT_SIM_V1 SIM v1 scanout backend (BL616)
 * @{ */

/** @brief Maximum non-active lines (vsw + vbp + vfp). */
#define DISPLAY_SIM_V1_INVALID_ROWS_MAX 300

/**
 * @brief Wiring and DMA resources. Pins are consecutive: DE = pin_clk + 1,
 * latch = pin_clk + 2, VSYNC = pin_hsync + 1, data starts at pin_data.
 * Signal inversion is supplied by display_video_mode_t (HSYNC/VSYNC defaults
 * false, DE true). SIM v1 ignores pclk_invert.
 */
typedef struct {
    const char *dma;                         /**< DMA channel, e.g. "dma0_ch3" */
    struct bflb_dma_channel_lli_pool_s *lli; /**< Two consecutive groups of DMA nodes, 16 bytes per node */
    uint16_t lli_count;                      /**< At least twice the nodes per frame; extra nodes are unused */
    uint8_t pin_clk;                         /**< PCLK pin */
    uint8_t pin_hsync;                       /**< HSYNC pin */
    uint8_t pin_data;                        /**< First data pin */
    bool latch_mode;                         /**< 8 data lines and an external latch; otherwise 16 lines */
    bool de;                                 /**< Mux the DE pin; independent of its polarity */
} display_sim_v1_cfg_t;

/** @brief SIM v1 scanout object; state is private to the backend. */
typedef struct {
    display_scanout_t base; /**< Must be first */
    display_sim_v1_cfg_t cfg;
    struct bflb_device_s *dma;
    uint16_t nodes_per_frame;
    int8_t active_lli; /**< Active group 0/1, or -1 before FIRST / after LAST */
    bool opened;
    volatile display_frame_op_t submitted_op; /**< Ready operation, published only after the idle group is built */
    struct bflb_dma_channel_lli_pool_s head;  /**< Shared pre-frame control node, loaded through CPU registers */
    struct bflb_dma_channel_lli_pool_s tail;  /**< Shared post-frame control node */
    uint32_t invalid_rows[(DISPLAY_SIM_V1_INVALID_ROWS_MAX + 1) * 2];
} display_sim_v1_scanout_t;

extern const display_scanout_ops_t display_sim_v1_scanout_ops;

/**
 * @brief Static definition. Each of the two groups needs
 * ceil(width * height * 2 / 16128) nodes. Frame storage is supplied by flip.
 * @param ... Designated initializers of display_sim_v1_cfg_t.
 */
#define DISPLAY_SIM_V1_SCANOUT(...)                                                              \
    {                                                                                            \
        .base = DISPLAY_SCANOUT_BASE(&display_sim_v1_scanout_ops, .format = DISPLAY_FMT_RGB565), \
        .cfg = { __VA_ARGS__ }                                                                   \
    }

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_SIM_V1_SCANOUT_H */