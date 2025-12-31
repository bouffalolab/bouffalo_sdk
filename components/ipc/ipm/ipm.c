
#include <bflb_ipc.h>
#ifdef CONFIG_WIFI_OTA_DUAL_CORE
#include "bl616d.h"
#endif
#include "ipm.h"
#include "bflb_l1c.h"

/* Ringbuffer size definitions for initialization */
#define RB_AP2NP_SIZE (8 * 1024) /* 8KB for AP->NP ringbuffer */
#define RB_NP2AP_SIZE (8 * 1024) /* 8KB for NP->AP ringbuffer */

/* Core detection - weak function that can be overridden by chip-specific code */
__attribute__((weak)) int simple_rpc_is_ap_core(void)
{
    /* Default implementation: AP core returns 1, NP core returns 0 */
    return 1; /* Default to AP core */
}

#ifndef GLB_CORE_ID_AP
#define GLB_CORE_ID_AP 0
#endif

#ifndef GLB_Get_Core_Type
#define GLB_Get_Core_Type() (simple_rpc_is_ap_core() ? GLB_CORE_ID_AP : 1)
#endif

static struct bflb_device_s *ipc_cpu0;
static struct bflb_device_s *ipc_cpu1;

struct ipm_data {
    ipm_callback_t callback;
    void *user_data;
    uint32_t this_core_id;
    uint32_t other_core_id;
};

static volatile bool g_ready = false;

/****************************************************************************
 * Ringbuffer Operations (kfifo style)
 ****************************************************************************/

static int ringbuffer_write(volatile ringbuffer_t *rb, const uint8_t *data, uint32_t len)
{
    if (rb->tail - rb->head >= RB_MAX_ENTRIES) {
        return -1; 
    }

    uint32_t idx = rb->tail & RB_INDEX_MASK;
    ringbuffer_entry_t *entry = (ringbuffer_entry_t *)&rb->entries[idx];

    if (len > RB_ENTRY_SIZE - sizeof(uint32_t)) {
        len = RB_ENTRY_SIZE - sizeof(uint32_t);
    }

    entry->len = len;
    if (len > 0 && data != NULL) {
        memcpy(entry->data, data, len);
    }

    rb->tail++;

    return 0;
}

static int ringbuffer_read(volatile ringbuffer_t *rb, uint8_t *data, uint32_t *len)
{
    if (rb->tail - rb->head == 0) {
        return -1;
    }

    uint32_t idx = rb->head & RB_INDEX_MASK;
    ringbuffer_entry_t *entry = (ringbuffer_entry_t *)&rb->entries[idx];

    *len = entry->len;
    if (*len > 0 && data != NULL) {
        memcpy(data, entry->data, *len);
    }

    rb->head++;

    return 0;
}

/****************************************************************************
 * Simple RPC Public API (only for BL616D dual-core WiFi OTA)
 ****************************************************************************/

#ifdef CONFIG_WIFI_OTA_DUAL_CORE
int simple_rpc_init(void)
{
    /* Get IPC devices */
    if (NULL == ipc_cpu0) {
        ipc_cpu0 = bflb_device_get_by_name("ipc0_ch0");
        if (NULL == ipc_cpu0) {
            printf("simple_rpc: ipc0_ch0 get failed\n");
            return -1;
        }
        bflb_ipc_init(ipc_cpu0);
        bflb_ipc_int_unmask(ipc_cpu0, IPC_BITS_ALL);
    }
    if (NULL == ipc_cpu1) {
        ipc_cpu1 = bflb_device_get_by_name("ipc0_ch1");
        if (NULL == ipc_cpu1) {
            printf("simple_rpc: ipc0_ch1 get failed\n");
            return -1;
        }
        bflb_ipc_init(ipc_cpu1);
        bflb_ipc_int_unmask(ipc_cpu1, IPC_BITS_ALL);
    }

    if (GLB_CORE_ID_AP == GLB_Get_Core_Type()) {
        memset((void *)XRAM_AP2NP_RINGBUF, 0, RB_AP2NP_SIZE);
        memset((void *)XRAM_NP2AP_RINGBUF, 0, RB_NP2AP_SIZE);

        bflb_l1c_dcache_clean_range((void *)XRAM_AP2NP_RINGBUF, RB_AP2NP_SIZE);
        bflb_l1c_dcache_clean_range((void *)XRAM_NP2AP_RINGBUF, RB_NP2AP_SIZE);
    }

    g_ready = true;

    return 0;
}

