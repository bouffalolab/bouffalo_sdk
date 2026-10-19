/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include "aef/aef.h"
#include "aef/aef_all.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

/**
 * @brief  regist audio effecter for sona
 * @return 0/-1
 */
NOINLINE_FUNC int aefx_register_sona()
{
    extern struct aefx_ops aefx_ops_sona;
    return aefx_ops_register(&aefx_ops_sona);
}

/**
 * @brief  regist audio effecter for ipc
 * @return 0/-1
 */
NOINLINE_FUNC int aefx_register_ipc()
{
    extern struct aefx_ops aefx_ops_ipc;
    return aefx_ops_register(&aefx_ops_ipc);
}

/**
 * @brief  regist audio effecter
 * @return 0/-1
 */
NOINLINE_FUNC int aefx_register()
{
#if defined(CONFIG_AEFXER_SONA)
    REGISTER_AEFXER(SONA, sona);
#endif
#if defined(CONFIG_AEFXER_IPC)
    REGISTER_AEFXER(IPC, ipc);
#endif
    return 0;
}

#endif
