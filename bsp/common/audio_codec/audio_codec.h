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
    uint8_t channels; /* 1 or 2 */
    audio_codec_i2s_fmt_t i2s_fmt;
    audio_codec_bits_t bits;
    bool codec_master; /* codec outputs BCLK/LRCK when true */
    bool enable_adc;
    bool enable_dac;
    audio_codec_work_mode_t work_mode; /* optional, codec-specific */
    audio_codec_mic_input_t mic_input_mode; /* optional, codec-specific */
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
int audio_codec_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg);
int audio_codec_set_mute(audio_codec_dev_t *dev, bool mute);
int audio_codec_set_volume(audio_codec_dev_t *dev, int volume_percent);
/* Recording/capture volume (not playback volume).
 * - volume_percent: 0..100
 * - returns 0 on success, negative on error, -2 if codec driver doesn't support it.
 */
int audio_codec_set_record_volume(audio_codec_dev_t *dev, int volume_percent);

/* Analog mic PGA gain as percentage (0..100).
 * Each codec maps the percentage to its step range.
 */
int audio_codec_set_mic_pga(audio_codec_dev_t *dev, int pga_percent);
int audio_codec_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz);

/* Simple helper: typical codecs use MCLK = Fs * 256.
 * If a codec needs a different ratio, its driver can override.
 */
uint32_t audio_codec_calc_mclk_hz(const audio_codec_dev_t *dev, uint32_t sample_rate_hz);

#ifdef __cplusplus
}
#endif

#endif
