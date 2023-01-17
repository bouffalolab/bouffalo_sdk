#include "bflb_mtimer.h"
#include "bflb_iso11898.h"
#include "board.h"

struct bflb_device_s *iso11898;

struct bflb_iso11898_config_s cfg = {
    .prescaler = 0,                   /*!< specifies the length of a time quantum, ranges from 1 to 63, tq=tclk*2*(prescaler+1) */
    .mode = ISO11898_MODE_NORMAL,     /*!< specifies the CAN operating mode. @ref ISO11898_MODE_XXX */
    .sample = ISO11898_SAMPLE_TRIPLE, /*!< specifies the number of sample times. @ref ISO11898_SAMPLE_XXX */
    .sjw = ISO11898_SJW_2TQ,          /*!< specifies the maximum number of time quanta the ISO11898 hardware is allowed to lengthen or shorten a bit to perform resynchronization. @ref ISO11898_SJW_XXX */
    .bs1 = ISO11898_BS1_9TQ,          /*!< specifies the number of time quanta in Bit Segment 1. @ref ISO11898_BS1_XXX */
    .bs2 = ISO11898_BS2_6TQ,          /*!< specifies the number of time quanta in Bit Segment 2. @ref ISO11898_BS2_XXX */
};

struct bflb_iso11898_msg_s msg_tx = {
    .std_id = 0x59,                                             /*!< specifies the standard identifier, this parameter can be a value between 0 to 0x7FF */
    .ext_id = 0x154863,                                         /*!< specifies the extended identifier, this parameter can be a value between 0 to 0x1FFFFFFF */
    .id_type = ISO11898_ID_EXTENDED,                            /*!< specifies the type of identifier for the message that will be send or received. @ref ISO11898_ID_XXX */
    .rtr = ISO11898_RTR_DATA,                                   /*!< specifies the type of frame for the message that will be send or received, @ref ISO11898_RTR_XXX */
    .dlc = 8,                                                   /*!< specifies the length of the frame that will be send or received, this parameter can be a value between 0 to 8 */
    .data = { 0x2E, 0x53, 0xF6, 0x81, 0xD4, 0xC0, 0x7A, 0xB9 }, /*!< contains the data to be send or received, it ranges from 0 to 0xFF */
};

void iso11898_isr(int irq, void *arg)
{
    uint32_t int_status = bflb_iso11898_get_int_status(iso11898);

    if (int_status & ISO11898_INTSTS_RX) {
        struct bflb_iso11898_msg_s msg;
        bflb_iso11898_recv(iso11898, &msg, 100);
        printf("Receive ");
        if (msg.id_type == ISO11898_ID_STANDARD) {
            printf("standard frame, ID = 0x%03lX, ", msg.std_id);
        } else {
            printf("extend frame, ID = 0x%08lX, ", msg.ext_id);
        }
        if (msg.rtr == ISO11898_RTR_DATA) {
            printf("data frame[%d]\r\n", msg.dlc);
            for (uint32_t i = 0; i < msg.dlc; i++) {
                printf("%02X ", msg.data[i]);
            }
            printf("\r\n\r\n");
        } else {
            printf("remote frame\r\n\r\n");
        }
    }
    if (int_status & ISO11898_INTSTS_TX) {
        printf("tx succeed!\r\n\r\n");
    }
}

int main(void)
{
    board_init();
    board_iso11898_gpio_init();

    iso11898 = bflb_device_get_by_name("iso11898");

    bflb_iso11898_init(iso11898, &cfg);
    bflb_irq_attach(iso11898->irq_num, iso11898_isr, NULL);
    bflb_iso11898_txint_mask(iso11898, false);
    bflb_iso11898_rxint_mask(iso11898, false);
    bflb_irq_enable(iso11898->irq_num);
    bflb_iso11898_send(iso11898, &msg_tx, 1000);

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
