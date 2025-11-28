#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "shell.h"
#include <openamp/open_amp.h>
#include <ipm.h>

static ipm_device_t ipm_handle;

static metal_phys_addr_t shm_physmap[] = { SHM_START_ADDR };

static volatile unsigned int received_data;

static struct virtio_vring_info rvrings[2] = {
    [0] = {
        .info.align = VRING_ALIGNMENT,
    },
    [1] = {
        .info.align = VRING_ALIGNMENT,
    },
};
static struct virtio_device vdev;
static struct rpmsg_virtio_device rvdev;
static struct metal_io_region shm_io_data;
static struct metal_io_region *io = &shm_io_data;
static struct virtqueue *vqueue[2];

static unsigned char ipc_virtio_get_status(struct virtio_device *dev)
{
    return VIRTIO_CONFIG_STATUS_DRIVER_OK;
}

static void ipc_virtio_set_status(struct virtio_device *dev, unsigned char status)
{
    ((*(volatile uint8_t *)(uintptr_t)(VDEV_STATUS_ADDR)) = (status));
}

static uint32_t ipc_virtio_get_features(struct virtio_device *dev)
{
    return 1 << VIRTIO_RPMSG_F_NS;
}

static void ipc_virtio_set_features(struct virtio_device *dev, uint32_t features)
{
}

static void ipc_virtio_notify(struct virtqueue *vq)
{
    uint32_t dummy_data = 0x55005500; /* Some data must be provided */
    ipm_send(&ipm_handle, 0, 0, &dummy_data, sizeof(dummy_data));
}

static struct virtio_dispatch dispatch = {
    .get_status = ipc_virtio_get_status,
    .set_status = ipc_virtio_set_status,
    .get_features = ipc_virtio_get_features,
    .set_features = ipc_virtio_set_features,
    .notify = ipc_virtio_notify,
};

static SemaphoreHandle_t data_sem;
static SemaphoreHandle_t data_rx_sem;
static SemaphoreHandle_t ept_sem;


static int sem_take(SemaphoreHandle_t sem, uint32_t ms)
{
    int status;
    if(xPortIsInsideInterrupt()) {
        BaseType_t pxHiProTskWkup = pdTRUE;
        status = xSemaphoreTakeFromISR(sem, &pxHiProTskWkup);
        portYIELD_FROM_ISR(pxHiProTskWkup);
    } else {
        status = xSemaphoreTake(sem, ms);
    }
    return (status == pdPASS ? 0 : -1);
}

static int sem_give(SemaphoreHandle_t sem)
{
    if(xPortIsInsideInterrupt()) {
        BaseType_t  temp = pdTRUE;
        xSemaphoreGiveFromISR(sem, &temp);
        portYIELD_FROM_ISR(temp);
     } else {
        xSemaphoreGive(sem);
     }
}

static void platform_ipm_callback(const struct device *dev, void *context,
                  uint32_t id, volatile void *data)
{
    sem_give(data_sem);
}

int endpoint_cb(struct rpmsg_endpoint *ept, void *data,
        size_t len, uint32_t src, void *priv)
{
    received_data = *((unsigned int *) data);
    sem_give(data_rx_sem);

    return RPMSG_SUCCESS;
}


struct rpmsg_endpoint my_ept;
struct rpmsg_endpoint *ep = &my_ept;

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
    (void)ept;
    rpmsg_destroy_ept(ep);
}

void ns_bind_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest)
{
    (void)rpmsg_create_ept(ep, rdev, name,
            RPMSG_ADDR_ANY, dest,
            endpoint_cb,
            rpmsg_service_unbind);
    sem_give(ept_sem);
}

static unsigned int receive_message(void)
{
    while (sem_take(data_rx_sem, 0) != 0) {
        int status = sem_take(data_sem, portMAX_DELAY); 
        if (status == 0) {
            virtqueue_notification(vqueue[0]);
        }
    }
    return received_data;
}

static int send_message(unsigned int message)
{
    return rpmsg_send(ep, &message, sizeof(message));
}

static struct rpmsg_virtio_shm_pool shpool;

