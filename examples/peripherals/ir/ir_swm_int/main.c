#include "bflb_ir.h"
#include "board.h"

#if defined(BL616D)
#define TEST_IR_TX
#endif

#if defined(BL616) || defined(BL616D) || defined(BL616L)
#define TEST_IR_RX
#endif

#ifdef TEST_IR_TX
struct bflb_device_s *irtx;
#endif
#ifdef TEST_IR_RX
struct bflb_device_s *irrx;
#endif

#define MAX_TX_LENGTH 129
#define MAX_RX_LENGTH 300
volatile uint32_t rx_count = 0;
volatile uint32_t tx_count = 0;
volatile uint8_t rx_end = 0;
volatile uint8_t tx_end = 0;
volatile uint32_t tx_length = MAX_TX_LENGTH;

uint16_t min_data = 0;
uint16_t tx_buffer[MAX_TX_LENGTH];
uint16_t rx_buffer[MAX_RX_LENGTH];

#ifdef TEST_IR_TX
#define DIVIDE_ROUND(a, b) ((2 * a + b) / (2 * b))
void ir_send_start(struct bflb_device_s *dev, uint16_t *data, uint32_t length)
{
    bflb_ir_txfifo_clear(dev);

    min_data = data[0];
    for (uint32_t i = 1; i < length; i++) {
        if (min_data > data[i] && data[i] != 0) {
            min_data = data[i];
        }
    }
    if (min_data == 0) {
        min_data = 1;
    }

    bflb_ir_feature_control(dev, IR_CMD_SWM_SET_UNIT_DATA, min_data - 1);
    bflb_ir_feature_control(dev, IR_CMD_SWM_SET_DATA_LEN, (length - 1));
}
#endif

void ir_isr(int irq, void *arg)
{
#ifdef TEST_IR_TX
    uint32_t intstatus_tx = bflb_ir_get_txint_status(irtx);
    if (intstatus_tx & IR_TX_INTSTS_FIFO) {
        uint8_t fifo_cnt = bflb_ir_get_txfifo_cnt(irtx);
        uint32_t group_size = 2;
        uint32_t remain = tx_length - tx_count;
        uint32_t send_num = ((remain + group_size - 1) / group_size) > fifo_cnt ? fifo_cnt : ((remain + group_size - 1) / group_size);

        for (uint32_t i = 0; i < send_num; i++) {
            if (tx_count >= tx_length)
                break;
            uint32_t pwval = 0;
            uint32_t regval = 0;
            uint32_t real_group = (tx_length - tx_count) > group_size ? group_size : (tx_length - tx_count);
            for (uint8_t j = 0; j < real_group; j++) {
                uint32_t idx = tx_count + j;
                regval = (DIVIDE_ROUND(tx_buffer[idx], min_data) - 1) & 0xffff;
                pwval |= regval << (16 * j);
            }
            bflb_ir_feature_control(irtx, IR_CMD_SWM_WRITE_TX_FIFO, pwval);
            tx_count += real_group;
        }
        // printf("tx fifo interrupt\r\n");
    }
    if (intstatus_tx & IR_TX_INTSTS_END) {
        bflb_ir_txint_clear(irtx);
        tx_end = 1;
        bflb_ir_txint_mask(irtx, IR_TX_INTEN_FIFO, true);
        bflb_ir_txint_mask(irtx, IR_TX_INTEN_END, true);
        printf("tx end interrupt\r\n");
    }
#endif

#ifdef TEST_IR_RX
    uint32_t intstatus_rx = bflb_ir_get_rxint_status(irrx);
    if (intstatus_rx & IR_RX_INTSTS_FIFO) {
        int rx_fifo_count = bflb_ir_get_rxfifo_cnt(irrx);
        while (rx_fifo_count--) {
            rx_buffer[rx_count++] = bflb_ir_feature_control(irrx, IR_CMD_SWM_READ_RX_FIFO, 0);
        }
        // printf("rx fifo interrupt\r\n");
    }
    if (intstatus_rx & IR_RX_INTSTS_END) {
        bflb_ir_rxint_clear(irrx);
        rx_end = 1;
        int rx_fifo_count = bflb_ir_get_rxfifo_cnt(irrx);
        while (rx_fifo_count--) {
            rx_buffer[rx_count++] = bflb_ir_feature_control(irrx, IR_CMD_SWM_READ_RX_FIFO, 0);
        }
        printf("rx end interrupt\r\n");
    }
#endif
}

/* main */
int main(void)
{
    uint32_t i;

    board_init();

    printf("IR SWM interrupt case:\r\n");

    board_ir_gpio_init();

    for (i = 0; i < MAX_TX_LENGTH; i++) {
        tx_buffer[i] = (rand() % 2 == 0) ? 1778 : 3556;
    }

#ifdef TEST_IR_TX

    struct bflb_ir_tx_config_s tx_cfg;

    irtx = bflb_device_get_by_name("irtx");

    /* TX init */
    tx_cfg.tx_mode = IR_TX_SWM;
    tx_cfg.output_inverse = true;
    tx_cfg.fifo_threshold = 1;

    bflb_ir_tx_init(irtx, &tx_cfg);

    bflb_ir_txint_mask(irtx, IR_TX_INTEN_FIFO, false);
    bflb_ir_txint_mask(irtx, IR_TX_INTEN_END, false);

    ir_send_start(irtx, tx_buffer, MAX_TX_LENGTH);

#endif

#ifdef TEST_IR_RX

    struct bflb_ir_rx_config_s rx_cfg;

    irrx = bflb_device_get_by_name("irrx");

    /* RX init */
    rx_cfg.rx_mode = IR_RX_SWM;
    rx_cfg.input_inverse = true;
    rx_cfg.deglitch_enable = false;
    rx_cfg.fifo_threshold = 31;
    rx_cfg.end_threshold = 7000;

    bflb_ir_rx_init(irrx, &rx_cfg);

    bflb_ir_rxint_mask(irrx, IR_RX_INTEN_FIFO, false);
    bflb_ir_rxint_mask(irrx, IR_RX_INTEN_END, false);

#endif

    bflb_irq_attach(irrx->irq_num, ir_isr, NULL);
    bflb_irq_enable(irrx->irq_num);

#if defined TEST_IR_RX
    /* Enable rx interrupt, wait for sending */
    bflb_ir_rx_enable(irrx, true);
#endif

#if defined TEST_IR_TX
    /* Enable tx interrupt, wait for sending */
    bflb_ir_tx_enable(irtx, true);
#endif

#ifdef TEST_IR_TX
    /* Send */
    while (tx_end == 0) {}
    bflb_mtimer_delay_ms(1000);
    printf("Send bit: %d, value:\r\n", sizeof(tx_buffer) / sizeof(tx_buffer[0]));
    for (i = 0; i < sizeof(tx_buffer) / sizeof(tx_buffer[0]); i++) {
        printf("%d ", tx_buffer[i]);
    }
    printf("\r\n");
#endif

#ifdef TEST_IR_RX
    /* Receive */
    while (rx_end == 0) {}
    bflb_mtimer_delay_ms(1000);
    printf("Receive bit: %d, value:\r\n", rx_count);
    for (i = 0; i < rx_count; i++) {
        printf("%d ", rx_buffer[i]);
    }
    printf("\nend\r\n");
#endif
    while (1) {
    }
}
