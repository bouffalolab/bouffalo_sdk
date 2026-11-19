/* Exercise the production driver over a mock I2C register bank.
 * AVE's microphone pair LIN2-RIN2 feeds the left ADC; speaker feedback
 * LIN1-RIN1 feeds the right ADC. Neither ADC should use analog mono mix. */
#include "audio_codec_es8388.h"
#include "bflb_i2c.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t registers[256];
static unsigned writes[256];
static int fail_read = -1;
static int fail_write = -1;
int bflb_i2c_transfer(struct bflb_device_s *dev, struct bflb_i2c_msg_s *msgs, int count)
{
    assert(dev && count == 2 && msgs[0].length == 1 && msgs[1].length == 1);
    assert(msgs[0].addr == 0x10 && msgs[1].addr == 0x10);
    uint8_t reg = *msgs[0].buffer;
    if (msgs[1].flags & I2C_M_READ) {
        if (reg == fail_read) {
            return -1;
        }
        *msgs[1].buffer = registers[reg];
    } else {
        if (reg == fail_write) {
            return -1;
        }
        registers[reg] = *msgs[1].buffer;
        ++writes[reg];
    }
    return 0;
}
void bflb_mtimer_delay_ms(uint32_t ms) { (void)ms; }

static void test_mic_slot_controls(audio_codec_dev_t *codec)
{
    const struct audio_codec_driver *driver = &audio_codec_es8388_driver;
    assert(driver->set_mic_channel_volume && driver->set_mic_channel_gain_db);
    audio_codec_channel_caps_t caps;
    assert(driver->get_mic_channel_caps(codec, 0, &caps) == 0);
    assert(caps.gain_step_count == 9 && caps.gain_steps_db[8] == 24.0f);
    assert(driver->get_speaker_channel_caps(codec, 0, &caps) == 0);
    assert(caps.linked_mask == 3 && caps.independent_controls == 0);
    registers[0x10] = 0x31;
    registers[0x11] = 0x52;
    registers[0x09] = 0x35;
    memset(writes, 0, sizeof(writes));
    assert(driver->set_mic_channel_volume(codec, 1, 100) == 0);
    assert(registers[0x10] == 0 && registers[0x11] == 0x52);
    assert(writes[0x10] == 1 && writes[0x11] == 0);
    assert(driver->set_mic_channel_volume(codec, 2, 0) == 0);
    assert(registers[0x10] == 0 && registers[0x11] == 192);
    assert(writes[0x10] == 1 && writes[0x11] == 1);
    assert(driver->set_mic_channel_gain_db(codec, 1, 24.0f) == 0 && registers[0x09] == 0x85);
    assert(driver->set_mic_channel_gain_db(codec, 2, 0) == 0 && registers[0x09] == 0x80);
    assert(driver->set_mic_channel_volume(codec, 3, 50) == 0);
    assert(registers[0x10] == 96 && registers[0x11] == 96);
    assert(driver->set_mic_channel_gain_db(codec, 3, 12.0f) == 0 && registers[0x09] == 0x44);
    uint8_t original[256];
    unsigned before[256];
    memcpy(original, registers, sizeof(original));
    memcpy(before, writes, sizeof(before));
    const uint16_t invalid_masks[] = { 0, 4, 5, UINT16_MAX };
    for (unsigned i = 0; i < sizeof(invalid_masks) / sizeof(invalid_masks[0]); ++i) {
        assert(driver->set_mic_channel_volume(codec, invalid_masks[i], 25) != 0);
        assert(driver->set_mic_channel_gain_db(codec, invalid_masks[i], 25) != 0);
    }
    assert(driver->set_mic_channel_volume(codec, 1, -1) != 0);
    assert(driver->set_mic_channel_gain_db(codec, 2, 101) != 0);
    assert(memcmp(original, registers, sizeof(original)) == 0);
    assert(memcmp(before, writes, sizeof(before)) == 0);
    fail_write = 0x10;
    assert(driver->set_mic_channel_volume(codec, 1, 25) != 0);
    fail_write = -1;
    fail_read = 0x09;
    assert(driver->set_mic_channel_gain_db(codec, 2, 6.0f) != 0);
    fail_read = -1;
    fail_write = 0x09;
    assert(driver->set_mic_channel_gain_db(codec, 1, 6.0f) != 0);
    fail_write = -1;
    assert(memcmp(original, registers, sizeof(original)) == 0);
    assert(memcmp(before, writes, sizeof(before)) == 0);
}

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
    test_mic_slot_controls(&codec);
    assert(audio_codec_es8388_driver.init(NULL, &cfg) < 0);
    puts("ES8388 routes, independent mic slot registers, invalid masks and I2C failures passed");
    return 0;
}
