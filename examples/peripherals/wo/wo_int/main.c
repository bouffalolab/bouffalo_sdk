#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_wo.h"

static uint16_t data_write_arr[256];

struct bflb_device_s *wo;

void wo_isr(int irq, void *arg)
{
    uint32_t int_status = bflb_wo_get_int_status(wo);

    if (int_status & WO_INT_END) {
        printf("interrupt end!\n");
        bflb_wo_int_clear(wo, WO_INT_END);
    }
    if (int_status & WO_INT_FIFO) {
        printf("interrupt fifo!\n");
        bflb_wo_int_clear(wo, WO_INT_FIFO | WO_INT_FER);
        bflb_wo_disable(wo);
        bflb_wo_int_mask(wo, WO_INT_FIFO);
    }
    if (int_status & WO_INT_FER) {
        printf("interrupt fer!\n");
        bflb_wo_int_clear(wo, WO_INT_FER);
    }
}

struct bflb_wo_cfg_s cfg = {
    .code_total_cnt = 10,
    .code0_first_cnt = 6,
    .code1_first_cnt = 2,
    .code0_first_level = 1,
    .code1_first_level = 1,
    .idle_level = 0,
    .fifo_threshold = 16,
    .mode = WO_MODE_WRITE,
};

int main(void)
{
    board_init();
    wo = bflb_device_get_by_name("wo");
    bflb_wo_init(wo, &cfg);
    bflb_wo_int_unmask(wo, WO_INT_END);
    bflb_wo_int_mask(wo, WO_INT_FIFO | WO_INT_FER);
    bflb_irq_attach(wo->irq_num, wo_isr, NULL);
    bflb_irq_enable(wo->irq_num);

    bflb_wo_disable(wo);
    bflb_wo_push_fifo(wo, data_write_arr, 5);
    bflb_wo_enable(wo);
    bflb_mtimer_delay_us(50);

    bflb_wo_disable(wo);
    bflb_wo_push_fifo(wo, data_write_arr, 128);
    bflb_wo_int_mask(wo, WO_INT_END);
    bflb_wo_int_unmask(wo, WO_INT_FIFO | WO_INT_FER);
    bflb_wo_enable(wo);
    bflb_mtimer_delay_us(200);
    for (uint32_t i = 0; i < 130; i++) {
        *(volatile uint32_t *)0x20000B04 = i;
    }
}
