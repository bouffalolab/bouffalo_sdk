#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "flash_ops_rpmsg.h"
#include <rpmsg_service.h>
#include <openamp/rpmsg_virtio.h>
#include "log.h"

/****************************************************************************
 * Global Variables (defined here, used in rpmsg_ops_slave.c)
 ****************************************************************************/
SemaphoreHandle_t g_rpmsg_mutex;
SemaphoreHandle_t g_response_sem;
bool g_rpmsg_initialized = false;
int g_flash_endpoint_id = -1;
int g_user_ep1_id = -1;
int g_user_ep2_id = -1;

extern int rpmsg_endpoint_flash_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv);

__attribute__((weak)) int rpmsg_user1_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    (void)ept;
    (void)data;
    (void)len;
    (void)src;
    (void)priv;
    /* TODO: Implement customer-specific EP1 handling */
    return RPMSG_SUCCESS;
}

__attribute__((weak)) int rpmsg_user2_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    (void)ept;
    (void)data;
    (void)len;
    (void)src;
    (void)priv;
    /* TODO: Implement customer-specific EP2 handling */
    return RPMSG_SUCCESS;
}

int rpmsg_slave_init(void)
{
    int ret;

    if (g_rpmsg_initialized) {
        return 0;
    }

    LOG_I("Initializing RPMsg Receiver on Slave\r\n");

    ret = rpmsg_service_register_endpoint("rpmsg_flash", rpmsg_endpoint_flash_cb);
    if (ret < 0) {
        LOG_E("rpmsg_service_register_endpoint failed, ret=%d\r\n", ret);
        return ret;
    }
    g_flash_endpoint_id = ret;

    /* Register user endpoints for customer use */
    ret = rpmsg_service_register_endpoint("user_ep1", rpmsg_user1_cb);
    if (ret < 0) {
        LOG_W("Failed to register user_ep1, ret=%d\r\n", ret);
    } else {
        g_user_ep1_id = ret;
        LOG_I("Registered user_ep1 with ID: %d\r\n", g_user_ep1_id);
    }

    ret = rpmsg_service_register_endpoint("user_ep2", rpmsg_user2_cb);
    if (ret < 0) {
        LOG_W("Failed to register user_ep2, ret=%d\r\n", ret);
    } else {
        g_user_ep2_id = ret;
        LOG_I("Registered user_ep2 with ID: %d\r\n", g_user_ep2_id);
    }

    ret = rpmsg_service_start();
    if (ret < 0) {
        return ret;
    }

    g_rpmsg_mutex = xSemaphoreCreateMutex();
    if (g_rpmsg_mutex == NULL) {
        return -1;
    }

    g_response_sem = xSemaphoreCreateBinary();
    if (g_response_sem == NULL) {
        vSemaphoreDelete(g_rpmsg_mutex);
        return -1;
    }

    g_rpmsg_initialized = true;
    LOG_I("RPMsg Receiver initialized on SLAVE\r\n");
    return 0;
}

int rpmsg_slave_deinit(void)
{
    if (!g_rpmsg_initialized) {
        return 0;
    }

    LOG_I("Deinitializing RPMsg SLAVE\r\n");

    if (g_rpmsg_mutex) {
        vSemaphoreDelete(g_rpmsg_mutex);
        g_rpmsg_mutex = NULL;
    }

    if (g_response_sem) {
        vSemaphoreDelete(g_response_sem);
        g_response_sem = NULL;
    }

    g_rpmsg_initialized = false;
    return 0;
}

/****************************************************************************
 * Reserved Endpoint Public API
 ****************************************************************************/

int rpmsg_send_user_ep1(const void *data, size_t len)
{
    if (!g_rpmsg_initialized || g_user_ep1_id < 0) {
        LOG_E("Reserved EP1 not initialized\r\n");
        return -EINVAL;
    }

    return rpmsg_service_send(g_user_ep1_id, data, len);
}

int rpmsg_send_user_ep2(const void *data, size_t len)
{
    if (!g_rpmsg_initialized || g_user_ep2_id < 0) {
        LOG_E("Reserved EP2 not initialized\r\n");
        return -EINVAL;
    }

    return rpmsg_service_send(g_user_ep2_id, data, len);
}
