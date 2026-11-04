#include "wave_player.h"
#include "audio_pcm.h"
#include "bflb_i2s.h"
#include "bflb_i2c.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_dma.h"
#include "bflb_audac.h"
#include "bflb_auadc.h"
#include "bflb_flash.h"
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
#include <errno.h>
#include <limits.h>

#if defined(CONFIG_BSP_AUDIO_PCM)
static bool audio_pcm_owned(void);
static bool wave_player_legacy_enter(void);
static void wave_player_legacy_leave(void);
static bool audio_pcm_keep_capture(void);
#if WAVE_PLAYER_ENABLE_REC
static bool audio_pcm_capture_prepared(void);
#endif
static void audio_pcm_player_cleanup(void);
static bool audio_pcm_player_wait_reference(void);
static void audio_pcm_player_started(void);
static void audio_pcm_player_failed(void);
static void audio_pcm_prepare_playback(int16_t *samples, size_t bytes);
#else
static bool audio_pcm_owned(void)
{
    return false;
}

static bool wave_player_legacy_enter(void)
{
    return true;
}

static void wave_player_legacy_leave(void)
{
}

static bool audio_pcm_keep_capture(void)
{
    return false;
}

#if WAVE_PLAYER_ENABLE_REC
static bool audio_pcm_capture_prepared(void)
{
    return false;
}
#endif

static void audio_pcm_player_cleanup(void)
{
}

static bool audio_pcm_player_wait_reference(void)
{
    return true;
}

static void audio_pcm_player_started(void)
{
}

static void audio_pcm_player_failed(void)
{
}

static void audio_pcm_prepare_playback(int16_t *samples, size_t bytes)
{
    (void)samples;
    (void)bytes;
}
#endif

static wave_player_playback_frame_cb_t playback_frame_cb;
static void *playback_frame_user_data;
static volatile uint32_t playback_flush_epoch;
static UBaseType_t playback_task_priority = 3;

static int wave_player_set_playback_callback_internal(wave_player_playback_frame_cb_t cb, void *user_data)
{
    playback_frame_cb = cb;
    playback_frame_user_data = user_data;
    return 0;
}

int wave_player_set_playback_callback(wave_player_playback_frame_cb_t cb, void *user_data)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_set_playback_callback_internal(cb, user_data);
    wave_player_legacy_leave();
    return result;
}

#ifdef CONFIG_BSP_AUDIO_CODEC_BL_ENABLE
#define WAVE_PLAYER_BUILTIN_CODEC_AVAILABLE 1
#else
#define WAVE_PLAYER_BUILTIN_CODEC_AVAILABLE 0
#endif

#ifndef CONFIG_WAVE_PLAYER_DIAG
#define WAVE_PLAYER_DIAG 0
#else
#define WAVE_PLAYER_DIAG CONFIG_WAVE_PLAYER_DIAG
#endif

#ifndef CONFIG_WAVE_PLAYER_CODEC_REG_CLI
#define WAVE_PLAYER_CODEC_REG_CLI 1
#else
#define WAVE_PLAYER_CODEC_REG_CLI CONFIG_WAVE_PLAYER_CODEC_REG_CLI
#endif

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
#if WAVE_PLAYER_ENABLE_REC
    bool restore_rec_enable;
    bool rec_enable_after_done;
#endif
} wave_src_task_cfg_t;

typedef struct {
    Ring_Buffer_Type *wave_rb;
    SemaphoreHandle_t sem_wave_rb_space;
    uint32_t src_sample_rate_hz;
    uint32_t output_sample_rate_hz;
    uint32_t tx_block_bytes;
    uint32_t logical_tx_block_bytes;
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
    uint32_t block_idx;
    uint32_t nbytes;
    uint32_t hz;
    uint32_t samples_per_ch;
    uint8_t channels;
    uint32_t seq;
    uint32_t epoch;
    uint64_t first_sample_index;
} cap_msg_t;

#define WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH ((WAVE_PLAYER_REC_MAX_FS_HZ * WAVE_PLAYER_REC_FRAME_MS) / 1000U)
#define WAVE_PLAYER_CAP_MAX_SAMPLES_TOTAL  (WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH * WAVE_PLAYER_REC_CHANNELS)
#if (WAVE_PLAYER_CAP_DMA_BLOCKS < 2U)
#error "WAVE_PLAYER_CAP_DMA_BLOCKS must be >= 2"
#endif

#if WAVE_PLAYER_ENABLE_REC_RB
#define WAVE_PLAYER_REC_MAX_SAMPLES (WAVE_PLAYER_REC_MAX_FS_HZ * WAVE_PLAYER_REC_SECONDS * WAVE_PLAYER_REC_CHANNELS)
#define WAVE_PLAYER_REC_DUMP_FLASH_ADDR_DEFAULT      (0x378000U)
#define WAVE_PLAYER_REC_DUMP_FLASH_MEDIA_SIZE_DEFAULT (0x71000U)
#define WAVE_PLAYER_REC_DUMP_FLASH_ERASE_ALIGN       (4096U)
#endif
#endif

#define WAVE_PLAYER_MAX_U32(a, b) (((a) > (b)) ? (a) : (b))
#define WAVE_PLAYER_DMA_LLI_SEGMENT_MAX_BYTES 4064U

#if WAVE_PLAYER_I2S_DSP_MODE
#if (WAVE_PLAYER_I2S_TX_SLOTS > 0U)
#define WAVE_PLAYER_TX_SLOT_MAX WAVE_PLAYER_I2S_TX_SLOTS
#elif WAVE_PLAYER_ENABLE_REC
#define WAVE_PLAYER_TX_SLOT_MAX WAVE_PLAYER_MAX_U32(WAVE_PLAYER_OUT_CHANNELS, WAVE_PLAYER_REC_CHANNELS)
#else
#define WAVE_PLAYER_TX_SLOT_MAX WAVE_PLAYER_OUT_CHANNELS
#endif
#if WAVE_PLAYER_I2S_DSP_LR_MERGE
#define WAVE_PLAYER_TX_SLOT_WORD_BYTES_MAX ((uint32_t)sizeof(int16_t))
#else
#define WAVE_PLAYER_TX_SLOT_WORD_BYTES_MAX ((uint32_t)sizeof(uint32_t))
#endif
#else
#define WAVE_PLAYER_TX_SLOT_MAX WAVE_PLAYER_OUT_CHANNELS
#define WAVE_PLAYER_TX_SLOT_WORD_BYTES_MAX ((uint32_t)sizeof(int16_t))
#endif

#define WAVE_PLAYER_TX_PCM_BYTES_MAX \
    ((uint32_t)WAVE_PLAYER_OUT_SAMPLES * (uint32_t)WAVE_PLAYER_TX_SLOT_MAX * WAVE_PLAYER_TX_SLOT_WORD_BYTES_MAX)
#define WAVE_PLAYER_TX_PCM_SAMPLES_MAX \
    ((WAVE_PLAYER_TX_PCM_BYTES_MAX + (uint32_t)sizeof(int16_t) - 1U) / (uint32_t)sizeof(int16_t))
#define WAVE_PLAYER_TX_LLI_COUNT \
    (2U * ((WAVE_PLAYER_TX_PCM_BYTES_MAX + WAVE_PLAYER_DMA_LLI_SEGMENT_MAX_BYTES - 1U) / \
           WAVE_PLAYER_DMA_LLI_SEGMENT_MAX_BYTES))

typedef struct {
    struct bflb_device_s *i2s_dev;
    struct bflb_device_s *i2c_dev;
    struct bflb_device_s *audac_dev;
    struct bflb_device_s *auadc_dev;
    struct bflb_device_s *dma_ch_tx;
    struct bflb_device_s *dma_ch_rx;
    struct bflb_dma_channel_lli_pool_s tx_llipool[WAVE_PLAYER_TX_LLI_COUNT];
    audio_codec_dev_t play_codec;
    audio_codec_dev_t cap_codec;
    wave_player_backend_t playback_backend;
    wave_player_backend_t capture_backend;
    uint32_t src_hz;
    uint32_t out_hz;
    uint32_t cap_hz;
    uint8_t src_channels;
    uint8_t out_channels;
    uint8_t cap_channels;
    uint32_t tx_block_bytes;
    uint32_t logical_tx_block_bytes;
    uint32_t rx_block_bytes;
    uint8_t tx_slot_count;
    uint8_t tx_slot_word_bytes;
    uint8_t volume;
    bool muted;
    bool io_started;
    SemaphoreHandle_t sem_tx_dma_done;
    SemaphoreHandle_t sem_wave_rb_space;
    Ring_Buffer_Type wave_rb;
    uint8_t wave_src_buf[WAVE_PLAYER_OUT_SAMPLES * WAVE_PLAYER_OUT_CHANNELS * 8 * sizeof(int16_t)];
    volatile uint32_t tx_dma_completions;
    uint64_t tx_total_samples;
    uint32_t tx_lli_count;
    uint32_t tx_period_samples;
    volatile bool stop;
    volatile bool paused;
    int16_t pcm_ping[WAVE_PLAYER_TX_PCM_SAMPLES_MAX] __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
    int16_t pcm_pong[WAVE_PLAYER_TX_PCM_SAMPLES_MAX] __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
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
    volatile uint32_t cap_isr_block_idx;
    volatile uint32_t cap_block_bytes;
    volatile uint32_t cap_samples_per_ch;
    volatile uint32_t cap_isr_seq;
    volatile uint32_t cap_epoch;
    uint64_t cap_total_samples;
#if WAVE_PLAYER_ENABLE_REC_RB
    int16_t rec_mono_buf[WAVE_PLAYER_REC_MAX_SAMPLES] __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));
    volatile uint32_t rec_write;
    volatile bool rec_filled;
#endif
    volatile bool rec_enable;
    volatile bool rec_path_enabled;
    volatile bool rec_stop;
    TaskHandle_t cap_task;
    wave_player_capture_layout_t capture_layout;
    wave_player_rec_cb_t rec_cb;
    void *rec_cb_user_data;
#endif
} wave_player_ctx_t;

#if WAVE_PLAYER_DIAG
typedef struct {
    volatile uint32_t tx_isr_cnt;
    volatile uint32_t tx_sem_take_cnt;
    volatile uint32_t tx_fill_cnt;
    volatile uint32_t tx_isr_sem_gap_max;
    volatile bool tx_source_data_seen;
    volatile uint32_t tx_refill_miss_cnt;
    volatile uint32_t tx_refill_isr_delta_max;
    volatile uint32_t tx_rb_under_cnt;
    volatile uint32_t tx_rb_under_bytes;
    volatile uint32_t tx_resample_pad_cnt;
    volatile uint32_t tx_resample_pad_bytes;
    volatile uint32_t tx_hw_fifo_under_cnt;
    volatile uint32_t tx_hw_fifo_over_cnt;
    volatile uint32_t tx_hw_fault_last;
    volatile uint32_t rx_isr_cnt;
    volatile uint32_t rx_q_send_ok;
    volatile uint32_t rx_q_send_fail;
    volatile uint32_t rx_q_depth_max;
    volatile uint32_t rx_msg_lag_max;
    volatile uint32_t rx_msg_overrun_risk_cnt;
} wave_player_diag_t;
#endif

static wave_player_ctx_t g_audio = {
    .i2s_dev = NULL,
    .i2c_dev = NULL,
    .audac_dev = NULL,
    .auadc_dev = NULL,
    .dma_ch_tx = NULL,
    .dma_ch_rx = NULL,
    .src_hz = 0,
    .out_hz = 0,
    .cap_hz = 0,
    .src_channels = 0,
    .out_channels = 0,
    .cap_channels = 0,
    .tx_block_bytes = 0,
    .logical_tx_block_bytes = 0,
    .rx_block_bytes = 0,
    .tx_slot_count = 0,
    .tx_slot_word_bytes = 0,
    .playback_backend = WAVE_PLAYER_BACKEND_AUTO,
    .capture_backend = WAVE_PLAYER_BACKEND_AUTO,
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

#if defined(CONFIG_BSP_AUDIO_PCM)
struct audio_pcm_session {
    audio_pcm_direction_t direction;
    audio_pcm_config_t config;
    audio_pcm_frame_cb_t capture_callback;
    audio_pcm_frame_cb_t playback_callback;
    void *capture_context;
    void *playback_context;
    uint32_t generation;
    uint32_t callback_users;
    uint32_t capture_callback_users;
    uint32_t playback_callback_users;
    uint32_t io_users;
    uint32_t open_capture_sequence;
    uint32_t capture_epoch;
    uint32_t previous_capture_sequence;
    uint64_t capture_sample_index;
    bool used;
    bool closing;
    bool aborted;
    bool capture_seen;
};

static struct {
    SemaphoreHandle_t lock;
    wave_player_hw_cfg_t hardware;
    char hardware_names[6][32];
    struct audio_pcm_session sessions[2];
    uint32_t generation;
    unsigned legacy_operations;
    unsigned control_operations;
    bool initialized;
    bool claimed;
    bool prepare_capture;
    bool requested_mute;
    uint8_t requested_volume;
    uint8_t requested_record_volume;
    uint8_t requested_record_pga;
    bool record_volume_set;
    bool record_pga_set;
    volatile int player_start_result;
    uint64_t playback_next_sample;
    uint32_t playback_sequence;
    bool playback_reference_valid;
    bool playback_reference_silent;
    bool playback_reference_gap;
    uint32_t engine_epoch;
    bool tx_resume_pending;
} g_pcm;

static bool audio_pcm_owned(void)
{
    bool owned;
    taskENTER_CRITICAL();
    owned = g_pcm.claimed;
    taskEXIT_CRITICAL();
    return owned;
}

/* Reserve legacy operations before touching the shared engine. The first PCM
 * session claims it under the same critical section, so a checked-but-not-yet
 * executed codec write cannot cross the handoff. Nested legacy calls share the
 * count; no lock is held across codec IO or a blocking player operation. */
static bool wave_player_legacy_enter(void)
{
    taskENTER_CRITICAL();
    bool available = !g_pcm.claimed && !g_pcm.control_operations;
    if (available) {
        g_pcm.legacy_operations++;
    }
    taskEXIT_CRITICAL();
    return available;
}

static void wave_player_legacy_leave(void)
{
    taskENTER_CRITICAL();
    g_pcm.legacy_operations--;
    taskEXIT_CRITICAL();
}

#if WAVE_PLAYER_ENABLE_REC
static void audio_pcm_capture_dispatch(const wave_player_rec_frame_t *frame, void *context);
#endif
static void audio_pcm_playback_dispatch(const int16_t *samples, uint32_t count, uint32_t rate, uint8_t channels,
                                        void *context);
#endif

#if WAVE_PLAYER_DIAG
static wave_player_diag_t g_wave_diag = { 0 };
#endif

#define WAVE_PLAYER_TX_DONE_NOTIFY_BIT (1UL << 0)

#if WAVE_PLAYER_TEST_WAVE || (WAVE_PLAYER_ENABLE_REC && WAVE_PLAYER_ENABLE_REC_RB)
static int wave_player_play_one_wave(const uint8_t *src, size_t src_len, uint32_t sample_rate_hz,
                                     uint8_t src_channels, bool restore_rec_enable, bool rec_enable_after_done);
#endif

#if WAVE_PLAYER_DIAG
static void wave_player_diag_reset(void)
{
    memset((void *)&g_wave_diag, 0, sizeof(g_wave_diag));
}

static bool wave_player_diag_should_log(uint32_t cnt)
{
    if (cnt <= 4U) {
        return true;
    }

    return (cnt & (cnt - 1U)) == 0U;
}

static void wave_player_diag_note_source_underrun(bool resample_path, uint32_t need_bytes, uint32_t got_bytes,
                                                  uint32_t src_hz, uint8_t src_channels, uint32_t out_hz,
                                                  uint8_t out_channels)
{
    uint32_t short_bytes;
    uint32_t cnt;

    if (!g_wave_diag.tx_source_data_seen) {
        return;
    }

    if (need_bytes <= got_bytes) {
        return;
    }

    short_bytes = need_bytes - got_bytes;
    if (resample_path) {
        g_wave_diag.tx_resample_pad_cnt++;
        g_wave_diag.tx_resample_pad_bytes += short_bytes;
        cnt = g_wave_diag.tx_resample_pad_cnt;
        if (wave_player_diag_should_log(cnt)) {
            printf("[AUDIO][XRUN] resample playback source underrun #%lu: short=%lu/%lu bytes, src=%luHz/%uch, out=%luHz/%uch\r\n",
                   (unsigned long)cnt, (unsigned long)short_bytes, (unsigned long)need_bytes, (unsigned long)src_hz,
                   (unsigned)src_channels, (unsigned long)out_hz, (unsigned)out_channels);
        }
        return;
    }

    g_wave_diag.tx_rb_under_cnt++;
    g_wave_diag.tx_rb_under_bytes += short_bytes;
    cnt = g_wave_diag.tx_rb_under_cnt;
    if (wave_player_diag_should_log(cnt)) {
        printf("[AUDIO][XRUN] direct playback source underrun #%lu: short=%lu/%lu bytes, src=%luHz/%uch, out=%luHz/%uch\r\n",
               (unsigned long)cnt, (unsigned long)short_bytes, (unsigned long)need_bytes, (unsigned long)src_hz,
               (unsigned)src_channels, (unsigned long)out_hz, (unsigned)out_channels);
    }
}

static void wave_player_diag_note_refill_miss(uint32_t isr_delta, bool ping_buf, uint32_t out_hz,
                                              uint8_t out_channels, uint32_t tx_block_bytes)
{
    uint32_t cnt;
    uint32_t block_us = 0U;

    g_wave_diag.tx_refill_miss_cnt++;
    if (isr_delta > g_wave_diag.tx_refill_isr_delta_max) {
        g_wave_diag.tx_refill_isr_delta_max = isr_delta;
    }

    cnt = g_wave_diag.tx_refill_miss_cnt;
    if (!wave_player_diag_should_log(cnt)) {
        return;
    }

    if (out_hz != 0U && out_channels != 0U) {
        uint32_t frame_bytes = (uint32_t)out_channels * (uint32_t)sizeof(int16_t);
        if (frame_bytes != 0U) {
            uint32_t samples_per_ch = tx_block_bytes / frame_bytes;
            block_us = (uint32_t)(((uint64_t)samples_per_ch * 1000000ULL) / (uint64_t)out_hz);
        }
    }

    printf("[AUDIO][XRUN] playback buffer refill miss #%lu: buf=%s, isr_delta=%lu, tx_block=%lu, block_us=%lu, stale dma block replay likely\r\n",
           (unsigned long)cnt, ping_buf ? "ping" : "pong", (unsigned long)isr_delta, (unsigned long)tx_block_bytes,
           (unsigned long)block_us);
}

static void wave_player_diag_poll_playback_hw_xrun(void)
{
    const uint32_t fault_mask = AUDAC_INTSTS_FIFO_UNDER | AUDAC_INTSTS_FIFO_OVER;
    uint32_t status;
    uint32_t new_fault;
    uint32_t cnt;

    if (g_audio.playback_backend != WAVE_PLAYER_BACKEND_BL_CODEC || g_audio.audac_dev == NULL || !g_audio.io_started) {
        g_wave_diag.tx_hw_fault_last = 0U;
        return;
    }

    status = ((uint32_t)bflb_audac_get_intstatus(g_audio.audac_dev)) & fault_mask;
    new_fault = status & ~g_wave_diag.tx_hw_fault_last;

    if ((new_fault & AUDAC_INTSTS_FIFO_UNDER) != 0U) {
        g_wave_diag.tx_hw_fifo_under_cnt++;
        cnt = g_wave_diag.tx_hw_fifo_under_cnt;
        if (wave_player_diag_should_log(cnt)) {
            printf("[AUDIO][XRUN] builtin audac fifo underrun #%lu: play_hz=%lu, out_ch=%u, tx_block=%lu\r\n",
                   (unsigned long)cnt, (unsigned long)g_audio.out_hz, (unsigned)g_audio.out_channels,
                   (unsigned long)g_audio.tx_block_bytes);
        }
    }

    if ((new_fault & AUDAC_INTSTS_FIFO_OVER) != 0U) {
        g_wave_diag.tx_hw_fifo_over_cnt++;
        cnt = g_wave_diag.tx_hw_fifo_over_cnt;
        if (wave_player_diag_should_log(cnt)) {
            printf("[AUDIO][XRUN] builtin audac fifo overrun #%lu: play_hz=%lu, out_ch=%u, tx_block=%lu\r\n",
                   (unsigned long)cnt, (unsigned long)g_audio.out_hz, (unsigned)g_audio.out_channels,
                   (unsigned long)g_audio.tx_block_bytes);
        }
    }

    g_wave_diag.tx_hw_fault_last = status;
}
#define WAVE_PLAYER_DIAG_TX_ISR()                  \
    do {                                           \
        g_wave_diag.tx_isr_cnt++;                  \
        {                                          \
            uint32_t gap = g_wave_diag.tx_isr_cnt - g_wave_diag.tx_sem_take_cnt; \
            if (gap > g_wave_diag.tx_isr_sem_gap_max) {                           \
                g_wave_diag.tx_isr_sem_gap_max = gap;                             \
            }                                                                     \
        }                                          \
    } while (0)
#define WAVE_PLAYER_DIAG_RX_ISR() do { g_wave_diag.rx_isr_cnt++; } while (0)
#define WAVE_PLAYER_DIAG_RX_Q_OK(depth)            \
    do {                                           \
        g_wave_diag.rx_q_send_ok++;                \
        if ((uint32_t)(depth) > g_wave_diag.rx_q_depth_max) { \
            g_wave_diag.rx_q_depth_max = (uint32_t)(depth);   \
        }                                          \
    } while (0)
#define WAVE_PLAYER_DIAG_RX_Q_FAIL() do { g_wave_diag.rx_q_send_fail++; } while (0)
#define WAVE_PLAYER_DIAG_SET_HW_FAULT_LAST(status) do { g_wave_diag.tx_hw_fault_last = (status); } while (0)
#define WAVE_PLAYER_DIAG_CLEAR_HW_FAULT_LAST() do { g_wave_diag.tx_hw_fault_last = 0U; } while (0)
#define WAVE_PLAYER_DIAG_RESET_TX_SOURCE_SEEN() do { g_wave_diag.tx_source_data_seen = false; } while (0)
#define WAVE_PLAYER_DIAG_NOTE_TX_SOURCE_WRITE(wrote) do { if ((wrote) > 0U) { g_wave_diag.tx_source_data_seen = true; } } while (0)
#define WAVE_PLAYER_DIAG_TX_SEM_TAKE() do { g_wave_diag.tx_sem_take_cnt++; } while (0)
#define WAVE_PLAYER_DIAG_TX_FILL() do { g_wave_diag.tx_fill_cnt++; } while (0)
#define WAVE_PLAYER_DIAG_GET_TX_ISR() (g_audio.tx_dma_completions)
#define WAVE_PLAYER_DIAG_NOTE_RX_LAG(lag)          \
    do {                                           \
        if ((lag) > g_wave_diag.rx_msg_lag_max) {  \
            g_wave_diag.rx_msg_lag_max = (lag);    \
        }                                          \
        if ((lag) >= (uint32_t)WAVE_PLAYER_CAP_DMA_BLOCKS) { \
            g_wave_diag.rx_msg_overrun_risk_cnt++; \
        }                                          \
    } while (0)
#else
static inline void wave_player_diag_reset(void) {}

static inline void wave_player_diag_note_source_underrun(bool resample_path, uint32_t need_bytes, uint32_t got_bytes,
                                                         uint32_t src_hz, uint8_t src_channels, uint32_t out_hz,
                                                         uint8_t out_channels)
{
    (void)resample_path;
    (void)need_bytes;
    (void)got_bytes;
    (void)src_hz;
    (void)src_channels;
    (void)out_hz;
    (void)out_channels;
}

static inline void wave_player_diag_note_refill_miss(uint32_t isr_delta, bool ping_buf, uint32_t out_hz,
                                                     uint8_t out_channels, uint32_t tx_block_bytes)
{
    (void)isr_delta;
    (void)ping_buf;
    (void)out_hz;
    (void)out_channels;
    (void)tx_block_bytes;
}

static inline void wave_player_diag_poll_playback_hw_xrun(void) {}

#define WAVE_PLAYER_DIAG_TX_ISR() do {} while (0)
#define WAVE_PLAYER_DIAG_RX_ISR() do {} while (0)
#define WAVE_PLAYER_DIAG_RX_Q_OK(depth) do { (void)(depth); } while (0)
#define WAVE_PLAYER_DIAG_RX_Q_FAIL() do {} while (0)
#define WAVE_PLAYER_DIAG_SET_HW_FAULT_LAST(status) do {} while (0)
#define WAVE_PLAYER_DIAG_CLEAR_HW_FAULT_LAST() do {} while (0)
#define WAVE_PLAYER_DIAG_RESET_TX_SOURCE_SEEN() do {} while (0)
#define WAVE_PLAYER_DIAG_NOTE_TX_SOURCE_WRITE(wrote) do { (void)(wrote); } while (0)
#define WAVE_PLAYER_DIAG_TX_SEM_TAKE() do {} while (0)
#define WAVE_PLAYER_DIAG_TX_FILL() do {} while (0)
#define WAVE_PLAYER_DIAG_GET_TX_ISR() (0U)
#define WAVE_PLAYER_DIAG_NOTE_RX_LAG(lag) do { (void)(lag); } while (0)
#endif

static void sem_drain(SemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return;
    }
    while (xSemaphoreTake(sem, 0) == pdTRUE) {}
}

