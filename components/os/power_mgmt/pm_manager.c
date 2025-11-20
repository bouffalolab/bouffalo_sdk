#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "pm_manager.h"
#include "bl616_lp.h"

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#define PM_MEM_POOL_SIZE (1460 * 2)
static linear_allocator pm_mem;
void *pm_pbuf = NULL;
int enable_multicast_broadcast = 0;
TaskHandle_t rxl_process_task_hd = NULL;
extern bl_lp_fw_cfg_t lpfw_cfg;

struct pm_wrap pbufc[LINEAR_ALLOCATOR_MAX_PTRS];

static size_t align_up(size_t ptr, size_t align)
{
    if (align == 0)
        return ptr;
    return (ptr + (align - 1)) & ~(align - 1);
}

void linear_allocator_init(linear_allocator *alloc, void *buffer, size_t size)
{
    alloc->buffer = (uint8_t *)buffer;
    alloc->size = size;
    alloc->offset = 0;
    alloc->ptr_count = 0;
    alloc->iter_index = 0;
}

void *linear_allocator_alloc(linear_allocator *alloc, size_t size, size_t align)
{
    size_t base = (size_t)(alloc->buffer);
    size_t current = base + alloc->offset;
    size_t aligned = align_up(current, align);
    size_t rel_offset = aligned - base;
    size_t new_offset = rel_offset + size;

    if (new_offset > alloc->size || alloc->ptr_count >= LINEAR_ALLOCATOR_MAX_PTRS)
        return NULL;

    void *ptr = (void *)(alloc->buffer + rel_offset);
    alloc->offset = new_offset;
    alloc->ptrs[alloc->ptr_count++] = ptr;
    return ptr;
}

size_t linear_allocator_get_ptrs(linear_allocator *alloc, void **out_ptrs, size_t max_ptrs)
{
    size_t count = (alloc->ptr_count < max_ptrs) ? alloc->ptr_count : max_ptrs;
    for (size_t i = 0; i < count; ++i) {
        out_ptrs[i] = alloc->ptrs[i];
    }
    return count;
}

void linear_allocator_reset(linear_allocator *alloc)
{
    alloc->offset = 0;
    alloc->ptr_count = 0;
    alloc->iter_index = 0;
}

void *linear_allocator_next(linear_allocator *alloc, size_t *size_out)
{
    if (!alloc)
        return NULL;

    if (alloc->iter_index >= alloc->ptr_count) {
        if (size_out)
            *size_out = 0;

        return NULL;
    }

    size_t idx = alloc->iter_index;
    if (size_out) {
        *size_out = alloc->bsize[idx];
    }

    alloc->iter_index++;

    return alloc->ptrs[idx];
}

void linear_allocator_iter_reset(linear_allocator *alloc)
{
    alloc->iter_index = 0;
}

void set_dtim_config(int dtim)
{
    int listen_interval = 1;

    if (dtim == 0) {
        lpfw_cfg.bcmc_dtim_mode = 0;
        listen_interval = 1;
    } else {
        lpfw_cfg.bcmc_dtim_mode = 1;
        listen_interval = dtim;
    }
    lpfw_cfg.dtim_origin = listen_interval;
}

uint8_t lp_interval_get(void)
{
    return lpfw_cfg.dtim_num;
}

int pm_mem_pool_alloc(void)
{
    uint32_t addr;

    if (pm_pbuf) {
        return 0;
    }

    addr = (uint8_t *)pvPortMalloc(PM_MEM_POOL_SIZE);
    pm_pbuf = (uint8_t*)addr;

    linear_allocator_init(&pm_mem, (void *)bflb_get_no_cache_addr(addr), PM_MEM_POOL_SIZE);

    return 0;
}

int pm_mem_pool_free(void)
{
    if (pm_pbuf) {
        vPortFree(pm_pbuf);
        pm_pbuf = NULL;
    }

    linear_allocator_reset(&pm_mem);

    return 0;
}

