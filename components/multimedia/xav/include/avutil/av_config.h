/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AV_CONFIG_H__
#define __AV_CONFIG_H__

#if __has_include("multimedia_user.h")
#include "multimedia_user.h"
#endif

/*
 * The following feature configuration macro is customizable 
 * by the user; if the user does not configure it, it is 
 * enabled by default.
 */
// audio decoder config
#ifndef CONFIG_DECODER_AAC
#define CONFIG_DECODER_AAC                             (1)
#endif
#ifndef CONFIG_DECODER_ADPCM_MS
#define CONFIG_DECODER_ADPCM_MS                        (1)
#endif
#ifndef CONFIG_DECODER_ALAW
#define CONFIG_DECODER_ALAW                            (1)
#endif
#ifndef CONFIG_DECODER_AMRNB
#define CONFIG_DECODER_AMRNB                           (1)
#endif
#ifndef CONFIG_DECODER_AMRWB
#define CONFIG_DECODER_AMRWB                           (1)
#endif
#ifndef CONFIG_DECODER_FLAC
#define CONFIG_DECODER_FLAC                            (1)
#endif
#ifndef CONFIG_DECODER_IPC
#define CONFIG_DECODER_IPC                             (0)
#endif
#ifndef CONFIG_DECODER_MULAW
#define CONFIG_DECODER_MULAW                           (1)
#endif
#ifndef CONFIG_DECODER_OPUS
#define CONFIG_DECODER_OPUS                            (1)
#endif
#ifndef CONFIG_DECODER_PCM
#define CONFIG_DECODER_PCM                             (1)
#endif
#ifndef CONFIG_DECODER_PVMP3
#define CONFIG_DECODER_PVMP3                           (1)
#endif
#ifndef CONFIG_DECODER_SPEEX
#define CONFIG_DECODER_SPEEX                           (1)
#endif
#ifndef CONFIG_DECODER_VORBIS
#define CONFIG_DECODER_VORBIS                          (1)
#endif

// audio streaming config
#ifndef CONFIG_STREAMER_ASHX
#define CONFIG_STREAMER_ASHX                           (1)
#endif
#ifndef CONFIG_STREAMER_CRYPTO
#define CONFIG_STREAMER_CRYPTO                         (1)
#endif
#ifndef CONFIG_STREAMER_DASH
#define CONFIG_STREAMER_DASH                           (1)
#endif
#ifndef CONFIG_STREAMER_FIFO
#define CONFIG_STREAMER_FIFO                           (1)
#endif
#ifndef CONFIG_STREAMER_FILE
#define CONFIG_STREAMER_FILE                           (1)
#endif
#ifndef CONFIG_STREAMER_HLS
#define CONFIG_STREAMER_HLS                            (1)
#endif
#ifndef CONFIG_STREAMER_HTTP
#define CONFIG_STREAMER_HTTP                           (1)
#endif
#ifndef CONFIG_STREAMER_MEM
#define CONFIG_STREAMER_MEM                            (1)
#endif

// audio output config
#ifndef CONFIG_AV_AO_ALSA
#define CONFIG_AV_AO_ALSA                              (1)
#endif
#ifndef CONFIG_AV_AO_DUMMY
#define CONFIG_AV_AO_DUMMY                             (0)
#endif
#ifndef CONFIG_AV_AO_ALSA_STANDARD
#define CONFIG_AV_AO_ALSA_STANDARD                     (0)
#endif

// audio filtering config
// resample config
#ifndef CONFIG_RESAMPLER_SILAN
#define CONFIG_RESAMPLER_SILAN                         (0)
#endif
#ifndef CONFIG_RESAMPLER_SPEEX
#define CONFIG_RESAMPLER_SPEEX                         (1)
#endif
#ifndef CONFIG_RESAMPLER_IPC
#define CONFIG_RESAMPLER_IPC                           (0)
#endif
// atempoer config
#ifndef CONFIG_ATEMPOER_IPC
#define CONFIG_ATEMPOER_IPC                            (0)
#endif
#ifndef CONFIG_ATEMPOER_SONIC
#define CONFIG_ATEMPOER_SONIC                          (1)
#endif
// aef config
#ifndef CONFIG_AEFXER_IPC
#define CONFIG_AEFXER_IPC                              (0)
#endif
#ifndef CONFIG_AEFXER_SONA
#define CONFIG_AEFXER_SONA                             (0)
#endif
// eq config
#ifndef CONFIG_AEF_EQ_ENABLE
#define CONFIG_AEF_EQ_ENABLE                           (1)
#endif
// mixer config
#ifndef CONFIG_AO_MIXER_SUPPORT
#define CONFIG_AO_MIXER_SUPPORT                        (1)
#endif


/*
 * The user only needs to be concerned about the decoding 
 * function you require. Here, the switch to enable common 
 * storage formats for decoding audio will be opened, so 
 * there is no need for the user to worry about it.
 */