static inline uint8_t audio_channel_mode_from_count(uint8_t channels)
{
#if WAVE_PLAYER_I2S_DSP_MODE
    switch (channels) {
        case 1U:
            return I2S_CHANNEL_MODE_NUM_1;
        case 3U:
            return I2S_CHANNEL_MODE_NUM_3;
        case 4U:
            return I2S_CHANNEL_MODE_NUM_4;
        case 6U:
            return I2S_CHANNEL_MODE_NUM_6;
        case 2U:
        default:
            return I2S_CHANNEL_MODE_NUM_2;
    }
#else
    return (channels <= 1U) ? I2S_CHANNEL_MODE_NUM_1 : I2S_CHANNEL_MODE_NUM_2;
#endif
}

static inline uint8_t audio_channel_count_clamp(uint8_t channels)
{
    if (channels == 0U) {
        return 1U;
    }
#if WAVE_PLAYER_I2S_DSP_MODE
    if (channels == 3U || channels == 4U || channels == 6U) {
        return channels;
    }
#endif
    if (channels > 2U) {
        return 2U;
    }
    return channels;
}

static bool audio_sample_rate_is_44k1_family(uint32_t sample_rate_hz)
{
    switch (sample_rate_hz) {
        case 11025U:
        case 22050U:
        case 44100U:
            return true;
        default:
            return false;
    }
}

static bool audio_sample_rate_share_pll_family(uint32_t a_hz, uint32_t b_hz)
{
    return audio_sample_rate_is_44k1_family(a_hz) == audio_sample_rate_is_44k1_family(b_hz);
}

static uint8_t wave_player_default_playback_channels(void)
{
    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC) {
        return 1U;
    }

    return audio_channel_count_clamp((uint8_t)WAVE_PLAYER_OUT_CHANNELS);
}

static uint8_t wave_player_default_capture_channels(void)
{
    if (g_audio.capture_backend == WAVE_PLAYER_BACKEND_BL_CODEC) {
        return 1U;
    }

#if WAVE_PLAYER_ENABLE_REC
    return audio_channel_count_clamp((uint8_t)WAVE_PLAYER_REC_CHANNELS);
#else
    return 2U;
#endif
}

static uint8_t wave_player_i2s_tx_slot_count(uint8_t i2s_channels, uint8_t out_channels,
                                            audio_codec_type_t codec_type)
{
#if WAVE_PLAYER_I2S_DSP_MODE
    /* The combined JY6316 driver programs a four-slot ADC/reference frame.
     * Keep that physical bus even without capture. Merely enabling its
     * driver must not change another codec detected on the same board. */
    if (codec_type == AUDIO_CODEC_TYPE_JY6316) {
#if (WAVE_PLAYER_I2S_TX_SLOTS > 0U) && (WAVE_PLAYER_I2S_TX_SLOTS != 4U)
        return 0U;
#endif
        return (i2s_channels <= 4U && out_channels <= 4U) ? 4U : 0U;
    }
#if (WAVE_PLAYER_I2S_TX_SLOTS > 0U)
    uint8_t slots = audio_channel_count_clamp((uint8_t)WAVE_PLAYER_I2S_TX_SLOTS);
    (void)i2s_channels;
#else
    uint8_t slots = audio_channel_count_clamp(i2s_channels);
#endif
    if (slots < out_channels) {
        slots = out_channels;
    }
    return slots;
#else
    (void)i2s_channels;
    (void)codec_type;
    return out_channels;
#endif
}

static uint8_t wave_player_i2s_tx_slot_word_bytes(void)
{
#if WAVE_PLAYER_I2S_DSP_MODE
#if WAVE_PLAYER_I2S_DSP_LR_MERGE
    return (uint8_t)sizeof(int16_t);
#else
    return (uint8_t)sizeof(uint32_t);
#endif
#else
    return (uint8_t)sizeof(int16_t);
#endif
}

static void audio_fill_i2s_cfg(struct bflb_i2s_config_s *cfg, uint32_t samplerate_hz, uint8_t channels)
{
    if (cfg == NULL) {
        return;
    }
    channels = audio_channel_count_clamp(channels);

#if WAVE_PLAYER_I2S_DSP_MODE
    *cfg = (struct bflb_i2s_config_s){
        .bclk_freq_hz = samplerate_hz * 16U * (uint32_t)channels,
        .role = I2S_ROLE_MASTER,
        .format_mode = I2S_MODE_DSP_SHORT_FRAME_SYNC,
        .channel_mode = audio_channel_mode_from_count(channels),
        .frame_width = I2S_SLOT_WIDTH_16,
        .data_width = I2S_SLOT_WIDTH_16,
        .fs_offset_cycle = 1,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };
#else
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
#endif
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

static void audio_fill_dma_bl_tx_cfg(struct bflb_dma_channel_config_s *cfg)
{
    if (cfg == NULL) {
        return;
    }
    *cfg = (struct bflb_dma_channel_config_s){
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_AUDAC_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };
}

#if WAVE_PLAYER_ENABLE_REC
static void audio_fill_dma_bl_rx_cfg(struct bflb_dma_channel_config_s *cfg)
{
    if (cfg == NULL) {
        return;
    }
    *cfg = (struct bflb_dma_channel_config_s){
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_AUADC_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };
}
#endif

static const char *wave_player_backend_name(wave_player_backend_t backend)
{
    switch (backend) {
        case WAVE_PLAYER_BACKEND_I2S_CODEC:
            return "i2s";
        case WAVE_PLAYER_BACKEND_BL_CODEC:
            return "bl";
        default:
            return "auto";
    }
}

#if WAVE_PLAYER_CODEC_REG_CLI
static int wave_player_codec_i2c_write_reg8(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val)
{
    struct bflb_i2c_msg_s msgs[2];

    if (i2c == NULL) {
        return -1;
    }

    msgs[0].addr = addr;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg;
    msgs[0].length = 1;

    msgs[1].addr = addr;
    msgs[1].flags = 0;
    msgs[1].buffer = &val;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c, msgs, 2);
}

static int wave_player_codec_i2c_read_reg8(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *out_val)
{
    struct bflb_i2c_msg_s msgs[2];

    if (i2c == NULL || out_val == NULL) {
        return -1;
    }

    msgs[0].addr = addr;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg;
    msgs[0].length = 1;

    msgs[1].addr = addr;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = out_val;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c, msgs, 2);
}

static audio_codec_dev_t *wave_player_codec_cli_get_codec(bool capture)
{
    return capture ? &g_audio.cap_codec : &g_audio.play_codec;
}

static struct bflb_device_s *wave_player_codec_cli_get_builtin_dev(bool capture)
{
    return capture ? g_audio.auadc_dev : g_audio.audac_dev;
}

static const char *wave_player_codec_cli_role_name(bool capture)
{
    return capture ? "cap" : "play";
}

static int wave_player_codec_cli_parse_role(const char *arg, bool *capture)
{
    if (arg == NULL || capture == NULL) {
        return -1;
    }

    if (strcmp(arg, "play") == 0 || strcmp(arg, "playback") == 0 || strcmp(arg, "tx") == 0) {
        *capture = false;
        return 0;
    }

    if (strcmp(arg, "cap") == 0 || strcmp(arg, "capture") == 0 || strcmp(arg, "rec") == 0 || strcmp(arg, "rx") == 0) {
        *capture = true;
        return 0;
    }

    return -1;
}

static int wave_player_codec_cli_validate_width(uint8_t width)
{
    if (width == 8U || width == 16U || width == 32U) {
        return 0;
    }

    return -1;
}

static int wave_player_codec_cli_mmio_read(uint32_t addr, uint8_t width, uint32_t *value)
{
    if (value == NULL) {
        return -1;
    }

    if (wave_player_codec_cli_validate_width(width) != 0) {
        return -1;
    }

    if ((width == 16U && (addr & 0x1U) != 0U) || (width == 32U && (addr & 0x3U) != 0U)) {
        return -1;
    }

    if (width == 8U) {
        *value = (uint32_t)getreg8(addr);
    } else if (width == 16U) {
        *value = (uint32_t)getreg16(addr);
    } else {
        *value = getreg32(addr);
    }

    return 0;
}

static int wave_player_codec_cli_mmio_write(uint32_t addr, uint8_t width, uint32_t value)
{
    if (wave_player_codec_cli_validate_width(width) != 0) {
        return -1;
    }

    if ((width == 16U && (addr & 0x1U) != 0U) || (width == 32U && (addr & 0x3U) != 0U)) {
        return -1;
    }

    if (width == 8U) {
        putreg8((uint8_t)value, addr);
    } else if (width == 16U) {
        putreg16((uint16_t)value, addr);
    } else {
        putreg32(value, addr);
    }

    return 0;
}

static int wave_player_codec_cli_read_internal(bool capture, uint32_t reg, uint8_t width, uint32_t *value)
{
    audio_codec_dev_t *codec = wave_player_codec_cli_get_codec(capture);

    if (codec == NULL || codec->drv == NULL) {
        return -1;
    }

    if (codec->type == AUDIO_CODEC_TYPE_BL) {
        struct bflb_device_s *dev = wave_player_codec_cli_get_builtin_dev(capture);

        if (dev == NULL) {
            return -2;
        }

        return wave_player_codec_cli_mmio_read(dev->reg_base + reg, width, value);
    }

    if (width != 8U || reg > 0xFFU || codec->i2c == NULL || value == NULL) {
        return -1;
    }

    {
        uint8_t regv = 0U;
        int ret = wave_player_codec_i2c_read_reg8(codec->i2c, codec->i2c_addr, (uint8_t)reg, &regv);
        if (ret != 0) {
            return ret;
        }
        *value = regv;
    }

    return 0;
}

/* A register read also reconfigures the shared I2C controller. */
static int wave_player_codec_cli_read(bool capture, uint32_t reg, uint8_t width, uint32_t *value)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_codec_cli_read_internal(capture, reg, width, value);
    wave_player_legacy_leave();
    return result;
}

static int wave_player_codec_cli_write_internal(bool capture, uint32_t reg, uint32_t value, uint8_t width)
{
    audio_codec_dev_t *codec = wave_player_codec_cli_get_codec(capture);

    if (codec == NULL || codec->drv == NULL) {
        return -1;
    }

    if (codec->type == AUDIO_CODEC_TYPE_BL) {
        struct bflb_device_s *dev = wave_player_codec_cli_get_builtin_dev(capture);

        if (dev == NULL) {
            return -2;
        }

        return wave_player_codec_cli_mmio_write(dev->reg_base + reg, width, value);
    }

    if (width != 8U || reg > 0xFFU || value > 0xFFU || codec->i2c == NULL) {
        return -1;
    }

    return wave_player_codec_i2c_write_reg8(codec->i2c, codec->i2c_addr, (uint8_t)reg, (uint8_t)value);
}

static int wave_player_codec_cli_write(bool capture, uint32_t reg, uint32_t value, uint8_t width)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_codec_cli_write_internal(capture, reg, value, width);
    wave_player_legacy_leave();
    return result;
}

static void wave_player_codec_cli_usage(void)
{
    printf("usage: codec_reg r <play|cap> <reg> [width]\r\n");
    printf("       codec_reg w <play|cap> <reg> <value> [width]\r\n");
    printf("note: ext codec width fixed to 8; builtin codec width supports 8/16/32 and defaults to 32\r\n");
}
#endif

static void dma_ch_tx_isr(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)arg;
    g_audio.tx_dma_completions++;
#if defined(CONFIG_BSP_AUDIO_PCM)
    g_audio.tx_total_samples += g_audio.tx_period_samples;
#endif
    WAVE_PLAYER_DIAG_TX_ISR();
    if (g_audio.wave_task != NULL) {
        (void)xTaskNotifyFromISR(g_audio.wave_task, WAVE_PLAYER_TX_DONE_NOTIFY_BIT, eSetBits, &xHigherPriorityTaskWoken);
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

#if WAVE_PLAYER_ENABLE_REC
static bool wave_capture_enable_requested(void)
{
    return g_audio.rec_enable;
}

static void dma_ch_rx_isr(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)arg;
    WAVE_PLAYER_DIAG_RX_ISR();

    if (g_audio.cap_queue != NULL && g_audio.cap_block_bytes != 0 && g_audio.cap_samples_per_ch != 0 &&
        g_audio.cap_hz > 0) {
        uint32_t seq = g_audio.cap_isr_seq + 1U;
        g_audio.cap_isr_seq = seq;
        uint32_t blk = g_audio.cap_isr_block_idx;
        cap_msg_t msg = {
            .block_idx = blk,
            .nbytes = g_audio.cap_block_bytes,
            .hz = (uint32_t)g_audio.cap_hz,
            .samples_per_ch = g_audio.cap_samples_per_ch,
            .channels = g_audio.cap_channels,
            .seq = seq,
            .epoch = g_audio.cap_epoch,
            .first_sample_index = g_audio.cap_total_samples,
        };
        g_audio.cap_total_samples += g_audio.cap_samples_per_ch;
        if (!wave_capture_enable_requested()) {
            /* The physical full-duplex clock may be retained by playback.
             * Keep ring position and sequence accurate without publishing PCM. */
        } else if (xQueueSendFromISR(g_audio.cap_queue, &msg, &xHigherPriorityTaskWoken) == pdTRUE) {
            {
                UBaseType_t depth = uxQueueMessagesWaitingFromISR(g_audio.cap_queue);
                WAVE_PLAYER_DIAG_RX_Q_OK(depth);
            }
        } else {
            WAVE_PLAYER_DIAG_RX_Q_FAIL();
        }
        blk++;
        if (blk >= WAVE_PLAYER_CAP_DMA_BLOCKS) {
            blk = 0U;
        }
        g_audio.cap_isr_block_idx = blk;
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif

static void audio_i2s_update_data_enable(bool tx_enable, bool rx_enable)
{
    if (g_audio.i2s_dev == NULL) {
        return;
    }

    uint32_t disable_control = 0U;
    uint32_t enable_control = 0U;

    if (!tx_enable) {
        disable_control |= I2S_ARG_CMD_TX;
    }
    if (!rx_enable) {
        disable_control |= I2S_ARG_CMD_RX;
    }
    if (!tx_enable && !rx_enable) {
        disable_control |= I2S_ARG_CMD_MASTER;
    }

    if (disable_control) {
        bflb_i2s_feature_control(g_audio.i2s_dev, I2S_CMD_DISABLE_CONTROL, disable_control);
    }

    if (tx_enable || rx_enable) {
        enable_control |= I2S_ARG_CMD_MASTER;
        if (tx_enable) {
            enable_control |= I2S_ARG_CMD_TX;
        }
        if (rx_enable) {
            enable_control |= I2S_ARG_CMD_RX;
        }

        bflb_i2s_feature_control(g_audio.i2s_dev, I2S_CMD_ENABLE_CONTROL, enable_control);
    }
}

static void audio_transport_stop(void)
{
    if ((g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC ||
         g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC) &&
        g_audio.i2s_dev != NULL) {
        audio_i2s_update_data_enable(false, false);
        bflb_i2s_link_txdma(g_audio.i2s_dev, false);
        bflb_i2s_link_rxdma(g_audio.i2s_dev, false);
    }

    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC && g_audio.audac_dev != NULL) {
        (void)bflb_audac_feature_control(g_audio.audac_dev, AUDAC_CMD_PLAY_STOP, 0);
        (void)bflb_audac_link_rxdma(g_audio.audac_dev, false);
    }

#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.capture_backend == WAVE_PLAYER_BACKEND_BL_CODEC && g_audio.auadc_dev != NULL) {
        (void)bflb_auadc_feature_control(g_audio.auadc_dev, AUADC_CMD_RECORD_STOP, 0);
        (void)bflb_auadc_link_rxdma(g_audio.auadc_dev, false);
    }
#endif

    if (g_audio.dma_ch_tx != NULL) {
        bflb_dma_channel_stop(g_audio.dma_ch_tx);
    }
    if (g_audio.dma_ch_rx != NULL) {
        bflb_dma_channel_stop(g_audio.dma_ch_rx);
    }
    WAVE_PLAYER_DIAG_CLEAR_HW_FAULT_LAST();
    g_audio.io_started = false;
}

