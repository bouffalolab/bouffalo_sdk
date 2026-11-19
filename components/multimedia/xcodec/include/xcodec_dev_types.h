/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_DEV_TYPES_H
#define XCODEC_DEV_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/* Device status codes. Backends may also return their own negative errors. */
#define XCODEC_DEV_OK           0     /**< Operation completed successfully. */
#define XCODEC_DEV_DRV_ERR      (-1)  /**< Driver operation failed. */
#define XCODEC_DEV_INVALID_ARG  (-2)  /**< Invalid handle, pointer or parameter. */
#define XCODEC_DEV_NO_MEM       (-3)  /**< Memory allocation failed. */
#define XCODEC_DEV_NOT_SUPPORT  (-4)  /**< Direction, format or operation is unsupported. */
#define XCODEC_DEV_NOT_FOUND    (-5)  /**< Requested resource was not found. */
#define XCODEC_DEV_WRONG_STATE  (-6)  /**< Operation is invalid in the current lifecycle state. */
#define XCODEC_DEV_WRITE_FAIL   (-7)  /**< Backend did not report a complete write. */
#define XCODEC_DEV_READ_FAIL    (-8)  /**< Backend did not report a complete read. */

/** Build a channel-mask bit; channel must be in the range 0..15. */
#define XCODEC_DEV_MAKE_CHANNEL_MASK(channel) ((uint16_t)(1U << (channel)))

/** Number of selected PCM channels; a zero mask selects all physical channels. */
#define XCODEC_CHANNEL_COUNT(channels, mask) \
    ((mask) ? (unsigned)__builtin_popcount((unsigned)(mask)) : (unsigned)(channels))

/** Device direction; IN and OUT flags can be combined as IN_OUT. */
typedef enum {
    XCODEC_DEV_TYPE_NONE = 0,   /**< No direction; invalid for device creation. */
    XCODEC_DEV_TYPE_IN = 1,     /**< Capture PCM from the converter. */
    XCODEC_DEV_TYPE_OUT = 2,    /**< Play PCM through the converter. */
    XCODEC_DEV_TYPE_IN_OUT = 3, /**< Capture and playback in one device. */
} xcodec_dev_type_t;

/** DMA buffering and blocking IO settings shared by the data backends. */
typedef struct {
    uint16_t block_frames; /**< Frames per DMA block; 0 uses the driver default. */
    uint16_t period_count; /**< DMA blocks and software queue capacity per direction, 2..1023. */
    uint32_t timeout_ms;   /**< Read/write timeout; 0 uses the driver default. */
} xcodec_channel_cfg_t;

#define XCODEC_CHANNEL_DEFAULT_CONFIG() { \
    .block_frames = 160,                 \
    .period_count = 8,                   \
    .timeout_ms = 1000,                  \
}

/**
 * @brief PCM format requested at device open.
 * @note Data/Codec backends may impose stricter format limits. The current
 *       platform data backends use PCM16 and board-configured slots. I2S master
 *       rates must be achievable from the board clock selected by set_clock.
 *       Selected channels are packed into interleaved PCM in channel order.
 *       channel_mask selects physical slots without changing channel or bus
 *       framing. Zero selects all slots; bit n selects CHn (CH0 is bit 0).
 *       With channel=4: 0x01 selects CH0, 0x08 selects CH3, and 0x0a selects
 *       CH1/CH3. Capture returns only selected samples; playback consumes only
 *       selected samples and fills unselected slots with silence. For 0x0a,
 *       application PCM is CH1,CH3,CH1,CH3,... in both directions.
 *       Bits outside channel are invalid. Set the mask before device open;
 *       close before changing it.
 */
typedef struct {
    uint8_t bits_per_sample; /**< Sample width in bits: 8, 16, 24 or 32. */
    uint8_t channel;         /**< Physical channel/slot count, 1..16. */
    uint16_t channel_mask;   /**< Physical slot bitmask per direction; 0 selects all. */
    uint32_t sample_rate;    /**< Physical sample rate in Hz; must be nonzero. */
    int mclk_multiple;       /**< MCLK/sample-rate ratio; 0 accepts the board-selected
                             *   clock for I2S (other backends normally use 256).
                             *   Negative values are invalid. */
} xcodec_dev_sample_info_t;

#define XCODEC_DEV_SAMPLE_INFO_DEFAULT_CONFIG() { \
    .bits_per_sample = 16,                        \
    .channel = 1,                                 \
    .channel_mask = 0,                            \
    .sample_rate = 16000,                         \
    .mclk_multiple = 0,                           \
}

#endif