#if defined(CONFIG_DECODER_ALAW) && CONFIG_DECODER_ALAW
#ifndef CONFIG_DEMUXER_WAV
#define CONFIG_DEMUXER_WAV                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_MULAW) && CONFIG_DECODER_MULAW
#ifndef CONFIG_DEMUXER_WAV
#define CONFIG_DEMUXER_WAV                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_AMRNB) && CONFIG_DECODER_AMRNB
#ifndef CONFIG_DEMUXER_AMR
#define CONFIG_DEMUXER_AMR                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_AMRWB) && CONFIG_DECODER_AMRWB
#ifndef CONFIG_DEMUXER_AMR
#define CONFIG_DEMUXER_AMR                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_PCM) && CONFIG_DECODER_PCM
#ifndef CONFIG_DEMUXER_RAWAUDIO
#define CONFIG_DEMUXER_RAWAUDIO                        (1)
#endif
#ifndef CONFIG_DEMUXER_WAV
#define CONFIG_DEMUXER_WAV                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_OPUS) && CONFIG_DECODER_OPUS
#ifndef CONFIG_DEMUXER_OGG
#define CONFIG_DEMUXER_OGG                             (1)
#endif
#ifndef CONFIG_DEMUXER_MP4
#define CONFIG_DEMUXER_MP4                             (1)
#endif
#ifndef CONFIG_DEMUXER_TS
#define CONFIG_DEMUXER_TS                              (1)
#endif
#endif

#if defined(CONFIG_DECODER_SPEEX) && CONFIG_DECODER_SPEEX
#ifndef CONFIG_DEMUXER_OGG
#define CONFIG_DEMUXER_OGG                             (1)
#endif
#ifndef CONFIG_DEMUXER_MP4
#define CONFIG_DEMUXER_MP4                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_VORBIS) && CONFIG_DECODER_VORBIS
#ifndef CONFIG_DEMUXER_OGG
#define CONFIG_DEMUXER_OGG                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_AAC) && CONFIG_DECODER_AAC
#ifndef CONFIG_DEMUXER_ADTS
#define CONFIG_DEMUXER_ADTS                            (1)
#endif
#ifndef CONFIG_AVPARSER_ADTS
#define CONFIG_AVPARSER_ADTS                           (1)
#endif
#ifndef CONFIG_DEMUXER_TS
#define CONFIG_DEMUXER_TS                              (1)
#endif
#ifndef CONFIG_DEMUXER_MP4
#define CONFIG_DEMUXER_MP4                             (1)
#endif
#ifndef CONFIG_DEMUXER_ASF
#define CONFIG_DEMUXER_ASF                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_PVMP3) && CONFIG_DECODER_PVMP3
#ifndef CONFIG_DEMUXER_TS
#define CONFIG_DEMUXER_TS                              (1)
#endif
#ifndef CONFIG_DEMUXER_MP3
#define CONFIG_DEMUXER_MP3                             (1)
#endif
#ifndef CONFIG_AVPARSER_MP3
#define CONFIG_AVPARSER_MP3                            (1)
#endif
#ifndef CONFIG_DEMUXER_MP4
#define CONFIG_DEMUXER_MP4                             (1)
#endif
#ifndef CONFIG_DEMUXER_ASF
#define CONFIG_DEMUXER_ASF                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_FLAC) && CONFIG_DECODER_FLAC
#ifndef CONFIG_DEMUXER_FLAC
#define CONFIG_DEMUXER_FLAC                            (1)
#endif
#ifndef CONFIG_DEMUXER_WAV
#define CONFIG_DEMUXER_WAV                             (1)
#endif
#ifndef CONFIG_DEMUXER_MP4
#define CONFIG_DEMUXER_MP4                             (1)
#endif
#endif

#if defined(CONFIG_DECODER_ADPCM_MS) && CONFIG_DECODER_ADPCM_MS
#ifndef CONFIG_DEMUXER_WAV
#define CONFIG_DEMUXER_WAV                             (1)
#endif
#endif


/*
 * HTTP and HLS streaming require the use of TCP/IP for 
 * data transmission.
 */
#if (defined(CONFIG_STREAMER_HLS) && CONFIG_STREAMER_HLS) || \
    (defined(CONFIG_STREAMER_HTTP) && CONFIG_STREAMER_HTTP)
#ifndef CONFIG_TCPIP
#define CONFIG_TCPIP                                   (1)
#else
#if !CONFIG_TCPIP
#error "CONFIG_STREAMER_HLS or CONFIG_STREAMER_HTTP defined, but CONFIG_TCPIP not defined"
#endif
#endif
#ifndef CONFIG_USING_TLS
#define CONFIG_USING_TLS                               (1)
#endif
#endif

/*
 * MP4 demuxer and crypto need the TLS encryption and 
 * decryption functionality
 */
#if (defined(CONFIG_DEMUXER_MP4) && CONFIG_DEMUXER_MP4) || \
    (defined(CONFIG_STREAMER_CRYPTO) && CONFIG_STREAMER_CRYPTO)
#ifndef CONFIG_USING_TLS
#define CONFIG_USING_TLS                               (1)
#else
#if !CONFIG_USING_TLS
#error "CONFIG_DEMUXER_MP4 or CONFIG_STREAMER_CRYPTO defined, but CONFIG_USING_TLS not defined"
#endif
#endif
#endif

#include "av_inner_config.h"

#endif /* __AV_CONFIG_H__ */