static void audio_io_pause(void)
{
    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC) {
        bool keep_rx = false;
#if WAVE_PLAYER_ENABLE_REC
        keep_rx = g_audio.rec_path_enabled && g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC &&
                  g_audio.rx_block_bytes > 0U;
#endif
        audio_i2s_update_data_enable(false, keep_rx);
        return;
    }

    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC && g_audio.audac_dev != NULL) {
        (void)bflb_audac_feature_control(g_audio.audac_dev, AUDAC_CMD_PLAY_STOP, 0);
    }
}

static void audio_io_resume(void)
{
    bool enable_tx = (g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC);
    bool enable_rx = false;
#if WAVE_PLAYER_ENABLE_REC
    enable_rx = g_audio.rec_path_enabled && g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC &&
                g_audio.rx_block_bytes > 0U;
#endif
    if (enable_tx || enable_rx) {
        audio_i2s_update_data_enable(enable_tx, enable_rx);
    }

    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC && g_audio.audac_dev != NULL) {
        (void)bflb_audac_feature_control(g_audio.audac_dev, AUDAC_CMD_PLAY_START, 0);
        WAVE_PLAYER_DIAG_SET_HW_FAULT_LAST(
            ((uint32_t)bflb_audac_get_intstatus(g_audio.audac_dev)) & (AUDAC_INTSTS_FIFO_UNDER | AUDAC_INTSTS_FIFO_OVER));
    }
}

static void audio_io_stop(void)
{
    audio_transport_stop();
#if WAVE_PLAYER_ENABLE_REC
    ++g_audio.cap_epoch;
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
    return audio_codec_set_mic_pga(&g_audio.cap_codec, (int)pga);
}

static int wave_player_reload_tx_ring(uint32_t block_bytes)
{
    struct bflb_dma_channel_lli_transfer_s transfers[2] = {
        {
            .src_addr = (uint32_t)(uintptr_t)g_audio.pcm_ping,
            .dst_addr =
                g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC ? DMA_ADDR_I2S_TDR : DMA_ADDR_AUDAC_TDR,
            .nbytes = block_bytes,
        },
        {
            .src_addr = (uint32_t)(uintptr_t)g_audio.pcm_pong,
            .dst_addr =
                g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC ? DMA_ADDR_I2S_TDR : DMA_ADDR_AUDAC_TDR,
            .nbytes = block_bytes,
        },
    };
    return bflb_dma_channel_lli_reload(g_audio.dma_ch_tx, g_audio.tx_llipool,
                                       sizeof(g_audio.tx_llipool) / sizeof(g_audio.tx_llipool[0]), transfers, 2);
}

static int wave_player_config_internal(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes,
                                       uint8_t channels)
{
    uint32_t source_hz = samplerate_hz;
    uint8_t src_channels = channels;
    uint8_t out_channels = wave_player_default_playback_channels();
    uint8_t cap_channels = wave_player_default_capture_channels();
    uint32_t out_hz = 0;
    uint32_t cap_hz = 0;
    const uint32_t sample_bytes = (uint32_t)sizeof(int16_t);
    const uint32_t requested_tx_block_bytes = tx_block_bytes;
    uint32_t tx_frame_bytes = 0;
    uint32_t tx_dma_frame_bytes = 0;
    uint32_t logical_tx_block_bytes = tx_block_bytes;
    uint32_t tx_samples_per_ch = 0;
    uint8_t i2s_channels = 0;
    uint8_t tx_slot_count = 0;
    uint8_t tx_slot_word_bytes = (uint8_t)sample_bytes;
    bool rec_path_enable = false;
    bool builtin_tx_block_adjusted = false;
#if WAVE_PLAYER_ENABLE_REC
    bool shared_i2s_rate_adjusted = false;
    bool shared_bl_rate_adjusted = false;
    bool mixed_backend_rate_adjusted = false;
    uint32_t shared_rate_old_cap_hz = 0U;
    uint32_t mixed_rate_old_cap_hz = 0U;
#endif

#if WAVE_PLAYER_ENABLE_REC
    rec_path_enable = wave_capture_enable_requested() || audio_pcm_capture_prepared();
    rx_block_bytes = rec_path_enable ? (WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH * (uint32_t)cap_channels * sample_bytes) : 0U;
#else
    rx_block_bytes = 0U;
#endif
    bool i2s_tx_enable = (g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC);
    bool i2s_rx_enable = false;
#if WAVE_PLAYER_ENABLE_REC
    i2s_rx_enable = rec_path_enable && g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC &&
                    g_audio.dma_ch_rx != NULL && rx_block_bytes > 0U;
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
    out_channels = audio_channel_count_clamp(out_channels);
    cap_channels = audio_channel_count_clamp(cap_channels);

    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC) {
        uint32_t preferred_tx_block_bytes = (uint32_t)WAVE_PLAYER_OUT_SAMPLES * (uint32_t)out_channels * sample_bytes;
        if (preferred_tx_block_bytes != 0U && tx_block_bytes > preferred_tx_block_bytes) {
            tx_block_bytes = preferred_tx_block_bytes;
            builtin_tx_block_adjusted = true;
        }
    }
    logical_tx_block_bytes = tx_block_bytes;

#if (WAVE_PLAYER_FIX_SAMPLE_RATE > 0U)
    out_hz = (uint32_t)WAVE_PLAYER_FIX_SAMPLE_RATE;
#else
    out_hz = source_hz;
#endif
    cap_hz = source_hz;

#if WAVE_PLAYER_ENABLE_REC
    if (rec_path_enable && g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC &&
        g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC && cap_hz != out_hz) {
        shared_i2s_rate_adjusted = true;
        shared_rate_old_cap_hz = cap_hz;
        cap_hz = out_hz;
    }
    if (rec_path_enable && g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC &&
        g_audio.capture_backend == WAVE_PLAYER_BACKEND_BL_CODEC && cap_hz != out_hz) {
        shared_bl_rate_adjusted = true;
        shared_rate_old_cap_hz = cap_hz;
        cap_hz = out_hz;
    }
    if (rec_path_enable && g_audio.playback_backend != g_audio.capture_backend && cap_hz != out_hz &&
        !audio_sample_rate_share_pll_family(cap_hz, out_hz)) {
        mixed_backend_rate_adjusted = true;
        mixed_rate_old_cap_hz = cap_hz;
        cap_hz = out_hz;
    }
#endif

#if (WAVE_PLAYER_FIX_SAMPLE_RATE > 0U) && !WAVE_PLAYER_ENABLE_RESAMPLE
    if (source_hz != out_hz) {
        printf("wave_player: fix sample rate(%u) requires resample for src_hz=%u\r\n", out_hz, source_hz);
        return -EINVAL;
    }
#endif

    i2s_channels = i2s_tx_enable ? out_channels : 0;
    if (i2s_rx_enable && cap_channels > i2s_channels) {
        i2s_channels = cap_channels;
    }
    audio_codec_type_t i2s_codec_type = AUDIO_CODEC_TYPE_UNKNOWN;
    if (i2s_tx_enable) {
        i2s_codec_type = g_audio.play_codec.type;
    }
#if WAVE_PLAYER_ENABLE_REC
    else if (i2s_rx_enable) {
        i2s_codec_type = g_audio.cap_codec.type;
    }
#endif
    if (i2s_codec_type == AUDIO_CODEC_TYPE_JY6316) {
        i2s_channels = wave_player_i2s_tx_slot_count(i2s_channels, out_channels, i2s_codec_type);
        if (i2s_channels == 0U) {
            printf("wave_player: JY6316 requires four DSP bus slots; configured=%u\r\n",
                   (unsigned)WAVE_PLAYER_I2S_TX_SLOTS);
            return -EINVAL;
        }
    }
    tx_slot_count = out_channels;
    tx_slot_word_bytes = (uint8_t)sample_bytes;
    if (i2s_tx_enable) {
        tx_slot_count = wave_player_i2s_tx_slot_count(i2s_channels, out_channels, i2s_codec_type);
        tx_slot_word_bytes = wave_player_i2s_tx_slot_word_bytes();
        if (tx_slot_count > i2s_channels) {
            i2s_channels = tx_slot_count;
        }
    }
#if WAVE_PLAYER_I2S_DSP_MODE && WAVE_PLAYER_I2S_DSP_LR_MERGE
    if ((i2s_tx_enable || i2s_rx_enable) && (i2s_channels & 0x01U) != 0U) {
        printf("wave_player: dsp lr merge requires an even slot count, got %u\r\n", i2s_channels);
        return -EINVAL;
    }
#endif

    tx_frame_bytes = (uint32_t)out_channels * sample_bytes;
    if (logical_tx_block_bytes == 0U || (logical_tx_block_bytes % tx_frame_bytes) != 0U) {
        printf("wave_player: bad logical tx_block_bytes=%u, frame=%u\r\n", logical_tx_block_bytes, tx_frame_bytes);
        return -EINVAL;
    }
    tx_samples_per_ch = logical_tx_block_bytes / tx_frame_bytes;
    tx_dma_frame_bytes = (uint32_t)tx_slot_count * (uint32_t)tx_slot_word_bytes;
    tx_block_bytes = tx_samples_per_ch * tx_dma_frame_bytes;
    if (tx_dma_frame_bytes == 0U || tx_block_bytes == 0U || tx_block_bytes > sizeof(g_audio.pcm_ping)) {
        printf("wave_player: bad tx_dma_block=%u, dma_frame=%u, max=%u\r\n", tx_block_bytes, tx_dma_frame_bytes,
               (unsigned)sizeof(g_audio.pcm_ping));
        return -EINVAL;
    }

    if (g_audio.src_hz == source_hz && g_audio.out_hz == out_hz && g_audio.src_channels == src_channels &&
        g_audio.out_channels == out_channels && g_audio.tx_block_bytes == tx_block_bytes &&
        g_audio.logical_tx_block_bytes == logical_tx_block_bytes && g_audio.rx_block_bytes == rx_block_bytes &&
        g_audio.cap_hz == cap_hz && g_audio.cap_channels == cap_channels &&
        g_audio.tx_slot_count == tx_slot_count && g_audio.tx_slot_word_bytes == tx_slot_word_bytes &&
#if WAVE_PLAYER_ENABLE_REC
        g_audio.rec_path_enabled == rec_path_enable &&
#endif
        g_audio.io_started) {
        return 0;
    }

#if WAVE_PLAYER_ENABLE_REC
    if (shared_i2s_rate_adjusted) {
        printf("[AUDIO][WARN] shared i2s playback/capture aligns cap_hz %u->%u\r\n", shared_rate_old_cap_hz, cap_hz);
    }
    if (shared_bl_rate_adjusted) {
        printf("[AUDIO][WARN] shared builtin playback/capture aligns cap_hz %u->%u to avoid reconfig jitter\r\n",
               shared_rate_old_cap_hz, cap_hz);
    }
    if (mixed_backend_rate_adjusted) {
        printf("[AUDIO][WARN] mixed %s/%s playback/capture aligns cap_hz %u->%u to share audio pll family\r\n",
               wave_player_backend_name(g_audio.playback_backend), wave_player_backend_name(g_audio.capture_backend),
               mixed_rate_old_cap_hz, cap_hz);
    }
#endif
    if (builtin_tx_block_adjusted) {
        uint32_t req_samples_per_ch = requested_tx_block_bytes / tx_frame_bytes;
        uint32_t cur_samples_per_ch = tx_block_bytes / tx_frame_bytes;
        uint32_t req_block_ms = (out_hz == 0U) ? 0U : (uint32_t)(((uint64_t)req_samples_per_ch * 1000ULL) / out_hz);
        uint32_t cur_block_ms = (out_hz == 0U) ? 0U : (uint32_t)(((uint64_t)cur_samples_per_ch * 1000ULL) / out_hz);
        printf("[AUDIO][WARN] builtin playback shrinks tx_block %u->%u (%u ms->%u ms, out_ch=%u) to reduce underrun/jitter\r\n",
               requested_tx_block_bytes, tx_block_bytes, req_block_ms, cur_block_ms, out_channels);
    }
    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC && src_channels != out_channels) {
        printf("[AUDIO][INFO] builtin playback downmixes %u ch source to %u ch output; spatial effect may differ from i2s codec\r\n",
               src_channels, out_channels);
    }
    if (tx_block_bytes != logical_tx_block_bytes) {
        printf("[AUDIO][INFO] i2s dsp tx expands logical_tx=%u to dma_tx=%u, slots=%u, slot_word=%u\r\n",
               logical_tx_block_bytes, tx_block_bytes, tx_slot_count, tx_slot_word_bytes);
    }

    printf("audio_io_start: play=%s cap=%s src_hz=%u play_hz=%u cap_hz=%u tx_block=%u rx_block=%u src_ch=%u out_ch=%u cap_ch=%u rec=%u\r\n",
           wave_player_backend_name(g_audio.playback_backend), wave_player_backend_name(g_audio.capture_backend), source_hz,
           out_hz, cap_hz, tx_block_bytes, rx_block_bytes, src_channels, out_channels, cap_channels,
           rec_path_enable ? 1U : 0U);

    audio_io_stop();
#if WAVE_PLAYER_ENABLE_REC
    /* Descriptors refer to this DMA ring/sequence generation only. */
    if (g_audio.cap_queue != NULL) {
        xQueueReset(g_audio.cap_queue);
    }
#endif
    wave_player_diag_reset();

    struct bflb_device_s *i2s0 = g_audio.i2s_dev;
    if ((i2s_tx_enable || i2s_rx_enable) && i2s0 == NULL) {
        printf("wave_player: i2s backend requested but i2s device is missing\r\n");
        return -EINVAL;
    }

    if (i2s_tx_enable || i2s_rx_enable) {
        board_audio_pll_config_for_rate(i2s_tx_enable ? out_hz : cap_hz);

        struct bflb_i2s_config_s i2s_cfg;
        audio_fill_i2s_cfg(&i2s_cfg, i2s_tx_enable ? out_hz : cap_hz, i2s_channels);

        bflb_i2s_init(i2s0, &i2s_cfg);
        bflb_i2s_feature_control(i2s0, I2S_CMD_CHANNEL_LR_MERGE,
                                  WAVE_PLAYER_I2S_DSP_MODE ? WAVE_PLAYER_I2S_DSP_LR_MERGE : true);
        bflb_i2s_link_txdma(i2s0, i2s_tx_enable);
        bflb_i2s_link_rxdma(i2s0, i2s_rx_enable);
    }

    struct bflb_dma_channel_config_s tx_config;
    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC) {
        audio_fill_dma_tx_cfg(&tx_config);
    } else {
        audio_fill_dma_bl_tx_cfg(&tx_config);
    }

    if (g_audio.dma_ch_tx == NULL) {
        printf("dma tx not configured\r\n");
        return -EINVAL;
    }
    bflb_dma_channel_init(g_audio.dma_ch_tx, &tx_config);
    bflb_dma_channel_irq_attach(g_audio.dma_ch_tx, dma_ch_tx_isr, NULL);

#if WAVE_PLAYER_ENABLE_REC
    if (rec_path_enable && g_audio.dma_ch_rx != NULL && rx_block_bytes > 0U) {
        struct bflb_dma_channel_config_s rx_config;
        if (g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC) {
            audio_fill_dma_rx_cfg(&rx_config);
        } else {
            audio_fill_dma_bl_rx_cfg(&rx_config);
        }
        bflb_dma_channel_init(g_audio.dma_ch_rx, &rx_config);
        bflb_dma_channel_irq_attach(g_audio.dma_ch_rx, dma_ch_rx_isr, NULL);
    }
#endif

    int used_tx = wave_player_reload_tx_ring(tx_block_bytes);
    if (used_tx <= 0) {
        printf("tx dma lli reload failed: %d\r\n", used_tx);
        return -EINVAL;
    }
    bflb_dma_channel_lli_link_head(g_audio.dma_ch_tx, g_audio.tx_llipool, (uint32_t)used_tx);
    g_audio.tx_lli_count = (uint32_t)used_tx;

#if WAVE_PLAYER_ENABLE_REC
    if (rec_path_enable && g_audio.dma_ch_rx != NULL && rx_block_bytes > 0U) {
        struct bflb_dma_channel_lli_transfer_s rx_pp[WAVE_PLAYER_CAP_DMA_BLOCKS];
        for (uint32_t i = 0U; i < (uint32_t)WAVE_PLAYER_CAP_DMA_BLOCKS; i++) {
            rx_pp[i].src_addr = (g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC) ? (uint32_t)DMA_ADDR_I2S_RDR
                                                                                            : (uint32_t)DMA_ADDR_AUADC_RDR;
            rx_pp[i].dst_addr = (uint32_t)(uintptr_t)g_audio.cap_blocks[i];
            rx_pp[i].nbytes = rx_block_bytes;
        }

        int used_rx =
            bflb_dma_channel_lli_reload(g_audio.dma_ch_rx, g_audio.rx_llipool,
                                        (uint32_t)(sizeof(g_audio.rx_llipool) / sizeof(g_audio.rx_llipool[0])), rx_pp,
                                        (uint32_t)WAVE_PLAYER_CAP_DMA_BLOCKS);
        if (used_rx <= 0) {
            printf("rx dma lli reload failed: %d\r\n", used_rx);
            return -EINVAL;
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
    g_audio.cap_hz = cap_hz;
    g_audio.src_channels = src_channels;
    g_audio.out_channels = out_channels;
    g_audio.cap_channels = cap_channels;
    g_audio.tx_block_bytes = tx_block_bytes;
    g_audio.logical_tx_block_bytes = logical_tx_block_bytes;
    g_audio.rx_block_bytes = rx_block_bytes;
    g_audio.tx_slot_count = tx_slot_count;
    g_audio.tx_slot_word_bytes = tx_slot_word_bytes;
    g_audio.tx_period_samples = tx_samples_per_ch;
    g_audio.tx_total_samples = 0;

#if WAVE_PLAYER_ENABLE_REC
    g_audio.rec_path_enabled = rec_path_enable && (rx_block_bytes > 0U);
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        g_audio.cap_block_bytes = rx_block_bytes;
        g_audio.cap_samples_per_ch = rx_block_bytes / ((uint32_t)g_audio.cap_channels * (uint32_t)sizeof(int16_t));
        g_audio.cap_isr_block_idx = 0U;
        g_audio.cap_isr_seq = 0U;
        g_audio.cap_total_samples = 0;
    } else {
        g_audio.cap_block_bytes = 0;
        g_audio.cap_samples_per_ch = 0;
        g_audio.cap_isr_block_idx = 0U;
        g_audio.cap_isr_seq = 0U;
        g_audio.cap_total_samples = 0;
    }
#endif

    if (g_audio.play_codec.drv != NULL) {
        int ret = audio_codec_set_sample_rate(&g_audio.play_codec, out_hz);
        if (ret != 0 && ret != -2) {
            printf("wave_player: playback codec set_sample_rate(%u) failed: %d\r\n", out_hz, ret);
            return -EINVAL;
        }
    }
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.rec_path_enabled && g_audio.cap_codec.drv != NULL) {
        int ret = audio_codec_set_sample_rate(&g_audio.cap_codec, cap_hz);
        if (ret != 0 && ret != -2) {
            printf("wave_player: capture codec set_sample_rate(%u) failed: %d\r\n", cap_hz, ret);
            return -EINVAL;
        }
    }
