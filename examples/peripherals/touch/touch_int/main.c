#include "bflb_touch.h"
#include "bflb_peri.h"
#include "bflb_clock.h"
#include "bflb_reset.h"
#include "shell.h"
#include "bflb_uart.h"
#include "board.h"

static struct bflb_device_s *touch;

void touch_channel_init()
{
    int channel;
    struct bflb_touch_chan_config_s ch_cfg;

    for (channel = 0; channel < TOUCH_CHANNEL_MAX; channel++) {
        ch_cfg.ch[channel] = TOUCH_CHANNEL_HIGHZ_CFG_HIGHZ;
    }
    /* mask all interrupt */
    bflb_touch_int_mask(touch, TOUCH_INT_ALL, 1);

    /* init test channel */
    for (channel = 3; channel <= 7; channel++) {
        bflb_touch_int_mask(touch, (1 << channel), 0);
        bflb_touch_set_vth_data(touch, channel, 50);
        ch_cfg.ch[channel] = TOUCH_CHANNEL_HIGHZ_CFG_GND;
    }
    bflb_touch_channel_init(touch, &ch_cfg, TOUCH_CHANNEL_NONE);
}

void touch_dump_raw_lta_flt()
{
    uint32_t raw_data, lta_data, flt_data;
    int i;

    for (i = 0; i < TOUCH_CHANNEL_MAX; i++) {
        raw_data = bflb_touch_get_raw_data(touch, i);
        lta_data = bflb_touch_get_lta_data(touch, i);
        flt_data = bflb_touch_get_flt_data(touch, i);
        printf("Touch channel %d: raw=%d lta=%d flt=%d\r\n", i, raw_data, lta_data, flt_data);
    }
}

void touch_calibration()
{
    struct bflb_touch_config_s cal_cfg = {
        .clk_sel = TOUCH_CLK_SEL_32M,
        .clk_div = TOUCH_CLK_DIV_16,
        .cur_sel = TOUCH_CUR_SEL_AON_BG,
        .cal_en = 1,
        .work_mode = TOUCH_WORK_MODE_SELF_CAP,
        .run_mode = TOUCH_RUN_MODE_SINGLE_SHOT,
        .ulp_en = 0,
        .cycle_en = 0,
        .sleep_time_ms = 0x0,
        .pcharge_low = TOUCH_PCHARGE_LOW_127,
        .pcharge_high = TOUCH_PCHARGE_HIGH_2047,
        .ldo_volt = TOUCH_LDO_VOLTAGE_2P0V,
        .vref_volt = TOUCH_VREF_VOLTAGE_1P4V,
        .hystersis_volt = TOUCH_HYSTERSIS_VOLTAGE_30MV,
        .compensation_cap = TOUCH_COMPENSATION_CAP_12PF,
        .filter_en = 1,
        .filter_ctrl = TOUCH_FILTER_ORDER_CTRL_0,
        .lta_en = 1,
        .lta_ctrl = TOUCH_LTA_ORDER_CTRL_0,
        .data_hystersis_en = 0,
        .data_hys_ctrl = 0x0
    };

    printf("Touch calibration\r\n");

    bflb_touch_init(touch, &cal_cfg);

    bflb_touch_enable(touch);

    /* wait calibration end */
    while (1 != bflb_touch_get_end_status(touch)) {
        bflb_mtimer_delay_ms(1);
    }

    /* dump lta data */
    touch_dump_raw_lta_flt();

    bflb_touch_disable(touch);
}

void touch_isr(int irq, void *arg)
{
    uint32_t int_status;
    int channel;

    int_status = bflb_touch_get_intstatus(touch);
    printf("Touch int status=%08x\r\n", int_status);

    if (int_status == 0) {
        return;
    }

    for (channel = 0; channel < TOUCH_CHANNEL_MAX; channel++) {
        if (int_status & (1 << channel)) {
            printf("Touch channel %d interrupt\r\n", channel);
            bflb_touch_int_clear(touch, (1 << channel));
        }
    }
}

int main(void)
{
    int ch;
    uint64_t target_time;
    struct bflb_device_s *uart0;
    struct bflb_touch_config_s cfg = {
        .clk_sel = TOUCH_CLK_SEL_32M,
        .clk_div = TOUCH_CLK_DIV_16,
        .cur_sel = TOUCH_CUR_SEL_AON_BG,
        .cal_en = 0,
        .work_mode = TOUCH_WORK_MODE_SELF_CAP,
        .run_mode = TOUCH_RUN_MODE_CONTINUE,
        .ulp_en = 0,
        .cycle_en = 0,
        .sleep_time_ms = 0x0,
        .pcharge_low = TOUCH_PCHARGE_LOW_127,
        .pcharge_high = TOUCH_PCHARGE_HIGH_4095,
        .ldo_volt = TOUCH_LDO_VOLTAGE_2P0V,
        .vref_volt = TOUCH_VREF_VOLTAGE_1P4V,
        .hystersis_volt = TOUCH_HYSTERSIS_VOLTAGE_30MV,
        .compensation_cap = TOUCH_COMPENSATION_CAP_12PF,
        .filter_en = 0,
        .filter_ctrl = TOUCH_FILTER_ORDER_CTRL_0,
        .lta_en = 0,
        .lta_ctrl = TOUCH_LTA_ORDER_CTRL_0,
        .data_hystersis_en = 0,
        .data_hys_ctrl = 0x0
    };

    board_init();

    board_touch_gpio_init();

    printf("Touch basic test\n");

    touch = bflb_device_get_by_name("touch");
    uart0 = bflb_device_get_by_name("uart0");

    /* init channel before calibration */
    touch_channel_init();

    /* calibration */
    touch_calibration();

    /* touch init */
    bflb_touch_init(touch, &cfg);

    /* register interrupt */
    bflb_irq_attach(touch->irq_num, touch_isr, NULL);
    bflb_irq_enable(touch->irq_num);

    /* enable touch */
    bflb_touch_enable(touch);

    /* init shell for debug */
    shell_init();

    target_time = bflb_mtimer_get_time_ms();
    while (1) {
        if ((ch = bflb_uart_getchar(uart0)) != -1) {
            shell_handler(ch);
        }
        if (bflb_mtimer_get_time_ms() >= target_time) {
            touch_dump_raw_lta_flt();
            target_time += 1000;
        }
    }
}
