#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

#define LINEAR_ALLOCATOR_MAX_PTRS 12

extern int enable_multicast_broadcast;

typedef struct {
    uint8_t *buffer;
    size_t size;
    size_t offset;

    void *ptrs[LINEAR_ALLOCATOR_MAX_PTRS];
    size_t bsize[LINEAR_ALLOCATOR_MAX_PTRS];
    size_t ptr_count;
    size_t iter_index;
} linear_allocator;

struct pm_wrap {
    struct pbuf_custom pc;
    uint8_t ref;
};

void linear_allocator_init(linear_allocator *alloc, void *buffer, size_t size);
void *linear_allocator_alloc(linear_allocator *alloc, size_t size, size_t align);
size_t linear_allocator_get_ptrs(linear_allocator *alloc, void **out_ptrs, size_t max_ptrs);
void *linear_allocator_next(linear_allocator *alloc, size_t *size_out);
void linear_allocator_reset(linear_allocator *alloc);
void linear_allocator_iter_reset(linear_allocator *alloc);

int pm_sys_init(void);
int pm_mem_pool_alloc(void);
int pm_mem_pool_free(void);
uint32_t pm_manager_handle_get(void);
void pm_pbuf_free_cb(struct pbuf *p);
int pm_pbufc_check(void);
int pm_enter_lp_perparation(void);
int pm_exit_lp_perparation(void);
int pm_alloc_mem_reset(void);
void set_dtim_config(int dtim);
int pm_enable_tickless(void);
int pm_disable_tickless(void);

#endif // LINEAR_ALLOCATOR_H
