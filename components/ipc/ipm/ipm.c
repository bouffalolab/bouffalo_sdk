#include <bl616d_glb.h>
#include <bflb_ipc.h>
#include "ipm.h"

static struct bflb_device_s *ipc_cpu0;
static struct bflb_device_s *ipc_cpu1;

struct ipm_data {
    ipm_callback_t callback;
    void *user_data;
    uint32_t this_core_id;
    uint32_t other_core_id;
};

int ipm_send(ipm_device_t *dev, int wait, uint32_t id,
                          const void *data, int size)
{
    struct ipm_data *driver_data = (struct ipm_data *)dev->data;

    if (driver_data->this_core_id == 0) {
        bflb_ipc_trig(ipc_cpu1, 1 << id);
    } else {
        bflb_ipc_trig(ipc_cpu0, 1 << id);
    }

    return 0;
}

void ipm_register_callback(ipm_device_t *dev,
                    ipm_callback_t cb,
                    void *user_data)
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
