/*
 * Play embedded PCM audio data via msp_pcm_* (ALSA-like) API.
 * Command: minialsa_play [wa|sin]
 *   wa  - 16 kHz / 16-bit / mono voice sample (default)
 *   sin - 16 kHz / 16-bit / mono 1 kHz sine wave
 *
 * API self-test command:
 *   minialsa_api_test [all|playback|capture|set_params] [wa|sin]
 *
 * Output clock test command:
 *   minialsa_output_clock_test <sample_rate_hz>
 */

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <devices/drv_snd_bl616.h>

#include <msp/kernel.h>
#include <alsa/pcm.h>
#include <bflb_mtimer.h>
#include <shell.h>
#include <xcodec.h>
#if defined(CONFIG_CODEC_USE_I2S) && CONFIG_CODEC_USE_I2S
#include "hardware/i2s_reg.h"
#include "hardware/glb_reg.h"
#if defined(BL616)
#include "hardware/bl616.h"
#elif defined(BL616CL)
#include "hardware/bl616cl.h"
#elif defined(BL618DG)
#include "hardware/bl618dg.h"
#elif defined(BL702)
#include "hardware/bl702.h"
#endif
#endif

#include "audio_file.h"

/* ─── Playback parameters ─────────────────────────────── */
#define ALSA_PLAY_PCM_NAME      "pcmP0"
#define ALSA_PLAY_PERIOD_FRAMES 256
#define ALSA_PLAY_BUFFER_FRAMES 1024
#define ALSA_CAP_PCM_NAME       "pcmC0"
#define ALSA_TEST_PERIOD_FRAMES 160
#define ALSA_TEST_BUFFER_FRAMES 640
#define ALSA_TEST_PLAYBACK_MS   2000
#define ALSA_TEST_CAPTURE_POLL_MS 100
#define ALSA_TEST_CAPTURE_POLL_STEP_MS 20
#define ALSA_TEST_MAX_RATE_HZ   192000
#define ALSA_CLOCK_TEST_CHANNELS           1U
#define ALSA_CLOCK_TEST_BIT_WIDTH          16U
#define ALSA_CLOCK_TEST_SLOT_WIDTH         16U
#define ALSA_CLOCK_TEST_PERIOD_BYTES       1024U
#define ALSA_CLOCK_TEST_HOLD_MS            10000U
#define ALSA_CLOCK_TEST_MIN_PERIOD_BYTES    32U
#define ALSA_CLOCK_TEST_BUFFER_PERIODS      8U
#define ALSA_CLOCK_TEST_BUFFER_PADDING      1024U

/* ─── Audio sample descriptor ─────────────────────────── */
typedef struct {
    const char         *name;
    const unsigned char *data;
    const unsigned int  *len;
    unsigned int         rate;
    unsigned int         channels;
    snd_pcm_format_t     format;
} alsa_sample_t;

static const alsa_sample_t g_samples[] = {
    { "wa",  wa_16K_16bit_mono_pcm,       &wa_16K_16bit_mono_pcm_len,       16000, 1, SND_PCM_FORMAT_S16_LE },
    { "sin", sin_1khz_16k_sample_0db_pcm, &sin_1khz_16k_sample_0db_pcm_len, 16000, 1, SND_PCM_FORMAT_S16_LE },
};

static volatile int g_alsa_running = 0;
static volatile int g_alsa_api_test_running = 0;

typedef struct {
    unsigned int passed;
    unsigned int failed;
} alsa_test_stats_t;

typedef struct {
    const alsa_sample_t *sample;
    int run_playback;
    int run_capture;
    int run_set_params;
} alsa_api_test_cfg_t;

typedef struct {
    uint32_t sample_rate_hz;
} alsa_output_clock_test_cfg_t;

static void alsa_api_test_print_usage(void)
{
    printf("usage: minialsa_api_test [all|playback|capture|set_params] [wa|sin]\r\n");
}

static void alsa_output_clock_test_print_usage(void)
{
    printf("usage: minialsa_output_clock_test <sample_rate_hz>\r\n");
    printf("fixed: channels=%u bit_width=%u slot_width=%u period=%u hold_ms=%u\r\n",
           ALSA_CLOCK_TEST_CHANNELS,
           ALSA_CLOCK_TEST_BIT_WIDTH,
           ALSA_CLOCK_TEST_SLOT_WIDTH,
           ALSA_CLOCK_TEST_PERIOD_BYTES,
           ALSA_CLOCK_TEST_HOLD_MS);
    printf("note : command keeps silence output for scope measurement only\r\n");
    printf("example : minialsa_output_clock_test 48000\r\n");
}

static const alsa_sample_t *alsa_find_sample(const char *name)
{
    size_t i;

    if (name == NULL) {
        return &g_samples[0];
    }

    for (i = 0; i < sizeof(g_samples) / sizeof(g_samples[0]); i++) {
        if (strcmp(name, g_samples[i].name) == 0) {
            return &g_samples[i];
        }
    }

    return NULL;
}

static int alsa_parse_u32_arg(const char *text, uint32_t *value)
{
    char *end = NULL;
    unsigned long parsed;

    if (text == NULL || value == NULL || text[0] == '\0') {
        return -1;
    }

    errno = 0;
    parsed = strtoul(text, &end, 0);
    if (errno != 0 || end == text || (end != NULL && *end != '\0')) {
        return -1;
    }

    *value = (uint32_t)parsed;
    return 0;
}

static uint32_t alsa_clock_test_transfer_bytes(uint32_t channels, uint32_t slot_width)
{
    uint32_t slot_bytes = (slot_width + 7U) / 8U;

    if ((slot_bytes != 1U) && (slot_bytes != 2U) && (slot_bytes != 4U)) {
        return 0U;
    }

    if (channels == 1U) {
        return slot_bytes;
    }

    if (channels == 2U) {
        return (slot_bytes == 1U) ? 2U : 4U;
    }

    return 0U;
}

static uint32_t alsa_clock_test_buffer_bytes(uint32_t period_bytes)
{
    uint64_t total = (uint64_t)period_bytes * ALSA_CLOCK_TEST_BUFFER_PERIODS +
                     ALSA_CLOCK_TEST_BUFFER_PADDING;

    if (total > UINT32_MAX) {
        return 0U;
    }

    return (uint32_t)total;
}

