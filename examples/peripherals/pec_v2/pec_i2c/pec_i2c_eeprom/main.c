#include "board.h"
#include "bflb_pec_v2_instance.h"

#define EEPROM_DEVICE_ADDR   (0x50)
#define EEPROM_REG_ADDR      (0x0000)
#define EEPROM_REG_ADDR_SIZE (2)
#define EEPROM_PAGE_SIZE     (32)

struct bflb_device_s *pec_i2c;
int ret = 0;

uint8_t i2c_buffer_w[256];
uint8_t i2c_buffer_r[256];

struct bflb_pec_i2c_s i2c_cfg = {
    .mem = 0,                        /*!< memory address of first instruction */
    .pin_scl = PEC_I2C_SCL_PIN % 32, /*!< pin of i2c clock */
    .pin_sda = PEC_I2C_SDA_PIN % 32, /*!< pin of i2c data */
};

struct bflb_pec_i2c_timing_s i2c_timing = {
    .clk = 400 * 1000,           /*!< i2c SCL clock frequency, if this member is zero, auto calculating will not be used */
    .div = 0,                    /*!< clock divisor */
    .time_high = 0,              /*!< high level time */
    .time_setup = 0,             /*!< setup time */
    .time_hold = 0,              /*!< hold time */
    .timeout = 0,                /*!< timeout, unit is clk frequency */
};

struct bflb_pec_i2c_data_s i2c_data_w = {
    .buff = i2c_buffer_w,             /*!< i2c data buffer */
    .length = 0,                      /*!< i2c data length */
    .slave_addr = EEPROM_DEVICE_ADDR, /*!< i2c slave address */
    .slave_10bit = 0,                 /*!< i2c slave address is 10bit */
    .head_10 = 0,                     /*!< send 2nd slave address byte or not */
    .read = 0,                        /*!< i2c read or write */
    .start = 1,                       /*!< generate start signal or not berfor frame */
    .stop = 1,                        /*!< generate stop signal or not after all data */
    .nak = 1,                         /*!< generate nak signal or not at last data when read */
};

struct bflb_pec_i2c_data_s i2c_data_r = {
    .buff = i2c_buffer_r,             /*!< i2c data buffer */
    .length = EEPROM_PAGE_SIZE,       /*!< i2c data length */
    .slave_addr = EEPROM_DEVICE_ADDR, /*!< i2c slave address */
    .slave_10bit = 0,                 /*!< i2c slave address is 10bit */
    .head_10 = 0,                     /*!< send 2nd slave address byte or not */
    .read = 1,                        /*!< i2c read or write */
    .start = 1,                       /*!< generate start signal or not berfor frame */
    .stop = 1,                        /*!< generate stop signal or not after all data */
    .nak = 1,                         /*!< generate nak signal or not at last data when read */
};

int main(void)
{
    board_init();
    board_pec_i2c_gpio_init();
    pec_i2c = bflb_device_get_by_name("pec_sm0");
    printf("pec_i2c = 0x%08lX\r\n", pec_i2c);

    bflb_pec_i2c_init(pec_i2c, &i2c_cfg);
    bflb_pec_i2c_set_timing(pec_i2c, &i2c_timing);

    ret = bflb_pec_i2c_is_busy(pec_i2c);
    if (ret) {
        printf("bflb_pec_i2c_is_busy error, ret = %d\r\n", ret);
        return -1;
    }
    i2c_buffer_w[0] = (uint8_t)((EEPROM_REG_ADDR >> 8) & 0xFF);
    i2c_buffer_w[1] = (uint8_t)(EEPROM_REG_ADDR & 0xFF);
    for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; i++) {
        i2c_buffer_w[EEPROM_REG_ADDR_SIZE + i] = i;
        i2c_buffer_r[i] = 0;
    }
    i2c_data_w.length = EEPROM_REG_ADDR_SIZE + EEPROM_PAGE_SIZE;
    i2c_data_w.start = 1;
    i2c_data_w.stop = 1;
    ret = bflb_pec_i2c_write(pec_i2c, &i2c_data_w);
    if (ret) {
        printf("bflb_pec_i2c_write error, ret = %d\r\n", ret);
        return -2;
    }

    bflb_mtimer_delay_ms(100);

    ret = bflb_pec_i2c_is_busy(pec_i2c);
    if (ret) {
        printf("bflb_pec_i2c_is_busy error, ret = %d\r\n", ret);
        return -3;
    }
    i2c_buffer_w[0] = (uint8_t)((EEPROM_REG_ADDR >> 8) & 0xFF);
    i2c_buffer_w[1] = (uint8_t)(EEPROM_REG_ADDR & 0xFF);
    i2c_data_w.length = EEPROM_REG_ADDR_SIZE;
    i2c_data_w.start = 1;
    i2c_data_w.stop = 0;
    ret = bflb_pec_i2c_write(pec_i2c, &i2c_data_w);
    if (ret) {
        printf("bflb_pec_i2c_write error, ret = %d\r\n", ret);
        return -4;
    }

    ret = bflb_pec_i2c_is_busy(pec_i2c);
    if (ret) {
        printf("bflb_pec_i2c_is_busy error, ret = %d\r\n", ret);
        return -5;
    }
    ret = bflb_pec_i2c_read(pec_i2c, &i2c_data_r);
    if (ret) {
        printf("bflb_pec_i2c_read error, ret = %d\r\n", ret);
        return -6;
    }
    printf("read data:\r\n");
    for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; i++) {
        printf("0x%02X ", i2c_data_r.buff[i]);
    }
    printf("\r\n");
    for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; i++) {
        if (i2c_data_r.buff[i] != i) {
            printf("error: [%u] = 0x%02X, expected 0x%02X\r\n", i, i2c_data_r.buff[i], i);
            return -7;
        }
    }

    printf("pec_i2c_eeprom test pass\r\n");
    return 0;
}
