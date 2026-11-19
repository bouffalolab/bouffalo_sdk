/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_DEV_DEFAULTS_H
#define XCODEC_DEV_DEFAULTS_H

#include "xcodec_data_if.h"
#include "xcodec_ctrl_if.h"
#include "xcodec_builtin_codec.h"
#include "xcodec_audio_codec.h"
#include "bflb_i2s.h"
#include "bflb_auadc.h"
#include "bflb_audac.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The board initializes pins; the backend initializes I2S at first device open.
 * I2S and DMA handles are borrowed exclusively until the interface is deleted.
 * One interface serves separate IN/OUT devices; disabling one preserves the
 * other direction and bus clocks. Stop all IO before deleting the interface.
 * The first enabled direction starts every configured DMA before the shared
 * clock. Unopened output sends silence; unopened input is discarded. The last
 * disabled direction stops the bus. No application silence priming is needed.
 * Device open supplies the sample rate; set_clock optionally selects its PLL.
 * Slots and FIFO packing remain board wiring constraints.
 * DMA buffers, software PCM queues and workers are allocated by data_new and
 * retained until deletion. Each queue has the same capacity as its DMA ring.
 * Each native DMA period must be a multiple of BFLB_CACHE_LINE_SIZE bytes.
 */
typedef struct {
    struct bflb_device_s *i2s;
    struct bflb_device_s *dma_tx;
    struct bflb_device_s *dma_rx;
    struct bflb_i2s_config_s config; /* bclk_freq_hz is supplied by device open. */
    void (*set_clock)(unsigned int rate); /* Optional board PLL selection; called while stopped. */
    xcodec_channel_cfg_t channel;
    uint8_t slots;
    bool lr_merge; /* Must match I2S: true packs PCM16, false uses 32-bit FIFO words. */
} xcodec_i2s_cfg_t;

#define XCODEC_I2S_DEFAULT_CONFIG(i2s_dev, tx_dma, rx_dma) {    \
    .i2s = (i2s_dev),                                           \
    .dma_tx = (tx_dma),                                         \
    .dma_rx = (rx_dma),                                         \
    .config = {                                                \
        .role = I2S_ROLE_MASTER,                                \
        .format_mode = I2S_MODE_LEFT_JUSTIFIED,                  \
        .channel_mode = I2S_CHANNEL_MODE_NUM_2,                  \
        .frame_width = I2S_SLOT_WIDTH_16,                        \
        .data_width = I2S_SLOT_WIDTH_16,                         \
        .fs_offset_cycle = 1,                                   \
        .tx_fifo_threshold = 0, .rx_fifo_threshold = 0,          \
    },                                                         \
    .channel = XCODEC_CHANNEL_DEFAULT_CONFIG(),                 \
    .slots = 2,                                                 \
    .lr_merge = true,                                           \
}

const xcodec_data_if_t *xcodec_i2s_data_new(const xcodec_i2s_cfg_t *cfg);

typedef struct {
    struct bflb_device_s *audac;
    struct bflb_device_s *dma_tx;
    struct bflb_audac_init_config_s config; /* sampling_rate is supplied by device open. */
    xcodec_channel_cfg_t channel;
    void (*set_clock)(unsigned int rate);
} xcodec_audac_cfg_t;

#define XCODEC_AUDAC_DEFAULT_CONFIG(audac_dev, tx_dma) {        \
    .audac = (audac_dev),                                       \
    .dma_tx = (tx_dma),                                         \
    .config = {                                                 \
        .sampling_rate = AUDAC_SAMPLING_RATE_16K,               \
        .output_mode = AUDAC_OUTPUT_MODE_PWM,                   \
        .source_channels_num = AUDAC_SOURCE_CHANNEL_SINGLE,     \
        .mixer_mode = AUDAC_MIXER_MODE_ONLY_L,                  \
        .data_format = AUDAC_DATA_FORMAT_16BIT,                 \
        .fifo_threshold = 3,                                    \
    },                                                          \
    .channel = XCODEC_CHANNEL_DEFAULT_CONFIG(),                 \
}

/* First device open initializes AUDAC muted; the codec device controls unmute. */
const xcodec_data_if_t *xcodec_audac_data_new(const xcodec_audac_cfg_t *cfg);

typedef struct {
    struct bflb_device_s *auadc;
    struct bflb_device_s *dma_rx;
    struct bflb_auadc_init_config_s config; /* sampling_rate is supplied by device open. */
#if AUADC_ANALOG_ADC_SUPPORT
    struct bflb_auadc_adc_init_config_s analog_config;
#endif
    xcodec_channel_cfg_t channel;
    void (*set_clock)(unsigned int rate);
} xcodec_auadc_cfg_t;

/* PDM defaults work on every supported AUADC; analog wiring belongs to the BSP. */
#define XCODEC_AUADC_DEFAULT_CONFIG(auadc_dev, rx_dma) {        \
    .auadc = (auadc_dev),                                       \
    .dma_rx = (rx_dma),                                         \
    .config = {                                                 \
        .sampling_rate = AUADC_SAMPLING_RATE_16K,               \
        .input_mode = AUADC_INPUT_MODE_PDM_L,                   \
        .data_format = AUADC_DATA_FORMAT_16BIT,                 \
        .fifo_threshold = 3,                                    \
    },                                                          \
    .channel = XCODEC_CHANNEL_DEFAULT_CONFIG(),                 \
}

const xcodec_data_if_t *xcodec_auadc_data_new(const xcodec_auadc_cfg_t *cfg);

/* The board owns the bus. addr is a seven-bit address; register bytes are MSB first. */
typedef struct {
    struct bflb_device_s *i2c;
    uint8_t addr;
} xcodec_i2c_ctrl_cfg_t;
const xcodec_ctrl_if_t *xcodec_i2c_ctrl_new(const xcodec_i2c_ctrl_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
#endif