#endif

    bflb_dma_channel_tcint_clear(g_audio.dma_ch_tx);
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        bflb_dma_channel_tcint_clear(g_audio.dma_ch_rx);
    }
#endif
    sem_drain(g_audio.sem_tx_dma_done);
    g_audio.tx_dma_completions = 0;
    bflb_dma_channel_start(g_audio.dma_ch_tx);
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        sem_drain(g_audio.sem_rx_dma_done);
    }
    if (g_audio.rec_path_enabled && g_audio.dma_ch_rx != NULL) {
        bflb_dma_channel_start(g_audio.dma_ch_rx);
    }
#endif

    if (i2s_tx_enable || i2s_rx_enable) {
        audio_i2s_update_data_enable(i2s_tx_enable, i2s_rx_enable);
    }
    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC && g_audio.audac_dev != NULL) {
        (void)bflb_audac_link_rxdma(g_audio.audac_dev, true);
        (void)bflb_audac_feature_control(g_audio.audac_dev, AUDAC_CMD_PLAY_START, 0);
        WAVE_PLAYER_DIAG_SET_HW_FAULT_LAST(
            ((uint32_t)bflb_audac_get_intstatus(g_audio.audac_dev)) & (AUDAC_INTSTS_FIFO_UNDER | AUDAC_INTSTS_FIFO_OVER));
    }
#if WAVE_PLAYER_ENABLE_REC
    if (g_audio.rec_path_enabled && g_audio.capture_backend == WAVE_PLAYER_BACKEND_BL_CODEC && g_audio.auadc_dev != NULL) {
        (void)bflb_auadc_link_rxdma(g_audio.auadc_dev, true);
        (void)bflb_auadc_feature_control(g_audio.auadc_dev, AUADC_CMD_RECORD_START, 0);
    }
#endif
    g_audio.io_started = true;
    return 0;
}

void wave_player_config(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes, uint8_t channels)
{
    if (!wave_player_legacy_enter()) {
        printf("wave_player: PCM sessions own hardware configuration\r\n");
        return;
    }
    (void)wave_player_config_internal(samplerate_hz, tx_block_bytes, rx_block_bytes, channels);
    wave_player_legacy_leave();
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

static uint32_t rb_read_block_aligned(Ring_Buffer_Type *rb, uint8_t *dst, uint32_t dst_bytes, uint32_t frame_bytes)
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

    return read_bytes;
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

#if WAVE_PLAYER_I2S_DSP_MODE
    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC &&
        g_audio.tx_slot_count >= dst_channels &&
        (g_audio.tx_slot_word_bytes == (uint8_t)sizeof(uint32_t) ||
         g_audio.tx_slot_word_bytes == (uint8_t)sizeof(int16_t))) {
        uint8_t slot_count = g_audio.tx_slot_count;
        uint32_t frame_words = (uint32_t)slot_count;
        uint32_t n = src_bytes / (src_channels * (uint32_t)sizeof(int16_t));
        uint32_t max_n = dst_block_bytes / (frame_words * (uint32_t)g_audio.tx_slot_word_bytes);
        uint32_t *dst32 = (uint32_t *)(uintptr_t)dst;

        if (n > max_n) {
            n = max_n;
        }
        memset(dst, 0, dst_block_bytes);
        if (src != NULL && n > 0U) {
            for (uint32_t i = 0; i < n; i++) {
                int16_t l;
                int16_t r;

                if (src_channels == 2U) {
                    l = src[i * 2U + 0U];
                    r = src[i * 2U + 1U];
                } else {
                    l = src[i];
                    r = l;
                }

                if (g_audio.tx_slot_word_bytes == (uint8_t)sizeof(uint32_t)) {
                    for (uint8_t slot = 0U; slot < slot_count; slot++) {
                        int16_t sample = ((slot & 0x01U) != 0U && dst_channels >= 2U) ? r : l;
                        dst32[i * frame_words + slot] = (uint32_t)(uint16_t)sample;
                    }
                } else {
                    for (uint8_t slot = 0U; slot < slot_count; slot++) {
                        dst[i * frame_words + slot] = ((slot & 0x01U) != 0U && dst_channels >= 2U) ? r : l;
                    }
                }
            }
        }
        bflb_l1c_dcache_clean_range((void *)dst, dst_block_bytes);
        return;
    }
#endif

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
    bool stop_audio = false;
    uint32_t tx_block_bytes = 0U;
    int16_t *src_pcm = NULL;
#if WAVE_PLAYER_ENABLE_RESAMPLE
    int16_t *resampled_pcm = NULL;
    uint32_t src_have_per_ch = 0;
    SpeexResamplerState *resampler = NULL;
#endif

    if (cfg == NULL || cfg->wave_rb == NULL || cfg->pcm_ping == NULL || cfg->pcm_pong == NULL ||
        cfg->pcm_fill == NULL || cfg->audio_stop == NULL || cfg->sem_tx_dma_done == NULL) {
        printf("wave_player: bad cfg\r\n");
        stop_audio = (cfg != NULL && cfg->audio_stop != NULL && g_audio.io_started);
        goto cleanup;
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
    uint32_t out_samples_per_ch = 0;
    uint32_t src_block_bytes = 0;
    uint32_t src_alloc_bytes = 0U;
    uint32_t logical_tx_block_bytes = 0U;

    tx_block_bytes = cfg->tx_block_bytes;
    logical_tx_block_bytes = cfg->logical_tx_block_bytes;
    if (logical_tx_block_bytes == 0U) {
        logical_tx_block_bytes = tx_block_bytes;
    }

    if (tx_block_bytes == 0U || tx_block_bytes > sizeof(g_audio.pcm_ping) || logical_tx_block_bytes == 0U ||
        (logical_tx_block_bytes % out_frame_bytes) != 0U) {
        printf("wave_player: bad tx_block=%u logical=%u, frame=%u, max=%u\r\n", tx_block_bytes,
               logical_tx_block_bytes, out_frame_bytes,
               (unsigned)sizeof(g_audio.pcm_ping));
        stop_audio = true;
        goto cleanup;
    }

    out_samples_per_ch = logical_tx_block_bytes / out_frame_bytes;
    if (out_samples_per_ch == 0U) {
        printf("wave_player: out_samples_per_ch is zero\r\n");
        stop_audio = true;
        goto cleanup;
    }

    {
        uint64_t src_block_bytes_u64 = (uint64_t)out_samples_per_ch * (uint64_t)src_channels * (uint64_t)sample_bytes;
        if (src_block_bytes_u64 > (uint64_t)UINT32_MAX) {
            printf("wave_player: src_block_bytes overflow (%llu)\r\n", (unsigned long long)src_block_bytes_u64);
            stop_audio = true;
            goto cleanup;
        }
        src_block_bytes = (uint32_t)src_block_bytes_u64;
    }

#if WAVE_PLAYER_ENABLE_RESAMPLE
    bool use_resample = (src_hz != out_hz);
    uint32_t src_need_per_ch = 0;
    src_alloc_bytes = src_block_bytes;

    if (use_resample) {
        uint64_t src_need_per_ch_u64 =
            (((uint64_t)out_samples_per_ch * (uint64_t)src_hz + (uint64_t)out_hz - 1U) / (uint64_t)out_hz) + 16U;
        uint64_t src_cap_per_ch_u64 = src_need_per_ch_u64 + 16U;
        uint64_t src_alloc_bytes_u64 = src_cap_per_ch_u64 * (uint64_t)src_channels * (uint64_t)sample_bytes;

        if (src_need_per_ch_u64 > (uint64_t)UINT32_MAX || src_alloc_bytes_u64 > (uint64_t)UINT32_MAX) {
            printf("wave_player: resample block overflow need=%llu alloc=%llu\r\n",
                   (unsigned long long)src_need_per_ch_u64, (unsigned long long)src_alloc_bytes_u64);
            stop_audio = true;
            goto cleanup;
        }
        src_need_per_ch = (uint32_t)src_need_per_ch_u64;
        src_alloc_bytes = (uint32_t)src_alloc_bytes_u64;
    }
#else
    src_alloc_bytes = src_block_bytes;
#endif

    src_pcm = (int16_t *)pvPortMalloc(src_alloc_bytes);
    if (src_pcm == NULL) {
        printf("wave_player: alloc scratch failed\r\n");
        stop_audio = true;
        goto cleanup;
    }
#if WAVE_PLAYER_ENABLE_RESAMPLE
    if (use_resample) {
        int err = RESAMPLER_ERR_SUCCESS;
        resampled_pcm = (int16_t *)pvPortMalloc(src_block_bytes);
        if (resampled_pcm == NULL) {
            printf("wave_player: alloc resample buf failed\r\n");
            stop_audio = true;
            goto cleanup;
        } else {
            resampler = speex_resampler_init((spx_uint32_t)src_channels, (spx_uint32_t)src_hz, (spx_uint32_t)out_hz,
                                             WAVE_PLAYER_RESAMPLE_QUALITY, &err);
            if (resampler == NULL || err != RESAMPLER_ERR_SUCCESS) {
                printf("wave_player: resampler init failed, err=%d\r\n", err);
                stop_audio = true;
                goto cleanup;
            } else {
                (void)speex_resampler_skip_zeros(resampler);
                printf("wave_player: enable resample %u->%u, ch=%u, q=%d\r\n", src_hz, out_hz, src_channels,
                       WAVE_PLAYER_RESAMPLE_QUALITY);
            }
        }
    }
#endif

    if (!audio_pcm_player_wait_reference()) {
        stop_audio = true;
        goto cleanup;
    }

    bool need_prime_pong = false;
    bool dma_started = false;
    bool ping_fill_seen = false;
    bool pong_fill_seen = false;
    uint32_t ping_last_fill_isr = 0U;
    uint32_t pong_last_fill_isr = 0U;
#if WAVE_PLAYER_ENABLE_RESAMPLE
    uint32_t resample_epoch = playback_flush_epoch;
#endif

    while (1) {
        if (g_audio.stop) {
            break;
        }
        uint32_t block_epoch = playback_flush_epoch;
#if WAVE_PLAYER_ENABLE_RESAMPLE
        if (resample_epoch != block_epoch) {
            src_have_per_ch = 0;
            if (resampler != NULL) speex_resampler_reset_mem(resampler);
            resample_epoch = block_epoch;
        }
#endif
        wave_player_diag_poll_playback_hw_xrun();
#if 0
        if (g_audio.paused) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
#endif
        int16_t *play_pcm = src_pcm;
        uint32_t src_read_bytes = src_block_bytes;
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
                        wave_player_diag_note_source_underrun(true, read_need_bytes, read_bytes, src_hz, src_channels,
                                                              out_hz, out_channels);
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
            src_read_bytes = rb_read_block_aligned(cfg->wave_rb, (uint8_t *)src_pcm, src_block_bytes, src_frame_bytes);
            if (dma_started == true && need_prime_pong == false && src_read_bytes < src_block_bytes) {
                wave_player_diag_note_source_underrun(false, src_block_bytes, src_read_bytes, src_hz, src_channels,
                                                      out_hz, out_channels);
            }
        }

        if (Ring_Buffer_Get_Length(cfg->wave_rb) < (Ring_Buffer_Get_Size(cfg->wave_rb) >> 1)) {
            xSemaphoreGive(cfg->sem_wave_rb_space);
        }
        if (!dma_started) {
            if (block_epoch != playback_flush_epoch) {
                memset(play_pcm, 0, src_block_bytes);
            }
            audio_pcm_prepare_playback(play_pcm, src_block_bytes);
            if (playback_frame_cb != NULL) {
                playback_frame_cb(play_pcm, out_samples_per_ch, out_hz, src_channels, playback_frame_user_data);
            }
            cfg->pcm_fill(cfg->pcm_ping, play_pcm, src_block_bytes, src_channels, (int)out_channels, tx_block_bytes);
            ping_fill_seen = true;
            ping_last_fill_isr = WAVE_PLAYER_DIAG_GET_TX_ISR();
            dma_started = true;
            need_prime_pong = true;
            continue;
        }

        if (need_prime_pong) {
            if (block_epoch != playback_flush_epoch) {
                memset(play_pcm, 0, src_block_bytes);
            }
            audio_pcm_prepare_playback(play_pcm, src_block_bytes);
            if (playback_frame_cb != NULL) {
                playback_frame_cb(play_pcm, out_samples_per_ch, out_hz, src_channels, playback_frame_user_data);
            }
            cfg->pcm_fill(cfg->pcm_pong, play_pcm, src_block_bytes, src_channels, (int)out_channels, tx_block_bytes);
            pong_fill_seen = true;
            pong_last_fill_isr = WAVE_PLAYER_DIAG_GET_TX_ISR();
            need_prime_pong = false;
            audio_pcm_player_started();
            continue;
        }
#if 0
        if (g_audio.paused) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
#endif
        (void)xTaskNotifyWait(0U, WAVE_PLAYER_TX_DONE_NOTIFY_BIT, NULL, portMAX_DELAY);
        if (g_audio.stop) {
            break;
        }
        if (block_epoch != playback_flush_epoch) {
            memset(play_pcm, 0, src_block_bytes);
#if WAVE_PLAYER_ENABLE_RESAMPLE
            src_have_per_ch = 0;
            if (resampler != NULL) speex_resampler_reset_mem(resampler);
#endif
        }
        WAVE_PLAYER_DIAG_TX_SEM_TAKE();
        wave_player_diag_poll_playback_hw_xrun();

        /* eSetBits coalesces notifications when DMA completes more than one
         * block before the task runs. Do not toggle once per task wakeup:
         * that can permanently put writes onto the active DMA block. Read
         * the current source as well, because hardware IRQs may coalesce
         * while interrupts are masked. Use the ISR sequence at transitions. */
        bool ping = (g_audio.tx_dma_completions & 1U) != 0U;
        uint32_t active_source = (uint32_t)bflb_dma_feature_control(
            g_audio.dma_ch_tx, DMA_CMD_GET_LLI_SRCADDR, 0);
        uint32_t ping_start = (uint32_t)(uintptr_t)cfg->pcm_ping;
        uint32_t pong_start = (uint32_t)(uintptr_t)cfg->pcm_pong;
        if (active_source >= ping_start && active_source - ping_start < tx_block_bytes) ping = false;
        else if (active_source >= pong_start && active_source - pong_start < tx_block_bytes) ping = true;
        int16_t *out = ping ? cfg->pcm_ping : cfg->pcm_pong;
        {
            uint32_t fill_isr = WAVE_PLAYER_DIAG_GET_TX_ISR();
            if (ping) {
                if (ping_fill_seen) {
                    uint32_t isr_delta = fill_isr - ping_last_fill_isr;
                    if (isr_delta > 2U) {
                        wave_player_diag_note_refill_miss(isr_delta, true, out_hz, out_channels, tx_block_bytes);
                    }
                }
                ping_last_fill_isr = fill_isr;
                ping_fill_seen = true;
            } else {
                if (pong_fill_seen) {
                    uint32_t isr_delta = fill_isr - pong_last_fill_isr;
                    if (isr_delta > 2U) {
                        wave_player_diag_note_refill_miss(isr_delta, false, out_hz, out_channels, tx_block_bytes);
                    }
                }
                pong_last_fill_isr = fill_isr;
                pong_fill_seen = true;
            }
        }
        audio_pcm_prepare_playback(play_pcm, src_block_bytes);
        if (playback_frame_cb != NULL) {
            playback_frame_cb(play_pcm, out_samples_per_ch, out_hz, src_channels, playback_frame_user_data);
        }
        cfg->pcm_fill(out, play_pcm, src_block_bytes, src_channels, (int)out_channels, tx_block_bytes);
        WAVE_PLAYER_DIAG_TX_FILL();
    }
    printf("wave_player: stopping\r\n");
    stop_audio = true;

cleanup:
    audio_pcm_player_failed();
    if (stop_audio && cfg != NULL && cfg->audio_stop != NULL) {
        cfg->audio_stop();
    }
    if (cfg != NULL && cfg->pcm_ping != NULL && tx_block_bytes > 0U && tx_block_bytes <= sizeof(g_audio.pcm_ping)) {
        memset(cfg->pcm_ping, 0, tx_block_bytes);
        if (audio_pcm_owned()) {
            bflb_l1c_dcache_clean_range(cfg->pcm_ping, tx_block_bytes);
        }
    }
    if (cfg != NULL && cfg->pcm_pong != NULL && tx_block_bytes > 0U && tx_block_bytes <= sizeof(g_audio.pcm_pong)) {
        memset(cfg->pcm_pong, 0, tx_block_bytes);
        if (audio_pcm_owned()) {
            bflb_l1c_dcache_clean_range(cfg->pcm_pong, tx_block_bytes);
        }
    }
#if WAVE_PLAYER_ENABLE_RESAMPLE
    if (resampler != NULL) {
        speex_resampler_destroy(resampler);
    }
    if (resampled_pcm != NULL) {
        vPortFree(resampled_pcm);
    }
#endif
    if (src_pcm != NULL) {
        vPortFree(src_pcm);
    }
    /* Clear the stop flag before making the engine available to a new owner. */
    taskENTER_CRITICAL();
    g_audio.stop = false;
    g_audio.wave_task = NULL;
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}

#if WAVE_PLAYER_ENABLE_REC
#if WAVE_PLAYER_ENABLE_REC_RB
static uint32_t align_up_u32(uint32_t value, uint32_t align)
{
    if (align == 0U) {
        return value;
    }

    uint32_t rem = value % align;
    if (rem == 0U) {
        return value;
    }

    return value + (align - rem);
}

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

static int wave_player_rec_dump_flash_internal(uint32_t flash_addr, uint32_t max_bytes)
{
    uint32_t write_pos;
    uint32_t sample_rate;
    uint32_t total_samples;
    uint32_t capacity = (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES;
    uint32_t start_idx;
    uint32_t erase_bytes;
    uint32_t first_samples;
    uint32_t second_samples;
    uint32_t dump_bytes;
    bool filled;
    bool truncated = false;

    if (g_audio.wave_task != NULL) {
        printf("rec_dump_flash: audio active, stop playback/record first\r\n");
        return -1;
    }

    taskENTER_CRITICAL();
    write_pos = g_audio.rec_write;
    filled = g_audio.rec_filled;
    sample_rate = g_audio.cap_hz;
    taskEXIT_CRITICAL();

    total_samples = filled ? capacity : write_pos;
    if (total_samples == 0U) {
        printf("rec_dump_flash: no data\r\n");
        return -1;
    }

    start_idx = filled ? (write_pos % capacity) : 0U;

    max_bytes &= ~(uint32_t)(sizeof(int16_t) - 1U);
    if (max_bytes == 0U) {
        printf("rec_dump_flash: max_bytes must be >= %u\r\n", (unsigned)sizeof(int16_t));
        return -1;
    }

    if ((total_samples * (uint32_t)sizeof(int16_t)) > max_bytes) {
        uint32_t limit_samples = max_bytes / (uint32_t)sizeof(int16_t);
        uint32_t skip_samples = total_samples - limit_samples;
        start_idx = (start_idx + skip_samples) % capacity;
        total_samples = limit_samples;
        truncated = true;
    }

    dump_bytes = total_samples * (uint32_t)sizeof(int16_t);
    erase_bytes = align_up_u32(dump_bytes, WAVE_PLAYER_REC_DUMP_FLASH_ERASE_ALIGN);
    if (erase_bytes < dump_bytes) {
        printf("rec_dump_flash: erase size overflow\r\n");
        return -1;
    }

    if ((flash_addr + erase_bytes) < flash_addr) {
        printf("rec_dump_flash: address overflow\r\n");
        return -1;
    }

    if (sample_rate == 0U) {
        sample_rate = 16000U;
    }

    if (bflb_flash_erase(flash_addr, erase_bytes) != 0) {
        printf("rec_dump_flash: erase failed addr=0x%08lx len=0x%08lx\r\n",
               (unsigned long)flash_addr, (unsigned long)erase_bytes);
        return -1;
    }

    first_samples = total_samples;
    if ((start_idx + first_samples) > capacity) {
        first_samples = capacity - start_idx;
    }
    second_samples = total_samples - first_samples;

    if (first_samples > 0U) {
        uint32_t first_bytes = first_samples * (uint32_t)sizeof(int16_t);
        if (bflb_flash_write(flash_addr, (uint8_t *)&g_audio.rec_mono_buf[start_idx], first_bytes) != 0) {
            printf("rec_dump_flash: write failed addr=0x%08lx len=0x%08lx\r\n",
                   (unsigned long)flash_addr, (unsigned long)first_bytes);
            return -1;
        }
    }

    if (second_samples > 0U) {
        uint32_t second_addr = flash_addr + first_samples * (uint32_t)sizeof(int16_t);
        uint32_t second_bytes = second_samples * (uint32_t)sizeof(int16_t);
        if (bflb_flash_write(second_addr, (uint8_t *)g_audio.rec_mono_buf, second_bytes) != 0) {
            printf("rec_dump_flash: write failed addr=0x%08lx len=0x%08lx\r\n",
                   (unsigned long)second_addr, (unsigned long)second_bytes);
            return -1;
        }
    }

    printf("rec_dump_flash: addr=0x%08lx bytes=%lu samples=%lu sample_rate=%lu ch=1 erase=0x%08lx%s\r\n",
           (unsigned long)flash_addr, (unsigned long)dump_bytes, (unsigned long)total_samples,
           (unsigned long)sample_rate, (unsigned long)erase_bytes,
           truncated ? " [truncated to media window]" : "");
    return 0;
}

static int wave_player_rec_dump_flash(uint32_t flash_addr, uint32_t max_bytes)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_rec_dump_flash_internal(flash_addr, max_bytes);
    wave_player_legacy_leave();
    return result;
}
#endif

