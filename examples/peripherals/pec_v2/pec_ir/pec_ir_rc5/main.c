#include "board.h"
#include "bflb_pec_v2_instance.h"

struct bflb_device_s *pec_ir;
int ret;

struct bflb_pec_ir_s ir_cfg = {
    .mem = 0,               /*!< memory address of first instruction */ 
    .div = 1403,            /*!< ir clock dividor, 160MHz / 1404 /（2 + 1）= 38KHz */
    .carrier_low = 2,       /*!< carrier low level time in erery carrier period, max 64, duty = 1 / (2 + 1) = 33% */
    .carrier_high = 1,      /*!< carrier high level time in erery carrier period, max 64 */
    .pin = PEC_IR_PIN % 32, /*!< pin of ir */
    .idle_level = 1,        /*!< pin level when idle */
};

struct bflb_pec_ir_rc5_timing_s rc5_timing = {
    .data0_carrier = 34, /*!< data_0 carrier length, 0.889ms * 38KHz = 34 */
    .data0_idle = 34,    /*!< data_0 idle length, 0.889ms * 38KHz = 34 */
    .data1_carrier = 34, /*!< data_1 carrier length, 0.889ms * 38KHz = 34 */
    .data1_idle = 34,    /*!< data_1 idle length, 0.889ms * 38KHz = 34 */
};

uint32_t ir_buff[] = {
    0x12345678, 0x9ABCDEF0, 0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00, 0x00000000, 0x11111111,
    0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888, 0x99999999,
    0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0x0F1E2D3C, 0x4B5A6978,
    0x8796A5B4, 0xC3D2E1F0, 0x14253647, 0x58691728, 0x39124578, 0x13467923, 0x56893728, 0x19467281};

uint32_t bit_buff[] = {32, 14, 0, 1, 2, 5, 8, 26, 32, 87, 1024, 1000, 500, 200, 300, 785, 92};

int main(void)
{
    board_init();
    board_pec_ir_gpio_init();
    pec_ir = bflb_device_get_by_name("pec_sm1");
    printf("pec_ir = 0x%08lX\r\n", pec_ir);

    bflb_pec_ir_rc5_init(pec_ir, &ir_cfg);
    bflb_pec_ir_rc5_set_timing(pec_ir, &rc5_timing);
    for (uint32_t i = 0; i < sizeof(bit_buff) / sizeof(bit_buff[0]); ) {
        ret = bflb_pec_ir_rc5_is_busy(pec_ir);
        if (ret == 0) {
            bflb_pec_ir_rc5_send(pec_ir, ir_buff, bit_buff[i++]);
        } else {
            bflb_mtimer_delay_ms(100);
        }
    }
    return 0;
}
