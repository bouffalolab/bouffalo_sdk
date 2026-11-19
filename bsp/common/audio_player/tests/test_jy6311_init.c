/* Validate production codec initialization without invoking demonstration teardown. */
#include "audio_codec_jy6311.h"
#include "audio_codec_driver.h"
#include "bflb_clock.h"
#include "bflb_i2c.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct bflb_device_s i2c;
static uint8_t registers[128][256];
static unsigned writes[128];
static unsigned transfers;
static unsigned fail_at;
static unsigned failures;
static bool fail_all;

int bflb_i2c_transfer(struct bflb_device_s *device, struct bflb_i2c_msg_s *messages, int count)
{
    assert(device == &i2c && messages != NULL && count == 2);
    ++transfers;
    if (fail_all || (fail_at != 0U && transfers == fail_at)) {
        ++failures;
        return -1;
    }
    unsigned address = messages[0].addr;
    assert(address < 128 && messages[0].length == 1 && messages[1].length == 1);
    unsigned reg = messages[0].buffer[0];
    if (messages[1].flags & I2C_M_READ) {
        messages[1].buffer[0] = registers[address][reg];
    } else {
        registers[address][reg] = messages[1].buffer[0];
        ++writes[address];
    }
    return 0;
}

void bflb_mtimer_delay_ms(uint32_t milliseconds)
{
    (void)milliseconds;
}

uint64_t bflb_mtimer_get_time_us(void)
{
    return transfers;
}

uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t index)
{
    assert(type == BFLB_DEVICE_TYPE_I2S && index == 0);
    return 24576000;
}

void bflb_i2c_init(struct bflb_device_s *device, uint32_t rate)
{
    assert(device == &i2c && rate == 200000);
}

struct bflb_device_s *bflb_device_get_by_name(const char *name)
{
    assert(strcmp(name, "i2c0") == 0);
    return &i2c;
}

static void reset_bus(void)
{
    memset(registers, 0, sizeof(registers));
    memset(writes, 0, sizeof(writes));
    transfers = 0;
    failures = 0;
    fail_at = 0;
    fail_all = false;
}

static void check_modes(void)
{
    for (unsigned address = JY6311_I2C_ADDR_0; address <= JY6311_I2C_ADDR_1; ++address) {
        for (audio_codec_work_mode_t mode = AUDIO_CODEC_WORK_MODE_CODEC; mode <= AUDIO_CODEC_WORK_MODE_PLAYBACK;
             ++mode) {
            reset_bus();
            audio_codec_dev_t device = { .i2c = &i2c, .i2c_addr = address };
            audio_codec_cfg_t config = {
                .sample_rate_hz = 16000,
                .channels = 2,
                .bits = AUDIO_CODEC_BITS_16,
                .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S,
                .enable_adc = mode != AUDIO_CODEC_WORK_MODE_PLAYBACK,
                .enable_dac = mode != AUDIO_CODEC_WORK_MODE_RECORDING,
                .work_mode = mode,
            };
            assert(audio_codec_jy6311_driver.init(&device, &config) == 0);
            assert(writes[address] != 0 && writes[address ^ 1U] == 0);
            if (config.enable_adc) {
                assert((registers[address][MODCLK_EN] & ADC_MODULE_CLK_ALL_EN_Msk) == ADC_MODULE_CLK_ALL_EN_Msk);
                assert(registers[address][PWR_CTRL2] & POWER_MICBIAS_EN_Msk);
            }
            if (config.enable_dac) {
                assert((registers[address][MODCLK_EN] & DAC_MODULE_CLK_ALL_EN_Msk) == DAC_MODULE_CLK_ALL_EN_Msk);
            }
        }
    }
}

static void check_failures_and_restore(void)
{
    reset_bus();
    audio_codec_dev_t device = { .i2c = &i2c, .i2c_addr = JY6311_I2C_ADDR_1 };
    audio_codec_cfg_t config = {
        .sample_rate_hz = 16000,
        .channels = 4,
        .bits = AUDIO_CODEC_BITS_16,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_DSP,
        .enable_adc = true,
        .enable_dac = true,
    };
    const audio_codec_driver_t *driver = &audio_codec_jy6311_driver;
    assert(driver->init(&device, &config) == 0);
    unsigned total = transfers;
    for (unsigned failure = 1; failure <= total; ++failure) {
        reset_bus();
        fail_at = failure;
        assert(driver->init(&device, &config) != 0);
        assert(failures == 1);
        reset_bus();
        assert(driver->init(&device, &config) == 0);
    }
    transfers = 0;
    failures = 0;
    fail_all = true;
    assert(driver->init(&device, &config) != 0 && failures != 0);
    fail_all = false;
    assert(driver->init(&device, &config) == 0);

    assert(driver->set_speaker_channel_volume(&device, 1, 37) == 0);
    assert(driver->set_mic_channel_volume(&device, 1, 51) == 0);
    assert(driver->set_mic_channel_gain_db(&device, 1, 15.0f) == 0);
    uint8_t dac = registers[device.i2c_addr][DAC_DVC_CTRL];
    uint8_t adc = registers[device.i2c_addr][ADC_DVC_CTRL];
    uint8_t pga = registers[device.i2c_addr][ANA_ADC_CTRL1] & ADC_PGA_GAIN_Msk;
    assert(driver->set_sample_rate(&device, 32000) == 0);
    assert(registers[device.i2c_addr][DAC_DVC_CTRL] == dac);
    assert(registers[device.i2c_addr][ADC_DVC_CTRL] == adc);
    assert((registers[device.i2c_addr][ANA_ADC_CTRL1] & ADC_PGA_GAIN_Msk) == pga);
    assert(driver->set_speaker_channel_mute(&device, 1, true) == 0);
    assert(driver->set_mic_channel_mute(&device, 1, true) == 0);
    assert(driver->set_sample_rate(&device, 48000) == 0);
    assert(registers[device.i2c_addr][DAC_DVC_CTRL] == 0 && registers[device.i2c_addr][ADC_DVC_CTRL] == 0);
    assert(driver->set_speaker_channel_mute(&device, 1, false) == 0);
    assert(driver->set_mic_channel_mute(&device, 1, false) == 0);
    assert(registers[device.i2c_addr][DAC_DVC_CTRL] == dac);
    assert(registers[device.i2c_addr][ADC_DVC_CTRL] == adc);
    assert(writes[JY6311_I2C_ADDR_0] == 0);
    printf("JY6311: two addresses, three modes, %u transfer failures and state restoration passed\n", total);
}

int main(void)
{
    check_modes();
    check_failures_and_restore();
    return 0;
}
