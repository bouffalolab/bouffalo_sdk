/**
 * @file nhif_manager.c
 * @brief nethub interface manager implementation
 */

#include "nh_log.h"
#include "nhif_ops.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Interface instance array - Allocate space only for enabled interfaces
 */
static nhif_instance_t g_if_instances[NHIF_TYPE_MAX] = {0};

/**
 * @brief Interface type name strings
 */
static const char* const g_if_type_names[NHIF_TYPE_MAX + 1] = {
    [NHIF_TYPE_STA]      = "STA",
    [NHIF_TYPE_AP]       = "AP",
    [NHIF_TYPE_TCPIPSTA] = "TCPIPSTA",
    [NHIF_TYPE_TCPIPAP]  = "TCPIPAP",
    [NHIF_TYPE_TCPIPNAT] = "TCPIPNAT",
    [NHIF_TYPE_BRIDGE]   = "BRIDGE",
    [NHIF_TYPE_SDIO]     = "SDIO",
    [NHIF_TYPE_USB]      = "USB",
    [NHIF_TYPE_SPI]      = "SPI",
    [NHIF_TYPE_MAX]      = "UNKNOWN"
};

/**
 * @brief Get interface type name string
 * @param type Interface type
 * @return Interface name string
 */
const char* nhif_type_to_string(nhif_type_t type)
{
    if (type >= NHIF_TYPE_MAX) {
        return "INVALID";
    }

    const char* name = g_if_type_names[type];
    return name ? name : "UNKNOWN";
}

/* Logging macros are included in nh_log.h */

/**
 * @brief Register network interface
 */
int nhif_register(const struct nhif_ops *ops, void *private_data)
{
    if (ops == NULL || ops->type >= NHIF_TYPE_MAX) {
        nh_err("Error: Invalid parameter ops=%p, type=%d\n", ops, ops ? (int)ops->type : -1);
        return NH_ERR_INVALID_PARAM;
    }

    nhif_type_t type = ops->type;

    if (g_if_instances[type].ops != NULL) {
        nh_warn("Error: Interface %d already registered\n", type);
        return NH_ERR_ALREADY_EXISTS;
    }

    nh_debug("Registering interface %d (%s)\n", type, nhif_type_to_string(type));

    /* Initialize interface instance */
    g_if_instances[type].type = type;
    g_if_instances[type].state = NHIF_STATE_INITIALIZED;
    g_if_instances[type].ops = ops;
    g_if_instances[type].private_data = private_data;

    /* Call interface initialization function */
    if (ops->init != NULL) {
        int ret = ops->init();
        if (ret != NH_OK) {
            nh_err("Error: Interface %d initialization failed %d\n", type, ret);
            g_if_instances[type].ops = NULL;
            return ret;
        }
    }

    nh_debug("Interface %d registered successfully\n", type);
    return NH_OK;
}

/**
 * @brief Unregister network interface
 */
int nhif_unregister(nhif_type_t type)
{
    if (type >= NHIF_TYPE_MAX) {
        return NH_ERR_INVALID_PARAM;
    }

    if (g_if_instances[type].ops == NULL) {
        nh_warn("Warning: Interface %d not registered\n", type);
        return NH_ERR_NOT_FOUND;
    }

    nh_debug("Unregistering interface %d (%s)\n", type, nhif_type_to_string(type));

    /* Call interface deinitialization function */
    if (g_if_instances[type].ops != NULL &&
        g_if_instances[type].ops->deinit != NULL) {
        g_if_instances[type].ops->deinit();
    }

    /* Clean up interface instance */
    memset(&g_if_instances[type], 0, sizeof(nhif_instance_t));

    nh_debug("Interface %d unregistered successfully\n", type);
    return NH_OK;
}

/**
 * @brief Find interface instance of specified type
 */
nhif_instance_t* nhif_find_by_type(nhif_type_t type)
{
    if (type >= NHIF_TYPE_MAX) {
        return NULL;
    }

    if (g_if_instances[type].ops == NULL) {
        return NULL;
    }

    return &g_if_instances[type];
}

/**
 * @brief Initialize all registered interfaces
 */
int nhif_init_all(void)
{
    nh_debug("Initializing all registered interfaces\n");

    for (int i = 0; i < NHIF_TYPE_MAX; i++) {
        if (g_if_instances[i].ops != NULL) {
            nh_debug("Found registered interface %d (%s)\n", i, nhif_type_to_string(i));
        }
    }

    nh_debug("All interfaces initialized\n");
    return NH_OK;
}