static int alsa_validate_output_clock_test_cfg(const alsa_output_clock_test_cfg_t *cfg)
{
    uint32_t transfer_bytes;

    if (cfg == NULL) {
        return -1;
    }

    if ((cfg->sample_rate_hz == 0U) ||
        (cfg->sample_rate_hz > ALSA_TEST_MAX_RATE_HZ) ||
        (cfg->sample_rate_hz > (UINT32_MAX / 1000U))) {
        printf("[clock_test] invalid sample_rate_hz=%lu, expected 1..%u\r\n",
               (unsigned long)cfg->sample_rate_hz, ALSA_TEST_MAX_RATE_HZ);
        return -1;
    }

    if ((cfg->sample_rate_hz % 8000U) != 0U) {
        printf("[clock_test] unsupported sample_rate_hz=%lu, expected multiple of 8000 Hz\r\n",
               (unsigned long)cfg->sample_rate_hz);
        return -1;
    }

    if (ALSA_CLOCK_TEST_PERIOD_BYTES < ALSA_CLOCK_TEST_MIN_PERIOD_BYTES ||
        (ALSA_CLOCK_TEST_PERIOD_BYTES & (ALSA_CLOCK_TEST_MIN_PERIOD_BYTES - 1U)) != 0U) {
        printf("[clock_test] invalid period_bytes=%lu, expected multiple of %u and >= %u\r\n",
               (unsigned long)ALSA_CLOCK_TEST_PERIOD_BYTES,
               ALSA_CLOCK_TEST_MIN_PERIOD_BYTES,
               ALSA_CLOCK_TEST_MIN_PERIOD_BYTES);
        return -1;
    }

    transfer_bytes = alsa_clock_test_transfer_bytes(ALSA_CLOCK_TEST_CHANNELS, ALSA_CLOCK_TEST_SLOT_WIDTH);
    if ((transfer_bytes == 0U) || ((ALSA_CLOCK_TEST_PERIOD_BYTES % transfer_bytes) != 0U)) {
        printf("[clock_test] invalid period_bytes=%lu for channels=%lu slot_width=%lu\r\n",
               (unsigned long)ALSA_CLOCK_TEST_PERIOD_BYTES,
               (unsigned long)ALSA_CLOCK_TEST_CHANNELS,
               (unsigned long)ALSA_CLOCK_TEST_SLOT_WIDTH);
        return -1;
    }

    if (ALSA_CLOCK_TEST_HOLD_MS == 0U) {
        printf("[clock_test] invalid hold_ms=0\r\n");
        return -1;
    }

    if (alsa_clock_test_buffer_bytes(ALSA_CLOCK_TEST_PERIOD_BYTES) == 0U) {
        printf("[clock_test] buffer size overflow, period_bytes=%lu\r\n",
               (unsigned long)ALSA_CLOCK_TEST_PERIOD_BYTES);
        return -1;
    }

    return 0;
}

static int alsa_test_expect_ret(alsa_test_stats_t *stats, const char *name, int ret)
{
    if (ret < 0) {
        printf("[api_test][FAIL] %s -> %d\r\n", name, ret);
        stats->failed++;
        return ret;
    }

    printf("[api_test][PASS] %s\r\n", name);
    stats->passed++;
    return 0;
}

static int alsa_test_expect_eq(alsa_test_stats_t *stats, const char *name,
                               long long actual, long long expected)
{
    if (actual != expected) {
        printf("[api_test][FAIL] %s expect=%lld actual=%lld\r\n",
               name, expected, actual);
        stats->failed++;
        return -1;
    }

    printf("[api_test][PASS] %s -> %lld\r\n", name, actual);
    stats->passed++;
    return 0;
}

static int alsa_test_expect_positive(alsa_test_stats_t *stats, const char *name, long long actual)
{
    if (actual <= 0) {
        printf("[api_test][FAIL] %s expect>0 actual=%lld\r\n", name, actual);
        stats->failed++;
        return -1;
    }

    printf("[api_test][PASS] %s -> %lld\r\n", name, actual);
    stats->passed++;
    return 0;
}

static size_t alsa_frame_bytes(unsigned int channels, snd_pcm_format_t format)
{
    return ((size_t)channels * (size_t)snd_pcm_format_physical_width(format)) / 8U;
}

static unsigned int alsa_sample_bit_width(const alsa_sample_t *sample)
{
    if (sample == NULL) {
        return 0U;
    }

    return (unsigned int)snd_pcm_format_width(sample->format);
}

static unsigned int alsa_sample_slot_width(const alsa_sample_t *sample)
{
    if (sample == NULL) {
        return 0U;
    }

    return (unsigned int)snd_pcm_format_physical_width(sample->format);
}

static unsigned int alsa_calc_bclk_hz(unsigned int rate, unsigned int channels, unsigned int slot_width)
{
    unsigned int frame_slots = (channels == 1U) ? 2U : channels;

    return rate * frame_slots * slot_width;
}

static void alsa_dump_sample_config(const char *tag, const alsa_sample_t *sample)
{
    unsigned int bit_width;
    unsigned int slot_width;
    unsigned int bclk_hz;

    if (sample == NULL) {
        printf("[%s] sample=<null>\r\n", tag);
        return;
    }

    bit_width = alsa_sample_bit_width(sample);
    slot_width = alsa_sample_slot_width(sample);
    bclk_hz = alsa_calc_bclk_hz(sample->rate, sample->channels, slot_width);

    printf("[%s] sample=%s fmt=%d bit_width=%u slot_width=%u ch=%u rate=%u wire_bclk=%u len=%u bytes\r\n",
           tag,
           sample->name,
           sample->format,
           bit_width,
           slot_width,
           sample->channels,
           sample->rate,
           bclk_hz,
           (unsigned int)(sample->len ? *sample->len : 0U));
}

static void alsa_dump_i2s_bclk_div(const char *tag)
{
#if defined(CONFIG_CODEC_USE_I2S) && CONFIG_CODEC_USE_I2S && \
    (defined(BL616) || defined(BL616CL) || defined(BL618DG) || defined(BL702))
    uint32_t reg;
    uint32_t div_l;
    uint32_t div_h;
    uint32_t div;

    reg = *(volatile uint32_t *)(uintptr_t)(I2S_BASE + I2S_BCLK_CONFIG_OFFSET);
    div_l = (reg & I2S_CR_BCLK_DIV_L_MASK) >> I2S_CR_BCLK_DIV_L_SHIFT;
    div_h = (reg & I2S_CR_BCLK_DIV_H_MASK) >> I2S_CR_BCLK_DIV_H_SHIFT;
    div = div_l + div_h;

    printf("[%s] I2S_BCLK_CONFIG=0x%08lx div_l=%lu div_h=%lu div=%lu ratio=%lu\r\n",
           tag,
           (unsigned long)reg,
           (unsigned long)div_l,
           (unsigned long)div_h,
           (unsigned long)div,
           (unsigned long)(div + 2U));
#else
    printf("[%s] I2S_CR_BCLK_DIV dump unsupported on this build\r\n", tag);
#endif
}

static void alsa_dump_i2s_ref_clk_div(const char *tag)
{
#if defined(CONFIG_CODEC_USE_I2S) && CONFIG_CODEC_USE_I2S && \
    (defined(BL616) || defined(BL616CL) || defined(BL618DG) || defined(BL702))
    uint32_t reg;
    uint32_t ref_div;

    reg = *(volatile uint32_t *)(uintptr_t)(GLB_BASE + GLB_I2S_CFG0_OFFSET);
    ref_div = (reg & GLB_REG_I2S_REF_CLK_DIV_MSK) >> GLB_REG_I2S_REF_CLK_DIV_POS;

    printf("[%s] GLB_I2S_CFG0=0x%08lx ref_clk_div=%lu ratio=%lu\r\n",
           tag,
           (unsigned long)reg,
           (unsigned long)ref_div,
           (unsigned long)(ref_div + 1U));
#else
    printf("[%s] I2S ref clk div dump unsupported on this build\r\n", tag);
#endif
}

