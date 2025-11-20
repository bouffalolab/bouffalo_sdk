/*
 * Copyright (c) 2020-2021, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rpmsg_backend.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <openamp/open_amp.h>
#include <stdio.h>

/* Configuration */
#ifndef CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS
#define CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS 3
#endif

#ifndef CONFIG_RPMSG_SERVICE_MODE_MASTER
#define CONFIG_RPMSG_SERVICE_MODE_MASTER 0
#endif

#ifndef CONFIG_RPMSG_SERVICE_LOG_LEVEL
#define CONFIG_RPMSG_SERVICE_LOG_LEVEL 0
#endif

#if CONFIG_RPMSG_SERVICE_LOG_LEVEL > 0
#define LOG_DBG(fmt, ...) printf("[RPMSG_DBG] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) printf("[RPMSG_ERR] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_INF(fmt, ...) printf("[RPMSG_INF] " fmt "\r\n", ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...) do {} while(0)
#define LOG_ERR(fmt, ...) printf("[RPMSG_ERR] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_INF(fmt, ...) do {} while(0)
#endif

#define MASTER (CONFIG_RPMSG_SERVICE_MODE_MASTER == 1)

static struct virtio_device vdev;
static struct rpmsg_virtio_device rvdev;
static struct metal_io_region *io;
static bool ep_crt_started;

#if MASTER
static struct rpmsg_virtio_shm_pool shpool;
#endif

static struct {
    const char *name;
    rpmsg_ept_cb cb;
    struct rpmsg_endpoint ep;
    volatile bool bound;
} endpoints[CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS];

static void rpmsg_service_unbind(struct rpmsg_endpoint *ep)
{
    rpmsg_destroy_ept(ep);
}

#if MASTER

static void ns_bind_cb(struct rpmsg_device *rdev,
                    const char *name,
                    uint32_t dest)
{
    int err;

    LOG_DBG("Received NS announcement for endpoint '%s' with dest %d", name, dest);

    for (int i = 0; i < CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS; ++i) {
        if (strcmp(name, endpoints[i].name) == 0) {
            LOG_DBG("Found matching endpoint at index %d", i);
            err = rpmsg_create_ept(&endpoints[i].ep,
                           rdev,
                           name,
                           RPMSG_ADDR_ANY,
                           dest,
                           endpoints[i].cb,
                           rpmsg_service_unbind);

            if (err != 0) {
                LOG_ERR("Creating remote endpoint %s"
                    " failed wirh error %d", name, err);
            } else {
                endpoints[i].bound = true;
                LOG_DBG("Successfully created and bound endpoint '%s'", name);
            }

            return;
        }
    }

    LOG_ERR("Remote endpoint %s not registered locally", name);
}

#endif

static int rpmsg_service_init(void)
{
    int32_t err;

    LOG_DBG("RPMsg service initialization start");

    err = rpmsg_backend_init(&io, &vdev);
    if (err) {
        LOG_ERR("RPMsg backend init failed with error %d", err);
        return err;
    }

#if MASTER
    rpmsg_virtio_init_shm_pool(&shpool, (void *)SHM_START_ADDR, SHM_SIZE);
    err = rpmsg_init_vdev(&rvdev, &vdev, ns_bind_cb, io, &shpool);
#else
    err = rpmsg_init_vdev(&rvdev, &vdev, NULL, io, NULL);
#endif

    if (err) {
        LOG_ERR("rpmsg_init_vdev failed %d", err);
        return err;
    }

    ep_crt_started = true;

#if !MASTER
    struct rpmsg_device *rdev;

    rdev = rpmsg_virtio_get_rpmsg_device(&rvdev);

    for (int i = 0; i < CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS; ++i) {
        if (endpoints[i].name) {
            LOG_DBG("Creating remote endpoint '%s' at index %d", endpoints[i].name, i);
            err = rpmsg_create_ept(&endpoints[i].ep,
                        rdev,
                        endpoints[i].name,
                        RPMSG_ADDR_ANY,
                        RPMSG_ADDR_ANY,
                        endpoints[i].cb,
                        rpmsg_service_unbind);

            if (err) {
                LOG_ERR("rpmsg_create_ept failed %d", err);
                return err;
            } 
#if 0
            else {
                endpoints[i].bound = true;
                LOG_DBG("Successfully created remote endpoint '%s'", endpoints[i].name);
            }
#endif
            /* Add small delay between endpoint creation to avoid NS message collision */
            if (i < CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS - 1) {
                vTaskDelay(pdMS_TO_TICKS(20));
            }
        }
    }
#endif

    LOG_DBG("RPMsg service initialized");

    return 0;
}

int rpmsg_service_register_endpoint(const char *name, rpmsg_ept_cb cb)
{
    if (ep_crt_started) {
        LOG_ERR("Cannot register endpoint after service started");
        return -EINPROGRESS;
    }

    if (!name || !cb) {
        LOG_ERR("Invalid parameters for endpoint registration");
        return -EINVAL;
    }

    for (int i = 0; i < CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS; ++i) {
        if (!endpoints[i].name) {
            endpoints[i].name = name;
            endpoints[i].cb = cb;
            LOG_DBG("Registered endpoint '%s' at index %d (slot %d)", name, i, i);

            return i;
        }
    }

    LOG_ERR("No free slots to register endpoint %s", name);

    return -ENOMEM;
}

bool rpmsg_service_endpoint_is_bound(int endpoint_id)
{
    if (endpoint_id < 0 || endpoint_id >= CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS) {
        return false;
    }

    return endpoints[endpoint_id].bound;
}

int rpmsg_service_send(int endpoint_id, const void *data, size_t len)
{
    if (endpoint_id < 0 || endpoint_id >= CONFIG_RPMSG_SERVICE_NUM_ENDPOINTS) {
        LOG_ERR("Invalid endpoint ID: %d", endpoint_id);
        return -EINVAL;
    }

    if (!data || len == 0) {
        LOG_ERR("Invalid data parameters");
        return -EINVAL;
    }

    return rpmsg_send(&endpoints[endpoint_id].ep, data, len);
}

/* Bouffalo SDK: Additional helper function */
struct rpmsg_device *rpmsg_service_get_device(void)
{
    return rpmsg_virtio_get_rpmsg_device(&rvdev);
}

/* Bouffalo SDK: Replace SYS_INIT with explicit function */
int rpmsg_service_start(void)
{
    return rpmsg_service_init();
}