static bool wave_player_capture_layout_valid(wave_player_capture_layout_t layout, uint8_t channels)
{
    return channels > 0U && channels <= 16U &&
           ((layout.mic_mask | layout.ref_mask) >> channels) == 0U &&
           (layout.mic_mask & layout.ref_mask) == 0U;
}

static void audio_cap_proc(void *arg)
{
    (void)arg;
    cap_msg_t msg;

    for (;;) {
        if (g_audio.cap_queue == NULL) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        if (xQueueReceive(g_audio.cap_queue, &msg, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (!g_audio.rec_enable || msg.epoch != g_audio.cap_epoch || msg.block_idx >= WAVE_PLAYER_CAP_DMA_BLOCKS ||
            msg.channels == 0U || msg.channels > WAVE_PLAYER_REC_CHANNELS || msg.samples_per_ch == 0U ||
            msg.samples_per_ch > WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH ||
            msg.nbytes != msg.samples_per_ch * msg.channels * sizeof(int16_t)) {
            continue;
        }
        {
            /* The next block is already being written by DMA. Drop queued
             * descriptors whose buffer is being reused, including at wrap. */
            uint32_t lag = g_audio.cap_isr_seq - msg.seq;
            WAVE_PLAYER_DIAG_NOTE_RX_LAG(lag);
            if (lag >= WAVE_PLAYER_CAP_DMA_BLOCKS - 1U) continue;
        }
        const int16_t *samples = g_audio.cap_blocks[msg.block_idx];
        bflb_l1c_dcache_invalidate_range((void *)samples, msg.nbytes);
        const wave_player_rec_frame_t frame = {
            .samples = samples,
            .samples_per_channel = msg.samples_per_ch,
            .sample_rate_hz = msg.hz,
            .channels = msg.channels,
            .layout = g_audio.capture_layout,
            .sequence = msg.seq,
            .epoch = msg.epoch,
            .first_sample_index = msg.first_sample_index,
        };
        wave_player_rec_cb_t callback;
        void *callback_context;
        taskENTER_CRITICAL();
        callback = g_audio.rec_cb;
        callback_context = g_audio.rec_cb_user_data;
        taskEXIT_CRITICAL();
        if (callback != NULL) {
            callback(&frame, callback_context);
        }
#if WAVE_PLAYER_ENABLE_REC_RB
        /* Compatibility storage only: the callback above always gets all slots. */
        uint32_t selected = 0U;
        for (uint32_t c = 0U; c < frame.channels; c++) {
            if (frame.layout.mic_mask & (1U << c)) selected++;
        }
        if (selected == 0U) continue;
        for (uint32_t i = 0U; i < frame.samples_per_channel; i++) {
            int32_t sum = 0;
            for (uint32_t c = 0U; c < frame.channels; c++) {
                if (frame.layout.mic_mask & (1U << c)) sum += samples[i * frame.channels + c];
            }
            g_audio.rec_mono_buf[g_audio.rec_write++] = (int16_t)(sum / (int32_t)selected);
            if (g_audio.rec_write >= (uint32_t)WAVE_PLAYER_REC_MAX_SAMPLES) {
                g_audio.rec_write = 0;
                g_audio.rec_filled = true;
            }
        }
#endif
    }
}
#endif

static wave_player_backend_t wave_player_resolve_backend(wave_player_backend_t requested, wave_player_backend_t configured,
                                                         bool has_i2s_path, bool has_bl_path)
{
    if (requested != WAVE_PLAYER_BACKEND_AUTO) {
        if (requested == WAVE_PLAYER_BACKEND_I2S_CODEC) {
            return has_i2s_path ? requested : WAVE_PLAYER_BACKEND_AUTO;
        }
        if (requested == WAVE_PLAYER_BACKEND_BL_CODEC) {
            return has_bl_path ? requested : WAVE_PLAYER_BACKEND_AUTO;
        }
        return requested;
    }
    if (configured != WAVE_PLAYER_BACKEND_AUTO) {
        if (configured == WAVE_PLAYER_BACKEND_I2S_CODEC) {
            return has_i2s_path ? configured : WAVE_PLAYER_BACKEND_AUTO;
        }
        if (configured == WAVE_PLAYER_BACKEND_BL_CODEC) {
            return has_bl_path ? configured : WAVE_PLAYER_BACKEND_AUTO;
        }
        return configured;
    }
    if (has_i2s_path) {
        return WAVE_PLAYER_BACKEND_I2S_CODEC;
    }
    if (has_bl_path) {
        return WAVE_PLAYER_BACKEND_BL_CODEC;
    }
    return WAVE_PLAYER_BACKEND_AUTO;
}

/* Initialize audio output */
int wave_player_init(const wave_player_hw_cfg_t *hw_cfg)
{
    audio_codec_cfg_t play_codec_cfg = {
        .sample_rate_hz = 48000,
        .channels = 2,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S,
        .bits = AUDIO_CODEC_BITS_16,
        .codec_master = false,
        .enable_adc = false,
        .enable_dac = true,
        .work_mode = AUDIO_CODEC_WORK_MODE_CODEC,
    };
    audio_codec_cfg_t cap_codec_cfg = play_codec_cfg;
    cap_codec_cfg.channels = 1;
    cap_codec_cfg.enable_adc = true;
    cap_codec_cfg.enable_dac = false;
    cap_codec_cfg.mic_input_mode = AUDIO_CODEC_MIC_INPUT_DIFF;

    const char *audac_name = NULL;
    const char *auadc_name = NULL;
    bool need_ext_play = false;
    bool need_ext_cap = false;
    bool need_bl_play = false;
    bool need_bl_cap = false;

    if (hw_cfg == NULL || hw_cfg->dma_tx_dev_name == NULL) {
        printf("wave_player_init: invalid hw cfg\r\n");
        return -1;
    }
    if (hw_cfg->playback_task_priority >= configMAX_PRIORITIES ||
        hw_cfg->capture_task_priority >= configMAX_PRIORITIES) {
        printf("wave_player_init: invalid task priority\r\n");
        return -1;
    }
    if (g_audio.sem_tx_dma_done != NULL) {
        printf("wave_player_init: already initialized\r\n");
        return -1;
    }
    playback_task_priority = hw_cfg->playback_task_priority ? hw_cfg->playback_task_priority : 3;

    audac_name = (hw_cfg->audac_dev_name != NULL) ? hw_cfg->audac_dev_name : "audac";
    auadc_name = (hw_cfg->auadc_dev_name != NULL) ? hw_cfg->auadc_dev_name : "auadc";

    g_audio.playback_backend =
        wave_player_resolve_backend(hw_cfg->playback_backend, WAVE_PLAYER_PLAYBACK_BACKEND_DEFAULT,
                                    (hw_cfg->i2s_dev_name != NULL && hw_cfg->i2c_dev_name != NULL),
                                    WAVE_PLAYER_BUILTIN_CODEC_AVAILABLE);
    g_audio.capture_backend =
        wave_player_resolve_backend(hw_cfg->capture_backend, WAVE_PLAYER_CAPTURE_BACKEND_DEFAULT,
                                    (hw_cfg->i2s_dev_name != NULL && hw_cfg->i2c_dev_name != NULL),
                                    WAVE_PLAYER_BUILTIN_CODEC_AVAILABLE);

    if (g_audio.playback_backend == WAVE_PLAYER_BACKEND_AUTO || g_audio.capture_backend == WAVE_PLAYER_BACKEND_AUTO) {
        printf("wave_player_init: backend resolution failed\r\n");
        return -1;
    }

    need_ext_play = (g_audio.playback_backend == WAVE_PLAYER_BACKEND_I2S_CODEC);
    need_ext_cap = (g_audio.capture_backend == WAVE_PLAYER_BACKEND_I2S_CODEC);
    need_bl_play = (g_audio.playback_backend == WAVE_PLAYER_BACKEND_BL_CODEC);
    need_bl_cap = (g_audio.capture_backend == WAVE_PLAYER_BACKEND_BL_CODEC);

    g_audio.i2s_dev = ((need_ext_play || need_ext_cap) && hw_cfg->i2s_dev_name != NULL)
                          ? bflb_device_get_by_name(hw_cfg->i2s_dev_name)
                          : NULL;
    g_audio.i2c_dev = ((need_ext_play || need_ext_cap) && hw_cfg->i2c_dev_name != NULL)
                          ? bflb_device_get_by_name(hw_cfg->i2c_dev_name)
                          : NULL;
    g_audio.audac_dev = need_bl_play ? bflb_device_get_by_name(audac_name) : NULL;
    g_audio.auadc_dev = need_bl_cap ? bflb_device_get_by_name(auadc_name) : NULL;
    g_audio.dma_ch_tx = bflb_device_get_by_name(hw_cfg->dma_tx_dev_name);
    g_audio.dma_ch_rx = (hw_cfg->dma_rx_dev_name != NULL) ? bflb_device_get_by_name(hw_cfg->dma_rx_dev_name) : NULL;

    if ((need_ext_play || need_ext_cap) && g_audio.i2s_dev == NULL) {
        printf("i2s device not found: %s\r\n", (hw_cfg->i2s_dev_name != NULL) ? hw_cfg->i2s_dev_name : "(null)");
        return -1;
    }
    if ((need_ext_play || need_ext_cap) && g_audio.i2c_dev == NULL) {
        printf("i2c device not found: %s\r\n", (hw_cfg->i2c_dev_name != NULL) ? hw_cfg->i2c_dev_name : "(null)");
        return -1;
    }
    if (need_bl_play && g_audio.audac_dev == NULL) {
        printf("audac device not found: %s\r\n", audac_name);
        return -1;
    }
    if (need_bl_cap && g_audio.auadc_dev == NULL) {
        printf("auadc device not found: %s\r\n", auadc_name);
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

    memset(&g_audio.play_codec, 0, sizeof(g_audio.play_codec));
    memset(&g_audio.cap_codec, 0, sizeof(g_audio.cap_codec));

    if (need_ext_play || need_ext_cap) {
        board_i2s_codec_gpio_init();
        bflb_i2c_init(g_audio.i2c_dev, 200000);

        audio_codec_dev_t ext_codec = { 0 };
        audio_codec_cfg_t ext_codec_cfg = need_ext_play ? play_codec_cfg : cap_codec_cfg;
        if (AUDIO_CODEC_TYPE_UNKNOWN == audio_codec_scan(g_audio.i2c_dev, &ext_codec)) {
            printf("audio codec scan failed\r\n");
            return -1;
        }

        if (hw_cfg->required_ext_codec_type != AUDIO_CODEC_TYPE_UNKNOWN &&
            ext_codec.type != hw_cfg->required_ext_codec_type) {
            printf("audio codec mismatch: required type=%u, found %s (type=%u)\r\n",
                   (unsigned)hw_cfg->required_ext_codec_type,
                   ext_codec.name != NULL ? ext_codec.name : "unknown",
                   (unsigned)ext_codec.type);
            return -1;
        }

        if (ext_codec.type == AUDIO_CODEC_TYPE_JY6316) {
#if WAVE_PLAYER_I2S_DSP_MODE
#if (WAVE_PLAYER_FIX_SAMPLE_RATE > 0U)
            ext_codec_cfg.sample_rate_hz = (uint32_t)WAVE_PLAYER_FIX_SAMPLE_RATE;
#endif
            ext_codec_cfg.i2s_fmt = AUDIO_CODEC_I2S_FMT_DSP;
            ext_codec_cfg.bits = AUDIO_CODEC_BITS_16;
            ext_codec_cfg.channels = need_ext_cap ? wave_player_default_capture_channels() :
                                                    wave_player_default_playback_channels();
#else
            printf("JY6316 requires CONFIG_I2S_DSP_MODE=1\r\n");
            return -1;
#endif
        } else if (ext_codec.type == AUDIO_CODEC_TYPE_JY6166) {
#if WAVE_PLAYER_I2S_DSP_MODE
            ext_codec_cfg.i2s_fmt = AUDIO_CODEC_I2S_FMT_DSP;
            ext_codec_cfg.bits = AUDIO_CODEC_BITS_16;
#else
            ext_codec_cfg.i2s_fmt = AUDIO_CODEC_I2S_FMT_LEFT_J;
            ext_codec_cfg.bits = AUDIO_CODEC_BITS_32;
#endif
            ext_codec_cfg.channels = need_ext_cap ? wave_player_default_capture_channels() :
                                                    wave_player_default_playback_channels();
        }

        ext_codec_cfg.enable_dac = need_ext_play;
        ext_codec_cfg.enable_adc = need_ext_cap;
        if (need_ext_cap) {
            ext_codec_cfg.mic_input_mode = cap_codec_cfg.mic_input_mode;
        }
        if (0 != audio_codec_init(&ext_codec, &ext_codec_cfg)) {
            printf("audio codec init failed\r\n");
            return -1;
        }

        if (need_ext_play) {
            g_audio.play_codec = ext_codec;
            (void)audio_codec_set_mute(&g_audio.play_codec, false);
            if (0 != audio_codec_set_volume(&g_audio.play_codec, 50)) {
                printf("audio codec set volume failed\r\n");
                return -1;
            }
        }

        if (need_ext_cap) {
            g_audio.cap_codec = ext_codec;
            if (0 != wave_player_apply_rec_mic_pga((uint8_t)WAVE_PLAYER_REC_DEFAULT_MIC_PGA)) {
                printf("audio codec set mic pga failed\r\n");
                return -1;
            }
        }

        printf("audio codec ready: %s (0x%02x)\r\n", ext_codec.name, ext_codec.i2c_addr);
    }

#ifdef CONFIG_BSP_AUDIO_CODEC_BL_ENABLE
    if (need_bl_play) {
        board_audac_gpio_init();
        audio_codec_bl_hw_cfg_t bl_play_cfg = {
            .audac = g_audio.audac_dev,
            .auadc = NULL,
        };
        if (audio_codec_open_builtin(&g_audio.play_codec, &bl_play_cfg) != 0 ||
            audio_codec_init(&g_audio.play_codec, &play_codec_cfg) != 0) {
            printf("wave_player_init: builtin playback codec init failed\r\n");
            return -1;
        }
        (void)audio_codec_set_mute(&g_audio.play_codec, false);
        if (0 != audio_codec_set_volume(&g_audio.play_codec, 50)) {
            printf("wave_player_init: builtin playback volume init failed\r\n");
            return -1;
        }
        printf("audio codec ready: %s (builtin playback)\r\n", g_audio.play_codec.name);
    }

    if (need_bl_cap) {
        board_auadc_gpio_init();
        audio_codec_bl_hw_cfg_t bl_cap_cfg = {
            .audac = NULL,
            .auadc = g_audio.auadc_dev,
        };
        if (audio_codec_open_builtin(&g_audio.cap_codec, &bl_cap_cfg) != 0 ||
            audio_codec_init(&g_audio.cap_codec, &cap_codec_cfg) != 0) {
            printf("wave_player_init: builtin capture codec init failed\r\n");
            return -1;
        }
        {
            int ret = wave_player_apply_rec_mic_pga((uint8_t)WAVE_PLAYER_REC_DEFAULT_MIC_PGA);
            if (ret != 0 && ret != -2) {
                printf("wave_player_init: builtin capture pga init failed\r\n");
                return -1;
            }
        }
        printf("audio codec ready: %s (builtin capture)\r\n", g_audio.cap_codec.name);
    }
#endif

    g_audio.src_hz = play_codec_cfg.sample_rate_hz;
    g_audio.out_hz = play_codec_cfg.sample_rate_hz;
    g_audio.cap_hz = cap_codec_cfg.sample_rate_hz;
    g_audio.out_channels = wave_player_default_playback_channels();
    g_audio.cap_channels = wave_player_default_capture_channels();
#if WAVE_PLAYER_ENABLE_REC
    if (!wave_player_capture_layout_valid(hw_cfg->capture_layout, g_audio.cap_channels)) {
        printf("wave_player_init: invalid capture channel layout\r\n");
        return -1;
    }
    g_audio.capture_layout = hw_cfg->capture_layout;
#if WAVE_PLAYER_ENABLE_REC_RB
    if (g_audio.capture_layout.mic_mask == 0U) {
        printf("wave_player_init: warning: capture_layout.mic_mask=0; mono recording ring will stay empty\r\n");
    }
#endif
#endif
    g_audio.tx_slot_count = g_audio.out_channels;
    g_audio.tx_slot_word_bytes = (uint8_t)sizeof(int16_t);
    g_audio.sem_tx_dma_done = xSemaphoreCreateBinary();
    g_audio.sem_wave_rb_space = xSemaphoreCreateBinary();
    if (g_audio.sem_tx_dma_done == NULL || g_audio.sem_wave_rb_space == NULL) {
        printf("wave_player_init: playback semaphore alloc failed\r\n");
        goto fail;
    }

    Ring_Buffer_Init(&g_audio.wave_rb, g_audio.wave_src_buf, sizeof(g_audio.wave_src_buf), rb_lock, rb_unlock);
    wave_player_diag_reset();

#if WAVE_PLAYER_ENABLE_REC
    g_audio.sem_rx_dma_done = xSemaphoreCreateBinary();
    if (g_audio.sem_rx_dma_done == NULL) {
        printf("wave_player_init: capture semaphore alloc failed\r\n");
        goto fail;
    }
    if (g_audio.cap_queue == NULL) {
        g_audio.cap_queue = xQueueCreate((UBaseType_t)(WAVE_PLAYER_CAP_DMA_BLOCKS * 2U), sizeof(cap_msg_t));
    }
    if (g_audio.cap_queue == NULL) {
        printf("wave_player_init: cap queue alloc failed\r\n");
        goto fail;
    }
    /* VAD callback path is heavier than plain copy; keep capture task stack sufficient. */
    UBaseType_t capture_priority = hw_cfg->capture_task_priority ? hw_cfg->capture_task_priority : 2;
    if (xTaskCreate(audio_cap_proc, (char *)"audio_cap", 2048, NULL, capture_priority, &g_audio.cap_task) != pdPASS) {
        printf("wave_player_init: capture task alloc failed\r\n");
        goto fail;
    }
#endif

    return 0;

fail:
#if WAVE_PLAYER_ENABLE_REC
    /* Task creation is the last allocation; no capture task runs on failure. */
    g_audio.cap_task = NULL;
    if (g_audio.cap_queue != NULL) {
        vQueueDelete(g_audio.cap_queue);
        g_audio.cap_queue = NULL;
    }
    if (g_audio.sem_rx_dma_done != NULL) {
        vSemaphoreDelete(g_audio.sem_rx_dma_done);
        g_audio.sem_rx_dma_done = NULL;
    }
#endif
    if (g_audio.sem_wave_rb_space != NULL) {
        vSemaphoreDelete(g_audio.sem_wave_rb_space);
        g_audio.sem_wave_rb_space = NULL;
    }
    if (g_audio.sem_tx_dma_done != NULL) {
        vSemaphoreDelete(g_audio.sem_tx_dma_done);
        g_audio.sem_tx_dma_done = NULL;
    }
    return -1;
}

static int wave_player_feed_internal(uint8_t *data, uint32_t len)
{
    uint32_t empty = Ring_Buffer_Get_Empty_Length(&g_audio.wave_rb);
    if (empty == 0) {
        /* Full is ordinary backpressure for this nonblocking API. Logging on
         * every retry can further delay the consumer. The caller owns retries. */
        return 0;
    }

    if (len > empty) {
        len = empty;
    }

    uint32_t wrote = Ring_Buffer_Write(&g_audio.wave_rb, data, len);
    WAVE_PLAYER_DIAG_NOTE_TX_SOURCE_WRITE(wrote);
    return wrote;
}

int wave_player_feed(uint8_t *data, uint32_t len)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_feed_internal(data, len);
    wave_player_legacy_leave();
    return result;
}

uint32_t wave_player_get_buffered_bytes(void)
{
    return Ring_Buffer_Get_Length(&g_audio.wave_rb);
}

static uint32_t wave_player_trim_buffer_internal(uint32_t max_bytes)
{
    uint32_t frame_bytes = (g_audio.src_channels ? g_audio.src_channels : 1U) * sizeof(int16_t);
    uint32_t dropped = 0;
    max_bytes -= max_bytes % frame_bytes;
    /* Serialize the read position update with the player task. Advancing
     * never passes the queued source length, including during a DMA wakeup. */
    taskENTER_CRITICAL();
    uint32_t queued = Ring_Buffer_Get_Length(&g_audio.wave_rb);
    queued -= queued % frame_bytes;
    if (queued > max_bytes) {
        dropped = Ring_Buffer_Advance_Read(&g_audio.wave_rb, queued - max_bytes);
    }
    taskEXIT_CRITICAL();
    return dropped;
}

uint32_t wave_player_trim_buffer(uint32_t max_bytes)
{
    if (!wave_player_legacy_enter()) {
        return 0;
    }
    uint32_t discarded = wave_player_trim_buffer_internal(max_bytes);
    wave_player_legacy_leave();
    return discarded;
}

static void wave_player_flush_internal(void)
{
    taskENTER_CRITICAL();
    Ring_Buffer_Reset(&g_audio.wave_rb);
    playback_flush_epoch++;
    taskEXIT_CRITICAL();
    WAVE_PLAYER_DIAG_RESET_TX_SOURCE_SEEN();
    if (g_audio.sem_wave_rb_space != NULL) {
        xSemaphoreGive(g_audio.sem_wave_rb_space);
    }
}

void wave_player_flush(void)
{
    if (!wave_player_legacy_enter()) {
        return;
    }
    wave_player_flush_internal();
    wave_player_legacy_leave();
}

uint32_t wave_player_feed_blocking(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0 || !wave_player_legacy_enter()) {
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
        WAVE_PLAYER_DIAG_NOTE_TX_SOURCE_WRITE(wrote);
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

    wave_player_legacy_leave();
    return wrote_total;
}

/* Start audio output */
static int wave_player_start_internal(void)
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
    WAVE_PLAYER_DIAG_RESET_TX_SOURCE_SEEN();

    g_audio.stop = false;
    g_audio.paused = false;
    g_audio.wave_cfg.wave_rb = &g_audio.wave_rb;
    g_audio.wave_cfg.sem_wave_rb_space = g_audio.sem_wave_rb_space;
    g_audio.wave_cfg.src_sample_rate_hz = g_audio.src_hz;
    g_audio.wave_cfg.output_sample_rate_hz = (uint32_t)g_audio.out_hz;
    g_audio.wave_cfg.tx_block_bytes = g_audio.tx_block_bytes;
    g_audio.wave_cfg.logical_tx_block_bytes = g_audio.logical_tx_block_bytes;
    g_audio.wave_cfg.src_channels = g_audio.src_channels;
    g_audio.wave_cfg.out_channels = g_audio.out_channels;
    g_audio.wave_cfg.sem_tx_dma_done = g_audio.sem_tx_dma_done;
    g_audio.wave_cfg.pcm_ping = g_audio.pcm_ping;
    g_audio.wave_cfg.pcm_pong = g_audio.pcm_pong;
    g_audio.wave_cfg.pcm_fill = pcm_fill_pcm_block;
    g_audio.wave_cfg.audio_stop = audio_pcm_owned() ? audio_pcm_player_cleanup : audio_io_stop;
    if (xTaskCreate(wave_player_task, (char *)"wav_play", WAVE_PLAYER_TASK_STACK_DEPTH, &g_audio.wave_cfg, playback_task_priority,
                    &g_audio.wave_task) != pdPASS) {
        printf("wave_player: create task failed\r\n");
        g_audio.wave_task = NULL;
        if (audio_pcm_owned()) {
            audio_pcm_player_cleanup();
        } else {
            audio_io_stop();
        }
        return -1;
    }

    return 0;
}

int wave_player_start(void)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_start_internal();
    wave_player_legacy_leave();
    return result;
}

