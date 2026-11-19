/**
 * @file display_bl618dg_dpu_clock.h
 * @brief Mapping of BL618DG DPU display-link parameters to clock settings.
 *
 * Pure functions without hardware access, shared by the DPU scanout and the DSI host. A module gives a
 * clock in Hz and a lane rate in Mbit/s; the register values follow from these functions.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_BL618DG_DPU_CLOCK_H
#define DISPLAY_BL618DG_DPU_CLOCK_H

#include "display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_SCANOUT
 * @{
 */

/**
 * @defgroup DISPLAY_BL618DG_DPU_CLOCK BL618DG DPU display clocks
 * @{
 */

/** @brief Source of the DPU display clock. The values are the selector values of the hardware. */
typedef enum {
    DISPLAY_BL618DG_DPU_CLK_XCLK = 0, /**< XCLK (crystal) */
    DISPLAY_BL618DG_DPU_CLK_160M = 1, /**< 160 MHz */
    DISPLAY_BL618DG_DPU_CLK_240M = 2, /**< 240 MHz */
    DISPLAY_BL618DG_DPU_CLK_96M = 3   /**< 96 MHz */
} display_bl618dg_dpu_clk_src_t;

/** @brief Largest divider value of the DPU display clock; the clock is `source / (div + 1)`. */
#define DISPLAY_BL618DG_DPU_CLK_DIV_MAX 15

/**
 * @brief DPU display clock setting.
 */
typedef struct {
    display_bl618dg_dpu_clk_src_t src; /**< Source */
    uint8_t div;                       /**< Divider value, 0 to @ref DISPLAY_BL618DG_DPU_CLK_DIV_MAX */
    uint32_t hz;                       /**< Resulting clock, Hz */
} display_bl618dg_dpu_clk_t;

/**
 * @brief Choose the DPU display clock closest to a requested clock.
 *
 * Every source and divider is tried. On equal distance the faster source wins, so the crystal is used
 * only when it is strictly closer.
 *
 * @param [in]  xclk_hz  frequency of XCLK
 * @param [in]  clock_hz requested clock, Hz
 * @param [out] out      setting
 * @retval 0 success
 * @retval -EINVAL @p out is NULL, or @p clock_hz is 0
 */
int display_bl618dg_dpu_clock_select(uint32_t xclk_hz, uint32_t clock_hz, display_bl618dg_dpu_clk_t *out);

/** @brief Number of DSI PLL steps. */
#define DISPLAY_BL618DG_DPU_DSIPLL_STEPS 6

/**
 * @brief Find the DSI PLL step for a lane rate.
 *
 * The rate of a DSI link is part of the panel timing, so it is not rounded: only the steps of the PLL
 * (400, 500, 550, 650, 750 and 850 Mbit/s) are accepted.
 *
 * @param [in] lane_mbps bit rate of one data lane, Mbit/s
 * @return index of the step (0 to @ref DISPLAY_BL618DG_DPU_DSIPLL_STEPS - 1, in ascending order of the rate), or
 *         -ENOTSUP when the rate is not a step
 */
int display_bl618dg_dpu_dsipll_step(uint16_t lane_mbps);

/** @} */ /* DISPLAY_BL618DG_DPU_CLOCK */
/** @} */ /* DISPLAY_SCANOUT */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_BL618DG_DPU_CLOCK_H */
