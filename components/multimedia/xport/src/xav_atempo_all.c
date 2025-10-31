/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include "atempo/atempo.h"
#include "atempo/atempo_all.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

/**
 * @brief  regist audio atempoer for sonic
 * @return 0/-1
 */
NOINLINE_FUNC int atempo_register_sonic()
{
    extern struct atempo_ops atempo_ops_sonic;
    return atempo_ops_register(&atempo_ops_sonic);
}

/**
 * @brief  regist audio atempoer for ipc
 * @return 0/-1
 */
NOINLINE_FUNC int atempo_register_ipc()
{
    extern struct atempo_ops atempo_ops_ipc;
    return atempo_ops_register(&atempo_ops_ipc);
}

/**
 * @brief  regist audio atempoer
 * @return 0/-1
 */
NOINLINE_FUNC int atempo_register()
{
#if defined(CONFIG_ATEMPOER_SONIC)
    REGISTER_ATEMPOER(SONIC, sonic);
#endif
#if defined(CONFIG_ATEMPOER_IPC)
    REGISTER_ATEMPOER(IPC, ipc);
#endif
    return 0;
}

#endif