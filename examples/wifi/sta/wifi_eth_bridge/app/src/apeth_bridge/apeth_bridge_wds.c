#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "netif/bridgeif.h"
#include "netifd_nano.h"
#include "fhost.h"
#include "fhost_wpa.h"
#include "wifi_mgmr_ext.h"
#include "wifi_wds.h"
#include "shell.h"
#include "app_apeth_bridge.h"

#ifdef CONFIG_WIFI_WDS
/* The example Ethernet bridge is owned by netifd. wifi_wds only sets the role and peer. Wi-Fi still starts through wifi_ap_start and wifi_sta_connect. */

static struct {
    SemaphoreHandle_t lock;
    volatile bool ready;
    bool configured;
    bool running;
    bool attached;
    bool ethernet_added;
    bool data_ready;
    bool ethernet_link;
    bool peer_active;
    uint8_t vif;
    wifi_wds_config_t config;
    struct netif *bridge_if;
    struct netif *wireless;
    int last_error;
    /* Wi-Fi events arrive from the timer task with the scheduler suspended. Record only the latest transition and apply it from poll. */
    volatile uint32_t radio_seq;
    volatile int8_t radio_edge;
    volatile bool radio_ap;
} bridge;

static void bridge_close_data(void);

static bool bridge_mac_parse(const char *text, uint8_t mac[6])
{
    unsigned int i;
    static const uint8_t zero[6];

    if (!text || strlen(text) != 17)
        return false;
    for (i = 0; i < 6; i++) {
        char byte[3];
        char *end;
        unsigned long value;
        if (i && text[i * 3 - 1] != ':')
            return false;
        byte[0] = text[i * 3];
        byte[1] = text[i * 3 + 1];
        byte[2] = '\0';
        if (!((byte[0] >= '0' && byte[0] <= '9') ||
              (byte[0] >= 'a' && byte[0] <= 'f') || (byte[0] >= 'A' && byte[0] <= 'F')) ||
            !((byte[1] >= '0' && byte[1] <= '9') ||
              (byte[1] >= 'a' && byte[1] <= 'f') || (byte[1] >= 'A' && byte[1] <= 'F')))
            return false;
        value = strtoul(byte, &end, 16);
        if (*end || value > 255)
            return false;
        mac[i] = value;
    }
    return !(mac[0] & 1) && memcmp(mac, zero, sizeof(zero));
}

static bool bridge_connect_pending(void)
{
    return rtos_task_get_handle("CONNECT_ENTRY") != NULL ||
           rtos_task_get_handle("CONNECT") != NULL;
}

static bool bridge_wifi_offline(void)
{
    wifi_wds_status_t sta;
    wifi_wds_status_t ap;

    if (!bridge.ready || bridge_connect_pending() ||
        wifi_mgmr_sta_state_get_ext() >= FHOST_VIF_SCANNING ||
        wifi_mgmr_ap_state_get() ||
        fhost_wpa_get_state(MGMR_VIF_STA) != FHOST_WPA_STATE_STOPPED ||
        fhost_wpa_get_state(MGMR_VIF_AP) != FHOST_WPA_STATE_STOPPED ||
        wifi_mgmr_wds_status_get(MGMR_VIF_STA, &sta) ||
        wifi_mgmr_wds_status_get(MGMR_VIF_AP, &ap))
        return false;
    return !sta.link_up && !ap.link_up;
}

static void netif_lwip_name(const struct netif *nif, char *out, size_t len)
{
    snprintf(out, len, "%c%c%u", nif->name[0], nif->name[1], (unsigned)nif->num);
}

static void bridge_unnumber(struct netif *nif)
{
    ip4_addr_t zero;

    ip4_addr_set_zero(&zero);
#if LWIP_DHCP
    dhcp_stop(nif);
#endif
    netif_set_addr(nif, &zero, &zero, &zero);
}

/* Caller holds the tcpip core lock. Do not take bridge.lock: bridge_ensure
 * already holds that mutex and then takes the core lock. */
