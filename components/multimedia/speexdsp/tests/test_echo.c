/* Delayed far-end echo with optional independent near-end speech. */
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define FRAME 320
#define HISTORY 4096
static uint32_t seed = 1;
static double far_history[HISTORY], far_filtered;
static unsigned sample_index;

/* Internal echo/post-filter boundary also used by preprocess.c. */
extern void speex_echo_get_residual(SpeexEchoState *, spx_int32_t *, int);

static void generate(spx_int16_t *mic, spx_int16_t *ref, double *near_power, int double_talk)
{
    for (unsigned i = 0; i < FRAME; ++i, ++sample_index) {
        seed = seed * 1664525U + 1013904223U;
        double noise = ((int32_t)(seed >> 16) - 32768) / 4.0;
        far_filtered = 0.7 * far_filtered + 0.3 * noise;
        ref[i] = far_filtered;
        far_history[sample_index % HISTORY] = ref[i];
        double echo = 0.32 * far_history[(sample_index - 640U) % HISTORY] +
                      0.08 * far_history[(sample_index - 701U) % HISTORY];
        double t = sample_index / 16000.0;
        double speech = double_talk ? 1800 * (0.6 + 0.4 * sin(2 * 3.7 * t)) *
            (sin(2 * 183 * t + 2 * sin(2 * 2.1 * t)) + 0.4 * sin(2 * 419 * t)) : 0;
        *near_power += speech * speech;
        mic[i] = echo + speech;
    }
}

static SpeexPreprocessState *make_preprocess(SpeexEchoState *echo)
{
    SpeexPreprocessState *pre = speex_preprocess_state_init(FRAME, 16000);
    assert(pre);
    int enabled = 1, noise_db = -15, echo_db = -40, double_talk_db = -15;
    assert(speex_preprocess_ctl(pre, SPEEX_PREPROCESS_SET_ECHO_STATE, echo) == 0);
    assert(speex_preprocess_ctl(pre, SPEEX_PREPROCESS_SET_DENOISE, &enabled) == 0);
    assert(speex_preprocess_ctl(pre, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noise_db) == 0);
    assert(speex_preprocess_ctl(pre, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &echo_db) == 0);
    assert(speex_preprocess_ctl(pre, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE, &double_talk_db) == 0);
    return pre;
}

int main(void)
{
    setbuf(stdout, NULL);
    SpeexEchoState *echo = speex_echo_state_init(FRAME, 3200);
    assert(echo);
    int hz = 16000;
    assert(speex_echo_ctl(echo, SPEEX_ECHO_SET_SAMPLING_RATE, &hz) == 0);
    SpeexPreprocessState *pre = make_preprocess(echo);
    spx_int16_t mic[FRAME], ref[FRAME], output[FRAME];
    double raw = 0, clean = 0, cold_raw = 0, cold_clean = 0;
    for (unsigned frame = 0; frame < 1000; ++frame) {
        double unused = 0;
        generate(mic, ref, &unused, 0);
        speex_echo_cancellation(echo, mic, ref, output);
        speex_preprocess_run(pre, output);
        for (unsigned i = 0; i < FRAME; ++i) {
            if (frame < 100) {
                cold_raw += (double)mic[i] * mic[i];
                cold_clean += (double)output[i] * output[i];
            }
            if (frame >= 500) {
                raw += (double)mic[i] * mic[i];
                clean += (double)output[i] * output[i];
            }
        }
    }
    double cold_db = 10 * log10(cold_raw / (cold_clean + 1));
    double warm_db = 10 * log10(raw / (clean + 1));
    printf("Echo reduction: startup %.1f dB, converged %.1f dB\n", cold_db, warm_db);
    assert(cold_db > 20 && warm_db > 20);

    for (unsigned pass = 0; pass < 2; ++pass) {
        double near = 0, power = 0;
        for (unsigned frame = 0; frame < (pass ? 100 : 400); ++frame) {
            generate(mic, ref, &near, 1);
            speex_echo_cancellation(echo, mic, ref, output);
            speex_preprocess_run(pre, output);
            for (unsigned i = 0; i < FRAME; ++i) power += (double)output[i] * output[i];
        }
        double gain = 10 * log10(power / near);
        printf("Near-end gain %s reset: %.1f dB\n", pass ? "after" : "before", gain);
        assert(gain > -12 && gain < 6);
        if (!pass) {
            spx_int32_t residual[FRAME + 1];
            speex_echo_get_residual(echo, residual, FRAME + 1);
            double residual_power = 0;
            for (unsigned i = 0; i <= FRAME; ++i) residual_power += residual[i];
            assert(residual_power > 0);
            speex_echo_state_reset(echo);
            speex_echo_get_residual(echo, residual, FRAME + 1);
            for (unsigned i = 0; i <= FRAME; ++i) assert(residual[i] == 0);
            speex_preprocess_state_destroy(pre);
            pre = make_preprocess(echo);
        }
    }
    speex_preprocess_state_destroy(pre);
    speex_echo_state_destroy(echo);
    puts("Speex startup, residual reset and double-talk checks passed");
    return 0;
}
