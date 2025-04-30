#include <sdiowifi_securec_wrap.h>
#include <stdbool.h>
#include <string.h>
#include <sdiowifi_platform_adapt.h>
#include <lwip/netif.h>
//#include <bl_wifi.h>
#include <wifi_mgmr_ext.h>
//#include <vfs.h>
#ifdef CFG_DUAL_ETH
#include <lwip/netifapi.h>
#include <netif/ethernet.h>
#include <net_def.h>
#include <lwip/inet.h>
#include <lwip/lwip_napt.h>
#include <sdio_pkt_hooks.h>
#include "netif/ethernet.h"
#include "lwip/etharp.h"
#endif
#include <rnm_server.h>

#include <sdiowifi_config.h>
#include <sdiowifi_mgmr.h>
#include <sdiowifi_mgmr_type.h>
#include <sdiowifi_tty.h>

#include "sdio_port.h"

#if 0
#if defined(SDIO_REUSE_LP_RAM) && !(defined(CFG_BL616) && defined(LP_APP))
#error "SDIO lp ram reuse configuration error"
#endif
#endif

#define SDU_WORKER_EV_RESET (1 << 0)

void notify_sdu_worker(int event, int isr);
void sdu_software_reset_set(int enable);

sdiowifi_mgmr_t g_sdiowifi_mgmr;

int sdiowifi_update_tx_credit(struct sdiowifi_tx_buf *tb)
{
    *(volatile unsigned char *)(0x2000d167) = (unsigned char)tb->una_seq;
    bf1b_write_s_reg(&g_sdiowifi_mgmr.trcver, 0, tb->una_seq + tb->window_size);

    return 0;
}

static int rnm_tx_data(void *env, rnm_output_data_type_t type, const void *data, uint16_t data_length)
{
    int ret = RNM_OUTPUT_FN_SUCCESS;
    int tx_ret = 0;
    net_wifi_trcver_t *trcver = (net_wifi_trcver_t *)env;

    if (type == RNM_OUT_COMMAND) {
        tx_ret = bf1b_msg_send(trcver, BF1B_MSG_TYPE_CMD, data, data_length);
    } else if (type == RNM_OUT_SNIFFER_FRAME) {
        // XXX not implemented
    }
    if (tx_ret == BF1B_MSG_ERR_DESC_USED) {
        HR_LOGW("cmd BF1B_MSG_ERR_DESC_USED -> sdiowifi_mgmr_reinit -> SDU_WORKER_EV_RESET\r\n");
        sdiowifi_mgmr_reinit(false);
        hr_coredump();
        ret = RNM_OUTPUT_FN_ERROR_OTHER;
    } else if (tx_ret == BF1B_MSG_ERR_TIMEOUT) {
        ret = RNM_OUTPUT_FN_TIMEOUT;
    } else if (tx_ret != 0) {
        ret = RNM_OUTPUT_FN_ERROR_OTHER;
    }
    return ret;
}

#ifdef SDIOWIFI_HEARTBEAT
static int heartbeat_failure(void *arg)
{
    sdiowifi_mgmr_t *sdm = (sdiowifi_mgmr_t *)arg;

    (void)sdm;

    HR_LOGW("heartbeat fail\r\n");
    sdiowifi_mgmr_reinit(false);
    return 0;
}
#endif

static void ready_cb(void *arg)
{
    sdiowifi_mgmr_t *sdm = (sdiowifi_mgmr_t *)arg;
    HR_LOGI("device ready\r\n");

    bflb_net_wifi_trcver_set_present(&sdm->trcver, true);

    sdiowifi_update_tx_credit(&sdm->tx_desc);
#ifdef DHCP_IN_EMB
#ifdef CFG_DUAL_ETH
    rnms_notify_sdioeth_ip_addr(sdm->rnm);
#else
    rnms_notify_sta_ip_addr(sdm->rnm);
#endif
#endif

#ifdef SDIOWIFI_HEARTBEAT
    rnms_heartbeat_register_failure_cb(sdm->rnm, heartbeat_failure, sdm);
    rnms_heartbeat_start(sdm->rnm, 200, 2);
#endif

#if SDIOWIFI_TTY
    sdiowifi_tty_send_bsr(sdm, 0, true, 0);
#endif
}

