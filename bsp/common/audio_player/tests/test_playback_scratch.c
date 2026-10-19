/* Fault-inject the production playback scratch/resampler preparation. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WAVE_PLAYER_ENABLE_RESAMPLE 1
#define WAVE_PLAYER_RESAMPLE_QUALITY 5
#define RESAMPLER_ERR_SUCCESS 0
typedef uint32_t spx_uint32_t;
typedef struct { int unused; } SpeexResamplerState;
static unsigned allocations, fail_at, live;
static bool init_error;
static void *pvPortMalloc(size_t bytes)
{
    if (++allocations == fail_at) return NULL;
    void *p = malloc(bytes);
    assert(p);
    ++live;
    return p;
}
static void release(void *p)
{
    if (p) { assert(live); --live; free(p); }
}
static SpeexResamplerState *speex_resampler_init(spx_uint32_t channels,
    spx_uint32_t input, spx_uint32_t output, int quality, int *error)
{
    assert(channels == 1 && input != output && quality == 5);
    SpeexResamplerState *p = pvPortMalloc(sizeof(*p));
    *error = (!p || init_error) ? -1 : 0;
    return p;
}
static int speex_resampler_skip_zeros(SpeexResamplerState *p) { assert(p); return 0; }

static bool prepare(uint32_t src_hz, uint32_t out_hz)
{
    const uint32_t out_samples_per_ch = 480, src_channels = 1, sample_bytes = 2;
    const uint32_t src_block_bytes = out_samples_per_ch * src_channels * sample_bytes;
    uint32_t src_alloc_bytes;
    int16_t *src_pcm = NULL, *resampled_pcm = NULL;
    SpeexResamplerState *resampler = NULL;
    bool stop_audio = false;
    /* Generated from wave_player.c, including the actual allocation failures. */
#include "wave_player_scratch_under_test.h"
    if (!use_resample) {
        /* The real non-resampling read and flush use this full block size. */
        memset(src_pcm, 0, src_block_bytes);
    } else {
        assert(resampler && resampled_pcm && src_need_per_ch > 0);
        memset(resampled_pcm, 0, src_block_bytes);
    }
cleanup:
    release(resampler);
    release(resampled_pcm);
    release(src_pcm);
    assert(live == 0);
    return !stop_audio;
}

int main(void)
{
    const uint32_t rates[][2] = {{16000, 48000}, {48000, 16000}, {16000, 16000}};
    for (unsigned r = 0; r < sizeof(rates) / sizeof(rates[0]); ++r) {
        unsigned sites = rates[r][0] == rates[r][1] ? 1 : 3;
        init_error = false;
        for (unsigned i = 1; i <= sites; ++i) {
            allocations = 0; fail_at = i;
            assert(!prepare(rates[r][0], rates[r][1]));
        }
        allocations = 0; fail_at = 0;
        assert(prepare(rates[r][0], rates[r][1]));
        if (sites == 3) {
            allocations = 0; init_error = true;
            assert(!prepare(rates[r][0], rates[r][1]));
        }
    }
    puts("Playback scratch allocation/init failures stop safely for up/downsampling");
    return 0;
}
