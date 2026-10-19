#include "platform_api_extension.h"
#include "platform_api_vmcore.h"

#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "mm.h"

int bh_platform_init(void)
{
    return 0;
}

void bh_platform_destroy(void)
{
}

void *os_malloc(unsigned size)
{
    return malloc(size);
}

void *os_realloc(void *ptr, unsigned size)
{
    return realloc(ptr, size);
}

void os_free(void *ptr)
{
    free(ptr);
}

int os_printf(const char *format, ...)
{
    va_list args;
    int result;

    va_start(args, format);
    result = vprintf(format, args);
    va_end(args);
    return result;
}

int os_vprintf(const char *format, va_list args)
{
    return vprintf(format, args);
}

uint64 os_time_get_boot_us(void)
{
    return bflb_mtimer_get_time_us();
}

uint64 os_time_thread_cputime_us(void)
{
    return os_time_get_boot_us();
}

korp_tid os_self_thread(void)
{
    return xTaskGetCurrentTaskHandle();
}

uint8 *os_thread_get_stack_boundary(void)
{
    return NULL;
}

void os_thread_jit_write_protect_np(bool enabled)
{
    (void)enabled;
}

int os_mutex_init(korp_mutex *mutex)
{
    *mutex = xSemaphoreCreateMutex();
    return *mutex ? 0 : -1;
}

int os_mutex_destroy(korp_mutex *mutex)
{
    vSemaphoreDelete(*mutex);
    return 0;
}

int os_mutex_lock(korp_mutex *mutex)
{
    return xSemaphoreTake(*mutex, portMAX_DELAY) == pdTRUE ? 0 : -1;
}

int os_mutex_unlock(korp_mutex *mutex)
{
    return xSemaphoreGive(*mutex) == pdTRUE ? 0 : -1;
}

void *os_mmap(void *hint, size_t size, int prot, int flags, os_file_handle file)
{
    (void)hint;
    (void)flags;
    (void)file;
    if (size > UINT32_MAX) {
        return NULL;
    }
    return kmalloc(size, (prot & MMAP_PROT_EXEC ? MM_FLAG_HEAP_OCRAM_0
                                             : MM_FLAG_HEAP_ANY)
                             | MM_FLAG_ALIGN_32BYTE | MM_FLAG_PROP_ZERO);
}

void os_munmap(void *addr, size_t size)
{
    (void)size;
    kfree(addr);
}

void *os_mremap(void *old_addr, size_t old_size, size_t new_size)
{
    return os_mremap_slow(old_addr, old_size, new_size);
}

int os_mprotect(void *addr, size_t size, int prot)
{
    (void)addr;
    (void)size;
    (void)prot;
    return 0;
}

void os_dcache_flush(void)
{
    bflb_l1c_dcache_clean_all();
}

void os_icache_flush(void *start, size_t len)
{
    bflb_l1c_icache_invalid_range(start, len);
    __asm__ volatile("fence.i" ::: "memory");
}
