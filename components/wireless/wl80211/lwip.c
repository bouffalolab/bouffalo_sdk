/* LWIP Porting */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lwip/etharp.h>
#include <lwip/mem.h>
#include <lwip/netifapi.h>
#include <lwip/pbuf.h>
#include <lwip/tcpip.h>
#include <dhcp_server.h>
#include <lwip/dns.h>
#include <lwip/inet.h>
#include <lwip/prot/ethernet.h>

#include "wl80211_mac.h"
#include "wl80211_platform.h"

#include "async_event.h"

struct netif vif2netif[WL80211_VIF_MAX];
static int g_wl80211_ap_dhcpd_running;

#ifndef CONFIG_WL80211_RX_ZEROCOPY_THRES
#define CONFIG_WL80211_RX_ZEROCOPY_THRES 500
#endif

_Static_assert(sizeof(struct wl80211_mac_tx_desc) <= PBUF_LINK_ENCAPSULATION_HLEN, "tx_desc too large");
_Static_assert(sizeof(struct rx_info) >= sizeof(struct pbuf_custom), "pbuf_custom too large");

/**
 * Intra-BSS forwarding for WiFi AP mode.
 * When our chip acts as AP, forward frames between connected stations.
 * - Broadcast/multicast frames: forward to all stations
 * - Unicast frames not destined to AP: forward to the target station
 */
static void wl80211_tcpip_forward_frame(void *frame, uint32_t len)
{
    struct netif *ap_netif = &vif2netif[WL80211_VIF_AP];
    struct pbuf *fwd;

    if (!netif_is_link_up(ap_netif)) {
        return;
    }

    /* Allocate pbuf with TX encapsulation headroom for MAC descriptor */
    fwd = pbuf_alloc(PBUF_RAW_TX, len, PBUF_RAM);
    if (fwd == NULL) {
        return;
    }

    memcpy(fwd->payload, frame, len);

    ap_netif->linkoutput(ap_netif, fwd);
    pbuf_free(fwd);
}

void wl80211_tcpip_input(uint8_t vif_type, void *rxhdr, void *buf, uint32_t frm_len,
                         uint32_t status /* enum rx_status_bits */)
{
    struct pbuf_custom *pc;
    struct pbuf *p;

    // TODO If the current packet is being forwarded to the host, use zerocopy.
    if (frm_len < CONFIG_WL80211_RX_ZEROCOPY_THRES) {
        pc = malloc(sizeof(struct pbuf_custom) + frm_len);
        if (pc == NULL) {
            /// TODO
            assert(0);
        }

        p = pbuf_alloced_custom(PBUF_RAW, frm_len, PBUF_REF | PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS, pc, pc + 1,
                                frm_len);
        pc->custom_free_function = (void *)free;

        memcpy(p->payload, buf, frm_len);
        // free rx right now
        wl80211_mac_rx_free(rxhdr);
    } else {
        pc = rxhdr;
        p = pbuf_alloced_custom(PBUF_RAW, frm_len, PBUF_REF | PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS, pc, buf, frm_len);
        pc->custom_free_function = (void *)wl80211_mac_rx_free;
    }

    assert(p != NULL);
    assert(p->ref > 0);

    /* AP intra-BSS forwarding: forward frames between connected stations */
    if (vif_type == WL80211_VIF_AP && frm_len >= SIZEOF_ETH_HDR) {
        struct eth_hdr *ethhdr = (struct eth_hdr *)p->payload;

        if (ethhdr->dest.addr[0] & 0x01) {
            /* Broadcast/multicast: forward to all stations, then process locally */
            wl80211_tcpip_forward_frame(p->payload, frm_len);
        } else if (memcmp(ethhdr->dest.addr, vif2netif[WL80211_VIF_AP].hwaddr, ETH_HWADDR_LEN) != 0) {
            /* Unicast not destined to AP: forward to target station only */
            wl80211_tcpip_forward_frame(p->payload, frm_len);
            pbuf_free(p);
            return;
        }
        /* Unicast destined to AP: fall through to normal lwIP processing */
    }

    assert(vif2netif[vif_type].input != NULL);
    if (vif2netif[vif_type].input(p, &vif2netif[vif_type])) {
        pbuf_free(p);
    }
}

