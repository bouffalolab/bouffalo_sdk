#include "bflb_ir.h"
#include "board.h"

#if defined(BL602) || defined(BL702) || defined(BL808) || defined(BL702L)
#define TEST_IR_TX
#endif

#if defined(BL602) || defined(BL702) || defined(BL808) || defined(BL616)
#define TEST_IR_RX
#endif

#ifdef TEST_IR_TX
struct bflb_device_s *irtx;
#endif
#ifdef TEST_IR_RX
struct bflb_device_s *irrx;
#endif

/* main */
int main(void)
{
    uint32_t i;

    board_init();

    printf("IR SWM case:\r\n");

    board_ir_gpio_init();

#ifdef TEST_IR_TX
    uint16_t tx_buffer[] = { 1777, 1777, 3555, 3555, 1777, 1777, 1777, 1777, 1777, 1777,
                             3555, 1777, 1777, 1777, 1777, 3555, 3555, 1777, 1777, 3555, 1777 };
    struct bflb_ir_tx_config_s tx_cfg;

    irtx = bflb_device_get_by_name("irtx");

    /* TX init */
    tx_cfg.tx_mode = IR_TX_SWM;
    bflb_ir_tx_init(irtx, &tx_cfg);
#endif

#ifdef TEST_IR_RX
    uint16_t rx_buffer[30];
    uint8_t rx_len;
    struct bflb_ir_rx_config_s rx_cfg;

    irrx = bflb_device_get_by_name("irrx");

    /* RX init */
    rx_cfg.rx_mode = IR_RX_SWM;
    rx_cfg.input_inverse = true;
    rx_cfg.deglitch_enable = false;
    rx_cfg.end_threshold = 3999;
    bflb_ir_rx_init(irrx, &rx_cfg);

    /* Enable rx, wait for sending */
    bflb_ir_rx_enable(irrx, true);
#endif

#ifdef TEST_IR_TX
    /* Send */
    bflb_ir_swm_send(irtx, tx_buffer, sizeof(tx_buffer) / sizeof(tx_buffer[0]));
    printf("Send bit: %d, value:\r\n", sizeof(tx_buffer) / sizeof(tx_buffer[0]));
    for (i = 0; i < sizeof(tx_buffer) / sizeof(tx_buffer[0]); i++) {
        printf("%d ", tx_buffer[i]);
    }
    printf("\r\n");
#endif

#ifdef TEST_IR_RX
    /* Receive */
    rx_len = bflb_ir_swm_receive(irrx, rx_buffer, 30);

    /* Print data received */
    printf("Receive bit: %d, value:\r\n", rx_len);
    for (i = 0; i < rx_len; i++) {
        printf("%d ", rx_buffer[i]);
    }
    printf("\r\n");
#endif

    printf("end\r\n");

    while (1) {
    }
}
