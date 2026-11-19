#include <string.h>
#include "netif/bridgeif.h"
#include "shell.h"

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

static struct netif ethernet_netifs[LWIP_EMAC_PORT_COUNT];

/* Referenced by the example linker script for uncached DMA storage. */
int nocache_memory_max = 1;

uint8_t app_emac_port_count(void)
{
    return LWIP_EMAC_PORT_COUNT;
}

struct netif *app_emac_netif(uint8_t port)
{
    return port < LWIP_EMAC_PORT_COUNT ? &ethernet_netifs[port] : NULL;
}

/* Caller holds the TCP/IP core lock. Each bit represents a physical port. */
uint8_t app_emac_physical_link_mask(void)
{
    uint8_t mask = 0;

    for (uint8_t port = 0; port < LWIP_EMAC_PORT_COUNT; port++) {
        if (netif_is_link_up(&ethernet_netifs[port]))
            mask |= 1U << port;
    }
    return mask;
}

static void lwip_status_update_task(void *arg)
{
    (void)arg;
    for (;;) {
        for (uint8_t port = 0; port < LWIP_EMAC_PORT_COUNT; port++)
            eth_link_state_update(&ethernet_netifs[port]);
        app_wds_bridge_poll();
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

int app_emac_init(void)
{
    emac_config_t cfg;
    uint8_t port;

    for (port = 0; port < LWIP_EMAC_PORT_COUNT; port++) {
        board_emac_rmii_gpio_init(port);
        board_emac_mdio_gpio_init(port);
        memset(&cfg, 0, sizeof(cfg));
        platform_get_mac(cfg.mac_addr);
        if (port) {
            /* Give the second port a distinct locally administered MAC. */
            cfg.mac_addr[0] |= 0x02;
            cfg.mac_addr[5] ^= port;
        }
        if (eth_emac_netif_add(&ethernet_netifs[port], port, &cfg) != ERR_OK) {
            LOG_E("EMAC%u netif initialization failed\r\n", port);
            goto failed;
        }
    }
    if (xTaskCreate(lwip_status_update_task, "bridge_poll", 1024, NULL, 15,
                    NULL) != pdPASS) {
        LOG_E("Bridge polling task creation failed\r\n");
        goto failed;
    }
    return 0;
failed:
    while (port > 0)
        eth_emac_netif_remove(&ethernet_netifs[--port]);
    return -1;
}

/****************************************************************************
 * Bridge FDB dump / lookup
 ****************************************************************************/
extern struct netif *g_bridge; /* created by netifd bridge create */

static int br_fdb_mac_parse(const char *str, struct eth_addr *addr)
{
    unsigned int m[6];
    char buf[18];
    size_t len = strlen(str);
    int i;

    if (len == 17) {
        if (str[2] != ':' && str[2] != '-') {
            return -1;
        }
        strncpy(buf, str, sizeof(buf));
        for (i = 2; i < 17; i += 3) {
            if (buf[i] == '-') {
                buf[i] = ':';
            }
        }
        if (sscanf(buf, "%2x:%2x:%2x:%2x:%2x:%2x", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]) != 6) {
            return -1;
        }
    } else if (len == 12) {
        if (sscanf(str, "%2x%2x%2x%2x%2x%2x", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]) != 6) {
            return -1;
        }
    } else {
        return -1;
    }

    for (i = 0; i < 6; i++) {
        if (m[i] > 0xff) {
            return -1;
        }
        addr->addr[i] = (u8_t)m[i];
    }
    return 0;
}

static void br_fdb_ports_print(bridgeif_portmask_t ports)
{
    struct netif *portif;
    bridgeif_portmask_t mask;
    u8_t i;
    int first = 1;

    if (ports == BR_FLOOD) {
        printf("flood");
        return;
    }
    for (i = 0, mask = 1; mask; i++, mask <<= 1) {
        if (ports & mask) {
            portif = bridgeif_port_get(g_bridge, i);
            if (!first) {
                printf(",");
            }
            if (portif != NULL) {
                printf("%c%c%d", portif->name[0], portif->name[1], portif->num);
            } else {
                printf("port%u", i);
            }
            first = 0;
        }
    }
}

static void br_fdb_dump(int argc, char **argv)
{
    struct eth_addr addr;
    u16_t idx;

    if (g_bridge == NULL) {
        printf("bridge not created (run wifi_wds config first)\r\n");
        return;
    }

    if (argc >= 2 && strcmp(argv[1], "get") == 0) {
        bridgeif_portmask_t ports;

        if (argc < 3 || br_fdb_mac_parse(argv[2], &addr)) {
            printf("usage: br_fdb get <aa:bb:cc:dd:ee:ff>\r\n");
            return;
        }
        ports = bridgeif_fdb_lookup(g_bridge, &addr);
        printf("%02x:%02x:%02x:%02x:%02x:%02x -> ",
               addr.addr[0], addr.addr[1], addr.addr[2],
               addr.addr[3], addr.addr[4], addr.addr[5]);
        br_fdb_ports_print(ports);
        printf("\r\n");
        return;
    }

    printf("%-4s %-8s %-18s %-10s %s\r\n", "idx", "type", "mac", "port", "age(s)");
    for (idx = 0;; idx++) {
        struct eth_addr a;
        u8_t is_static;
        bridgeif_portmask_t ports;
        u16_t age;

        if (bridgeif_fdb_entry_get(g_bridge, idx, &a, &is_static, &ports, &age) != ERR_OK) {
            break;
        }
        printf("%-4u %-8s %02x:%02x:%02x:%02x:%02x:%02x  ",
               idx, is_static ? "static" : "dynamic",
               a.addr[0], a.addr[1], a.addr[2], a.addr[3], a.addr[4], a.addr[5]);
        br_fdb_ports_print(ports);
        if (!is_static) {
            printf(" %u", age);
        }
        printf("\r\n");
    }
}

static void cmd_br_fdb(int argc, char **argv)
{
    /* The upstream release command can detach and free the bridge. */
    LOCK_TCPIP_CORE();
    br_fdb_dump(argc, argv);
    UNLOCK_TCPIP_CORE();
}

SHELL_CMD_EXPORT_ALIAS(cmd_br_fdb, br_fdb, bridge fdb dump | get <mac>.);
