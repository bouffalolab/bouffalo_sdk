#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_uart.h"
#include "shell.h"
#include "board.h"

/****************************************************************************************************

             +--16.1--+     +--16.3--+     +--16.5--+           +--16.7--+     +--17.1--+
----+----    |        |     |        |     |        |           |        |     |        |
    |      16.0     11.1  16.2     11.3  16.4     11.5        16.6     11.7  17.0     12.1
    |        |        |     |        |     |        |           |        |     |        |
   1.0       +--11.0--+     +--11.2--+     +--11.4--+           +--11.6--+     +--12.0--+    2.0C
    |        |        |     |        |     |        |           |        |     |        |
    |       6.0      6.1   6.2      6.3   6.4      6.5         6.6      6.7   7.0      7.1
    |        |        |     |        |     |        |           |        |     |        |
             +---1.1--+     +---1.3--+     +---1.5--+    1.4    +---1.7--+     +---2.1--+

             +---0.1--+     +---0.3--+     +---0.5--+           +---0.7--+     +---2.2--+
|       |    |        |     |        |     |        |           |        |     |        |
|       |   5.0      5.1   5.2      5.3   5.4      5.5         5.6      5.7   7.3      7.2
|       |    |        |     |        |     |        |           |        |     |        |
+--0.0--+    +--10.0--+     +--10.2--+     +--10.4--+           +--10.6--+     +--12.3--+    2.3RH
|       |    |        |     |        |     |        |           |        |     |        |
|       |  15.0     10.1  15.2     10.3  15.4     10.5        15.6     10.7  17.3     12.2
|       |    |        |     |        |     |        |           |        |     |        |
             +--15.1--+     +--15.3--+     +--15.5--+    0.4    +--15.7--+     +--17.2--+

****************************************************************************************************/

#define I2C_PCA8553_DEVICE_ADDR (0x38)
#define I2C_PCA8553_BUFFER_SIZE (20)

static struct bflb_device_s *uart0;
static struct bflb_device_s *i2c;

static uint8_t buffer[I2C_PCA8553_BUFFER_SIZE];
static int temperature = 2500;
static int humidity = 6000;

struct lcd_pos_s {
    uint8_t addr;
    uint8_t bit;
};

const struct lcd_pos_s lcd_pos[10][7] = {
    /* temperature */
    { {16, 1}, {11, 1}, {6, 1}, {1, 1}, {6, 0}, {16, 0}, {11, 0}, },
    { {16, 3}, {11, 3}, {6, 3}, {1, 3}, {6, 2}, {16, 2}, {11, 2}, },
    { {16, 5}, {11, 5}, {6, 5}, {1, 5}, {6, 4}, {16, 4}, {11, 4}, },
    { {16, 7}, {11, 7}, {6, 7}, {1, 7}, {6, 6}, {16, 6}, {11, 6}, },
    { {17, 1}, {12, 1}, {7, 1}, {2, 1}, {7, 0}, {17, 0}, {12, 0}, },
    /* humidity */
    { {0, 1}, {5, 1}, {10, 1}, {15, 1}, {15, 0}, {5, 0}, {10, 0}, },
    { {0, 3}, {5, 3}, {10, 3}, {15, 3}, {15, 2}, {5, 2}, {10, 2}, },
    { {0, 5}, {5, 5}, {10, 5}, {15, 5}, {15, 4}, {5, 4}, {10, 4}, },
    { {0, 7}, {5, 7}, {10, 7}, {15, 7}, {15, 6}, {5, 6}, {10, 6}, },
    { {2, 2}, {7, 2}, {12, 2}, {17, 2}, {17, 3}, {7, 3}, {12, 3}, },
};

/*                           0     1     2     3     4     5     6     7     8     9     A     b     C     d     E     F     -       */
const uint8_t lcd_num[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x40, 0x00};