static size_t alsa_fill_period_buffer(const alsa_sample_t *sample, size_t *offset,
                                      void *buffer, size_t bytes)
{
    size_t remain;
    size_t copy_bytes;

    memset(buffer, 0, bytes);
    if (sample == NULL || offset == NULL || buffer == NULL || bytes == 0) {
        return 0;
    }

    if (*offset >= *sample->len) {
        return 0;
    }

    remain = *sample->len - *offset;
    copy_bytes = remain < bytes ? remain : bytes;
    memcpy(buffer, sample->data + *offset, copy_bytes);
    *offset += copy_bytes;

    return copy_bytes;
}

static void alsa_fill_loop_buffer(const alsa_sample_t *sample, size_t *offset,
                                  void *buffer, size_t bytes)
{
    unsigned char *dst = (unsigned char *)buffer;
    size_t filled = 0;
    size_t copy_bytes;

    memset(buffer, 0, bytes);
    if (sample == NULL || offset == NULL || buffer == NULL || bytes == 0 || *sample->len == 0) {
        return;
    }

    while (filled < bytes) {
        if (*offset >= *sample->len) {
            *offset = 0;
        }

        copy_bytes = *sample->len - *offset;
        if (copy_bytes > (bytes - filled)) {
            copy_bytes = bytes - filled;
        }

        memcpy(dst + filled, sample->data + *offset, copy_bytes);
        *offset += copy_bytes;
        filled += copy_bytes;
    }
}

static int alsa_test_capture_poll_avail(msp_pcm_t *pcm, msp_pcm_uframes_t period_size)
{
    int elapsed_ms = 0;
    msp_pcm_sframes_t avail = 0;

    while (elapsed_ms <= ALSA_TEST_CAPTURE_POLL_MS) {
        avail = msp_pcm_avail(pcm);
        if (avail < 0 || avail >= period_size) {
            return (int)avail;
        }
        if (elapsed_ms == ALSA_TEST_CAPTURE_POLL_MS) {
            break;
        }

        msp_msleep(ALSA_TEST_CAPTURE_POLL_STEP_MS);
        elapsed_ms += ALSA_TEST_CAPTURE_POLL_STEP_MS;
    }

    return (int)avail;
}

static void alsa_clock_test_output_event_cb(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    (void)output;
    (void)event;
    (void)arg;
}

