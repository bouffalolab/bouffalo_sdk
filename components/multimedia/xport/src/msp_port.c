/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

// #include <bl616_platform.h>
#include <stdio.h>
#include <msp/kernel.h>
#include <xutils/debug.h>
#include "msp_port.h"

#ifdef CONFIG_CHIP_BL606P_E907
#include <bl606p_glb_gpio.h>
#include <bl606p_gpio.h>
#else
#include <bl616_glb_gpio.h>
#include <bl616_gpio.h>
#endif

int msp_gpio_set_func(uint8_t pin, uint8_t pull_type, uint8_t gpio_mode, uint8_t pin_func)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = gpio_mode;
    gpio_cfg.pullType = pull_type;
    gpio_cfg.gpioFun = pin_func;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioPin = pin;
 
    GLB_GPIO_Init(&gpio_cfg);

    return 0;
}

int msp_gpio_output_config(uint8_t pin, uint8_t pull_type)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioFun = GPIO_FUN_GPIO;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioPin = pin;
    if (pull_type) {
        gpio_cfg.pullType = GPIO_PULL_UP;
    } else {
        gpio_cfg.pullType = GPIO_PULL_DOWN;
    }

    GLB_GPIO_Init(&gpio_cfg);

    return 0;
}

int msp_gpio_output_set(uint8_t pin, uint8_t value)
{
    GLB_GPIO_Write((GLB_GPIO_Type)pin, value ? 1 : 0);

    return 0;
}

#if 0
extern void bl_irq_register_with_ctx(int irqnum, void *handler, void *ctx);
extern void bl_irq_unregister(int irqnum, void *handler);
extern void bl_irq_enable(unsigned int source);
extern void bl_irq_disable(unsigned int source);

// irq register
void msp_irq_register(int irqnum, void *handler, void *ctx)
{
   bl_irq_register_with_ctx(irqnum, handler, ctx);
}

void msp_irq_unregister(int irqnum)
{
   bl_irq_register_with_ctx(irqnum, NULL, NULL);
}

void msp_irq_enable(int irqnum)
{
   bl_irq_enable((unsigned int)irqnum);
}

void msp_irq_disenable(int irqnum)
{
   bl_irq_disable((unsigned int)irqnum);
}

// memory
void *msp_malloc_align(int size, int align_bytes)
{
    void *base_ptr = NULL;
    void *mem_ptr = NULL;

    base_ptr = msp_malloc(size + align_bytes);//alloc alignbytes, rather than align_bytes-1, because we need to store offset

    mem_ptr = (void *)((int)((int *)base_ptr + align_bytes -1) & ~(align_bytes-1));
    if(mem_ptr == base_ptr) {//base_ptr already align_bytes align
        mem_ptr = base_ptr + align_bytes;//force move it to one more  alignbytes
    }
    *((int *)mem_ptr-1) = mem_ptr - base_ptr;
    // MSP_LOGD("offset is %d base prt %x mem_ptr %x\n",*((int *)mem_ptr -1),base_ptr,mem_ptr);
    return mem_ptr;
}

void msp_free_align(void *ptr)
{
    void *base_addr = NULL;
    // MSP_LOGD("%x %x\n",ptr,*((int *)ptr - 1));
    base_addr = (void *)(ptr- *((int *)ptr-1));
    // MSP_LOGD("ptr %x base_addr %x\n",ptr,base_addr);
    msp_free(base_addr);
}

//xTaskGetTickCount
uint64_t msp_gettick_count(void)
{
    uint64_t count;

    count = (uint64_t)((xPortIsInsideInterrupt())?(xTaskGetTickCountFromISR()):(xTaskGetTickCount()));

    return count;
}

/*
 * msp_test_mem(10, 100, 32);
 * return 0-success
 */
int msp_test_mem(uint32_t test_times, uint32_t test_mem_length, uint32_t test_mem_align)
{
    char *p1 = NULL;
    char *p2 = NULL;
    int i = 0;

    for (i = 0; i < test_times; i++) {
        // test malloc p1
        p1 = msp_malloc_align(test_mem_length, test_mem_align);
        if (NULL == p1) {
            MSP_LOGE("mem error\r\n");
            return -1;
        }

        // ("msp_malloc_align = %p\r\n", p1);

        if ((uint32_t)p1%test_mem_align) {
            MSP_LOGE("mem error\r\n");
            return -2;
        }

        // test malloc p2
        p2 = msp_malloc_align(test_mem_length, test_mem_align);
        if (NULL == p2) {
            MSP_LOGE("mem error\r\n");
            return -1;
        }

        // ("msp_malloc_align = %p\r\n", p2);

        if ((uint32_t)p2%test_mem_align) {
            MSP_LOGE("mem error\r\n");
            return -2;
        }

        // test free p1/p2
        msp_free_align(p1);
        msp_free_align(p2);
    }

    return 0;
}

void msp_dma_chan_start(int chan)
{
    hosal_dma_chan_start(chan);
}

void msp_dma_chan_stop(int chan)
{
    hosal_dma_chan_stop(chan);
}

void msp_dma_irq_callback_set(int chan, msp_dma_irq_t pfn, void *p_arg)
{
    hosal_dma_irq_callback_set(chan, pfn, p_arg);
}

int msp_dma_chan_request(int arg)
{
    int chan = 0;

    chan = hosal_dma_chan_request(0);

    return chan;
}

int msp_dma_chan_release(int chan)
{
    int ret;

    ret = hosal_dma_chan_release(chan);

    return ret;
}
#endif
