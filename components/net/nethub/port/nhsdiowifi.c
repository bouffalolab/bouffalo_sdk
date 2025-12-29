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
#define USER_LOG(fmt, ...) printf("[NHSDIOWIFI] " fmt, ##__VA_ARGS__)
#else
#define USER_LOG(fmt, ...)
#endif

/**
 * @brief Register all network interfaces
 * @return NH_OK for success, other values for failure
 */
static int nhsdiowifi_register_all_interfaces(void)
{
    int ret;

    USER_LOG("Starting network interface registration\n");

    /* Register SDIO interface */
    ret = nhif_register(&nhsdio_ops, NULL);
    if (ret != NH_OK) {
        USER_LOG("Failed to register SDIO interface: %d\n", ret);
        return ret;
    }
    USER_LOG("SDIO interface registered successfully\n");

    /* Register STA interface */
    ret = nhif_register(&nhsta_ops, NULL);
    if (ret != NH_OK) {
        USER_LOG("Failed to register STA interface: %d\n", ret);
        return ret;
    }
    USER_LOG("STA interface registered successfully\n");

    /* Register TCPIPSTA interface */
    ret = nhif_register(&nhtcpipsta_ops, NULL);
    if (ret != NH_OK) {
        USER_LOG("Failed to register TCPIPSTA interface: %d\n", ret);
        return ret;
    }
    USER_LOG("TCPIPSTA interface registered successfully\n");

    /* Register AP interface */
    ret = nhif_register(&nhap_ops, NULL);
    if (ret != NH_OK) {
        USER_LOG("Failed to register AP interface: %d\n", ret);
        return ret;
    }
    USER_LOG("AP interface registered successfully\n");

    /* Register TCPIPAP interface */
    ret = nhif_register(&nhtcpipap_ops, NULL);
    if (ret != NH_OK) {
        USER_LOG("Failed to register TCPIPAP interface: %d\n", ret);
        return ret;
    }
    USER_LOG("TCPIPAP interface registered successfully\n");

    USER_LOG("All interfaces registration completed\n");
    return NH_OK;
}

/**
 * @brief Initialize nethcomponents/net/nethub/port/nhsdiowifi.cub framework
 * @return NH_OK for success, other values for failure
 */
int nethub_sdiowifi_init(void)
{
    USER_LOG("Starting nethub framework initialization\n");

    /* Initialize nethub framework */
    int ret = nethub_init(sdiowifi_get_forward_config());
    if (ret != NH_OK) {
        USER_LOG("nethub initialization failed: %d\n", ret);
        return ret;
    }

    /* Start nethub framework */
    ret = nethub_start();
    if (ret != NH_OK) {
        USER_LOG("nethub start failed: %d\n", ret);
        return ret;
    }

    USER_LOG("nethub framework initialization completed\n");

    /* Register all network interfaces */
    nhsdiowifi_register_all_interfaces();

    return NH_OK;
}

/**
 * @brief Deinitialize nethub framework
 */
void nethub_sdiowifi_deinit(void)
{
    USER_LOG("Starting nethub framework deinitialization\n");

    /* Stop and deinitialize nethub framework */
    nethub_stop();
    nethub_deinit();

    USER_LOG("nethub framework deinitialization completed\n");
}
