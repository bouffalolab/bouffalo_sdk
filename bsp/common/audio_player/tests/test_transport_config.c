/* SPDX-License-Identifier: Apache-2.0 */
/* Execute the complete production configuration function against transport probes. */
#include <stddef.h>

#include "wave_player.h"
#include "bflb_i2c.h"
#include "bflb_i2s.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH        640U
#define DMA_ADDR_I2S_TDR                          0x100U
#define DMA_ADDR_I2S_RDR                          0x104U
#define DMA_ADDR_AUDAC_TDR                        0x200U
#define DMA_ADDR_AUADC_RDR                        0x300U
#define AUDAC_CMD_PLAY_START                      1
#define AUADC_CMD_RECORD_START                    1
#define AUDAC_INTSTS_FIFO_UNDER                   4U
#define AUDAC_INTSTS_FIFO_OVER                    8U
#define WAVE_PLAYER_DIAG_SET_HW_FAULT_LAST(value) ((void)(value))
struct bflb_dma_channel_config_s {
    unsigned transport;
};
struct bflb_dma_channel_lli_pool_s {
    unsigned unused;
};
struct bflb_dma_channel_lli_transfer_s {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nbytes;
};
static struct bflb_device_s devices[5];
static struct {
    wave_player_backend_t playback_backend;
    wave_player_backend_t capture_backend;
    audio_codec_dev_t play_codec;
    audio_codec_dev_t cap_codec;
    struct bflb_device_s *i2s_dev;
    struct bflb_device_s *dma_ch_tx;
    struct bflb_device_s *dma_ch_rx;
    struct bflb_device_s *audac_dev;
    struct bflb_device_s *auadc_dev;
    struct bflb_dma_channel_lli_pool_s tx_llipool[8];
    struct bflb_dma_channel_lli_pool_s rx_llipool[32];
    int16_t pcm_ping[WAVE_PLAYER_OUT_SAMPLES * 8];
    int16_t pcm_pong[WAVE_PLAYER_OUT_SAMPLES * 8];
    int16_t cap_blocks[WAVE_PLAYER_CAP_DMA_BLOCKS][WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH * 6];
    void *cap_queue;
    void *sem_tx_dma_done;
    void *sem_rx_dma_done;
    uint32_t src_hz;
    uint32_t out_hz;
    uint32_t cap_hz;
    uint32_t tx_block_bytes;
    uint32_t logical_tx_block_bytes;
    uint32_t rx_block_bytes;
    uint32_t tx_lli_count;
    uint32_t tx_period_samples;
    uint32_t tx_dma_completions;
    uint32_t cap_block_bytes;
    uint32_t cap_samples_per_ch;
    uint32_t cap_isr_block_idx;
    uint32_t cap_isr_seq;
    uint64_t tx_total_samples;
    uint64_t cap_total_samples;
    uint8_t src_channels;
    uint8_t out_channels;
    uint8_t cap_channels;
    uint8_t tx_slot_count;
    uint8_t tx_slot_word_bytes;
    bool io_started;
    bool rec_path_enabled;
} g_audio;
static bool capture_requested;
static uint8_t external_output_channels;
static uint8_t external_capture_channels;
static unsigned i2s_calls;
static unsigned pll_calls;
static unsigned builtin_play_starts;
static unsigned builtin_capture_starts;
static unsigned dma_starts;
static bool final_i2s_tx;
static bool final_i2s_rx;
static struct bflb_i2s_config_s observed_i2s;

#include "wave_player_slots_under_test.h"
#include "wave_player_transport_helpers_under_test.h"

static bool wave_capture_enable_requested(void)
{
    return capture_requested;
}

static bool audio_pcm_capture_prepared(void)
{
    return false;
}

static void audio_io_stop(void)
{
    g_audio.io_started = false;
}

static void xQueueReset(void *queue)
{
    (void)queue;
}

static void wave_player_diag_reset(void)
{
}

static const char *wave_player_backend_name(wave_player_backend_t backend)
{
    return backend == WAVE_PLAYER_BACKEND_I2S_CODEC ? "i2s" : "builtin";
}

static void board_audio_pll_config_for_rate(uint32_t rate)
{
    assert(rate == 16000);
    ++pll_calls;
}

void bflb_i2s_init(struct bflb_device_s *device, const struct bflb_i2s_config_s *config)
{
    assert(device == g_audio.i2s_dev);
    observed_i2s = *config;
    ++i2s_calls;
}

