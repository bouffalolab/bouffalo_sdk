/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <stdint.h>

/* buf debug */
#if ENABLE_CODEC_DEBUG_OUTPUT
#define LEN_CODEC_DEBUG_OUTPUT              (8*1024*1024)
uint8_t *g_rawauo_data = (uint8_t *)0x55000000;
uint32_t g_rawauo_data_len = 0;
uint32_t g_rawauo_data_real_len = 0;
#endif

#if ENABLE_CODEC_DEBUG_INPUT
#define LEN_CODEC_DEBUG_INPUT               (8*1024*1024)
uint8_t *g_rawaui_data = (uint8_t *)0x55000000;
uint32_t g_rawaui_data_len = 0;
uint32_t g_rawaui_data_real_len = 0;
#endif

void codec_debug_input(uint32_t type, uint8_t *buf, uint32_t buf_len)
{
#if ENABLE_CODEC_DEBUG_OUTPUT
    if (TYPE_CODEC_DEBUG_OUTPUT == type) {
        if (0 == g_rawauo_data_real_len) {
            if (buf_len + g_rawauo_data_len > LEN_CODEC_DEBUG_OUTPUT) {
                g_rawauo_data_real_len = g_rawauo_data_len;
            } else {
                memcpy(g_rawauo_data + g_rawauo_data_len, (uint8_t*)buf, buf_len);
                g_rawauo_data_len += buf_len;
            }
        }
    }
#endif

#if ENABLE_CODEC_DEBUG_INPUT
    if (TYPE_CODEC_DEBUG_INPUT == type) {
        if (0 == g_rawaui_data_real_len) {
            if (buf_len + g_rawaui_data_len > LEN_CODEC_DEBUG_INPUT) {
                g_rawaui_data_real_len = g_rawaui_data_len;
            } else {
                memcpy(g_rawaui_data + g_rawaui_data_len, (uint8_t*)buf, buf_len);
                g_rawaui_data_len += buf_len;
            }
        }
    }
#endif
}

void msp_clock_tree(void)
{
    {
        int aupll_refclk_sel;
        int aupll_postdiv;

        float audio_pll_clk = 0.0;

        aupll_refclk_sel = ((*((volatile uint32_t *)(0x20000754))) >> 16) & 0x3;
        aupll_postdiv = ((*((volatile uint32_t *)(0x20008754))) >> 0) & 0x7F;
        if (0 != aupll_postdiv) {
            audio_pll_clk = 442.368/(aupll_postdiv);
        }

        MSP_LOGI("XTAL[40M] -> aupll_refclk_sel[%d] -> AUPLL[442.368M] -> aupll_postdiv[div %d] -> audio_pll_clk[%f M]\r\n",
                aupll_refclk_sel, aupll_postdiv, audio_pll_clk);
    }

#if 0
    {
        int dac_rate_reg = 0;

        dac_rate_reg = ((*((volatile uint32_t *)(0x20050000))) >> 24) & 0x7;
        if (4 == dac_rate_reg)
        user_log("Codec dac rate %d\r\n", );
    }
#endif
}