uint32_t pm_manager_handle_get(void)
{
    uint32_t addr = (uint32_t)&pm_mem;

    if ((addr & 0xF0000000) == 0x60000000) {
        addr = (addr & 0x0FFFFFFF) | 0x20000000;
    }

    return addr;
}

void pm_pbuf_free_cb(struct pbuf *p)
{
    struct pbuf_custom *pc = (struct pbuf_custom *)p;
    struct pm_wrap *wrap = container_of(pc, struct pm_wrap, pc);

    if (wrap->ref) {
        wrap->ref--;
    }
}

int pm_pbufc_check(void)
{
    for (int i = 0; i < LINEAR_ALLOCATOR_MAX_PTRS; i++) {
        if (pbufc[i].ref) {
            return 1;
        }
    }

    return 0;
}

int pm_enter_lp_perparation(void)
{
    int ret = 0;
    if (enable_multicast_broadcast) {
        ret = pm_mem_pool_alloc();

        if (!ret) {
            lpfw_cfg.buf_addr = pm_manager_handle_get();
        } else {
            lpfw_cfg.buf_addr = NULL;
        }
    } else {
        lpfw_cfg.buf_addr = NULL;
    }

    bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);

    if (wifi_mgmr_sta_state_get()) {
        wifi_mgmr_sta_ps_enter();
    }

    return ret;
}

int pm_exit_lp_perparation(void)
{
    pm_mem_pool_free();
    lpfw_cfg.buf_addr = NULL;

    if (wifi_mgmr_sta_state_get()) {
        wifi_mgmr_sta_ps_exit();
    }

    return 0;
}

int pm_enable_tickless(void)
{
    pm_enter_lp_perparation();

    tickless_enter();

    return 0;
}

int pm_disable_tickless(void)
{
    pm_exit_lp_perparation();

    tickless_exit();

    return 0;
}

int pm_status_update(int status)
{
    //update status
    if (status) {
        pm_disable_tickless();
    } else {
        pm_enable_tickless();
    }

    return 0;
}

static void process_multicast_broadcast(void *pvParameters)
{
    void *pload;
    size_t bsize = 0;
    uint8_t cnt = 0;

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        linear_allocator_iter_reset(&pm_mem);
        pload = linear_allocator_next(&pm_mem, &bsize);
        while (pload && bsize) {
            if (pload != NULL) {
                memset(&(pbufc[cnt].pc), 0, sizeof(pbufc[cnt].pc));
                pbufc[cnt].pc.custom_free_function = pm_pbuf_free_cb;
                struct pbuf *p = pbuf_alloced_custom(PBUF_RAW, bsize, PBUF_REF, &(pbufc[cnt].pc), pload, bsize);

//print_pbuf_contents(p, 0);
#if CONFIG_LWIP_ONHOST_ENABLE
                extern int dual_stack_input(struct pbuf * p, bool is_sta);
                dual_stack_input(p, 1);
#else
                pbufc[cnt].ref++;
                if (netif_default && netif_default->input) {
                    err_t ret = netif_default->input(p, netif_default);
                    if (ret != ERR_OK) {
                        printf("[RECV_TASK] Failed to input pbuf to netif\r\n");
                        pbuf_free(p);
                    }
                } else {
                    pbuf_free(p);
                }
#endif
            }
            pload = linear_allocator_next(&pm_mem, &bsize);
        }

        linear_allocator_reset(&pm_mem);
    }

    vTaskDelete(NULL);
}

int pm_alloc_mem_reset(void)
{
    linear_allocator_reset(&pm_mem);

    return;
}

int pm_sys_init(void)
{
    memset(&pm_mem, 0, sizeof(linear_allocator));

    //wifi_mgmr_sta_ps_status_register(tickless_handke_get());

    lpfw_cfg.bcmc_dtim_mode = 0;
    lpfw_cfg.dtim_origin = 1;

    printf("[OS] Starting process_multicase_broadcast task...\r\n");
    xTaskCreate(process_multicast_broadcast, (char *)"hellow", 384, NULL, 10, &rxl_process_task_hd);

    return 0;
}
