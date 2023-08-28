#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_wo.h"

#define PIN_USE     (23)
#define DATA_LENGTH (256)
#define BUFF_LENGTH (DATA_LENGTH * 10)
uint8_t byte_arr[DATA_LENGTH];
uint16_t buff[BUFF_LENGTH];
struct bflb_device_s *wo;

struct bflb_wo_cfg_s cfg = {
    .code_total_cnt = 20,
    .code0_first_cnt = 0,
    .code1_first_cnt = 0,
    .code0_first_level = 0,
    .code1_first_level = 0,
    .idle_level = 1,
    .fifo_threshold = 0,
    .mode = WO_MODE_SET_CLR,
};

void wo_byte_to_buff(uint8_t *c, uint16_t *buff, uint32_t len, uint8_t pin)
{
    pin = pin % 8;
    uint16_t val1 = (1 << pin);
    uint16_t val0 = (1 << (pin + 8));
    for (uint32_t i = 0; i < len; i++) {
        buff[i * 10] = val0;
        for (uint32_t j = 0; j < 8; j++) {
            if ((c[i] >> j) & 1) {
                buff[i * 10 + 1 + j] = val1;
            } else {
                buff[i * 10 + 1 + j] = val0;
            }
        }
        buff[i * 10 + 9] = val1;
    }
}

void data_init(void)
{
    for (uint32_t i = 0; i < DATA_LENGTH; i++) {
        byte_arr[i] = i;
    }
    wo_byte_to_buff(byte_arr, buff, DATA_LENGTH, PIN_USE);
}

int main(void)
{
    board_init();
    wo = bflb_device_get_by_name("wo");
    bflb_wo_init(wo, &cfg);
    bflb_wo_pin_init(wo, PIN_USE, WO_MODE_SET_CLR);
    data_init();
    bflb_wo_push_fifo(wo, buff, BUFF_LENGTH);
}
