/* Exercise the production driver over a mock I2C register bank.
 * AVE's microphone pair LIN2-RIN2 feeds the left ADC; speaker feedback
 * LIN1-RIN1 feeds the right ADC. Neither ADC should use analog mono mix. */
#include "audio_codec_es8388.h"
#include "bflb_i2c.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t registers[256];
int bflb_i2c_transfer(struct bflb_device_s *dev, struct bflb_i2c_msg_s *msgs, int count)
{
    assert(dev && count == 2 && msgs[0].length == 1 && msgs[1].length == 1);
    assert(msgs[0].addr == 0x10 && msgs[1].addr == 0x10);
    uint8_t reg = *msgs[0].buffer;
    if (msgs[1].flags & I2C_M_READ) *msgs[1].buffer = registers[reg];
    else registers[reg] = *msgs[1].buffer;
    return 0;
}
void bflb_mtimer_delay_ms(uint32_t ms) { (void)ms; }

int main(void)
{
    struct bflb_device_s i2c = {0};
    audio_codec_dev_t codec = {.i2c = &i2c, .i2c_addr = 0x10};
    audio_codec_cfg_t cfg = {.sample_rate_hz = 16000, .channels = 2,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S, .bits = AUDIO_CODEC_BITS_16,
        .enable_adc = true, .enable_dac = true};
    const audio_codec_work_mode_t modes[] = {AUDIO_CODEC_WORK_MODE_CODEC, AUDIO_CODEC_WORK_MODE_RECORDING};
    const audio_codec_mic_input_t inputs[] = {AUDIO_CODEC_MIC_INPUT_SINGLE, AUDIO_CODEC_MIC_INPUT_DIFF};
    for (unsigned i = 0; i < sizeof(modes)/sizeof(modes[0]); ++i) {
        cfg.work_mode = modes[i];
        for (unsigned input = 0; input < sizeof(inputs)/sizeof(inputs[0]); ++input) {
            cfg.mic_input_mode = inputs[input];
            memset(registers, 0xff, sizeof(registers));
            assert(audio_codec_es8388_driver.init(&codec, &cfg) == 0);
            bool single = cfg.mic_input_mode == AUDIO_CODEC_MIC_INPUT_SINGLE;
            assert(registers[0x0a] == (single ? 0x00 : 0xf8));
            assert(registers[0x0b] == (single ? 0x02 : 0x82));
            assert(registers[0x0c] == 0x0c); /* 16-bit I2S ADC. */
            assert(registers[0x03] == 0x09); /* ADC on, MICBIAS off. */
        }
    }
    assert(audio_codec_es8388_driver.init(NULL, &cfg) < 0);
    puts("ES8388 independent differential routes in codec and recording modes passed");
    return 0;
}
