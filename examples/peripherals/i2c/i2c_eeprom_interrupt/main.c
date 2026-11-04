#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

#define EEPROM_SLAVE_ADDRESS   (0x50)
#define EEPROM_TRANSFER_LENGTH (32)
#define EEPROM_SELECT_PAGE0    (0 << 5)
#define EEPROM_SELECT_PAGE1    (1 << 5)

static struct bflb_device_s *i2c0;

struct bflb_i2c_msg_s msgs[2];
uint8_t subaddr[2] = { 0x00, 0x00 };
uint8_t write_data[256];
uint8_t read_data[256];
volatile int flag_transfer_end = 0;
#if defined(BL616CL)
volatile uint32_t transfer_idx = 0;
#endif
volatile int flag_write_fifo = 0;

void i2c_isr(int irq, void *arg)
{
    uint32_t status;

    status = bflb_i2c_get_intstatus(i2c0) & I2C_INTEN_ALL;

    if (status & I2C_INTSTS_END) {
        bflb_i2c_int_clear(i2c0, I2C_INTCLR_END);
        printf("Transfer end interrupt\r\n");
        flag_transfer_end = 1;
        if (status & I2C_INTSTS_NACK) {
            bflb_i2c_int_clear(i2c0, I2C_INTCLR_NACK);
            printf("Error! NACK interrupt\r\n");
        }
    }
#if defined(BL616CL)
    if ((status & I2C_INTSTS_TX_FIFO) && (flag_write_fifo != 0)) {
        static uint32_t idx_tx = 0;
        idx_tx += bflb_i2c_fifo_write(i2c0, msgs[transfer_idx].buffer + idx_tx, msgs[transfer_idx].length - idx_tx);
        if (idx_tx >= msgs[transfer_idx].length) {
            bflb_i2c_int_mask(i2c0, I2C_INTEN_TX_FIFO, true);
            idx_tx = 0;
            flag_write_fifo = 0;
            if ((transfer_idx + 1) < (sizeof(msgs) / sizeof(msgs[0]))) {
                if (msgs[transfer_idx + 1].flags & I2C_M_READ) {
                    bflb_i2c_int_mask(i2c0, I2C_INTEN_RX_FIFO, false);
                } else {
                    flag_write_fifo = 1;
                    bflb_i2c_int_mask(i2c0, I2C_INTEN_TX_FIFO, false);
                }
                transfer_idx++;
            }
        }
    }
    if (status & I2C_INTSTS_RX_FIFO) {
        static uint32_t idx_rx = 0;
        idx_rx += bflb_i2c_fifo_read(i2c0, msgs[transfer_idx].buffer + idx_rx, msgs[transfer_idx].length - idx_rx);
        if (idx_rx >= msgs[transfer_idx].length) {
            bflb_i2c_int_mask(i2c0, I2C_INTEN_RX_FIFO, true);
            idx_rx = 0;
            if ((transfer_idx + 1) < (sizeof(msgs) / sizeof(msgs[0]))) {
                if (msgs[transfer_idx + 1].flags & I2C_M_READ) {
                    bflb_i2c_int_mask(i2c0, I2C_INTEN_RX_FIFO, false);
                } else {
                    bflb_i2c_int_mask(i2c0, I2C_INTEN_TX_FIFO, false);
                }
                transfer_idx++;
            }
        }
    }
#else
    if ((status & I2C_INTSTS_TX_FIFO) && (flag_write_fifo != 0)) {
        static uint32_t idx_tx = 0;
        idx_tx += bflb_i2c_fifo_write(i2c0, msgs[1].buffer + idx_tx, msgs[1].length - idx_tx);
        if (idx_tx >= msgs[1].length) {
            bflb_i2c_int_mask(i2c0, I2C_INTEN_TX_FIFO, true);
            idx_tx = 0;
            flag_write_fifo = 0;
        }
    }
    if (status & I2C_INTSTS_RX_FIFO) {
        static uint32_t idx_rx = 0;
        idx_rx += bflb_i2c_fifo_read(i2c0, msgs[1].buffer + idx_rx, msgs[1].length - idx_rx);
        if (idx_rx >= msgs[1].length) {
            bflb_i2c_int_mask(i2c0, I2C_INTEN_RX_FIFO, true);
            idx_rx = 0;
        }
    }
#endif
    if (status & I2C_INTSTS_ARB) {
        bflb_i2c_int_clear(i2c0, I2C_INTCLR_ARB);
        printf("Error! Arbitration lost interrupt\r\n");
    }
    if (status & I2C_INTSTS_FER) {
        bflb_i2c_int_mask(i2c0, I2C_INTEN_FER, true);
        printf("Error! TX/RX FIFO error interrupt\r\n");
    }
#if defined(BL616CL)
    if (status & I2C_INTSTS_TIMEOUT) {
        bflb_i2c_int_clear(i2c0, I2C_INTCLR_TIMEOUT);
        printf("timeout interrupt\r\n");
    }
#endif
}

