#include "mimiclaw_ext_rtc.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "bflb_core.h"
#include "bflb_i2c.h"
#include "board.h"

#define DS3231_I2C_NAME "i2c0"
#define DS3231_I2C_FREQ_HZ 100000U
#define DS3231_ADDR 0x68

#define DS3231_REG_TIME_START 0x00
#define DS3231_TIME_REG_COUNT 7

static struct bflb_device_s *s_i2c;
static bool s_i2c_ready;
static bool s_ds3231_present;

static int day_of_year(int year, int month, int day)
{
    static const int day_offset[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    bool leap = ((year % 4) == 0 && (year % 100) != 0) || ((year % 400) == 0);
    int yday = day_offset[month - 1] + day - 1;

    if (leap && month > 2) {
        yday++;
    }

    return yday;
}

static uint8_t bcd_to_bin(uint8_t bcd)
{
    return (uint8_t)(((bcd >> 4) * 10U) + (bcd & 0x0FU));
}

static uint8_t bin_to_bcd(uint8_t val)
{
    return (uint8_t)(((val / 10U) << 4) | (val % 10U));
}

static bool ds3231_read_regs(uint8_t reg, uint8_t *data, uint16_t len)
{
    struct bflb_i2c_msg_s msgs[2];
    uint8_t reg_addr = reg;

    msgs[0].addr = DS3231_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg_addr;
    msgs[0].length = 1;

    msgs[1].addr = DS3231_ADDR;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = data;
    msgs[1].length = len;

    return bflb_i2c_transfer(s_i2c, msgs, 2) == 0;
}

static bool ds3231_write_regs(uint8_t reg, const uint8_t *data, uint16_t len)
{
    struct bflb_i2c_msg_s msg;
    uint8_t buf[1 + DS3231_TIME_REG_COUNT];

    if (len > DS3231_TIME_REG_COUNT) {
        return false;
    }

    buf[0] = reg;
    memcpy(&buf[1], data, len);

    msg.addr = DS3231_ADDR;
    msg.flags = 0;
    msg.buffer = buf;
    msg.length = (uint16_t)(1 + len);

    return bflb_i2c_transfer(s_i2c, &msg, 1) == 0;
}

static bool ensure_ds3231_ready(void)
{
    uint8_t sec;

    if (!s_i2c_ready) {
        board_i2c0_gpio_init();
        s_i2c = bflb_device_get_by_name(DS3231_I2C_NAME);
        if (!s_i2c) {
            return false;
        }
        bflb_i2c_init(s_i2c, DS3231_I2C_FREQ_HZ);
        s_i2c_ready = true;
    }

    if (!s_ds3231_present) {
        if (!ds3231_read_regs(DS3231_REG_TIME_START, &sec, 1)) {
            return false;
        }
        s_ds3231_present = true;
    }

    return true;
}

__attribute__((weak)) bool mimiclaw_ext_rtc_is_available(void)
{
    return ensure_ds3231_ready();
}

__attribute__((weak)) bool mimiclaw_ext_rtc_read_utc(struct bflb_tm *tm)
{
    uint8_t raw[DS3231_TIME_REG_COUNT];
    int year;
    int month;
    int day;
    int wday;

    if (!tm || !ensure_ds3231_ready()) {
        return false;
    }

    if (!ds3231_read_regs(DS3231_REG_TIME_START, raw, sizeof(raw))) {
        return false;
    }

    tm->tm_sec = bcd_to_bin(raw[0] & 0x7FU);
    tm->tm_min = bcd_to_bin(raw[1] & 0x7FU);

    if (raw[2] & 0x40U) {
        uint8_t hour12 = bcd_to_bin(raw[2] & 0x1FU);
        bool pm = (raw[2] & 0x20U) != 0;

        if (hour12 == 12U) {
            hour12 = 0;
        }
        tm->tm_hour = (int)hour12 + (pm ? 12 : 0);
    } else {
        tm->tm_hour = bcd_to_bin(raw[2] & 0x3FU);
    }

    wday = bcd_to_bin(raw[3] & 0x07U);
    if (wday >= 1 && wday <= 7) {
        tm->tm_wday = wday - 1;
    } else {
        tm->tm_wday = 0;
    }

    day = bcd_to_bin(raw[4] & 0x3FU);
    month = bcd_to_bin(raw[5] & 0x1FU);
    year = 2000 + bcd_to_bin(raw[6]);
    if (raw[5] & 0x80U) {
        year += 100;
    }

    if (month < 1 || month > 12 || day < 1 || day > 31 ||
        tm->tm_hour > 23 || tm->tm_min > 59 || tm->tm_sec > 59) {
        return false;
    }

    tm->tm_mday = day;
    tm->tm_mon = month - 1;
    tm->tm_year = year - 1900;
    tm->tm_yday = day_of_year(year, month, day);

    return true;
}

__attribute__((weak)) bool mimiclaw_ext_rtc_write_utc(const struct bflb_tm *tm)
{
    uint8_t raw[DS3231_TIME_REG_COUNT];
    int year;
    int month;
    int day;
    int wday;
    uint8_t ds_wday;

    if (!tm || !ensure_ds3231_ready()) {
        return false;
    }

    year = tm->tm_year + 1900;
    month = tm->tm_mon + 1;
    day = tm->tm_mday;
    wday = tm->tm_wday;

    if (year < 2000 || year > 2199 ||
        month < 1 || month > 12 ||
        day < 1 || day > 31 ||
        tm->tm_hour < 0 || tm->tm_hour > 23 ||
        tm->tm_min < 0 || tm->tm_min > 59 ||
        tm->tm_sec < 0 || tm->tm_sec > 59) {
        return false;
    }

    /* bflb_rtc_get_utc_time() may expose weekday as 1..7, standard tm uses 0..6. */
    if (wday >= 1 && wday <= 7) {
        ds_wday = (uint8_t)wday;
    } else if (wday >= 0 && wday <= 6) {
        ds_wday = (uint8_t)(wday + 1);
    } else {
        ds_wday = 1;
    }

    raw[0] = bin_to_bcd((uint8_t)tm->tm_sec);
    raw[1] = bin_to_bcd((uint8_t)tm->tm_min);
    raw[2] = bin_to_bcd((uint8_t)tm->tm_hour);
    raw[3] = bin_to_bcd(ds_wday);
    raw[4] = bin_to_bcd((uint8_t)day);
    raw[5] = bin_to_bcd((uint8_t)month);
    raw[6] = bin_to_bcd((uint8_t)(year % 100));
    if (year >= 2100) {
        raw[5] |= 0x80U;
    }

    return ds3231_write_regs(DS3231_REG_TIME_START, raw, sizeof(raw));
}
