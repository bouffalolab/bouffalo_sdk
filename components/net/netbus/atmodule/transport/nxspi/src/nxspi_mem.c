
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <nxspi.h>
#include <nxspi_log.h>

#include "bl616_glb.h"
#include "bflb_clock.h"
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <bflb_dma.h>
#include <bflb_spi.h>

#include <hardware/spi_reg.h>
#include <utils_list.h>
#include <shell.h>

#define DBG_TAG "NXSPIMEM"
#include <bflb_core.h>

#define NXSPI_GETMAX_LEN(a,b)     (((((a)>(b))?(a):(b)) + 3) & (~3))// 4 align

int nxspi_hwmem_init(void)
{

}

void *malloc_aligned_with_padding(int size, int align)
{
    size_t aligned_size = (size + (align - 1)) & ~(align - 1);
    uintptr_t addr = (uintptr_t)pvPortMalloc(aligned_size + align);
    if(addr == NULL) {
        return NULL;
    }
    uintptr_t aligned_addr = (addr + align) & ~(align - 1);

    // save pad size
    *(char *)(aligned_addr - 1) = aligned_addr - addr;

    return (void *)aligned_addr;
}

void free_aligned_with_padding(void *ptr)
{
    if(ptr == NULL) {
        return;
    }
    uintptr_t aligned_addr = (uintptr_t)ptr;
    uintptr_t addr = aligned_addr - *(char *)(aligned_addr - 1);

    vPortFree((void *)addr);
}

void *malloc_aligned_with_padding_nocache(int size, int align_bytes)
{
    return ((uint32_t)(malloc_aligned_with_padding(size, align_bytes))) & 0xBFFFFFFF;
}

void free_aligned_with_padding_nocache(void *ptr)
{
    free_aligned_with_padding(((uint32_t)(ptr)) | 0x60000000);
}

