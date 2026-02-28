#include "wave_player.h"
#include "bflb_i2s.h"
#include "bflb_i2c.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_dma.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "audio_codec.h"
#include "ring_buffer.h"
#include "board.h"
#include "shell.h"
#if WAVE_PLAYER_ENABLE_RESAMPLE
#include "speex/speex_resampler.h"
#endif

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#if WAVE_PLAYER_TEST_WAVE
#include "incbin.h"
INCBIN(wave_src_buffer0, "../../bsp/common/audio_player/ruyuan_30s_16k_mono.wav");
INCBIN(wave_src_buffer1, "../../bsp/common/audio_player/therose_30s_11k.wav");
#endif

typedef void (*wave_player_pcm_fill_fn)(int16_t *dst, const int16_t *src, uint32_t src_bytes, uint32_t src_channels,
                                        uint32_t dst_channels, uint32_t dst_block_bytes);

typedef void (*wave_player_reconfig_fn)(uint32_t hz, uint32_t tx_block_bytes, uint8_t channels);

typedef void (*wave_player_stop_fn)(void);

typedef struct {
    const uint8_t *src;
    size_t src_len;
    Ring_Buffer_Type *rb;
    SemaphoreHandle_t sem_wave_rb_space;
} wave_src_task_cfg_t;

typedef struct {
    Ring_Buffer_Type *wave_rb;
    SemaphoreHandle_t sem_wave_rb_space;
    uint32_t src_sample_rate_hz;
    uint32_t output_sample_rate_hz;
    uint32_t tx_block_bytes;
    uint8_t src_channels;
    uint8_t out_channels;
    SemaphoreHandle_t sem_tx_dma_done;
    int16_t *pcm_ping;
    int16_t *pcm_pong;
    wave_player_pcm_fill_fn pcm_fill;
    wave_player_stop_fn audio_stop;
    TaskHandle_t done_notify_task;
} wave_player_cfg_t;

#if WAVE_PLAYER_ENABLE_REC
typedef struct {
    uint32_t sample_rate_hz;
    uint32_t duration_ms;
    uint8_t out_channels;
    SemaphoreHandle_t sem_rx_dma_done;
    int16_t *pcm_ping;
    int16_t *pcm_pong;
    wave_player_pcm_fill_fn pcm_fill;
    wave_player_stop_fn audio_stop;
    volatile bool *rec_enable;
} recorder_cfg_t;

typedef struct {
    uint8_t block_idx;
    uint32_t nbytes;
    uint32_t hz;
    uint32_t samples_per_ch;
    uint8_t channels;
    uint32_t seq;
} cap_msg_t;

#define WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH ((WAVE_PLAYER_REC_MAX_FS_HZ * WAVE_PLAYER_REC_FRAME_MS) / 1000U)
#define WAVE_PLAYER_CAP_MAX_SAMPLES_TOTAL  (WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH * WAVE_PLAYER_REC_CHANNELS)
#if (WAVE_PLAYER_CAP_DMA_BLOCKS < 2U)
#error "WAVE_PLAYER_CAP_DMA_BLOCKS must be >= 2"
#endif
#if WAVE_PLAYER_ENABLE_REC_RB
#define WAVE_PLAYER_REC_MAX_SAMPLES (WAVE_PLAYER_REC_MAX_FS_HZ * WAVE_PLAYER_REC_SECONDS * WAVE_PLAYER_REC_CHANNELS)
#endif
#endif

typedef struct {
    struct bflb_device_s *i2s_dev;
    struct bflb_device_s *i2c_dev;
    struct bflb_device_s *dma_ch_tx;
    struct bflb_device_s *dma_ch_rx;
    struct bflb_dma_channel_lli_pool_s tx_llipool[2 * (AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS + 4063) / 4064];
    audio_codec_dev_t codec;
    uint32_t src_hz;
    uint32_t out_hz;
    uint8_t src_channels;
    uint8_t out_channels;
    uint32_t tx_block_bytes;
    uint32_t rx_block_bytes;
    uint8_t volume;
    bool muted;
    bool io_started;
    SemaphoreHandle_t sem_tx_dma_done;
    SemaphoreHandle_t sem_wave_rb_space;
    Ring_Buffer_Type wave_rb;
    uint8_t wave_src_buf[AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS * 8 * sizeof(int16_t)];
    volatile bool stop;
    volatile bool paused;
    int16_t pcm_ping[AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS] __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
    int16_t pcm_pong[AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS] __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
    wave_player_cfg_t wave_cfg;
    TaskHandle_t wave_task;
    TaskHandle_t wave_src_task;
#if WAVE_PLAYER_ENABLE_REC
    SemaphoreHandle_t sem_rx_dma_done;
    struct bflb_dma_channel_lli_pool_s
        rx_llipool[WAVE_PLAYER_CAP_DMA_BLOCKS * (WAVE_PLAYER_CAP_MAX_SAMPLES_TOTAL + 4063) / 4064];
    int16_t cap_blocks[WAVE_PLAYER_CAP_DMA_BLOCKS][WAVE_PLAYER_CAP_MAX_SAMPLES_TOTAL]
        __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
    QueueHandle_t cap_queue;
    volatile uint8_t cap_isr_block_idx;
    volatile uint32_t cap_block_bytes;
    volatile uint32_t cap_samples_per_ch;
    volatile uint32_t cap_isr_seq;
#if WAVE_PLAYER_ENABLE_REC_RB
    int16_t rec_mono_buf[WAVE_PLAYER_REC_MAX_SAMPLES] __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
    volatile uint32_t rec_write;
    volatile bool rec_filled;
#endif
    volatile bool rec_enable;
    volatile bool rec_path_enabled;
    volatile bool rec_stop;
    TaskHandle_t cap_task;
    wave_player_rec_mono_frame_cb_t rec_cb;
    void *rec_cb_user_data;
#endif
} wave_player_ctx_t;

typedef struct {
    volatile uint32_t tx_isr_cnt;
    volatile uint32_t tx_sem_take_cnt;
    volatile uint32_t tx_fill_cnt;
    volatile uint32_t tx_isr_sem_gap_max;
    volatile uint32_t rx_isr_cnt;
    volatile uint32_t rx_q_send_ok;
    volatile uint32_t rx_q_send_fail;
    volatile uint32_t rx_q_depth_max;
    volatile uint32_t rx_msg_lag_max;
    volatile uint32_t rx_msg_overrun_risk_cnt;
} wave_player_diag_t;

static wave_player_ctx_t g_audio = {
    .i2s_dev = NULL,
    .i2c_dev = NULL,
    .dma_ch_tx = NULL,
    .dma_ch_rx = NULL,
    .src_hz = 0,
    .out_hz = 0,
    .src_channels = 0,
    .out_channels = 0,
    .tx_block_bytes = 0,
    .rx_block_bytes = 0,
    .volume = 50,
    .muted = false,
    .io_started = false,
    .sem_tx_dma_done = NULL,
    .sem_wave_rb_space = NULL,
    .stop = false,
    .paused = false,
    .wave_task = NULL,
    .wave_src_task = NULL,
#if WAVE_PLAYER_ENABLE_REC
    .sem_rx_dma_done = NULL,
    .cap_queue = NULL,
    .cap_isr_block_idx = 0,
    .cap_block_bytes = 0,
    .cap_samples_per_ch = 0,
    .cap_isr_seq = 0,
#if WAVE_PLAYER_ENABLE_REC_RB
    .rec_write = 0,
    .rec_filled = false,
#endif
    .rec_enable = false,
    .rec_path_enabled = false,
    .rec_stop = false,
    .cap_task = NULL,
    .rec_cb = NULL,
    .rec_cb_user_data = NULL,
#endif
};

static wave_player_diag_t g_wave_diag = { 0 };

#if WAVE_PLAYER_TEST_WAVE || WAVE_PLAYER_ENABLE_REC
static void wave_player_play_one_wave(const uint8_t *src, size_t src_len, uint32_t sample_rate_hz,
                                      uint8_t src_channels);
#endif

static void wave_player_diag_reset(void)
{
    memset((void *)&g_wave_diag, 0, sizeof(g_wave_diag));
}

static void sem_drain(SemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return;
    }
    while (xSemaphoreTake(sem, 0) == pdTRUE) {}
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
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };
}

