/*
 * Copyright (c) 2021, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rpmsg_backend.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <openamp/open_amp.h>
#include <ipm.h>
#include <stdio.h>

/* Logging */
#define LOG_DBG(fmt, ...) printf("[RPMSG_DBG] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) printf("[RPMSG_ERR] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_INF(fmt, ...) printf("[RPMSG_INF] " fmt "\r\n", ##__VA_ARGS__)

#ifndef CONFIG_RPMSG_SERVICE_MODE_MASTER
#define CONFIG_RPMSG_SERVICE_MODE_MASTER        0
#endif

#define MASTER (CONFIG_RPMSG_SERVICE_MODE_MASTER == 1)

#if MASTER
#define VIRTQUEUE_ID            0
#define RPMSG_ROLE RPMSG_HOST
#else
#define VIRTQUEUE_ID            1
#define RPMSG_ROLE RPMSG_REMOTE
#endif

#define IPM_TASK_STACK_SIZE    512
#define IPM_TASK_PRIORITY      (configMAX_PRIORITIES - 1)

static TaskHandle_t ipm_task_handle = NULL;
static SemaphoreHandle_t data_sem = NULL;
static StackType_t ipm_task_stack[IPM_TASK_STACK_SIZE];
static StaticTask_t ipm_task_buffer;

/* End of configuration defines */

/* IPM device */
static ipm_device_t ipm_handle;
static metal_phys_addr_t shm_physmap[] = { SHM_START_ADDR };
static struct metal_io_region shm_io;

static struct virtio_vring_info rvrings[2] = {
    [0] = {
        .info.align = VRING_ALIGNMENT,
    },
    [1] = {
        .info.align = VRING_ALIGNMENT,
    },
};
static struct virtqueue *vqueue[2];

static unsigned char ipc_virtio_get_status(struct virtio_device *vdev)
{
#if MASTER
    return VIRTIO_CONFIG_STATUS_DRIVER_OK;
#else
    return (*((volatile uint8_t *)(uintptr_t)(VDEV_STATUS_ADDR)));
#endif
}

static void ipc_virtio_set_status(struct virtio_device *vdev, unsigned char status)
{
    ((*(volatile uint8_t *)(uintptr_t)(VDEV_STATUS_ADDR)) = (status));
}

static uint32_t ipc_virtio_get_features(struct virtio_device *vdev)
{
    return 1 << VIRTIO_RPMSG_F_NS;
}

static void ipc_virtio_set_features(struct virtio_device *vdev, uint32_t features)
{
}

static void ipc_virtio_notify(struct virtqueue *vq)
{
    int status;

    uint32_t dummy_data = 0x55005500;

    status = ipm_send(&ipm_handle, 0, 0, &dummy_data, sizeof(dummy_data));

    if (status != 0) {
        LOG_ERR("ipm_send failed to notify: %d", status);
    }
}

const struct virtio_dispatch dispatch = {
    .get_status = ipc_virtio_get_status,
    .set_status = ipc_virtio_set_status,
    .get_features = ipc_virtio_get_features,
    .set_features = ipc_virtio_set_features,
    .notify = ipc_virtio_notify,
};

static int sem_take(SemaphoreHandle_t sem, uint32_t ms)
{
    int status;
    if (xPortIsInsideInterrupt()) {
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
    if (xPortIsInsideInterrupt()) {
        BaseType_t  temp = pdTRUE;
        xSemaphoreGiveFromISR(sem, &temp);
        portYIELD_FROM_ISR(temp);
     } else {
        xSemaphoreGive(sem);
     }
    return 0;
}

static void ipm_callback(const struct device *dev,
                        void *context, uint32_t id,
                        volatile void *data)
{
    (void)dev;
    (void)context;
    (void)id;
    (void)data;

    //LOG_DBG("Got callback of id %u", id);
    sem_give(data_sem);
}

static void ipm_task(void *param)
{
    while (1) {
        int status = sem_take(data_sem, portMAX_DELAY);
        if (status == 0) {
            virtqueue_notification(vqueue[VIRTQUEUE_ID]);
        }
    }
}

int rpmsg_backend_init(struct metal_io_region **io, struct virtio_device *vdev)
{
    int32_t                  err;
    struct metal_init_params metal_params = METAL_INIT_DEFAULTS;

    data_sem = xSemaphoreCreateCounting(1, 0);
    if (!data_sem) {
        LOG_ERR("Failed to create data semaphore");
        return -ENOMEM;
    }
#if MASTER
    ipc_virtio_set_status(NULL, 0);
#endif
    /* Create IPM task */
    ipm_task_handle = xTaskCreateStatic(
        ipm_task,                    /* Task function */
        "ipm_task",                  /* Task name */
        IPM_TASK_STACK_SIZE,         /* Stack size */
        NULL,                        /* Parameter */
        IPM_TASK_PRIORITY,           /* Priority */
        ipm_task_stack,              /* Static stack */
        &ipm_task_buffer             /* Task control block */
    );

    if (!ipm_task_handle) {
        LOG_ERR("Failed to create IPM task");
        return -ENOMEM;
    }

    /* Libmetal setup */
    err = metal_init(&metal_params);
    if (err) {
        LOG_ERR("metal_init: failed - error code %d", err);
        return err;
    }

    /* declare shared memory region */
    metal_io_init(&shm_io, (void *)SHM_START_ADDR, shm_physmap, SHM_SIZE, -1, 0, NULL);
    *io = &shm_io;

    /* IPM setup */
    err = ipm_init(&ipm_handle);
    if (err != 0) {
        LOG_ERR("IPM init failed: %d", err);
        return err;
    }

    /* Register IPM callback */
    ipm_register_callback(&ipm_handle, ipm_callback, NULL);

    /* Enable IPM */
    err = ipm_set_enabled(&ipm_handle, 1);
    if (err != 0) {
        LOG_ERR("Failed to enable IPM: %d", err);
        return err;
    }

    /* Virtqueue setup */
    vqueue[0] = virtqueue_allocate(VRING_SIZE);
    if (!vqueue[0]) {
        LOG_ERR("virtqueue_allocate failed to alloc vqueue[0]");
        return -ENOMEM;
    }

    vqueue[1] = virtqueue_allocate(VRING_SIZE);
    if (!vqueue[1]) {
        LOG_ERR("virtqueue_allocate failed to alloc vqueue[1]");
        return -ENOMEM;
    }

    rvrings[0].io = *io;
    rvrings[0].info.vaddr = (void *)VRING_TX_ADDRESS;
    rvrings[0].info.num_descs = VRING_SIZE;
    rvrings[0].info.align = VRING_ALIGNMENT;
    rvrings[0].vq = vqueue[0];

    rvrings[1].io = *io;
    rvrings[1].info.vaddr = (void *)VRING_RX_ADDRESS;
    rvrings[1].info.num_descs = VRING_SIZE;
    rvrings[1].info.align = VRING_ALIGNMENT;
    rvrings[1].vq = vqueue[1];

    vdev->role = RPMSG_ROLE;
    vdev->vrings_num = VRING_COUNT;
    vdev->func = &dispatch;
    vdev->vrings_info = &rvrings[0];

    return 0;
}

