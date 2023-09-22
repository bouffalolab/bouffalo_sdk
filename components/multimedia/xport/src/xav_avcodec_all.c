/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906

#include "avcodec/ad.h"
#include "avcodec/avcodec.h"
#include "avcodec/avcodec_all.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

/**
 * @brief  regist ad for pcm
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_pcm()
{
#if defined(CONFIG_DECODER_PCM)
    extern struct ad_ops ad_ops_pcm;
    return ad_ops_register(&ad_ops_pcm);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for ipc
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_ipc()
{
#if defined(CONFIG_DECODER_IPC)
    extern struct ad_ops ad_ops_ipc;
    return ad_ops_register(&ad_ops_ipc);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for tmall
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_tmall()
{
#if defined(CONFIG_DECODER_TMALL)
    extern struct ad_ops ad_ops_tmall;
    return ad_ops_register(&ad_ops_tmall);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for pvmp3
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_pvmp3()
{
#if defined(CONFIG_DECODER_PVMP3)
    extern struct ad_ops ad_ops_pvmp3;
    return ad_ops_register(&ad_ops_pvmp3);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for adpcm_ms
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_adpcm_ms()
{
#if defined(CONFIG_DECODER_ADPCM_MS)
    extern struct ad_ops ad_ops_adpcm_ms;
    return ad_ops_register(&ad_ops_adpcm_ms);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for flac
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_flac()
{
#if defined(CONFIG_DECODER_FLAC)
    extern struct ad_ops ad_ops_flac;
    return ad_ops_register(&ad_ops_flac);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for amrnb
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_amrnb()
{
#if defined(CONFIG_DECODER_AMRNB)
    extern struct ad_ops ad_ops_amrnb;
    return ad_ops_register(&ad_ops_amrnb);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for amrwb
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_amrwb()
{
#if defined(CONFIG_DECODER_AMRWB)
    extern struct ad_ops ad_ops_amrwb;
    return ad_ops_register(&ad_ops_amrwb);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for opus
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_opus()
{
#if defined(CONFIG_DECODER_OPUS)
    extern struct ad_ops ad_ops_opus;
    return ad_ops_register(&ad_ops_opus);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for speex
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_speex()
{
#if defined(CONFIG_DECODER_SPEEX)
    extern struct ad_ops ad_ops_speex;
    return ad_ops_register(&ad_ops_speex);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for vorbis
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_vorbis()
{
#if defined(CONFIG_DECODER_VORBIS)
    extern struct ad_ops ad_ops_vorbis;
    return ad_ops_register(&ad_ops_vorbis);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for alaw
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_alaw()
{
#if defined(CONFIG_DECODER_ALAW)
    extern struct ad_ops ad_ops_alaw;
    return ad_ops_register(&ad_ops_alaw);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for mulaw
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_mulaw()
{
#if defined(CONFIG_DECODER_MULAW)
    extern struct ad_ops ad_ops_mulaw;
    return ad_ops_register(&ad_ops_mulaw);
#else
    return 0;
#endif
}

/**
 * @brief  regist ad for aac
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_aac()
{
#if defined(CONFIG_DECODER_AAC)
    extern struct ad_ops ad_ops_aac;
    return ad_ops_register(&ad_ops_aac);
#else
    return 0;
#endif
}

/**
 * @brief  regist all decoder
 * @return 0/-1
 */
NOINLINE_FUNC int ad_register_all()
{
#if defined(CONFIG_DECODER_PCM)
    REGISTER_DECODER(PCM, pcm);
#endif
#if defined(CONFIG_DECODER_IPC)
    REGISTER_DECODER(IPC, ipc);
#endif
#if defined(CONFIG_DECODER_PVMP3)
    REGISTER_DECODER(PVMP3, pvmp3);
#endif
#if defined(CONFIG_DECODER_ADPCM_MS)
    REGISTER_DECODER(ADPCM_MS, adpcm_ms);
#endif
#if defined(CONFIG_DECODER_FLAC)
    REGISTER_DECODER(FLAC, flac);
#endif
#if defined(CONFIG_DECODER_AMRNB)
    REGISTER_DECODER(AMRNB, amrnb);
#endif
#if defined(CONFIG_DECODER_AMRWB)
    REGISTER_DECODER(AMRWB, amrwb);
#endif
#if defined(CONFIG_DECODER_OPUS)
    REGISTER_DECODER(OPUS, opus);
#endif
#if defined(CONFIG_DECODER_SPEEX)
    REGISTER_DECODER(SPEEX, speex);
#endif
#if defined(CONFIG_DECODER_VORBIS)
    REGISTER_DECODER(VORBIS, vorbis);
#endif
#if defined(CONFIG_DECODER_ALAW)
    REGISTER_DECODER(ALAW, alaw);
#endif
#if defined(CONFIG_DECODER_MULAW)
    REGISTER_DECODER(MULAW, mulaw);
#endif
#if defined(CONFIG_DECODER_AAC)
    REGISTER_DECODER(AAC, aac);
#endif
    return 0;
}
#endif
