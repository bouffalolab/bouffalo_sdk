/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include "avformat/avparser.h"
#include "avformat/avparser_all.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

/**
 * @brief  regist avparser for mp3
 * @return 0/-1
 */
NOINLINE_FUNC int avparser_register_mp3()
{
    extern struct avparser_ops avparser_ops_mp3;
    return avparser_ops_register(&avparser_ops_mp3);
}

/**
 * @brief  regist avparser for adts
 * @return 0/-1
 */
NOINLINE_FUNC int avparser_register_adts()
{
    extern struct avparser_ops avparser_ops_adts;
    return avparser_ops_register(&avparser_ops_adts);
}

/**
 * @brief  regist all avparser
 * @return 0/-1
 */
NOINLINE_FUNC int avparser_register_all()
{
#if defined(CONFIG_AVPARSER_MP3)
    REGISTER_AVPARSER(MP3, mp3);
#endif
#if defined(CONFIG_AVPARSER_ADTS)
    REGISTER_AVPARSER(ADTS, adts);
#endif
    return 0;
}
#endif