static void master_app_task(void *param)
{
    int status = 0;
    unsigned int message = 0U;
    struct metal_init_params metal_params = METAL_INIT_DEFAULTS;

    data_sem = xSemaphoreCreateCounting(1, 0);
    data_rx_sem = xSemaphoreCreateCounting(1, 0);
    ept_sem = xSemaphoreCreateCounting(1, 0);
    printf("\r\nOpenAMP[master] demo started\r\n");

    status = metal_init(&metal_params);
    if (status != 0) {
        printf("metal_init: failed - error code %d\n", status);
        vTaskDelete(NULL);
    }

    /* declare shared memory region */
    metal_io_init(io, (void *)SHM_START_ADDR, shm_physmap, SHM_SIZE, -1, 0, NULL);

    ipm_init(&ipm_handle);
#if 0
    /* setup IPM */
    if (!device_is_ready(ipm_handle)) {
        printf("IPM device is not ready\n");
        return;
    }
#endif
    ipm_register_callback(&ipm_handle, platform_ipm_callback, NULL);

    status = ipm_set_enabled(&ipm_handle, 1);
    if (status != 0) {
        printf("ipm_set_enabled failed\n");
        vTaskDelete(NULL);
    }

    /* setup vdev */
    vqueue[0] = virtqueue_allocate(VRING_SIZE);
    if (vqueue[0] == NULL) {
        printf("virtqueue_allocate failed to alloc vqueue[0]\n");
        vTaskDelete(NULL);
    }
    vqueue[1] = virtqueue_allocate(VRING_SIZE);
    if (vqueue[1] == NULL) {
        printf("virtqueue_allocate failed to alloc vqueue[1]\n");
        vTaskDelete(NULL);
    }

    vdev.role = RPMSG_HOST;
    vdev.vrings_num = VRING_COUNT;
    vdev.func = &dispatch;
    rvrings[0].io = io;
    rvrings[0].info.vaddr = (void *)VRING_TX_ADDRESS;
    rvrings[0].info.num_descs = VRING_SIZE;
    rvrings[0].info.align = VRING_ALIGNMENT;
    rvrings[0].vq = vqueue[0];

    rvrings[1].io = io;
    rvrings[1].info.vaddr = (void *)VRING_RX_ADDRESS;
    rvrings[1].info.num_descs = VRING_SIZE;
    rvrings[1].info.align = VRING_ALIGNMENT;
    rvrings[1].vq = vqueue[1];

    vdev.vrings_info = &rvrings[0];

    /* setup rvdev */
    rpmsg_virtio_init_shm_pool(&shpool, (void *)SHM_START_ADDR, SHM_SIZE);
    status = rpmsg_init_vdev(&rvdev, &vdev, ns_bind_cb, io, &shpool);
    if (status != 0) {
        printf("rpmsg_init_vdev failed %d\n", status);
        vTaskDelete(NULL);
    }

    /* Since we are using name service, we need to wait for a response
     * from NS setup and than we need to process it
     */
    printf("wait for remote\r\n");
    sem_take(data_sem, portMAX_DELAY);
    virtqueue_notification(vqueue[0]);

    /* Wait til nameservice ep is setup */
    sem_take(ept_sem, portMAX_DELAY);

    while (message < 100) {
        printf("Master core send a message: %d\r\n", message);
        status = send_message(message);
        if (status < 0) {
            printf("send_message(%d) failed with status %d\n",
                   message, status);
            goto _cleanup;
        }

        message = receive_message();
        printf("Master core received a message: %d\r\n", message);

        message++;
    }

_cleanup:
    vTaskDelay(10);
    rpmsg_deinit_vdev(&rvdev);
    metal_finish();

    vTaskDelay(10);
    printf("OpenAMP demo ended.\n");
    vTaskDelete(NULL);
}

int ipc_master(void)
{
    printf("Starting application thread!\n");

    ipc_virtio_set_status(NULL, 0);
    TaskHandle_t xHandle;
    xTaskCreate(master_app_task, "ipc_master", 1024, NULL, 10, &xHandle);
    return 0;
}

int shell_master_ipc(int argc, char **argv)
{
    ipc_master();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_master_ipc, master_ipc, ipc master.);
