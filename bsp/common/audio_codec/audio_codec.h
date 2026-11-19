/**
 * @file audio_codec.h
 * @brief Unified audio codec abstraction + scan/init helpers
 */

#ifndef _BSP_COMMON_AUDIO_CODEC_H_
#define _BSP_COMMON_AUDIO_CODEC_H_

#include <stdbool.h>
#include <stdint.h>

struct bflb_device_s;
struct audio_codec_driver;

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: Do not use enum here.
 * Keep public values as macros to make it easy to share across C toolchains
 * and avoid enum-related ABI/diagnostic differences.
 */

typedef uint8_t audio_codec_type_t;
#define AUDIO_CODEC_TYPE_UNKNOWN ((audio_codec_type_t)0)
#define AUDIO_CODEC_TYPE_ES8311  ((audio_codec_type_t)1)
#define AUDIO_CODEC_TYPE_ES8389  ((audio_codec_type_t)2)
#define AUDIO_CODEC_TYPE_ES8388  ((audio_codec_type_t)3)
#define AUDIO_CODEC_TYPE_WM8978  ((audio_codec_type_t)4)
#define AUDIO_CODEC_TYPE_JY6311  ((audio_codec_type_t)5)
#define AUDIO_CODEC_TYPE_BL      ((audio_codec_type_t)6)
#define AUDIO_CODEC_TYPE_JY6166  ((audio_codec_type_t)7)
#define AUDIO_CODEC_TYPE_JY6316  ((audio_codec_type_t)8)
#define AUDIO_CODEC_TYPE_AC101S  ((audio_codec_type_t)9)
#define AUDIO_CODEC_TYPE_ACM8852 ((audio_codec_type_t)10)
#define AUDIO_CODEC_TYPE_JY6416  ((audio_codec_type_t)11)

typedef uint8_t audio_codec_i2s_fmt_t;
#define AUDIO_CODEC_I2S_FMT_I2S     ((audio_codec_i2s_fmt_t)0)
#define AUDIO_CODEC_I2S_FMT_LEFT_J  ((audio_codec_i2s_fmt_t)1)
#define AUDIO_CODEC_I2S_FMT_RIGHT_J ((audio_codec_i2s_fmt_t)2)
#define AUDIO_CODEC_I2S_FMT_DSP     ((audio_codec_i2s_fmt_t)3)

typedef uint8_t audio_codec_bits_t;
#define AUDIO_CODEC_BITS_16 ((audio_codec_bits_t)16)
#define AUDIO_CODEC_BITS_24 ((audio_codec_bits_t)24)
#define AUDIO_CODEC_BITS_32 ((audio_codec_bits_t)32)

typedef uint8_t audio_codec_mic_input_t;
/* Some codecs can switch mic input topology.
 * Default is DIFF when the field is omitted (0).
 */
#define AUDIO_CODEC_MIC_INPUT_DIFF   ((audio_codec_mic_input_t)0)
#define AUDIO_CODEC_MIC_INPUT_SINGLE ((audio_codec_mic_input_t)1)
#define AUDIO_CODEC_MIC_INPUT_DIGITAL ((audio_codec_mic_input_t)2)

typedef uint8_t audio_codec_work_mode_t;
/* Codec init mode (codec-specific). Default is CODEC when omitted (0). */
#define AUDIO_CODEC_WORK_MODE_CODEC     ((audio_codec_work_mode_t)0)
#define AUDIO_CODEC_WORK_MODE_RECORDING ((audio_codec_work_mode_t)1)
#define AUDIO_CODEC_WORK_MODE_PLAYBACK  ((audio_codec_work_mode_t)2)

typedef struct {
    uint32_t sample_rate_hz;
    uint8_t channels; /* Codec-specific channel count. Typical I2S codecs use 1 or 2. */
    audio_codec_i2s_fmt_t i2s_fmt;
    audio_codec_bits_t bits;
    bool codec_master; /* codec outputs BCLK/LRCK when true */
    bool enable_adc;
    bool enable_dac;
    audio_codec_work_mode_t work_mode; /* optional, codec-specific */
    /* Optional, codec-specific. ES8388 SINGLE now selects LIN1/RIN1; otherwise
     * left ADC = LIN2-RIN2 and right ADC = LIN1-RIN1, without analog mono mix.
     * This replaces the BSP's legacy F8/88 mono mix. Review wiring and DMA
     * slot selection when migrating. The separate soln_utils codec API
     * retains its legacy routing.
     * Single-ended input 2 requires a codec-specific route; selecting an I2S
     * mono source changes DMA extraction, not these analog input selectors. */
    audio_codec_mic_input_t mic_input_mode;
} audio_codec_cfg_t;

