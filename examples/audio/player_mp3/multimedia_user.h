/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */
#ifndef MULTIMEDIA_USER_H
#define MULTIMEDIA_USER_H

// xav config decoder
#define CONFIG_DECODER_ALAW                  (0)
#define CONFIG_DECODER_AMRNB                 (0)
#define CONFIG_DECODER_AMRWB                 (0)
#define CONFIG_DECODER_PCM                   (1)
#define CONFIG_DECODER_OPUS                  (0)
#define CONFIG_DECODER_VORBIS                (0)
#define CONFIG_DECODER_AAC                   (0)
#define CONFIG_DECODER_MULAW                 (0)
#define CONFIG_DECODER_PVMP3                 (1)
#define CONFIG_DECODER_SPEEX                 (0)
#define CONFIG_DECODER_ADPCM_MS              (0)
#define CONFIG_DECODER_FLAC                  (0)

// xav config streaming
#define CONFIG_STREAMER_FILE                 (1)
#define CONFIG_STREAMER_FIFO                 (0)
#define CONFIG_STREAMER_HLS                  (0)
#define CONFIG_STREAMER_HTTP                 (0)
#define CONFIG_STREAMER_CRYPTO               (0)
#define CONFIG_STREAMER_MEM                  (1)
#define CONFIG_STREAMER_DASH                 (0)
#define CONFIG_STREAMER_ASHX                 (0)

// xav config filtering
/* Configuration switch for controlling audio speed feature. */
/* if bt is enabled, CONFIG_ATEMPOER_SONIC must be enabled too */
#define CONFIG_ATEMPOER_SONIC                (0)
/* Configuration switch for controlling audio output mixing support. */
#define CONFIG_AO_MIXER_SUPPORT              (0)
/* Configuration switch for controlling audio effecter feature. */
#define CONFIG_AEF_EQ_ENABLE                 (0) // need sona lib

#endif