/* ─── Playback task ───────────────────────────────────── */
static void alsa_play_task(void *arg)
{
    const alsa_sample_t *s = (const alsa_sample_t *)arg;

    msp_pcm_t          *pcm      = NULL;
    msp_pcm_hw_params_t *tmp_hw  = NULL;   /* For demonstrating malloc/free lifecycle only */
    msp_pcm_hw_params_t *hw      = NULL;   /* Actual config, freed locally after apply */
    msp_pcm_sw_params_t *sw      = NULL;   /* Freed locally after apply */
    unsigned char       *tail_buf = NULL;

    int               ret, dir = 0;
    msp_pcm_uframes_t period_size = ALSA_PLAY_PERIOD_FRAMES;
    msp_pcm_uframes_t buffer_size = ALSA_PLAY_BUFFER_FRAMES;
    unsigned int      rate        = 0;
    unsigned int      channels    = 0;
    snd_pcm_format_t  format      = SND_PCM_FORMAT_UNKNOWN;

    alsa_dump_sample_config("alsa_play", s);

    /* ── 1. Open PCM device ──────────────────────────── */
    ret = msp_pcm_open(&pcm, ALSA_PLAY_PCM_NAME, MSP_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        printf("[alsa_play] msp_pcm_open(%s) -> %d\r\n", ALSA_PLAY_PCM_NAME, ret);
        goto out;
    }

    /* ── 2. Demonstrate hw_params malloc/free lifecycle ── */
    ret = msp_pcm_hw_params_malloc(&tmp_hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params_malloc(tmp) -> %d\r\n", ret);
        goto out_close;
    }
    msp_pcm_hw_params_free(tmp_hw);
    tmp_hw = NULL;

    /* ── 3. Allocate and configure actual hw_params ──── */
    ret = msp_pcm_hw_params_malloc(&hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params_malloc -> %d\r\n", ret);
        goto out_close;
    }

    /* Initialize to defaults before applying to the PCM */
    ret = msp_pcm_hw_params_any(pcm, hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params_any -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* Interleaved read/write mode */
    ret = msp_pcm_hw_params_set_access(pcm, hw, MSP_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0) {
        printf("[alsa_play] set_access -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* Sample bit depth */
    ret = msp_pcm_hw_params_set_format(pcm, hw, s->format);
    if (ret < 0) {
        printf("[alsa_play] set_format -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* Channel count */
    ret = msp_pcm_hw_params_set_channels(pcm, hw, s->channels);
    if (ret < 0) {
        printf("[alsa_play] set_channels -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* Sample rate */
    ret = msp_pcm_hw_params_set_rate(pcm, hw, s->rate, 0);
    if (ret < 0) {
        printf("[alsa_play] set_rate -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* Buffer / period size (near auto-aligns to hardware-supported values) */
    ret = msp_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer_size);
    if (ret < 0) {
        printf("[alsa_play] set_buffer_size_near -> %d\r\n", ret);
        goto out_free_hw;
    }

    ret = msp_pcm_hw_params_set_period_size_near(pcm, hw, &period_size, &dir);
    if (ret < 0) {
        printf("[alsa_play] set_period_size_near -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* Read back actual negotiated values */
    msp_pcm_hw_params_get_period_size(hw, &period_size, &dir);
    msp_pcm_hw_params_get_buffer_size(hw, &buffer_size);
    msp_pcm_hw_params_get_rate(hw, &rate, &dir);
    msp_pcm_hw_params_get_channels(hw, &channels);
    msp_pcm_hw_params_get_format(hw, &format);
    printf("[alsa_play] hw: fmt=%d, width=%d bits, ch=%u, rate=%u Hz, period=%d, buf=%d\r\n",
           format, snd_pcm_format_width((snd_pcm_format_t)format),
           channels, rate, (int)period_size, (int)buffer_size);

    /* Apply hw_params (triggers low-level xcodec / I2S initialization) */
    ret = msp_pcm_hw_params(pcm, hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params -> %d\r\n", ret);
        goto out_free_hw;
    }
    alsa_dump_i2s_ref_clk_div("alsa_play");
    alsa_dump_i2s_bclk_div("alsa_play");
    msp_pcm_hw_params_free(hw);
    hw = NULL;

    /* ── 4. Configure sw_params ──────────────────────── */
    ret = msp_pcm_sw_params_alloca(&sw);
    if (ret < 0) {
        printf("[alsa_play] sw_params_alloca -> %d\r\n", ret);
        goto out_close;
    }

    ret = msp_pcm_sw_params_current(pcm, sw);
    if (ret < 0) {
        printf("[alsa_play] sw_params_current -> %d\r\n", ret);
        goto out_free_sw;
    }

    /* Auto-start DMA when one period is filled */
    ret = msp_pcm_sw_params_set_start_threshold(pcm, sw, period_size);
    if (ret < 0) {
        printf("[alsa_play] set_start_threshold -> %d\r\n", ret);
        goto out_free_sw;
    }

    /* Stop when buffer is full */
    ret = msp_pcm_sw_params_set_stop_threshold(pcm, sw, buffer_size);
    if (ret < 0) {
        printf("[alsa_play] set_stop_threshold -> %d\r\n", ret);
        goto out_free_sw;
    }

    {
        msp_pcm_uframes_t start_th = 0;
        msp_pcm_sw_params_get_start_threshold(sw, &start_th);
        printf("[alsa_play] sw start_threshold=%d\r\n", (int)start_th);
    }

    /* Apply sw_params to the PCM runtime state */
    ret = msp_pcm_sw_params(pcm, sw);
    if (ret < 0) {
        printf("[alsa_play] sw_params -> %d\r\n", ret);
        goto out_free_sw;
    }
    msp_pcm_sw_params_free(sw);
    sw = NULL;

    /* ── 5. Write audio data ─────────────────────────── */
    size_t frame_bytes = ((size_t)s->channels *
                          (size_t)snd_pcm_format_physical_width(s->format)) / 8U;
    size_t chunk_bytes = (size_t)period_size * frame_bytes;

    tail_buf = msp_zalloc_check(chunk_bytes);
    if (tail_buf == NULL) {
        printf("[alsa_play] alloc tail_buf(%u) failed\r\n", (unsigned int)chunk_bytes);
        ret = -1;
        goto out_close;
    }

    printf("[alsa_play] playing '%s', len=%u bytes, chunk=%u bytes\r\n",
           s->name, *s->len, (unsigned int)chunk_bytes);

    size_t offset = 0;
    while (offset < *s->len) {
        size_t remain     = *s->len - offset;
        size_t copy_bytes = (remain < chunk_bytes) ? remain : chunk_bytes;
        int write_ret;

        if (copy_bytes < chunk_bytes) {
            /* Last chunk is incomplete, pad with silence */
            memset(tail_buf, 0, chunk_bytes);
            memcpy(tail_buf, s->data + offset, copy_bytes);
            write_ret = msp_pcm_writei(pcm, tail_buf, period_size);
        } else {
            write_ret = msp_pcm_writei(pcm, s->data + offset, period_size);
        }

        if (write_ret < 0) {
            /* Xrun recovery */
            ret = msp_pcm_recover(pcm, write_ret, 0);
            if (ret < 0) {
                printf("[alsa_play] recover -> %d\r\n", ret);
                goto out_tail;
            }
            continue;
        }
        offset += copy_bytes;
    }

    /* Wait for hardware to finish playing all data */
    msp_pcm_drain(pcm);
    printf("[alsa_play] done, total_frames=%d\r\n",
           msp_pcm_bytes_to_frames(pcm, (ssize_t)*s->len));
    ret = 0;

out_tail:
    msp_free(tail_buf);
    goto out_close;

out_free_sw:
    if (sw) {
        msp_pcm_sw_params_free(sw);
    }
    goto out_close;

out_free_hw:
    if (hw) {
        msp_pcm_hw_params_free(hw);
    }

out_close:
    msp_pcm_close(pcm);

out:
    g_alsa_running = 0;
    msp_task_exit(0);
}

static int alsa_test_playback_refined_api(const alsa_sample_t *sample, alsa_test_stats_t *stats)
{
    msp_pcm_t *pcm = NULL;
    msp_pcm_hw_params_t *hw = NULL;
    msp_pcm_hw_params_t *cur_hw = NULL;
    msp_pcm_sw_params_t *sw = NULL;
    msp_pcm_sw_params_t *cur_sw = NULL;
    unsigned char *period_buf = NULL;
    msp_pcm_uframes_t period_size = ALSA_TEST_PERIOD_FRAMES;
    msp_pcm_uframes_t buffer_size = ALSA_TEST_BUFFER_FRAMES;
    msp_pcm_uframes_t start_threshold = 0;
    unsigned int rate = 0;
    unsigned int channels = 0;
    snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
    size_t frame_bytes;
    size_t offset = 0;
    size_t period_bytes;
    msp_pcm_uframes_t target_frames;
    msp_pcm_uframes_t played_frames = 0;
    msp_pcm_uframes_t chunk_frames;
    int dir = -1;
    int ret;
    unsigned int slot_width = 0;

    printf("[api_test] playback refined api begin\r\n");
    alsa_dump_sample_config("api_test", sample);

    ret = msp_pcm_open(&pcm, ALSA_PLAY_PCM_NAME, MSP_PCM_STREAM_PLAYBACK, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_open(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_malloc(&hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_malloc(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_any(pcm, hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_any(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_get_channels(hw, &channels);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_channels(default)", ret) < 0 ||
        alsa_test_expect_eq(stats, "default channels", channels, 2) < 0) {
        goto out;
    }

    dir = -1;
    ret = msp_pcm_hw_params_get_rate(hw, &rate, &dir);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_rate(default)", ret) < 0 ||
        alsa_test_expect_eq(stats, "default rate", rate, 16000) < 0 ||
        alsa_test_expect_eq(stats, "default rate dir", dir, 0) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_get_format(hw, &format);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_format(default)", ret) < 0 ||
        alsa_test_expect_eq(stats, "default format width", snd_pcm_format_width(format), 16) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_access(pcm, hw, MSP_PCM_ACCESS_RW_INTERLEAVED);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_access(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_format(pcm, hw, sample->format);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_format(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_channels(pcm, hw, sample->channels);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_channels(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_rate(pcm, hw, sample->rate, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_rate(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer_size);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_buffer_size_near(playback)", ret) < 0) {
        goto out;
    }

    dir = -1;
    ret = msp_pcm_hw_params_set_period_size_near(pcm, hw, &period_size, &dir);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_period_size_near(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "playback period dir", dir, 0) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_get_period_size(hw, &period_size, &dir);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_period_size(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "playback get period dir", dir, 0) < 0 ||
        alsa_test_expect_eq(stats, "playback period size", period_size, ALSA_TEST_PERIOD_FRAMES) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_get_buffer_size(hw, &buffer_size);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_buffer_size(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "playback buffer size", buffer_size, ALSA_TEST_BUFFER_FRAMES) < 0) {
        goto out;
    }

    dir = -1;
    ret = msp_pcm_hw_params_get_rate(hw, &rate, &dir);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_rate(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "playback rate", rate, sample->rate) < 0 ||
        alsa_test_expect_eq(stats, "playback rate get dir", dir, 0) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_get_channels(hw, &channels);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_channels(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "playback channels", channels, sample->channels) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_get_format(hw, &format);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_get_format(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "playback format", format, sample->format) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params(pcm, hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_alloca(&cur_hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_alloca(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_current(pcm, cur_hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_current(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "current playback access", cur_hw->access, MSP_PCM_ACCESS_RW_INTERLEAVED) < 0 ||
        alsa_test_expect_eq(stats, "current playback format", cur_hw->format, sample->format) < 0 ||
        alsa_test_expect_eq(stats, "current playback rate", cur_hw->rate, sample->rate) < 0 ||
        alsa_test_expect_eq(stats, "current playback channels", cur_hw->channels, sample->channels) < 0) {
        goto out;
    }

    slot_width = (cur_hw->channels > 0) ? (unsigned int)(cur_hw->frame_bits / cur_hw->channels) : 0U;
    printf("[api_test] playback hw: fmt=%d width=%d bits slot_width=%u ch=%d rate=%d wire_bclk=%u period=%d buf=%d play_ms=%d\r\n",
           cur_hw->format,
           cur_hw->sample_bits,
           slot_width,
           cur_hw->channels,
           cur_hw->rate,
           alsa_calc_bclk_hz((unsigned int)cur_hw->rate,
                             (unsigned int)cur_hw->channels,
                             slot_width),
           cur_hw->period_size,
           cur_hw->buffer_size,
           ALSA_TEST_PLAYBACK_MS);
    alsa_dump_i2s_ref_clk_div("api_test");
    alsa_dump_i2s_bclk_div("api_test");

    ret = msp_pcm_recover(pcm, -EPIPE, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_recover(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params_alloca(&sw);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params_alloca(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params_current(pcm, sw);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params_current(default)", ret) < 0 ||
        alsa_test_expect_eq(stats, "default start_threshold", sw->start_threshold, period_size) < 0 ||
        alsa_test_expect_eq(stats, "default stop_threshold", sw->stop_threshold, buffer_size) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params_set_start_threshold(pcm, sw, period_size);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params_set_start_threshold(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params_get_start_threshold(sw, &start_threshold);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params_get_start_threshold(playback)", ret) < 0 ||
        alsa_test_expect_eq(stats, "playback start_threshold", start_threshold, period_size) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params_set_stop_threshold(pcm, sw, buffer_size);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params_set_stop_threshold(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params(pcm, sw);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params(playback)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params_alloca(&cur_sw);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params_alloca(current)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_sw_params_current(pcm, cur_sw);
    if (alsa_test_expect_ret(stats, "msp_pcm_sw_params_current(applied)", ret) < 0 ||
        alsa_test_expect_eq(stats, "current start_threshold", cur_sw->start_threshold, period_size) < 0 ||
        alsa_test_expect_eq(stats, "current stop_threshold", cur_sw->stop_threshold, buffer_size) < 0) {
        goto out;
    }

    frame_bytes = alsa_frame_bytes(sample->channels, sample->format);
    period_bytes = (size_t)period_size * frame_bytes;
    target_frames = (msp_pcm_uframes_t)(((unsigned int)sample->rate * ALSA_TEST_PLAYBACK_MS) / 1000U);
    if (target_frames < (period_size * 2)) {
        target_frames = period_size * 2;
    }

    period_buf = msp_zalloc_check(period_bytes);
    if (period_buf == NULL) {
        printf("[api_test][FAIL] alloc playback period buffer\r\n");
        stats->failed++;
        goto out;
    }

    alsa_fill_loop_buffer(sample, &offset, period_buf, period_bytes);
    ret = msp_pcm_writei(pcm, period_buf, period_size);
    if (alsa_test_expect_positive(stats, "msp_pcm_writei(playback first)", ret) < 0) {
        goto out;
    }
    played_frames += ret;

    ret = msp_pcm_pause(pcm, 1);
    if (alsa_test_expect_ret(stats, "msp_pcm_pause(enable)", ret) < 0) {
        goto out;
    }

    msp_msleep(20);

    ret = msp_pcm_pause(pcm, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_pause(disable)", ret) < 0) {
        goto out;
    }

    alsa_fill_loop_buffer(sample, &offset, period_buf, period_bytes);
    ret = msp_pcm_writei(pcm, period_buf, period_size);
    if (alsa_test_expect_positive(stats, "msp_pcm_writei(playback second)", ret) < 0) {
        goto out;
    }
    played_frames += ret;

    while (played_frames < target_frames) {
        chunk_frames = target_frames - played_frames;
        if (chunk_frames > period_size) {
            chunk_frames = period_size;
        }

        alsa_fill_loop_buffer(sample, &offset, period_buf, (size_t)chunk_frames * frame_bytes);
        ret = msp_pcm_writei(pcm, period_buf, chunk_frames);
        if (ret <= 0) {
            printf("[api_test][FAIL] msp_pcm_writei(playback continue) -> %d\r\n", ret);
            stats->failed++;
            goto out;
        }

        played_frames += ret;
    }

    if (alsa_test_expect_eq(stats, "playback duration frames", played_frames, target_frames) < 0) {
        goto out;
    }

    ret = msp_pcm_drain(pcm);
    if (alsa_test_expect_ret(stats, "msp_pcm_drain(playback)", ret) < 0) {
        goto out;
    }

    ret = 0;

out:
    if (period_buf != NULL) {
        msp_free(period_buf);
    }
    if (cur_sw != NULL) {
        msp_pcm_sw_params_free(cur_sw);
    }
    if (sw != NULL) {
        msp_pcm_sw_params_free(sw);
    }
    if (cur_hw != NULL) {
        msp_pcm_hw_params_free(cur_hw);
    }
    if (hw != NULL) {
        msp_pcm_hw_params_free(hw);
    }
    if (pcm != NULL) {
        msp_pcm_close(pcm);
    }

    return ret;
}

static int alsa_test_capture_interleaved_api(alsa_test_stats_t *stats)
{
    msp_pcm_t *pcm = NULL;
    msp_pcm_hw_params_t *hw = NULL;
    msp_pcm_hw_params_t *cur_hw = NULL;
    unsigned char *capture_buf = NULL;
    msp_pcm_uframes_t period_size = ALSA_TEST_PERIOD_FRAMES;
    msp_pcm_uframes_t buffer_size = ALSA_TEST_BUFFER_FRAMES;
    unsigned int rate = 16000;
    size_t capture_bytes;
    int dir = -1;
    int ret;

    printf("[api_test] capture interleaved api begin\r\n");

    ret = msp_pcm_open(&pcm, ALSA_CAP_PCM_NAME, MSP_PCM_STREAM_CAPTURE, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_open(capture interleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_alloca(&hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_alloca(capture interleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_any(pcm, hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_any(capture interleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_access(pcm, hw, MSP_PCM_ACCESS_RW_INTERLEAVED);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_access(capture interleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_format(capture interleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_channels(pcm, hw, 1);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_channels(capture interleaved)", ret) < 0) {
        goto out;
    }

    dir = -1;
    ret = msp_pcm_hw_params_set_rate_near(pcm, hw, &rate, &dir);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_rate_near(capture interleaved)", ret) < 0 ||
        alsa_test_expect_eq(stats, "capture interleaved rate dir", dir, 0) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer_size);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_buffer_size_near(capture interleaved)", ret) < 0) {
        goto out;
    }

    dir = -1;
    ret = msp_pcm_hw_params_set_period_size_near(pcm, hw, &period_size, &dir);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_period_size_near(capture interleaved)", ret) < 0 ||
        alsa_test_expect_eq(stats, "capture interleaved period dir", dir, 0) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params(pcm, hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params(capture interleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_malloc(&cur_hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_malloc(capture current)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_current(pcm, cur_hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_current(capture interleaved)", ret) < 0 ||
        alsa_test_expect_eq(stats, "current capture access", cur_hw->access, MSP_PCM_ACCESS_RW_INTERLEAVED) < 0 ||
        alsa_test_expect_eq(stats, "current capture rate", cur_hw->rate, rate) < 0 ||
        alsa_test_expect_eq(stats, "current capture channels", cur_hw->channels, 1) < 0) {
        goto out;
    }

    ret = msp_pcm_avail(pcm);
    if (ret < 0) {
        printf("[api_test][FAIL] msp_pcm_avail(capture interleaved) -> %d\r\n", ret);
        stats->failed++;
        goto out;
    }
    printf("[api_test][PASS] msp_pcm_avail(capture interleaved) -> %d\r\n", ret);
    stats->passed++;

    ret = alsa_test_capture_poll_avail(pcm, period_size);
    if (ret < 0) {
        printf("[api_test][FAIL] capture poll avail(interleaved) -> %d\r\n", ret);
        stats->failed++;
        goto out;
    }
    if (ret < period_size) {
        printf("[api_test] skip msp_pcm_readi(capture interleaved), avail=%d frames < period=%d\r\n",
               ret, period_size);
        ret = 0;
        goto out;
    }

    capture_bytes = (size_t)msp_pcm_frames_to_bytes(pcm, period_size);
    capture_buf = msp_zalloc_check(capture_bytes);
    if (capture_buf == NULL) {
        printf("[api_test][FAIL] alloc capture interleaved buffer\r\n");
        stats->failed++;
        goto out;
    }

    ret = msp_pcm_readi(pcm, capture_buf, period_size);
    if (alsa_test_expect_positive(stats, "msp_pcm_readi(capture interleaved)", ret) < 0) {
        goto out;
    }

    ret = 0;

out:
    if (capture_buf != NULL) {
        msp_free(capture_buf);
    }
    if (cur_hw != NULL) {
        msp_pcm_hw_params_free(cur_hw);
    }
    if (hw != NULL) {
        msp_pcm_hw_params_free(hw);
    }
    if (pcm != NULL) {
        msp_pcm_close(pcm);
    }

    return ret;
}

static int alsa_test_capture_noninterleaved_api(alsa_test_stats_t *stats)
{
    msp_pcm_t *pcm = NULL;
    msp_pcm_hw_params_t *hw = NULL;
    unsigned char *capture_buf = NULL;
    void *bufs[1] = {NULL};
    msp_pcm_uframes_t period_size = ALSA_TEST_PERIOD_FRAMES;
    msp_pcm_uframes_t buffer_size = ALSA_TEST_BUFFER_FRAMES;
    size_t capture_bytes;
    int ret;

    printf("[api_test] capture noninterleaved api begin\r\n");

    ret = msp_pcm_open(&pcm, ALSA_CAP_PCM_NAME, MSP_PCM_STREAM_CAPTURE, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_open(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_malloc(&hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_malloc(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_any(pcm, hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_any(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_access(pcm, hw, MSP_PCM_ACCESS_RW_NONINTERLEAVED);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_access(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_format(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_channels(pcm, hw, 1);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_channels(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_rate(pcm, hw, 16000, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_rate(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer_size);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_buffer_size_near(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_set_period_size_near(pcm, hw, &period_size, NULL);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_set_period_size_near(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params(pcm, hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_avail(pcm);
    if (ret < 0) {
        printf("[api_test][FAIL] msp_pcm_avail(capture noninterleaved) -> %d\r\n", ret);
        stats->failed++;
        goto out;
    }
    printf("[api_test][PASS] msp_pcm_avail(capture noninterleaved) -> %d\r\n", ret);
    stats->passed++;

    ret = alsa_test_capture_poll_avail(pcm, period_size);
    if (ret < 0) {
        printf("[api_test][FAIL] capture poll avail(noninterleaved) -> %d\r\n", ret);
        stats->failed++;
        goto out;
    }
    if (ret < period_size) {
        printf("[api_test] skip msp_pcm_readn(capture noninterleaved), avail=%d frames < period=%d\r\n",
               ret, period_size);
        ret = 0;
        goto out;
    }

    capture_bytes = (size_t)msp_pcm_frames_to_bytes(pcm, period_size);
    capture_buf = msp_zalloc_check(capture_bytes);
    if (capture_buf == NULL) {
        printf("[api_test][FAIL] alloc capture noninterleaved buffer\r\n");
        stats->failed++;
        goto out;
    }

    bufs[0] = capture_buf;
    ret = msp_pcm_readn(pcm, bufs, period_size);
    if (alsa_test_expect_positive(stats, "msp_pcm_readn(capture noninterleaved)", ret) < 0) {
        goto out;
    }

    ret = 0;

out:
    if (capture_buf != NULL) {
        msp_free(capture_buf);
    }
    if (hw != NULL) {
        msp_pcm_hw_params_free(hw);
    }
    if (pcm != NULL) {
        msp_pcm_close(pcm);
    }

    return ret;
}

static int alsa_test_set_params_api(const alsa_sample_t *sample, alsa_test_stats_t *stats)
{
    msp_pcm_t *pcm = NULL;
    msp_pcm_hw_params_t *cur_hw = NULL;
    unsigned char *period_buf = NULL;
    size_t frame_bytes;
    size_t offset = 0;
    msp_pcm_uframes_t period_size = 0;
    int ret;

    printf("[api_test] set_params api begin\r\n");

    ret = msp_pcm_open(&pcm, ALSA_PLAY_PCM_NAME, MSP_PCM_STREAM_PLAYBACK, 0);
    if (alsa_test_expect_ret(stats, "msp_pcm_open(set_params)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_set_params(pcm, sample->format, MSP_PCM_ACCESS_RW_INTERLEAVED,
                             sample->channels, sample->rate, 0, 200000);
    if (alsa_test_expect_ret(stats, "msp_pcm_set_params", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_alloca(&cur_hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_alloca(set_params)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_hw_params_current(pcm, cur_hw);
    if (alsa_test_expect_ret(stats, "msp_pcm_hw_params_current(set_params)", ret) < 0 ||
        alsa_test_expect_eq(stats, "set_params current format", cur_hw->format, sample->format) < 0 ||
        alsa_test_expect_eq(stats, "set_params current channels", cur_hw->channels, sample->channels) < 0 ||
        alsa_test_expect_eq(stats, "set_params current rate", cur_hw->rate, sample->rate) < 0) {
        goto out;
    }

    period_size = cur_hw->period_size;
    if (alsa_test_expect_positive(stats, "set_params period_size", period_size) < 0) {
        goto out;
    }

    frame_bytes = alsa_frame_bytes(sample->channels, sample->format);
    period_buf = msp_zalloc_check((size_t)period_size * frame_bytes);
    if (period_buf == NULL) {
        printf("[api_test][FAIL] alloc set_params buffer\r\n");
        stats->failed++;
        goto out;
    }

    alsa_fill_period_buffer(sample, &offset, period_buf, (size_t)period_size * frame_bytes);
    ret = msp_pcm_writei(pcm, period_buf, period_size);
    if (alsa_test_expect_positive(stats, "msp_pcm_writei(set_params)", ret) < 0) {
        goto out;
    }

    ret = msp_pcm_drop(pcm);
    if (alsa_test_expect_ret(stats, "msp_pcm_drop(set_params)", ret) < 0) {
        goto out;
    }

    ret = 0;

out:
    if (period_buf != NULL) {
        msp_free(period_buf);
    }
    if (cur_hw != NULL) {
        msp_pcm_hw_params_free(cur_hw);
    }
    if (pcm != NULL) {
        msp_pcm_close(pcm);
    }

    return ret;
}

static void alsa_api_test_task(void *arg)
{
    alsa_api_test_cfg_t cfg = *(alsa_api_test_cfg_t *)arg;
    alsa_test_stats_t stats = {0};
    int ret = 0;

    msp_free(arg);

    printf("[api_test] sample=%s playback=%d capture=%d set_params=%d\r\n",
           cfg.sample->name, cfg.run_playback, cfg.run_capture, cfg.run_set_params);

    if (cfg.run_playback) {
        ret = alsa_test_playback_refined_api(cfg.sample, &stats);
    }
    if (ret == 0 && cfg.run_capture) {
        ret = alsa_test_capture_interleaved_api(&stats);
    }
    if (ret == 0 && cfg.run_capture) {
        ret = alsa_test_capture_noninterleaved_api(&stats);
    }
    if (ret == 0 && cfg.run_set_params) {
        ret = alsa_test_set_params_api(cfg.sample, &stats);
    }

    printf("[api_test] summary pass=%u fail=%u result=%s\r\n",
           stats.passed, stats.failed,
           (ret == 0 && stats.failed == 0) ? "PASS" : "FAIL");

    g_alsa_api_test_running = 0;
    msp_task_exit(0);
}

static void alsa_output_clock_test_task(void *arg)
{
    alsa_output_clock_test_cfg_t cfg;
    alsa_test_stats_t stats = { 0 };
    xcodec_t codec = { 0 };
    xcodec_output_t output_ch = { 0 };
    xcodec_output_config_t output_cfg = { 0 };
    m_ringbuf_t ring_buffer = { 0 };
    xcodec_dma_ch_t *dma_hdl = NULL;
    uint8_t *output_buf = NULL;
    uint8_t *silence_buf = NULL;
    uint32_t output_clock_mhz;
    uint32_t current_clock_mhz = 0;
    uint32_t buffer_bytes;
    uint64_t start_us;
    uint64_t end_us;
    uint64_t total_written = 0;
    uint64_t elapsed_us = 0;
    uint32_t frame_slots;
    uint32_t expected_bclk_hz;
    int clear_ret;
    int output_opened = 0;
    int callback_attached = 0;
    int dma_linked = 0;
    int ret = 0;

    if (arg == NULL) {
        printf("[clock_test] invalid config\r\n");
        ret = -1;
        goto out;
    }
    cfg = *(alsa_output_clock_test_cfg_t *)arg;
    msp_free(arg);

    if (alsa_validate_output_clock_test_cfg(&cfg) != 0) {
        ret = -1;
        goto out;
    }

    output_clock_mhz = cfg.sample_rate_hz * 1000U;
    frame_slots = (ALSA_CLOCK_TEST_CHANNELS == 1U) ? 2U : ALSA_CLOCK_TEST_CHANNELS;
    expected_bclk_hz = alsa_calc_bclk_hz(cfg.sample_rate_hz, ALSA_CLOCK_TEST_CHANNELS, ALSA_CLOCK_TEST_SLOT_WIDTH);
    buffer_bytes = alsa_clock_test_buffer_bytes(ALSA_CLOCK_TEST_PERIOD_BYTES);

    printf("[clock_test] request: fs=%lu Hz clock=%lu mHz ch=%u bit_width=%u slot_width=%u period=%u hold=%u ms\r\n",
           (unsigned long)cfg.sample_rate_hz,
           (unsigned long)output_clock_mhz,
           ALSA_CLOCK_TEST_CHANNELS,
           ALSA_CLOCK_TEST_BIT_WIDTH,
           ALSA_CLOCK_TEST_SLOT_WIDTH,
           ALSA_CLOCK_TEST_PERIOD_BYTES,
           ALSA_CLOCK_TEST_HOLD_MS);
    printf("[clock_test] expected: fs=%lu Hz frame_slots=%lu wire_bclk=%lu Hz buffer=%lu bytes\r\n",
           (unsigned long)cfg.sample_rate_hz,
           (unsigned long)frame_slots,
           (unsigned long)expected_bclk_hz,
           (unsigned long)buffer_bytes);

    dma_hdl = msp_zalloc_check(sizeof(*dma_hdl));
    output_buf = msp_zalloc_check(buffer_bytes);
    silence_buf = msp_zalloc_check(ALSA_CLOCK_TEST_PERIOD_BYTES);
    if ((dma_hdl == NULL) || (output_buf == NULL) || (silence_buf == NULL)) {
        printf("[clock_test] alloc buffer failed\r\n");
        ret = -1;
        goto out;
    }

    ret = xcodec_set_audio_output_clock_mHz(0U);
    if (alsa_test_expect_ret(&stats, "xcodec_set_audio_output_clock_mHz(clear before start)", ret) < 0) {
        goto out;
    }

    codec.output_chs = &output_ch;
    output_ch.dma = dma_hdl;
    output_ch.sound_channel_num = ALSA_CLOCK_TEST_CHANNELS;
    output_ch.ring_buf = &ring_buffer;

    ret = xcodec_init(&codec, 0);
    if (alsa_test_expect_ret(&stats, "xcodec_init(clock_test)", ret) < 0) {
        goto out;
    }

    ret = xcodec_output_open(&codec, &output_ch, 0);
    if (alsa_test_expect_ret(&stats, "xcodec_output_open(clock_test)", ret) < 0) {
        goto out;
    }
    output_opened = 1;

    ret = xcodec_output_attach_callback(&output_ch, alsa_clock_test_output_event_cb, NULL);
    if (alsa_test_expect_ret(&stats, "xcodec_output_attach_callback(clock_test)", ret) < 0) {
        goto out;
    }
    callback_attached = 1;

    output_cfg.bit_width = ALSA_CLOCK_TEST_BIT_WIDTH;
    output_cfg.slot_width = ALSA_CLOCK_TEST_SLOT_WIDTH;
    output_cfg.sample_rate = cfg.sample_rate_hz;
    output_cfg.buffer = output_buf;
    output_cfg.buffer_size = buffer_bytes;
    output_cfg.period = ALSA_CLOCK_TEST_PERIOD_BYTES;
    output_cfg.mode = XCODEC_OUTPUT_DIFFERENCE;
    output_cfg.sound_channel_num = ALSA_CLOCK_TEST_CHANNELS;

    ret = xcodec_output_config(&output_ch, &output_cfg);
    if (alsa_test_expect_ret(&stats, "xcodec_output_config(clock_test)", ret) < 0) {
        goto out;
    }
    alsa_dump_i2s_ref_clk_div("clock_test");
    alsa_dump_i2s_bclk_div("clock_test");

    ret = xcodec_output_link_dma(&output_ch, dma_hdl);
    if (alsa_test_expect_ret(&stats, "xcodec_output_link_dma(clock_test)", ret) < 0) {
        goto out;
    }
    dma_linked = 1;

    ret = xcodec_output_start(&output_ch);
    if (alsa_test_expect_ret(&stats, "xcodec_output_start(clock_test)", ret) < 0) {
        goto out;
    }

    ret = xcodec_set_audio_output_clock_mHz(output_clock_mhz);
    if (alsa_test_expect_ret(&stats, "xcodec_set_audio_output_clock_mHz", ret) < 0) {
        goto out;
    }

    ret = xcodec_get_audio_output_clock_mHz(&current_clock_mhz);
    if (alsa_test_expect_ret(&stats, "xcodec_get_audio_output_clock_mHz", ret) < 0 ||
        alsa_test_expect_eq(&stats, "output_clock current mHz",
                            current_clock_mhz, output_clock_mhz) < 0) {
        goto out;
    }

    printf("[clock_test] current output_clock=%lu mHz\r\n", (unsigned long)current_clock_mhz);
    alsa_dump_i2s_ref_clk_div("clock_test");
    alsa_dump_i2s_bclk_div("clock_test");
    printf("[clock_test] silence output started, measure FS/BCLK now\r\n");

    start_us = bflb_mtimer_get_time_us();
    end_us = start_us + ((uint64_t)ALSA_CLOCK_TEST_HOLD_MS * 1000ULL);
    while (bflb_mtimer_get_time_us() < end_us) {
        uint32_t written = xcodec_output_write_async(&output_ch, silence_buf, ALSA_CLOCK_TEST_PERIOD_BYTES);

        if (written == 0U) {
            msp_msleep(2);
            continue;
        }

        total_written += written;
        if (written < ALSA_CLOCK_TEST_PERIOD_BYTES) {
            msp_msleep(1);
        }
    }
    elapsed_us = bflb_mtimer_get_time_us() - start_us;
    printf("[clock_test] silence queued=%llu bytes elapsed=%llu us\r\n",
           (unsigned long long)total_written,
           (unsigned long long)elapsed_us);

out:
    if (dma_linked) {
        xcodec_output_stop(&output_ch);
        xcodec_output_link_dma(&output_ch, NULL);
    }
    if (callback_attached) {
        xcodec_output_detach_callback(&output_ch);
    }
    if (output_opened) {
        xcodec_output_close(&output_ch);
        xcodec_uninit(&codec);
    }

    clear_ret = xcodec_set_audio_output_clock_mHz(0U);
    if (clear_ret == XCODEC_OK) {
        if (xcodec_get_audio_output_clock_mHz(&current_clock_mhz) == XCODEC_OK) {
            printf("[clock_test] clear output_clock -> %lu mHz\r\n",
                   (unsigned long)current_clock_mhz);
        }
    } else {
        printf("[clock_test] clear output_clock failed -> %d\r\n", clear_ret);
    }

    printf("[clock_test] summary pass=%u fail=%u result=%s\r\n",
           stats.passed, stats.failed,
           (ret == 0 && stats.failed == 0) ? "PASS" : "FAIL");

    msp_free(silence_buf);
    msp_free(output_buf);
    msp_free(dma_hdl);
    g_alsa_api_test_running = 0;
    msp_task_exit(0);
}

/* ─── Shell command entry ─────────────────────────────── */
int cmd_minialsa_play(int argc, char **argv)
{
    if (g_alsa_running || g_alsa_api_test_running) {
        printf("[alsa_play] already running\r\n");
        return 0;
    }

    /* Default: play 'wa' sample */
    const alsa_sample_t *s = alsa_find_sample((argc > 1) ? argv[1] : NULL);
    if (s == NULL) {
        printf("[alsa_play] unknown sample '%s'\r\n", argv[1]);
        return -1;
    }

    g_alsa_running = 1;
    if (msp_task_new("alsa_play", alsa_play_task, (void *)s, 4096) < 0) {
        g_alsa_running = 0;
        printf("[alsa_play] create task failed\r\n");
        return -1;
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_minialsa_play, minialsa_play, play PCM via ALSA API [wa|sin]);

int cmd_minialsa_api_test(int argc, char **argv)
{
    alsa_api_test_cfg_t *cfg;
    int i;

    if (g_alsa_running || g_alsa_api_test_running) {
        printf("[api_test] another minialsa task is already running\r\n");
        return 0;
    }

    cfg = msp_zalloc_check(sizeof(*cfg));
    if (cfg == NULL) {
        printf("[api_test] alloc config failed\r\n");
        return -1;
    }

    cfg->sample = &g_samples[0];
    cfg->run_playback = 1;
    cfg->run_capture = 1;
    cfg->run_set_params = 1;

    for (i = 1; i < argc; i++) {
        const alsa_sample_t *sample;

        if (strcmp(argv[i], "help") == 0 || strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0) {
            alsa_api_test_print_usage();
            msp_free(cfg);
            return 0;
        }
        if (strcmp(argv[i], "all") == 0) {
            cfg->run_playback = 1;
            cfg->run_capture = 1;
            cfg->run_set_params = 1;
            continue;
        }
        if (strcmp(argv[i], "playback") == 0) {
            cfg->run_playback = 1;
            cfg->run_capture = 0;
            cfg->run_set_params = 0;
            continue;
        }
        if (strcmp(argv[i], "capture") == 0) {
            cfg->run_playback = 0;
            cfg->run_capture = 1;
            cfg->run_set_params = 0;
            continue;
        }
        if (strcmp(argv[i], "set_params") == 0) {
            cfg->run_playback = 0;
            cfg->run_capture = 0;
            cfg->run_set_params = 1;
            continue;
        }

        sample = alsa_find_sample(argv[i]);
        if (sample != NULL) {
            cfg->sample = sample;
            continue;
        }

        printf("[api_test] unknown option '%s'\r\n", argv[i]);
        alsa_api_test_print_usage();
        msp_free(cfg);
        return -1;
    }

    g_alsa_api_test_running = 1;
    if (msp_task_new("alsa_api_test", alsa_api_test_task, cfg, 6144) < 0) {
        g_alsa_api_test_running = 0;
        msp_free(cfg);
        printf("[api_test] create task failed\r\n");
        return -1;
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_minialsa_api_test, minialsa_api_test,
                       test minialsa apis [all|playback|capture|set_params] [wa|sin]);

int cmd_minialsa_output_clock_test(int argc, char **argv)
{
    alsa_output_clock_test_cfg_t *cfg;

    if (g_alsa_running || g_alsa_api_test_running) {
        printf("[clock_test] another minialsa task is already running\r\n");
        return 0;
    }

    if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "--help") == 0) {
        alsa_output_clock_test_print_usage();
        return (argc < 2) ? -1 : 0;
    }

    cfg = msp_zalloc_check(sizeof(*cfg));
    if (cfg == NULL) {
        printf("[clock_test] alloc config failed\r\n");
        return -1;
    }

    if (alsa_parse_u32_arg(argv[1], &cfg->sample_rate_hz) < 0 || cfg->sample_rate_hz == 0U) {
        printf("[clock_test] invalid sample_rate_hz '%s'\r\n", argv[1]);
        alsa_output_clock_test_print_usage();
        msp_free(cfg);
        return -1;
    }

    if (alsa_validate_output_clock_test_cfg(cfg) != 0) {
        alsa_output_clock_test_print_usage();
        msp_free(cfg);
        return -1;
    }

    g_alsa_api_test_running = 1;
    if (msp_task_new("alsa_clock_test", alsa_output_clock_test_task, cfg, 6144) < 0) {
        g_alsa_api_test_running = 0;
        msp_free(cfg);
        printf("[clock_test] create task failed\r\n");
        return -1;
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_minialsa_output_clock_test, minialsa_output_clock_test,
                       keep silence and tune xcodec output clock for scope measurement);

static void board_audio_init(void)
{
    snd_bl616_config_t snd_config;

    snd_card_bl616_register(&snd_config);
}

int app_minialsa_init(void)
{
    static char inited = 0;

    if (0 == inited) {
        board_audio_init();
        inited = 1;
    } else {
        printf("----------------------- reinit. -----------------------\r\n");
    }

    return 0;
}
