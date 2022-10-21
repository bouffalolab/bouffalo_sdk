#include "bflb_ir.h"
#include "board.h"

struct bflb_device_s *irtx;
struct bflb_device_s *irrx;

/* main */
int main(void)
{
    uint32_t tx_buffer[1] = {0xE916FF00};
    uint64_t rx_data;
    uint8_t rx_len;
    struct bflb_ir_tx_config_s tx_cfg;
    struct bflb_ir_rx_config_s rx_cfg;
    
    board_init();
    
    printf("IR NEC case:\n\r");
    
    board_ir_gpio_init();

    irtx = bflb_device_get_by_name("irtx");
    irrx = bflb_device_get_by_name("irrx");
    
    /* TX init */
    tx_cfg.tx_mode = IR_TX_NEC;
    bflb_ir_tx_init(irtx, &tx_cfg);
    
    /* RX init */
    rx_cfg.rx_mode = IR_RX_NEC;
    rx_cfg.input_inverse = true;
    rx_cfg.deglitch_enable = false;
    bflb_ir_rx_init(irrx, &rx_cfg);
    
    /* Enable rx, wait for sending */
    bflb_ir_rx_enable(irrx, true);
    
    /* Send */
    bflb_ir_send(irtx, tx_buffer, 1);
    
    /* Receive */
    rx_len = bflb_ir_receive(irrx, &rx_data);
    
    /* Check data received */
    if (rx_data != tx_buffer[0]) {
        printf("Data error! receive bit: %d, value: %016llx\n\r", rx_len, rx_data);
    } else {
        printf("Success\n\r");
    }

    printf("end\n\r");

    while (1) {
    }
}
