#include <string.h>
#include <bl60x_fw_api.h>
#include <lwip/tcpip.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>
//#include <ethernetif.h>
//#include <bl_wifi.h>
#include <utils_hex.h>
#include <semphr.h>

#include <bflb_os_private.h>
#include "bl_main.h"
#include "bl_rx.h"
#include "wifi_mgmr.h"
#include "include/wifi_mgmr_ext.h"
#include "wifi_mgmr_api.h"
#include "wifi_mgmr_profile.h"
#include "bl_mod_params.h"
#include "bl_msg_tx.h"

#include "wifi_hosal.h"
#include <wifi_hosal.h>

#include <supplicant_api.h>
#include <bl_wpa.h>

#ifdef CFG_NETBUS_WIFI_ENABLE
#include "netbus_mgmr.h"
#endif
/*just limit packet len with a reasonable value*/
#define LEN_PKT_RAW_80211   (480)

#define WIFI_SCAN_MAX_AP      (WIFI_MGMR_SCAN_ITEMS_MAX)

BL_Sem_t scan_sig = NULL;

extern int bl606a0_wifi_init(wifi_conf_t *conf);
err_t bl606a0_wifi_netif_init(struct netif *netif);
static scan_complete_cb_t scan_cb;
static void* scan_data;

static void cb_scan_complete(void *data, void *param)
{
    wifi_mgmr_ap_item_t *ap_ary_ptr = (wifi_mgmr_ap_item_t *)data;
    int status = *((int *)param);
    bflb_os_printf("scan complete status: %d, ssid_len = %lu\r\n", status, ap_ary_ptr->ssid_len);

    if (scan_sig != NULL){
        bflb_os_sem_give(scan_sig);
    }
}

static void cb_scan_item_parse(wifi_mgmr_ap_item_t *env, uint32_t *param1, wifi_mgmr_ap_item_t *item)
{
    /*recall the env*/
    uint32_t counter = (*param1);

    /*scan overflow check*/
    if (counter >= WIFI_SCAN_MAX_AP) {
        /*XXX maybe we should warning here*/
        return;
    }

    wifi_mgmr_ap_item_t *ap_ary_ptr = (wifi_mgmr_ap_item_t *)env + counter;//get and move the ptr to the next item
    counter++;

    /*copy out scan data*/
    ap_ary_ptr->channel = item->channel;
    ap_ary_ptr->rssi = item->rssi;
    memcpy(ap_ary_ptr->bssid, item->bssid, sizeof(ap_ary_ptr->bssid));
    memcpy(ap_ary_ptr->ssid, item->ssid, sizeof(ap_ary_ptr->ssid));//XXX boundary attack, copy too much data from wifi manager internal?
    ap_ary_ptr->ssid_tail[0] = '\0';
    ap_ary_ptr->ssid_len = strlen((char *)ap_ary_ptr->ssid);
    ap_ary_ptr->auth = item->auth;
    ap_ary_ptr->cipher = item->cipher;

    /*store back counter*/
    (*param1) = counter;
}

static int mac_is_unvalid(uint8_t mac[6])
{
    int i;
    uint8_t data;

    data = mac[0];
    for (i = 1; i < 6; i++) {
        if (data != mac[i]) {
            break;
        }
    }
    if (6 == i) {
        if (0 == data || 0xFF == data) {
            /*all 0 or 0xFF found*/
            return 1;
        }
    }

    return 0;
}

#ifdef CONFIG_ENABLE_IPV6_ADDR_CALLBACK
static void netif_nd6_callback(struct netif *netif, uint8_t ip_index)
{
    platform_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP6, 0);
}

void nd6_set_cb(struct netif *netif, void (*cb)(struct netif *netif, u8_t ip_index))
{
  if (netif != NULL) {
      netif->ipv6_addr_cb = cb;
  }
}
#endif

static void wifi_eth_sta_enable(struct netif *netif, uint8_t mac[6])
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

#if 0
    IP4_ADDR(&ipaddr, 192, 168, 11, 111);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 11, 1);
#else
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#endif

    memcpy(netif->hwaddr, mac, 6);//overwrite mac addressin netif
    if (mac_is_unvalid(netif->hwaddr)) {
        /* set netif MAC hardware address */
        wifi_hosal_efuse_read_mac(netif->hwaddr);
        if (mac_is_unvalid(netif->hwaddr)) {
            /*Make sure we have a default valid MAC address*/
            //TODO generate a random mac address
            netif->hwaddr[0] =  0xC0;
            netif->hwaddr[1] =  0x50;
            netif->hwaddr[2] =  0x43;
            netif->hwaddr[3] =  0xC9;
            netif->hwaddr[4] =  0x00;
            netif->hwaddr[5] =  0x01;
        }
        /*since hwaddr is change, so we back over-write mac*/
        memcpy(mac, netif->hwaddr, 6);//overwrite mac address in netif
    }

    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
    *  struct ip_addr *netmask, struct ip_addr *gw,
    *  void *state, err_t (* init)(struct netif *netif),
    *  err_t (* input)(struct pbuf *p, struct netif *netif))
    *
    *  Adds your network interface to the netif_list. Allocate a struct
    *  netif and pass a pointer to this structure as the first argument.
    *  Give pointers to cleared ip_addr structures when using DHCP,
    *  or fill them with sane numbers otherwise. The state pointer may be NULL.
    *
    * The init function pointer must point to a initialization function for
    * your ethernet netif interface. The following code illustrates it's use.*/

    netif->name[0] = 's';
    netif->name[1] = 't';