int main(void)
{
    int ret;

    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);

    /* Write and read buffer init */
    for (size_t i = 0; i < 256; i++) {
        write_data[i] = i;
        read_data[i] = 0;
    }

    /* Set i2c interrupt */
#if defined(BL616CL)
    bflb_i2c_int_mask(i2c0, I2C_INTEN_END | I2C_INTEN_NACK | I2C_INTEN_ARB | I2C_INTEN_FER | I2C_INTEN_TIMEOUT, false);
#else
    bflb_i2c_int_mask(i2c0, I2C_INTEN_END | I2C_INTEN_NACK | I2C_INTEN_ARB | I2C_INTEN_FER, false);
#endif
    bflb_irq_attach(i2c0->irq_num, i2c_isr, NULL);
    bflb_irq_enable(i2c0->irq_num);

    /* Write page 0 */
    subaddr[1] = EEPROM_SELECT_PAGE0;
    msgs[0].addr = EEPROM_SLAVE_ADDRESS;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;
    msgs[1].addr = EEPROM_SLAVE_ADDRESS;
    msgs[1].flags = I2C_M_INT;
    msgs[1].buffer = write_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
#if defined(BL616CL)
    transfer_idx = 0;
#endif
    flag_write_fifo = 1;
    ret = bflb_i2c_transfer(i2c0, msgs, 2);
    if (ret) {
        printf("bflb_i2c_transfer write error, ret = %d\r\n", ret);
        return ret;
    }
    while (flag_transfer_end == 0);
    flag_transfer_end = 0;
    printf("page0 write over\r\n\r\n");
    bflb_mtimer_delay_ms(100);

    /* Write page 1 */
    subaddr[1] = EEPROM_SELECT_PAGE1;
    msgs[1].addr = EEPROM_SLAVE_ADDRESS;
    msgs[1].flags = I2C_M_INT;
    msgs[1].buffer = write_data + EEPROM_TRANSFER_LENGTH;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
#if defined(BL616CL)
    transfer_idx = 0;
#endif
    flag_write_fifo = 1;
    ret = bflb_i2c_transfer(i2c0, msgs, 2);
    if (ret) {
        printf("bflb_i2c_transfer write error, ret = %d\r\n", ret);
        return ret;
    }
    while (flag_transfer_end == 0);
    flag_transfer_end = 0;
    printf("page1 write over\r\n\r\n");
    bflb_mtimer_delay_ms(100);

    /* Read page 0 */
    subaddr[1] = EEPROM_SELECT_PAGE0;
    msgs[1].addr = EEPROM_SLAVE_ADDRESS;
    msgs[1].flags = I2C_M_INT | I2C_M_READ;
    msgs[1].buffer = read_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
#if defined(BL616CL)
    transfer_idx = 0;
    flag_write_fifo = 1;
#endif
    ret = bflb_i2c_transfer(i2c0, msgs, 2);
    if (ret) {
        printf("bflb_i2c_transfer read error, ret = %d\r\n", ret);
        return ret;
    }
    while (flag_transfer_end == 0);
    flag_transfer_end = 0;
    printf("page0 read over\r\n\r\n");
    bflb_mtimer_delay_ms(100);

    /* Read page 1 */
    subaddr[1] = EEPROM_SELECT_PAGE1;
    msgs[1].addr = EEPROM_SLAVE_ADDRESS;
    msgs[1].flags = I2C_M_INT | I2C_M_READ;
    msgs[1].buffer = read_data + EEPROM_TRANSFER_LENGTH;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
#if defined(BL616CL)
    transfer_idx = 0;
    flag_write_fifo = 1;
#endif
    ret = bflb_i2c_transfer(i2c0, msgs, 2);
    if (ret) {
        printf("bflb_i2c_transfer read error, ret = %d\r\n", ret);
        return ret;
    }
    while (flag_transfer_end == 0);
    flag_transfer_end = 0;
    printf("page1 read over\r\n\r\n");
    bflb_mtimer_delay_ms(100);

    /* Check read data */
    for (uint8_t i = 0; i < 2 * EEPROM_TRANSFER_LENGTH; i++) {
        if (write_data[i] != read_data[i]) {
            printf("check fail, %d write: %02x, read: %02x\r\n", i, write_data[i], read_data[i]);
        }
    }
    printf("check over\r\n");
    printf("end\r\n");

    while (1) {
    }
}
