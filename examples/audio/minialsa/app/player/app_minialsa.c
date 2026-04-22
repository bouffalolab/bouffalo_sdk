/*
 * 通过 msp_pcm_* (ALSA-like) API 播放内嵌 PCM 音频数据。
 * 命令: codec_alsa_play [wa|sin]
 *   wa  - 16 kHz / 16-bit / mono 人声样本（默认）
 *   sin - 16 kHz / 16-bit / mono 1 kHz 正弦波
 */

#include <stdio.h>
#include <string.h>

#include <devices/drv_snd_bl616.h>

#include <msp/kernel.h>
#include <alsa/pcm.h>
#include <shell.h>

#include "audio_file.h"

/* ─── 播放参数 ─────────────────────────────────────────── */
#define ALSA_PLAY_PCM_NAME      "pcmP0"
#define ALSA_PLAY_PERIOD_FRAMES 256
#define ALSA_PLAY_BUFFER_FRAMES 1024

/* ─── 音频样本描述 ─────────────────────────────────────── */
typedef struct {
    const char         *name;
    const unsigned char *data;
    const unsigned int  *len;
    unsigned int         rate;
    unsigned int         channels;
    int                  format_bits;
} alsa_sample_t;

static const alsa_sample_t g_samples[] = {
    { "wa",  wa_16K_16bit_mono_pcm,       &wa_16K_16bit_mono_pcm_len,       16000, 1, 16 },
    { "sin", sin_1khz_16k_sample_0db_pcm, &sin_1khz_16k_sample_0db_pcm_len, 16000, 1, 16 },
};

static volatile int g_alsa_running = 0;

