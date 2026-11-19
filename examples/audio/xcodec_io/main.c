/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "board.h"
#include "board_audio.h"
#include "cli_xcodec.h"
#include "FreeRTOS.h"
#include "task.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

static xcodec_dev_sample_info_t record_format = {
    .sample_rate = 16000,             /* Capture at 16 kHz. */
    .bits_per_sample = 16,            /* PCM16 samples. */
    .channel = BOARD_AUDIO_CHANNELS,  /* Physical slots: Orion 4, built-in ADC/DAC 1. */
    .channel_mask = 0,                /* 0: all slots; bit n selects CHn (0x08: CH3 only). */
    .mclk_multiple = 0,               /* Accept the board-selected MCLK. */
};
static xcodec_dev_sample_info_t play_format = {
    .sample_rate = 16000,             /* Match capture rate for echo. */
    .bits_per_sample = 16,            /* PCM16 samples. */
    .channel = BOARD_AUDIO_CHANNELS,  /* Physical slots on the playback bus. */
    .channel_mask = 0x01,             /* Bit 0: output to CH0; other slots carry silence. */
    .mclk_multiple = 0,               /* Accept the board-selected MCLK. */
};

static int echo_channel;
/* Reads can end between channels of the same PCM frame. */
static unsigned record_channel;
static unsigned tone_samples;
static unsigned tone_phase;
static int tone_amplitude;
static unsigned blocks;
static int16_t minimum[BOARD_AUDIO_CHANNELS];
static int16_t maximum[BOARD_AUDIO_CHANNELS];

int audio_set_echo_channel(int channel)
{
    unsigned mask = record_format.channel_mask ? record_format.channel_mask : (1U << record_format.channel) - 1U;
    if (channel == -1) {
        channel = (mask & 8U) ? 3 : __builtin_ctz(mask);
    }
    if ((unsigned)channel >= record_format.channel || !(mask & (1U << channel))) {
        printf("xcodec: CH%d is not captured (mask=0x%02X)\r\n", channel, mask);
        return -EINVAL;
    }
    echo_channel = channel;
    tone_samples = 0;
    return 0;
}

void audio_start_tone(int amplitude)
{
    tone_samples = 2U * record_format.sample_rate;
    tone_phase = 0;
    tone_amplitude = amplitude;
}

void audio_print_status(void)
{
    printf("xcodec: echo channel=%d tone_samples=%u\r\n", echo_channel, tone_samples);
}

static unsigned process_pcm(const int16_t *record_pcm, unsigned record_bytes, int16_t *play_pcm)
{
    static const int16_t sine[16] = {0, 1567, 2896, 3784, 4096, 3784, 2896, 1567,
                                   0, -1567, -2896, -3784, -4096, -3784, -2896, -1567};
    unsigned playback_channels = XCODEC_CHANNEL_COUNT(play_format.channel, play_format.channel_mask);
    unsigned mask = record_format.channel_mask ? record_format.channel_mask : (1U << record_format.channel) - 1U;
    unsigned output_samples = 0;

    for (unsigned i = 0; i < record_bytes / sizeof(*record_pcm); ++i) {
        while (!(mask & (1U << record_channel))) {
            record_channel = (record_channel + 1U) % record_format.channel;
        }
        unsigned ch = record_channel;
        record_channel = (ch + 1U) % record_format.channel;
        int16_t sample = record_pcm[i];
        if (sample < minimum[ch]) {
            minimum[ch] = sample;
        }
        if (sample > maximum[ch]) {
            maximum[ch] = sample;
        }
        if (ch != (unsigned)echo_channel) {
            continue;
        }
        int16_t output = sample;
        if (tone_samples) {
            output = sine[tone_phase * 16U / record_format.sample_rate] * tone_amplitude / 4096;
            tone_phase = (tone_phase + 1000U) % record_format.sample_rate;
            if (--tone_samples == 0) {
                printf("xcodec: tone done, echo channel=%d\r\n", echo_channel);
            }
        }
        for (unsigned ch = 0; ch < playback_channels; ++ch) {
            play_pcm[output_samples++] = output;
        }
    }
    if (++blocks % 100 == 0) {
        printf("xcodec: blocks=%u", blocks);
        for (unsigned ch = 0; ch < record_format.channel; ++ch) {
            if (!(mask & (1U << ch))) {
                continue;
            }
            printf(" ch%u=[%d,%d]", ch, minimum[ch], maximum[ch]);
            minimum[ch] = 0;
            maximum[ch] = 0;
        }
        printf("\r\n");
    }
    return output_samples * sizeof(*play_pcm);
}

