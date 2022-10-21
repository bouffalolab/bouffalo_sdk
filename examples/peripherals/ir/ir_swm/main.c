#include "bflb_ir.h"
#include "board.h"

struct bflb_device_s *irtx;
struct bflb_device_s *irrx;

/* main */
int main(void)
{
    uint16_t tx_buffer[21] = {1777,1777,3555,3555,1777,1777,1777,1777,1777,1777,
                            3555,1777,1777,1777,1777,3555,3555,1777,1777,3555,1777};
    uint16_t rx_buffer[30];
    uint8_t rx_len;
    uint32_t i;
    struct bflb_ir_tx_config_s tx_cfg;
    struct bflb_ir_rx_config_s rx_cfg;

    board_init();
    
    printf("IR SWM case:\n\r");
    
    board_ir_gpio_init();

    irtx = bflb_device_get_by_name("irtx");
    irrx = bflb_device_get_by_name("irrx");
    
    /* TX init */
    tx_cfg.tx_mode = IR_TX_SWM;
    bflb_ir_tx_init(irtx, &tx_cfg);
    
    /* RX init */
    rx_cfg.rx_mode = IR_RX_SWM;
    rx_cfg.input_inverse = true;
    rx_cfg.deglitch_enable = false;
    rx_cfg.end_threshold = 3999;
    bflb_ir_rx_init(irrx, &rx_cfg);
    
    /* Enable rx, wait for sending */
    bflb_ir_rx_enable(irrx, true);
    
    /* Send */
    bflb_ir_swm_send(irtx, tx_buffer, 21);
    
    /* Receive */
    rx_len = bflb_ir_swm_receive(irrx, rx_buffer, 30);
    
    /* Print data received */
    printf("Receive bit: %d, value:\n\r", rx_len);
    for (i = 0; i < rx_len; i ++) {
        printf("%d ", rx_buffer[i]);
    }

    printf("\n\rend\n\r");

    while (1) {
    }
}