int bflb_i2s_feature_control(struct bflb_device_s *device, int command, size_t argument)
{
    assert(device == g_audio.i2s_dev && command == I2S_CMD_CHANNEL_LR_MERGE);
    assert(argument == WAVE_PLAYER_I2S_DSP_LR_MERGE);
    return 0;
}

void bflb_i2s_link_txdma(struct bflb_device_s *device, bool enabled)
{
    assert(device == g_audio.i2s_dev);
    final_i2s_tx = enabled;
}

void bflb_i2s_link_rxdma(struct bflb_device_s *device, bool enabled)
{
    assert(device == g_audio.i2s_dev);
    final_i2s_rx = enabled;
}

static void audio_i2s_update_data_enable(bool tx, bool rx)
{
    assert(tx == final_i2s_tx && rx == final_i2s_rx);
}

static void audio_fill_dma_tx_cfg(struct bflb_dma_channel_config_s *config)
{
    config->transport = 1;
}

static void audio_fill_dma_rx_cfg(struct bflb_dma_channel_config_s *config)
{
    config->transport = 1;
}

static void audio_fill_dma_bl_tx_cfg(struct bflb_dma_channel_config_s *config)
{
    config->transport = 2;
}

static void audio_fill_dma_bl_rx_cfg(struct bflb_dma_channel_config_s *config)
{
    config->transport = 2;
}

static void bflb_dma_channel_init(struct bflb_device_s *device, const struct bflb_dma_channel_config_s *config)
{
    bool tx = device == g_audio.dma_ch_tx;
    wave_player_backend_t backend = tx ? g_audio.playback_backend : g_audio.capture_backend;
    assert(config->transport == (backend == WAVE_PLAYER_BACKEND_I2S_CODEC ? 1U : 2U));
}

static void dma_ch_tx_isr(void *argument)
{
    (void)argument;
}

static void dma_ch_rx_isr(void *argument)
{
    (void)argument;
}

static void bflb_dma_channel_irq_attach(struct bflb_device_s *device, void (*callback)(void *), void *argument)
{
    assert(callback == (device == g_audio.dma_ch_tx ? dma_ch_tx_isr : dma_ch_rx_isr) && argument == NULL);
}

static int bflb_dma_channel_lli_reload(struct bflb_device_s *device, struct bflb_dma_channel_lli_pool_s *pool,
                                       uint32_t capacity, struct bflb_dma_channel_lli_transfer_s *transfers,
                                       uint32_t count)
{
    assert(pool != NULL && count <= capacity && transfers != NULL);
    bool tx = device == g_audio.dma_ch_tx;
    wave_player_backend_t backend = tx ? g_audio.playback_backend : g_audio.capture_backend;
    uint32_t address = backend == WAVE_PLAYER_BACKEND_I2S_CODEC ? (tx ? DMA_ADDR_I2S_TDR : DMA_ADDR_I2S_RDR) :
                                                                  (tx ? DMA_ADDR_AUDAC_TDR : DMA_ADDR_AUADC_RDR);
    for (uint32_t i = 0; i < count; ++i) {
        assert((tx ? transfers[i].dst_addr : transfers[i].src_addr) == address);
        assert(transfers[i].nbytes != 0);
    }
    return (int)count;
}

static void bflb_dma_channel_lli_link_head(struct bflb_device_s *device, struct bflb_dma_channel_lli_pool_s *pool,
                                           uint32_t count)
{
    (void)device;
    assert(pool != NULL && count != 0);
}

static void bflb_l1c_dcache_clean_range(void *buffer, uint32_t bytes)
{
    assert(buffer != NULL && bytes != 0);
}

static void bflb_l1c_dcache_clean_invalidate_range(void *buffer, uint32_t bytes)
{
    assert(buffer != NULL && bytes != 0);
}

int audio_codec_set_sample_rate(audio_codec_dev_t *device, uint32_t rate)
{
    (void)device;
    assert(rate == 16000);
    return 0;
}

static void bflb_dma_channel_tcint_clear(struct bflb_device_s *device)
{
    assert(device == g_audio.dma_ch_tx || device == g_audio.dma_ch_rx);
}

static void sem_drain(void *semaphore)
{
    (void)semaphore;
}

static void bflb_dma_channel_start(struct bflb_device_s *device)
{
    assert(device == g_audio.dma_ch_tx || device == g_audio.dma_ch_rx);
    ++dma_starts;
}

static int bflb_audac_link_rxdma(struct bflb_device_s *device, bool enabled)
{
    assert(device == g_audio.audac_dev && enabled);
    return 0;
}

