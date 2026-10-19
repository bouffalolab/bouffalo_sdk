#include "nh_internal.h"

#define DBG_TAG "NETHUB_EP"
#include "log.h"

#include <string.h>

static nhif_instance_t g_if_instances[NETHUB_CHANNEL_MAX];

static const char *const g_if_type_names[NETHUB_CHANNEL_MAX] = {
    [NETHUB_CHANNEL_WIFI_STA] = "WIFI_STA",
    [NETHUB_CHANNEL_WIFI_AP] = "WIFI_AP",
    [NETHUB_CHANNEL_STACK_STA] = "STACK_STA",
    [NETHUB_CHANNEL_STACK_AP] = "STACK_AP",
    [NETHUB_CHANNEL_STACK_NAT] = "STACK_NAT",
    [NETHUB_CHANNEL_BRIDGE] = "BRIDGE",
    [NETHUB_CHANNEL_SDIO] = "SDIO",
    [NETHUB_CHANNEL_USB] = "USB",
    [NETHUB_CHANNEL_SPI] = "SPI",
};

static void nhif_reset_instance(nhif_instance_t *instance)
{
    if (instance == NULL) {
        return;
    }

    memset(instance, 0, sizeof(*instance));
    instance->state = NETHUB_ENDPOINT_STATE_UNINITIALIZED;
}

const char *nethub_channel_to_string(nethub_channel_t type)
{
    if (type >= NETHUB_CHANNEL_MAX) {
        return "INVALID";
    }

    if (g_if_type_names[type] == NULL) {
        return "UNKNOWN";
    }

    return g_if_type_names[type];
}

int nhif_register(const struct nhif_ops *ops, void *private_data)
{
    nhif_instance_t *instance;

    if (ops == NULL || ops->type >= NETHUB_CHANNEL_MAX) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    instance = &g_if_instances[ops->type];
    if (instance->ops != NULL) {
        return NETHUB_ERR_ALREADY_EXISTS;
    }

    instance->type = ops->type;
    instance->ops = ops;
    instance->private_data = private_data;
    instance->state = NETHUB_ENDPOINT_STATE_INITIALIZED;

    LOG_D("register endpoint %s\r\n", nethub_channel_to_string(ops->type));
    return NETHUB_OK;
}

int nhif_unregister(nethub_channel_t type)
{
    nhif_instance_t *instance;

    if (type >= NETHUB_CHANNEL_MAX) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    instance = &g_if_instances[type];
    if (instance->ops == NULL) {
        return NETHUB_ERR_NOT_FOUND;
    }

    if (instance->state == NETHUB_ENDPOINT_STATE_ACTIVE && instance->ops->deinit != NULL) {
        int ret = instance->ops->deinit();
        if (ret != NETHUB_OK) {
            instance->state = NETHUB_ENDPOINT_STATE_ERROR;
            return ret;
        }
    }

    nhif_reset_instance(instance);
    return NETHUB_OK;
}

int nhif_unregister_all(void)
{
    int ret;
    int i;

    for (i = NETHUB_CHANNEL_MAX - 1; i >= 0; i--) {
        if (g_if_instances[i].ops == NULL) {
            continue;
        }

        ret = nhif_unregister((nethub_channel_t)i);
        if (ret != NETHUB_OK) {
            return ret;
        }
    }

    return NETHUB_OK;
}

nhif_instance_t *nhif_find_by_type(nethub_channel_t type)
{
    if (type >= NETHUB_CHANNEL_MAX) {
        return NULL;
    }

    if (g_if_instances[type].ops == NULL) {
        return NULL;
    }

    return &g_if_instances[type];
}

int nhif_init_all(void)
{
    bool found = false;
    int i;

    for (i = 0; i < NETHUB_CHANNEL_MAX; i++) {
        nhif_instance_t *instance = &g_if_instances[i];

        if (instance->ops == NULL) {
            continue;
        }

        found = true;
        if (instance->state == NETHUB_ENDPOINT_STATE_ACTIVE) {
            continue;
        }

        if (instance->ops->init != NULL) {
            int ret = instance->ops->init();
            if (ret != NETHUB_OK) {
                instance->state = NETHUB_ENDPOINT_STATE_ERROR;
                nhif_deinit_all();
                return ret;
            }
        }

        instance->state = NETHUB_ENDPOINT_STATE_ACTIVE;
    }

    if (!found) {
        return NETHUB_ERR_NOT_FOUND;
    }

    return NETHUB_OK;
}

int nhif_deinit_all(void)
{
    int i;

    for (i = NETHUB_CHANNEL_MAX - 1; i >= 0; i--) {
        nhif_instance_t *instance = &g_if_instances[i];

        if (instance->ops == NULL || instance->state != NETHUB_ENDPOINT_STATE_ACTIVE) {
            continue;
        }

        if (instance->ops->deinit != NULL) {
            int ret = instance->ops->deinit();
            if (ret != NETHUB_OK) {
                instance->state = NETHUB_ENDPOINT_STATE_ERROR;
                return ret;
            }
        }

        instance->state = NETHUB_ENDPOINT_STATE_INITIALIZED;
    }

    return NETHUB_OK;
}