static void tx_buf_init(sdiowifi_mgmr_t *sdm)
{
    sdiowifi_tx_buf_init(&sdm->tx_desc);

    sdiowifi_tx_buf_lpmem_register(&sdm->tx_desc);
    
#if CONFIG_SDIO_HIGH_PERFORMANCE
    extern uint8_t _heap_wifi_start;
    extern uint8_t _heap_wifi_size;
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, &_heap_wifi_start, (size_t)&_heap_wifi_size);
#endif
#if 0
#ifdef CFG_BL616
#ifdef LP_APP
#ifdef SDIO_REUSE_LP_RAM
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, (void *)SW_TX_BUF_START, SW_TX_BUF_SIZE);
#else
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;
    if (em_size == 0) {
        sdiowifi_tx_buf_mem_register(&sdm->tx_desc, (void *)(0x23010000 + (160 - 32) * 1024), 32 * 1024);
    }
#endif // SDIO_REUSE_LP_RAM
#endif // LP_APP

#ifndef BL_SDIO_BUFF_REDUCTION
    extern uint8_t _heap_wifi_start;
    extern uint8_t _heap_wifi_size;
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, &_heap_wifi_start, (size_t)&_heap_wifi_size);
#endif

#else
    static uint32_t tx_buf[22 * 1024 / 4] __attribute__((section(".wifi_ram")));
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, tx_buf, sizeof(tx_buf));
#endif
#endif
    sdiowifi_tx_buf_init_done(&sdm->tx_desc);
}

#ifdef CFG_DUAL_ETH

inet_if_t sdio_eth_netif;

int sdioeth_netif_mac_get(uint8_t mac[6]) {

#ifdef CFG_BL616
    wifi_mgmr_sta_mac_get(mac);
#else
    bl_wifi_mac_addr_get(mac);
#endif

    mac[5] += 2;

    return 0;
}

static err_t sdio_net_if_output(struct netif *net_if, struct pbuf *p_buf)
{
    err_t status = ERR_BUF;

    bool is_sta = false;

    pbuf_ref(p_buf);

    // Push the buffer and verify the status
    if (netif_is_up(net_if) && !bl_sdio_pkt_eth_output_hook(is_sta, p_buf, bl_sdio_pkt_eth_output_hook_arg))
    {
        status = ERR_OK;
    }
    else
    {
        // Failed to push message to TX task, call pbuf_free only to decrease ref count
        pbuf_free(p_buf);
    }

    return status;
}

static int sdioeth_netif_init(inet_if_t *netif) {
    uint8_t mac[6];

    netif->name[0] = 's';
    netif->name[1] = 'd';

    netif->output = etharp_output;
    
    sdioeth_netif_mac_get(mac);

    netif->hwaddr[0] = mac[0];
    netif->hwaddr[1] = mac[2];
    netif->hwaddr[2] = mac[2];
    netif->hwaddr[3] = mac[3];
    netif->hwaddr[4] = mac[4];
    netif->hwaddr[5] = mac[5] + 1;

    #ifdef CFG_IPV6
    netif->flags = NETIF_FLAG_LINK_UP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    netif->output_ip6 = ethip6_output;
    #else
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    #endif

    #if LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
    #endif

    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    // hwaddr is updated in net_if_add
    netif->mtu = 1500;

    netif->linkoutput = sdio_net_if_output;

    return 0;
}

static void sdio_eth_init(void) {

    netifapi_netif_add(&sdio_eth_netif, NULL, NULL, NULL, NULL, sdioeth_netif_init, tcpip_input);

    netifapi_netif_set_up(&sdio_eth_netif);

    netifapi_netif_set_link_up(&sdio_eth_netif);
}

