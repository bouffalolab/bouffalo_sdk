/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include <stdio.h>
#include <stdint.h>

#if __has_include("avutil/av_config.h")
#include "avutil/av_config.h"
#endif

#ifdef CONFIG_CHIP_BL606P_E907
#ifndef CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT
#define CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT          (512000)
#endif

#ifndef CONFIG_PLAYER_TASK_STACK_SIZE
#define CONFIG_PLAYER_TASK_STACK_SIZE                (96*1024)
#endif

#ifndef CONFIG_WEB_CACHE_TASK_STACK_SIZE
#define CONFIG_WEB_CACHE_TASK_STACK_SIZE             (6144)
#endif

#ifndef CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT
#define CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT     (5)
#endif

#ifndef CONFIG_AV_PROBE_SIZE_MAX
#define CONFIG_AV_PROBE_SIZE_MAX                     (2048)
#endif

#ifndef CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX
#define CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX           (320)
#endif
#else
#ifndef CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT
#define CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT          (10*1024)
#endif

#ifndef CONFIG_PLAYER_TASK_STACK_SIZE
#define CONFIG_PLAYER_TASK_STACK_SIZE                (20*1024)
#endif

#ifndef CONFIG_WEB_CACHE_TASK_STACK_SIZE
#define CONFIG_WEB_CACHE_TASK_STACK_SIZE             (6144)
#endif

#ifndef CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT
#define CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT     (80)
#endif

#ifndef CONFIG_AV_PROBE_SIZE_MAX
#define CONFIG_AV_PROBE_SIZE_MAX                     (2048)
#endif

#ifndef CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX
#define CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX           (320)
#endif
#endif

#define NOINLINE_FUNC __attribute__ ((noinline))

// CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT
NOINLINE_FUNC int config_av_stream_cache_size_default(void)
{
    return CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT;
}

// CONFIG_PLAYER_TASK_STACK_SIZE
NOINLINE_FUNC int config_player_task_stack_size(void)
{
    return CONFIG_PLAYER_TASK_STACK_SIZE;
}

// CONFIG_WEB_CACHE_TASK_STACK_SIZE
NOINLINE_FUNC int config_web_cache_task_stack_size(void)
{
    return CONFIG_WEB_CACHE_TASK_STACK_SIZE;
}

// CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT
NOINLINE_FUNC int config_av_stream_cache_threshold_default(void)
{
    return CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT;
}

// CONFIG_AV_PROBE_SIZE_MAX
NOINLINE_FUNC int config_av_probe_size_max(void)
{
    return CONFIG_AV_PROBE_SIZE_MAX;
}

// CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX
NOINLINE_FUNC int config_av_sample_num_per_frame_max(void)
{
    return CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX;
}

// CONFIG_ATEMPOER_SONIC
NOINLINE_FUNC int config_av_atempoer_enable(void)
{
#if defined(CONFIG_ATEMPOER_SONIC) && CONFIG_ATEMPOER_SONIC
    return 1;
#else
    return 0;
#endif
}

// CONFIG_AO_MIXER_SUPPORT
NOINLINE_FUNC int config_av_mixer_enable(void)
{
#if defined(CONFIG_AO_MIXER_SUPPORT) && CONFIG_AO_MIXER_SUPPORT
    return 1;
#else
    return 0;
#endif
}

// CONFIG_AEF_EQ_ENABLE
NOINLINE_FUNC int config_av_aef_enable(void)
{
#if defined(CONFIG_AEF_EQ_ENABLE) && CONFIG_AEF_EQ_ENABLE
    return 1;
#else
    return 0;
#endif
}

// CONFIG_RESAMPLER_SPEEX
NOINLINE_FUNC int config_av_resampler_enable(void)
{
#if defined(CONFIG_RESAMPLER_SPEEX) && CONFIG_RESAMPLER_SPEEX
    return 1;
#else
    return 0;
#endif
}

// CONFIG_MSP_DEBUG
NOINLINE_FUNC int config_msp_debug_enable(void)
{
#if defined(CONFIG_MSP_DEBUG) && CONFIG_MSP_DEBUG
    return 1;
#else
    return 0;
#endif
}

#endif