#if WAVE_PLAYER_ENABLE_REC
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
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };
}
#endif

static void dma_ch_tx_isr(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)arg;
    g_wave_diag.tx_isr_cnt++;
    {
        uint32_t gap = g_wave_diag.tx_isr_cnt - g_wave_diag.tx_sem_take_cnt;
        if (gap > g_wave_diag.tx_isr_sem_gap_max) {
            g_wave_diag.tx_isr_sem_gap_max = gap;
        }
    }
    xSemaphoreGiveFromISR(g_audio.sem_tx_dma_done, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

#if WAVE_PLAYER_ENABLE_REC
static void dma_ch_rx_isr(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)arg;
    g_wave_diag.rx_isr_cnt++;

    if (g_audio.sem_rx_dma_done != NULL) {
        xSemaphoreGiveFromISR(g_audio.sem_rx_dma_done, &xHigherPriorityTaskWoken);
    }

    if (g_audio.rec_enable && g_audio.cap_queue != NULL && g_audio.cap_block_bytes != 0 &&
        g_audio.cap_samples_per_ch != 0 && g_audio.out_hz > 0) {
        uint32_t seq = g_audio.cap_isr_seq + 1U;
        g_audio.cap_isr_seq = seq;
        uint8_t blk = g_audio.cap_isr_block_idx;
        cap_msg_t msg = {
            .block_idx = blk,
            .nbytes = g_audio.cap_block_bytes,
            .hz = (uint32_t)g_audio.out_hz,
            .samples_per_ch = g_audio.cap_samples_per_ch,
            .channels = g_audio.out_channels,
            .seq = seq,
        };
        if (xQueueSendFromISR(g_audio.cap_queue, &msg, &xHigherPriorityTaskWoken) == pdTRUE) {
            g_wave_diag.rx_q_send_ok++;
            {
                UBaseType_t depth = uxQueueMessagesWaitingFromISR(g_audio.cap_queue);
                if ((uint32_t)depth > g_wave_diag.rx_q_depth_max) {
                    g_wave_diag.rx_q_depth_max = (uint32_t)depth;
                }
            }
        } else {
            g_wave_diag.rx_q_send_fail++;
        }
        blk++;
        if (blk >= (uint8_t)WAVE_PLAYER_CAP_DMA_BLOCKS) {
            blk = 0U;
        }
        g_audio.cap_isr_block_idx = blk;
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif

static void i2s_tx_rx_stop(void)
{
    if (g_audio.i2s_dev == NULL) {
        g_audio.io_started = false;
        return;
    }

    bflb_i2s_feature_control(g_audio.i2s_dev, I2S_CMD_DATA_ENABLE, 0);
    bflb_i2s_link_txdma(g_audio.i2s_dev, false);
    bflb_i2s_link_rxdma(g_audio.i2s_dev, false);
    if (g_audio.dma_ch_tx != NULL) {
        bflb_dma_channel_stop(g_audio.dma_ch_tx);
    }
    if (g_audio.dma_ch_rx != NULL) {
        bflb_dma_channel_stop(g_audio.dma_ch_rx);
    }
    g_audio.io_started = false;
}

static void audio_io_pause(void)
{
    if (g_audio.i2s_dev == NULL || g_audio.dma_ch_tx == NULL) {
        return;
    }
    bflb_i2s_feature_control(g_audio.i2s_dev, I2S_CMD_DATA_ENABLE, 0);
}

static void audio_io_resume(void)
{
    if (g_audio.i2s_dev == NULL || g_audio.dma_ch_tx == NULL) {
        return;
    }

    uint32_t data_enable = I2S_CMD_DATA_ENABLE_TX;
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.rec_path_enabled && g_audio.rx_block_bytes > 0U) {
        data_enable |= I2S_CMD_DATA_ENABLE_RX;
    }
#endif
    bflb_i2s_feature_control(g_audio.i2s_dev, I2S_CMD_DATA_ENABLE, data_enable);
}

static void audio_io_stop(void)
{
    i2s_tx_rx_stop();
#if WAVE_PLAYER_ENABLE_REC
    g_audio.cap_block_bytes = 0;
    g_audio.cap_samples_per_ch = 0;
    g_audio.rec_path_enabled = false;
#endif
}

static int wave_player_apply_rec_mic_pga(uint8_t pga)
{
    if (pga > 100U) {
        pga = 100U;
    }
    return audio_codec_set_mic_pga(&g_audio.codec, (int)pga);
}

void wave_player_config(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes, uint8_t channels)
{
    uint32_t source_hz = samplerate_hz;
    uint8_t src_channels = channels;
    uint8_t out_channels = (uint8_t)AUDIO_OUT_CHANNELS;
    uint32_t out_hz = 0;
    const uint32_t sample_bytes = (uint32_t)sizeof(int16_t);
    uint32_t tx_frame_bytes = 0;
    bool rec_path_enable = false;

#if WAVE_PLAYER_ENABLE_REC
    rec_path_enable = g_audio.rec_enable;
    rx_block_bytes = rec_path_enable ? (WAVE_PLAYER_CAP_MAX_SAMPLES_TOTAL * (uint32_t)sizeof(int16_t)) : 0U;
#endif

    if (source_hz == 0U) {
        source_hz = 16000U;
    }
    if (src_channels == 0U) {
        src_channels = 1U;
    }
    if (src_channels > 2U) {
        src_channels = 2U;
    }
    if (out_channels == 0U) {
        out_channels = 1U;
    }
    if (out_channels > 2U) {
        out_channels = 2U;
    }

#if (WAVE_PLAYER_FIX_SAMPLE_RATE > 0U)
    out_hz = (uint32_t)WAVE_PLAYER_FIX_SAMPLE_RATE;
#else
    out_hz = source_hz;
#endif

#if (WAVE_PLAYER_FIX_SAMPLE_RATE > 0U) && !WAVE_PLAYER_ENABLE_RESAMPLE
    if (source_hz != out_hz) {
        printf("wave_player: fix sample rate(%u) requires resample for src_hz=%u\r\n", out_hz, source_hz);
        return;
    }
#endif

    tx_frame_bytes = (uint32_t)out_channels * sample_bytes;
    if (tx_block_bytes == 0U || tx_block_bytes > sizeof(g_audio.pcm_ping) || (tx_block_bytes % tx_frame_bytes) != 0U) {
        printf("wave_player: bad tx_block_bytes=%u, frame=%u, max=%u\r\n", tx_block_bytes, tx_frame_bytes,
               (unsigned)sizeof(g_audio.pcm_ping));
        return;
    }

    if (g_audio.src_hz == source_hz && g_audio.out_hz == out_hz && g_audio.src_channels == src_channels &&
        g_audio.out_channels == out_channels && g_audio.tx_block_bytes == tx_block_bytes &&
        g_audio.rx_block_bytes == rx_block_bytes &&
#if WAVE_PLAYER_ENABLE_REC
        g_audio.rec_path_enabled == rec_path_enable &&
#endif
        g_audio.io_started) {
        return;
    }

    printf("audio_io_start: src_hz=%u, i2s_hz=%u, tx_block_bytes=%u, rx_block_bytes=%u, channels=%u, rec=%u\r\n",
           source_hz, out_hz, tx_block_bytes, rx_block_bytes, src_channels, rec_path_enable ? 1U : 0U);

    audio_io_stop();
    wave_player_diag_reset();

    struct bflb_device_s *i2s0 = g_audio.i2s_dev;
    if (i2s0 == NULL) {
        printf("i2s0 not found\r\n");
        return;
    }

    board_audio_pll_config_for_rate(out_hz);

    struct bflb_i2s_config_s i2s_cfg;
    audio_fill_i2s_cfg(&i2s_cfg, out_hz, out_channels);

    bflb_i2s_init(i2s0, &i2s_cfg);

    bflb_i2s_feature_control(i2s0, I2S_CMD_CHANNEL_LR_MERGE, true);

    bflb_i2s_link_txdma(i2s0, true);
#if WAVE_PLAYER_ENABLE_REC
    if (rec_path_enable && g_audio.dma_ch_rx != NULL && rx_block_bytes > 0U) {
        bflb_i2s_link_rxdma(i2s0, true);
    }
#endif

    struct bflb_dma_channel_config_s tx_config;
    audio_fill_dma_tx_cfg(&tx_config);

    if (g_audio.dma_ch_tx == NULL) {
        printf("dma tx not configured\r\n");
        return;
    }
    bflb_dma_channel_init(g_audio.dma_ch_tx, &tx_config);
    bflb_dma_channel_irq_attach(g_audio.dma_ch_tx, dma_ch_tx_isr, NULL);

#if WAVE_PLAYER_ENABLE_REC
    if (rec_path_enable && g_audio.dma_ch_rx != NULL && rx_block_bytes > 0U) {
        struct bflb_dma_channel_config_s rx_config;
        audio_fill_dma_rx_cfg(&rx_config);
        bflb_dma_channel_init(g_audio.dma_ch_rx, &rx_config);
        bflb_dma_channel_irq_attach(g_audio.dma_ch_rx, dma_ch_rx_isr, NULL);
    }
#endif

    struct bflb_dma_channel_lli_transfer_s tx_pp[2];
    tx_pp[0].src_addr = (uint32_t)(uintptr_t)g_audio.pcm_ping;
    tx_pp[0].dst_addr = (uint32_t)DMA_ADDR_I2S_TDR;
    tx_pp[0].nbytes = tx_block_bytes;
    tx_pp[1].src_addr = (uint32_t)(uintptr_t)g_audio.pcm_pong;
    tx_pp[1].dst_addr = (uint32_t)DMA_ADDR_I2S_TDR;
    tx_pp[1].nbytes = tx_block_bytes;

    int used_tx =
        bflb_dma_channel_lli_reload(g_audio.dma_ch_tx, g_audio.tx_llipool,
                                    (uint32_t)(sizeof(g_audio.tx_llipool) / sizeof(g_audio.tx_llipool[0])), tx_pp, 2);
    if (used_tx <= 0) {
        printf("tx dma lli reload failed: %d\r\n", used_tx);
        return;
    }
    bflb_dma_channel_lli_link_head(g_audio.dma_ch_tx, g_audio.tx_llipool, (uint32_t)used_tx);

#if WAVE_PLAYER_ENABLE_REC
    if (rec_path_enable && g_audio.dma_ch_rx != NULL && rx_block_bytes > 0U) {
        struct bflb_dma_channel_lli_transfer_s rx_pp[WAVE_PLAYER_CAP_DMA_BLOCKS];
        for (uint32_t i = 0U; i < (uint32_t)WAVE_PLAYER_CAP_DMA_BLOCKS; i++) {
            rx_pp[i].src_addr = (uint32_t)DMA_ADDR_I2S_RDR;
            rx_pp[i].dst_addr = (uint32_t)(uintptr_t)g_audio.cap_blocks[i];
            rx_pp[i].nbytes = rx_block_bytes;
        }

        int used_rx =
            bflb_dma_channel_lli_reload(g_audio.dma_ch_rx, g_audio.rx_llipool,
                                        (uint32_t)(sizeof(g_audio.rx_llipool) / sizeof(g_audio.rx_llipool[0])), rx_pp,
                                        (uint32_t)WAVE_PLAYER_CAP_DMA_BLOCKS);
        if (used_rx <= 0) {
            printf("rx dma lli reload failed: %d\r\n", used_rx);
            return;
        }
        bflb_dma_channel_lli_link_head(g_audio.dma_ch_rx, g_audio.rx_llipool, (uint32_t)used_rx);
    }
#endif

    memset(g_audio.pcm_ping, 0, sizeof(g_audio.pcm_ping));
    memset(g_audio.pcm_pong, 0, sizeof(g_audio.pcm_pong));
    bflb_l1c_dcache_clean_range((void *)g_audio.pcm_ping, sizeof(g_audio.pcm_ping));
    bflb_l1c_dcache_clean_range((void *)g_audio.pcm_pong, sizeof(g_audio.pcm_pong));

#if WAVE_PLAYER_ENABLE_REC
    if (rec_path_enable && g_audio.dma_ch_rx != NULL && rx_block_bytes > 0U) {
        for (uint32_t i = 0U; i < (uint32_t)WAVE_PLAYER_CAP_DMA_BLOCKS; i++) {
            bflb_l1c_dcache_clean_invalidate_range((void *)g_audio.cap_blocks[i], rx_block_bytes);
        }
    }
#endif

    g_audio.src_hz = source_hz;
    g_audio.out_hz = out_hz;
    g_audio.src_channels = src_channels;
    g_audio.out_channels = out_channels;
    g_audio.tx_block_bytes = tx_block_bytes;
    g_audio.rx_block_bytes = rx_block_bytes;

#if WAVE_PLAYER_ENABLE_REC
    g_audio.rec_path_enabled = rec_path_enable && (rx_block_bytes > 0U);
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        g_audio.cap_block_bytes = rx_block_bytes;
        g_audio.cap_samples_per_ch = rx_block_bytes / ((uint32_t)g_audio.out_channels * (uint32_t)sizeof(int16_t));
        g_audio.cap_isr_block_idx = 0U;
        g_audio.cap_isr_seq = 0U;
    } else {
        g_audio.cap_block_bytes = 0;
        g_audio.cap_samples_per_ch = 0;
        g_audio.cap_isr_block_idx = 0U;
        g_audio.cap_isr_seq = 0U;
    }
#endif

    bflb_dma_channel_tcint_clear(g_audio.dma_ch_tx);
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        bflb_dma_channel_tcint_clear(g_audio.dma_ch_rx);
    }
