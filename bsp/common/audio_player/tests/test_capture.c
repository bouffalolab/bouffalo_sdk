/* Production capture task: slot ownership, roles, gates and raw-slot ring storage. */
#include "wave_player.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define taskENTER_CRITICAL() ((void)0)
#define taskEXIT_CRITICAL()  ((void)0)
#define WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH 16U
#define WAVE_PLAYER_REC_MAX_FRAMES 7U
#define portMAX_DELAY UINT32_MAX
#define pdTRUE 1
#define pdMS_TO_TICKS(ms) (ms)
#define WAVE_PLAYER_DIAG_NOTE_RX_LAG(lag) ((void)(lag))
typedef struct {
    uint32_t block_idx;
    uint32_t nbytes, hz, samples_per_ch;
    uint8_t channels;
    uint32_t seq;
    uint32_t epoch;
    uint64_t first_sample_index;
} cap_msg_t;
static struct {
    void *cap_queue;
    int16_t cap_blocks[WAVE_PLAYER_CAP_DMA_BLOCKS][16 * WAVE_PLAYER_REC_CHANNELS];
    uint32_t cap_isr_seq;
    uint32_t cap_epoch;
    bool rec_enable;
    bool rec_path_enabled;
    bool rec_callback_active;
    wave_player_capture_layout_t capture_layout;
    wave_player_rec_cb_t rec_cb;
    void *rec_cb_user_data;
    wave_player_rec_stop_cb_t rec_stop_cb;
    void *rec_stop_cb_user_data;
#if WAVE_PLAYER_ENABLE_REC_RB
    uint32_t rec_write;
    bool rec_filled;
    uint32_t rec_capacity;
    uint32_t rec_sample_rate_hz;
    uint8_t rec_channels;
#endif
} g_audio;
#if WAVE_PLAYER_ENABLE_REC_RB
static int16_t g_rec_pcm[WAVE_PLAYER_REC_MAX_FRAMES * WAVE_PLAYER_REC_CHANNELS];
#endif
static cap_msg_t message;
static uint16_t mic_muted_mask;

uint16_t wave_player_get_mic_mute_mask(void)
{
    return mic_muted_mask;
}
static jmp_buf done;
static unsigned reads, calls;
static bool stop_on_invalidate;
static const int context;

static int xQueueReceive(void *queue, void *dst, uint32_t wait)
{
    assert(queue == g_audio.cap_queue && wait == portMAX_DELAY);
    if (reads++) longjmp(done, 1);
    *(cap_msg_t *)dst = message;
    return pdTRUE;
}
static void vTaskDelay(unsigned delay) { (void)delay; assert(0); }
static void bflb_l1c_dcache_invalidate_range(void *ptr, uint32_t bytes)
{
    assert(ptr == g_audio.cap_blocks[message.block_idx] && bytes == message.nbytes);
    if (stop_on_invalidate) {
        g_audio.rec_path_enabled = false;
        ++g_audio.cap_epoch;
        stop_on_invalidate = false;
    }
}
#include "wave_player_capture_under_test.h"

