/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include <stdio.h>
#include <stdint.h>
#include "avutil/av_config.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

// CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT
NOINLINE_FUNC int config_av_stream_cache_size_default(void)
{
#ifdef CONFIG_CHIP_BL606P_E907
    return 512000;
#else
    return 10*1024;
#endif
}

// CONFIG_PLAYER_TASK_STACK_SIZE
NOINLINE_FUNC int config_player_task_stack_size(void)
{
#ifdef CONFIG_CHIP_BL606P_E907
    return 98304;
#else
    return 20480;
#endif
}

// CONFIG_WEB_CACHE_TASK_STACK_SIZE
NOINLINE_FUNC int config_web_cache_task_stack_size(void)
{
#ifdef CONFIG_CHIP_BL606P_E907
    return 6144;
#else
    return 6144;
#endif
}

//CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT
NOINLINE_FUNC int config_av_stream_cache_threshold_default(void)
{
#ifdef CONFIG_CHIP_BL606P_E907
    return 5;
#else
    return 80; // 80%
#endif
}

//CONFIG_AV_PROBE_SIZE_MAX
NOINLINE_FUNC int config_av_probe_size_max(void)
{
#ifdef CONFIG_CHIP_BL606P_E907
    return 2048;
#else
    return 2048;
#endif
}

//CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX
NOINLINE_FUNC int config_av_sample_num_per_frame_max(void)
{
#ifdef CONFIG_CHIP_BL606P_E907
    return 320;
#else
    return 320;
#endif
}

//CONFIG_ATEMPOER_SONIC
NOINLINE_FUNC int config_av_atempoer_enable(void)
{
#if defined(CONFIG_ATEMPOER_SONIC) && CONFIG_ATEMPOER_SONIC
    return 1;
#else
    return 0;
#endif
}

//CONFIG_AO_MIXER_SUPPORT
NOINLINE_FUNC int config_av_mixer_enable(void)
{
#if defined(CONFIG_AO_MIXER_SUPPORT) && CONFIG_AO_MIXER_SUPPORT
    return 1;
#else
    return 0;
#endif
}

//CONFIG_AEF_EQ_ENABLE
NOINLINE_FUNC int config_av_aef_enable(void)
{
#if defined(CONFIG_AEF_EQ_ENABLE) && CONFIG_AEF_EQ_ENABLE
    return 1;
#else
    return 0;
#endif
}

#endif
