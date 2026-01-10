#include "board.h"
#include "bflb_clock.h"
#include "bflb_i2s.h"
#include "bflb_i2c.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "board_gpio.h"
#include "audio_codec.h"
#include "ring_buffer.h"
#include "mp3_player.h"
#include "wave_player.h"
#include "recorder.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

/* Demo flow:
 * 1) Play embedded MP3 while streaming RX (AEC-like 20ms frames)
 * 2) Track3: fixed 16kHz silence TX + record 2s, then playback recorded 2s
 */

#include "incbin.h"
INCBIN(mp3_src_buffer, "../../examples/peripherals/i2s/i2s_codec/TheRose_30s_48k.mp3");
INCBIN(mp3_src_buffer2, "../../examples/peripherals/i2s/i2s_codec/ruyuan_30s_16k_mono.mp3");

struct bflb_device_s *i2s0;
struct bflb_device_s *i2c0;
struct bflb_device_s *dma0_ch0;
struct bflb_device_s *dma0_ch1;

static struct bflb_dma_channel_lli_pool_s tx_llipool[8];
/* RX capture may be split into multiple LLIs by bflb_dma_channel_lli_reload(). */
static struct bflb_dma_channel_lli_pool_s rx_llipool[64];

static audio_codec_dev_t g_codec;
static int g_i2s_hz;
static bool g_i2s_inited;
static uint8_t g_i2s_channels = 2;
static SemaphoreHandle_t sem_dma_done;

static SemaphoreHandle_t sem_rb_space;
static Ring_Buffer_Type audio_src_rb;
static uint8_t audio_src_buf[16 * 1024];
static volatile bool g_src_done;
static volatile bool g_src_stop;
/* Ping-pong PCM buffers: sized for one MP3 frame max (stereo) */
static int16_t pcm_ping[PVMP3_MAX_SAMPLES_TOTAL] __attribute__((aligned(32)));
static int16_t pcm_pong[PVMP3_MAX_SAMPLES_TOTAL] __attribute__((aligned(32)));

/* AEC-like streaming capture: 20ms ping-pong at up to 48k stereo */
#define CAP_FRAME_MS           20U
#define CAP_MAX_FS_HZ          48000U
#define CAP_MAX_SAMPLES_PER_CH ((CAP_MAX_FS_HZ * CAP_FRAME_MS) / 1000U) /* 960 */
#define CAP_CHANNELS           2U
#define CAP_MAX_SAMPLES_TOTAL  (CAP_MAX_SAMPLES_PER_CH * CAP_CHANNELS)
static int16_t cap_ping[CAP_MAX_SAMPLES_TOTAL] __attribute__((aligned(32)));
static int16_t cap_pong[CAP_MAX_SAMPLES_TOTAL] __attribute__((aligned(32)));
typedef struct {
    int16_t *buf_stereo;
    uint32_t nbytes;
    uint32_t hz;
    uint32_t samples_per_ch;
    uint8_t channels;
} cap_msg_t;
static QueueHandle_t g_cap_queue;
static volatile bool g_cap_tc_ping;
static volatile uint32_t g_cap_block_bytes;
static volatile uint32_t g_cap_samples_per_ch;

/* Accumulate up to 2s mono (left channel) for verification */
#define REC_MIN_SECONDS 2U
#define REC_MAX_SAMPLES (CAP_MAX_FS_HZ * REC_MIN_SECONDS)
static int16_t rec_mono_buf[REC_MAX_SAMPLES] __attribute__((aligned(32)));
static volatile uint32_t g_rec_write = 0;
static volatile bool g_rec_filled = false;
static volatile bool g_rec_enable = false;

typedef struct {
    const uint8_t *src;
    size_t src_len;
    Ring_Buffer_Type *rb;
    SemaphoreHandle_t sem_rb_space;
    volatile bool *src_done;
    volatile bool *stop;
    TaskHandle_t done_notify_task;
} audio_src_task_cfg_t;