/* Start audio suspend */
static int wave_player_suspend_internal(void)
{
    if (g_audio.wave_task == NULL) {
        return 0;
    }
    audio_io_pause();
    g_audio.paused = true;
    xSemaphoreGive(g_audio.sem_wave_rb_space);

    return 0;
}

int wave_player_suspend(void)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_suspend_internal();
    wave_player_legacy_leave();
    return result;
}

static int wave_player_resume_internal(void)
{
    if (g_audio.wave_task == NULL || !g_audio.paused) {
        return 0;
    }
    audio_io_resume();
    g_audio.paused = false;

    return 0;
}

int wave_player_resume(void)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_resume_internal();
    wave_player_legacy_leave();
    return result;
}

/* Stop audio output */
static int wave_player_stop_timeout_internal(uint32_t timeout_ms)
{
    uint64_t started = bflb_mtimer_get_time_us();
    if (g_audio.wave_task != NULL && g_audio.wave_task == xTaskGetCurrentTaskHandle()) {
        return -EDEADLK;
    }
    if (g_audio.wave_task == NULL) {
        if (g_audio.io_started && !audio_pcm_keep_capture()) {
            audio_io_stop();
        }
        return 0;
    }
    /* enable audio output to wakeup player thead to exit */
    if (g_audio.paused) {
        audio_io_resume();
    }
    taskENTER_CRITICAL();
    g_audio.stop = true;
    g_audio.paused = false;
    if (g_audio.wave_task != NULL) {
        (void)xTaskNotify(g_audio.wave_task, WAVE_PLAYER_TX_DONE_NOTIFY_BIT, eSetBits);
    }
    taskEXIT_CRITICAL();
    while (g_audio.wave_task != NULL) {
        if (timeout_ms != UINT32_MAX && bflb_mtimer_get_time_us() - started >= (uint64_t)timeout_ms * 1000U) {
            return -ETIMEDOUT;
        }
        vTaskDelay(1);
    }
    return 0;
}

int wave_player_stop(void)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_stop_timeout_internal(UINT32_MAX);
    wave_player_legacy_leave();
    return result;
}

/* Mute/unmute audio output */
static int wave_player_set_mute_internal(bool mute)
{
    g_audio.muted = mute;
    return audio_codec_set_mute(&g_audio.play_codec, mute);
}

int wave_player_set_mute(bool mute)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_set_mute_internal(mute);
    wave_player_legacy_leave();
    return result;
}

/* Set volume (0-100) */
static int wave_player_set_volume_internal(uint8_t volume)
{
    if (volume > 100) {
        volume = 100;
    }

    g_audio.volume = volume;
    int ret = audio_codec_set_volume(&g_audio.play_codec, volume);

    if (ret == 0) {
        printf("[AUDIO] Volume set to %d\r\n", volume);
    } else {
        printf("[AUDIO] ERROR: Failed to set volume: %d\r\n", ret);
    }

    return ret;
}

int wave_player_set_volume(uint8_t volume)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_set_volume_internal(volume);
    wave_player_legacy_leave();
    return result;
}

static int wave_player_set_record_volume_internal(uint8_t volume)
{
    if (volume > 100) {
        volume = 100;
    }

    return audio_codec_set_record_volume(&g_audio.cap_codec, volume);
}

int wave_player_set_record_volume(uint8_t volume)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_set_record_volume_internal(volume);
    wave_player_legacy_leave();
    return result;
}

static int wave_player_set_record_pga_internal(uint8_t pga)
{
    if (pga > 100) {
        pga = 100;
    }

    return wave_player_apply_rec_mic_pga(pga);
}

int wave_player_set_record_pga(uint8_t pga)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_set_record_pga_internal(pga);
    wave_player_legacy_leave();
    return result;
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
static int wave_player_rec_set_callback_internal(wave_player_rec_cb_t cb, void *user_data)
{
    taskENTER_CRITICAL();
    g_audio.rec_cb = cb;
    g_audio.rec_cb_user_data = user_data;
    taskEXIT_CRITICAL();
    return 0;
}

int wave_player_rec_set_callback(wave_player_rec_cb_t cb, void *user_data)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_rec_set_callback_internal(cb, user_data);
    wave_player_legacy_leave();
    return result;
}

static int wave_record_enable_internal(bool enable)
{
    bool changed = (g_audio.rec_enable != enable);
    g_audio.rec_enable = enable;

    if (changed && g_audio.io_started && g_audio.logical_tx_block_bytes > 0U && g_audio.src_hz > 0U) {
        wave_player_config(g_audio.src_hz, g_audio.logical_tx_block_bytes, g_audio.rx_block_bytes, g_audio.src_channels);
    }

    return 0;
}

int wave_record_enable(bool enable)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_record_enable_internal(enable);
    wave_player_legacy_leave();
    return result;
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

static int wave_player_rec_start_internal(uint32_t duration_ms, uint32_t sample_rate_hz, uint8_t channels)
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
    uint32_t tx_block_bytes = WAVE_PLAYER_OUT_SAMPLES * WAVE_PLAYER_OUT_CHANNELS * sizeof(int16_t);
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

int wave_player_rec_start(uint32_t duration_ms, uint32_t sample_rate_hz, uint8_t channels)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_rec_start_internal(duration_ms, sample_rate_hz, channels);
    wave_player_legacy_leave();
    return result;
}

static int wave_player_rec_stop_internal(void)
{
    if (!g_audio.rec_enable) {
        return 0;
    }

    g_audio.rec_stop = true;
    return 0;
}

int wave_player_rec_stop(void)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_rec_stop_internal();
    wave_player_legacy_leave();
    return result;
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

static int cmd_rec_play_internal(int argc, char **argv)
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

    uint32_t sample_rate = (g_audio.cap_hz > 0U) ? (uint32_t)g_audio.cap_hz : 16000U;
    const uint8_t channels = 1U;
    if (g_audio.cap_hz == 0U) {
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
    bool prev_rec_enable = wave_record_is_enabled();
    wave_player_stop();
    wave_record_enable(false);
    return wave_player_play_one_wave((const uint8_t *)g_audio.rec_mono_buf, out_samples * (uint32_t)sizeof(int16_t),
                                     sample_rate, (uint8_t)channels, true, prev_rec_enable);
#endif
}

static int cmd_rec_play(int argc, char **argv)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = cmd_rec_play_internal(argc, argv);
    wave_player_legacy_leave();
    return result;
}

static int cmd_rec_dump_flash(int argc, char **argv)
{
#if !WAVE_PLAYER_ENABLE_REC_RB
    (void)argc;
    (void)argv;
    printf("rec_dump_flash: disabled (WAVE_PLAYER_ENABLE_REC_RB=0)\r\n");
    return -1;
#else
    uint32_t flash_addr = WAVE_PLAYER_REC_DUMP_FLASH_ADDR_DEFAULT;
    uint32_t max_bytes = WAVE_PLAYER_REC_DUMP_FLASH_MEDIA_SIZE_DEFAULT;

    if (argc >= 2) {
        flash_addr = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        max_bytes = (uint32_t)strtoul(argv[2], NULL, 0);
    }
    if (argc > 3) {
        printf("usage: rec_dump_flash [flash_addr] [max_bytes]\r\n");
        printf("default: rec_dump_flash 0x%08lx 0x%08lx\r\n",
               (unsigned long)WAVE_PLAYER_REC_DUMP_FLASH_ADDR_DEFAULT,
               (unsigned long)WAVE_PLAYER_REC_DUMP_FLASH_MEDIA_SIZE_DEFAULT);
        return -1;
    }

    return wave_player_rec_dump_flash(flash_addr, max_bytes);
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
    printf("audio: playing=%u play=%s cap=%s src_hz=%u play_hz=%u cap_hz=%u out_ch=%u cap_ch=%u tx=%u/%u slot=%ux%u volume=%u mute=%u\r\n",
           (g_audio.wave_task != NULL) ? 1U : 0U, wave_player_backend_name(g_audio.playback_backend),
           wave_player_backend_name(g_audio.capture_backend), g_audio.src_hz, g_audio.out_hz, g_audio.cap_hz,
           g_audio.out_channels, g_audio.cap_channels, g_audio.logical_tx_block_bytes, g_audio.tx_block_bytes,
           g_audio.tx_slot_count, g_audio.tx_slot_word_bytes, g_audio.volume, g_audio.muted ? 1U : 0U);
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
#if !WAVE_PLAYER_DIAG
    (void)argc;
    (void)argv;
    printf("audio_diag: disabled (CONFIG_WAVE_PLAYER_DIAG not enabled)\r\n");
    return 0;
#else
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
    printf("audio_diag: tx_refill_miss=%lu, tx_refill_isr_delta_max=%lu\r\n",
           (unsigned long)g_wave_diag.tx_refill_miss_cnt, (unsigned long)g_wave_diag.tx_refill_isr_delta_max);
    printf("audio_diag: tx_rb_under=%lu (%lu bytes), tx_resample_pad=%lu (%lu bytes)\r\n",
           (unsigned long)g_wave_diag.tx_rb_under_cnt, (unsigned long)g_wave_diag.tx_rb_under_bytes,
           (unsigned long)g_wave_diag.tx_resample_pad_cnt, (unsigned long)g_wave_diag.tx_resample_pad_bytes);
    printf("audio_diag: tx_hw_fifo_under=%lu, tx_hw_fifo_over=%lu\r\n",
           (unsigned long)g_wave_diag.tx_hw_fifo_under_cnt, (unsigned long)g_wave_diag.tx_hw_fifo_over_cnt);
    printf("audio_diag: rx_isr=%lu, rx_q_ok=%lu, rx_q_fail=%lu, rx_q_depth_max=%lu\r\n",
           (unsigned long)g_wave_diag.rx_isr_cnt, (unsigned long)g_wave_diag.rx_q_send_ok,
           (unsigned long)g_wave_diag.rx_q_send_fail, (unsigned long)g_wave_diag.rx_q_depth_max);
    printf("audio_diag: rx_msg_lag_max=%lu, rx_overrun_risk=%lu\r\n", (unsigned long)g_wave_diag.rx_msg_lag_max,
           (unsigned long)g_wave_diag.rx_msg_overrun_risk_cnt);
    return 0;
#endif
}

#if WAVE_PLAYER_CODEC_REG_CLI
static int cmd_codec_reg(int argc, char **argv)
{
    bool capture = false;
    bool is_write = false;
    audio_codec_dev_t *codec;
    uint32_t reg;
    uint32_t value = 0U;
    uint8_t width;
    int ret;

    if (argc < 4) {
        wave_player_codec_cli_usage();
        return 0;
    }

    if (strcmp(argv[1], "r") == 0 || strcmp(argv[1], "read") == 0) {
        is_write = false;
    } else if (strcmp(argv[1], "w") == 0 || strcmp(argv[1], "write") == 0) {
        is_write = true;
    } else {
        wave_player_codec_cli_usage();
        return -1;
    }

    if (wave_player_codec_cli_parse_role(argv[2], &capture) != 0) {
        wave_player_codec_cli_usage();
        return -1;
    }

    codec = wave_player_codec_cli_get_codec(capture);
    if (codec == NULL || codec->drv == NULL) {
        printf("codec_reg: %s codec not initialized\r\n", wave_player_codec_cli_role_name(capture));
        return -1;
    }

    reg = (uint32_t)strtoul(argv[3], NULL, 0);
    width = (codec->type == AUDIO_CODEC_TYPE_BL) ? 32U : 8U;

    if (is_write) {
        if (argc < 5 || argc > 6) {
            wave_player_codec_cli_usage();
            return -1;
        }
        value = (uint32_t)strtoul(argv[4], NULL, 0);
        if (argc == 6) {
            width = (uint8_t)strtoul(argv[5], NULL, 0);
        }
        ret = wave_player_codec_cli_write(capture, reg, value, width);
        if (ret != 0) {
            printf("codec_reg: write failed, role=%s codec=%s type=%u reg=0x%lx width=%u ret=%d\r\n",
                   wave_player_codec_cli_role_name(capture), codec->name != NULL ? codec->name : "unknown",
                   (unsigned)codec->type, (unsigned long)reg, (unsigned)width, ret);
            return ret;
        }
        printf("codec_reg: write ok, role=%s codec=%s type=%u reg=0x%lx val=0x%lx width=%u\r\n",
               wave_player_codec_cli_role_name(capture), codec->name != NULL ? codec->name : "unknown",
               (unsigned)codec->type, (unsigned long)reg, (unsigned long)value, (unsigned)width);
        return 0;
    }

    if (argc > 5) {
        wave_player_codec_cli_usage();
        return -1;
    }
    if (argc == 5) {
        width = (uint8_t)strtoul(argv[4], NULL, 0);
    }

    ret = wave_player_codec_cli_read(capture, reg, width, &value);
    if (ret != 0) {
        printf("codec_reg: read failed, role=%s codec=%s type=%u reg=0x%lx width=%u ret=%d\r\n",
               wave_player_codec_cli_role_name(capture), codec->name != NULL ? codec->name : "unknown",
               (unsigned)codec->type, (unsigned long)reg, (unsigned)width, ret);
        return ret;
    }

    if (codec->type == AUDIO_CODEC_TYPE_BL) {
        struct bflb_device_s *dev = wave_player_codec_cli_get_builtin_dev(capture);
        printf("codec_reg: role=%s codec=%s base=0x%08lx reg=0x%lx addr=0x%08lx width=%u val=0x%lx\r\n",
               wave_player_codec_cli_role_name(capture), codec->name != NULL ? codec->name : "unknown",
               (unsigned long)((dev != NULL) ? dev->reg_base : 0U), (unsigned long)reg,
               (unsigned long)(((dev != NULL) ? dev->reg_base : 0U) + reg), (unsigned)width, (unsigned long)value);
    } else {
        printf("codec_reg: role=%s codec=%s i2c=0x%02x reg=0x%02lx val=0x%02lx\r\n",
               wave_player_codec_cli_role_name(capture), codec->name != NULL ? codec->name : "unknown",
               (unsigned)codec->i2c_addr, (unsigned long)reg, (unsigned long)value);
    }

    return 0;
}
#endif

#if WAVE_PLAYER_ENABLE_REC
static int cmd_rec_vol(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: rec_vol <0-100>\r\n");
        return 0;
    }

    unsigned long vol = strtoul(argv[1], NULL, 0);
    if (vol > 100U) {
        vol = 100U;
    }

    int ret = wave_player_set_record_volume((uint8_t)vol);
    if (ret == 0) {
        printf("rec_vol set to %u\r\n", (unsigned)vol);
    } else {
        printf("rec_vol set failed: %d\r\n", ret);
    }
    return ret;
}

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

    int ret = wave_player_set_record_pga((uint8_t)pga);
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

#if WAVE_PLAYER_TEST_WAVE || (WAVE_PLAYER_ENABLE_REC && WAVE_PLAYER_ENABLE_REC_RB)
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
        WAVE_PLAYER_DIAG_NOTE_TX_SOURCE_WRITE(wrote);
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
    bool restore_rec_enable = false;
    bool rec_enable_after_done = false;
