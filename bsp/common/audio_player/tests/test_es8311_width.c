/* Exercise native word-length rejection before any I2C configuration changes. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Include the real driver to exercise its private runtime setter as well. */
#include "../../audio_codec/audio_codec_es8311.c"

static uint8_t registers[256];
static unsigned transfers;

int bflb_i2c_transfer(struct bflb_device_s *device, struct bflb_i2c_msg_s *messages, int count)
{
    assert(device != NULL && messages != NULL && count == 2);
    ++transfers;
    uint8_t reg = messages[0].buffer[0];
    if (messages[1].flags & I2C_M_READ) {
        messages[1].buffer[0] = registers[reg];
    } else {
        registers[reg] = messages[1].buffer[0];
    }
    return 0;
}

void bflb_mtimer_delay_ms(uint32_t milliseconds)
{
    (void)milliseconds;
}

int main(void)
{
    struct bflb_device_s i2c;
    audio_codec_dev_t device = { .i2c = &i2c, .i2c_addr = 0x18 };
    audio_codec_cfg_t config = {
        .sample_rate_hz = 16000,
        .channels = 2,
        .bits = AUDIO_CODEC_BITS_16,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S,
        .enable_adc = true,
        .enable_dac = true,
    };
    assert(audio_codec_es8311_driver.init(&device, &config) == 0);
    assert((registers[ES8311_SDPIN_REG09] & 0x1c) == 0x0c);
    assert((registers[ES8311_SDPOUT_REG0A] & 0x1c) == 0x0c);

    uint8_t before[sizeof(registers)];
    memcpy(before, registers, sizeof(before));
    config.bits = AUDIO_CODEC_BITS_24;
    transfers = 0;
    assert(audio_codec_es8311_driver.init(&device, &config) == -2);
    assert(transfers == 0 && memcmp(before, registers, sizeof(before)) == 0);
    assert(map_bits(AUDIO_CODEC_BITS_24) == ES8311_BIT_LENGTH_24BITS);
    assert(es8311_set_bits_per_sample(&i2c, device.i2c_addr, ES8311_BIT_LENGTH_24BITS) == -2);
    assert(transfers == 0 && memcmp(before, registers, sizeof(before)) == 0);

    config.bits = AUDIO_CODEC_BITS_32;
    assert(audio_codec_es8311_driver.init(&device, &config) == 0);
    assert((registers[ES8311_SDPIN_REG09] & 0x1c) == 0x10);
    assert((registers[ES8311_SDPOUT_REG0A] & 0x1c) == 0x10);
    assert(es8311_set_bits_per_sample(&i2c, device.i2c_addr, ES8311_BIT_LENGTH_16BITS) == 0);
    assert((registers[ES8311_SDPIN_REG09] & 0x1c) == 0x0c);
    assert((registers[ES8311_SDPOUT_REG0A] & 0x1c) == 0x0c);
    puts("ES8311: native PCM16/32 configuration and side-effect-free PCM24 rejection passed");
    return 0;
}