static int bflb_audac_feature_control(struct bflb_device_s *device, int command, size_t argument)
{
    assert(device == g_audio.audac_dev && command == AUDAC_CMD_PLAY_START && argument == 0);
    ++builtin_play_starts;
    return 0;
}

static uint32_t bflb_audac_get_intstatus(struct bflb_device_s *device)
{
    assert(device == g_audio.audac_dev);
    return 0;
}

static int bflb_auadc_link_rxdma(struct bflb_device_s *device, bool enabled)
{
    assert(device == g_audio.auadc_dev && enabled);
    return 0;
}

static int bflb_auadc_feature_control(struct bflb_device_s *device, int command, size_t argument)
{
    assert(device == g_audio.auadc_dev && command == AUADC_CMD_RECORD_START && argument == 0);
    ++builtin_capture_starts;
    return 0;
}

#include "wave_player_transport_config_under_test.h"

static void reset(wave_player_backend_t playback, wave_player_backend_t capture)
{
    memset(&g_audio, 0, sizeof(g_audio));
    i2s_calls = pll_calls = builtin_play_starts = builtin_capture_starts = dma_starts = 0;
    final_i2s_tx = final_i2s_rx = false;
    capture_requested = true;
    g_audio.playback_backend = playback;
    g_audio.capture_backend = capture;
    g_audio.i2s_dev = &devices[0];
    g_audio.dma_ch_tx = &devices[1];
    g_audio.dma_ch_rx = &devices[2];
    g_audio.audac_dev = &devices[3];
    g_audio.auadc_dev = &devices[4];
    g_audio.play_codec.type = AUDIO_CODEC_TYPE_ES8388;
    g_audio.cap_codec.type = AUDIO_CODEC_TYPE_ES8388;
}

int main(void)
{
    external_output_channels = WAVE_PLAYER_OUT_CHANNELS;
    external_capture_channels = WAVE_PLAYER_REC_CHANNELS;
    assert(external_output_channels == 1 && external_capture_channels == 3);
    reset(WAVE_PLAYER_BACKEND_BL_CODEC, WAVE_PLAYER_BACKEND_BL_CODEC);
    assert(wave_player_config_internal(16000, 320 * sizeof(int16_t), 0, 1) == 0);
    assert(g_audio.io_started && g_audio.out_channels == 1 && g_audio.cap_channels == 1);
    assert(i2s_calls == 0 && pll_calls == 0 && dma_starts == 2);
    assert(builtin_play_starts == 1 && builtin_capture_starts == 1 && g_audio.tx_slot_count == 1);
    assert(g_audio.tx_block_bytes == 320 * sizeof(int16_t));
    reset(WAVE_PLAYER_BACKEND_I2S_CODEC, WAVE_PLAYER_BACKEND_I2S_CODEC);
    assert(wave_player_config_internal(16000, 320 * sizeof(int16_t), 0, 1) == -EINVAL);
    assert(!g_audio.io_started && i2s_calls == 0 && dma_starts == 0);
    reset(WAVE_PLAYER_BACKEND_BL_CODEC, WAVE_PLAYER_BACKEND_I2S_CODEC);
    assert(wave_player_config_internal(16000, 320 * sizeof(int16_t), 0, 1) == -EINVAL);
    assert(!g_audio.io_started && i2s_calls == 0 && dma_starts == 0);
    capture_requested = false;
    /* An inactive external capture codec cannot impose a DSP geometry on the
     * builtin DAC, including when its codec type requires a four-slot bus. */
    g_audio.cap_codec.type = AUDIO_CODEC_TYPE_JY6316;
    assert(wave_player_config_internal(16000, 320 * sizeof(int16_t), 0, 1) == 0);
    assert(i2s_calls == 0 && builtin_play_starts == 1 && builtin_capture_starts == 0);
    reset(WAVE_PLAYER_BACKEND_I2S_CODEC, WAVE_PLAYER_BACKEND_I2S_CODEC);
    capture_requested = false;
    g_audio.play_codec.type = AUDIO_CODEC_TYPE_JY6316;
    assert(wave_player_config_internal(16000, 320 * sizeof(int16_t), 0, 1) == 0);
    assert(i2s_calls == 1 && pll_calls == 1 && dma_starts == 1);
    assert(final_i2s_tx && !final_i2s_rx && builtin_play_starts == 0 && builtin_capture_starts == 0);
    assert(observed_i2s.channel_mode == I2S_CHANNEL_MODE_NUM_4 && g_audio.tx_slot_count == 4);
    puts("Production config: builtin mono bypasses board DSP merge; active odd I2S slots remain rejected");
    return 0;
}
