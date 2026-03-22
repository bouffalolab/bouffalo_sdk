/*
 * MP3 Player Task (decode and feed PCM into shared ring buffer)
 */

#include "mp3_player.h"
#include "wave_player.h"
#if MP3_PLAYER_TEST_MP3
#include "shell.h"
#endif

/* MP3 decode (pvmp3dec) */
#include "pvmp3dec/pvmp3decoder_api.h"

#include <FreeRTOS.h>
#include <task.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MP3_PLAYER_TEST_MP3
#include "incbin.h"
INCBIN(mp3_src_buffer0, "../../bsp/common/audio_player/ruyuan_30s_16k_mono.mp3");
INCBIN(mp3_src_buffer1, "../../bsp/common/audio_player/therose_30s_48k.mp3");
#endif

/* pvmp3dec prints version via msp_print_version(); provide a stub for this example. */
__attribute__((weak)) void msp_print_version(const char *format, ...)
{
    (void)format;
}

/* Max PCM per MP3 frame:
 * MPEG-1: 1152 samples per channel. Stereo => 2304 int16.
 */
#define MP3_MAX_SAMPLES_PER_CH 1152
#define MP3_MAX_SAMPLES_TOTAL  (MP3_MAX_SAMPLES_PER_CH * 2)

typedef struct {
    const uint8_t *data;
    size_t len;
    size_t off;
} mp3_mem_stream_t;

typedef struct {
    TaskHandle_t task;
    volatile bool pause;
    volatile bool stop;
} mp3_player_ctx_t;

static mp3_player_ctx_t g_mp3 = {
    .task = NULL,
    .pause = false,
    .stop = false,
};

#if MP3_PLAYER_TEST_MP3
static mp3_mem_stream_t mp3_stream;
static audio_mp3_task_cfg_t mp3_cfg;

static int mp3_mem_init(void *user_data)
{
    mp3_mem_stream_t *stream = (mp3_mem_stream_t *)user_data;
    if (stream == NULL) {
        return -1;
    }
    stream->off = 0;
    return 0;
}

static int mp3_mem_read(void *user_data, uint8_t *buf, uint32_t len)
{
    mp3_mem_stream_t *stream = (mp3_mem_stream_t *)user_data;
    if (stream == NULL || buf == NULL || len == 0) {
        return 0;
    }

    if (stream->off >= stream->len) {
        return 0;
    }

    size_t remain = stream->len - stream->off;
    size_t to_copy = remain < (size_t)len ? remain : (size_t)len;
    memcpy(buf, stream->data + stream->off, to_copy);
    stream->off += to_copy;
    return (int)to_copy;
}

static void mp3_mem_close(void *user_data)
{
    (void)user_data;
}
#endif

static int find_mp3_sync(const uint8_t *buf, size_t len)
{
    if (buf == NULL || len < 2) {
        return -1;
    }

    /* Basic MP3 frame sync search: 0xFF followed by 0xE0 mask set. */
    for (size_t i = 0; i + 1 < len; i++) {
        if (buf[i] == 0xFF && (buf[i + 1] & 0xE0) == 0xE0) {
            return (int)i;
        }
    }
    return -1;
}

static uint32_t mp3_feed_pcm_blocking(const uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0) {
        return 0;
    }

    return wave_player_feed_blocking((uint8_t *)data, len);
}