#ifdef CFG_NETBUS_WIFI_ENABLE
    g_netbus_wifi_mgmr_env.sta_netif = netif;
#endif
    netifapi_netif_add(netif, &ipaddr, &netmask, &gw, NULL, &bl606a0_wifi_netif_init, &tcpip_input);

#ifdef CONFIG_ENABLE_IPV6_ADDR_CALLBACK
    nd6_set_cb(netif, netif_nd6_callback);
#endif

    netifapi_netif_set_default(netif);
    netifapi_netif_set_up(netif);
}

int wifi_mgmr_psk_cal(char *password, char *ssid, int ssid_len, char *output)
{
    int ret;
    char psk[32];

    if (password == NULL || ssid == NULL || output == NULL) {
        return -1;
    }

    ret = pbkdf2_sha1(password, ssid, ssid_len, 4096, (uint8_t *)psk, sizeof(psk));
    if (0 == ret) {
        utils_bin2hex(output, psk, 32);
    }

    return ret;
}

int wifi_mgmr_drv_init(wifi_conf_t *conf)
{
    int ret;
    if (conf == NULL) {
        return -1;
    }

    ret = bl606a0_wifi_init(conf);
    if (ret != 0) {
        goto err;
    }
    wifi_mgmr_internel_init();
    ret = wifi_mgmr_api_ifaceup();
err:
    return ret;
}

void wifi_mgmr_get_wifi_channel_conf(wifi_conf_t *wifi_chan_conf)
{
    if (wifi_chan_conf == NULL) {
        return;
    }
    strncpy(wifi_chan_conf->country_code, wifiMgmr.country_code, sizeof(wifiMgmr.country_code));
    wifi_chan_conf->channel_nums = wifiMgmr.channel_nums;
}

wifi_interface_t wifi_mgmr_sta_enable(void)
{
    static int done = 0;

    if (1 == done) {
        bflb_os_printf("----- STA has already been enable\r\n");
        return &(wifiMgmr.wlan_sta);
    }
    done = 1;

    bflb_os_printf("---------STA enable\r\n");
    wifiMgmr.wlan_sta.mode = 0;//sta mode
    //TODO check wifiMgmr.wlan_sta status
    wifi_eth_sta_enable(&(wifiMgmr.wlan_sta.netif), wifiMgmr.wlan_sta.mac);
    return &(wifiMgmr.wlan_sta);
}

int wifi_mgmr_sta_disable(wifi_interface_t *interface)
{
    return wifi_mgmr_api_idle();
}

struct netif *wifi_mgmr_sta_netif_get(void)
{
    return &wifiMgmr.wlan_sta.netif;
}

struct netif *wifi_mgmr_ap_netif_get(void)
{
    return &wifiMgmr.wlan_ap.netif;
}

int wifi_mgmr_sta_mac_set(uint8_t mac[6])
{
    if (mac == NULL) {
        return -1;
    }
    memcpy(wifiMgmr.wlan_sta.mac, mac, 6);
    return 0;
}

int wifi_mgmr_sta_mac_get(uint8_t mac[6])
{
    if (mac == NULL) {
        return -1;
    }
    /*should we set default mac address/get efuse address here?*/
    //TODO use unified mac address init
    if (mac_is_unvalid(wifiMgmr.wlan_sta.mac)) {
        wifi_hosal_efuse_read_mac(wifiMgmr.wlan_sta.mac);
        if (mac_is_unvalid(wifiMgmr.wlan_sta.mac)) {
            /*Make sure we have a default valid MAC address*/
            //TODO generate a random mac address
            wifiMgmr.wlan_sta.mac[0] =  0x18;
            wifiMgmr.wlan_sta.mac[1] =  0xB9;
            wifiMgmr.wlan_sta.mac[2] =  0x05;
            wifiMgmr.wlan_sta.mac[3] =  0x00;
            wifiMgmr.wlan_sta.mac[4] =  0x00;
            wifiMgmr.wlan_sta.mac[5] =  0x01;
        }
    }
    memcpy(mac, wifiMgmr.wlan_sta.mac, 6);
    return 0;
}

int wifi_mgmr_sta_ip_get(uint32_t *ip, uint32_t *gw, uint32_t *mask)
{
    if (ip == NULL || gw == NULL || mask == NULL) {
        return -1;
    }

    *ip = netif_ip4_addr(&wifiMgmr.wlan_sta.netif)->addr;
    *mask = netif_ip4_netmask(&wifiMgmr.wlan_sta.netif)->addr;
    *gw = netif_ip4_gw(&wifiMgmr.wlan_sta.netif)->addr;

    return 0;
}

int wifi_mgmr_sta_dns_get(uint32_t *dns1, uint32_t *dns2)
{
    const ip_addr_t* dns;

    if (dns1 == NULL || dns2 == NULL) {
        return -1;
    }

    if (!netif_is_link_up(&wifiMgmr.wlan_sta.netif)) {
        ip_addr_set_any(0, (ip_addr_t*)dns1);
        ip_addr_set_any(0, (ip_addr_t*)dns2);
        return 0;
    }

    /*Get DNS1*/
    dns = dns_getserver(0);
    *dns1 = ip_addr_get_ip4_u32(dns);

    /*Get DNS2*/
    if (DNS_MAX_SERVERS > 1) {
        dns = dns_getserver(1);
        *dns2 = ip_addr_get_ip4_u32(dns);
    } else {
        *dns2 = 0;
    }

    return 0;
}

