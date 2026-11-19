/* Exercise the real codec initialization paths over a fault-injected I2C bus. */
#include "audio_codec.h"
#include "audio_codec_driver.h"
#include "audio_codec_es8388.h"
#include "audio_codec_es8389.h"
#include "audio_codec_wm8978.h"
#include "bflb_i2c.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned transfers;
static unsigned failed_transfers;
static unsigned fail_at;
static bool fail_all;

int bflb_i2c_transfer(struct bflb_device_s *dev, struct bflb_i2c_msg_s *messages, int count)
{
    assert(dev != NULL && messages != NULL && count == 2);
    ++transfers;
    if (fail_all || (fail_at != 0U && transfers == fail_at)) {
        ++failed_transfers;
        return -1;
    }
    for (int i = 0; i < count; ++i) {
        if (messages[i].flags & I2C_M_READ) {
            memset(messages[i].buffer, 0, messages[i].length);
        }
    }
    return 0;
}

void bflb_mtimer_delay_ms(uint32_t milliseconds)
{
    (void)milliseconds;
}

static void check_init(const audio_codec_driver_t *driver, uint8_t address, audio_codec_work_mode_t mode)
{
    struct bflb_device_s i2c;
    audio_codec_dev_t device = {
        .i2c = &i2c,
        .i2c_addr = address,
        .drv = driver,
    };
    audio_codec_cfg_t config = {
        .sample_rate_hz = 16000,
        .channels = 2,
        .bits = AUDIO_CODEC_BITS_16,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S,
        .enable_adc = mode != AUDIO_CODEC_WORK_MODE_PLAYBACK,
        .enable_dac = mode != AUDIO_CODEC_WORK_MODE_RECORDING,
        .work_mode = mode,
    };

    transfers = 0;
    failed_transfers = 0;
    fail_at = 0;
    fail_all = false;
    assert(driver->init(&device, &config) == 0);
    unsigned total = transfers;
    assert(total != 0U && failed_transfers == 0U);

    for (unsigned failure = 1; failure <= total; ++failure) {
        transfers = 0;
        failed_transfers = 0;
        fail_at = failure;
        assert(driver->init(&device, &config) != 0);
        assert(failed_transfers == 1U);

        /* A reported failure must not prevent a later complete retry. */
        transfers = 0;
        failed_transfers = 0;
        fail_at = 0;
        assert(driver->init(&device, &config) == 0);
        assert(transfers == total && failed_transfers == 0U);
    }

    transfers = 0;
    failed_transfers = 0;
    fail_all = true;
    assert(driver->init(&device, &config) != 0);
    assert(failed_transfers != 0U);
    fail_all = false;

    printf("%s mode=%u: checked %u transfer failures and successful retries\n", driver->name, mode, total);
}

int main(void)
{
    for (audio_codec_work_mode_t mode = AUDIO_CODEC_WORK_MODE_CODEC; mode <= AUDIO_CODEC_WORK_MODE_PLAYBACK; ++mode) {
        check_init(&audio_codec_es8388_driver, 0x10, mode);
    }
    check_init(&audio_codec_es8389_driver, 0x12, AUDIO_CODEC_WORK_MODE_CODEC);
    check_init(&audio_codec_wm8978_driver, 0x1a, AUDIO_CODEC_WORK_MODE_CODEC);
    return 0;
}
