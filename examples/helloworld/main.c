#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

struct bflb_device_s *gpio;

int main(void)
{
    board_init();

    gpio = bflb_device_get_by_name("gpio");
    printf("gpio output\r\n");
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

    while (1) {
        bflb_gpio_set(gpio, GPIO_PIN_27);
        bflb_gpio_reset(gpio, GPIO_PIN_28);
        bflb_mtimer_delay_ms(500);

        bflb_gpio_reset(gpio, GPIO_PIN_27);

        bflb_gpio_set(gpio, GPIO_PIN_28);
        bflb_mtimer_delay_ms(500);

        LOG_F("hello world fatal\r\n");
        LOG_E("hello world error\r\n");
        LOG_W("hello world warning\r\n");
        LOG_I("hello world information\r\n");
        LOG_D("hello world debug\r\n");
        LOG_T("hello world trace\r\n");
        LOG_RF("hello world fatal raw\r\n");
        LOG_RE("hello world error raw\r\n");
        LOG_RW("hello world warning raw\r\n");
        LOG_RI("hello world information raw\r\n");
        LOG_RD("hello world debug raw\r\n");
        LOG_RT("hello world trace raw\r\n");
        bflb_mtimer_delay_ms(1000);
    }
}
