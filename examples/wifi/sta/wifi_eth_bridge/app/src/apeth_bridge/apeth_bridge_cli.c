#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"
#include "platform_al.h"
#include "board.h"
#include "apeth_bridge_emac.h"
#include "app_apeth_bridge.h"

#define DBG_TAG "BRIDGE_EMAC"
#include "log.h"

static struct netif ethernet_netif;

/* Referenced by the example linker script for uncached DMA storage. */
int nocache_memory_max = 1;

struct netif *app_emac_netif(void)
{
    return &ethernet_netif;
}

bool app_emac_physical_link_up(void)
{
    return netif_is_link_up(&ethernet_netif);
}

static void lwip_status_update_task(void *arg)
{
    (void)arg;
    for (;;) {
        eth_link_state_update(&ethernet_netif);
        app_wds_bridge_poll();
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

int app_emac_init(void)
{
    emac_config_t cfg;

    board_emac_rmii_gpio_init(0);
    board_emac_mdio_gpio_init(0);
    memset(&cfg, 0, sizeof(cfg));
    platform_get_mac(cfg.mac_addr);
    eth_emac_hw_config(&cfg);
    if (netifapi_netif_add(&ethernet_netif, NULL, NULL, NULL, NULL,
                          eth_emac_if_init, tcpip_input) != ERR_OK) {
        LOG_E("EMAC netif initialization failed\r\n");
        return -1;
    }
    if (xTaskCreate(lwip_status_update_task, "bridge_poll", 1024, NULL, 15,
                    NULL) != pdPASS) {
        LOG_E("Bridge polling task creation failed\r\n");
        return -1;
    }
    return 0;
}