int app_wds_bridge_in_use(const struct netif *netif)
{
    return netif != NULL && bridge.bridge_if == netif;
}

/* netif_find and bridgeif_add_port require the tcpip core lock. */
static int bridge_ensure(void)
{
    struct netif *ethernet = app_emac_netif();
    struct netif *wireless = fhost_to_net_if(bridge.vif);
    char bridge_name[8];
    char eth_name[8];
    char wifi_name[8];
    int ret = WIFI_WDS_ERR_NOT_READY;

    if (!ethernet || !wireless || !ethernet->linkoutput || !wireless->linkoutput)
        return WIFI_WDS_ERR_NOT_READY;

    LOCK_TCPIP_CORE();
    if (!bridge.bridge_if) {
        bridge.bridge_if = netifd_bridge_create("br");
        if (!bridge.bridge_if)
            goto end;
        bridge_unnumber(bridge.bridge_if);
    }
    netif_lwip_name(bridge.bridge_if, bridge_name, sizeof(bridge_name));
    netif_lwip_name(ethernet, eth_name, sizeof(eth_name));
    netif_lwip_name(wireless, wifi_name, sizeof(wifi_name));
    if (bridge.wireless && bridge.wireless != wireless) {
        char old_name[8];

        netif_lwip_name(bridge.wireless, old_name, sizeof(old_name));
        netif_set_link_down(bridge.wireless);
        if (netifd_bridge_remove_if(bridge_name, old_name))
            goto end;
        bridge.wireless = NULL;
        bridge.attached = false;
    }
    if (!bridge.ethernet_added) {
        bridge_unnumber(ethernet);
        if (netifd_bridge_add_if(bridge_name, eth_name))
            goto end;
        netif_set_up(ethernet);
        bridge.ethernet_added = true;
    }
    if (!bridge.wireless) {
        netif_set_flags(wireless, NETIF_FLAG_ETHERNET);
        bridge_unnumber(wireless);
        if (netifd_bridge_add_if(bridge_name, wifi_name))
            goto end;
        /* fhost leaves the wireless port link-up. Forwarding waits for WDS status. */
        netif_set_link_down(wireless);
        bridge.wireless = wireless;
    }
    netif_set_up(bridge.bridge_if);
    netif_set_default(bridge.bridge_if);
    bridge.attached = true;
    ret = 0;
end:
    UNLOCK_TCPIP_CORE();
    return ret;
}

static int bridge_configure(wifi_wds_role_t role, wifi_wds_peer_mode_t peer_mode,
                            const uint8_t peer[6], bool four_addr)
{
    wifi_wds_config_t config;
    wifi_wds_config_t disabled = {0};
    uint8_t vif = role == WIFI_WDS_ROLE_AP ? MGMR_VIF_AP : MGMR_VIF_STA;
    int ret;

    if (!bridge_wifi_offline())
        return WIFI_WDS_ERR_BUSY;
    if (four_addr && peer_mode == WIFI_WDS_PEER_STATIC_MAC && !peer)
        return WIFI_WDS_ERR_INVALID_ARGUMENT;
    bridge.running = false;
    if (bridge.configured && bridge.vif != vif) {
        ret = wifi_mgmr_wds_config_set(bridge.vif, &disabled);
        if (ret)
            return ret;
        bridge.configured = false;
    }
    memset(&config, 0, sizeof(config));
    if (four_addr) {
        /* The library transmit path reads enable. The compile macros do not select the frame format. */
        config.enable = 1;
        config.role = role;
        config.peer_mode = peer_mode;
        config.allow_normal_sta = 1;
        if (peer_mode == WIFI_WDS_PEER_STATIC_MAC)
            memcpy(config.peer_mac, peer, 6);
    }
    ret = wifi_mgmr_wds_config_set(vif, &config);
    if (!ret) {
        bridge.config = config;
        if (!four_addr)
            bridge.config.role = role;
        bridge.vif = vif;
        bridge.configured = true;
        /* Configuration attaches the bridge through netifd. Later wifi_ap_start and wifi_sta_connect only start the radio. */
        ret = bridge_ensure();
    }
    return ret;
}