int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns1, uint32_t dns2)
{
    bflb_os_enter_critical();

    wifiMgmr.wlan_sta.ipv4.ip = ip;
    wifiMgmr.wlan_sta.ipv4.mask = mask;
    wifiMgmr.wlan_sta.ipv4.gw = gw;
    wifiMgmr.wlan_sta.ipv4.dns1 = dns1;
    wifiMgmr.wlan_sta.ipv4.dns2 = dns2;

    bflb_os_exit_critical();

    return wifi_mgmr_api_ip_update();
}

int wifi_mgmr_sta_ip_unset(void)
{
    return wifi_mgmr_sta_ip_set(0, 0, 0, 0, 0);
}

int wifi_mgmr_sta_connect_ext(wifi_interface_t *wifi_interface, char *ssid, char *passphr, const ap_connect_adv_t *conn_adv_param)
{
    int ssid_len = ssid ? strlen(ssid) : 0;
    int passphr_len = passphr ? strlen(passphr) : 0;

    if (conn_adv_param == NULL) {
        return -1;
    }
    int psk_len = conn_adv_param->psk ? strlen(conn_adv_param->psk) : 0;

    if (!ssid || ssid_len > 32) {
        return -1;
    }

    if ((passphr && ((passphr_len < 8 && passphr_len != 5) || passphr_len > 63)) ||
        (conn_adv_param->psk && (psk_len != 64)))
    {
        return -1;
    }

    wifi_mgmr_sta_ssid_set(ssid);
    wifi_mgmr_sta_passphr_set(passphr);

    return wifi_mgmr_api_connect(ssid, passphr, conn_adv_param);
}

int wifi_mgmr_sta_connect_mid(wifi_interface_t *wifi_interface, char *ssid, char *passphrase, char *psk, uint8_t *mac, uint8_t band, uint8_t chan_id, uint8_t use_dhcp, uint32_t flags)
{
    struct ap_connect_adv ext_param;

    ext_param.psk = psk;
    ext_param.ap_info.type = AP_INFO_TYPE_SUGGEST;
    ext_param.ap_info.time_to_live = 5;
    ext_param.ap_info.bssid = mac;
    ext_param.ap_info.band = 0;
    if (0 == chan_id || (chan_id > bl_msg_get_channel_nums())) {
        ext_param.ap_info.freq = 0;
    } else {
        ext_param.ap_info.freq = phy_channel_to_freq(ext_param.ap_info.band, chan_id);
    }
    ext_param.ap_info.use_dhcp = use_dhcp;
    ext_param.flags = (flags & WIFI_CONNECT_DEFAULT) ? (flags | WIFI_CONNECT_PMF_CAPABLE) : (flags);
    return wifi_mgmr_sta_connect_ext(wifi_interface, ssid, passphrase, &ext_param);
}

int wifi_mgmr_sta_connect(wifi_interface_t *wifi_interface, char *ssid, char *passphrase, char *psk, uint8_t *mac, uint8_t band, uint8_t chan_id)
{
    return wifi_mgmr_sta_connect_mid(wifi_interface, ssid, passphrase, psk, mac, band, chan_id, 1, WIFI_CONNECT_DEFAULT);
}

int wifi_mgmr_sta_disconnect(void)
{
    return wifi_mgmr_api_disconnect();
}

int wifi_sta_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns)
{
    struct netif *ni = &wifiMgmr.wlan_sta.netif;

    if (addr) {
        *addr = netif_ip4_addr(ni)->addr;
    }
    if (mask) {
        *mask = netif_ip4_netmask(ni)->addr;
    }
    if (gw) {
        *gw = netif_ip4_gw(ni)->addr;
    }
    if (dns) {
        const ip_addr_t *ip;
        ip = dns_getserver(0);
        *dns = ip_addr_get_ip4_u32(ip);
    }
    return 0;
}

int wifi_mgmr_sta_pm_ops(void *arg)
{
    int mode;

    mode = (int )arg;
    return wifi_mgmr_api_fw_powersaving(mode);
}

int wifi_mgmr_pm_ops_register(void)
{
    int ret;
    ret = wifi_hosal_pm_event_register(WLAN_PM_EVENT_CONTROL, WLAN_CODE_PM_NOTIFY_START, NODE_CAP_BIT_UAPSD_MODE, 8, wifi_mgmr_sta_pm_ops, (void *)PS_MODE_ON_DYN, PM_ENABLE);
    if (ret != 0) {
        goto err;
    }
    ret = wifi_hosal_pm_event_register(WLAN_PM_EVENT_CONTROL, WLAN_CODE_PM_NOTIFY_STOP, NODE_CAP_BIT_UAPSD_MODE, 6, wifi_mgmr_sta_pm_ops, (void *)PS_MODE_OFF, PM_ENABLE);
err:
    return ret;
}

