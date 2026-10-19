#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <spisync_port.h>

#if TPDBG_ENABLE
#define ONE_LINE_MAX_NUM (32)

static char s_debug_buf[ONE_LINE_MAX_NUM*3 + 1];
static SemaphoreHandle_t s_debug_mutex = NULL;

static int buf_out(const char *str, const void *inbuf, int len)
{
    char *buf = (char *)inbuf;
    char *pbuffer = NULL;

    pbuffer = (char *)s_debug_buf;
    int m = 0, n = 0;
    int j = 0, k = 0, tmp = 0;

    tmp = (sizeof(s_debug_buf)) / 3; // Maximum array length

    if ((ONE_LINE_MAX_NUM > tmp) || (len < 1)) {
        return -1;
    }

    if (NULL == s_debug_mutex) {
        s_debug_mutex = xSemaphoreCreateMutex();
    }

    m = len / ONE_LINE_MAX_NUM;
    n = len % ONE_LINE_MAX_NUM;

    if (n > 0) {
        m++;
    }

    if (n > 0) {
        // Non-multiple
        for (k = 0; k < m; k++) {
            if ((k + 1) == m) {
                // Last frame data
                tmp = 0;
                for (j = 0; j < n; j++) {
                    tmp += snprintf(pbuffer + tmp, sizeof(s_debug_buf) - tmp, "%02x ", (uint8_t)buf[k * ONE_LINE_MAX_NUM + j]);
                }
                printf("%s %.*s\r\n", str, tmp, pbuffer);
            } else {
                tmp = 0;
                for (j = 0; j < ONE_LINE_MAX_NUM; j++) {
                    tmp += snprintf(pbuffer + tmp, sizeof(s_debug_buf) - tmp, "%02x ", (uint8_t)buf[k * ONE_LINE_MAX_NUM + j]);
                }
                printf("%s %.*s\r\n", str, tmp, pbuffer);
            }
        }
    } else {
        // Multiple
        for (k = 0; k < m; k++) {
            tmp = 0;
            for (j = 0; j < ONE_LINE_MAX_NUM; j++) {
                tmp += snprintf(pbuffer + tmp, sizeof(s_debug_buf) - tmp, "%02x ", (uint8_t)buf[k * ONE_LINE_MAX_NUM + j]);
            }
            printf("%s %.*s\r\n", str, tmp, pbuffer);
        }
    }

    if (s_debug_mutex) {
        xSemaphoreGive(s_debug_mutex);
    }

    return 0;
}

void spisync_buf(const char *str, void *buf, uint32_t len)
{
    if ((NULL == buf) || (len == 0)) {
        printf("arg error buf = %p, len = %ld\r\n", buf, len);
        return;
    }

    buf_out(str, buf, len);
}
#endif

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

