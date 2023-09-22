/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef CONFIG_CPU_C906
#include "stream/stream.h"
#include "stream/stream_all.h"

#define NOINLINE_FUNC __attribute__ ((noinline))

/**
 * @brief  regist stream for memory
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_mem()
{
    extern struct stream_ops stream_ops_mem;
    return stream_ops_register(&stream_ops_mem);
}

/**
 * @brief  regist stream for file
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_file()
{
    extern struct stream_ops stream_ops_file;
    return stream_ops_register(&stream_ops_file);
}

/**
 * @brief  regist stream for http
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_http()
{
    extern struct stream_ops stream_ops_http;
    return stream_ops_register(&stream_ops_http);
}

/**
 * @brief  regist stream for fifo
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_fifo()
{
    extern struct stream_ops stream_ops_fifo;
    return stream_ops_register(&stream_ops_fifo);
}

/**
 * @brief  regist stream for crypto
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_crypto()
{
    extern struct stream_ops stream_ops_crypto;
    return stream_ops_register(&stream_ops_crypto);
}

/**
 * @brief  regist stream for hls
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_hls()
{
    extern struct stream_ops stream_ops_hls;
    return stream_ops_register(&stream_ops_hls);
}

/**
 * @brief  regist stream for ashx
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_ashx()
{
    extern struct stream_ops stream_ops_ashx;
    return stream_ops_register(&stream_ops_ashx);
}

/**
 * @brief  regist stream for dash
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_dash()
{
    extern struct stream_ops stream_ops_dash;
    return stream_ops_register(&stream_ops_dash);
}

/**
 * @brief  regist all streamer
 * @return 0/-1
 */
NOINLINE_FUNC int stream_register_all()
{
#if defined(CONFIG_STREAMER_MEM)
    REGISTER_STREAMER(MEM, mem);
#endif
#if defined(CONFIG_STREAMER_FILE)
    REGISTER_STREAMER(FILE, file);
#endif
#if defined(CONFIG_STREAMER_HTTP)
    REGISTER_STREAMER(HTTP, http);
#endif
#if defined(CONFIG_STREAMER_FIFO)
    REGISTER_STREAMER(FIFO, fifo);
#endif
#if defined(CONFIG_STREAMER_CRYPTO) && defined(CONFIG_USING_TLS)
    REGISTER_STREAMER(CRYPTO, crypto);
#endif
#if defined(CONFIG_STREAMER_HLS)
    REGISTER_STREAMER(HLS, hls);
#endif
#if defined(CONFIG_STREAMER_ASHX)
    REGISTER_STREAMER(ASHX, ashx);
#endif
#if defined(CONFIG_STREAMER_DASH)
    REGISTER_STREAMER(DASH, dash);
#endif
    return 0;
}
#endif