int wifi_mgmr_fw_affair_ops(void)
{
    int ret;
    ret = wifi_hosal_pm_event_switch(WLAN_PM_EVENT_BEACON_LOSS, WLAN_CODE_BEACON_LOSS, PM_ENABLE);
    if (ret != 0) {
        goto err;
    }

    ret = wifi_hosal_pm_event_switch(WLAN_PM_EVENT_SEND_NULLDATA, WLAN_CODE_PM_NULLDATA_NOACK, PM_ENABLE);
    if (ret != 0) {
        goto err;
    }
    ret = wifi_hosal_pm_event_switch(WLAN_PM_EVENT_EXIT_SLEEP, WLAN_CODE_PM_ENTER_SLEEP_PRE_FAILED, PM_ENABLE);
err:
    return ret;
}

int wifi_mgmr_sta_ps_enter(uint32_t ps_level)
{
    int retval = 0;

    if (!wifi_hosal_pm_capacity_set(ps_level)) {
        retval = wifi_hosal_pm_post_event(WLAN_PM_EVENT_CONTROL, WLAN_CODE_PM_NOTIFY_START, NULL);
    } else {
        bflb_os_printf("----- Error PS Mode\r\n");
    }

    return retval;
}

int wifi_mgmr_sta_ps_exit(void)
{
    return wifi_hosal_pm_post_event(WLAN_PM_EVENT_CONTROL, WLAN_CODE_PM_NOTIFY_STOP, NULL);
}

int wifi_mgmr_sta_ps_get(void)
{
    return wifi_mgmr_api_fw_powersaving_get();
}

int wifi_mgmr_sta_autoconnect_enable(void)
{
    return wifi_mgmr_api_enable_autoreconnect();
}

int wifi_mgmr_sta_autoconnect_disable(void)
{
    return wifi_mgmr_api_disable_autoreconnect();
}

void wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_sta_connect_ind_stat_info_t *wifi_mgmr_ind_stat)
{
    int ssid_len = strlen(wifiMgmr.wifi_mgmr_stat_info.ssid);

    if (wifi_mgmr_ind_stat == NULL) {
        return;
    }
    if (ssid_len > 0) {
        memcpy(wifi_mgmr_ind_stat->ssid, wifiMgmr.wifi_mgmr_stat_info.ssid, ssid_len);
        wifi_mgmr_ind_stat->ssid[ssid_len] = '\0';
    }

    int passphr_len = strlen(wifiMgmr.wifi_mgmr_stat_info.passphr);
    if (passphr_len > 0) {
        memcpy(wifi_mgmr_ind_stat->passphr, wifiMgmr.wifi_mgmr_stat_info.passphr, passphr_len);
        wifi_mgmr_ind_stat->passphr[passphr_len] = '\0';
    }

    memcpy(wifi_mgmr_ind_stat->bssid, wifiMgmr.wifi_mgmr_stat_info.bssid, 6);

    wifi_mgmr_ind_stat->status_code = wifiMgmr.wifi_mgmr_stat_info.status_code;
    wifi_mgmr_ind_stat->chan_band = wifiMgmr.wifi_mgmr_stat_info.chan_band;
    wifi_mgmr_ind_stat->chan_id = phy_freq_to_channel(wifiMgmr.wifi_mgmr_stat_info.chan_band, wifiMgmr.wifi_mgmr_stat_info.chan_freq);
    wifi_mgmr_ind_stat->type_ind = wifiMgmr.wifi_mgmr_stat_info.type_ind;

    bflb_os_printf("wifi mgmr ind status code = %d\r\n",  wifi_mgmr_ind_stat->status_code);
    bflb_os_printf("ssid: %s, passphr: %s, band: %d, chan_id: %d, type_ind: %d\r\n",
            wifi_mgmr_ind_stat->ssid,
            wifi_mgmr_ind_stat->passphr,
            wifi_mgmr_ind_stat->chan_band,
            wifi_mgmr_ind_stat->chan_id,
            wifi_mgmr_ind_stat->type_ind);
    bflb_os_printf("bssid: %02x%02x%02x%02x%02x%02x\r\n",
                wifi_mgmr_ind_stat->bssid[0],
                wifi_mgmr_ind_stat->bssid[1],
                wifi_mgmr_ind_stat->bssid[2],
                wifi_mgmr_ind_stat->bssid[3],
                wifi_mgmr_ind_stat->bssid[4],
                wifi_mgmr_ind_stat->bssid[5]
    );
}

void wifi_mgmr_sta_ssid_set(char *ssid)
{
  int len = strlen(ssid);

  if (len > 0) {
    len = len > (sizeof(wifiMgmr.wifi_mgmr_stat_info.ssid) - 1) ? sizeof(wifiMgmr.wifi_mgmr_stat_info.ssid) - 1 : len;
    memcpy(wifiMgmr.wifi_mgmr_stat_info.ssid, ssid, len);
    wifiMgmr.wifi_mgmr_stat_info.ssid[len] = 0;
  }
}

void wifi_mgmr_sta_passphr_set(char *passphr)
{
  int len = 0;
  if (passphr) {
     len = strlen(passphr);
  }

  if (len > 0) {
    len = len > (sizeof(wifiMgmr.wifi_mgmr_stat_info.passphr) - 1) ? sizeof(wifiMgmr.wifi_mgmr_stat_info.passphr) - 1 : len;
    memcpy(wifiMgmr.wifi_mgmr_stat_info.passphr, passphr, len);
  }
  wifiMgmr.wifi_mgmr_stat_info.passphr[len] = 0;
}

void wifi_mgmr_sta_psk_set(char *psk) __attribute__ ((alias ("wifi_mgmr_sta_passphr_set")));