void pca8553_number_to_buffer(int idx, uint8_t num, uint8_t *buff)
{
    if (idx >= sizeof(lcd_pos) / sizeof(lcd_pos[0])) {
        return;
    }
    if (num >= sizeof(lcd_num) / sizeof(lcd_num[0])) {
        return;
    }
    num = lcd_num[num];
    for (int i = 0; i < sizeof(lcd_pos[0]) / sizeof(lcd_pos[0][0]); i++) {
        if ((num >> i) & 1) {
            buff[lcd_pos[idx][i].addr] |= (1 << (lcd_pos[idx][i].bit));
        } else {
            buff[lcd_pos[idx][i].addr] &= ~(1 << (lcd_pos[idx][i].bit));
        }
    }
}

static void pca8553_init(void)
{
    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr;

    /* common i2c data init */
    msgs[0].addr = I2C_PCA8553_DEVICE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &subaddr;
    msgs[0].length = 1;
    msgs[1].addr = I2C_PCA8553_DEVICE_ADDR;
    msgs[1].flags = 0;
    msgs[1].buffer = buffer;

    /* reset: [0x00]=0x2C */
    subaddr = 0x00;
    buffer[0] = 0x2C;
    msgs[1].length = 1;
    bflb_i2c_transfer(i2c, msgs, 2);
    bflb_mtimer_delay_ms(1);

    /* release reset: [0x00]=0x00 */
    subaddr = 0x00;
    buffer[0] = 0x00;
    msgs[1].length = 1;
    bflb_i2c_transfer(i2c, msgs, 2);
    bflb_mtimer_delay_ms(1);

    /* ctrl: */
    subaddr = 0x01;
    buffer[0] = 0x1D; /* [4:2]:256KHz, [1]:enable internal oscillator, [0]:clock signal available on pin CLK */
    buffer[1] = 0x11; /* [4]:enhanced power drive, [3:2]:COM0~3, [1]:1/3 bias, [0]:display enable */
    buffer[2] = 0x00; /* [2:1]:blink off, [0]:line inversion */
    msgs[1].length = 3;
    bflb_i2c_transfer(i2c, msgs, 2);
    bflb_mtimer_delay_ms(1);
}

static void pca8553_display(uint8_t *data)
{
    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr = 0x04;

    msgs[0].addr = I2C_PCA8553_DEVICE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &subaddr;
    msgs[0].length = 1;
    msgs[1].addr = I2C_PCA8553_DEVICE_ADDR;
    msgs[1].flags = 0;
    msgs[1].buffer = data;
    msgs[1].length = 20;
    bflb_i2c_transfer(i2c, msgs, 2);
}

static void user_display(uint32_t t, uint32_t h, uint8_t *b)
{
    int i;
    int zero_flag;
    int val[5];

    /* temperature */
    val[0] = t / 10000;
    val[1] = (t % 10000) / 1000;
    val[2] = (t % 1000) / 100;
    val[3] = (t % 100) / 10;
    val[4] = t % 10;
    zero_flag = 0;
    i = 0;
    while (i < 5) {
        if ((val[i] == 0) && (zero_flag == 0)) {
            pca8553_number_to_buffer(i, 17, b);
        } else {
            zero_flag = 1;
            pca8553_number_to_buffer(i, val[i], b);
        }
        i++;
    }

    /* humidity */
    val[0] = h / 10000;
    val[1] = (h % 10000) / 1000;
    val[2] = (h % 1000) / 100;
    val[3] = (h % 100) / 10;
    val[4] = h % 10;
    zero_flag = 0;
    i = 0;
    while (i < 5) {
        if ((val[i] == 0) && (zero_flag == 0)) {
            pca8553_number_to_buffer(i + 5, 17, b);
        } else {
            zero_flag = 1;
            pca8553_number_to_buffer(i + 5, val[i], b);
        }
        i++;
    }
}

