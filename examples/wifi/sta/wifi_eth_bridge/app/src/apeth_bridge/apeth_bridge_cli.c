#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <netif/bridgeif.h>
#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/netif.h>
#include <lwip/netifapi.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "platform_al.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_sec_trng.h"

#include "bl616_glb.h"
#include "bl616_mfg_media.h"
#include "rfparam_adapter.h"

#include "board.h"
#include "shell.h"

#include "eth_phy.h"
#include "ephy_general.h"

#include "bflb_emac.h"

#include "apeth_bridge_emac.h"

#include <shell.h>

#define DBG_TAG "MAIN"
#include "log.h"

struct bflb_device_s *gpio;

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)
#define DHCP_STACK_SIZE  (256)
#define TASK_PRIORITY_DH (16)
#define MAC_ADDRESS_LEN  (6)

/****************************************************************************
 * Private Types
 ****************************************************************************/
struct bridge_api_arg_t {
    struct netif *bridge_netif;
    struct netif *port_netif;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/
static TaskHandle_t bridge_netif_dhcp_task;


static struct netif bridge_netif;
static struct netif ethernet_netif;
static int bridge_port_up = 0;

static struct bridge_api_arg_t bridge_api_arg = {
    .bridge_netif = &bridge_netif,
    .port_netif   = 0,
};

// XXX: used for ld file for nocache ram
int nocache_memory_max = 1;
/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/
extern void shell_init_with_task(struct bflb_device_s *shell);

void port_link_status_updated(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
        if (!bridge_port_up) {
            netif_set_link_up(&bridge_netif);
            LOG_I("Bridge netif link up\r\n");
        }
        bridge_port_up++;
    } else {
        bridge_port_up--;
        if (!bridge_port_up) {
            netif_set_link_down(&bridge_netif);
            LOG_I("Bridge netif link down\r\n");
        }
    }

    LOG_I("Ports on Bridge, link up cnt:%d\r\n", bridge_port_up);
}

int cmd_bridge(int argc, char **argv)
{
    LOG_I("bridge ...\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_bridge, bridege, bridge test.);

void lwip_status_update_task(void *pvParameters)
{
    LOG_I("lwip_status_update_task Run...\r\n");

    /* Infinite loop */
    while (1) {
        vTaskDelay(200);

        /* update link status */
        eth_link_state_update(&ethernet_netif);

#if LWIP_DHCP
        /* update dhcp status */
        //dhcp_sta_update(&ethernet_netif);
#endif
    }
}

int app_emac_init(void)
{
    emac_config_t cfg;

    /* Emac gpio init */
    board_emac_gpio_init();
    LOG_I("EMAC GPIO init OK!\r\n");

    platform_get_mac(cfg.mac_addr);
    eth_emac_hw_config(&cfg);

    /* EMAC init and netif config */
    netif_add(&ethernet_netif, NULL, NULL, NULL, NULL, eth_emac_if_init, tcpip_input);

    // XXX: ethernetif_init will set link up, so directly update
    #if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(&ethernet_netif, port_link_status_updated);
    #endif

    /* lwip status update task */
    xTaskCreate((void *)lwip_status_update_task, (char *)"lwip_sta_update", 256, NULL, 15, NULL);

    return 0;
}