static void wifi_eth_ap_enable(struct netif *netif, uint8_t mac[6])
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

#if 1
    IP4_ADDR(&ipaddr, 192, 168, 11, 1);//XXX address will be configured again by dhcpd
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);
#else
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#endif

    memcpy(netif->hwaddr, mac, 6);//overwrite mac addressin netif
    if (mac_is_unvalid(netif->hwaddr)) {
        /* set netif MAC hardware address */
        wifi_hosal_efuse_read_mac(netif->hwaddr);
        if (mac_is_unvalid(netif->hwaddr)) {
            /*Make sure we have a default valid MAC address*/
            //TODO generate a random mac address
            netif->hwaddr[0] =  0xC0;
            netif->hwaddr[1] =  0x50;
            netif->hwaddr[2] =  0x43;
            netif->hwaddr[3] =  0xC9;
            netif->hwaddr[4] =  0x00;
            netif->hwaddr[5] =  0x01;
        }
        /*since hwaddr is change, so we back over-write mac*/
        memcpy(mac, netif->hwaddr, 6);//overwrite mac address in netif
    }

#ifdef CFG_NETBUS_WIFI_ENABLE
    g_netbus_wifi_mgmr_env.ap_netif = netif;
#endif
    netif->name[0] = 'a';
    netif->name[1] = 'p';

    netifapi_netif_add(netif, &ipaddr, &netmask, &gw, NULL, &bl606a0_wifi_netif_init, &tcpip_input);
    netifapi_netif_set_up(netif);
}

wifi_interface_t wifi_mgmr_ap_enable()
{
    static int done = 0;                                                                                                             
                                                                                                                                    
    if (1 == done) {                                                                                                                 
        bflb_os_printf("----- AP has already been enable\r\n");                                                                        
        return &(wifiMgmr.wlan_ap);                                                                                                  
    }
    done = 1;                                                                                                                        
                                                                                                                                    
    bflb_os_printf("---------AP enable\r\n");                                                                                          
                                                                                                                                    
    wifiMgmr.wlan_ap.mode = 1;//ap mode                                                                                              
    wifi_eth_ap_enable(&(wifiMgmr.wlan_ap.netif), wifiMgmr.wlan_ap.mac);

    return &(wifiMgmr.wlan_ap);
}

int wifi_mgmr_ap_mac_set(uint8_t mac[6])
{
    if (mac == NULL) {
        return -1;
    }
    memcpy(wifiMgmr.wlan_ap.mac, mac, 6);
    return 0;
}

int wifi_mgmr_ap_mac_get(uint8_t mac[6])
{
    if (mac == NULL) {
        return -1;
    }

    /*should we set default mac address/get efuse address here?*/
    //TODO use unified mac address init
    mac_is_unvalid(wifiMgmr.wlan_ap.mac);
    wifi_hosal_efuse_read_mac(wifiMgmr.wlan_ap.mac);
    if (mac_is_unvalid(wifiMgmr.wlan_ap.mac)) {
        /*Make sure we have a default valid MAC address*/
        //TODO generate a random mac address
        wifiMgmr.wlan_ap.mac[0] =  0xC0;
        wifiMgmr.wlan_ap.mac[1] =  0x50;
        wifiMgmr.wlan_ap.mac[2] =  0x43;
        wifiMgmr.wlan_ap.mac[3] =  0xC9;
        wifiMgmr.wlan_ap.mac[4] =  0x00;
        wifiMgmr.wlan_ap.mac[5] =  0x01;
    }
    memcpy(mac, wifiMgmr.wlan_ap.mac, 6);
    return 0;
}

int wifi_mgmr_ap_ip_get(uint32_t *ip, uint32_t *gw, uint32_t *mask)
{
    if (ip == NULL || gw == NULL || mask == NULL) {
        return -1;
    }

    *ip = netif_ip4_addr(&wifiMgmr.wlan_ap.netif)->addr;
    *mask = netif_ip4_netmask(&wifiMgmr.wlan_ap.netif)->addr;
    *gw = netif_ip4_gw(&wifiMgmr.wlan_ap.netif)->addr;

    return 0;
}

//TODO this API is still NOT completed, more features need to be implemented
int wifi_mgmr_ap_start(wifi_interface_t *interface, char *ssid, int hidden_ssid, char *passwd, int channel)
{
    return wifi_mgmr_api_ap_start(ssid, passwd, channel, hidden_ssid, -1, 1);
}

int wifi_mgmr_ap_chan_switch(wifi_interface_t *interface, int channel, uint8_t cs_count)
{
    return wifi_mgmr_api_chan_switch(channel, cs_count);
}

int wifi_mgmr_ap_start_adv(wifi_interface_t *interface, char *ssid, int hidden_ssid, char *passwd, int channel, uint8_t use_dhcp)
{
    return wifi_mgmr_api_ap_start(ssid, passwd, channel, hidden_ssid, -1, use_dhcp);
}

int wifi_mgmr_ap_start_atcmd(wifi_interface_t *interface, char *ssid, int hidden_ssid, char *passwd, int channel, int max_sta_supported)
{
    return wifi_mgmr_api_ap_start(ssid, passwd, channel, hidden_ssid, max_sta_supported, 1);
}

int wifi_mgmr_ap_stop(wifi_interface_t *interface)
{
    return wifi_mgmr_api_ap_stop();
}