static void capture(const wave_player_rec_frame_t *frame, void *ctx)
{
    assert(g_audio.rec_callback_active);
    assert(ctx == &context && frame->sample_rate_hz == 16000);
    assert(frame->channels == WAVE_PLAYER_REC_CHANNELS && frame->samples_per_channel == 9);
    assert(frame->samples == g_audio.cap_blocks[message.block_idx]);
    assert(frame->muted_mic_mask == mic_muted_mask);
    assert(frame->layout.mic_mask == g_audio.capture_layout.mic_mask);
    assert(frame->layout.ref_mask == g_audio.capture_layout.ref_mask);
    for (unsigned i = 0; i < 9; i++) {
        for (unsigned c = 0; c < frame->channels; c++) {
            int16_t original = (int16_t)(1000 * c + 3 * i - 50);
            assert(frame->samples[i * frame->channels + c] == original);
            assert(g_audio.cap_blocks[message.block_idx][i * frame->channels + c] == original);
        }
    }
    calls++;
}
static void run(void)
{
    reads = calls = 0;
    if (!setjmp(done)) audio_cap_proc(NULL);
    assert(!g_audio.rec_callback_active);
}
#if WAVE_PLAYER_ENABLE_REC_RB
static void check_raw_ring(void)
{
    assert(g_audio.rec_capacity == 7 && g_audio.rec_sample_rate_hz == 16000);
    assert(g_audio.rec_channels == WAVE_PLAYER_REC_CHANNELS);
    assert(g_audio.rec_filled && g_audio.rec_write == 2);
    for (unsigned i = 2; i < 9; i++) {
        for (unsigned c = 0; c < WAVE_PLAYER_REC_CHANNELS; c++) {
            int16_t expected = mic_muted_mask & (1U << c) ?
                                   0 : (int16_t)(1000 * c + 3 * i - 50);
            assert(g_rec_pcm[(i % 7) * WAVE_PLAYER_REC_CHANNELS + c] == expected);
        }
    }
}
#endif
int main(void)
{
    const uint32_t mic_mask = WAVE_PLAYER_REC_CHANNELS == 4 ? 0x0c :
                              WAVE_PLAYER_REC_CHANNELS == 2 ? 0x02 : 0x01;
    g_audio.capture_layout = (wave_player_capture_layout_t){mic_mask, WAVE_PLAYER_REC_CHANNELS > 1 ? 1U : 0U};
    message = (cap_msg_t){.nbytes = 9 * WAVE_PLAYER_REC_CHANNELS * sizeof(int16_t),
        .hz = 16000, .samples_per_ch = 9, .channels = WAVE_PLAYER_REC_CHANNELS, .seq = 1};
    g_audio.cap_queue = &message;
    g_audio.cap_isr_seq = 1;
    g_audio.rec_enable = true;
    g_audio.rec_path_enabled = true;
    for (unsigned i = 0; i < 9; i++)
        for (unsigned c = 0; c < WAVE_PLAYER_REC_CHANNELS; c++)
            g_audio.cap_blocks[0][i * WAVE_PLAYER_REC_CHANNELS + c] = (int16_t)(1000 * c + 3 * i - 50);
    assert(wave_player_rec_set_callback(capture, (void *)&context) == 0);
    run();
    assert(calls == 1);
#if WAVE_PLAYER_ENABLE_REC_RB
    check_raw_ring();
#endif
    /* Raw callbacks retain DMA samples and expose the mute mask; owned ring
     * storage zeros microphone slots without touching reference or DMA PCM. */
    mic_muted_mask = mic_mask;
#if WAVE_PLAYER_ENABLE_REC_RB
    g_audio.rec_write = 0;
    g_audio.rec_filled = false;
#endif
    run();
    assert(calls == 1);
#if WAVE_PLAYER_ENABLE_REC_RB
    check_raw_ring();
#endif
    if ((mic_mask & (mic_mask - 1U)) != 0U) {
        mic_muted_mask = (uint16_t)(mic_mask & (0U - mic_mask));
#if WAVE_PLAYER_ENABLE_REC_RB
        g_audio.rec_write = 0;
        g_audio.rec_filled = false;
#endif
        run();
        assert(calls == 1);
#if WAVE_PLAYER_ENABLE_REC_RB
        check_raw_ring();
#endif
    }
    mic_muted_mask = 0;
#if WAVE_PLAYER_ENABLE_REC_RB
    g_audio.rec_write = 0;
    g_audio.rec_filled = false;
#endif
    run();
    assert(calls == 1);
#if WAVE_PLAYER_ENABLE_REC_RB
    check_raw_ring();
#endif
    wave_player_rec_set_callback(NULL, NULL);
    run();
    assert(calls == 0);
    wave_player_rec_set_callback(capture, (void *)&context);
    g_audio.rec_enable = false;
    run();
    assert(calls == 0);
    g_audio.rec_enable = true;
    message.channels = 0;
    run(); assert(calls == 0);
    message.channels = WAVE_PLAYER_REC_CHANNELS;
    message.nbytes--;
    run(); assert(calls == 0);
    message.nbytes++;
    message.samples_per_ch = 17;
    run(); assert(calls == 0);
    message.samples_per_ch = 9;
    /* Exercise the last ring block, including indices beyond uint8_t. */
    message.block_idx = WAVE_PLAYER_CAP_DMA_BLOCKS - 1U;
    memcpy(g_audio.cap_blocks[message.block_idx], g_audio.cap_blocks[0], message.nbytes);
    run(); assert(calls == 1);
    message.block_idx = WAVE_PLAYER_CAP_DMA_BLOCKS;
    run(); assert(calls == 0);
    message.block_idx = 0;
    g_audio.capture_layout = (wave_player_capture_layout_t){0};
    /* Unknown roles do not prevent raw PCM delivery or per-slot storage. */
#if WAVE_PLAYER_ENABLE_REC_RB
    g_audio.rec_write = 0;
    g_audio.rec_filled = false;
    memset(g_rec_pcm, 0, sizeof(g_rec_pcm));
#endif
    run(); assert(calls == 1);
#if WAVE_PLAYER_ENABLE_REC_RB
    check_raw_ring();
#endif
    g_audio.cap_isr_seq = message.seq + WAVE_PLAYER_CAP_DMA_BLOCKS - 2U;
    run(); assert(calls == 1); /* Last completed buffer not yet being reused. */
    g_audio.cap_isr_seq++;
    run(); assert(calls == 0); /* DMA is already writing this block again. */
    message.seq = UINT32_MAX - 1U;
    g_audio.cap_isr_seq = message.seq + WAVE_PLAYER_CAP_DMA_BLOCKS - 2U;
    run(); assert(calls == 1); /* Sequence arithmetic survives wrap. */
    g_audio.cap_isr_seq++;
    run(); assert(calls == 0);
    /* Stop between descriptor validation and callback dispatch rejects old PCM. */
    g_audio.cap_isr_seq = message.seq;
    stop_on_invalidate = true;
    run(); assert(calls == 0);
    assert(!g_audio.rec_path_enabled && g_audio.cap_epoch != message.epoch);
    g_audio.rec_path_enabled = true;
    message.epoch = g_audio.cap_epoch;
    run(); assert(calls == 1);
    puts("Capture slots, role metadata, stale DMA rejection, gates and raw-slot ring passed");
}
