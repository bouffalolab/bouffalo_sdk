#include <bl602_romdriver.h>
#include <bl602_glb.h>

#include <stdio.h>
#include <stdbool.h>
#include "bl_sys.h"

#define MFG_CONFIG_REG    (0x4000F100)
#define MFG_CONFIG_VAL    ("0mfg")

#define REASON_WDT        (0x77646F67) // watchdog reboot wdog
#define REASON_SOFTWARE   (0x736F6674) // software        soft
#define REASON_POWEROFF   (0x0) // software        soft

void BL602_Delay_MS(uint32_t cnt);
int bl_sys_reset_por(void)
{
    volatile uint32_t *p = (volatile uint32_t *)0x40000010;

    __disable_irq();
#if 0 //FIXME: Due to compilation issues, we are temporarily commenting here.
    DMA_Disable();
    DMA_Channel_Disable(0);
    DMA_Channel_Disable(1);
    DMA_Channel_Disable(2);
    DMA_Channel_Disable(3);
#endif
    BL602_Delay_MS(4);

    *p = (1 << 4) | (1 << 8);
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    *p = 0;
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    GLB_SW_POR_Reset();
    while (1) {
        /*empty dead loop*/
    }

    return 0;
}