static void audio_src_sink(void *arg);
static void audio_reconfig_txrx(uint32_t hz, uint32_t tx_block_bytes, uint8_t channels);
static void audio_io_stop(void);
static void pcm_fill_pcm_block(int16_t *dst, const int16_t *src, int samples_per_ch, int src_channels, int dst_channels,
                               uint32_t dst_block_bytes);
static void audio_io_start(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes, uint8_t channels);
static uint32_t record_one_wave(uint32_t hz);

static void sem_drain(SemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return;
    }
    while (xSemaphoreTake(sem, 0) == pdTRUE) {}
}

static void play_one_wave(const uint8_t *src, size_t src_len, uint32_t sample_rate_hz, uint8_t src_channels)
{
    if (src == NULL || src_len == 0) {
        printf("play_one_wave: empty src\r\n");
        return;
    }

    /* Reset source + ringbuffer for a new track */
    g_src_done = false;
    g_src_stop = false;
    Ring_Buffer_Reset(&audio_src_rb);
    sem_drain(sem_rb_space);

    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    uint32_t tmp = 0;
    /* Clear any stale notify bits before starting */
    (void)xTaskNotifyWait(0, 0xFFFFFFFFu, &tmp, 0);

    audio_src_task_cfg_t *src_cfg = (audio_src_task_cfg_t *)pvPortMalloc(sizeof(*src_cfg));
    if (src_cfg == NULL) {
        printf("play_one_wave: no mem for src_cfg\r\n");
        return;
    }
    *src_cfg = (audio_src_task_cfg_t){
        .src = src,
        .src_len = src_len,
        .rb = &audio_src_rb,
        .sem_rb_space = sem_rb_space,
        .src_done = &g_src_done,
        .stop = &g_src_stop,
        .done_notify_task = self,
    };
    (void)xTaskCreate(audio_src_sink, (char *)"audio_src", 2048, src_cfg, 2, NULL);

    wave_player_cfg_t wave_cfg = (wave_player_cfg_t){
        .src_rb = &audio_src_rb,
        .sem_rb_space = sem_rb_space,
        .src_done = &g_src_done,
        .sample_rate_hz = sample_rate_hz,
        .src_channels = src_channels,
        .out_channels = g_i2s_channels,
        .sem_dma_done = sem_dma_done,
        .pcm_ping = pcm_ping,
        .pcm_pong = pcm_pong,
        .pcm_fill = pcm_fill_pcm_block,
        .reconfig_txrx = audio_reconfig_txrx,
        .audio_stop = audio_io_stop,
        .done_notify_task = self,
    };
    (void)xTaskCreate(wave_player_task, (char *)"wav_play", 3072, &wave_cfg, 3, NULL);

    uint32_t done = 0;
    const uint32_t want = (AUDIO_NOTIFY_WAVE_DONE | AUDIO_NOTIFY_SRC_DONE);
    while ((done & want) != want) {
        uint32_t bits = 0;
        if (xTaskNotifyWait(0, 0xFFFFFFFFu, &bits, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        done |= bits;
        if (bits & AUDIO_NOTIFY_WAVE_DONE) {
            g_src_stop = true;
            g_src_done = true;
            if (sem_rb_space != NULL) {
                (void)xSemaphoreGive(sem_rb_space);
            }
        }
    }
}

static void play_one_mp3(const uint8_t *src, size_t src_len)
{
    if (src == NULL || src_len == 0) {
        printf("play_one_mp3: empty src\r\n");
        return;
    }

    /* Reset source + ringbuffer for a new track */
    g_src_done = false;
    g_src_stop = false;
    Ring_Buffer_Reset(&audio_src_rb);
    sem_drain(sem_rb_space);

    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    uint32_t tmp = 0;
    /* Clear any stale notify bits before starting */
    (void)xTaskNotifyWait(0, 0xFFFFFFFFu, &tmp, 0);

    /* Create a per-track source feeder task and pass this track info. */
    audio_src_task_cfg_t *src_cfg = (audio_src_task_cfg_t *)pvPortMalloc(sizeof(*src_cfg));
    if (src_cfg == NULL) {
        printf("play_one_mp3: no mem for src_cfg\r\n");
        return;
    }
    *src_cfg = (audio_src_task_cfg_t){
        .src = src,
        .src_len = src_len,
        .rb = &audio_src_rb,
        .sem_rb_space = sem_rb_space,
        .src_done = &g_src_done,
        .stop = &g_src_stop,
        .done_notify_task = self,
    };
    (void)xTaskCreate(audio_src_sink, (char *)"audio_src", 2048, src_cfg, 2, NULL);

    /* Create per-track MP3 player task. */
    mp3_player_cfg_t mp3_cfg = (mp3_player_cfg_t){
        .src_rb = &audio_src_rb,
        .sem_rb_space = sem_rb_space,
        .src_done = &g_src_done,
        .out_channels = g_i2s_channels,
        .sem_dma_done = sem_dma_done,
        .pcm_ping = pcm_ping,
        .pcm_pong = pcm_pong,
        .pcm_fill = pcm_fill_pcm_block,
        .reconfig_txrx = audio_reconfig_txrx,
        .audio_stop = audio_io_stop,
        .done_notify_task = self,
    };
    (void)xTaskCreate(mp3_player_task, (char *)"mp3_play", 4096, &mp3_cfg, 3, NULL);

    /* Wait until both tasks finish this track */
    uint32_t done = 0;
    const uint32_t want = (AUDIO_NOTIFY_MP3_DONE | AUDIO_NOTIFY_SRC_DONE);
    while ((done & want) != want) {
        uint32_t bits = 0;
        if (xTaskNotifyWait(0, 0xFFFFFFFFu, &bits, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        done |= bits;
        /* If player finished first, stop/unblock feeder (if still running). */
        if (bits & AUDIO_NOTIFY_MP3_DONE) {
            g_src_stop = true;
            g_src_done = true;
            if (sem_rb_space != NULL) {
                (void)xSemaphoreGive(sem_rb_space);
            }
        }
    }
}

static inline uint32_t audio_bytes_per_sample(void)
{
    return (uint32_t)sizeof(int16_t);
}

static inline uint32_t audio_calc_samples_per_ch(uint32_t hz, uint32_t frame_ms)
{
    /* Round down: acceptable for a demo (e.g. 44.1kHz -> 882 for 20ms). */
    uint32_t s = (hz * frame_ms) / 1000U;
    return (s == 0) ? 1U : s;
}

static inline uint32_t audio_calc_block_bytes(uint32_t samples_per_ch, uint32_t channels)
{
    return samples_per_ch * channels * audio_bytes_per_sample();
}

static inline uint8_t audio_channel_mode_from_count(uint8_t channels)
{
    return (channels <= 1U) ? I2S_CHANNEL_MODE_NUM_1 : I2S_CHANNEL_MODE_NUM_2;
}

static void audio_fill_i2s_cfg(struct bflb_i2s_config_s *cfg, uint32_t samplerate_hz, uint8_t channels)
{
    if (cfg == NULL) {
        return;
    }
    if (channels == 0) {
        channels = 1;
    }
    if (channels > 2) {
        channels = 2;
    }

    *cfg = (struct bflb_i2s_config_s){
        /* Use 32-bit slots (data 16-bit) => BCLK = Fs * 32 * channels */
        .bclk_freq_hz = samplerate_hz * 32U * (uint32_t)channels,
        .role = I2S_ROLE_MASTER,
        .format_mode = I2S_MODE_LEFT_JUSTIFIED,
        .channel_mode = audio_channel_mode_from_count(channels),
        .frame_width = I2S_SLOT_WIDTH_32,
        .data_width = I2S_SLOT_WIDTH_16,
        .fs_offset_cycle = 1,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };
}

static void audio_fill_dma_tx_cfg(struct bflb_dma_channel_config_s *cfg)
{
    if (cfg == NULL) {
        return;
    }
    *cfg = (struct bflb_dma_channel_config_s){
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_I2S_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };
}

static void audio_fill_dma_rx_cfg(struct bflb_dma_channel_config_s *cfg)
{
    if (cfg == NULL) {
        return;
    }
    *cfg = (struct bflb_dma_channel_config_s){
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_I2S_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };
}

static uint32_t audio_calc_rx_block_bytes(uint32_t hz, uint8_t channels)
{
    if (channels == 0) {
        channels = 1;
    }
    /* Capture buffers are sized for CAP_CHANNELS. Clamp to avoid overflow. */
    if (channels > CAP_CHANNELS) {
        channels = CAP_CHANNELS;
    }
    const uint32_t samples_per_ch = audio_calc_samples_per_ch(hz, CAP_FRAME_MS);
    uint32_t bytes = audio_calc_block_bytes(samples_per_ch, (uint32_t)channels);
    const uint32_t max_bytes = CAP_MAX_SAMPLES_TOTAL * audio_bytes_per_sample();
    if (bytes > max_bytes) {
        bytes = max_bytes;
    }
    return bytes;
}

static void audio_reconfig_txrx(uint32_t hz, uint32_t tx_block_bytes, uint8_t channels)
{
    const uint32_t rx_block_bytes = audio_calc_rx_block_bytes(hz, channels);
    audio_codec_set_sample_rate(&g_codec, hz);
    audio_io_start(hz, tx_block_bytes, rx_block_bytes, channels);
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    while (1) {}
}

void vAssertCalled(void)
{
    taskDISABLE_INTERRUPTS();
    while (1) {}
}

void dma0_ch0_isr(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)arg;
    xSemaphoreGiveFromISR(sem_dma_done, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void dma0_ch1_isr(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)arg;

    /* Streaming RX: every 20ms (or configured block) post buffer to processing task */
    if (g_cap_queue != NULL && g_cap_block_bytes != 0 && g_cap_samples_per_ch != 0 && g_i2s_hz > 0) {
        cap_msg_t msg = {
            .buf_stereo = g_cap_tc_ping ? cap_ping : cap_pong,
            .nbytes = g_cap_block_bytes,
            .hz = (uint32_t)g_i2s_hz,
            .samples_per_ch = g_cap_samples_per_ch,
            .channels = g_i2s_channels,
        };
        (void)xQueueSendFromISR(g_cap_queue, &msg, &xHigherPriorityTaskWoken);
        g_cap_tc_ping = !g_cap_tc_ping;
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void i2s_tx_stop(void)
{
    if (i2s0 == NULL) {
        return;
    }

    bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, 0);
    /* Also unlink I2S DMA requests from the peripheral. */
    bflb_i2s_link_txdma(i2s0, false);
    bflb_i2s_link_rxdma(i2s0, false);
    if (dma0_ch0 != NULL) {
        bflb_dma_channel_stop(dma0_ch0);
    }
    if (dma0_ch1 != NULL) {
        bflb_dma_channel_stop(dma0_ch1);
    }
    g_i2s_hz = 0;
}

static void audio_io_stop(void)
{
    i2s_tx_stop();
    g_cap_block_bytes = 0;
    g_cap_samples_per_ch = 0;
}

static void audio_io_start(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes, uint8_t channels)
{
    /* Full-duplex (TX+RX) restart. Allows brief capture gaps on reconfig. */
    audio_io_stop();

    i2s0 = bflb_device_get_by_name("i2s0");
    if (i2s0 == NULL) {
        printf("i2s0 not found\r\n");
        return;
    }

    struct bflb_i2s_config_s i2s_cfg;
    audio_fill_i2s_cfg(&i2s_cfg, samplerate_hz, channels);

    /* IMPORTANT: Avoid clock stop/start pop.
     * First time: bflb_i2s_init enables master clock.
     * Subsequent reconfig: use bflb_i2s_reconfig to keep master enable set.
     */
    if (!g_i2s_inited) {
        bflb_i2s_init(i2s0, &i2s_cfg);
        g_i2s_inited = true;
    } else {
        bflb_i2s_init(i2s0, &i2s_cfg);
    }
    bflb_i2s_feature_control(i2s0, I2S_CMD_CHANNEL_LR_MERGE, false);

    bflb_i2s_link_txdma(i2s0, true);
    bflb_i2s_link_rxdma(i2s0, true);

    struct bflb_dma_channel_config_s tx_config;
    struct bflb_dma_channel_config_s rx_config;
    audio_fill_dma_tx_cfg(&tx_config);
    audio_fill_dma_rx_cfg(&rx_config);

    if (dma0_ch0 == NULL) {
        dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    }
    if (dma0_ch1 == NULL) {
        dma0_ch1 = bflb_device_get_by_name("dma0_ch1");
    }
    bflb_dma_channel_init(dma0_ch0, &tx_config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    bflb_dma_channel_init(dma0_ch1, &rx_config);
    bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL);

    /* TX ping-pong (continuous). Local transfer descriptors are safe:
     * bflb_dma_channel_lli_reload() copies them into tx_llipool immediately.
     */
    struct bflb_dma_channel_lli_transfer_s tx_pp[2];
    tx_pp[0].src_addr = (uint32_t)(uintptr_t)pcm_ping;
    tx_pp[0].dst_addr = (uint32_t)DMA_ADDR_I2S_TDR;
    tx_pp[0].nbytes = tx_block_bytes;
    tx_pp[1].src_addr = (uint32_t)(uintptr_t)pcm_pong;
    tx_pp[1].dst_addr = (uint32_t)DMA_ADDR_I2S_TDR;
    tx_pp[1].nbytes = tx_block_bytes;

    int used_tx = bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool,
                                              (uint32_t)(sizeof(tx_llipool) / sizeof(tx_llipool[0])), tx_pp, 2);
    if (used_tx <= 0) {
        printf("tx dma lli reload failed: %d\r\n", used_tx);
        return;
    }
    bflb_dma_channel_lli_link_head(dma0_ch0, tx_llipool, (uint32_t)used_tx);

    /* RX ping-pong (continuous) */
    struct bflb_dma_channel_lli_transfer_s rx_pp[2];
    rx_pp[0].src_addr = (uint32_t)DMA_ADDR_I2S_RDR;
    rx_pp[0].dst_addr = (uint32_t)(uintptr_t)cap_ping;
    rx_pp[0].nbytes = rx_block_bytes;
    rx_pp[1].src_addr = (uint32_t)DMA_ADDR_I2S_RDR;
    rx_pp[1].dst_addr = (uint32_t)(uintptr_t)cap_pong;
    rx_pp[1].nbytes = rx_block_bytes;

    int used_rx = bflb_dma_channel_lli_reload(dma0_ch1, rx_llipool,
                                              (uint32_t)(sizeof(rx_llipool) / sizeof(rx_llipool[0])), rx_pp, 2);
    if (used_rx <= 0) {
        printf("rx dma lli reload failed: %d\r\n", used_rx);
        return;
    }
    bflb_dma_channel_lli_link_head(dma0_ch1, rx_llipool, (uint32_t)used_rx);

    /* Cache prep */
    bflb_l1c_dcache_clean_range((void *)pcm_ping, tx_block_bytes);
    bflb_l1c_dcache_clean_range((void *)pcm_pong, tx_block_bytes);
    bflb_l1c_dcache_clean_invalidate_range((void *)cap_ping, rx_block_bytes);
    bflb_l1c_dcache_clean_invalidate_range((void *)cap_pong, rx_block_bytes);

    /* State and interrupt hygiene */
    g_i2s_hz = (int)samplerate_hz;
    g_i2s_channels = (channels == 0) ? 1 : ((channels > 2) ? 2 : channels);
    g_cap_block_bytes = rx_block_bytes;
    /* rx_block_bytes contains interleaved stereo int16 samples.
     * samples_per_ch = bytes / (channels * bytes_per_sample)
     */
    g_cap_samples_per_ch =
        (rx_block_bytes == 0) ? 0 : (rx_block_bytes / ((uint32_t)g_i2s_channels * (uint32_t)sizeof(int16_t)));
    g_cap_tc_ping = true;

    bflb_dma_channel_tcint_clear(dma0_ch0);
    bflb_dma_channel_tcint_clear(dma0_ch1);
    sem_drain(sem_dma_done);

    bflb_dma_channel_start(dma0_ch0);
    bflb_dma_channel_start(dma0_ch1);
    bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX | I2S_CMD_DATA_ENABLE_RX);
}

static void rb_lock(void)
{
    taskENTER_CRITICAL();
}

static void rb_unlock(void)
{
    taskEXIT_CRITICAL();
}

static void pcm_fill_pcm_block(int16_t *dst, const int16_t *src, int samples_per_ch, int src_channels, int dst_channels,
                               uint32_t dst_block_bytes)
{
    if (dst == NULL || dst_block_bytes == 0) {
        return;
    }

    if (dst_channels <= 0) {
        dst_channels = 1;
    }
    if (dst_channels > 2) {
        dst_channels = 2;
    }

    if (samples_per_ch < 0) {
        samples_per_ch = 0;
    }
    uint32_t n = (uint32_t)samples_per_ch;
    const uint32_t max_n = dst_block_bytes / ((uint32_t)dst_channels * (uint32_t)sizeof(int16_t));
    if (n > max_n) {
        n = max_n;
    }

    if (src == NULL || n == 0) {
        memset(dst, 0, dst_block_bytes);
        bflb_l1c_dcache_clean_range((void *)dst, dst_block_bytes);
        return;
    }

    if (src_channels <= 0) {
        src_channels = 1;
    }
    if (src_channels > 2) {
        src_channels = 2;
    }

    if (dst_channels == 2) {
        if (src_channels == 2) {
            memcpy(dst, src, n * 2U * sizeof(int16_t));
        } else {
            for (int i = (int)n - 1; i >= 0; i--) {
                int16_t v = src[i];
                dst[i * 2 + 0] = v;
                dst[i * 2 + 1] = v;
            }
        }
    } else {
        /* dst mono */
        if (src_channels == 2) {
            for (uint32_t i = 0; i < n; i++) {
                int32_t l = src[i * 2 + 0];
                int32_t r = src[i * 2 + 1];
                dst[i] = (int16_t)((l + r) / 2);
            }
        } else {
            memcpy(dst, src, n * sizeof(int16_t));
        }
    }

    uint32_t used_bytes = n * (uint32_t)dst_channels * (uint32_t)sizeof(int16_t);
    if (used_bytes < dst_block_bytes) {
        memset(((uint8_t *)dst) + used_bytes, 0, dst_block_bytes - used_bytes);
    }

    bflb_l1c_dcache_clean_range((void *)dst, dst_block_bytes);
}

static void audio_src_sink(void *arg)
{
    audio_src_task_cfg_t *cfg = (audio_src_task_cfg_t *)arg;
    if (cfg == NULL || cfg->src == NULL || cfg->src_len == 0 || cfg->rb == NULL || cfg->src_done == NULL) {
        printf("audio_src_sink: bad cfg\r\n");
        if (cfg != NULL) {
            vPortFree(cfg);
        }
        vTaskDelete(NULL);
    }

    size_t off = 0;
    while (off < cfg->src_len) {
        if (cfg->stop != NULL && *cfg->stop) {
            break;
        }

        uint32_t empty = Ring_Buffer_Get_Empty_Length(cfg->rb);
        if (empty == 0) {
            if (cfg->sem_rb_space != NULL) {
                (void)xSemaphoreTake(cfg->sem_rb_space, portMAX_DELAY);
            } else {
                taskYIELD();
            }
            continue;
        }

        uint32_t chunk = empty;
        if (chunk > 512) {
            chunk = 512;
        }
        size_t remain = cfg->src_len - off;
        if (chunk > (uint32_t)remain) {
            chunk = (uint32_t)remain;
        }

        uint32_t wrote = Ring_Buffer_Write(cfg->rb, cfg->src + off, chunk);
        off += wrote;
        if (wrote == 0) {
            if (cfg->sem_rb_space != NULL) {
                (void)xSemaphoreTake(cfg->sem_rb_space, portMAX_DELAY);
            } else {
                taskYIELD();
            }
        }
    }

    *(cfg->src_done) = true;
    if (cfg->done_notify_task != NULL) {
        (void)xTaskNotify(cfg->done_notify_task, AUDIO_NOTIFY_SRC_DONE, eSetBits);
    }
    vPortFree(cfg);
    vTaskDelete(NULL);
}

static uint32_t record_one_wave(uint32_t hz)
{
    if (hz == 0U) {
        return 0;
    }

    /* Provide I2S clocks with silence (recorder TX) while capturing. */
    (void)audio_codec_set_mute(&g_codec, true);

    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    uint32_t tmp = 0;
    /* Clear any stale notify bits before starting */
    (void)xTaskNotifyWait(0, 0xFFFFFFFFu, &tmp, 0);

    /* Run long enough to fill the ring once (+slack). */
    const uint32_t ring_ms = (uint32_t)(((uint64_t)REC_MAX_SAMPLES * 1000ULL) / (uint64_t)hz);

    /* Reset ring state for this capture window (writer is audio_cap_proc). */
    g_rec_enable = false;
    g_rec_write = 0;
    g_rec_filled = false;

    recorder_cfg_t *rec_cfg = (recorder_cfg_t *)pvPortMalloc(sizeof(recorder_cfg_t));
    if (rec_cfg == NULL) {
        printf("record_one_wave: no mem for rec_cfg\r\n");
        return 0;
    }
    *rec_cfg = (recorder_cfg_t){
        .sample_rate_hz = hz,
        .duration_ms = ring_ms + 50U,
        .out_channels = CAP_CHANNELS,
        .sem_dma_done = sem_dma_done,
        .pcm_ping = pcm_ping,
        .pcm_pong = pcm_pong,
        .pcm_fill = pcm_fill_pcm_block,
        .reconfig_txrx = audio_reconfig_txrx,
        .audio_stop = audio_io_stop,
        .rec_enable = &g_rec_enable,
        .done_notify_task = self,
    };

    if (xTaskCreate(recorder_task, (char *)"recorder", 2048, rec_cfg, 3, NULL) != pdPASS) {
        vPortFree(rec_cfg);
        printf("record_one_wave: create recorder failed\r\n");
        return 0;
    }

    /* Wait recorder done. */
    while (1) {
        uint32_t bits = 0;
        if (xTaskNotifyWait(0, 0xFFFFFFFFu, &bits, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (bits & AUDIO_NOTIFY_REC_DONE) {
            break;
        }
    }
    uint32_t out_samples = 0;
    recorder_reorder_ring_s16(rec_mono_buf, (uint32_t)REC_MAX_SAMPLES, g_rec_write, g_rec_filled, &out_samples);
    audio_codec_set_mute(&g_codec, false);
    return out_samples;
}

static void audio_flow_task(void *arg)
{
    (void)arg;

    while (1) {
        play_one_mp3((const uint8_t *)gmp3_src_bufferData, (size_t)gmp3_src_bufferSize);
        vTaskDelay(pdMS_TO_TICKS(2000));
        play_one_mp3((const uint8_t *)gmp3_src_buffer2Data, (size_t)gmp3_src_buffer2Size);
        vTaskDelay(pdMS_TO_TICKS(2000));

        printf("record start\r\n");
        const uint32_t hz = 16000U;
        uint32_t out_samples = record_one_wave(hz);
        printf("record done: %u samples\r\n", out_samples);
        play_one_wave((const uint8_t *)rec_mono_buf, (size_t)out_samples * sizeof(int16_t), hz, 1);
        printf("record playback done\r\n");
    }

    vTaskDelete(NULL);
}

static void audio_cap_proc(void *arg)
{
    (void)arg;
    cap_msg_t msg;

    for (;;) {
        if (g_cap_queue == NULL) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (xQueueReceive(g_cap_queue, &msg, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        if (msg.buf_stereo == NULL || msg.nbytes == 0 || msg.samples_per_ch == 0) {
            continue;
        }

        /* Make DMA-written PCM visible */
        bflb_l1c_dcache_invalidate_range((void *)msg.buf_stereo, msg.nbytes);

        if (!g_rec_enable) {
            continue;
        }
        /* Split interleaved channels -> mono (use channel 0), write into ring. */
        uint32_t ch = (msg.channels == 0) ? 1U : (uint32_t)msg.channels;
        for (uint32_t i = 0; i < msg.samples_per_ch; i++) {
            rec_mono_buf[g_rec_write] = msg.buf_stereo[i * ch + 0];
            g_rec_write++;
            if (g_rec_write >= (uint32_t)REC_MAX_SAMPLES) {
                g_rec_write = 0;
                g_rec_filled = true;
            }
        }
    }
}

int main(void)
{
    audio_codec_cfg_t codec_cfg = {
        .sample_rate_hz = 48000,
        .channels = g_i2s_channels,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S,
        .bits = AUDIO_CODEC_BITS_16,
        .codec_master = false,
        /* Full-duplex AEC-like demo: keep ADC enabled during playback */
        .enable_adc = true,
        .enable_dac = true,
        .work_mode = AUDIO_CODEC_WORK_MODE_CODEC,
        .mic_input_mode = AUDIO_CODEC_MIC_INPUT_SINGLE,
    };

    board_init();

    /* gpio init */
    board_i2s_codec_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    if (i2c0 == NULL) {
        printf("i2c0 not found\r\n");
        while (1) {}
    }
    bflb_i2c_init(i2c0, 200000);
    /* init audio codec (scan + init) */
    if (AUDIO_CODEC_TYPE_UNKNOWN != audio_codec_scan(i2c0, &g_codec)) {
        int init_ret = audio_codec_init(&g_codec, &codec_cfg);
        const int boot_mic_pga_percent = 80;
        const int boot_vol_percent = 50;
        audio_codec_set_mute(&g_codec, false);
        int pga_ret = audio_codec_set_mic_pga(&g_codec, boot_mic_pga_percent);
        int vol_ret = audio_codec_set_volume(&g_codec, boot_vol_percent);
        printf("audio codec ready: %s (0x%02x) init=%d mic_pga=%d ret=%d setvol=%d ret=%d\r\n",
               g_codec.name, g_codec.i2c_addr, init_ret, boot_mic_pga_percent, pga_ret, boot_vol_percent, vol_ret);
    } else {
        printf("audio codec scan failed\r\n");
        while (1) {}
    }

    sem_dma_done = xSemaphoreCreateBinary();
    g_cap_queue = xQueueCreate(8, sizeof(cap_msg_t));
    sem_rb_space = xSemaphoreCreateBinary();

    Ring_Buffer_Init(&audio_src_rb, audio_src_buf, sizeof(audio_src_buf), rb_lock, rb_unlock);

    xTaskCreate(audio_flow_task, (char *)"audio_flow", 4096, NULL, 2, NULL);

    xTaskCreate(audio_cap_proc, (char *)"audio_cap", 2048, NULL, 2, NULL);

    vTaskStartScheduler();

    while (1) {}
}
