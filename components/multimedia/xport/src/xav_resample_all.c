/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include "swresample/resample.h"
#include "swresample/resample_all.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

/**
 * @brief  regist swresample for ipc
 * @return 0/-1
 */
NOINLINE_FUNC int resample_register_ipc()
{
    extern struct resx_ops resx_ops_ipc;
    return resx_ops_register(&resx_ops_ipc);
}

/**
 * @brief  regist swresample for speex
 * @return 0/-1
 */
NOINLINE_FUNC int resample_register_speex()
{
    extern struct resx_ops resx_ops_speex;
    return resx_ops_register(&resx_ops_speex);
}

/**
 * @brief  regist all resampler
 * @return 0/-1
 */
NOINLINE_FUNC int resample_register_all()
{
#if defined(CONFIG_RESAMPLER_IPC)
    REGISTER_RESAMPLER(IPC, ipc);
#endif
#if defined(CONFIG_RESAMPLER_SPEEX)
    REGISTER_RESAMPLER(SPEEX, speex);
#endif
    return 0;
}
#endif
