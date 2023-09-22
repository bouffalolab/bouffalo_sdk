/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include "avformat/avformat.h"
#include "avformat/avformat_all.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

/**
 * @brief  regist demux for wav
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_wav()
{
    extern struct demux_ops demux_ops_wav;
    return demux_ops_register(&demux_ops_wav);
}

/**
 * @brief  regist demux for mp3
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_mp3()
{
    extern struct demux_ops demux_ops_mp3;
    return demux_ops_register(&demux_ops_mp3);
}

/**
 * @brief  regist demux for mp4
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_mp4()
{
    extern struct demux_ops demux_ops_mp4;
    return demux_ops_register(&demux_ops_mp4);
}

/**
 * @brief  regist demux for rawaudio
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_rawaudio()
{
    extern struct demux_ops demux_ops_rawaudio;
    return demux_ops_register(&demux_ops_rawaudio);
}

/**
 * @brief  regist demux for adts
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_adts()
{
    extern struct demux_ops demux_ops_adts;
    return demux_ops_register(&demux_ops_adts);
}

/**
 * @brief  regist demux for flac
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_flac()
{
    extern struct demux_ops demux_ops_flac;
    return demux_ops_register(&demux_ops_flac);
}

/**
 * @brief  regist demux for asf
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_asf()
{
    extern struct demux_ops demux_ops_asf;
    return demux_ops_register(&demux_ops_asf);
}

/**
 * @brief  regist demux for amr
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_amr()
{
    extern struct demux_ops demux_ops_amr;
    return demux_ops_register(&demux_ops_amr);
}

/**
 * @brief  regist demux for ts
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_ts()
{
    extern struct demux_ops demux_ops_ts;
    return demux_ops_register(&demux_ops_ts);
}

/**
 * @brief  regist demux for ogg
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_ogg()
{
    extern struct demux_ops demux_ops_ogg;
    return demux_ops_register(&demux_ops_ogg);
}

/**
 * @brief  regist all demuxer
 * @return 0/-1
 */
NOINLINE_FUNC int demux_register_all()
{
#if defined(CONFIG_DEMUXER_WAV)
    REGISTER_DEMUXER(WAV, wav);
#endif
#if defined(CONFIG_DEMUXER_MP3)
    REGISTER_DEMUXER(MP3, mp3);
#endif
#if defined(CONFIG_DEMUXER_MP4) && defined(CONFIG_USING_TLS)
    REGISTER_DEMUXER(MP4, mp4);
#endif
#if defined(CONFIG_DEMUXER_ADTS)
    REGISTER_DEMUXER(ADTS, adts);
#endif
#if defined(CONFIG_DEMUXER_RAWAUDIO)
    REGISTER_DEMUXER(RAWAUDIO, rawaudio);
#endif
#if defined(CONFIG_DEMUXER_FLAC)
    REGISTER_DEMUXER(FLAC, flac);
#endif
#if defined(CONFIG_DEMUXER_ASF)
    REGISTER_DEMUXER(ASF, asf);
#endif
#if defined(CONFIG_DEMUXER_AMR)
    REGISTER_DEMUXER(AMR, amr);
#endif
#if defined(CONFIG_DEMUXER_TS)
    REGISTER_DEMUXER(TS, ts);
#endif
#if defined(CONFIG_DEMUXER_OGG)
    REGISTER_DEMUXER(OGG, ogg);
#endif

    return 0;
}
#endif