int wifi_mgmr_ap_sta_cnt_get(uint8_t *sta_cnt)
{
    if (sta_cnt == NULL) {
        return -1;
    }
    return wifi_mgmr_ap_sta_cnt_get_internal(sta_cnt);
}

int wifi_mgmr_ap_sta_info_get(struct wifi_sta_basic_info *sta_info, uint8_t idx)
{
    if (sta_info == NULL) {
        return -1;
    }

    struct wifi_mgmr_sta_basic_info sta_info_internal;
    memset(&sta_info_internal, 0, sizeof(struct wifi_mgmr_sta_basic_info));
    wifi_mgmr_ap_sta_info_get_internal(&sta_info_internal, idx);
    sta_info->sta_idx = sta_info_internal.sta_idx;
    sta_info->is_used = sta_info_internal.is_used;
    sta_info->rssi = sta_info_internal.rssi;
    sta_info->tsflo = sta_info_internal.tsflo;
    sta_info->tsfhi = sta_info_internal.tsfhi;
    sta_info->data_rate = sta_info_internal.data_rate;
    memcpy(sta_info->sta_mac, sta_info_internal.sta_mac, 6);
    return 0;
}

int wifi_mgmr_ap_sta_delete(uint8_t sta_idx)
{
    return wifi_mgmr_ap_sta_delete_internal(sta_idx);
}

int wifi_mgmr_ap_set_gateway(char *gateway)
{
    //memcpy(wifiMgmr.wlan_ap.netif.gateway, gateway, strlen(gateway));
    return 0;
}

int wifi_mgmr_sniffer_register(void *env, sniffer_cb_t cb)
{
    return bl_rx_pkt_cb_register(env, cb);
}

int wifi_mgmr_sniffer_unregister(void *env)
{
    return bl_rx_pkt_cb_unregister(env);
}

int wifi_mgmr_sniffer_register_adv(void *env, sniffer_cb_adv_t cb)
{
    return bl_rx_pkt_adv_cb_register(env, cb);
}

int wifi_mgmr_sniffer_unregister_adv(void *env)
{
    return bl_rx_pkt_adv_cb_unregister(env);
}

int wifi_mgmr_sniffer_enable()
{
    return wifi_mgmr_api_sniffer_enable();
}

int wifi_mgmr_sniffer_disable()
{
    return wifi_mgmr_api_idle();
}

int wifi_mgmr_rate_config(uint16_t config)
{
    return wifi_mgmr_api_rate_config(config);
}

int wifi_mgmr_conf_max_sta(uint8_t max_sta_supported)
{
    return wifi_mgmr_api_conf_max_sta(max_sta_supported);
}

int wifi_mgmr_state_get(int *state)
{
    if (state == NULL) {
        return -1;
    }

    return wifi_mgmr_state_get_internal(state);
}

int wifi_mgmr_detailed_state_get(int *state, int *state_detailed)
{
    if (state == NULL || state_detailed == NULL) {
        return -1;
    }
    return wifi_mgmr_detailed_state_get_internal(state, state_detailed);
}

int wifi_mgmr_status_code_get(int *s_code)
{
    if (s_code == NULL) {
        return -1;
    }
    return wifi_mgmr_status_code_get_internal(s_code);
}

int wifi_mgmr_rssi_get(int *rssi)
{
    if (rssi == NULL) {
        return -1;
    }
    *rssi = wifiMgmr.wlan_sta.sta.rssi;
    return 0;
}

int wifi_mgmr_channel_get(int *channel)
{
    if (channel == NULL) {
        return -1;
    }
    *channel = wifiMgmr.channel;
    return 0;
}

int wifi_mgmr_channel_set(int channel, int use_40Mhz)
{
    int ret;
    wifiMgmr.channel = channel;
    ret = wifi_mgmr_api_channel_set(channel, use_40Mhz);
    bflb_os_printf("set channel %d, 40Mhz %d\r\n", channel, use_40Mhz);
    return ret;
}

int wifi_mgmr_raw_80211_send(uint8_t *pkt, int len)
{
    if (len > LEN_PKT_RAW_80211) {
        /*raw packet len is too long*/
        return -1;
    }
    return wifi_mgmr_api_raw_send(pkt, len);
}

int wifi_mgmr_all_ap_scan(wifi_mgmr_ap_item_t **ap_ary, uint32_t *num)
{
    uint32_t counter = 0;
    wifi_mgmr_ap_item_t *ap_ary_p;

    if (NULL == ap_ary || NULL == num) {
        return -1;
    }

    ap_ary_p = (wifi_mgmr_ap_item_t *)bflb_os_malloc(sizeof(wifi_mgmr_ap_item_t) * WIFI_SCAN_MAX_AP);
    if(NULL == ap_ary_p) {
        return -1;
    }
    memset(ap_ary_p, 0, sizeof(wifi_mgmr_ap_item_t) * WIFI_SCAN_MAX_AP);

    if(NULL == (scan_sig = bflb_os_sem_create(0))){
        bflb_os_free(ap_ary_p);
        return -1;
    }

    wifi_mgmr_scan(ap_ary_p, cb_scan_complete);//trigger the scan

    if ((0 == bflb_os_sem_take(scan_sig, BL_OS_WAITING_FOREVER))) {
        bflb_os_printf("wifi scan Done\r\n");
    }

    wifi_mgmr_cli_scanlist();
    /*Start to scan the AP*/
    wifi_mgmr_scan_ap_all(ap_ary_p, &counter, cb_scan_item_parse);

    *num = counter;
    *ap_ary = ap_ary_p;

    bflb_os_sem_delete(scan_sig);

    return 0;
}

