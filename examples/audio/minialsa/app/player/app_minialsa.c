/*
 * Play embedded PCM audio data via msp_pcm_* (ALSA-like) API.
 * Command: minialsa_play [wa|sin]
 *   wa  - 16 kHz / 16-bit / mono voice sample (default)
 *   sin - 16 kHz / 16-bit / mono 1 kHz sine wave
 *
 * API self-test command:
 *   minialsa_api_test [all|playback|capture|set_params] [wa|sin]
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <devices/drv_snd_bl616.h>

#include <msp/kernel.h>
#include <alsa/pcm.h>
#include <shell.h>

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

static void alsa_api_test_print_usage(void)
{
    printf("usage: minialsa_api_test [all|playback|capture|set_params] [wa|sin]\r\n");
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

    printf("[api_test] playback refined api begin\r\n");

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