typedef struct {
    const char *name;
    struct bflb_device_s *i2c;
    const struct audio_codec_driver *drv;
    audio_codec_type_t type;
    uint8_t i2c_addr;
    void *priv;
} audio_codec_dev_t;

#ifdef CONFIG_BSP_AUDIO_CODEC_BL_ENABLE
typedef struct {
    struct bflb_device_s *audac;
    struct bflb_device_s *auadc;
} audio_codec_bl_hw_cfg_t;
#endif

/* Scan codecs by probing known I2C addresses.
 * - returns matched codec type on success
 * - returns AUDIO_CODEC_TYPE_UNKNOWN if not found / invalid args
 *
 * On success, out_dev is filled (including out_dev->drv used by wrappers)
 * and its fields remain valid as long as the caller keeps the storage.
 */
audio_codec_type_t audio_codec_scan(struct bflb_device_s *i2c, audio_codec_dev_t *out_dev);
#ifdef CONFIG_BSP_AUDIO_CODEC_BL_ENABLE
int audio_codec_open_builtin(audio_codec_dev_t *out_dev, const audio_codec_bl_hw_cfg_t *cfg);
#endif

/* Capacity of the uint16_t slot masks, independent of device channel count. */
#define AUDIO_CODEC_MAX_SLOTS 16U

#define AUDIO_CODEC_CONTROL_VOLUME  (1U << 0)
#define AUDIO_CODEC_CONTROL_GAIN_DB (1U << 1)
#define AUDIO_CODEC_CONTROL_MUTE    (1U << 2)

typedef struct {
    uint32_t controls;
    uint32_t independent_controls;
    uint16_t linked_mask;
    const float *gain_steps_db;
    uint16_t gain_step_count;
} audio_codec_channel_caps_t;

typedef struct {
    float gain_db;
    uint8_t volume_percent;
    uint8_t valid; /* AUDIO_CODEC_CONTROL_* bits with confirmed values. */
    bool muted;
} audio_codec_channel_state_t;

int audio_codec_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg);
/* Masks select physical PCM slots. Zero is invalid. Linked controls require
 * their complete linked_mask; unsupported partial requests never change peers.
 * Volume is 0..100 percent. Gain uses the closest advertised native dB step
 * (ties select the lower gain); requests outside the advertised range fail.
 * All calls on a shared device/bus must be serialized by the caller.
 * Returns 0 on success, -1 for invalid arguments/transfers, -2 if unsupported.
 */
int audio_codec_set_mic_channel_volume(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent);
int audio_codec_set_mic_channel_gain_db(audio_codec_dev_t *dev, uint16_t slot_mask, float gain_db);
int audio_codec_set_mic_channel_mute(audio_codec_dev_t *dev, uint16_t slot_mask, bool mute);
int audio_codec_set_speaker_channel_volume(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent);
int audio_codec_set_speaker_channel_mute(audio_codec_dev_t *dev, uint16_t slot_mask, bool mute);
/* A present capture/reference slot may report no controls. Unsupported slots
 * return -2. Gain step storage remains owned by the driver. */
int audio_codec_get_mic_channel_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps);
int audio_codec_get_speaker_channel_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps);
int audio_codec_get_channel_masks(audio_codec_dev_t *dev, uint16_t *capture_mask, uint16_t *playback_mask);
/* Rate changes retain per-channel volume, gain and native mute settings.
 * Drivers that reset converters must restore their cached controls on success. */
int audio_codec_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz);
/* Direction gating retains shared converter clocks; CODEC selects both. */
int audio_codec_set_enable(audio_codec_dev_t *dev, audio_codec_work_mode_t mode, bool enable);
/* Register addresses/values use the driver's native encoding and width. */
int audio_codec_set_reg(audio_codec_dev_t *dev, int reg, int value);
int audio_codec_get_reg(audio_codec_dev_t *dev, int reg, int *value);
int audio_codec_dump_reg(audio_codec_dev_t *dev);
int audio_codec_deinit(audio_codec_dev_t *dev);

/* Simple helper: typical codecs use MCLK = Fs * 256.
 * If a codec needs a different ratio, its driver can override.
 */
uint32_t audio_codec_calc_mclk_hz(const audio_codec_dev_t *dev, uint32_t sample_rate_hz);

#ifdef __cplusplus
}
#endif

#endif
