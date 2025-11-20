/**
 * @file nhsdiowifi.c
 * @brief SDIO WiFi port adapter layer implementation
 */

#include <stdio.h>
#include "nhsdiowifi.h"

/* Function declaration for getting forwarding rule configuration */
const nh_forward_config_t* sdiowifi_get_forward_config(void);

/* Example program debug output */
#ifdef CONFIG_NETHUB_DEBUG
#define EXAMPLE_DEBUG(fmt, ...) printf("[NHSDIOWIFI] " fmt, ##__VA_ARGS__)
#else
#define EXAMPLE_DEBUG(fmt, ...)
#endif

/**
 * @brief Initialize nethub framework
 * @return NH_OK for success, other values for failure
 */
int nhsdiowifi_init(void)
{
    EXAMPLE_DEBUG("Starting nethub framework initialization\n");

    /* Initialize nethub framework */
    int ret = nethub_init(sdiowifi_get_forward_config());
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("nethub initialization failed: %d\n", ret);
        return ret;
    }

    /* Start nethub framework */
    ret = nethub_start();
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("nethub start failed: %d\n", ret);
        return ret;
    }

    EXAMPLE_DEBUG("nethub framework initialization completed\n");
    return NH_OK;
}

/**
 * @brief Register all network interfaces
 * @return NH_OK for success, other values for failure
 */
int nhsdiowifi_register_all_interfaces(void)
{
    int ret;

    EXAMPLE_DEBUG("Starting network interface registration\n");

    /* Register SDIO interface */
    ret = nhif_register(&nhsdio_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register SDIO interface: %d\n", ret);
        return ret;
    }
    EXAMPLE_DEBUG("SDIO interface registered successfully\n");

    /* Register STA interface */
    ret = nhif_register(&nhsta_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register STA interface: %d\n", ret);
        return ret;
    }
    EXAMPLE_DEBUG("STA interface registered successfully\n");

    /* Register TCPIPSTA interface */
    ret = nhif_register(&nhtcpipsta_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register TCPIPSTA interface: %d\n", ret);
        return ret;
    }
    EXAMPLE_DEBUG("TCPIPSTA interface registered successfully\n");

    /* Register AP interface */
    ret = nhif_register(&nhap_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register AP interface: %d\n", ret);
        return ret;
    }
    EXAMPLE_DEBUG("AP interface registered successfully\n");

    /* Register TCPIPAP interface */
    ret = nhif_register(&nhtcpipap_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register TCPIPAP interface: %d\n", ret);
        return ret;
    }
    EXAMPLE_DEBUG("TCPIPAP interface registered successfully\n");

    EXAMPLE_DEBUG("All interfaces registration completed\n");
    return NH_OK;
}

/**
 * @brief Deinitialize nethub framework
 */
void nhsdiowifi_deinit(void)
{
    EXAMPLE_DEBUG("Starting nethub framework deinitialization\n");

    /* Stop and deinitialize nethub framework */
    nethub_stop();
    nethub_deinit();

    EXAMPLE_DEBUG("nethub framework deinitialization completed\n");
}