int main(void)
{
    int ch;
    uint64_t time_pre = 0;
    uint64_t time_now = 0;

    board_init();
    board_i2c0_gpio_init();

    i2c = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(i2c, 400 * 1000);
    uart0 = bflb_device_get_by_name("uart0");
    shell_init();
    pca8553_init();
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x11; /* show humidity char and point */
    buffer[1] = 0x11; /* show temperature char and point  */
    buffer[2] = 0x09; /* show unit of degree and RH */
    user_display(temperature, humidity, buffer);

    while (1) {
        if((ch = bflb_uart_getchar(uart0)) != -1)
        {
            shell_handler(ch);
        }
        time_now = bflb_mtimer_get_time_ms();
        if (time_now > (time_pre + 10)) {
            time_pre = time_now;
            pca8553_display(buffer);
            if (++temperature > 99999) {
                temperature = 0;
            }
            if (++humidity > 99999) {
                humidity = 0;
            }
            user_display(temperature, humidity, buffer);
        }
    }

    return 0;
}

void shell_cfg_temperature(int argc, char **argv)
{
    int val;

    if (argc != 2) {
        printf("Usage: temp 12345\r\n");
        return;
    }
    val = atoi(argv[1]);
    if ((val < -9999) || (val > 99999)) {
        printf("Usage: temp number\r\nnumber must be in [-9999, 99999]\r\nnumber = temperature * 100\r\n");
        return;
    }
    printf("config temperature to to %d\r\n", val);
    temperature = val;
    user_display(temperature, humidity, buffer);
}
SHELL_CMD_EXPORT_ALIAS(shell_cfg_temperature, temp, shell cfg temperature.);

void shell_cfg_humidity(int argc, char **argv)
{
    int val;

    if (argc != 2) {
        printf("Usage: humi 12345\r\n");
        return;
    }
    val = atoi(argv[1]);
    if ((val < -9999) || (val > 99999)) {
        printf("Usage: humi number\r\nnumber must be in [-9999, 99999]\r\nnumber = humidity * 100\r\n");
        return;
    }
    printf("config humidity to %d\r\n", val);
    humidity = val;
    user_display(temperature, humidity, buffer);
}
SHELL_CMD_EXPORT_ALIAS(shell_cfg_humidity, humi, shell cfg humidity.);

void shell_cfg_buffer(int argc, char **argv)
{
    int idx, val;

    if (argc != 3) {
        printf("Usage: buff index number\r\n");
        return;
    }
    idx = atoi(argv[1]);
    val = atoi(argv[2]);
    if ((idx < 0) || (idx > I2C_PCA8553_BUFFER_SIZE)) {
        printf("index must be in [0:%d]\r\n", I2C_PCA8553_BUFFER_SIZE - 1);
        return;
    }
    if ((val < 0) || (val > 0xFF)) {
        printf("value must be in [0:255]\r\n");
        return;
    }
    printf("config buffer[%d] to 0x%02X\r\n", idx, val);
    buffer[idx] = val;
}
SHELL_CMD_EXPORT_ALIAS(shell_cfg_buffer, buff, shell cfg buffer.);

void shell_cfg_number(int argc, char **argv)
{
    int idx, num;

    if (argc != 3) {
        printf("Usage: num index number\r\n");
        return;
    }
    idx = atoi(argv[1]);
    num = atoi(argv[2]);
    if ((idx < 0) || (idx >= sizeof(lcd_pos) / sizeof(lcd_pos[0]))) {
        printf("index must be in [0:%d]\r\n", sizeof(lcd_pos) / sizeof(lcd_pos[0]) - 1);
        return;
    }
    if ((num < 0) || (num >= (sizeof(lcd_num) / sizeof(lcd_num[0])))) {
        printf("number must be in [0:%d]\r\n", sizeof(lcd_num) / sizeof(lcd_num[0]) - 1);
        return;
    }
    printf("config number[%d] to 0x%02X\r\n", idx, num);
    pca8553_number_to_buffer(idx, num, buffer);
}
SHELL_CMD_EXPORT_ALIAS(shell_cfg_number, num, shell cfg number.);