/* ─── 播放任务 ─────────────────────────────────────────── */
static void alsa_play_task(void *arg)
{
    const alsa_sample_t *s = (const alsa_sample_t *)arg;

    msp_pcm_t          *pcm      = NULL;
    msp_pcm_hw_params_t *tmp_hw  = NULL;   /* 仅用于演示 malloc/free 生命周期 */
    msp_pcm_hw_params_t *hw      = NULL;   /* 实际配置用，由 msp_pcm_close 释放 */
    msp_pcm_sw_params_t *sw      = NULL;   /* 由 msp_pcm_close 释放 */
    unsigned char       *tail_buf = NULL;

    int               ret, dir = 0;
    msp_pcm_uframes_t period_size = ALSA_PLAY_PERIOD_FRAMES;
    msp_pcm_uframes_t buffer_size = ALSA_PLAY_BUFFER_FRAMES;
    unsigned int      rate        = 0;
    unsigned int      channels    = 0;
    int               format      = 0;

    /* ── 1. 打开 PCM 设备 ─────────────────────────────── */
    ret = msp_pcm_open(&pcm, ALSA_PLAY_PCM_NAME, MSP_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        printf("[alsa_play] msp_pcm_open(%s) -> %d\r\n", ALSA_PLAY_PCM_NAME, ret);
        goto out;
    }

    /* ── 2. 演示 hw_params 的 malloc / free 生命周期 ──── */
    ret = msp_pcm_hw_params_malloc(&tmp_hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params_malloc(tmp) -> %d\r\n", ret);
        goto out_close;
    }
    msp_pcm_hw_params_free(tmp_hw);
    tmp_hw = NULL;

    /* ── 3. 分配并配置实际 hw_params ─────────────────── */
    ret = msp_pcm_hw_params_malloc(&hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params_malloc -> %d\r\n", ret);
        goto out_close;
    }

    /* 初始化为默认值，同时将 pcm->hw_params = hw */
    ret = msp_pcm_hw_params_any(pcm, hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params_any -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* 交错读写模式 */
    ret = msp_pcm_hw_params_set_access(pcm, hw, MSP_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0) {
        printf("[alsa_play] set_access -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* 采样位深 */
    ret = msp_pcm_hw_params_set_format(pcm, hw, s->format_bits);
    if (ret < 0) {
        printf("[alsa_play] set_format -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* 声道数 */
    ret = msp_pcm_hw_params_set_channels(pcm, hw, s->channels);
    if (ret < 0) {
        printf("[alsa_play] set_channels -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* 采样率 */
    ret = msp_pcm_hw_params_set_rate(pcm, hw, s->rate, 0);
    if (ret < 0) {
        printf("[alsa_play] set_rate -> %d\r\n", ret);
        goto out_free_hw;
    }

    /* buffer / period 大小（near 会自动对齐到硬件支持值） */
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

    /* 回读实际协商值 */
    msp_pcm_hw_params_get_period_size(hw, &period_size, &dir);
    msp_pcm_hw_params_get_buffer_size(hw, &buffer_size);
    msp_pcm_hw_params_get_rate(hw, &rate, &dir);
    msp_pcm_hw_params_get_channels(hw, &channels);
    msp_pcm_hw_params_get_format(hw, &format);
    printf("[alsa_play] hw: fmt=%d bits, ch=%u, rate=%u Hz, period=%d, buf=%d\r\n",
           format, channels, rate, (int)period_size, (int)buffer_size);

    /* 应用 hw_params（触发底层 xcodec / I2S 初始化） */
    ret = msp_pcm_hw_params(pcm, hw);
    if (ret < 0) {
        printf("[alsa_play] hw_params -> %d\r\n", ret);
        goto out_free_hw;
    }
    /* hw 已关联到 pcm->hw_params，由 msp_pcm_close 负责释放，不再显式 free */
    hw = NULL;

    /* ── 4. 配置 sw_params ────────────────────────────── */
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

    /* 攒满一个 period 后自动启动 DMA */
    ret = msp_pcm_sw_params_set_start_threshold(pcm, sw, period_size);
    if (ret < 0) {
        printf("[alsa_play] set_start_threshold -> %d\r\n", ret);
        goto out_free_sw;
    }

    /* buffer 满时停止 */
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

    /* 应用 sw_params（设置 pcm->sw_params = sw，由 msp_pcm_close 释放） */
    ret = msp_pcm_sw_params(pcm, sw);
    if (ret < 0) {
        printf("[alsa_play] sw_params -> %d\r\n", ret);
        goto out_free_sw;
    }
    sw = NULL;  /* 已关联，不再显式 free */

    /* ── 5. 写入音频数据 ──────────────────────────────── */
    size_t frame_bytes = ((size_t)s->channels * (size_t)s->format_bits) / 8U;
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
            /* 最后一帧不足，用静音补齐 */
            memset(tail_buf, 0, chunk_bytes);
            memcpy(tail_buf, s->data + offset, copy_bytes);
            write_ret = msp_pcm_writei(pcm, tail_buf, period_size);
        } else {
            write_ret = msp_pcm_writei(pcm, s->data + offset, period_size);
        }

        if (write_ret < 0) {
            /* xrun 恢复 */
            ret = msp_pcm_recover(pcm, write_ret, 0);
            if (ret < 0) {
                printf("[alsa_play] recover -> %d\r\n", ret);
                goto out_tail;
            }
            continue;
        }
        offset += copy_bytes;
    }

    /* 等待硬件播完所有数据 */
    msp_pcm_drain(pcm);
    printf("[alsa_play] done, total_frames=%d\r\n",
           msp_pcm_bytes_to_frames(pcm, (ssize_t)*s->len));
    ret = 0;

out_tail:
    msp_free(tail_buf);
    goto out_close;

out_free_sw:
    if (sw) {
        msp_free(sw);
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

/* ─── shell 命令入口 ───────────────────────────────────── */
int cmd_minialsa_play(int argc, char **argv)
{
    if (g_alsa_running) {
        printf("[alsa_play] already running\r\n");
        return 0;
    }

    /* 默认播放 wa 样本 */
    const alsa_sample_t *s = &g_samples[0];
    if (argc > 1 && argv[1] != NULL) {
        for (size_t i = 0; i < sizeof(g_samples) / sizeof(g_samples[0]); i++) {
            if (strcmp(argv[1], g_samples[i].name) == 0) {
                s = &g_samples[i];
                break;
            }
        }
    }

    g_alsa_running = 1;
    msp_task_new("alsa_play", alsa_play_task, (void *)s, 4096);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_minialsa_play, minialsa_play, play PCM via ALSA API [wa|sin]);

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