#include "bl616_glb_gpio.h"

void bflb_gpio_pad_check(uint8_t pin)
{
    if (GLB_GPIO_Pad_LeadOut_Sts(pin) == RESET) {
        while (1) {}
    }
}