int simple_rpc_send(const uint8_t *data, uint32_t len)
{
    int ret;
    uint32_t ipm_id;
    uint32_t core_type = GLB_Get_Core_Type();

    if (!g_ready) {
        return -EAGAIN;
    }

    if (GLB_CORE_ID_AP == core_type) {
        ret = ringbuffer_write(XRAM_AP2NP_RINGBUF, data, len);
        ipm_id = IPM_ID_FLASH_RESP;
    } else {
        ret = ringbuffer_write(XRAM_NP2AP_RINGBUF, data, len);
        ipm_id = IPM_ID_FLASH_CMD;
    }

    if (ret != 0) {
        return -EBUSY;
    }

    if (GLB_CORE_ID_AP == core_type) {
        bflb_ipc_trig(ipc_cpu0, 1 << ipm_id);
    } else {
        bflb_ipc_trig(ipc_cpu1, 1 << ipm_id);
    }

    return 0;
}

int simple_rpc_recv(uint8_t *data, uint32_t *len)
{
    int ret;

    if (!g_ready) {
        return -EAGAIN;
    }

    if (GLB_CORE_ID_AP == GLB_Get_Core_Type()) {
        ret = ringbuffer_read(XRAM_NP2AP_RINGBUF, data, len);
    } else {
        ret = ringbuffer_read(XRAM_AP2NP_RINGBUF, data, len);
    }

    return ret;
}
#endif /* CONFIG_WIFI_OTA_DUAL_CORE */

/****************************************************************************
 * IPM Driver API
 ****************************************************************************/

int ipm_send(ipm_device_t *dev, int wait, uint32_t id, const void *data, int size)
{
    struct ipm_data *driver_data = (struct ipm_data *)dev->data;

    if (driver_data->this_core_id == 0) {
        bflb_ipc_trig(ipc_cpu1, 1 << id);
    } else {
        bflb_ipc_trig(ipc_cpu0, 1 << id);
    }

    return 0;
}

void ipm_register_callback(ipm_device_t *dev, ipm_callback_t cb, void *user_data)
{
    struct ipm_data *data = (struct ipm_data *)dev->data;

    data->callback = cb;
    data->user_data = user_data;
}

int ipm_max_data_size_get(ipm_device_t *dev)
{
    return 0; /* no data transport, only id/signaling */
}

uint32_t ipm_max_id_val_get(ipm_device_t *dev)
{
    return 15; /* 16 possible doorbells (0-15) */
}

int ipm_set_enabled(ipm_device_t *dev, int enable)
{
    return 0;
}

void ipm_isr(int irq, void *dev)
{
    struct ipm_data *driver_data = ((ipm_device_t *)dev)->data;
    uint32_t ipc_intsta = 0;

    if (driver_data->this_core_id == 0) {
        ipc_intsta = bflb_ipc_get_intsta(ipc_cpu0);
    } else {
        ipc_intsta = bflb_ipc_get_intsta(ipc_cpu1);
    }

    if (driver_data->callback) {
        driver_data->callback(dev, driver_data->user_data, 0, NULL);
    }

    if (driver_data->this_core_id == 0) {
        bflb_ipc_clear(ipc_cpu0, ipc_intsta);
    } else {
        bflb_ipc_clear(ipc_cpu1, ipc_intsta);
    }
}

int ipm_init(ipm_device_t *dev)
{
    struct ipm_data *data = malloc(sizeof(struct ipm_data));
    dev->data = data;
    ipc_cpu0 = bflb_device_get_by_name("ipc0_ch0");
    if (NULL == ipc_cpu0) {
        printf("ipc0_ch0 get failed\n");
        return -1;
    }
    bflb_ipc_init(ipc_cpu0);
    bflb_ipc_int_unmask(ipc_cpu0, IPC_BITS_ALL);

    ipc_cpu1 = bflb_device_get_by_name("ipc0_ch1");
    if (NULL == ipc_cpu1) {
        printf("ipc0_ch1 get failed\n");
        return -1;
    }
    bflb_ipc_init(ipc_cpu1);
    bflb_ipc_int_unmask(ipc_cpu1, IPC_BITS_ALL);

    if (GLB_CORE_ID_AP == GLB_Get_Core_Type()) {
        data->this_core_id = 0;
        data->other_core_id = 1;
        bflb_irq_attach(ipc_cpu0->irq_num, ipm_isr, dev);
        bflb_irq_enable(ipc_cpu0->irq_num);
    } else {
        data->this_core_id = 1;
        data->other_core_id = 0;
        bflb_irq_attach(ipc_cpu1->irq_num, ipm_isr, dev);
        bflb_irq_enable(ipc_cpu1->irq_num);
    }

    return 0;
}