static void xcodec_dev_echo_task(void *unused)
{
    (void)unused;
    static int16_t record_pcm[256];
    static int16_t play_pcm[256 * BOARD_AUDIO_CHANNELS];

    xcodec_dev_handle_t play_dev = NULL, record_dev = NULL;

    xcodec_board_handle_t board = xcodec_dev_board_init();
    if (!board) {
        printf("xcodec: unable to initialize board audio\r\n");
        goto exit;
    }

    xcodec_dev_cfg_t dev_cfg = {
        .dev_type = XCODEC_DEV_TYPE_OUT,
        .data_if = board->play_data,
        .codec_if = board->play_codec,
    };
    play_dev = xcodec_dev_new(&dev_cfg);

    dev_cfg.dev_type = XCODEC_DEV_TYPE_IN;
    dev_cfg.data_if = board->record_data;
    dev_cfg.codec_if = board->record_codec;
    record_dev = xcodec_dev_new(&dev_cfg);

    if (!play_dev || !record_dev) {
        printf("xcodec: unable to create audio devices\r\n");
        goto close;
    }

    if (xcodec_dev_open(play_dev, &play_format)) {
        goto close;
    }
    if (xcodec_dev_open(record_dev, &record_format)) {
        goto close;
    }
    xcodec_dev_set_out_vol(play_dev, 60);
    xcodec_dev_set_out_mute(play_dev, false);

    audio_set_echo_channel(-1);

    record_channel = 0;
    tone_samples = 0;
    tone_phase = 0;
    blocks = 0;
    memset(minimum, 0, sizeof(minimum));
    memset(maximum, 0, sizeof(maximum));

    printf("xcodec: PCM16 %lu Hz, slots=%u capture=%u mask=0x%02X\r\n",
           (unsigned long)record_format.sample_rate, (unsigned)record_format.channel,
           XCODEC_CHANNEL_COUNT(record_format.channel, record_format.channel_mask), (unsigned)record_format.channel_mask);

    while (!xcodec_dev_read(record_dev, record_pcm, sizeof(record_pcm))) {

        xcodec_cli_process(play_dev);
        unsigned play_bytes = process_pcm(record_pcm, sizeof(record_pcm), play_pcm);

        if (xcodec_dev_write(play_dev, play_pcm, play_bytes)) {
            break;
        }
    }

close:;
    int ret = 0;
    if (record_dev) {
        ret = xcodec_dev_close(record_dev);
    }
    if (play_dev) {
        ret |= xcodec_dev_close(play_dev);
    }
    if (ret) {
        printf("xcodec: unable to close audio devices\r\n");
        goto exit;
    }
    xcodec_dev_delete(record_dev);
    xcodec_dev_delete(play_dev);
    if (xcodec_dev_board_deinit()) {
        printf("xcodec: unable to release audio resources\r\n");
    }
exit:

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();
    if (xcodec_cli_init()) {
        printf("xcodec: unable to initialize CLI\r\n");
        return 1;
    }
    if (xTaskCreate(xcodec_dev_echo_task, "xcodec_echo", 1024, NULL, 3, NULL) != pdPASS) {
        printf("xcodec: unable to create task\r\n");
        return 1;
    }
    vTaskStartScheduler();
    return 0;
}