int wifi_mgmr_scan(void *data, scan_complete_cb_t cb)
{
    wifi_mgmr_scan_params_t *scan_params = NULL;

    scan_params = (wifi_mgmr_scan_params_t *)bflb_os_zalloc(sizeof(wifi_mgmr_scan_params_t));
    if (!scan_params) {
        bflb_os_printf("%s malloc scan_params failed!\r\n", __FUNCTION__);
        return -1;
    }

    scan_cb = cb;
    scan_data = data;

    scan_params->channel_num = 0;
    memcpy(scan_params->bssid, (uint8_t *)&mac_addr_bcst, sizeof(struct mac_addr));
    scan_params->ssid.length = 0;
    scan_params->scan_mode = SCAN_ACTIVE;
    /*if 0, use default scan time in fw,
     * unit:us*/
    scan_params->duration_scan = 0;

    return wifi_mgmr_api_fw_scan(scan_params);
}

int wifi_mgmr_scan_adv(void *data, scan_complete_cb_t cb, uint16_t *channels, uint16_t channel_num, const uint8_t bssid[6], const char *ssid, uint8_t scan_mode, uint32_t duration_scan)
{
    wifi_mgmr_scan_params_t* scan_params = NULL;

    if (0 != channel_num && NULL == channels) {
        bflb_os_printf("%s channels is NULL while has channel_num!\r\n", __FUNCTION__);
        return -1;
    }

    if (channel_num > MAX_FIXED_CHANNELS_LIMIT) {
        bflb_os_printf("%s exceed max channel number!\r\n", __FUNCTION__);
        return -1;
    }

    scan_params = (wifi_mgmr_scan_params_t *)bflb_os_zalloc(sizeof(wifi_mgmr_scan_params_t) + sizeof(uint16_t)*channel_num);
    if (!scan_params) {
        bflb_os_printf("%s malloc scan_params failed!\r\n", __FUNCTION__);
        return -1;
    }

    scan_cb = cb;
    scan_data = data;

    scan_params->scan_mode = scan_mode;
    scan_params->duration_scan = duration_scan;

    if (bssid) {
        memcpy(scan_params->bssid, bssid, ETH_ALEN);
    } else {
        memset(scan_params->bssid, 0xff, ETH_ALEN);
    }

    scan_params->channel_num = channel_num;
    if (channel_num) {
        memcpy(scan_params->channels, channels, sizeof(uint16_t) * channel_num);
    }

    if (ssid != NULL) {
        scan_params->ssid.length = strlen(ssid);
        scan_params->ssid.length = (scan_params->ssid.length > MAC_SSID_LEN) ? MAC_SSID_LEN : scan_params->ssid.length;
        memcpy(scan_params->ssid.array, ssid, scan_params->ssid.length);
        scan_params->ssid.array_tail[0] = '\0';
    } else {
        scan_params->ssid.length = 0;
    }

    return wifi_mgmr_api_fw_scan(scan_params);
}

int wifi_mgmr_cfg_req(uint32_t ops, uint32_t task, uint32_t element, uint32_t type, uint32_t length, uint32_t *buf)
{
    return wifi_mgmr_api_cfg_req(ops, task, element, type, length, buf);
}

int wifi_mgmr_beacon_interval_set(uint16_t beacon_int)
{
    wifiMgmr.ap_bcn_int = beacon_int;
    return 0;
}

int wifi_mgmr_scan_filter_hidden_ssid(int filter)
{
    bflb_os_enter_critical();
    if (filter) {
        wifiMgmr.features &= (~WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID);
    } else {
        wifiMgmr.features |= WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID;
    }
    bflb_os_exit_critical();
    return 0;
}

int wifi_mgmr_scan_complete_callback()
{
    int status = 0;

    bflb_os_printf("%s: scan complete\r\n", __func__);
    if (scan_cb != NULL){
        status = 1;
        scan_cb(scan_data, &status);
    }
    return 0;
}

int wifi_mgmr_scan_ap(char *ssid, wifi_mgmr_ap_item_t *item)
{
    int i, index = -1;
    int8_t rssi = -127;//FIXME magic here?
    wifi_mgmr_scan_item_t *scan;

    if (NULL == ssid || NULL == item) {
        return -1;
    }

    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        if (wifiMgmr.scan_items[i].is_used &&
                (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &(wifiMgmr.scan_items[i]))) && 0 == strcmp(wifiMgmr.scan_items[i].ssid, ssid)) {
            /*found the ssid*/
            index = i;
            if (wifiMgmr.scan_items[i].rssi > rssi) {
                /*found ap with better rssi*/
                rssi = wifiMgmr.scan_items[i].rssi;
            }
            break;
        }
    }
    if ((sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]) != i) && (index >= 0)) {
        /*proper item found, copy back data now*/
        //TODO FIXME we should scan the ap from the cache within the Wi-Fi manager thread
        scan = &wifiMgmr.scan_items[index];
        memcpy(item->ssid, scan->ssid, sizeof(item->ssid));
        item->ssid_tail[0] = '\0';
        item->ssid_len = strlen(item->ssid);
        memcpy(item->bssid, scan->bssid, sizeof(item->bssid));
        item->channel = scan->channel;
        item->rssi = scan->rssi;
    } else {
        return -1;
    }

    return 0;
}

