#include "usbd_core.h"
#include "bflb_mtimer.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

extern void cdc_ecm_init(void);
extern void ecm_emac_poll(void);

int main(void)
{
    board_init();
    /* emac gpio init */
    board_emac_gpio_init();

    bflb_mtimer_delay_ms(100);

    LOG_I("ECM EMAC test!\r\n");

    cdc_ecm_init();
    
    while (1) {
        ecm_emac_poll();
        bflb_mtimer_delay_us(1);
    }
}