uint32_t get_alternate_ip(uint32_t current_ip, uint32_t current_mask) {
    uint32_t network = current_ip & current_mask;
    
    uint32_t host_mask = ~current_mask;

    uint32_t new_network = network + (host_mask + 1);
    
    uint32_t new_ip = (new_network & current_mask) | (current_ip & ~current_mask);
    
    return new_ip;
}

#ifdef SDIOWIFI_OFFLINE
uint32_t sta_addr_default = 0xc8c8a8c0;
uint32_t sta_mask_default = 0x00ffffff;
uint32_t sta_gw_default = 0x01c8a8c0;
uint32_t sta_dns_default = 0x00000000;
int sdio_mgmr_sdioeth_ip4_addr_default_set(uint32_t addr, uint32_t mask, uint32_t gw, uint32_t dns)
{
    sta_addr_default = addr;
    sta_mask_default = mask;
    sta_gw_default = gw;
    sta_dns_default = dns;

    return 0;
}
#endif

int sdio_mgmr_sdioeth_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns)
{
    uint32_t sta_addr, sta_gw;

    if (wifi_sta_ip4_addr_get(&sta_addr, mask, &sta_gw, dns) != 0)
    {
#ifdef SDIOWIFI_OFFLINE
        sta_addr = sta_addr_default;
        *mask = sta_mask_default;
        sta_gw = sta_gw_default;
        *dns = sta_dns_default;
#endif
    }

    uint32_t network_addr = htonl(sta_addr);
    uint32_t network_gw = htonl(sta_gw);
    uint32_t network_mask = htonl(*mask);

    sta_addr = get_alternate_ip(network_addr, network_mask);
    sta_gw = get_alternate_ip(network_gw, network_mask);

    *addr = ntohl(sta_addr);
    *gw = ntohl(sta_gw);

    netifapi_netif_set_addr(&sdio_eth_netif, (const ip4_addr_t *)gw, (const ip4_addr_t *)mask, (const ip4_addr_t *)gw);

    printf("sdio_mgmr_sdioeth_ip4_addr_get: %x %x %x %x\n", *addr, *mask, *gw, *dns);
    
    LOCK_TCPIP_CORE();
    ip_napt_enable(*gw, 1);
    UNLOCK_TCPIP_CORE();

    return 0;
}
#endif

int sdiowifi_mgmr_start(sdiowifi_mgmr_t *sdm)
{
    if (!sdm || sdm->init) {
        return -1;
    }
    MEMSET_SAFE(sdm,  sizeof(*sdm),  0,  sizeof(*sdm));
    if ((sdm->rnm = rnm_server_init(&sdm->trcver, rnm_tx_data, sdm, ready_cb)) == NULL) {
        return -1;
    }
    bf1b_net_wifi_trcver_init(&sdm->trcver, sdm);
    tx_buf_init(sdm);

#if SDIOWIFI_TTY
    sdiowifi_tty_init(sdm, 0, SDIOWIFI_TTY_PATH);
#endif

#ifdef CFG_DUAL_ETH
    sdio_eth_init();
#endif

    sdm->init = true;

    return 0;
}

int sdiowifi_mgmr_reinit(bool init_tx_buf)
{
    (void)init_tx_buf;

    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;
    if (!sdm->init) {
        HR_LOGW("%s(%d): sdm is not initialized\r\n", __func__, __LINE__);
        return -1;
    }
    sdu_software_reset_set(true);
    notify_sdu_worker(SDU_WORKER_EV_RESET, 0);
    return 0;
}

int sdiowifi_tx_buf_init_after_lp(void)
{
    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;
    if (!sdm->init) {
        HR_LOGW("%s(%d): sdm is not initialized\r\n", __func__, __LINE__);
        return -1;
    }

    tx_buf_init(sdm);
    return 0;
}

int sdiowifi_mgmr_ps_prepare(void)
{
    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;

    bflb_net_wifi_trcver_set_present(&sdm->trcver, false);
    bl_sdio_tx_timer_stop();

#ifdef SDIOWIFI_HEARTBEAT
    rnms_heartbeat_stop(sdm->rnm);
#endif
    return 0;
}
