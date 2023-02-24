#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

#define EEPROM_TRANSFER_LENGTH 32
#define EEPROM_SELECT_PAGE0    (0 << 5)
#define EEPROM_SELECT_PAGE1    (1 << 5)

static struct bflb_device_s *i2c0;
static volatile uint32_t txFifoFlag = 0;
static volatile uint32_t rxFifoFlag = 0;

void i2c_isr(int irq, void *arg)
{
    uint32_t status;

    status = bflb_i2c_get_intstatus(i2c0);

    if (status & I2C_INTSTS_END) {
        bflb_i2c_int_mask(i2c0, I2C_INTEN_END, true);
        printf("Transfer end interrupt\r\n");
    }
    if (status & I2C_INTSTS_TX_FIFO) {
        bflb_i2c_int_mask(i2c0, I2C_INTEN_TX_FIFO, true);
        txFifoFlag = 1;
    }
    if (status & I2C_INTSTS_RX_FIFO) {
        bflb_i2c_int_mask(i2c0, I2C_INTEN_RX_FIFO, true);
        rxFifoFlag = 1;
    }
    if (status & I2C_INTSTS_NACK) {
        bflb_i2c_int_mask(i2c0, I2C_INTEN_NACK, true);
        printf("Error! NACK interrupt\r\n");
    }
    if (status & I2C_INTSTS_ARB) {
        bflb_i2c_int_mask(i2c0, I2C_INTEN_ARB, true);
        printf("Error! Arbitration lost interrupt\r\n");
    }
    if (status & I2C_INTSTS_FER) {
        bflb_i2c_int_mask(i2c0, I2C_INTEN_FER, true);
        printf("Error! TX/RX FIFO error interrupt\r\n");
    }
}

int main(void)
{
    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);

    /* Set i2c interrupt */
    bflb_i2c_int_mask(i2c0, I2C_INTEN_END | I2C_INTEN_TX_FIFO | I2C_INTEN_RX_FIFO | I2C_INTEN_NACK | I2C_INTEN_ARB | I2C_INTEN_FER, false);
    bflb_irq_attach(i2c0->irq_num, i2c_isr, NULL);
    bflb_irq_enable(i2c0->irq_num);

    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr[2] = { 0x00, 0x00 };

    uint8_t write_data[256];
    uint8_t read_data[256];

    /* Write and read buffer init */
    for (size_t i = 0; i < 256; i++) {
        write_data[i] = i;
        read_data[i] = 0;
    }

    /* Write page 0 */
    subaddr[1] = EEPROM_SELECT_PAGE0;

    msgs[0].addr = 0x50;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;

    msgs[1].addr = 0x50;
    msgs[1].flags = 0;
    msgs[1].buffer = write_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;

    bflb_i2c_transfer(i2c0, msgs, 2);
    if (txFifoFlag) {
        printf("TX FIFO Ready interrupt generated\r\n");
        txFifoFlag = 0;
    }
    if (rxFifoFlag) {
        printf("RX FIFO Ready interrupt generated\r\n");
        rxFifoFlag = 0;
    }
    printf("write over\r\n\r\n");
    bflb_mtimer_delay_ms(100);

    /* Unmask interrupt */
    bflb_i2c_int_mask(i2c0, I2C_INTEN_END | I2C_INTEN_TX_FIFO | I2C_INTEN_RX_FIFO | I2C_INTEN_NACK | I2C_INTEN_ARB | I2C_INTEN_FER, false);

    /* Write page 1 */
    subaddr[1] = EEPROM_SELECT_PAGE1;

    msgs[1].addr = 0x50;
    msgs[1].flags = 0;
    msgs[1].buffer = write_data + EEPROM_TRANSFER_LENGTH;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;

    bflb_i2c_transfer(i2c0, msgs, 2);
    if (txFifoFlag) {
        printf("TX FIFO Ready interrupt generated\r\n");
        txFifoFlag = 0;
    }
    if (rxFifoFlag) {
        printf("RX FIFO Ready interrupt generated\r\n");
        rxFifoFlag = 0;
    }
    printf("write over\r\n\r\n");
    bflb_mtimer_delay_ms(100);

    /* Unmask interrupt */
    bflb_i2c_int_mask(i2c0, I2C_INTEN_END | I2C_INTEN_TX_FIFO | I2C_INTEN_RX_FIFO | I2C_INTEN_NACK | I2C_INTEN_ARB | I2C_INTEN_FER, false);

    /* Read page 0 */
    subaddr[1] = EEPROM_SELECT_PAGE0;

    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = read_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
    bflb_i2c_transfer(i2c0, msgs, 2);
    if (txFifoFlag) {
        printf("TX FIFO Ready interrupt generated\r\n");
        txFifoFlag = 0;
    }
    if (rxFifoFlag) {
        printf("RX FIFO Ready interrupt generated\r\n");
        rxFifoFlag = 0;
    }
    printf("read over\r\n\r\n");
    bflb_mtimer_delay_ms(100);

    /* Unmask interrupt */
    bflb_i2c_int_mask(i2c0, I2C_INTEN_END | I2C_INTEN_TX_FIFO | I2C_INTEN_RX_FIFO | I2C_INTEN_NACK | I2C_INTEN_ARB | I2C_INTEN_FER, false);

    /* Read page 1 */
    subaddr[1] = EEPROM_SELECT_PAGE1;

    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = read_data + EEPROM_TRANSFER_LENGTH;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
    bflb_i2c_transfer(i2c0, msgs, 2);
    if (txFifoFlag) {
        printf("TX FIFO Ready interrupt generated\r\n");
        txFifoFlag = 0;
    }
    if (rxFifoFlag) {
        printf("RX FIFO Ready interrupt generated\r\n");
        rxFifoFlag = 0;
    }
    printf("read over\r\n\r\n");
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