static void bridge_close_data(void)
{
    bridge.data_ready = false;
    bridge.ethernet_link = false;
    bridge.peer_active = false;
    if (!bridge.attached || !bridge.wireless)
        return;
    LOCK_TCPIP_CORE();
    netif_set_link_down(bridge.wireless);
    if (bridge.bridge_if)
        bridgeif_fdb_flush(bridge.bridge_if);
    UNLOCK_TCPIP_CORE();
    wifi_mgmr_wds_fdb_flush(bridge.vif);
}

/* Caller holds bridge.lock. Keep the same lock order as bridge_ensure. */
static int bridge_release(void)
{
    wifi_wds_config_t disabled = {0};
    uint32_t seq = bridge.radio_seq;
    char bridge_name[8];
    int ret;

    if (!bridge_wifi_offline())
        return WIFI_WDS_ERR_BUSY;
    if (bridge.configured) {
        ret = wifi_mgmr_wds_config_set(bridge.vif, &disabled);
        if (ret)
            return ret;
    }
    /* If deletion fails, retain ownership but report the disabled state. */
    bridge.configured = false;
    memset(&bridge.config, 0, sizeof(bridge.config));
    bridge.running = false;
    bridge_close_data();

    LOCK_TCPIP_CORE();
    if (bridge.bridge_if) {
        netif_lwip_name(bridge.bridge_if, bridge_name, sizeof(bridge_name));
#if LWIP_DHCP
        dhcp_stop(bridge.bridge_if);
        dhcp_cleanup(bridge.bridge_if);
#endif
        /* deinit detaches the ports and cancels FDB aging before freeing. */
        if (netifd_bridge_delete(bridge_name)) {
            UNLOCK_TCPIP_CORE();
            return WIFI_WDS_ERR_NOT_READY;
        }
        bridge.bridge_if = NULL;
    }
    bridge.wireless = NULL;
    bridge.attached = false;
    bridge.ethernet_added = false;
    UNLOCK_TCPIP_CORE();
    if (bridge.radio_seq == seq)
        bridge.radio_edge = 0;
    return WIFI_WDS_OK;
}

static void bridge_publish_radio(bool ap, int8_t edge)
{
    bridge.radio_ap = ap;
    bridge.radio_edge = edge;
    bridge.radio_seq++;
}

static void bridge_apply_radio(void)
{
    uint32_t seq = bridge.radio_seq;
    int8_t edge = bridge.radio_edge;
    wifi_wds_role_t role = bridge.radio_ap ? WIFI_WDS_ROLE_AP : WIFI_WDS_ROLE_STA;

    if (edge == 0 || bridge.radio_seq != seq)
        return;
    if (bridge.configured && bridge.config.role == role) {
        if (edge > 0) {
            if (!bridge.attached)
                bridge.last_error = bridge_ensure();
            if (bridge.attached)
                bridge.running = true;
        } else {
            /* wifi_ap_stop and wifi_sta_disconnect have already stopped the radio. Stop forwarding here and keep the bridge. */
            bridge.running = false;
            bridge_close_data();
        }
    }
    if (bridge.radio_seq == seq)
        bridge.radio_edge = 0;
}

void app_wds_bridge_radio_up(bool ap)
{
    bridge_publish_radio(ap, 1);
}

void app_wds_bridge_radio_down(bool ap)
{
    bridge_publish_radio(ap, -1);
}