static void audio_player_mp3_task(void *arg)
{
    audio_mp3_task_cfg_t *cfg = (audio_mp3_task_cfg_t *)arg;
    if (cfg == NULL || cfg->ops.read == NULL) {
        printf("mp3_player: bad cfg\r\n");
        g_mp3.task = NULL;
        vTaskDelete(NULL);
        return;
    }

    if (cfg->ops.init != NULL) {
        int ret = cfg->ops.init(cfg->user_data);
        if (ret != 0) {
            printf("mp3_player: init failed (%d)\r\n", ret);
            if (cfg->ops.close != NULL) {
                cfg->ops.close(cfg->user_data);
            }
            g_mp3.task = NULL;
            vTaskDelete(NULL);
            return;
        }
    }

    wave_player_stop();

    uint32_t msize = pvmp3_decoderMemRequirements();
    void *pvmp3_dbuf = pvPortMalloc(msize);
    if (pvmp3_dbuf == NULL) {
        printf("mp3_player: alloc %u failed\r\n", (unsigned)msize);
        if (cfg->ops.close != NULL) {
            cfg->ops.close(cfg->user_data);
        }
        g_mp3.task = NULL;
        vTaskDelete(NULL);
        return;
    }

    int16_t *mp3_pcm_tmp = (int16_t *)pvPortMalloc((size_t)MP3_MAX_SAMPLES_TOTAL * sizeof(int16_t));
    uint8_t *mp3_cache = (uint8_t *)pvPortMalloc(4096);
    if (mp3_pcm_tmp == NULL || mp3_cache == NULL) {
        printf("mp3_player: alloc scratch failed\r\n");
        if (mp3_pcm_tmp != NULL) {
            vPortFree(mp3_pcm_tmp);
        }
        if (mp3_cache != NULL) {
            vPortFree(mp3_cache);
        }
        vPortFree(pvmp3_dbuf);
        if (cfg->ops.close != NULL) {
            cfg->ops.close(cfg->user_data);
        }
        g_mp3.task = NULL;
        vTaskDelete(NULL);
        return;
    }

    tPVMP3DecoderExternal pvmp3_ext;
    memset(&pvmp3_ext, 0, sizeof(pvmp3_ext));
    pvmp3_InitDecoder(&pvmp3_ext, pvmp3_dbuf);
    pvmp3_ext.crcEnabled = false;
    pvmp3_ext.equalizerType = flat;
    pvmp3_ext.pOutputBuffer = mp3_pcm_tmp;

    size_t mp3_cache_len = 0;
    bool stream_done = false;
    bool playback_started = false;
    int last_info_hz = -1;
    int last_info_ch = -1;

    while (1) {
        if (g_mp3.stop) {
            break;
        }

        while (g_mp3.pause && !g_mp3.stop) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        if (g_mp3.stop) {
            break;
        }

        if (!stream_done && mp3_cache_len < 1024) {
            uint32_t space = (uint32_t)(4096U - mp3_cache_len);
            if (space > 0) {
                int r = cfg->ops.read(cfg->user_data, mp3_cache + mp3_cache_len, space);
                if (r > 0) {
                    mp3_cache_len += (size_t)r;
                } else {
                    stream_done = true;
                }
            }
        }

        if (mp3_cache_len == 0) {
            if (stream_done) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        int sync_off = find_mp3_sync(mp3_cache, mp3_cache_len);
        if (sync_off > 0) {
            mp3_cache_len -= (size_t)sync_off;
            memmove(mp3_cache, mp3_cache + sync_off, mp3_cache_len);
        } else if (sync_off < 0) {
            if (mp3_cache_len > 1) {
                mp3_cache[0] = mp3_cache[mp3_cache_len - 1];
                mp3_cache_len = 1;
            }
            if (stream_done) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        pvmp3_ext.pInputBuffer = mp3_cache;
        pvmp3_ext.inputBufferCurrentLength = (int32)mp3_cache_len;
        pvmp3_ext.inputBufferUsedLength = 0;
        pvmp3_ext.outputFrameSize = (int32)MP3_MAX_SAMPLES_TOTAL;
        pvmp3_ext.pOutputBuffer = mp3_pcm_tmp;

        ERROR_CODE ecode = pvmp3_framedecoder(&pvmp3_ext, pvmp3_dbuf);

        if (pvmp3_ext.inputBufferUsedLength > 0) {
            size_t consume = (size_t)pvmp3_ext.inputBufferUsedLength;
            if (consume > mp3_cache_len) {
                consume = mp3_cache_len;
            }
            mp3_cache_len -= consume;
            if (mp3_cache_len > 0) {
                memmove(mp3_cache, mp3_cache + consume, mp3_cache_len);
            }
        }

        if (ecode != NO_DECODING_ERROR || pvmp3_ext.outputFrameSize <= 0 || pvmp3_ext.num_channels <= 0 ||
            pvmp3_ext.samplingRate <= 0) {
            if (stream_done && mp3_cache_len == 0) {
                break;
            }
            if (mp3_cache_len == 0) {
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            continue;
        }

        int samples_per_ch = (int)(pvmp3_ext.outputFrameSize / pvmp3_ext.num_channels);
        int hz = (int)pvmp3_ext.samplingRate;
        int src_ch = (int)pvmp3_ext.num_channels;

        if (hz != last_info_hz || src_ch != last_info_ch) {
            last_info_hz = hz;
            last_info_ch = src_ch;
            printf("mp3 info: samplerate=%d ch=%d\r\n", hz, src_ch);
        }

        if (!playback_started) {
            uint32_t tx_block_bytes = AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS * sizeof(int16_t);
            wave_player_config((uint32_t)hz, tx_block_bytes, 0, (uint8_t)src_ch);
            wave_player_start();
            playback_started = true;
        }

        if (samples_per_ch <= 0) {
            continue;
        }

        uint32_t total_samples = (uint32_t)pvmp3_ext.outputFrameSize;
        uint32_t out_bytes = total_samples * (uint32_t)sizeof(int16_t);
        (void)mp3_feed_pcm_blocking((const uint8_t *)mp3_pcm_tmp, out_bytes);
    }

    wave_player_stop();

    if (cfg->ops.close != NULL) {
        cfg->ops.close(cfg->user_data);
    }

    vPortFree(pvmp3_dbuf);
    vPortFree(mp3_pcm_tmp);
    vPortFree(mp3_cache);

    g_mp3.task = NULL;
    g_mp3.pause = false;
    g_mp3.stop = false;
    vTaskDelete(NULL);
}

int mp3_player_start(audio_mp3_task_cfg_t *cfg)
{
    if (g_mp3.task != NULL) {
        printf("mp3_player: already running\r\n");
        return -1;
    }

    if (cfg == NULL || cfg->ops.read == NULL) {
        printf("mp3_player: bad cfg\r\n");
        return -1;
    }

    g_mp3.pause = false;
    g_mp3.stop = false;

    if (xTaskCreate(audio_player_mp3_task, (char *)"mp3_play", MP3_PLAYER_TASK_STACK_DEPTH, cfg, 3, &g_mp3.task) !=
        pdPASS) {
        g_mp3.task = NULL;
        printf("mp3_player: create task failed\r\n");
        return -1;
    }

    return 0;
}

#if MP3_PLAYER_TEST_MP3
int mp3_player_play_buffer(const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0) {
        printf("mp3_player: empty data\r\n");
        return -1;
    }

    mp3_player_stop();

    mp3_stream = (mp3_mem_stream_t){
        .data = data,
        .len = len,
        .off = 0,
    };

    mp3_cfg = (audio_mp3_task_cfg_t){
        .ops = {
            .init = mp3_mem_init,
            .read = mp3_mem_read,
            .close = mp3_mem_close,
        },
        .user_data = &mp3_stream,
    };

    return mp3_player_start(&mp3_cfg);
}
#endif

int mp3_player_pause(void)
{
    if (g_mp3.task == NULL) {
        return -1;
    }
    g_mp3.pause = true;

    return 0;
}

int mp3_player_resume(void)
{
    if (g_mp3.task == NULL) {
        return -1;
    }
    g_mp3.pause = false;

    return 0;
}

int mp3_player_stop(void)
{
    if (g_mp3.task == NULL) {
        return 0;
    }

    g_mp3.stop = true;
    while (g_mp3.task != NULL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;
}

bool mp3_player_is_running(void)
{
    return (g_mp3.task != NULL);
}

void mp3_player_dump_stack(void)
{
#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
    printf("stack current=%u\r\n", (unsigned)uxTaskGetStackHighWaterMark(NULL));
    if (g_mp3.task != NULL) {
        printf("stack mp3_task=%u\r\n", (unsigned)uxTaskGetStackHighWaterMark(g_mp3.task));
    }
#else
    printf("stack watermark not enabled (INCLUDE_uxTaskGetStackHighWaterMark=0)\r\n");
#endif
}

#if MP3_PLAYER_TEST_MP3
static int cmd_mp3_play0(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return mp3_player_play_buffer(gmp3_src_buffer0Data, gmp3_src_buffer0Size);
}

static int cmd_mp3_stack(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    mp3_player_dump_stack();
    return 0;
}

static int cmd_mp3_play1(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return mp3_player_play_buffer(gmp3_src_buffer1Data, gmp3_src_buffer1Size);
}

SHELL_CMD_EXPORT_ALIAS(cmd_mp3_play0, mp3_play0, "play embedded mp3 buffer 0");
SHELL_CMD_EXPORT_ALIAS(cmd_mp3_play1, mp3_play1, "play embedded mp3 buffer 1");
SHELL_CMD_EXPORT_ALIAS(cmd_mp3_stack, mp3_stack, "show mp3 stack high watermarks");
#endif
