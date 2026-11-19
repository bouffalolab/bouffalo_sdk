/**
 * @file display_bl618dg_dpu_clock.c
 * @brief Mapping of BL618DG DPU display-link parameters to clock settings.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_clock.h"

int display_bl618dg_dpu_clock_select(uint32_t xclk_hz, uint32_t clock_hz, display_bl618dg_dpu_clk_t *out)
{
    /* Order of preference on equal distance: PLL sources first, the crystal last */
    const struct {
        display_bl618dg_dpu_clk_src_t src;
        uint32_t hz;
    } sources[] = {
        { DISPLAY_BL618DG_DPU_CLK_240M, 240000000u },
        { DISPLAY_BL618DG_DPU_CLK_160M, 160000000u },
        { DISPLAY_BL618DG_DPU_CLK_96M, 96000000u },
        { DISPLAY_BL618DG_DPU_CLK_XCLK, xclk_hz },
    };
    uint32_t best_error = UINT32_MAX;

    if (!out || !clock_hz) {
        return -EINVAL;
    }
    for (unsigned i = 0; i < sizeof(sources) / sizeof(sources[0]); i++) {
        for (uint8_t div = 0; div <= DISPLAY_BL618DG_DPU_CLK_DIV_MAX; div++) {
            uint32_t hz = sources[i].hz / ((uint32_t)div + 1u);
            uint32_t error = hz > clock_hz ? hz - clock_hz : clock_hz - hz;

            if (error < best_error) {
                best_error = error;
                out->src = sources[i].src;
                out->div = div;
                out->hz = hz;
            }
        }
    }
    return 0;
}

int display_bl618dg_dpu_dsipll_step(uint16_t lane_mbps)
{
    static const uint16_t steps[DISPLAY_BL618DG_DPU_DSIPLL_STEPS] = { 400, 500, 550, 650, 750, 850 };

    for (unsigned i = 0; i < sizeof(steps) / sizeof(steps[0]); i++) {
        if (steps[i] == lane_mbps) {
            return (int)i;
        }
    }
    return -ENOTSUP;
}