void app_wds_bridge_poll(void)
{
    wifi_wds_status_t status;
    bool data_ready;
    bool ethernet_link;
    bool forward_ready;
    bool flush;

    if (!bridge.lock || xSemaphoreTake(bridge.lock, 0) != pdTRUE)
        return;
    bridge_apply_radio();
    if (bridge.running && bridge.attached &&
        !wifi_mgmr_wds_status_get(bridge.vif, &status)) {
        LOCK_TCPIP_CORE();
        ethernet_link = app_emac_physical_link_up();
        if (bridge.config.enable) {
            forward_ready = ethernet_link &&
                            (bridge.config.role == WIFI_WDS_ROLE_AP ? status.link_up :
                             status.active);
            data_ready = status.active && ethernet_link;
        } else {
            forward_ready = ethernet_link && status.link_up;
            data_ready = forward_ready;
        }
        flush = (bridge.ethernet_link && !ethernet_link) ||
                (bridge.peer_active && !status.active);
        if (flush && bridge.bridge_if)
            bridgeif_fdb_flush(bridge.bridge_if);
        if (bridge.wireless) {
            if (forward_ready)
                netif_set_link_up(bridge.wireless);
            else
                netif_set_link_down(bridge.wireless);
        }
        /* Connection setup may temporarily select the unnumbered Wi-Fi port. */
        if (bridge.bridge_if)
            netif_set_default(bridge.bridge_if);
        UNLOCK_TCPIP_CORE();
        if (flush)
            wifi_mgmr_wds_fdb_flush(bridge.vif);
        bridge.data_ready = data_ready;
        bridge.ethernet_link = ethernet_link;
        bridge.peer_active = status.active;
    }
    xSemaphoreGive(bridge.lock);
}

int app_wds_bridge_init(void)
{
    bridge.lock = xSemaphoreCreateMutex();
    return bridge.lock ? 0 : -1;
}

void app_wds_bridge_wifi_ready(void)
{
    bridge.ready = true;
    if (!bridge.lock)
        return;
    xSemaphoreTake(bridge.lock, portMAX_DELAY);
    bridge.last_error = bridge_configure(WIFI_WDS_ROLE_AP, WIFI_WDS_PEER_STATIC_MAC, NULL, false);
    xSemaphoreGive(bridge.lock);
}

static void bridge_status(void)
{
    wifi_wds_status_t status = {0};
    char wifi_name[8] = "none";
    char bridge_name[8] = "none";
    bool eth_up;
    const char *role = "none";
    const char *peer_mode = "none";

    if (bridge.configured)
        wifi_mgmr_wds_status_get(bridge.vif, &status);
    LOCK_TCPIP_CORE();
    eth_up = app_emac_physical_link_up();
    if (bridge.attached && bridge.bridge_if && bridge.wireless) {
        netif_lwip_name(bridge.bridge_if, bridge_name, sizeof(bridge_name));
        netif_lwip_name(bridge.wireless, wifi_name, sizeof(wifi_name));
    }
    UNLOCK_TCPIP_CORE();
    if (bridge.configured)
        role = bridge.config.role == WIFI_WDS_ROLE_AP ? "ap" : "sta";
    if (bridge.config.enable)
        peer_mode = bridge.config.peer_mode == WIFI_WDS_PEER_AUTO_MULTI_AP ?
                    "auto" : "static";
    printf("{\"ready\":%u,\"configured\":%u,\"running\":%u,"
           "\"bridge\":\"%s\",\"wireless\":\"%s\",\"vif\":%u,\"role\":\"%s\","
           "\"peer_mode\":\"%s\",\"peer\":\"" MACSTR "\","
           "\"resolved_peer\":\"" MACSTR "\",\"discovery_pending\":%u,"
           "\"ethernet_link\":%u,\"peer_associated\":%u,"
           "\"addr4\":%u,\"tx_4addr_ready\":%u,\"rx_4addr_seen\":%u,\"active\":%u,"
           "\"data_ready\":%u,\"fdb_count\":%u,\"last_error\":%d}\r\n",
           bridge.ready, bridge.configured, bridge.running,
           bridge_name, wifi_name, bridge.vif, role,
           peer_mode,
           MAC2STR(bridge.config.peer_mac), MAC2STR(status.resolved_peer_mac),
           status.discovery_pending, eth_up, status.peer_associated,
           bridge.config.enable, status.tx_4addr_ready, status.rx_4addr_seen,
           status.active, bridge.data_ready, status.fdb_count, bridge.last_error);
}