#if WAVE_PLAYER_ENABLE_REC
    restore_rec_enable = cfg->restore_rec_enable;
    rec_enable_after_done = cfg->rec_enable_after_done;
#endif
    vPortFree(cfg);
    wave_player_stop();
#if WAVE_PLAYER_ENABLE_REC
    if (restore_rec_enable) {
        wave_record_enable(rec_enable_after_done);
    }
#endif
    taskENTER_CRITICAL();
    g_audio.wave_src_task = NULL;
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}
#endif

#if WAVE_PLAYER_TEST_WAVE || (WAVE_PLAYER_ENABLE_REC && WAVE_PLAYER_ENABLE_REC_RB)
static int wave_player_play_one_wave(const uint8_t *src, size_t src_len, uint32_t sample_rate_hz, uint8_t src_channels,
                                     bool restore_rec_enable, bool rec_enable_after_done)
{
    if (src == NULL || src_len == 0) {
        printf("play_one_wave: empty src\r\n");
        return -1;
    }

    uint32_t tx_block_bytes = WAVE_PLAYER_OUT_SAMPLES * WAVE_PLAYER_OUT_CHANNELS * sizeof(int16_t);
    wave_player_config(sample_rate_hz, tx_block_bytes, tx_block_bytes, src_channels);
    if (wave_player_start() != 0) {
#if WAVE_PLAYER_ENABLE_REC
        if (restore_rec_enable) {
            wave_record_enable(rec_enable_after_done);
        }
#endif
        return -1;
    }

    wave_src_task_cfg_t *src_cfg = (wave_src_task_cfg_t *)pvPortMalloc(sizeof(*src_cfg));
    if (src_cfg == NULL) {
        printf("play_one_wave: no mem for src_cfg\r\n");
        wave_player_stop();
#if WAVE_PLAYER_ENABLE_REC
        if (restore_rec_enable) {
            wave_record_enable(rec_enable_after_done);
        }
#endif
        return -1;
    }
    *src_cfg = (wave_src_task_cfg_t){
        .src = src,
        .src_len = src_len,
        .rb = &g_audio.wave_rb,
        .sem_wave_rb_space = g_audio.sem_wave_rb_space,
#if WAVE_PLAYER_ENABLE_REC
        .restore_rec_enable = restore_rec_enable,
        .rec_enable_after_done = rec_enable_after_done,
#endif
    };
    if (xTaskCreate(wave_src_sink, (char *)"wave_src", 256, src_cfg, 2, &g_audio.wave_src_task) != pdPASS) {
        printf("play_one_wave: create src task failed\r\n");
        vPortFree(src_cfg);
        wave_player_stop();
#if WAVE_PLAYER_ENABLE_REC
        if (restore_rec_enable) {
            wave_record_enable(rec_enable_after_done);
        }
#endif
        return -1;
    }
    return 0;
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

static int wave_player_play_wave_buffer_internal(const uint8_t *buf, size_t len)
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
    return wave_player_play_one_wave(data, data_size, sample_rate, (uint8_t)channels, false, false);
}

static int wave_player_play_wave_buffer(const uint8_t *buf, size_t len)
{
    if (!wave_player_legacy_enter()) {
        return -EBUSY;
    }
    int result = wave_player_play_wave_buffer_internal(buf, len);
    wave_player_legacy_leave();
    return result;
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
#if WAVE_PLAYER_CODEC_REG_CLI
SHELL_CMD_EXPORT_ALIAS(cmd_codec_reg, codec_reg, "read/write codec registers: codec_reg <r|w> <play|cap> <reg> [value] [width]");
#endif
#if WAVE_PLAYER_ENABLE_REC
SHELL_CMD_EXPORT_ALIAS(cmd_rec_start, rec_start, "start recording: rec_start <duration_ms> <sample_rate> <channels>");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_stop, rec_stop, "stop recording");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_play, rec_play, "play recording: rec_play");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_dump_flash, rec_dump_flash, "dump last record buffer to flash: rec_dump_flash [flash_addr] [max_bytes]");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_vol, rec_vol, "set record digital volume: rec_vol <0-100>");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_pga, rec_pga, "set record mic pga: rec_pga <0-100>");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_en, rec_en, "enable/disable record path: rec_en <0|1>");
#endif
#if WAVE_PLAYER_TEST_WAVE
SHELL_CMD_EXPORT_ALIAS(cmd_audio_play0, audio_play0, "play embedded wave buffer 0");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_play1, audio_play1, "play embedded wave buffer 1");
#endif

#if defined(CONFIG_BSP_AUDIO_PCM)
/* The PCM device shares the existing DMA engine. Application-specific queues
 * and read deadlines belong to its consumer, not to this hardware owner. */
static int audio_pcm_lock(void)
{
    if (g_pcm.lock == NULL) {
        return -ENODEV;
    }
    return xSemaphoreTake(g_pcm.lock, portMAX_DELAY) == pdTRUE ? 0 : -EIO;
}

static int audio_pcm_lock_until(uint64_t started, uint32_t timeout_ms)
{
    if (g_pcm.lock == NULL) {
        return -ENODEV;
    }
    uint64_t elapsed = (bflb_mtimer_get_time_us() - started) / 1000U;
    uint32_t remaining = elapsed < timeout_ms ? timeout_ms - (uint32_t)elapsed : 0;
    TickType_t ticks = pdMS_TO_TICKS(remaining);
    if (remaining != 0U && ticks == 0U) {
        ticks = 1;
    }
    return xSemaphoreTake(g_pcm.lock, ticks) == pdTRUE ? 0 : -ETIMEDOUT;
}

static int audio_pcm_try_lock(void)
{
    if (g_pcm.lock == NULL) {
        return -ENODEV;
    }
    return xSemaphoreTake(g_pcm.lock, 0) == pdTRUE ? 0 : -EAGAIN;
}

static void audio_pcm_unlock(void)
{
    (void)xSemaphoreGive(g_pcm.lock);
}

static bool audio_pcm_session_valid(audio_pcm_handle_t session)
{
    return session == &g_pcm.sessions[AUDIO_PCM_PLAYBACK] || session == &g_pcm.sessions[AUDIO_PCM_CAPTURE];
}

static bool audio_pcm_session_live(audio_pcm_handle_t session)
{
    return audio_pcm_session_valid(session) && session->used && !session->closing && !session->aborted;
}

static bool audio_pcm_keep_capture(void)
{
    bool keep;
    taskENTER_CRITICAL();
    keep = g_pcm.claimed && audio_pcm_session_live(&g_pcm.sessions[AUDIO_PCM_CAPTURE]);
    taskEXIT_CRITICAL();
    return keep;
}

#if WAVE_PLAYER_ENABLE_REC
static bool audio_pcm_capture_prepared(void)
{
    return g_pcm.prepare_capture;
}
#endif

static void audio_pcm_player_cleanup(void)
{
    if (!audio_pcm_keep_capture()) {
        audio_io_stop();
    }
}

static bool audio_pcm_player_wait_reference(void)
{
    if (!audio_pcm_owned()) {
        return true;
    }
    for (;;) {
        taskENTER_CRITICAL();
        uint32_t users = g_pcm.sessions[AUDIO_PCM_CAPTURE].playback_callback_users;
        bool stopped = g_audio.stop;
        taskEXIT_CRITICAL();
        if (stopped || users == 0U) {
            return !stopped;
        }
        /* Writer reservation blocks new silence. Let the capture task finish
         * its admitted reference before this task publishes either prime. */
        vTaskDelay(1);
    }
}

static void audio_pcm_player_started(void)
{
    if (audio_pcm_owned()) {
        if (g_pcm.tx_resume_pending) {
            /* Linking a ring alone does not reset a stopped channel's current
             * source, destination or remaining transfer count. Reload the
             * primed ping/pong blocks without disturbing capture DMA. */
            int used = wave_player_reload_tx_ring(g_audio.tx_block_bytes);
            if (used <= 0) {
                g_pcm.player_start_result = -EIO;
                g_audio.stop = true;
                return;
            }
            g_audio.tx_lli_count = (uint32_t)used;
            bflb_dma_channel_lli_link_head(g_audio.dma_ch_tx, g_audio.tx_llipool, g_audio.tx_lli_count);
            bflb_dma_channel_tcint_clear(g_audio.dma_ch_tx);
            bflb_dma_channel_start(g_audio.dma_ch_tx);
            audio_io_resume();
            g_pcm.tx_resume_pending = false;
        }
        g_pcm.player_start_result = 0;
    }
}

static void audio_pcm_player_failed(void)
{
    if (audio_pcm_owned() && !g_audio.stop) {
        g_pcm.player_start_result = -EIO;
    }
}

static void audio_pcm_prepare_playback(int16_t *samples, size_t bytes)
{
    bool silence;
    taskENTER_CRITICAL();
    silence = g_pcm.claimed && (!audio_pcm_session_live(&g_pcm.sessions[AUDIO_PCM_PLAYBACK]) || g_audio.muted);
    taskEXIT_CRITICAL();
    if (silence) {
        memset(samples, 0, bytes);
    }
}

static bool audio_pcm_same_name(const char *a, const char *b)
{
    return a == b || (a != NULL && b != NULL && strcmp(a, b) == 0);
}

static bool audio_pcm_same_hardware(const wave_player_hw_cfg_t *a, const wave_player_hw_cfg_t *b)
{
    return audio_pcm_same_name(a->i2s_dev_name, b->i2s_dev_name) &&
           audio_pcm_same_name(a->i2c_dev_name, b->i2c_dev_name) &&
           audio_pcm_same_name(a->dma_tx_dev_name, b->dma_tx_dev_name) &&
           audio_pcm_same_name(a->dma_rx_dev_name, b->dma_rx_dev_name) &&
           audio_pcm_same_name(a->audac_dev_name, b->audac_dev_name) &&
           audio_pcm_same_name(a->auadc_dev_name, b->auadc_dev_name) && a->playback_backend == b->playback_backend &&
           a->capture_backend == b->capture_backend && a->required_ext_codec_type == b->required_ext_codec_type &&
           a->playback_task_priority == b->playback_task_priority &&
           a->capture_task_priority == b->capture_task_priority &&
           a->capture_layout.mic_mask == b->capture_layout.mic_mask &&
           a->capture_layout.ref_mask == b->capture_layout.ref_mask;
}

static int audio_pcm_retain_hardware(const wave_player_hw_cfg_t *hardware)
{
    const char *names[] = {
        hardware->i2s_dev_name,    hardware->i2c_dev_name,   hardware->dma_tx_dev_name,
        hardware->dma_rx_dev_name, hardware->audac_dev_name, hardware->auadc_dev_name,
    };
    const char **retained[] = {
        &g_pcm.hardware.i2s_dev_name,    &g_pcm.hardware.i2c_dev_name,   &g_pcm.hardware.dma_tx_dev_name,
        &g_pcm.hardware.dma_rx_dev_name, &g_pcm.hardware.audac_dev_name, &g_pcm.hardware.auadc_dev_name,
    };
    for (unsigned i = 0; i < 6; ++i) {
        if (names[i] == NULL) {
            continue;
        }
        size_t length = 0;
        while (length < sizeof(g_pcm.hardware_names[i]) && names[i][length] != '\0') {
            ++length;
        }
        if (length == sizeof(g_pcm.hardware_names[i])) {
            return -ENAMETOOLONG;
        }
    }
    g_pcm.hardware = *hardware;
    for (unsigned i = 0; i < 6; ++i) {
        if (names[i] != NULL) {
            strcpy(g_pcm.hardware_names[i], names[i]);
            *retained[i] = g_pcm.hardware_names[i];
        }
    }
    return 0;
}

static int audio_pcm_control_result(int result)
{
    if (result == -2) {
        return -ENOTSUP;
    }
    return result == 0 ? 0 : -EIO;
}

static int audio_pcm_apply_controls(void)
{
    int result = audio_pcm_control_result(wave_player_set_volume_internal(g_pcm.requested_volume));
    if (result) {
        return result;
    }
    result = audio_codec_set_mute(&g_audio.play_codec, g_pcm.requested_mute);
    if (result != 0 && result != -2) {
        return -EIO;
    }
    /* A codec without a mute operation uses PCM silence in the worker. */
    g_audio.muted = g_pcm.requested_mute;
    if (g_pcm.record_volume_set) {
        result = audio_pcm_control_result(wave_player_set_record_volume_internal(g_pcm.requested_record_volume));
        if (result) {
            return result;
        }
    }
    if (g_pcm.record_pga_set) {
        result = audio_pcm_control_result(wave_player_set_record_pga_internal(g_pcm.requested_record_pga));
    }
    return result;
}

