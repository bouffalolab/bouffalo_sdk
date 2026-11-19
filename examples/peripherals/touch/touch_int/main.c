#include <stdio.h>

#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "touch_common.h"

static struct bflb_device_s *touch;
static struct touch_irq_context_s touch_irq;

static int touch_init(void)
{
    struct bflb_touch_v2_config_s config;
    int ret;

    bflb_irq_disable(touch->irq_num);

    ret = touch_calibrate(touch, false);
    if (ret != 0) {
        return ret;
    }

    touch_config_init(&config, false, false);
    ret = bflb_touch_v2_init(touch, &config);
    if (ret != 0) {
        return ret;
    }

    touch_set_thresholds(touch, TOUCH_SELF_CAP_CHANNEL_MASK);
    bflb_touch_v2_clear_int(touch, TOUCH_INT_CLEAR_ALL);
    touch_irq.dev = touch;
    touch_irq.channel_mask = TOUCH_SELF_CAP_CHANNEL_MASK;
    touch_irq.pending_status = 0;
    ret = bflb_irq_attach(touch->irq_num, touch_irq_handler, &touch_irq);
    if (ret != 0) {
        bflb_touch_v2_set_int_mask(touch, TOUCH_INT_CLEAR_ALL);
        bflb_touch_v2_scan_stop(touch);
        return ret;
    }
    bflb_irq_clear_pending(touch->irq_num);
    bflb_irq_enable(touch->irq_num);
    return 0;
}

int main(void)
{
    int ret;

    board_init();
    printf("Touch V2 self-capacitance interrupt example\r\n");

    ret = touch_gpio_init();
    if (ret != 0) {
        printf("Touch GPIO initialization failed: %d\r\n", ret);
        return ret;
    }
    touch_print_gpio_map(false);

    touch = bflb_device_get_by_name(BFLB_NAME_TOUCH_V2);
    if (touch == NULL) {
        printf("Touch V2 device not found\r\n");
        return -1;
    }

    ret = touch_init();
    if (ret != 0) {
        printf("Touch initialization failed: %d\r\n", ret);
        return ret;
    }

    printf("Touch scanning started\r\n");
    bflb_touch_v2_scan_start(touch);

    while (1) {
        touch_process_events(&touch_irq);
        bflb_mtimer_delay_ms(5);
    }
}