/* lwip output function */
err_t wl80211_output(struct netif *net_if, struct pbuf *buf)
{
    struct wl80211_tx_header *txhdr;
    struct iovec txseg[5];
    int seg_cnt;
    int payload_len = buf->tot_len;
    int remain_len = payload_len;

    // first segment record payload
    txseg[0].iov_base = buf->payload;
    txseg[0].iov_len = buf->len;
    remain_len -= buf->len;

    // Increase the ref count so that the buffer is not freed by the networking
    // stack until it is actually sent over the WiFi interface
    pbuf_ref(buf);

    if (pbuf_header(buf, PBUF_LINK_ENCAPSULATION_HLEN)) {
        abort();
    }

    /* get tx desc */
    txhdr = (void *)ALIGN4_HI((uint32_t)buf->payload);

    struct pbuf *buf_t = buf->next;
    int idx = 1;
    while (remain_len && buf_t && (idx < 5)) {
        txseg[idx].iov_base = buf_t->payload;
        txseg[idx].iov_len = buf_t->len;

        remain_len -= buf_t->len;
        idx++;
        buf_t = buf_t->next;
    }
    seg_cnt = idx;
    assert(remain_len == 0);

    if (wl80211_mac_tx(net_if - vif2netif, txhdr, 0, txseg, seg_cnt, pbuf_free, buf)) {
        pbuf_free(buf);
        return ERR_BUF;
    }

    return (ERR_OK);
}

/* raw packet send: eapol and mgmt */
struct pbuf_custom_raw {
    void *opaque;
    void (*cb)(void *);

    struct pbuf_custom pc;
};

static void pbuf_custom_raw_free(struct pbuf_custom *pc)
{
    struct pbuf_custom_raw *p = container_of(pc, struct pbuf_custom_raw, pc);

    if (p->cb) {
        p->cb(p->opaque);
    }
    wl80211_platform_free_wram_nolimit(p);
}

int wl80211_output_raw(uint8_t vif_type, void *buffer, uint16_t len, unsigned int flags, void (*cb)(void *),
                       void *opaque)
{
    struct pbuf *p;
    struct pbuf_custom_raw *pc;
    struct wl80211_tx_header *txhdr;
    struct iovec txseg[1];

    // alloc tx packet in lwip heap (wifi ram)
    uint16_t payload_len = (uint16_t)(LWIP_MEM_ALIGN_SIZE(PBUF_RAW_TX) + LWIP_MEM_ALIGN_SIZE(len));

    pc = (struct pbuf_custom_raw *)wl80211_platform_malloc_wram_nolimit(payload_len + sizeof(struct pbuf_custom_raw));
    if (!pc) {
        return -1;
    }

    // reinit pbuf as pbuf_custom_wpa
    p = pbuf_alloced_custom(PBUF_RAW_TX, len, PBUF_RAM, &pc->pc, pc + 1, payload_len);
    assert(p != NULL);
    pc->pc.custom_free_function = (void *)pbuf_custom_raw_free;
    pc->cb = cb;
    pc->opaque = opaque;

    if (ERR_OK != pbuf_take(p, buffer, len)) {
        pbuf_free(p);
        return -2;
    }

    txseg[0].iov_base = p->payload;
    txseg[0].iov_len = p->len;

    if (pbuf_header(p, PBUF_LINK_ENCAPSULATION_HLEN)) {
        abort();
    }

    /* get tx desc */
    txhdr = (void *)ALIGN4_HI((uint32_t)p->payload);

    if (wl80211_mac_tx(vif_type, txhdr, flags, txseg, 1, pbuf_free, p)) {
        pbuf_free(p);
        return -3;
    } else {
        return 0;
    }
}

static err_t wl80211_netif_init(struct netif *net_if)
{
    err_t status = ERR_OK;

#if LWIP_NETIF_HOSTNAME
    {
        /* Initialize interface hostname */
        net_if->hostname = "ctrl";
    }
#endif /* LWIP_NETIF_HOSTNAME */

    net_if->name[0] = 'w';
    net_if->name[1] = 'l';

    net_if->output = etharp_output;

#ifdef CFG_IPV6
    net_if->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6 |
                    NETIF_FLAG_UP;
    net_if->output_ip6 = ethip6_output;
#else
    net_if->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_UP;
#endif

    net_if->hwaddr_len = ETHARP_HWADDR_LEN;

    platform_get_mac((uintptr_t)net_if->state, (uint8_t *)net_if->hwaddr);

    wl80211_printf("mac addr: %02X:%02X:%02X:%02X:%02X:%02X\r\n", net_if->hwaddr[0], net_if->hwaddr[1],
                   net_if->hwaddr[2], net_if->hwaddr[3], net_if->hwaddr[4], net_if->hwaddr[5]);

    // hwaddr is updated in net_if_add
    net_if->mtu = 1500;
    net_if->linkoutput = wl80211_output;

    return status;
}