uint32_t wifi_mgmr_sta_scanlist_nums_get()
{
    uint32_t i, cnt = 0;
    wifi_mgmr_scan_item_t *scan;

    for (i = 0; i < sizeof(wifiMgmr.scan_items) / sizeof(wifiMgmr.scan_items[0]); i++) {
        scan = &wifiMgmr.scan_items[i];
        if (scan->is_used && (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &(wifiMgmr.scan_items[i])))) {
            cnt++;
        }
    }

    return cnt;
}

int wifi_mgmr_scan_ap_all(wifi_mgmr_ap_item_t *env, uint32_t *param1, scan_item_cb_t cb)
{
    int i;
    wifi_mgmr_scan_item_t *scan;
    wifi_mgmr_ap_item_t item;

    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        scan = &wifiMgmr.scan_items[i];
        if (scan->is_used && (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &(wifiMgmr.scan_items[i])))) {
            /*convert internal scan results to ext_scan_result*/
            memcpy(item.ssid, scan->ssid, sizeof(item.ssid));
            item.ssid_tail[0] = '\0';
            item.ssid_len = strlen(item.ssid);
            memcpy(item.bssid, scan->bssid, sizeof(item.bssid));
            item.channel = scan->channel;
            item.rssi = scan->rssi;
            item.auth = scan->auth;
            item.cipher = scan->cipher;
            cb(env, param1, &item);
        }
    }

    return 0;
}

int wifi_mgmr_set_country_code(char *country_code)
{
    if (NULL == country_code) {
        return -1;
    }
    bflb_os_printf("%s:code = %s\r\n", __func__, country_code);
    return wifi_mgmr_api_set_country_code(country_code);
}

int wifi_mgmr_set_wifi_active_time(uint32_t ms)
{
    if (ms < 10 || ms > 80) {
        bflb_os_printf("Wrong, MUST set wifi ps acitve [10, 80] ms\r\n");
        return -1;
    }

    void td_set_tim_time(uint8_t vif_index, uint32_t us);
    extern struct bl_hw wifi_hw;
    td_set_tim_time(wifi_hw.vif_table[BL_VIF_STA].vif_idx, ms * 1000);

    return 0;
}

int wifi_mgmr_set_listen_interval(uint16_t itv)
{
    if (itv < 1) {
        return -1;
    }

    bl_mod_params.listen_itv = itv;

    return 0;
}

const char* wifi_mgmr_status_code_str(uint16_t status_code)
{
    return wifi_mgmr_get_sm_status_code_str(status_code);
}

void wifi_mgmr_conn_result_get(uint16_t *status_code, uint16_t *reason_code)
{
    if (status_code == NULL || reason_code == NULL) {
        return;
	}
    (*status_code) = wifiMgmr.wifi_mgmr_stat_info.status_code;
    (*reason_code) = wifiMgmr.wifi_mgmr_stat_info.reason_code;
}

void wifi_mgmr_diagnose_tlv_free(struct sm_tlv_list* list)
{
    /* Cant use co_list, free it on our own */
    if (!list || !list->first)
        return;

    struct sm_tlv_list_hdr *next = list->first;
    struct sm_tlv_list_hdr *current = NULL;
    while (next) {
        current = next;
        next = next->next;
        /* XXX NO container_of here and next is the head, so just free! */
        bflb_os_free(current);
    }
}

struct sm_connect_tlv_desc* wifi_mgmr_diagnose_tlv_get_ele(void)
{
    static struct sm_tlv_list list = {NULL, NULL};
    static struct sm_tlv_list_hdr *next = NULL;
    struct sm_tlv_list_hdr *current = NULL;

    bflb_os_mutex_lock(wifiMgmr.wifi_mgmr_stat_info.diagnose_get_lock);
    if (!list.first)
    {
        bflb_os_mutex_lock(wifiMgmr.wifi_mgmr_stat_info.diagnose_lock);
        list = wifiMgmr.wifi_mgmr_stat_info.connect_diagnose;
        wifiMgmr.wifi_mgmr_stat_info.connect_diagnose.first = NULL;
        wifiMgmr.wifi_mgmr_stat_info.connect_diagnose.last = NULL;
        bflb_os_mutex_unlock(wifiMgmr.wifi_mgmr_stat_info.diagnose_lock);

        current = list.first;
        next = (list.first) ? list.first->next : NULL;

        bflb_os_mutex_unlock(wifiMgmr.wifi_mgmr_stat_info.diagnose_get_lock);
        return (struct sm_connect_tlv_desc *)(current);
    }

    /* Get current */
    current = next;
    if (next)
    {
        next = next->next;
    }
    /* Reset list when we arrive the end */
    if (!current)
    {    
        list.first = NULL;
        list.last = NULL;
    }

    bflb_os_mutex_unlock(wifiMgmr.wifi_mgmr_stat_info.diagnose_get_lock);
    return (struct sm_connect_tlv_desc *)(current);
}

void wifi_mgmr_diagnose_tlv_free_ele(struct sm_connect_tlv_desc *ele)
{
    if (!ele)
    {
        return;
    }

    bflb_os_free(ele);
}
