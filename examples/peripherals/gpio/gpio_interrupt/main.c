#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *gpio;

void gpio_isr(int irq, void *arg)
{
    static int i = 0;
    bool intstatus = bflb_gpio_get_intstatus(gpio, GPIO_PIN_0);
    if (intstatus) {
        bflb_gpio_int_clear(gpio, GPIO_PIN_0);
        printf("%d\r\n", i++);
    }
}

int main(void)
{
    board_init();

    gpio = bflb_device_get_by_name("gpio");
    printf("gpio interrupt\r\n");

    bflb_gpio_int_init(gpio, GPIO_PIN_0, GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL);
    bflb_gpio_int_mask(gpio, GPIO_PIN_0, false);

    bflb_irq_attach(gpio->irq_num, gpio_isr, gpio);
    bflb_irq_enable(gpio->irq_num);

    while (1) {
        printf("helloworld\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