int wl80211_lwip_init(void)
{
    err_t status;

    // Add default sta netif
    status = netifapi_netif_add(&vif2netif[WL80211_VIF_STA], (const ip4_addr_t *)NULL, (const ip4_addr_t *)NULL,
                                (const ip4_addr_t *)NULL, (void *)WL80211_VIF_STA, wl80211_netif_init, tcpip_input);
    assert(status == ERR_OK);

    // Add default ap netif
    status = netifapi_netif_add(&vif2netif[WL80211_VIF_AP], (const ip4_addr_t *)NULL, (const ip4_addr_t *)NULL,
                                (const ip4_addr_t *)NULL, (void *)WL80211_VIF_AP, wl80211_netif_init, tcpip_input);
    assert(status == ERR_OK);

    return 0;
}

#ifdef COMPAT_WIFI_MGMR
/* wifi_mgmr lwip porting  */

#include "wifi_mgmr.h"

// for wifi mgmr
static void ip_got_cb(struct netif *netif)
{
    if (!ip_addr_isany_val(netif->ip_addr)) {
        netifapi_netif_set_default(&vif2netif[WL80211_VIF_STA]);
        async_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 1);
    } else {
        async_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
    }
}

void _wifi_mgmr_sta_link_up(int use_dhcp)
{
    netif_set_status_callback(&vif2netif[WL80211_VIF_STA], ip_got_cb);
    netif_set_link_up(&vif2netif[WL80211_VIF_STA]);

    if (use_dhcp) {
        if (netifapi_dhcp_start(&vif2netif[WL80211_VIF_STA]) == ERR_OK) {
            wl80211_printf("start dhcpc success.\n");
        } else {
            // system in strange state, abort it.
            wl80211_printf("start dhcpc fail.\n");
            abort();
        }
    }
}

void _wifi_mgmr_sta_link_down(void)
{
    netif_set_link_down(&vif2netif[WL80211_VIF_STA]);
    netif_set_status_callback(&vif2netif[WL80211_VIF_STA], NULL);

    if (netifapi_dhcp_stop(&vif2netif[WL80211_VIF_STA]) == ERR_OK) {
        wl80211_printf("stop dhcpc success.\n");
    } else {
        // system in strange state, abort it.
        wl80211_printf("stop dhcpc fail.\n");
        abort();
    }
}

void _wifi_mgmr_ip_got_dump(void)
{
    char *state, *connected;
    struct netif *netif = &vif2netif[WL80211_VIF_STA];

    if (wifi_mgmr_sta_state_get()) {
        state = "UP";
        connected = ",CONNECTED";
    } else {
        state = "DOWN";
        connected = "";
    }

    extern size_t kfree_size(uint32_t heap_id);
    wl80211_printf("Memory left is %d Bytes\r\n", kfree_size(0));
    wl80211_printf("[%d]  %c%c: MAC=%02x:%02x:%02x:%02x:%02x:%02x ip=%d.%d.%d.%d/%d %s%s\n", netif->num, netif->name[0],
                   netif->name[1], netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3],
                   netif->hwaddr[4], netif->hwaddr[5], ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr)) & 0xff,
                   (ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr)) >> 8) & 0xff,
                   (ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr)) >> 16) & 0xff,
                   (ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr)) >> 24) & 0xff,
                   32 - __builtin_clz(ip4_addr_get_u32(ip_2_ip4(&netif->netmask))), state, connected);
}

int wifi_mgmr_sta_ip_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns)
{
    struct netif *netif = &vif2netif[WL80211_VIF_STA];
    const ip_addr_t *dns_addr;
    *addr = ip4_addr_get_u32(netif_ip4_addr(netif));
    *mask = ip4_addr_get_u32(netif_ip4_netmask(netif));
    *gw = ip4_addr_get_u32(netif_ip4_gw(netif));

    dns_addr = dns_getserver(0);
    *dns = ip4_addr_get_u32(ip_2_ip4(dns_addr));

    return 0;
}