#endif
    sem_drain(g_audio.sem_tx_dma_done);
    bflb_dma_channel_start(g_audio.dma_ch_tx);
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        sem_drain(g_audio.sem_rx_dma_done);
    }
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        bflb_dma_channel_start(g_audio.dma_ch_rx);
    }
#endif
    {
        uint32_t data_enable = I2S_CMD_DATA_ENABLE_TX;
#if WAVE_PLAYER_ENABLE_REC
        if (g_audio.rec_path_enabled) {
            data_enable |= I2S_CMD_DATA_ENABLE_RX;
        }
#endif
        bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, data_enable);
    }
    g_audio.io_started = true;
}

void wave_player_dump_stack(void)
{
#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
    printf("stack current=%u\r\n", (unsigned)uxTaskGetStackHighWaterMark(NULL));
    if (g_audio.wave_task != NULL) {
        printf("stack wave_task=%u\r\n", (unsigned)uxTaskGetStackHighWaterMark(g_audio.wave_task));
    }
    if (g_audio.wave_src_task != NULL) {
        printf("stack wave_src=%u\r\n", (unsigned)uxTaskGetStackHighWaterMark(g_audio.wave_src_task));
    }
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.cap_task != NULL) {
        printf("stack audio_cap=%u\r\n", (unsigned)uxTaskGetStackHighWaterMark(g_audio.cap_task));
    }
#endif
#else
    printf("stack watermark not enabled (INCLUDE_uxTaskGetStackHighWaterMark=0)\r\n");
#endif
}

static void rb_lock(void)
{
    taskENTER_CRITICAL();
}

static void rb_unlock(void)
{
    taskEXIT_CRITICAL();
}

static void rb_read_block_aligned(Ring_Buffer_Type *rb, uint8_t *dst, uint32_t dst_bytes, uint32_t frame_bytes)
{
    uint32_t avail = Ring_Buffer_Get_Length(rb);
    uint32_t read_bytes = (avail < dst_bytes) ? avail : dst_bytes;

    read_bytes = (read_bytes / frame_bytes) * frame_bytes;
    if (read_bytes > 0U) {
        Ring_Buffer_Read(rb, dst, read_bytes);
    }
    if (read_bytes < dst_bytes) {
        memset(dst + read_bytes, 0, dst_bytes - read_bytes);
    }
}

