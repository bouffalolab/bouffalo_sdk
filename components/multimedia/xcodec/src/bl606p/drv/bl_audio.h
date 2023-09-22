/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef _YOC_BL_AUDIO_H_
#define _YOC_BL_AUDIO_H_

#if 1
#include <msp_bl606p_platform.h>
#include <msp_bl606p_dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Audio Channel Sel
 */
typedef enum {
    BL_AUDIO_LEFT_CHANNEL_  = 1, /*!< select mono mode left only */
    BL_AUDIO_STEREO_CHANNEL_ = 3,   /*!< select stereo */
    BL_AUDIO_THREE_CHANNEL_  = 7,
} BL_Audio_Channel_Type;

/**
 *  @brief Audio Play/recording bit width
 */
typedef enum {
    BL_AUDIO_BIT_WIDTH_16, /*!< 16 bit */
    BL_AUDIO_BIT_WIDTH_24, /*!< 24 bit */
    BL_AUDIO_BIT_WIDTH_32, /*!< 32 bit */
} BL_Audio_BitWidth_Type;

#define AUDIO_DEFAULT_INITIAL_VOLUME         (0x0)
#define AUDIO_TX_FIFO_ADDR                      (AUDIO_BASE + 0x94)
#define AUDIO_RX_FIFO_ADDR                      (AUDIO_BASE + 0x88)
#define AUDIO_ALIGNMENT_MASK (0x001f)
#define AUDIO_ALIGNMENT_BYTES (32)

/* audio sample  rate */
#define BL_AUDIO_RATE_8K                 8000
#define BL_AUDIO_RATE_16K                16000
#define BL_AUDIO_RATE_24K                24000
#define BL_AUDIO_RATE_32K                32000
#define BL_AUDIO_RATE_48K                48000
#define BL_AUDIO_RATE_44_1K              44100
#define BL_AUDIO_RATE_96K                96000
#define BL_AUDIO_RATE_192K               192000

/* audio bit width */
#define BL_AUDIO_BIT_WIDTH_16            16
#define BL_AUDIO_BIT_WIDTH_24            24
#define BL_AUDIO_BIT_WIDTH_32            32
/* audio track */
#define BL_AUDIO_LEFT_CHANNEL            1
#define BL_AUDIO_STEREO_CHANNEL          2
#define BL_AUDIO_THREE_CHANNEL           3

typedef enum {
    BL_EVENT_NODE_READ_COMPLETE        = 0U,  ///< A peroid data read completed
    BL_EVENT_NODE_WRITE_COMPLETE       = 1U,  ///< A peroid data write completed
    BL_EVENT_WRITE_BUFFER_EMPTY          = 2U,  ///< Fifo is empty
    BL_EVENT_READ_BUFFER_FULL            = 3U,  ///< Fifo is full
    BL_EVENT_ERROR_OVERFLOW              = 4U,  ///< Fifo overflow error
    BL_EVENT_ERROR_UNDERFLOW             = 5U,  ///< Fifo underflow error
    BL_EVENT_ERROR                       = 6U,  ///< The device has a hardware error
} audio_codec_event_t;

typedef struct
{
    uint32_t srcDmaAddr;            /*!< Source address of DMA transfer */
    uint32_t destDmaAddr;           /*!< Destination address of DMA transfer */
    uint32_t nextLLI;               /*!< Next LLI address */
    struct DMA_Control_Reg dmaCtrl; /*!< DMA transaction control */
} _dmahw_cfg_t;

 #ifdef __cplusplus
 }
 #endif

#endif
 #endif /* _YOC_BL_AUDIO_H_ */

