/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#if 0
#include <bl616_platform.h>

#include <msp/kernel.h>

#include <FreeRTOS.h>
#include <task.h>

extern BL_Err_Type GLB_Config_AUDIO_PLL_To_491P52M(void);

// audio pll config
void msp_config_audiopll(void)
{
    GLB_Config_AUDIO_PLL_To_491P52M();
}
#endif