static void pcm_fill_pcm_block(int16_t *dst, const int16_t *src, uint32_t src_bytes, uint32_t src_channels,
                               uint32_t dst_channels, uint32_t dst_block_bytes)
{
    if (src_channels == 0U) {
        src_channels = 1U;
    }
    if (src_channels > 2U) {
        src_channels = 2U;
    }
    if (dst_channels == 0U) {
        dst_channels = 1U;
    }
    if (dst_channels > 2U) {
        dst_channels = 2U;
    }

    uint32_t n = src_bytes / (src_channels * sizeof(int16_t));
    const uint32_t max_n = dst_block_bytes / (dst_channels * sizeof(int16_t));
    if (n > max_n) {
        n = max_n;
    }

    if (src == NULL || n == 0) {
        memset(dst, 0, dst_block_bytes);
        bflb_l1c_dcache_clean_range((void *)dst, dst_block_bytes);
        return;
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

static void wave_player_task(void *arg)
{
    wave_player_cfg_t *cfg = (wave_player_cfg_t *)arg;

    if (cfg == NULL || cfg->wave_rb == NULL || cfg->pcm_ping == NULL || cfg->pcm_pong == NULL ||
        cfg->pcm_fill == NULL || cfg->audio_stop == NULL || cfg->sem_tx_dma_done == NULL) {
        printf("wave_player: bad cfg\r\n");
        g_audio.wave_task = NULL;
        vTaskDelete(NULL);
        return;
    }

    uint8_t src_channels = cfg->src_channels;
    if (src_channels == 0) {
        src_channels = 1;
    }
    if (src_channels > 2) {
        src_channels = 2;
    }

    uint8_t out_channels = cfg->out_channels;
    if (out_channels == 0) {
        out_channels = 1;
    }
    if (out_channels > 2) {
        out_channels = 2;
    }

    uint32_t src_hz = cfg->src_sample_rate_hz;
    if (src_hz == 0U) {
        src_hz = 16000U;
    }
    uint32_t out_hz = cfg->output_sample_rate_hz;
    if (out_hz == 0U) {
        out_hz = src_hz;
    }
#if !WAVE_PLAYER_ENABLE_RESAMPLE
    out_hz = src_hz;
#endif

    const uint32_t sample_bytes = (uint32_t)sizeof(int16_t);
    const uint32_t out_frame_bytes = (uint32_t)out_channels * sample_bytes;
    const uint32_t src_frame_bytes = (uint32_t)src_channels * sample_bytes;
    const uint32_t tx_block_bytes = cfg->tx_block_bytes;
    uint32_t out_samples_per_ch = 0;
    uint32_t src_block_bytes = 0;

    if (tx_block_bytes == 0U || tx_block_bytes > sizeof(g_audio.pcm_ping) || (tx_block_bytes % out_frame_bytes) != 0U) {
        printf("wave_player: bad tx_block_bytes=%u, frame=%u, max=%u\r\n", tx_block_bytes, out_frame_bytes,
               (unsigned)sizeof(g_audio.pcm_ping));
        g_audio.wave_task = NULL;
        vTaskDelete(NULL);
        return;
    }

    out_samples_per_ch = tx_block_bytes / out_frame_bytes;
    if (out_samples_per_ch == 0U) {
        printf("wave_player: out_samples_per_ch is zero\r\n");
        g_audio.wave_task = NULL;
        vTaskDelete(NULL);
        return;
    }

    {
        uint64_t src_block_bytes_u64 = (uint64_t)out_samples_per_ch * (uint64_t)src_channels * (uint64_t)sample_bytes;
        if (src_block_bytes_u64 > (uint64_t)UINT32_MAX) {
            printf("wave_player: src_block_bytes overflow (%llu)\r\n", (unsigned long long)src_block_bytes_u64);
            g_audio.wave_task = NULL;
            vTaskDelete(NULL);
            return;
        }
        src_block_bytes = (uint32_t)src_block_bytes_u64;
    }

#if WAVE_PLAYER_ENABLE_RESAMPLE
    bool use_resample = (src_hz != out_hz);
    uint32_t src_need_per_ch = 0;
    uint32_t src_alloc_bytes = src_block_bytes;

    if (use_resample) {
        uint64_t src_need_per_ch_u64 =
            (((uint64_t)out_samples_per_ch * (uint64_t)src_hz + (uint64_t)out_hz - 1U) / (uint64_t)out_hz) + 16U;
        uint64_t src_cap_per_ch_u64 = src_need_per_ch_u64 + 16U;
        uint64_t src_alloc_bytes_u64 = src_cap_per_ch_u64 * (uint64_t)src_channels * (uint64_t)sample_bytes;

        if (src_need_per_ch_u64 > (uint64_t)UINT32_MAX || src_alloc_bytes_u64 > (uint64_t)UINT32_MAX) {
            printf("wave_player: resample block overflow need=%llu alloc=%llu\r\n",
                   (unsigned long long)src_need_per_ch_u64, (unsigned long long)src_alloc_bytes_u64);
            g_audio.wave_task = NULL;
            vTaskDelete(NULL);
            return;
        }
        src_need_per_ch = (uint32_t)src_need_per_ch_u64;
        src_alloc_bytes = (uint32_t)src_alloc_bytes_u64;
    }
#else
    const uint32_t src_alloc_bytes = src_block_bytes;
#endif

    int16_t *src_pcm = (int16_t *)pvPortMalloc(src_alloc_bytes);
#if WAVE_PLAYER_ENABLE_RESAMPLE
    int16_t *resampled_pcm = NULL;
    uint32_t src_have_per_ch = 0;
    SpeexResamplerState *resampler = NULL;
#endif
    if (src_pcm == NULL) {
        printf("wave_player: alloc scratch failed\r\n");
        vTaskDelete(NULL);
        return;
    }
#if WAVE_PLAYER_ENABLE_RESAMPLE
    if (use_resample) {
        int err = RESAMPLER_ERR_SUCCESS;
        resampled_pcm = (int16_t *)pvPortMalloc(src_block_bytes);
        if (resampled_pcm == NULL) {
            printf("wave_player: alloc resample buf failed\r\n");
            use_resample = false;
        } else {
            resampler = speex_resampler_init((spx_uint32_t)src_channels, (spx_uint32_t)src_hz, (spx_uint32_t)out_hz,
                                             WAVE_PLAYER_RESAMPLE_QUALITY, &err);
            if (resampler == NULL || err != RESAMPLER_ERR_SUCCESS) {
                printf("wave_player: resampler init failed, err=%d\r\n", err);
                use_resample = false;
            } else {
                (void)speex_resampler_skip_zeros(resampler);
                printf("wave_player: enable resample %u->%u, ch=%u, q=%d\r\n", src_hz, out_hz, src_channels,
                       WAVE_PLAYER_RESAMPLE_QUALITY);
            }
        }
    }
#endif

    bool ping = true;
    bool need_prime_pong = false;
    bool dma_started = false;

    while (1) {
        if (g_audio.stop) {
            break;
        }
#if 0
        if (g_audio.paused) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
#endif
        int16_t *play_pcm = src_pcm;
#if WAVE_PLAYER_ENABLE_RESAMPLE
        uint32_t retry = 0;
        if (use_resample) {
            while (src_have_per_ch < src_need_per_ch) {
                uint32_t read_need_samples = src_need_per_ch - src_have_per_ch;
                uint32_t read_need_bytes = read_need_samples * (uint32_t)src_channels * sample_bytes;
                uint32_t rb_avail = Ring_Buffer_Get_Length(cfg->wave_rb);
                uint32_t read_bytes = (rb_avail < read_need_bytes) ? rb_avail : read_need_bytes;
                uint32_t write_off_bytes = src_have_per_ch * (uint32_t)src_channels * sample_bytes;

                read_bytes = (read_bytes / src_frame_bytes) * src_frame_bytes;

                if (read_bytes > 0) {
                    Ring_Buffer_Read(cfg->wave_rb, ((uint8_t *)src_pcm) + write_off_bytes, read_bytes);
                    src_have_per_ch += read_bytes / ((uint32_t)src_channels * sample_bytes);
                }

                if (read_bytes < read_need_bytes) {
                    /* if not start yet，we wait for more data */
                    if (dma_started == true && need_prime_pong == false) {
                        uint32_t pad_bytes = read_need_bytes - read_bytes;
                        memset(((uint8_t *)src_pcm) + write_off_bytes + read_bytes, 0, pad_bytes);
                        src_have_per_ch += pad_bytes / ((uint32_t)src_channels * sample_bytes);
                        break;
                    } else {
                        if (retry++ > 100) {
                            break;
                        }
                        vTaskDelay(pdMS_TO_TICKS(2));
                    }
                }
            }

            spx_uint32_t in_len = src_have_per_ch;
            spx_uint32_t out_len = out_samples_per_ch;
            int err = speex_resampler_process_interleaved_int(resampler, src_pcm, &in_len, resampled_pcm, &out_len);
            if (err != RESAMPLER_ERR_SUCCESS) {
                printf("wave_player: resample failed (%d), mute block\r\n", err);
                out_len = 0;
                in_len = 0;
            }

            if (in_len < src_have_per_ch) {
                uint32_t remain_per_ch = src_have_per_ch - in_len;
                memmove(src_pcm, src_pcm + (in_len * (spx_uint32_t)src_channels),
                        remain_per_ch * (uint32_t)src_channels * sizeof(int16_t));
                src_have_per_ch = remain_per_ch;
            } else {
                src_have_per_ch = 0;
            }

            if (out_len < out_samples_per_ch) {
                memset(resampled_pcm + out_len * (spx_uint32_t)src_channels, 0,
                       (out_samples_per_ch - out_len) * (uint32_t)src_channels * sizeof(int16_t));
            }

            play_pcm = resampled_pcm;
        } else
#endif
        {
            rb_read_block_aligned(cfg->wave_rb, (uint8_t *)src_pcm, src_block_bytes, src_frame_bytes);
        }

        if (Ring_Buffer_Get_Length(cfg->wave_rb) < (Ring_Buffer_Get_Size(cfg->wave_rb) >> 1)) {
            xSemaphoreGive(cfg->sem_wave_rb_space);
        }
        if (!dma_started) {
            cfg->pcm_fill(cfg->pcm_ping, play_pcm, src_block_bytes, src_channels, (int)out_channels, tx_block_bytes);
            dma_started = true;
            ping = true;
            need_prime_pong = true;
            continue;
        }

        if (need_prime_pong) {
            cfg->pcm_fill(cfg->pcm_pong, play_pcm, src_block_bytes, src_channels, (int)out_channels, tx_block_bytes);
            need_prime_pong = false;
            continue;
        }
#if 0
        if (g_audio.paused) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
#endif
        xSemaphoreTake(cfg->sem_tx_dma_done, portMAX_DELAY);
        g_wave_diag.tx_sem_take_cnt++;

        int16_t *out = ping ? cfg->pcm_ping : cfg->pcm_pong;
        cfg->pcm_fill(out, play_pcm, src_block_bytes, src_channels, (int)out_channels, tx_block_bytes);
        g_wave_diag.tx_fill_cnt++;
        ping = !ping;
    }
    printf("wave_player: stopping\r\n");
    cfg->audio_stop();
    memset(cfg->pcm_ping, 0, src_block_bytes);
    memset(cfg->pcm_pong, 0, src_block_bytes);
#if WAVE_PLAYER_ENABLE_RESAMPLE
    if (use_resample && resampler != NULL) {
        speex_resampler_destroy(resampler);
    }
    if (resampled_pcm != NULL) {
        vPortFree(resampled_pcm);
    }
#endif
    vPortFree(src_pcm);
    g_audio.wave_task = NULL;
    g_audio.stop = false;
    vTaskDelete(NULL);
}

#if WAVE_PLAYER_ENABLE_REC
#if WAVE_PLAYER_ENABLE_REC_RB
static uint32_t gcd_u32(uint32_t a, uint32_t b)
{
    while (b != 0U) {
        uint32_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

static void rotate_left_s16(int16_t *buf, uint32_t len, uint32_t shift)
{
    if (buf == NULL || len <= 1U) {
        return;
    }
    shift %= len;
    if (shift == 0U) {
        return;
    }

    uint32_t g = gcd_u32(len, shift);
    for (uint32_t start = 0; start < g; start++) {
        int16_t tmp = buf[start];
        uint32_t j = start;
        while (1) {
            uint32_t k = j + shift;
            if (k >= len) {
                k -= len;
            }
            if (k == start) {
                break;
            }
            buf[j] = buf[k];
            j = k;
        }
        buf[j] = tmp;
    }
}

static void recorder_reorder_ring_s16(int16_t *buf, uint32_t len, uint32_t write_pos, bool filled,
                                      uint32_t *out_samples)
{
    if (out_samples != NULL) {
        *out_samples = 0;
    }
    if (buf == NULL || len == 0U) {
        return;
    }

    if (!filled) {
        if (write_pos > len) {
            write_pos = len;
        }
        if (out_samples != NULL) {
            *out_samples = write_pos;
        }
        return;
    }

    if (out_samples != NULL) {
        *out_samples = len;
    }

    write_pos %= len;
    if (write_pos != 0U) {
        rotate_left_s16(buf, len, write_pos);
    }
}
#endif

static void audio_cap_proc(void *arg)
{
    (void)arg;
    cap_msg_t msg;
    static int16_t mono_frame[WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH];
    int16_t *buf_stereo = NULL;

    for (;;) {
        if (g_audio.cap_queue == NULL) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (xQueueReceive(g_audio.cap_queue, &msg, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (msg.block_idx >= (uint8_t)WAVE_PLAYER_CAP_DMA_BLOCKS || msg.nbytes == 0 || msg.samples_per_ch == 0) {
            continue;
        }
        buf_stereo = g_audio.cap_blocks[msg.block_idx];
        if (buf_stereo == NULL) {
            continue;
        }

        bflb_l1c_dcache_invalidate_range((void *)buf_stereo, msg.nbytes);
        {
            uint32_t cur_seq = g_audio.cap_isr_seq;
            uint32_t lag = (cur_seq >= msg.seq) ? (cur_seq - msg.seq) : 0U;
            if (lag > g_wave_diag.rx_msg_lag_max) {
                g_wave_diag.rx_msg_lag_max = lag;
            }
            if (lag >= (uint32_t)WAVE_PLAYER_CAP_DMA_BLOCKS) {
                g_wave_diag.rx_msg_overrun_risk_cnt++;
            }
        }
        uint32_t ch = (msg.channels == 0) ? 1U : (uint32_t)msg.channels;
        wave_player_rec_mono_frame_cb_t cb = g_audio.rec_cb;
        void *cb_user_data = g_audio.rec_cb_user_data;

        if (!g_audio.rec_enable) {
            continue;
        }
#if !WAVE_PLAYER_ENABLE_REC_RB
        if (cb == NULL) {
            continue;
        }
#endif

        uint32_t processed = 0U;
        while (processed < msg.samples_per_ch) {
            uint32_t chunk = msg.samples_per_ch - processed;
            if (chunk > WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH) {
                chunk = WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH;
            }

            for (uint32_t i = 0; i < chunk; i++) {
                uint32_t src_idx = (processed + i) * ch;
                int16_t mono = buf_stereo[src_idx];
                if (ch > 1U) {
#if (WAVE_PLAYER_REC_MONO_SOURCE == 0U)
                    mono = buf_stereo[src_idx];
#elif (WAVE_PLAYER_REC_MONO_SOURCE == 1U)
                    mono = buf_stereo[src_idx + 1U];
#else
                    int32_t l = (int32_t)buf_stereo[src_idx];
                    int32_t r = (int32_t)buf_stereo[src_idx + 1U];
                    mono = (int16_t)((l + r) / 2);
#endif
                }
                mono_frame[i] = mono;

#if WAVE_PLAYER_ENABLE_REC_RB
                g_audio.rec_mono_buf[g_audio.rec_write] = mono;
                g_audio.rec_write++;
                if (g_audio.rec_write >= (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES) {
                    g_audio.rec_write = 0;
                    g_audio.rec_filled = true;
                }
#endif
            }

            if (cb != NULL) {
                cb(mono_frame, chunk, msg.hz, cb_user_data);
            }

            processed += chunk;
        }
    }
}
#endif

/* Initialize audio output */
int wave_player_init(const wave_player_hw_cfg_t *hw_cfg)
{
    audio_codec_cfg_t codec_cfg = {
        .sample_rate_hz = 48000,
        .channels = 2,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S,
        .bits = AUDIO_CODEC_BITS_16,
        .codec_master = false,
        .enable_adc = true,
        .enable_dac = true,
        .work_mode = AUDIO_CODEC_WORK_MODE_CODEC,
        .mic_input_mode = AUDIO_CODEC_MIC_INPUT_SINGLE,
    };

    if (hw_cfg == NULL || hw_cfg->i2s_dev_name == NULL || hw_cfg->i2c_dev_name == NULL ||
        hw_cfg->dma_tx_dev_name == NULL) {
        printf("wave_player_init: invalid hw cfg\r\n");
        return -1;
    }

    g_audio.i2s_dev = bflb_device_get_by_name(hw_cfg->i2s_dev_name);
    g_audio.i2c_dev = bflb_device_get_by_name(hw_cfg->i2c_dev_name);
    g_audio.dma_ch_tx = bflb_device_get_by_name(hw_cfg->dma_tx_dev_name);
    g_audio.dma_ch_rx = (hw_cfg->dma_rx_dev_name != NULL) ? bflb_device_get_by_name(hw_cfg->dma_rx_dev_name) : NULL;

    if (g_audio.i2s_dev == NULL) {
        printf("i2s device not found: %s\r\n", hw_cfg->i2s_dev_name);
        return -1;
    }
    if (g_audio.i2c_dev == NULL) {
        printf("i2c device not found: %s\r\n", hw_cfg->i2c_dev_name);
        return -1;
    }
    if (g_audio.dma_ch_tx == NULL) {
        printf("dma tx device not found: %s\r\n", hw_cfg->dma_tx_dev_name);
        return -1;
    }

#if WAVE_PLAYER_ENABLE_REC
    if (hw_cfg->dma_rx_dev_name == NULL || g_audio.dma_ch_rx == NULL) {
        printf("dma rx device not found: %s\r\n",
               (hw_cfg->dma_rx_dev_name != NULL) ? hw_cfg->dma_rx_dev_name : "(null)");
        return -1;
    }
#endif

    board_i2s_codec_gpio_init();

    bflb_i2c_init(g_audio.i2c_dev, 200000);

    if (AUDIO_CODEC_TYPE_UNKNOWN != audio_codec_scan(g_audio.i2c_dev, &g_audio.codec)) {
        if (0 != audio_codec_init(&g_audio.codec, &codec_cfg)) {
            printf("audio codec init failed\r\n");
            while (1) {}
        }
        g_audio.src_hz = codec_cfg.sample_rate_hz;
        audio_codec_set_mute(&g_audio.codec, false);
        if (0 != wave_player_apply_rec_mic_pga((uint8_t)WAVE_PLAYER_REC_DEFAULT_MIC_PGA)) {
            printf("audio codec set mic pga failed\r\n");
            while (1) {}
        }
        if (0 != audio_codec_set_volume(&g_audio.codec, 50)) {
            printf("audio codec set volume failed\r\n");
            while (1) {}
        }
        printf("audio codec ready: %s (0x%02x)\r\n", g_audio.codec.name, g_audio.codec.i2c_addr);
    } else {
        printf("audio codec scan failed\r\n");
        //while (1) {}
    }
    g_audio.sem_tx_dma_done = xSemaphoreCreateBinary();
    g_audio.sem_wave_rb_space = xSemaphoreCreateBinary();

    Ring_Buffer_Init(&g_audio.wave_rb, g_audio.wave_src_buf, sizeof(g_audio.wave_src_buf), rb_lock, rb_unlock);
    wave_player_diag_reset();

#if WAVE_PLAYER_ENABLE_REC
    g_audio.sem_rx_dma_done = xSemaphoreCreateBinary();
    if (g_audio.cap_queue == NULL) {
        g_audio.cap_queue = xQueueCreate((UBaseType_t)(WAVE_PLAYER_CAP_DMA_BLOCKS * 2U), sizeof(cap_msg_t));
    }
    if (g_audio.cap_queue == NULL) {
        printf("wave_player_init: cap queue alloc failed\r\n");
        return -1;
    }
    /* VAD callback path is heavier than plain copy; keep capture task stack sufficient. */
    (void)xTaskCreate(audio_cap_proc, (char *)"audio_cap", 1024, NULL, 2, &g_audio.cap_task);
#endif

    return 0;
}

int wave_player_feed(uint8_t *data, uint32_t len)
{
    uint32_t empty = Ring_Buffer_Get_Empty_Length(&g_audio.wave_rb);
    if (empty == 0) {
        printf("RB full\r\n");
        return 0;
    }

    if (len > empty) {
        len = empty;
    }

    return Ring_Buffer_Write(&g_audio.wave_rb, data, len);
}

uint32_t wave_player_feed_blocking(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0) {
        return 0;
    }

    uint32_t wrote_total = 0;
    while (wrote_total < len) {
        if (g_audio.paused || g_audio.stop) {
            break;
        }
        uint32_t empty = Ring_Buffer_Get_Empty_Length(&g_audio.wave_rb);
        if (empty == 0) {
            if (g_audio.sem_wave_rb_space != NULL) {
                (void)xSemaphoreTake(g_audio.sem_wave_rb_space, portMAX_DELAY);
            } else {
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            continue;
        }

        uint32_t remain = len - wrote_total;
        uint32_t chunk = (remain < empty) ? remain : empty;
        uint32_t wrote = Ring_Buffer_Write(&g_audio.wave_rb, data + wrote_total, chunk);
        wrote_total += wrote;
        if (wrote < chunk) {
            if (g_audio.paused || g_audio.stop) {
                break;
            }
            if (g_audio.sem_wave_rb_space != NULL) {
                (void)xSemaphoreTake(g_audio.sem_wave_rb_space, portMAX_DELAY);
            } else {
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        }
    }

    return wrote_total;
}

/* Start audio output */
int wave_player_start(void)
{
    if (g_audio.wave_task != NULL) {
        printf("wave_player already started\r\n");
        if (g_audio.paused) {
            g_audio.paused = false;
            audio_io_resume();
        }
        return 0;
    }

    Ring_Buffer_Reset(&g_audio.wave_rb);

    g_audio.stop = false;
    g_audio.paused = false;
    g_audio.wave_cfg.wave_rb = &g_audio.wave_rb;
    g_audio.wave_cfg.sem_wave_rb_space = g_audio.sem_wave_rb_space;
    g_audio.wave_cfg.src_sample_rate_hz = g_audio.src_hz;
    g_audio.wave_cfg.output_sample_rate_hz = (uint32_t)g_audio.out_hz;
    g_audio.wave_cfg.tx_block_bytes = g_audio.tx_block_bytes;
    g_audio.wave_cfg.src_channels = g_audio.src_channels;
    g_audio.wave_cfg.out_channels = g_audio.out_channels;
    g_audio.wave_cfg.sem_tx_dma_done = g_audio.sem_tx_dma_done;
    g_audio.wave_cfg.pcm_ping = g_audio.pcm_ping;
    g_audio.wave_cfg.pcm_pong = g_audio.pcm_pong;
    g_audio.wave_cfg.pcm_fill = pcm_fill_pcm_block;
    g_audio.wave_cfg.audio_stop = audio_io_stop;
    if (xTaskCreate(wave_player_task, (char *)"wav_play", 256, &g_audio.wave_cfg, 3, &g_audio.wave_task) != pdPASS) {
        printf("wave_player: create task failed\r\n");
        g_audio.wave_task = NULL;
        audio_io_stop();
        return -1;
    }

    return 0;
}

/* Start audio suspend */
int wave_player_suspend(void)
{
    if (g_audio.wave_task == NULL) {
        return 0;
    }
    audio_io_pause();
    g_audio.paused = true;
    xSemaphoreGive(g_audio.sem_wave_rb_space);

    return 0;
}

int wave_player_resume(void)
{
    if (g_audio.wave_task == NULL || !g_audio.paused) {
        return 0;
    }
    audio_io_resume();
    g_audio.paused = false;

    return 0;
}

/* Stop audio output */
int wave_player_stop(void)
{
    if (g_audio.wave_task == NULL) {
        if (g_audio.io_started) {
            audio_io_stop();
        }
        return 0;
    }
    /* enable audio output to wakeup player thead to exit */
    if (g_audio.paused) {
        audio_io_resume();
    }
    g_audio.stop = true;
    g_audio.paused = false;
    while (g_audio.wave_task != NULL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;
}

/* Mute/unmute audio output */
int wave_player_set_mute(bool mute)
{
    g_audio.muted = mute;
    return audio_codec_set_mute(&g_audio.codec, mute);
}

/* Set volume (0-100) */
int wave_player_set_volume(uint8_t volume)
{
    if (volume > 100) {
        volume = 100;
    }

    g_audio.volume = volume;
    int ret = audio_codec_set_volume(&g_audio.codec, volume);

    if (ret == 0) {
        printf("[AUDIO] Volume set to %d\r\n", volume);
    } else {
        printf("[AUDIO] ERROR: Failed to set volume: %d\r\n", ret);
    }

    return ret;
}

/* Get volume (0-100) */
uint8_t wave_player_get_volume(void)
{
    return g_audio.volume;
}

/* Get mute status */
bool wave_player_get_mute(void)
{
    return g_audio.muted;
}

#if WAVE_PLAYER_ENABLE_REC
int wave_player_rec_set_callback(wave_player_rec_mono_frame_cb_t cb, void *user_data)
{
    g_audio.rec_cb = cb;
    g_audio.rec_cb_user_data = user_data;
    return 0;
}

int wave_record_enable(bool enable)
{
    bool changed = (g_audio.rec_enable != enable);
    g_audio.rec_enable = enable;

    if (changed && g_audio.io_started && g_audio.tx_block_bytes > 0U && g_audio.src_hz > 0U) {
        wave_player_config(g_audio.src_hz, g_audio.tx_block_bytes, g_audio.rx_block_bytes, g_audio.src_channels);
    }

    return 0;
}

bool wave_record_is_enabled(void)
{
    return g_audio.rec_enable;
}

int wave_player_rec_get_mono(int16_t *dst, uint32_t max_samples, uint32_t *out_samples)
{
#if WAVE_PLAYER_ENABLE_REC_RB
    if (out_samples != NULL) {
        *out_samples = 0U;
    }
    if (dst == NULL || out_samples == NULL || max_samples == 0U) {
        return -1;
    }

    taskENTER_CRITICAL();
    uint32_t write_pos = g_audio.rec_write;
    bool filled = g_audio.rec_filled;
    taskEXIT_CRITICAL();

    uint32_t total = filled ? (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES : write_pos;
    if (total == 0U) {
        *out_samples = 0U;
        return 0;
    }
    if (total > max_samples) {
        total = max_samples;
    }

    taskENTER_CRITICAL();
    if (!filled) {
        memcpy(dst, g_audio.rec_mono_buf, total * sizeof(int16_t));
    } else {
        uint32_t start = write_pos;
        if (total < (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES) {
            start = (write_pos + (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES - total) % (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES;
        }
        for (uint32_t i = 0; i < total; i++) {
            uint32_t idx = start + i;
            if (idx >= (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES) {
                idx -= (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES;
            }
            dst[i] = g_audio.rec_mono_buf[idx];
        }
    }
    taskEXIT_CRITICAL();

    *out_samples = total;
    return 0;
#else
    (void)dst;
    (void)max_samples;
    if (out_samples != NULL) {
        *out_samples = 0U;
    }
    return -1;
#endif
}

int wave_player_rec_start(uint32_t duration_ms, uint32_t sample_rate_hz, uint8_t channels)
{
    if (duration_ms == 0 || sample_rate_hz == 0 || channels == 0) {
        return -1;
    }

    bool prev_enable = wave_record_is_enabled();
    (void)wave_record_enable(true);
#if WAVE_PLAYER_ENABLE_REC_RB
    g_audio.rec_write = 0;
    g_audio.rec_filled = false;
#endif
    g_audio.rec_stop = false;

#if WAVE_PLAYER_ENABLE_REC_RB
    memset(g_audio.rec_mono_buf, 0, sizeof(g_audio.rec_mono_buf));
#endif
    uint32_t tx_block_bytes = AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS * sizeof(int16_t);
    wave_player_config(sample_rate_hz, tx_block_bytes, tx_block_bytes, channels);
    wave_player_start();

    const TickType_t ticks_total = pdMS_TO_TICKS(duration_ms);
    const TickType_t tick_start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - tick_start) < ticks_total && !g_audio.rec_stop) {
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    wave_player_stop();
    g_audio.rec_stop = false;
    (void)wave_record_enable(prev_enable);

    return 0;
}

int wave_player_rec_stop(void)
{
    if (!g_audio.rec_enable) {
        return 0;
    }

    g_audio.rec_stop = true;
    return 0;
}

static int cmd_rec_start(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: rec_start <duration_ms> <sample_rate> <channels>\r\n");
        return 0;
    }

    uint32_t duration_ms = (uint32_t)strtoul(argv[1], NULL, 0);
    uint32_t sample_rate = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t channels = (uint32_t)strtoul(argv[3], NULL, 0);
    return wave_player_rec_start(duration_ms, sample_rate, (uint8_t)channels);
}

static int cmd_rec_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return wave_player_rec_stop();
}

static int cmd_rec_play(int argc, char **argv)
{
#if !WAVE_PLAYER_ENABLE_REC_RB
    (void)argc;
    (void)argv;
    printf("rec_play: disabled (WAVE_PLAYER_ENABLE_REC_RB=0)\r\n");
    return -1;
#else
    if (argc > 1) {
        printf("usage: rec_play\r\n");
        return 0;
    }

    uint32_t sample_rate = (g_audio.out_hz > 0U) ? (uint32_t)g_audio.out_hz : 16000U;
    const uint8_t channels = 1U;
    if (g_audio.out_hz == 0U) {
        printf("rec_play: invalid record sample rate, fallback to 16000\r\n");
    }

    uint32_t out_samples = 0;
    recorder_reorder_ring_s16(g_audio.rec_mono_buf, WAVE_PLAYER_REC_MAX_SAMPLES, g_audio.rec_write, g_audio.rec_filled,
                              &out_samples);
    if (out_samples == 0) {
        printf("rec_play: no data\r\n");
        return -1;
    }
    g_audio.rec_write = out_samples;
    g_audio.rec_filled = 0;
    wave_player_stop();
    wave_record_enable(0);
    wave_player_play_one_wave((const uint8_t *)g_audio.rec_mono_buf, out_samples * (uint32_t)sizeof(int16_t),
                              sample_rate, (uint8_t)channels);
    return 0;
#endif
}
#endif

static int cmd_audio_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return wave_player_start();
}

static int cmd_audio_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return wave_player_stop();
}

static int cmd_audio_vol(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: audio_vol <0-100>\r\n");
        printf("current volume: %u\r\n", wave_player_get_volume());
        return 0;
    }

    unsigned long vol = strtoul(argv[1], NULL, 0);
    if (vol > 100) {
        vol = 100;
    }
    return wave_player_set_volume((uint8_t)vol);
}

static int cmd_audio_mute(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: audio_mute <0|1>\r\n");
        printf("current mute: %u\r\n", wave_player_get_mute() ? 1U : 0U);
        return 0;
    }

    unsigned long mute = strtoul(argv[1], NULL, 0);
    return wave_player_set_mute(mute != 0U);
}

static int cmd_audio_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("audio: playing=%u, src_hz=%u, i2s_hz=%d, channels=%u, volume=%u, mute=%u\r\n",
           (g_audio.wave_task != NULL) ? 1U : 0U, g_audio.src_hz, g_audio.out_hz, g_audio.out_channels, g_audio.volume,
           g_audio.muted ? 1U : 0U);
    return 0;
}

static int cmd_audio_stack(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    wave_player_dump_stack();
    return 0;
}

static int cmd_audio_diag(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "clr") == 0) {
        wave_player_diag_reset();
        printf("audio_diag: cleared\r\n");
        return 0;
    }

    uint32_t tx_isr = g_wave_diag.tx_isr_cnt;
    uint32_t tx_take = g_wave_diag.tx_sem_take_cnt;
    uint32_t tx_fill = g_wave_diag.tx_fill_cnt;
    uint32_t tx_gap = (tx_isr >= tx_take) ? (tx_isr - tx_take) : 0U;

    printf("audio_diag: tx_isr=%lu, tx_sem_take=%lu, tx_fill=%lu, tx_gap=%lu, tx_gap_max=%lu\r\n",
           (unsigned long)tx_isr, (unsigned long)tx_take, (unsigned long)tx_fill, (unsigned long)tx_gap,
           (unsigned long)g_wave_diag.tx_isr_sem_gap_max);
    printf("audio_diag: rx_isr=%lu, rx_q_ok=%lu, rx_q_fail=%lu, rx_q_depth_max=%lu\r\n",
           (unsigned long)g_wave_diag.rx_isr_cnt, (unsigned long)g_wave_diag.rx_q_send_ok,
           (unsigned long)g_wave_diag.rx_q_send_fail, (unsigned long)g_wave_diag.rx_q_depth_max);
    printf("audio_diag: rx_msg_lag_max=%lu, rx_overrun_risk=%lu\r\n", (unsigned long)g_wave_diag.rx_msg_lag_max,
           (unsigned long)g_wave_diag.rx_msg_overrun_risk_cnt);
    return 0;
}

#if WAVE_PLAYER_ENABLE_REC
static int cmd_rec_pga(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: rec_pga <0-100>\r\n");
        return 0;
    }

    unsigned long pga = strtoul(argv[1], NULL, 0);
    if (pga > 100U) {
        pga = 100U;
    }

    int ret = wave_player_apply_rec_mic_pga((uint8_t)pga);
    if (ret == 0) {
        printf("rec_pga set to %u\r\n", (unsigned)pga);
    } else {
        printf("rec_pga set failed: %d\r\n", ret);
    }
    return ret;
}

static int cmd_rec_en(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: rec_en <0|1>\r\n");
        printf("current rec_enable: %u\r\n", wave_record_is_enabled() ? 1U : 0U);
        return 0;
    }

    unsigned long en = strtoul(argv[1], NULL, 0);
    int ret = wave_record_enable(en != 0U);
    if (ret == 0) {
        printf("rec_enable=%u\r\n", wave_record_is_enabled() ? 1U : 0U);
    }
    return ret;
}
#endif

#if WAVE_PLAYER_TEST_WAVE || WAVE_PLAYER_ENABLE_REC
static void wave_src_sink(void *arg)
{
    wave_src_task_cfg_t *cfg = (wave_src_task_cfg_t *)arg;
    if (cfg == NULL || cfg->src == NULL || cfg->src_len == 0 || cfg->rb == NULL) {
        printf("wave_src_sink: bad cfg\r\n");
        if (cfg != NULL) {
            vPortFree(cfg);
        }
        vTaskDelete(NULL);
    }

    size_t off = 0;
    while (off < cfg->src_len) {
        uint32_t empty = Ring_Buffer_Get_Empty_Length(cfg->rb);
        if (empty == 0) {
            if (cfg->sem_wave_rb_space != NULL) {
                (void)xSemaphoreTake(cfg->sem_wave_rb_space, portMAX_DELAY);
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
            if (cfg->sem_wave_rb_space != NULL) {
                (void)xSemaphoreTake(cfg->sem_wave_rb_space, portMAX_DELAY);
            } else {
                taskYIELD();
            }
        }
    }
    while (Ring_Buffer_Get_Length(cfg->rb) > 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("wave_src_sink: done feeding\r\n");
    vPortFree(cfg);
    wave_player_stop();
    g_audio.wave_src_task = NULL;
    vTaskDelete(NULL);
}
#endif

#if WAVE_PLAYER_TEST_WAVE || WAVE_PLAYER_ENABLE_REC
static void wave_player_play_one_wave(const uint8_t *src, size_t src_len, uint32_t sample_rate_hz, uint8_t src_channels)
{
    if (src == NULL || src_len == 0) {
        printf("play_one_wave: empty src\r\n");
        return;
    }

    uint32_t tx_block_bytes = AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS * sizeof(int16_t);
    wave_player_config(sample_rate_hz, tx_block_bytes, tx_block_bytes, src_channels);
    wave_player_start();

    wave_src_task_cfg_t *src_cfg = (wave_src_task_cfg_t *)pvPortMalloc(sizeof(*src_cfg));
    if (src_cfg == NULL) {
        printf("play_one_wave: no mem for src_cfg\r\n");
        return;
    }
    *src_cfg = (wave_src_task_cfg_t){
        .src = src,
        .src_len = src_len,
        .rb = &g_audio.wave_rb,
        .sem_wave_rb_space = g_audio.sem_wave_rb_space,
    };
    (void)xTaskCreate(wave_src_sink, (char *)"wave_src", 256, src_cfg, 2, &g_audio.wave_src_task);
}
#endif

#if WAVE_PLAYER_TEST_WAVE
static uint16_t wav_le16(const uint8_t *p)
{
    return (uint16_t)p[0] | (uint16_t)(p[1] << 8);
}

static uint32_t wav_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static int wave_player_play_wave_buffer(const uint8_t *buf, size_t len)
{
    if (buf == NULL || len < 12) {
        printf("play_wave: invalid buffer\r\n");
        return -1;
    }

    if (memcmp(buf, "RIFF", 4) != 0 || memcmp(buf + 8, "WAVE", 4) != 0) {
        printf("play_wave: invalid header\r\n");
        return -1;
    }

    uint16_t channels = 0;
    uint32_t sample_rate = 0;
    uint16_t bits_per_sample = 0;
    const uint8_t *data = NULL;
    uint32_t data_size = 0;

    size_t off = 12;
    while (off + 8 <= len) {
        const uint8_t *chunk_id = buf + off;
        uint32_t chunk_size = wav_le32(buf + off + 4);
        off += 8;
        if (off + chunk_size > len) {
            break;
        }

        if (memcmp(chunk_id, "fmt ", 4) == 0) {
            if (chunk_size < 16) {
                printf("play_wave: fmt chunk too small\r\n");
                return -1;
            }
            uint16_t audio_format = wav_le16(buf + off + 0);
            channels = wav_le16(buf + off + 2);
            sample_rate = wav_le32(buf + off + 4);
            bits_per_sample = wav_le16(buf + off + 14);
            if (audio_format != 1 || bits_per_sample != 16) {
                printf("play_wave: unsupported format (fmt=%u, bits=%u)\r\n", audio_format, bits_per_sample);
                return -1;
            }
        } else if (memcmp(chunk_id, "data", 4) == 0) {
            data = buf + off;
            data_size = chunk_size;
        }

        off += chunk_size + (chunk_size & 1U);
    }

    if (data == NULL || data_size == 0 || channels == 0 || sample_rate == 0) {
        printf("play_wave: missing fmt/data chunk\r\n");
        return -1;
    }

    wave_player_stop();
    wave_player_play_one_wave(data, data_size, sample_rate, (uint8_t)channels);
    return 0;
}

static int cmd_audio_play0(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return wave_player_play_wave_buffer(gwave_src_buffer0Data, gwave_src_buffer0Size);
}

static int cmd_audio_play1(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return wave_player_play_wave_buffer(gwave_src_buffer1Data, gwave_src_buffer1Size);
}
#endif

SHELL_CMD_EXPORT_ALIAS(cmd_audio_start, audio_start, "start audio player");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_stop, audio_stop, "stop audio player");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_vol, audio_vol, "set audio volume (0-100)");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_mute, audio_mute, "set audio mute (0/1)");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_info, audio_info, "show audio player status");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_stack, audio_stack, "show audio stack high watermarks");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_diag, audio_diag, "show/reset audio diagnostics: audio_diag [clr]");
#if WAVE_PLAYER_ENABLE_REC
SHELL_CMD_EXPORT_ALIAS(cmd_rec_start, rec_start, "start recording: rec_start <duration_ms> <sample_rate> <channels>");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_stop, rec_stop, "stop recording");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_play, rec_play, "play recording: rec_play");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_pga, rec_pga, "set record mic pga: rec_pga <0-100>");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_en, rec_en, "enable/disable record path: rec_en <0|1>");
#endif
#if WAVE_PLAYER_TEST_WAVE
SHELL_CMD_EXPORT_ALIAS(cmd_audio_play0, audio_play0, "play embedded wave buffer 0");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_play1, audio_play1, "play embedded wave buffer 1");
#endif
