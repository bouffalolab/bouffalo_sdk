/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */
#ifndef MULTIMEDIA_USER_H
#define MULTIMEDIA_USER_H

// xav config decoder
#define CONFIG_DECODER_ALAW                  (1)
#define CONFIG_DECODER_AMRNB                 (1)
#define CONFIG_DECODER_AMRWB                 (1)
#define CONFIG_DECODER_PCM                   (1)
#define CONFIG_DECODER_OPUS                  (1)
#define CONFIG_DECODER_VORBIS                (1)
#define CONFIG_DECODER_AAC                   (1)
#define CONFIG_DECODER_MULAW                 (1)
#define CONFIG_DECODER_PVMP3                 (1)
#define CONFIG_DECODER_SPEEX                 (1)
#define CONFIG_DECODER_ADPCM_MS              (1)
#define CONFIG_DECODER_FLAC                  (1)

// xav config streaming
#define CONFIG_STREAMER_FILE                 (1)
#define CONFIG_STREAMER_FIFO                 (1)
#define CONFIG_STREAMER_HLS                  (1)
#define CONFIG_STREAMER_HTTP                 (1)
#define CONFIG_STREAMER_CRYPTO               (1)
#define CONFIG_STREAMER_MEM                  (1)
#define CONFIG_STREAMER_DASH                 (0)
#define CONFIG_STREAMER_ASHX                 (1)

// xav config filtering
/* Configuration switch for controlling audio speed feature. */
/* if bt is enabled, CONFIG_ATEMPOER_SONIC must be enabled too */
#define CONFIG_ATEMPOER_SONIC                (1)
/* Configuration switch for controlling audio resampling with specified frequency. */
/* if CONFIG_RESAMPLER_SPEEX set disabled, input audio channel only support mono */
#define CONFIG_RESAMPLER_SPEEX               (1)
/* Configuration switch for controlling audio output mixing support. */
#define CONFIG_AO_MIXER_SUPPORT              (1)
/* Configuration switch for controlling audio effecter feature. */
#define CONFIG_AEF_EQ_ENABLE                 (0) // need sona lib

#endif