int audio_pcm_init(const wave_player_hw_cfg_t *hardware)
{
    if (hardware == NULL) {
        return -EINVAL;
    }
    if (g_pcm.lock == NULL) {
        SemaphoreHandle_t candidate = xSemaphoreCreateMutex();
        if (candidate == NULL) {
            return -ENOMEM;
        }
        taskENTER_CRITICAL();
        if (g_pcm.lock == NULL) {
            g_pcm.lock = candidate;
            candidate = NULL;
        }
        taskEXIT_CRITICAL();
        if (candidate != NULL) {
            vSemaphoreDelete(candidate);
        }
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    if (g_pcm.initialized) {
        result = audio_pcm_same_hardware(hardware, &g_pcm.hardware) ? 0 : -EBUSY;
    } else if (g_audio.sem_tx_dma_done != NULL) {
        result = -EBUSY;
    } else {
        result = audio_pcm_retain_hardware(hardware);
        if (!result) {
            result = wave_player_init(&g_pcm.hardware);
            if (result) {
                result = -EIO;
            }
        }
        if (!result) {
            g_pcm.initialized = true;
            g_pcm.requested_mute = g_audio.muted;
            g_pcm.requested_volume = g_audio.volume;
        }
    }
    audio_pcm_unlock();
    return result;
}

int audio_pcm_get_caps(audio_pcm_caps_t *caps)
{
    if (caps == NULL) {
        return -EINVAL;
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    if (!g_pcm.initialized) {
        result = -ENODEV;
    } else {
        memset(caps, 0, sizeof(*caps));
        caps->playback_supported = true;
        caps->bits_per_sample = 16;
        caps->playback_channels = wave_player_default_playback_channels();
        caps->playback_source_channels_max = 2;
        caps->fixed_sample_rate_hz = WAVE_PLAYER_FIX_SAMPLE_RATE;
        caps->playback_period_samples_max = WAVE_PLAYER_OUT_SAMPLES;
#if WAVE_PLAYER_ENABLE_REC
        caps->capture_supported = g_audio.dma_ch_rx != NULL;
        caps->capture_channels = wave_player_default_capture_channels();
        caps->capture_block_samples = WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH;
        caps->capture_layout = g_audio.capture_layout;
#endif
    }
    audio_pcm_unlock();
    return result;
}

static bool audio_pcm_rate_supported(uint32_t rate)
{
    switch (rate) {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
            return true;
        default:
            return false;
    }
}

static int audio_pcm_validate_config(audio_pcm_direction_t direction, const audio_pcm_config_t *config)
{
    if (config == NULL || (direction != AUDIO_PCM_CAPTURE && direction != AUDIO_PCM_PLAYBACK)) {
        return -EINVAL;
    }
    if (config->bits_per_sample != 16 || !audio_pcm_rate_supported(config->sample_rate_hz) ||
        config->period_samples > WAVE_PLAYER_OUT_SAMPLES) {
        return -ENOTSUP;
    }
#if WAVE_PLAYER_FIX_SAMPLE_RATE > 0
    if (config->sample_rate_hz != WAVE_PLAYER_FIX_SAMPLE_RATE) {
        return -ENOTSUP;
    }
#endif
    if (direction == AUDIO_PCM_PLAYBACK) {
        return config->channels >= 1 && config->channels <= 2 ? 0 : -ENOTSUP;
    }
#if WAVE_PLAYER_ENABLE_REC
    return g_audio.dma_ch_rx != NULL && config->channels == wave_player_default_capture_channels() ? 0 : -ENOTSUP;
#else
    return -ENOTSUP;
#endif
}

int audio_pcm_open(audio_pcm_direction_t direction, const audio_pcm_config_t *config, audio_pcm_handle_t *out)
{
    if (out == NULL) {
        return -EINVAL;
    }
    *out = NULL;
    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    if (self != NULL && (self == g_audio.wave_task
#if WAVE_PLAYER_ENABLE_REC
                         || self == g_audio.cap_task
#endif
                         )) {
        return -EDEADLK;
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    result = g_pcm.initialized ? audio_pcm_validate_config(direction, config) : -ENODEV;
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    struct audio_pcm_session *session = &g_pcm.sessions[direction];
    bool first = !g_pcm.claimed;
    if (session->used || g_pcm.sessions[0].closing || g_pcm.sessions[1].closing) {
        result = -EBUSY;
    } else if (!first && (g_audio.out_hz != config->sample_rate_hz ||
                          (config->period_samples &&
                           config->period_samples * wave_player_default_playback_channels() * sizeof(int16_t) !=
                               g_audio.logical_tx_block_bytes))) {
        result = -EBUSY;
    } else if (first && (g_audio.io_started || g_audio.wave_task != NULL || playback_frame_cb != NULL
#if WAVE_PLAYER_ENABLE_REC
                         || g_audio.rec_cb != NULL
#endif
                         )) {
        result = -EBUSY;
    }
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    if (first) {
        uint32_t period = config->period_samples ? config->period_samples : WAVE_PLAYER_OUT_SAMPLES;
        uint8_t source_channels = direction == AUDIO_PCM_PLAYBACK ? config->channels : 1U;
        taskENTER_CRITICAL();
        /* Recheck engine activity after reserving the handoff: an earlier
         * legacy operation may have finished since the initial validation. */
        if (g_pcm.legacy_operations || g_pcm.control_operations || g_audio.io_started || g_audio.wave_task != NULL ||
            playback_frame_cb != NULL
#if WAVE_PLAYER_ENABLE_REC
            || g_audio.rec_cb != NULL
#endif
#if WAVE_PLAYER_TEST_WAVE || (WAVE_PLAYER_ENABLE_REC && WAVE_PLAYER_ENABLE_REC_RB)
            || g_audio.wave_src_task != NULL
#endif
        ) {
            taskEXIT_CRITICAL();
            audio_pcm_unlock();
            return -EBUSY;
        }
        g_pcm.claimed = true;
#if WAVE_PLAYER_ENABLE_REC
        g_pcm.prepare_capture = g_audio.dma_ch_rx != NULL;
        g_audio.rec_enable = direction == AUDIO_PCM_CAPTURE;
        g_audio.rec_cb = audio_pcm_capture_dispatch;
        g_audio.rec_cb_user_data = NULL;
#endif
        playback_frame_cb = audio_pcm_playback_dispatch;
        playback_frame_user_data = NULL;
        taskEXIT_CRITICAL();
        result = wave_player_config_internal(config->sample_rate_hz,
                                             period * wave_player_default_playback_channels() * sizeof(int16_t), 0,
                                             source_channels);
        g_pcm.prepare_capture = false;
        if (!result &&
            (!g_audio.io_started || g_audio.out_hz != config->sample_rate_hz || g_audio.src_channels != source_channels
#if WAVE_PLAYER_ENABLE_REC
             || (g_audio.dma_ch_rx != NULL && g_audio.cap_hz != config->sample_rate_hz)
#endif
                 )) {
            result = -ENOTSUP;
        }
        if (!result) {
            result = audio_pcm_apply_controls();
        }
        if (result) {
            audio_io_stop();
            taskENTER_CRITICAL();
            g_pcm.claimed = false;
            playback_frame_cb = NULL;
            playback_frame_user_data = NULL;
#if WAVE_PLAYER_ENABLE_REC
            g_audio.rec_enable = false;
            g_audio.rec_cb = NULL;
            g_audio.rec_cb_user_data = NULL;
#endif
            taskEXIT_CRITICAL();
            audio_pcm_unlock();
            return result;
        }
        g_pcm.playback_next_sample = 0;
        g_pcm.playback_sequence = 0;
        g_pcm.playback_reference_valid = false;
        g_pcm.playback_reference_gap = false;
        ++g_pcm.engine_epoch;
    }
    taskENTER_CRITICAL();
    memset(session, 0, sizeof(*session));
    session->used = true;
    session->direction = direction;
    session->config = *config;
    session->generation = ++g_pcm.generation;
#if WAVE_PLAYER_ENABLE_REC
    session->open_capture_sequence = g_audio.cap_isr_seq;
    session->capture_epoch = g_audio.cap_epoch;
    if (direction == AUDIO_PCM_CAPTURE) {
        g_audio.rec_enable = true;
    }
#endif
    taskEXIT_CRITICAL();
    if (direction == AUDIO_PCM_PLAYBACK) {
        result = audio_codec_set_mute(&g_audio.play_codec, g_pcm.requested_mute);
        if (result != 0 && result != -2) {
            /* No worker has been created yet, so rollback cannot abandon an
             * asynchronously running task or invalidate an in-flight lease. */
            taskENTER_CRITICAL();
            session->used = false;
            taskEXIT_CRITICAL();
            if (first) {
                audio_io_stop();
                taskENTER_CRITICAL();
                g_pcm.claimed = false;
                playback_frame_cb = NULL;
                playback_frame_user_data = NULL;
#if WAVE_PLAYER_ENABLE_REC
                g_audio.rec_enable = false;
                g_audio.rec_cb = NULL;
                g_audio.rec_cb_user_data = NULL;
#endif
                taskEXIT_CRITICAL();
            }
            audio_pcm_unlock();
            return -EIO;
        }
        /* Capture-only operation retains a silent TX DMA clock without a
         * software player. Selecting the source channels here does not touch
         * RX DMA, its queue, or its sample timeline. */
        g_audio.src_channels = config->channels;
        g_pcm.player_start_result = 1;
        /* TX may have been running a silent ring for capture-only clocks.
         * Prime both buffers with DMA stopped so the task never writes an
         * active block; RX and the shared serial clock continue unchanged. */
        audio_io_pause();
        bflb_dma_channel_stop(g_audio.dma_ch_tx);
        bflb_dma_channel_tcint_clear(g_audio.dma_ch_tx);
        g_audio.tx_dma_completions = 0;
        g_pcm.tx_resume_pending = true;
        result = wave_player_start_internal();
        if (result) {
            taskENTER_CRITICAL();
            session->aborted = true;
            session->closing = true;
            taskEXIT_CRITICAL();
            /* Task creation failed synchronously; the existing capture
             * transport remains owned and the silent TX clock can resume. */
            if (audio_pcm_keep_capture() && g_pcm.tx_resume_pending) {
                audio_pcm_player_started();
            }
            taskENTER_CRITICAL();
            session->used = false;
            session->closing = false;
            if (!g_pcm.sessions[AUDIO_PCM_CAPTURE].used) {
                g_pcm.claimed = false;
                playback_frame_cb = NULL;
#if WAVE_PLAYER_ENABLE_REC
                g_audio.rec_enable = false;
                g_audio.rec_cb = NULL;
#endif
            }
            taskEXIT_CRITICAL();
            audio_pcm_unlock();
            return -EIO;
        }
    }
    *out = session;
    audio_pcm_unlock();
    return 0;
}

static int audio_pcm_set_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context,
                                  bool capture)
{
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    taskENTER_CRITICAL();
    if (!audio_pcm_session_live(session) || (capture && session->direction != AUDIO_PCM_CAPTURE)) {
        result = -EINVAL;
    } else if (capture ? session->capture_callback_users != 0 : session->playback_callback_users != 0) {
        result = -EBUSY;
    } else if (capture) {
        session->capture_callback = callback;
        session->capture_context = context;
    } else {
        session->playback_callback = callback;
        session->playback_context = context;
    }
    taskEXIT_CRITICAL();
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_capture_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context)
{
    return audio_pcm_set_callback(session, callback, context, true);
}

int audio_pcm_set_playback_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context)
{
    return audio_pcm_set_callback(session, callback, context, false);
}

int audio_pcm_write(audio_pcm_handle_t session, const void *samples, size_t bytes)
{
    if (samples == NULL || bytes > INT_MAX) {
        return -EINVAL;
    }
    int result = audio_pcm_try_lock();
    if (result) {
        return result;
    }
    if (!audio_pcm_session_valid(session) || !session->used || session->direction != AUDIO_PCM_PLAYBACK) {
        result = -EINVAL;
    } else if (session->aborted || session->closing) {
        result = -ECANCELED;
    } else if (bytes % (session->config.channels * sizeof(int16_t))) {
        result = -EINVAL;
    } else if (g_pcm.player_start_result < 0 || g_audio.wave_task == NULL) {
        result = -EIO;
    } else {
        uint32_t frame_bytes = session->config.channels * sizeof(int16_t);
        uint32_t capacity = Ring_Buffer_Get_Empty_Length(&g_audio.wave_rb);
        size_t writable = bytes < capacity ? bytes : capacity;
        writable -= writable % frame_bytes;
        result = wave_player_feed_internal((uint8_t *)(uintptr_t)samples, (uint32_t)writable);
    }
    audio_pcm_unlock();
    return result;
}

int audio_pcm_abort(audio_pcm_handle_t session)
{
    int result = 0;
    bool playback = false;
    taskENTER_CRITICAL();
    if (!audio_pcm_session_valid(session) || !session->used) {
        result = -EINVAL;
    } else {
        session->aborted = true;
        playback = session->direction == AUDIO_PCM_PLAYBACK;
        if (playback) {
            /* Keep the ring owner alive until the lock-free flush finishes. */
            ++session->io_users;
        }
#if WAVE_PLAYER_ENABLE_REC
        if (session->direction == AUDIO_PCM_CAPTURE) {
            g_audio.rec_enable = false;
        }
#endif
    }
    taskEXIT_CRITICAL();
    if (playback) {
        wave_player_flush_internal();
        taskENTER_CRITICAL();
        --session->io_users;
        taskEXIT_CRITICAL();
    }
    return result;
}

int audio_pcm_close(audio_pcm_handle_t session, uint32_t timeout_ms)
{
    uint64_t started = bflb_mtimer_get_time_us();
    int result = audio_pcm_lock_until(started, timeout_ms);
    if (result) {
        return result;
    }
    if (!audio_pcm_session_valid(session) || !session->used) {
        audio_pcm_unlock();
        return -EINVAL;
    }
    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    if (self == g_audio.wave_task
#if WAVE_PLAYER_ENABLE_REC
        || self == g_audio.cap_task
#endif
    ) {
        audio_pcm_unlock();
        return -EDEADLK;
    }
    uint32_t generation = session->generation;
    taskENTER_CRITICAL();
    session->closing = true;
    session->aborted = true;
#if WAVE_PLAYER_ENABLE_REC
    if (session->direction == AUDIO_PCM_CAPTURE) {
        g_audio.rec_enable = false;
    }
#endif
    taskEXIT_CRITICAL();
    audio_pcm_unlock();
    for (;;) {
        uint32_t users;
        taskENTER_CRITICAL();
        users = session->callback_users + session->io_users;
        taskEXIT_CRITICAL();
        if (!users) {
            break;
        }
        if (bflb_mtimer_get_time_us() - started >= (uint64_t)timeout_ms * 1000U) {
            return -ETIMEDOUT;
        }
        vTaskDelay(1);
    }
    result = audio_pcm_lock_until(started, timeout_ms);
    if (result) {
        return result;
    }
    if (!session->used || session->generation != generation) {
        audio_pcm_unlock();
        return -EINVAL;
    }
    if (session->direction == AUDIO_PCM_PLAYBACK) {
        wave_player_flush_internal();
        uint64_t elapsed = (bflb_mtimer_get_time_us() - started) / 1000U;
        uint32_t remaining = elapsed < timeout_ms ? timeout_ms - (uint32_t)elapsed : 0;
        result = wave_player_stop_timeout_internal(remaining);
    } else if (!g_pcm.sessions[AUDIO_PCM_PLAYBACK].used) {
        uint64_t elapsed = (bflb_mtimer_get_time_us() - started) / 1000U;
        uint32_t remaining = elapsed < timeout_ms ? timeout_ms - (uint32_t)elapsed : 0;
        result = wave_player_stop_timeout_internal(remaining);
    }
    if (!result) {
        taskENTER_CRITICAL();
        session->capture_callback = NULL;
        session->playback_callback = NULL;
        session->capture_context = NULL;
        session->playback_context = NULL;
        session->used = false;
        session->closing = false;
        if (!g_pcm.sessions[0].used && !g_pcm.sessions[1].used) {
            g_pcm.claimed = false;
            playback_frame_cb = NULL;
            playback_frame_user_data = NULL;
#if WAVE_PLAYER_ENABLE_REC
            g_audio.rec_cb = NULL;
            g_audio.rec_cb_user_data = NULL;
#endif
        }
        taskEXIT_CRITICAL();
    }
    audio_pcm_unlock();
    return result;
}

static bool audio_pcm_callback_enter(audio_pcm_handle_t session, bool capture, audio_pcm_frame_cb_t *callback,
                                     void **context)
{
    bool acquired = false;
    taskENTER_CRITICAL();
    if (audio_pcm_session_live(session)) {
        *callback = capture ? session->capture_callback : session->playback_callback;
        *context = capture ? session->capture_context : session->playback_context;
        if (*callback != NULL) {
            ++session->callback_users;
            if (capture) {
                ++session->capture_callback_users;
            } else {
                ++session->playback_callback_users;
            }
            acquired = true;
        }
    }
    taskEXIT_CRITICAL();
    return acquired;
}

static void audio_pcm_callback_leave(audio_pcm_handle_t session, bool callback, bool capture)
{
    taskENTER_CRITICAL();
    --session->callback_users;
    if (callback) {
        if (capture) {
            --session->capture_callback_users;
        } else {
            --session->playback_callback_users;
        }
    }
    taskEXIT_CRITICAL();
}

static bool audio_pcm_reference_update(audio_pcm_frame_info_t *info, bool silent)
{
    taskENTER_CRITICAL();
    if (!silent) {
        uint64_t scheduled = g_audio.tx_total_samples;
        if (!g_pcm.tx_resume_pending) {
            /* A running player's inactive refill follows the current DMA
             * block, including after coalesced completion notifications. */
            scheduled += g_audio.tx_period_samples;
        }
        info->first_sample_index = g_pcm.playback_next_sample;
        if (info->first_sample_index < scheduled) {
            info->first_sample_index = scheduled;
            info->flags |= AUDIO_PCM_FRAME_DISCONTINUITY;
        }
    }
    if (info->first_sample_index < g_pcm.playback_next_sample) {
        uint64_t overlap = g_pcm.playback_next_sample - info->first_sample_index;
        if (overlap >= info->samples_per_channel) {
            /* A queued capture descriptor can predate the final player
             * reference. Discard it instead of moving the clock backward. */
            g_pcm.playback_reference_gap = true;
            taskEXIT_CRITICAL();
            return false;
        }
        info->samples_per_channel -= (uint32_t)overlap;
        info->first_sample_index = g_pcm.playback_next_sample;
        info->flags |= AUDIO_PCM_FRAME_DISCONTINUITY;
    }
    if (info->first_sample_index > g_pcm.playback_next_sample || g_pcm.playback_reference_gap ||
        (g_pcm.playback_reference_valid && g_pcm.playback_reference_silent != silent)) {
        info->flags |= AUDIO_PCM_FRAME_DISCONTINUITY;
    }
    info->sequence = ++g_pcm.playback_sequence;
    g_pcm.playback_next_sample = info->first_sample_index + info->samples_per_channel;
    g_pcm.playback_reference_valid = true;
    g_pcm.playback_reference_silent = silent;
    g_pcm.playback_reference_gap = false;
    taskEXIT_CRITICAL();
    info->pts_us = info->first_sample_index * 1000000U / info->sample_rate_hz;
    return true;
}

#if WAVE_PLAYER_ENABLE_REC
static const int16_t audio_pcm_silence[WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH];

static void audio_pcm_capture_dispatch(const wave_player_rec_frame_t *frame, void *context)
{
    (void)context;
    struct audio_pcm_session *session = &g_pcm.sessions[AUDIO_PCM_CAPTURE];
    audio_pcm_frame_cb_t callback = NULL;
    void *callback_context = NULL;
    if (frame == NULL || frame->sample_rate_hz == 0) {
        return;
    }
    /* Admit a frame even without a data callback so that a reference-only
     * subscriber and later callbacks observe the same capture sample clock. */
    taskENTER_CRITICAL();
    uint32_t previous_sequence =
        session->capture_seen ? session->previous_capture_sequence : session->open_capture_sequence;
    uint32_t sequence_delta = frame->sequence - previous_sequence;
    bool live = audio_pcm_session_live(session) && frame->epoch == session->capture_epoch &&
                sequence_delta <= INT32_MAX && sequence_delta != 0;
    if (live) {
        ++session->callback_users;
    }
    taskEXIT_CRITICAL();
    if (!live) {
        return;
    }
    uint32_t flags = sequence_delta != 1U ? AUDIO_PCM_FRAME_DISCONTINUITY : 0;
    session->capture_sample_index = frame->first_sample_index;
    audio_pcm_frame_info_t info = {
        .sample_rate_hz = frame->sample_rate_hz,
        .samples_per_channel = frame->samples_per_channel,
        .sequence = frame->sequence,
        .epoch = g_pcm.engine_epoch,
        .flags = flags,
        .first_sample_index = session->capture_sample_index,
        .pts_us = session->capture_sample_index * 1000000U / frame->sample_rate_hz,
        .channels = frame->channels,
        .bits_per_sample = 16,
        .layout = frame->layout,
    };
    session->previous_capture_sequence = frame->sequence;
    session->capture_seen = true;
    taskENTER_CRITICAL();
    callback = session->capture_callback;
    callback_context = session->capture_context;
    if (callback != NULL) {
        ++session->capture_callback_users;
    }
    taskEXIT_CRITICAL();
    if (callback != NULL) {
        callback(frame->samples, &info, callback_context);
        taskENTER_CRITICAL();
        --session->capture_callback_users;
        taskEXIT_CRITICAL();
    }
    bool silent_reference;
    taskENTER_CRITICAL();
    /* A closing/aborted player still emits scheduled silence until its task
     * exits. Synthesize reference only after that producer is gone. */
    silent_reference = g_audio.wave_task == NULL && !g_pcm.sessions[AUDIO_PCM_PLAYBACK].used;
    callback = session->playback_callback;
    callback_context = session->playback_context;
    if (silent_reference && callback != NULL) {
        ++session->playback_callback_users;
    }
    taskEXIT_CRITICAL();
    if (silent_reference && callback != NULL && frame->samples_per_channel <= WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH) {
        info.channels = 1;
        info.layout.mic_mask = 0;
        info.layout.ref_mask = 1;
        /* Silence can be shortened safely when the capture descriptor partly
         * overlaps a reference already emitted by the stopped player. */
        if (audio_pcm_reference_update(&info, true)) {
            callback(audio_pcm_silence, &info, callback_context);
        }
    }
    if (silent_reference && callback != NULL) {
        taskENTER_CRITICAL();
        --session->playback_callback_users;
        taskEXIT_CRITICAL();
    }
    audio_pcm_callback_leave(session, false, false);
}
#endif

static void audio_pcm_playback_dispatch(const int16_t *samples, uint32_t count, uint32_t rate, uint8_t channels,
                                        void *context)
{
    (void)context;
    audio_pcm_frame_info_t info = {
        .sample_rate_hz = rate,
        .samples_per_channel = count,
        .epoch = g_pcm.engine_epoch,
        .channels = channels,
        .bits_per_sample = 16,
        .layout = { .mic_mask = 0, .ref_mask = (1U << channels) - 1U },
    };
    if (!audio_pcm_reference_update(&info, false)) {
        return;
    }
    for (unsigned i = 0; i < 2; ++i) {
        struct audio_pcm_session *session = &g_pcm.sessions[i];
        audio_pcm_frame_cb_t callback = NULL;
        void *callback_context = NULL;
        if (!audio_pcm_callback_enter(session, false, &callback, &callback_context)) {
            continue;
        }
        callback(samples, &info, callback_context);
        audio_pcm_callback_leave(session, true, false);
    }
}

uint32_t audio_pcm_buffered_bytes(audio_pcm_handle_t session)
{
    uint32_t bytes = 0;
    if (!audio_pcm_lock()) {
        if (audio_pcm_session_live(session) && session->direction == AUDIO_PCM_PLAYBACK) {
            bytes = wave_player_get_buffered_bytes();
        }
        audio_pcm_unlock();
    }
    return bytes;
}

uint32_t audio_pcm_trim_buffer(audio_pcm_handle_t session, uint32_t max_bytes)
{
    uint32_t discarded = 0;
    if (!audio_pcm_lock()) {
        if (audio_pcm_session_live(session) && session->direction == AUDIO_PCM_PLAYBACK) {
            discarded = wave_player_trim_buffer_internal(max_bytes);
        }
        audio_pcm_unlock();
    }
    return discarded;
}

int audio_pcm_flush(audio_pcm_handle_t session)
{
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    if (!audio_pcm_session_valid(session) || !session->used || session->direction != AUDIO_PCM_PLAYBACK) {
        result = -EINVAL;
    } else {
        wave_player_flush_internal();
    }
    audio_pcm_unlock();
    return result;
}

/* Global PCM controls may be used before opening a session. Reserve that
 * short hardware operation as well, without taking a legacy engine over. */
static int audio_pcm_control_enter(void)
{
    taskENTER_CRITICAL();
    if (!g_pcm.claimed &&
        (g_pcm.legacy_operations || g_audio.io_started || g_audio.wave_task != NULL || playback_frame_cb != NULL
#if WAVE_PLAYER_ENABLE_REC
         || g_audio.rec_cb != NULL
#endif
#if WAVE_PLAYER_TEST_WAVE || (WAVE_PLAYER_ENABLE_REC && WAVE_PLAYER_ENABLE_REC_RB)
         || g_audio.wave_src_task != NULL
#endif
         )) {
        taskEXIT_CRITICAL();
        return -EBUSY;
    }
    g_pcm.control_operations++;
    taskEXIT_CRITICAL();
    return 0;
}

static void audio_pcm_control_leave(void)
{
    taskENTER_CRITICAL();
    g_pcm.control_operations--;
    taskEXIT_CRITICAL();
}

int audio_pcm_set_volume(uint8_t volume)
{
    if (volume > 100U) {
        return -EINVAL;
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    result = g_pcm.initialized ? audio_pcm_control_enter() : -ENODEV;
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    uint8_t previous_volume = g_audio.volume;
    result = audio_pcm_control_result(wave_player_set_volume_internal(volume));
    if (!result) {
        g_pcm.requested_volume = volume;
    } else {
        g_audio.volume = previous_volume;
    }
    audio_pcm_control_leave();
    audio_pcm_unlock();
    return result;
}

uint8_t audio_pcm_get_volume(void)
{
    return wave_player_get_volume();
}

int audio_pcm_set_mute(bool mute)
{
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    result = g_pcm.initialized ? audio_pcm_control_enter() : -ENODEV;
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    result = audio_codec_set_mute(&g_audio.play_codec, mute);
    if (result == -2) {
        result = 0;
    } else if (result != -ENODEV) {
        result = audio_pcm_control_result(result);
    }
    if (!result) {
        g_pcm.requested_mute = mute;
        g_audio.muted = mute;
    }
    audio_pcm_control_leave();
    audio_pcm_unlock();
    return result;
}

bool audio_pcm_get_mute(void)
{
    return wave_player_get_mute();
}

int audio_pcm_set_record_volume(uint8_t volume)
{
    if (volume > 100U) {
        return -EINVAL;
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    result = g_pcm.initialized ? audio_pcm_control_enter() : -ENODEV;
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    result = audio_pcm_control_result(wave_player_set_record_volume_internal(volume));
    if (!result) {
        g_pcm.requested_record_volume = volume;
        g_pcm.record_volume_set = true;
    }
    audio_pcm_control_leave();
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_record_pga(uint8_t pga)
{
    if (pga > 100U) {
        return -EINVAL;
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    result = g_pcm.initialized ? audio_pcm_control_enter() : -ENODEV;
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    result = audio_pcm_control_result(wave_player_set_record_pga_internal(pga));
    if (!result) {
        g_pcm.requested_record_pga = pga;
        g_pcm.record_pga_set = true;
    }
    audio_pcm_control_leave();
    audio_pcm_unlock();
    return result;
}
#endif