static void bridge_usage(void)
{
    printf("wifi_wds config <ap|sta> [auto|<peer-mac>]\r\n"
           "wifi_wds status\r\n"
           "wifi_wds release\r\n"
           "wifi_ap_start -s <ssid> [-k <key>] [-a <akm>] [-c <ch>] [-b 0|1|2] -g 0 -d 0\r\n"
           "wifi_sta_connect [-b <bssid>] [-2] [-D] <ssid> [password]\r\n"
           "wifi_ap_stop\r\n"
           "wifi_sta_disconnect\r\n"
           "Boot is three-address AP. config <ap|sta> keeps three-address and selects the role.\r\n"
           "config <ap|sta> auto or a peer MAC enables four-address WDS.\r\n"
           "MAT translates only while four-address WDS is off. CONFIG_WIFI_MAT=n omits translation.\r\n"
           "Run config before starting Wi-Fi. Stop Wi-Fi before changing roles.\r\n"
           "config creates the bridge through netifd. Do not create a second bridge by hand.\r\n"
           "Stop both Wi-Fi roles before release. Run config again to recreate the bridge.\r\n"
           "netifd bridge delete/remove cannot change a bridge held by wifi_wds.\r\n"
           "Pass -g 0 -d 0 on the AP and -D on the STA so a bridge port does not get an IP.\r\n"
           "For a static four-address peer, pass that MAC to STA -b.\r\n");
}

static int cmd_wifi_wds(int argc, char **argv)
{
    int ret = WIFI_WDS_ERR_INVALID_ARGUMENT;
    uint8_t peer[6];
    wifi_wds_role_t role;

    if (argc < 2 || !bridge.lock)
        goto usage;
    xSemaphoreTake(bridge.lock, portMAX_DELAY);
    if (!strcmp(argv[1], "status") && argc == 2) {
        bridge_status();
        xSemaphoreGive(bridge.lock);
        return 0;
    }
    if (!strcmp(argv[1], "release") && argc == 2) {
        ret = bridge_release();
        goto done;
    }
    if (strcmp(argv[1], "config") || argc < 3 ||
        (strcmp(argv[2], "ap") && strcmp(argv[2], "sta"))) {
        xSemaphoreGive(bridge.lock);
        goto usage;
    }
    role = !strcmp(argv[2], "ap") ? WIFI_WDS_ROLE_AP : WIFI_WDS_ROLE_STA;
    if (argc == 3) {
        ret = bridge_configure(role, WIFI_WDS_PEER_STATIC_MAC, NULL, false);
    } else if (argc == 4 && !strcmp(argv[3], "auto")) {
        ret = bridge_configure(role, WIFI_WDS_PEER_AUTO_MULTI_AP, NULL, true);
    } else if (argc == 4 && bridge_mac_parse(argv[3], peer)) {
        ret = bridge_configure(role, WIFI_WDS_PEER_STATIC_MAC, peer, true);
    } else {
        xSemaphoreGive(bridge.lock);
        goto usage;
    }
done:
    bridge.last_error = ret;
    xSemaphoreGive(bridge.lock);
    printf("wifi_wds %s ret=%d\r\n", argv[1], ret);
    return ret;
usage:
    bridge_usage();
    return ret;
}
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_wds, wifi_wds, WDS config status and release);

#else
int app_wds_bridge_init(void) { return 0; }
void app_wds_bridge_wifi_ready(void) {}
void app_wds_bridge_poll(void) {}
void app_wds_bridge_radio_up(bool ap) { (void)ap; }
void app_wds_bridge_radio_down(bool ap) { (void)ap; }
#endif
