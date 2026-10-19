
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bflb_core.h"

#include "mm.h"
#include "lwip/pbuf.h"
#include "hb_port.h"

#ifdef CONFIG_SOLN_HB_TX_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_HB_TX_LOG_LEVEL
#endif
#define DBG_TAG "HB_PORT"
#include "log.h"

/*************************** mem port  ****************************/
/* mem debug */
#define HB_MEM_LIMIT_DEBUG 0

#if (HB_MEM_LIMIT_DEBUG)
static uint64_t malloc_count = 0;
static uint64_t free_count = 0;
#endif

void *hb_malloc(size_t size)
{
#if (HB_MEM_LIMIT_DEBUG)
    malloc_count++;
    if (malloc_count - free_count > HB_MEM_LIMIT_DEBUG) {
        LOG_W("High number of outstanding allocations: malloc_count=%llu, free_count=%llu, size=%u\r\n", malloc_count, free_count, size);
    }
#endif

    void *ptr = kmalloc(size, (MM_FLAG_HEAP_OCRAM_0 | MM_FLAG_PROP_ZERO));
    if (ptr == NULL) {
        LOG_E("Failed to allocate memory of size %u\r\n", size);
    }
    return ptr;
}

void hb_free(void *ptr)
{
#if (HB_MEM_LIMIT_DEBUG)
    free_count++;
#endif

    if (ptr) {
        kfree(ptr);
    } else {
        LOG_W("Attempted to free NULL pointer\r\n");
    }
}

/*************************** time port  ****************************/
uint32_t hb_get_ms(void)
{
    return bflb_mtimer_get_time_ms();
}

/*************************** lwIP port ****************************/
struct pbuf *hb_pbuf_alloc_tx(uint16_t len)
{
    return pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
}
