
#include <bflb_ipc.h>
#include <bflb_irq.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ipm.h"
#if defined(BL616D)
#include "bl616d_glb.h"

#define IPC_MASTER_REG_ADDR      0x20040008
#define IPC_SLAVE_REG_ADDR       0x2004000c

#endif
struct bflb_device_s *ipc_cpu0;
struct bflb_device_s *ipc_cpu1;

struct ipm_data {
    ipm_callback_t callback;
    void *user_data;
    uint32_t this_core_id;
    uint32_t other_core_id;
};

static uint32_t g_ipc_sync_ack = 0;

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


    if (ipc_intsta & (1 << IPC_CHANNEL_MCS)) {
        extern int ipc_sync_isr(void);
        if (ipc_sync_isr()) {
            if (driver_data->this_core_id == 0) {
                bflb_ipc_clear(ipc_cpu0, (1 << IPC_CHANNEL_MCS));
            } else {
                bflb_ipc_clear(ipc_cpu1, (1 << IPC_CHANNEL_MCS));
            }
            return;
        }
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
    *((volatile uint32_t *)IPC_MASTER_REG_ADDR) = 0;
    *((volatile uint32_t *)IPC_SLAVE_REG_ADDR) = 0;
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

int ipc_sync_send(uint16_t cmd_code)
{
    uint8_t status = 0;
    static uint32_t dummy_data = 0xA5A5A5A5;
    g_ipc_sync_ack = 0;
    if (GLB_CORE_ID_AP == GLB_Get_Core_Type()) {
        *((volatile uint32_t *)IPC_MASTER_REG_ADDR) = cmd_code;
    } else {
        *((volatile uint32_t *)IPC_SLAVE_REG_ADDR) = cmd_code;
    }

    status = ipm_send(&ipm_handle, 0, IPC_CHANNEL_MCS, &dummy_data, sizeof(dummy_data));
    if (status != 0) {
        printf("ipm_send failed to notify: %d\r\n", status);
        return -1;
    }
    return 0;
}

int ipc_sync_send_wait_ack(uint16_t cmd_code, uint16_t expected_ack, uint32_t timeout_ms)
{
    uint32_t start_time;
    uint32_t elapsed = 0;

    if (ipc_sync_send(cmd_code) != 0) {
        return -1;
    }

    start_time = xTaskGetTickCount();

    while (g_ipc_sync_ack != expected_ack) {
        elapsed = xTaskGetTickCount() - start_time;

        if (elapsed > timeout_ms) {
            return -1;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    return 0;
}

int ipc_sync_isr(void)
{
    uint32_t cmd = 0;
    if (GLB_CORE_ID_NP == GLB_Get_Core_Type()) {
        cmd = *((volatile uint32_t *)IPC_MASTER_REG_ADDR);
    } else {
        cmd = *((volatile uint32_t *)IPC_SLAVE_REG_ADDR);
    }

    switch (cmd) {
        case IPC_SYNC_SUSPEND_CMD: {
            uint32_t flag = bflb_irq_save();

            if (GLB_CORE_ID_NP == GLB_Get_Core_Type()) {
                *((volatile uint32_t *)IPC_SLAVE_REG_ADDR) = IPC_SYNC_SUSPEND_ACK;
                bflb_ipc_trig(ipc_cpu0, 1 << IPC_CHANNEL_MCS);
            } else {
                *((volatile uint32_t *)IPC_MASTER_REG_ADDR) = IPC_SYNC_SUSPEND_ACK;
                bflb_ipc_trig(ipc_cpu1, 1 << IPC_CHANNEL_MCS);
            }

            wait_for_resume();

            if (GLB_CORE_ID_NP == GLB_Get_Core_Type()) {
                *((volatile uint32_t *)IPC_SLAVE_REG_ADDR) = IPC_SYNC_RESUME_ACK;
                bflb_ipc_trig(ipc_cpu0, 1 << IPC_CHANNEL_MCS);
            } else {
                *((volatile uint32_t *)IPC_MASTER_REG_ADDR) = IPC_SYNC_RESUME_ACK;
                bflb_ipc_trig(ipc_cpu1, 1 << IPC_CHANNEL_MCS);
            }

            bflb_irq_restore(flag);
        }
            return 1;

        case IPC_SYNC_RESUME_CMD:

            if (GLB_CORE_ID_NP == GLB_Get_Core_Type()) {
                *((volatile uint32_t *)IPC_SLAVE_REG_ADDR) = IPC_SYNC_RESUME_ACK;
                bflb_ipc_trig(ipc_cpu0, 1 << IPC_CHANNEL_MCS);
            } else {
                *((volatile uint32_t *)IPC_MASTER_REG_ADDR) = IPC_SYNC_RESUME_ACK;
                bflb_ipc_trig(ipc_cpu1, 1 << IPC_CHANNEL_MCS);
            }

            return 1;

        case IPC_SYNC_SUSPEND_ACK:
            g_ipc_sync_ack = IPC_SYNC_SUSPEND_ACK;
            return 1;

        case IPC_SYNC_RESUME_ACK:
            g_ipc_sync_ack = IPC_SYNC_RESUME_ACK;
            return 1;

        default:
            printf("[SYNC ISR] Core %d unknown cmd: 0x%02X\r\n", GLB_Get_Core_Type(), cmd);
            break;
    }

    return 0;
}

ATTR_TCM_SECTION
int wait_for_resume(void)
{
    uint32_t ret;
    static uint32_t loop_count = 0;

    while (1) {
        uint32_t temp_cmd;

        if (GLB_CORE_ID_NP == GLB_Get_Core_Type()) {
            temp_cmd = *((volatile uint32_t *)IPC_MASTER_REG_ADDR);
        } else {
            temp_cmd = *((volatile uint32_t *)IPC_SLAVE_REG_ADDR);
        }

        if (temp_cmd == IPC_SYNC_RESUME_CMD) {
            ret = temp_cmd;
            break;
        }

        loop_count++;
        for (volatile int i = 0; i < 100; i++) {
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
        }
    }
    return ret;
}