int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns)
{
    struct netif *netif = &vif2netif[WL80211_VIF_STA];
    ip_addr_t dns_addr;
    ip_addr_t _ip, _mask, _gw;

    ip_addr_set_ip4_u32_val(_ip, ip);
    ip_addr_set_ip4_u32_val(_mask, mask);
    ip_addr_set_ip4_u32_val(_gw, gw);
    netifapi_netif_set_addr(netif, (const ip4_addr_t *)&_ip, (const ip4_addr_t *)&_mask, (const ip4_addr_t *)&_gw);

    if (dns != 0) {
        ip_addr_set_ip4_u32_val(dns_addr, dns);
        dns_setserver(0, &dns_addr);
    }

    return 0;
}

void _wifi_mgmr_ap_stop_dhcpd(void)
{
    if (!g_wl80211_ap_dhcpd_running) {
        return;
    }

    wl80211_printf("stop dhcpd\n");

    extern err_t dhcp_server_stop(struct netif * netif);
    dhcpd_clear_dns_server(&vif2netif[WL80211_VIF_AP]);
    dhcp_server_stop(&vif2netif[WL80211_VIF_AP]);
    g_wl80211_ap_dhcpd_running = 0;
}

void _wifi_mgmr_ap_start_dhcpd(bool use_ipcfg, bool use_dhcpd, int start, int limit, uint32_t ap_ipaddr,
                               uint32_t ap_mask)
{
    struct netif *ap_netif = &vif2netif[WL80211_VIF_AP];

    if (g_wl80211_ap_dhcpd_running) {
        _wifi_mgmr_ap_stop_dhcpd();
    }

    if (use_ipcfg) {
        if (ap_ipaddr && ap_mask) {
            ip4_addr_t ipaddr;
            ip4_addr_t netmask;
            ip4_addr_t gw;

            ip4_addr_set_u32(&ipaddr, ap_ipaddr);
            ip4_addr_set_u32(&netmask, ap_mask);
            ip4_addr_set_u32(&gw, ap_ipaddr);

            wl80211_printf("DHCPD: Setting AP IP: %s\r\n", ip4addr_ntoa(&ipaddr));
            wl80211_printf("DHCPD: Setting AP Mask: %s\r\n", ip4addr_ntoa(&netmask));

            netifapi_netif_set_addr(ap_netif, &ipaddr, &netmask, &gw);
            netifapi_netif_set_up(ap_netif);
        }

        if (use_dhcpd) {
            wl80211_printf("DHCPD: Starting with pool start=%d, limit=%d\r\n", start, limit);

            netifapi_netif_set_link_up(ap_netif);
            dhcpd_start(ap_netif, start, limit);
            g_wl80211_ap_dhcpd_running = 1;
        }
    } else {
        netifapi_netif_set_link_up(ap_netif);
    }
}

#define SET_IPV4_USAGE                                             \
    "set_ipv4 [ip] [dns] [gw] [mask]\r\n"                          \
    "\t ip: set static ip\r\n"                                     \
    "\t dns: set local host dns\r\n"                               \
    "\t gw: set local host gate way. default is 255.255.255.0\r\n" \
    "\t mask: set local host mask. default is 255.255.255.0\r\n"

void set_ipv4_cmd(int argc, char **argv)
{
    char *addr, *mask, *gw, *dns;
    if (argc < 3) {
        printf("%s", SET_IPV4_USAGE);
        return;
    }

    /* ip addr */
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = "127.0.0.1";
    }

    /* ip dns */
    if (argc > 2) {
        dns = argv[2];
    } else {
        dns = "0.0.0.0";
    }

    /* ip gw */
    if (argc > 3) {
        gw = argv[3];
    } else {
        gw = "255.255.255.0";
    }

    /* ip mask */
    if (argc > 4) {
        mask = argv[4];
    } else {
        mask = "255.255.255.0";
    }

    printf("addr:%s,mask:%s,gw:%s,dns:%s\r\n", addr, mask, gw, dns);

    wifi_mgmr_sta_ip_set(inet_addr(addr), inet_addr(mask), inet_addr(gw), inet_addr(dns));
    return;
}
#endif
