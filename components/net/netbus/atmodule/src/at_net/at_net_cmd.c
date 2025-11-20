/**
  ******************************************************************************
  * @file    at_net_cmd.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
//#include <aos/kernel.h>
//#include <wifi_mgmr_ext.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>
//#include <ping.h>
#include <at_net_ping.h>
#include <time.h>
//#include <utils_time.h>
#include <sys/stat.h>

#include "at_main.h"
#include "at_core.h"
#include "at_base_config.h"
#include "at_wifi_config.h"
#include "at_net_main.h"
#include "at_net_config.h"
#include "at_through.h"
#include "at_port.h"
#include "assert.h"
#include "wifi_mgmr_ext.h"
#if AT_TRANS_ZEROCOPY
#include "nxspi.h"
#endif
#define AT_NET_CMD_PRINTF printf

#define AT_THROUGHPUT_NOWIFI (0)
#define AT_NET_CIPSTART_TIMEOUT_DEFAULT_MS (10*1000)

#if (!AT_TRANS_ZEROCOPY)
static uint8_t at_net_tx_buffer[AT_NET_TX_MAX_LEN];
#endif
//static uint8_t at_net_tx_buffer[AT_NET_TX_MAX_LEN];

/*static int string_is_valid_ip(char *string)
{
    uint32_t ipaddr;

    ipaddr = ipaddr_addr(string);
    if (ipaddr == IPADDR_NONE || ipaddr == IPADDR_ANY) {
        return 0;
    }

    return 1;
}*/

static int file_exists(const char *path) {
    struct stat st;
    return (path && path[0] && at_fs_stat(path, &st) == 0);
}

static int at_exe_cmd_cifsr(int argc, const char **argv)
{
    ip4_addr_t sta_addr = {0}, sta_gw = {0}, sta_mask = {0}, dns = {0};
    ip4_addr_t ap_addr;

    if (at_wifi_config->dhcp_state.bit.sta_dhcp == 0) {
        sta_addr.addr = at_wifi_config->sta_ip.ip;
    } else {
        at_wifi_sta_ip4_addr_get(&sta_addr.addr, &sta_gw.addr, &sta_mask.addr, &dns.addr);
    }

    ap_addr.addr = at_wifi_config->ap_ip.ip;

    if (at_wifi_config->wifi_mode == WIFI_STATION_MODE || at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
        at_response_string("+CIFSR:%s,\"%s\"\r\n", "STAIP", ip4addr_ntoa(&sta_addr));
#if LWIP_IPV6
    if (at_net_config->ipv6_enable) {
        struct netif *nif = (struct netif *)at_wifi_netif_get(AT_WIFI_VIF_STA);

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            const ip6_addr_t * ip6addr = netif_ip6_addr(nif, i);
            if (ip6_addr_isvalid(netif_ip6_addr_state(nif, i)) && ip6_addr_islinklocal(ip6addr)) {
                at_response_string("+CIFSR:%s:\"%s\"\r\n", "STAIP6LL", ipaddr_ntoa(ip6addr));
            }
            if (ip6_addr_isvalid(netif_ip6_addr_state(nif, i)) && ip6_addr_isglobal(ip6addr)) {
                at_response_string("+CIFSR:%s:\"%s\"\r\n", "STAIP6GL", ipaddr_ntoa(ip6addr));
            }
        }
    }
#endif
        at_response_string("+CIFSR:%s,\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n", "STAMAC",
            at_wifi_config->sta_mac.addr[0],
            at_wifi_config->sta_mac.addr[1],
            at_wifi_config->sta_mac.addr[2],
            at_wifi_config->sta_mac.addr[3],
            at_wifi_config->sta_mac.addr[4],
            at_wifi_config->sta_mac.addr[5]);
    }

    if (at_wifi_config->wifi_mode == WIFI_SOFTAP_MODE || at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
        at_response_string("+CIFSR:%s,\"%s\"\r\n", "APIP", ip4addr_ntoa(&ap_addr));
#if LWIP_IPV6
    if (at_net_config->ipv6_enable) {
        struct netif *nif = (struct netif *)at_wifi_netif_get(AT_WIFI_VIF_AP);

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            const ip6_addr_t * ip6addr = netif_ip6_addr(nif, i);
            if (ip6_addr_isvalid(netif_ip6_addr_state(nif, i)) && ip6_addr_islinklocal(ip6addr)) {
                at_response_string("+CIFSR:%s:\"%s\"\r\n", "APIP6LL", ipaddr_ntoa(ip6addr));
            }
        }
    }
#endif
        at_response_string("+CIFSR:%s,\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n", "APMAC",
            at_wifi_config->ap_mac.addr[0],
            at_wifi_config->ap_mac.addr[1],
            at_wifi_config->ap_mac.addr[2],
            at_wifi_config->ap_mac.addr[3],
            at_wifi_config->ap_mac.addr[4],
            at_wifi_config->ap_mac.addr[5]);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipv6(int argc, const char **argv)
{
    at_response_string("+CIPV6:%d\r\n", at_net_config->ipv6_enable);

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipv6(int argc, const char **argv)
{
    int ipv6 = 0;

    AT_CMD_PARSE_NUMBER(0, &ipv6);

    if (ipv6 != 0 && ipv6 != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    at_net_config->ipv6_enable = ipv6;
#if LWIP_IPV6
    wifi_sta_ipv6_enable(ipv6);
#endif

    if (at->store) {
        at_net_config_save(AT_CONFIG_KEY_NET_IPV6_ENABLE);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipdns(int argc, const char **argv)
{
    ip_addr_t *dns1, *dns2, *dns3;
    char dns_str1[16] = {0};
    char dns_str2[16] = {0};
    char dns_str3[16] = {0};

    dns1 = dns_getserver(0);
    dns2 = dns_getserver(1);
    dns3 = dns_getserver(2);

    strlcpy(dns_str1, ipaddr_ntoa((ip_addr_t *)dns1), sizeof(dns_str1));
    strlcpy(dns_str2, ipaddr_ntoa((ip_addr_t *)dns2), sizeof(dns_str2));
    strlcpy(dns_str3, ipaddr_ntoa((ip_addr_t *)dns3), sizeof(dns_str3));
    at_response_string("+CIPDNS:%d,\"%s\",\"%s\",\"%s\"\r\n", at_net_config->dns.dns_isset, dns_str1, dns_str2, dns_str3);

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipdns(int argc, const char **argv)
{
    char dns_str1[16] = {0};
    char dns_str2[16] = {0};
    char dns_str3[16] = {0};
    int enable;
    int dns1_valid = 0, dns2_valid = 0, dns3_valid = 0;
    ip_addr_t dns1 = {0};
    ip_addr_t dns2 = {0};
    ip_addr_t dns3 = {0};

    AT_CMD_PARSE_NUMBER(0, &enable);
    AT_CMD_PARSE_OPT_STRING(1, dns_str1, sizeof(dns_str1), dns1_valid);
    AT_CMD_PARSE_OPT_STRING(2, dns_str2, sizeof(dns_str2), dns2_valid);
    AT_CMD_PARSE_OPT_STRING(3, dns_str3, sizeof(dns_str3), dns3_valid);

    if (enable < 0 || enable > 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (enable == 0 && argc != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (dns1_valid) {
        ipaddr_aton(dns_str1, &dns1);
        if (ip_addr_isany(&dns1)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_config->dns.dns[0] = dns1;
    }
    if (dns2_valid) {
        ipaddr_aton(dns_str2, &dns2);
        if (ip_addr_isany(&dns2)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_config->dns.dns[1] = dns2;
    } else {
        ip_addr_set_zero(&at_net_config->dns.dns[1]);
    }
    if (dns3_valid) {
        ipaddr_aton(dns_str3, &dns3);
        if (ip_addr_isany(&dns3)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_config->dns.dns[2] = dns3;
    } else {
        ip_addr_set_zero(&at_net_config->dns.dns[2]);
    }
    at_net_config->dns.dns_isset = 1;

    
    if (enable == 0 || ((enable == 1) && (!dns1_valid) && (!dns2_valid) && (!dns3_valid))) {
        ipaddr_aton(AT_CONFIG_DEFAULT_DNS1, &at_net_config->dns.dns[0]);
        ipaddr_aton(AT_CONFIG_DEFAULT_DNS2, &at_net_config->dns.dns[1]);
        ipaddr_aton("0.0.0.0", &at_net_config->dns.dns[2]);
        at_net_config->dns.dns_isset = 0;
    }
    dns_setserver(0, &at_net_config->dns.dns[0]);
    dns_setserver(1, &at_net_config->dns.dns[1]);
    dns_setserver(2, &at_net_config->dns.dns[2]);

    if (at->store) {
        at_net_config_save(AT_CONFIG_KEY_NET_DNS);
    }
    return AT_RESULT_CODE_OK;
}

static void _dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)callback_arg;

    if (ipaddr == NULL) {
        ipaddr = IP_ANY_TYPE;
    }

    at_response_string("+CIPDOMAIN:\"%s\"\r\n", ipaddr_ntoa((ip_addr_t *)ipaddr));

    xSemaphoreGive(sem);
}

static int at_setup_cmd_cipdomain(int argc, const char **argv)
{
    char hostname[128];
    int ip_network_valid = 0, ip_network = 2;
    ip_addr_t addr;
    uint8_t dns_addrtype;
    int ret;
    SemaphoreHandle_t sem = NULL;

    AT_CMD_PARSE_STRING(0, hostname, sizeof(hostname));
    AT_CMD_PARSE_OPT_NUMBER(1, &ip_network, ip_network_valid);

    if (ip_network_valid && (ip_network != 1 && ip_network != 2 && ip_network != 3)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (ip_network == 2) {
        dns_addrtype = LWIP_DNS_ADDRTYPE_IPV4;
    }
#if LWIP_IPV6
    else if (ip_network == 1 && at_net_config->ipv6_enable) {
        dns_addrtype = LWIP_DNS_ADDRTYPE_IPV4_IPV6;
    } else if (ip_network == 3 && at_net_config->ipv6_enable) {
        dns_addrtype = LWIP_DNS_ADDRTYPE_IPV6;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }
#endif

    sem = xSemaphoreCreateBinary();
    if (sem == NULL) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    ret = dns_gethostbyname_addrtype(hostname, &addr, _dns_found_callback, sem, dns_addrtype);
    //hostinfo = gethostbyname(hostname);
    if (ret == ERR_OK) {

        at_response_string("+CIPDOMAIN:\"%s\"\r\n", ipaddr_ntoa((ip_addr_t *)&addr));

    } else if (ret == ERR_INPROGRESS) {
        xSemaphoreTake(sem, portMAX_DELAY);
    } else {
        vSemaphoreDelete(sem);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
    vSemaphoreDelete(sem);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipstate(int argc, const char **argv)
{
    char type[8];
    uint16_t remote_port, local_port;
    uint8_t tetype;
    ip_addr_t ipaddr;
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (at_net_client_get_info(i, type, 8, &ipaddr, &remote_port, &local_port, &tetype) == 0) {
            at_response_string("+CIPSTATUS:%d,\"%s\",\"%s\",%d,%d,%d\r\n", i, type, ipaddr_ntoa(&ipaddr), remote_port, local_port, tetype);
        }
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipstart(int argc, const char **argv)
{
    char type[8];
    uint16_t remote_port, local_port;
    uint8_t tetype;
    ip_addr_t ipaddr;
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (at_net_client_get_info(i, type, 8, &ipaddr, &remote_port, &local_port, &tetype) == 0) {
            at_response_string("+CIPSTART:%d,\"%s\",\"%s\",%d,%d,%d\r\n", i, type, ipaddr_ntoa(&ipaddr), remote_port, local_port, tetype);
        }
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipstart(int argc, const char **argv)
{
    int linkid = 0;
    char type[8];
    char remote_host[128];
    int remote_port;
    int keepalive_valid = 0, keepalive = 0;
    int local_port_valid = 0, local_port = 0;
    int mode_valid = 0, mode = 0;
    int timeout_valid = 0, timeout = AT_NET_CIPSTART_TIMEOUT_DEFAULT_MS;
    char local_ip[16];
    int local_ip_valid = 0;
    ip_addr_t remote_ipaddr;
    int argc_index = 0, ret = 0;

    if (at_net_config->mux_mode == NET_LINK_MULT) {
        AT_CMD_PARSE_NUMBER(argc_index, &linkid);
        argc_index++;
    }

    AT_CMD_PARSE_STRING(argc_index, type, sizeof(type));
    argc_index++;
    AT_CMD_PARSE_STRING(argc_index, remote_host, sizeof(remote_host));
    argc_index++;
    AT_CMD_PARSE_NUMBER(argc_index, &remote_port);
    argc_index++;

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (at_string_host_to_ip(remote_host, &remote_ipaddr) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);

    }
    if (remote_port < 1 || remote_port > 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (strcasecmp(type, "TCP") == 0) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "TCPv6") == 0 && at_net_config->ipv6_enable) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "UDP") == 0) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &local_port, local_port_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &mode, mode_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "UDPv6") == 0 && at_net_config->ipv6_enable) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &local_port, local_port_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &mode, mode_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "SSL") == 0) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "SSLv6") == 0 && at_net_config->ipv6_enable) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    AT_CMD_PARSE_OPT_NUMBER(argc_index, &timeout, timeout_valid);
    argc_index++;

    if (timeout_valid && (timeout < 0 || timeout > 20000)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (keepalive_valid && (keepalive < 0 || keepalive > 7200)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (local_port_valid && (local_port < 0 || local_port > 65535)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (mode_valid && (mode < 0 || mode > 2)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (strcasecmp(type, "TCP") == 0 || strcasecmp(type, "TCPv6") == 0) {
        ret = at_net_client_tcp_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, keepalive, timeout);
    } else if (strcasecmp(type, "UDP") == 0 || strcasecmp(type, "UDPv6") == 0) {
        ret = at_net_client_udp_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, (uint16_t)local_port, mode, timeout);
    } else if (strcasecmp(type, "SSL") == 0 || strcasecmp(type, "SSLv6") == 0) {
        ret = at_net_client_ssl_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, keepalive, timeout);
    }

    if (ret != 0) {
        return AT_RESULT_WITH_SUB_CODE(ret);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipstartex(int argc, const char **argv)
{
    int linkid = 0;
    char type[8];
    char remote_host[128];
    int remote_port;
    int keepalive_valid = 0, keepalive = 0;
    int local_port_valid = 0, local_port = 0;
    int mode_valid = 0, mode = 0;
    int timeout_valid = 0, timeout = AT_NET_CIPSTART_TIMEOUT_DEFAULT_MS;
    char local_ip[16];
    int local_ip_valid = 0;
    ip_addr_t remote_ipaddr = {0};
    int argc_index = 0, ret = 0;

    linkid = at_net_client_get_valid_id();
    if (linkid < 0 || (at_net_config->mux_mode == NET_LINK_SINGLE && linkid != 0))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);

    AT_CMD_PARSE_STRING(argc_index, type, sizeof(type));
    argc_index++;
    AT_CMD_PARSE_STRING(argc_index, remote_host, sizeof(remote_host));
    argc_index++;
    AT_CMD_PARSE_NUMBER(argc_index, &remote_port);
    argc_index++;

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (at_string_host_to_ip(remote_host, &remote_ipaddr) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }
    if (remote_port < 1 || remote_port > 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (strcasecmp(type, "TCP") == 0) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "TCPv6") == 0 && at_net_config->ipv6_enable) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    }else if (strcasecmp(type, "UDP") == 0) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &local_port, local_port_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &mode, mode_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "SSL") == 0) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else if (strcasecmp(type, "SSLv6") == 0 && at_net_config->ipv6_enable) {
        AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
        argc_index++;
        AT_CMD_PARSE_OPT_STRING(argc_index, local_ip, sizeof(local_ip), local_ip_valid);
        argc_index++;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    AT_CMD_PARSE_OPT_NUMBER(argc_index, &timeout, timeout_valid);
    argc_index++;

    if (timeout_valid && (timeout < 0 || timeout > 20000)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (keepalive_valid && (keepalive < 0 || keepalive > 7200)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (local_port_valid && (local_port < 0 || local_port > 65535)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (mode_valid && (mode < 0 || mode > 2)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (strcasecmp(type, "TCP") == 0 || strcasecmp(type, "TCPv6") == 0) {
        ret = at_net_client_tcp_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, keepalive, timeout);
    } else if (strcasecmp(type, "UDP") == 0 || strcasecmp(type, "UDPv6") == 0) {
        ret = at_net_client_udp_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, (uint16_t)local_port, mode, timeout);
    } else if (strcasecmp(type, "SSL") == 0 || strcasecmp(type, "SSLv6") == 0) {
        ret = at_net_client_ssl_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, keepalive, timeout);
    }

    if (ret != 0) {
        return AT_RESULT_WITH_SUB_CODE(ret);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ciptcport(int argc, const char **argv)
{
    int i;

    for (i = 0; i < sizeof(at_net_config->tcp_opt)/sizeof(net_tcp_opt); i++) {
        at_response_string("+CIPTCPOPT:%d,%d,%d,%d,%d\r\n", i,
                at_net_config->tcp_opt[i].so_linger,
                at_net_config->tcp_opt[i].tcp_nodelay,
                at_net_config->tcp_opt[i].so_sndtimeo,
                at_net_config->tcp_opt[i].keep_alive);
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ciptcport(int argc, const char **argv)
{
    int linkid = 0;
    int so_linger_valid = 0, so_linger;
    int tcp_nodelay_valid = 0, tcp_nodelay;
    int so_sndtimeo_valid = 0, so_sndtimeo;
    int keepalive_valid = 0, keepalive;
    int argc_index = 0;

    if (at_net_config->mux_mode == NET_LINK_MULT) {
        AT_CMD_PARSE_NUMBER(argc_index, &linkid);
        argc_index++;
    }

    AT_CMD_PARSE_OPT_NUMBER(argc_index, &so_linger, so_linger_valid);
    argc_index++;
    AT_CMD_PARSE_OPT_NUMBER(argc_index, &tcp_nodelay, tcp_nodelay_valid);
    argc_index++;
    AT_CMD_PARSE_OPT_NUMBER(argc_index, &so_sndtimeo, so_sndtimeo_valid);
    argc_index++;
    AT_CMD_PARSE_OPT_NUMBER(argc_index, &keepalive, keepalive_valid);
    argc_index++;

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (so_linger_valid) {
        if (so_linger < 0 && so_linger != -1) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        at_net_config->tcp_opt[linkid].so_linger = so_linger;
    }
    if (tcp_nodelay_valid) {
        if (tcp_nodelay != 0 && tcp_nodelay != 1) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        at_net_config->tcp_opt[linkid].tcp_nodelay = tcp_nodelay;
    }
    if (so_sndtimeo_valid) {
        if (so_sndtimeo < 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        at_net_config->tcp_opt[linkid].so_sndtimeo = so_sndtimeo;
    }
    if (keepalive_valid) {
        if (keepalive < 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        at_net_config->tcp_opt[linkid].keep_alive = keepalive;
    }


    if (at->store) {
        at_net_config_save(AT_CONFIG_KEY_NET_TCP_OPT);
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipclose(int argc, const char **argv)
{
    int linkid;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    AT_CMD_PARSE_NUMBER(0, &linkid);

    if (at_net_client_id_is_valid(linkid)) {
        if (at_net_client_close(linkid) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
    } else if (linkid == AT_NET_CLIENT_HANDLE_MAX) {
        at_net_client_close_all();
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_cipclose(int argc, const char **argv)
{
    int linkid = 0;

    if (at_net_config->mux_mode == NET_LINK_MULT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    if (at_net_client_close(linkid) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

#if AT_TRANS_ZEROCOPY
static void _net_tx_async(int linkid, void *arg)
{
    trans_desc_t *desc_buf = (trans_desc_t *)arg;
#if (!AT_THROUGHPUT_NOWIFI)
    at_net_client_send(linkid, desc_buf->payload, desc_buf->len);
#endif
    nxspi_readbuf_push(desc_buf);
}
#endif

static int at_setup_cmd_cipsend(int argc, const char **argv)
{
    int linkid = 0;
    int length;
    char remote_host[128];
    int remote_port = 0;
    int remote_host_valid = 0;
    int remote_port_valid = 0;
    ip_addr_t remote_ipaddr;
    int recv_num = 0;
#if AT_TRANS_ZEROCOPY
    trans_desc_t *desc_buf = NULL;
#else
    uint8_t *buffer = at_net_tx_buffer;
#endif

    //AT_DEBUG_POINT(0);

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_NUMBER(0, &length);
        AT_CMD_PARSE_OPT_STRING(1, remote_host, sizeof(remote_host), remote_host_valid);
        AT_CMD_PARSE_OPT_NUMBER(2, &remote_port, remote_port_valid);
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        AT_CMD_PARSE_NUMBER(1, &length);
        AT_CMD_PARSE_OPT_STRING(2, remote_host, sizeof(remote_host), remote_host_valid);
        AT_CMD_PARSE_OPT_NUMBER(3, &remote_port, remote_port_valid);
    }

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (length <= 0 || length > AT_NET_TX_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (remote_host_valid) {
        if (at_string_host_to_ip(remote_host, &remote_ipaddr) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
    }
    if (remote_port_valid && (remote_port <= 0 || remote_port > 65535)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (remote_host_valid && remote_port_valid && (!at_net_client_is_connected(linkid))) {
        if (at_net_client_udp_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, 0, 0, 0) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
    }

    if (!at_net_client_is_connected(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
    if (remote_host_valid) {
        if (at_net_client_set_remote(linkid, &remote_ipaddr, (uint16_t)remote_port) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
    }
    //AT_DEBUG_POINT(0);

    AT_CMD_DATA_SEND("\r\nOK\r\n"AT_CMD_MSG_WAIT_DATA, strlen("\r\nOK\r\n"AT_CMD_MSG_WAIT_DATA));

#if AT_TRANS_ZEROCOPY
    
    at_workq_dowork(linkid, 0);

    do {
        desc_buf = nxspi_readbuf_pop(NXSPI_TYPE_AT, portMAX_DELAY);
        if (desc_buf) {
            desc_buf->len = desc_buf->len > (length - recv_num) ? (length - recv_num) : desc_buf->len;
            recv_num += desc_buf->len;
        }

        struct at_workq wq = {
            .pfunc = _net_tx_async,
            .arg = desc_buf,
        };
        at_workq_send(linkid, &wq, portMAX_DELAY);

    } while(recv_num < length);

#else 
    while(recv_num < length) {
        //AT_DEBUG_POINT(0);
        recv_num += AT_CMD_DATA_RECV(buffer + recv_num, length - recv_num);
    }

    if (at_net_client_send(linkid, buffer, recv_num) < 0) {
        return AT_RESULT_CODE_SEND_FAIL;
    }
#endif

    //AT_DEBUG_POINT(0);
    at_response_string("Recv %d bytes\r\n", recv_num);
    //AT_DEBUG_POINT(0);

    return AT_RESULT_CODE_SEND_OK;
}

static int at_exe_cmd_cipsend(int argc, const char **argv)
{
    int linkid = 0;

    printf("at_set_work_mode AT_WORK_MODE_THROUGHPUT\r\n");
    at_set_work_mode(AT_WORK_MODE_THROUGHPUT);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsendl(int argc, const char **argv)
{
    int linkid = 0;
    int length;
    char remote_host[128];
    int remote_port = 0;
    int remote_host_valid = 0;
    int remote_port_valid = 0;
    ip_addr_t remote_ipaddr;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_NUMBER(0, &length);
        AT_CMD_PARSE_OPT_STRING(1, remote_host, sizeof(remote_host), remote_host_valid);
        AT_CMD_PARSE_OPT_NUMBER(2, &remote_port, remote_port_valid);
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        AT_CMD_PARSE_NUMBER(1, &length);
        AT_CMD_PARSE_OPT_STRING(2, remote_host, sizeof(remote_host), remote_host_valid);
        AT_CMD_PARSE_OPT_NUMBER(3, &remote_port, remote_port_valid);
    }

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (length <= 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (remote_host_valid) {
        if (at_string_host_to_ip(remote_host, &remote_ipaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
    }
    if (remote_port_valid && (remote_port <= 0 || remote_port > 65535)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (remote_host_valid && remote_port_valid && (!at_net_client_is_connected(linkid))) {
        if (at_net_client_udp_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, 0, 0, 0) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
    }

    if (!at_net_client_is_connected(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
    if (remote_host_valid) {
        if (at_net_client_set_remote(linkid, &remote_ipaddr, (uint16_t)remote_port) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
    }

    at_through_set_length(linkid, length, at_net_config->sendl_cfg.report_size, at_net_config->sendl_cfg.transmit_size);
    at_set_work_mode(AT_WORK_MODE_CMD_THROUGHPUT);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsendlcfg(int argc, const char **argv)
{
    at_response_string("+CIPSENDLCFG:%d,%d\r\n", at_net_config->sendl_cfg.report_size, at_net_config->sendl_cfg.transmit_size);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsendlcfg(int argc, const char **argv)
{
    int report_size;
    int transmit_size;

    AT_CMD_PARSE_NUMBER(0, &report_size);
    AT_CMD_PARSE_NUMBER(1, &transmit_size);

    if (report_size < 100 || report_size > 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (transmit_size < 100 || transmit_size > 2920) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_net_config->sendl_cfg.report_size = (uint16_t)report_size;
    at_net_config->sendl_cfg.transmit_size = (uint16_t)transmit_size;
    return AT_RESULT_CODE_OK;
}

static int at_senddata_parse(char *data, int length, int *index, int *finish)
{
    int i;
    int flag_slash = 0;

    for (i = *index; i < length; i++) {
        if (data[i] == 0x30 && flag_slash == 1) {
            *index = 0;
            *finish = 1;
            return i - 1;
        }

        if (flag_slash) {
            memmove(data + (i - 1), data + i, length - i);
            length--;
            i--;
            flag_slash = 0;
            *index = i;
        } else {
            if (data[i] == 0x5C) {
                flag_slash = 1;
            } else {
                flag_slash = 0;
            }
        }
    }

    return length;
}

static int at_setup_cmd_cipsendex(int argc, const char **argv)
{
    int linkid = 0;
    int length;
    char remote_host[128];
    int remote_port = 0;
    int remote_host_valid = 0;
    int remote_port_valid = 0;
    ip_addr_t remote_ipaddr;
    int recv_num = 0;
    int index = 0, finish = 0;
    int ret;
#if AT_TRANS_ZEROCOPY
    trans_desc_t *desc_buf = NULL;
#else
    uint8_t *buffer = at_net_tx_buffer;
#endif

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_NUMBER(0, &length);
        AT_CMD_PARSE_OPT_STRING(1, remote_host, sizeof(remote_host), remote_host_valid);
        AT_CMD_PARSE_OPT_NUMBER(2, &remote_port, remote_port_valid);
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        AT_CMD_PARSE_NUMBER(1, &length);
        AT_CMD_PARSE_OPT_STRING(2, remote_host, sizeof(remote_host), remote_host_valid);
        AT_CMD_PARSE_OPT_NUMBER(3, &remote_port, remote_port_valid);
    }

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (length <= 0 || length > AT_NET_TX_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (remote_host_valid) {
        if (at_string_host_to_ip(remote_host, &remote_ipaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
    }
    if (remote_port_valid && (remote_port_valid <= 0 || remote_port_valid > 65535)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (remote_host_valid && remote_port_valid && (!at_net_client_is_connected(linkid))) {
        if (at_net_client_udp_connect(linkid, &remote_ipaddr, (uint16_t)remote_port, 0, 0, 0) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
    }

    if (!at_net_client_is_connected(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
    if (remote_host_valid) {
        if (at_net_client_set_remote(linkid, &remote_ipaddr, (uint16_t)remote_port) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
    }

    at_response_string("%s%s", AT_CMD_MSG_OK, AT_CMD_MSG_WAIT_DATA);

#if AT_TRANS_ZEROCOPY 
 
    at_workq_dowork(linkid, 0);

    do {
        desc_buf = nxspi_readbuf_pop(NXSPI_TYPE_AT, portMAX_DELAY);
        if (desc_buf) {
            desc_buf->len = desc_buf->len > (length - recv_num) ? (length - recv_num) : desc_buf->len;
            desc_buf->len = at_senddata_parse(desc_buf->payload, desc_buf->len, &index, &finish);
            recv_num += desc_buf->len;
        }

        struct at_workq wq = {
            .pfunc = _net_tx_async,
            .arg = desc_buf,
        };
        at_workq_send(linkid, &wq, portMAX_DELAY);

        if (finish) {
            break;
        }

    } while(recv_num < length);

#else
    while(recv_num < length) {
        ret = AT_CMD_DATA_RECV(buffer + recv_num, length - recv_num);
        if (ret > 0) {
            recv_num += ret;
            recv_num = at_senddata_parse(buffer, recv_num, &index, &finish);
            if (finish)
                break;
        }
    }

    if (at_net_client_send(linkid, buffer, recv_num) < 0) {
        return AT_RESULT_CODE_SEND_FAIL;
    }
#endif

    at_response_string("Recv %d bytes\r\n", recv_num);

    return AT_RESULT_CODE_SEND_OK;
}

static int at_query_cmd_cipdinfo(int argc, const char **argv)
{
    if (at_net_config->ipd_info == NET_IPDINFO_DISABLE_IPPORT) {
        at_response_string("+CIPDINFO:%s", "false");
    } else {
        at_response_string("+CIPDINFO:%s", "true");
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipdinfo(int argc, const char **argv)
{
    int enable;

    AT_CMD_PARSE_NUMBER(0, &enable);
    if (enable != NET_IPDINFO_DISABLE_IPPORT && enable != NET_IPDINFO_ENABLE_IPPORT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_net_config->ipd_info = (net_ipd_info)enable;
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipevt(int argc, const char **argv)
{
    int enable;

    AT_CMD_PARSE_NUMBER(0, &enable);

    if (enable < 0 || enable > 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_net_config->wips_enable = (uint8_t)enable;
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipevt(int argc, const char **argv)
{
    at_response_string("+CIPEVT:%d\r\n", at_net_config->wips_enable);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipmux(int argc, const char **argv)
{
    at_response_string("+CIPMUX:%d\r\n", at_net_config->mux_mode);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipmux(int argc, const char **argv)
{
    int mode;

    AT_CMD_PARSE_NUMBER(0, &mode);
    if(mode != NET_LINK_SINGLE && mode != NET_LINK_MULT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (at_net_sock_is_build()) {
        AT_NET_CMD_PRINTF("link is builded\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }

    at_net_config->mux_mode = mode;
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ciprecvmode(int argc, const char **argv)
{
    at_response_string("+CIPRECVMODE:%d\r\n", at_net_config->recv_mode);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ciprecvmode(int argc, const char **argv)
{
    int mode;

    AT_CMD_PARSE_NUMBER(0, &mode);
    if(mode != NET_RECV_MODE_ACTIVE && mode != NET_RECV_MODE_PASSIVE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (at_get_work_mode() != AT_WORK_MODE_CMD) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }
    for (int linkid = 0; linkid < AT_NET_CLIENT_HANDLE_MAX; linkid++) {
        if (at_net_client_is_connected(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        if (mode == NET_RECV_MODE_ACTIVE) {
            at_net_recvbuf_delete(linkid);
        }
    }

    at_net_config->recv_mode = mode;
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ciprecvdata(int argc, const char **argv)
{
    int read_len, remain_len, single_len, linkid = 0, size, ret = 0, n, offset = 0;
    uint8_t *buffer;
    uint8_t ishead = 1;

    AT_DEBUG_POINT(0);
    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        if (argc != 1) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
        }
        AT_CMD_PARSE_NUMBER(0, &size);
    } else {
        if (argc != 2) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
        }
        AT_CMD_PARSE_NUMBER(0, &linkid);
        AT_CMD_PARSE_NUMBER(1, &size);
    }

    if (size <= 0 || size > at_net_recvbuf_size_get(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

#if AT_THROUGHPUT_NOWIFI
    read_len = 1470;
#else
    read_len = at_net_client_get_recvsize(linkid);
#endif
    remain_len = read_len = read_len > size ? size : read_len;

    do {

        single_len = remain_len > (AT_NET_TX_MAX_LEN - (ishead?48:2)) ? (AT_NET_TX_MAX_LEN - (ishead?48:2)) : remain_len;

#if (AT_TRANS_ZEROCOPY)
        trans_desc_t *desc_buf = nxspi_writebuf_pop(NXSPI_TYPE_AT, portMAX_DELAY);
        if (!desc_buf) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
        buffer = desc_buf->payload;
        desc_buf->len = 0;
#else
        buffer = at_net_tx_buffer;
#endif

        if (ishead) {
            /* Use strncat + itoa instead of snprintf to improve performance. */
            uint8_t tmp[10];
            buffer[0] = 0;
            strncat(buffer, "+CIPRECVDATA:", 48);
            strncat(buffer, itoa(read_len, tmp, 10), 48 - strlen(buffer));
            strncat(buffer, ",", 48 - strlen(buffer));
            offset = strlen(buffer);
            ishead = 0;
        }

        AT_DEBUG_POINT(0);
        if (read_len) {
#if AT_THROUGHPUT_NOWIFI
            ret = read_len;
#else
            ret = at_net_recvbuf_read(linkid, NULL, NULL, buffer + offset, single_len);
#endif
            if (ret != single_len) {
                printf("at_net_recvbuf_read error %d\r\n", ret);
            }
        }
        AT_DEBUG_POINT(0);

        offset += ret;

        if (remain_len - single_len == 0) {
            memcpy(buffer + offset, "\r\n", 2);
        }

#if (AT_TRANS_ZEROCOPY)
        desc_buf->len = (remain_len - single_len == 0) ? (offset + 2) : offset;
        nxspi_writebuf_push(desc_buf);
#else
        AT_CMD_DATA_SEND((uint8_t *)buffer, (remain_len - single_len == 0) ? (offset + 2) : offset);
#endif
        AT_DEBUG_POINT(0);
        remain_len -= single_len;
        offset = 0;
    } while(remain_len > 0);

    //Release buffer if socket is closed and all data is read (fix memory leak)
    if (!at_net_client_is_connected(linkid) && at_net_client_get_recvsize(linkid) == 0) {
        at_net_recvbuf_delete(linkid);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ciprecvbuf(int argc, const char **argv)
{
    int linkid = 0, size;
    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_NUMBER(0, &size);
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        AT_CMD_PARSE_NUMBER(1, &size);
    }

    /* Reserve some size to prevent fragmented memory */
    if (size <= 0 || (size + 10240 > kfree_size())) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (at_net_client_is_connected(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }
    at_net_recvbuf_size_set(linkid, size);
    return AT_RESULT_CODE_OK;
}


static int at_query_cmd_ciprecvbuf(int argc, const char **argv)
{
    int linkid = 0, linkid_valid = 0;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_OPT_NUMBER(0, &linkid, linkid_valid);
        if (linkid_valid) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
    } else {
    	if (argc <= 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
    	}
        AT_CMD_PARSE_NUMBER(0, &linkid);
    }

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    at_response_string("+CIPRECVBUF:%d\r\n", at_net_recvbuf_size_get(linkid));
    return AT_RESULT_CODE_OK;
}
static int at_query_cmd_ciprecvlen(int argc, const char **argv)
{
    int id = 0;
    char buf[32] = {0};

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        at_response_string("+CIPRECVLEN:%d\r\n", at_net_client_get_recvsize(id));
    } else {
        for (id = 0; id < AT_NET_CLIENT_HANDLE_MAX; id++) {
            snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%d,", at_net_client_get_recvsize(id));
        }
        if (strlen(buf) > 0) {
            buf[strlen(buf)] = '\0';
        }
        at_response_string("+CIPRECVLEN:%s\r\n", buf);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipserver(int argc, const char **argv)
{
    uint16_t port;
    char type[8];
    int ca_enable;
    int keepalive;

    if (at_net_server_is_created(&port, type, &ca_enable, &keepalive)) {
        at_response_string("+CIPSERVER:%d,%d,\"%s\",%d,%d\r\n", 1, port, type, ca_enable, keepalive);
    } else {
        at_response_string("+CIPSERVER:%d\r\n", 0);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipserver(int argc, const char **argv)
{
    int mode;
    int param_valid = 0, param = 0;
    int port;
    char type[8];
    int type_valid = 0;
    int ca_enable_valid = 0, ca_enable = 0;
    int keepalive_valid = 0, keepalive = 0;
    int ret = 0;

    if (at_net_config->mux_mode != NET_LINK_MULT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    AT_CMD_PARSE_NUMBER(0, &mode);
    if (mode == 0) {
        AT_CMD_PARSE_OPT_NUMBER(1, &param, param_valid);

        if (param == 0) {
            ret = at_net_server_close();
        } else if (param == 1) {
            ret = at_net_server_close();
            ret |= at_net_server_sockets_close_all();
        } else {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
    } else if (mode == 1) {
        if (argc < 2) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
        }

        AT_CMD_PARSE_NUMBER(1, &port);
        AT_CMD_PARSE_OPT_STRING(2, type, sizeof(type), type_valid);
        if (!type_valid) {
            strlcpy(type, "TCP", sizeof(type));
        }
        AT_CMD_PARSE_OPT_NUMBER(3, &ca_enable, ca_enable_valid);
        AT_CMD_PARSE_OPT_NUMBER(4, &keepalive, keepalive_valid);

        if (port <= 0 || port > 65535) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        if (ca_enable != 0 && ca_enable != 1) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        if (keepalive_valid && (keepalive < 0 || keepalive > 7200)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }

        if (strcasecmp(type, "TCP") == 0) {
            ret = at_net_server_tcp_create((uint16_t)port, at_net_config->server_maxconn, at_net_config->server_timeout, 0, keepalive);
        } else if (strcasecmp(type, "UDP") == 0) {
            ret = at_net_server_udp_create((uint16_t)port, at_net_config->server_maxconn, at_net_config->server_timeout, 0);
        } else if (strcasecmp(type, "UDPv6") == 0) {
            ret = at_net_server_udp_create((uint16_t)port, at_net_config->server_maxconn, at_net_config->server_timeout, 1);
        } else  if (strcasecmp(type, "SSL") == 0) {
            ret = at_net_server_ssl_create((uint16_t)port, at_net_config->server_maxconn, at_net_config->server_timeout, ca_enable, 0, keepalive);
        } else  if (strcasecmp(type, "TCPv6") == 0 && at_net_config->ipv6_enable) {
            ret = at_net_server_tcp_create((uint16_t)port, at_net_config->server_maxconn, at_net_config->server_timeout, 1, keepalive);
        } else  if (strcasecmp(type, "SSLv6") == 0 && at_net_config->ipv6_enable) {
            ret = at_net_server_ssl_create((uint16_t)port, at_net_config->server_maxconn, at_net_config->server_timeout, ca_enable, 1, keepalive);
        } else {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (ret != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipservermaxconn(int argc, const char **argv)
{
    at_response_string("+CIPSERVERMAXCONN:%d\r\n", at_net_config->server_maxconn);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipservermaxconn(int argc, const char **argv)
{
    int maxconn = 0;

    AT_CMD_PARSE_NUMBER(0, &maxconn);

    if (maxconn < 1 || maxconn > AT_NET_CLIENT_HANDLE_MAX)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (at_net_server_is_created(NULL, NULL, NULL, NULL))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);

    at_net_config->server_maxconn = maxconn;
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsslcsni(int argc, const char **argv)
{
    int linkid = 0;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        at_response_string("+CIPSSLCSNI:%s\r\n", at_net_ssl_sni_get(linkid));
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
        at_response_string("+CIPSSLCSNI:%d,%s\r\n", linkid, at_net_ssl_sni_get(linkid));
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsslcsni(int argc, const char **argv)
{
    int linkid = 0;
    char hostname[255] = {0};

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_STRING(0, hostname, sizeof(hostname));
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
        AT_CMD_PARSE_STRING(1, hostname, sizeof(hostname));
    }

    at_net_ssl_sni_set(linkid, hostname);

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsslcalpn(int argc, const char **argv)
{
    int i, n, offset = 0;
    int linkid = 0, count = 0;
    char **alpn;
    char output[255] = {0};

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
    }
    alpn = at_net_ssl_alpn_get(linkid, &count);


    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        n = snprintf(output + offset, sizeof(output) - offset, "+CIPSSLCALPN:");
        if (n > 0) {
            offset += n;
        }
    } else {
        n = snprintf(output + offset, sizeof(output) - offset, "+CIPSSLCALPN:%d", linkid);
        if (n > 0) {
            offset += n;
        }
    }

    for (i = 0; i < count; i++) {
        n = snprintf(output + offset, sizeof(output) - offset, ",\"%s\"", alpn[i]);
        if (n > 0) {
            offset += n;
        }
    }

    n = snprintf(output + offset, sizeof(output) - offset, "%s", "\r\n");
    if (n > 0) {
        offset += n;
    }
    at_response_string(output);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsslcalpn(int argc, const char **argv)
{
    int i, offset;
    int linkid = 0, count = 0;
    char alpn[255] = {0};

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_NUMBER(0, &count);
        if (count != argc - 1) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
        }
        offset = 1;
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
        AT_CMD_PARSE_NUMBER(1, &count);
        if (count != argc - 2) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
        }
        offset = 2;
    }

    for (i = 0; i < count; i++) {
        AT_CMD_PARSE_STRING(i + offset, alpn, sizeof(alpn));
        at_net_ssl_alpn_set(linkid, i, alpn);
    }
    for (; i < 6; i++) {
        at_net_ssl_alpn_set(linkid, i, NULL);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsslcpsk(int argc, const char **argv)
{
    int linkid = 0;
    char *psk, *hint;
    int psk_len, hint_len;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
    }

    at_net_ssl_psk_get(linkid, &psk, &psk_len, &hint, &hint_len);
    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        at_response_string("+CIPSSLCPSK:\"%s\",\"%s\"\r\n", psk, hint);
    } else {
        at_response_string("+CIPSSLCPSK:%d,\"%s\",\"%s\"\r\n", linkid, psk, hint);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsslcpsk(int argc, const char **argv)
{
    int linkid = 0;
    char psk[32 + 1] = {0};
    char hint[32 + 1] = {0};

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_STRING(0, psk, sizeof(psk));
        AT_CMD_PARSE_STRING(1, hint, sizeof(hint));
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
        AT_CMD_PARSE_STRING(1, psk, sizeof(psk));
        AT_CMD_PARSE_STRING(2, hint, sizeof(hint));
    }
    at_net_ssl_psk_set(linkid, psk, strlen(psk), hint, strlen(hint));
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsslcpskhex(int argc, const char **argv)
{
    int linkid = 0;
    char *psk, *hint;
    int psk_len, hint_len;
    char output[128];
    int offset = 0, n;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
    }

    at_net_ssl_psk_get(linkid, &psk, &psk_len, &hint, &hint_len);

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        n = snprintf(output + offset, sizeof(output) - offset, "+CIPSSLCPSKHEX:\"");
        if (n > 0) {
            offset += n;
        }
    } else {
        n = snprintf(output + offset, sizeof(output) - offset, "+CIPSSLCPSKHEX:%d,\"", linkid);
        if (n > 0) {
            offset += n;
        }
    }

    for (int i = 0; i < psk_len; i++) {
        n = snprintf(output + offset, sizeof(output) - offset, "%02x", psk[i]);
        if (n > 0) {
            offset += n;
        }
    }

    n = snprintf(output + offset, sizeof(output) - offset, "\",\"%s\"\r\n", hint);
    if (n > 0) {
        offset += n;
    }

    at_response_string(output);

    return AT_RESULT_CODE_OK;
}

static int _str_to_hex(const char *str, uint8_t *hex)
{
    int i;
    int len = 0;

    len = strlen(str) / 2;

    for (i = 0; i < len; i++) {
        sscanf(str, "%02x", &hex[i]);
        str += 2;
    }

    return len;
}

static int at_setup_cmd_cipsslcpskhex(int argc, const char **argv)
{
    int linkid = 0, psk_len;
    char psk[64] = {0};
    char psk_hex[32] = {0};
    char hint[32] = {0};

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_STRING(0, psk, sizeof(psk));
        AT_CMD_PARSE_STRING(1, hint, sizeof(hint));
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        if (!at_net_client_id_is_valid(linkid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
        }
        AT_CMD_PARSE_STRING(1, psk, sizeof(psk));
        AT_CMD_PARSE_STRING(2, hint, sizeof(hint));
    }

    psk_len = _str_to_hex(psk, psk_hex);

    at_net_ssl_psk_set(linkid, psk_hex, psk_len, hint, strlen(hint));

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsto(int argc, const char **argv)
{
    at_response_string("+CIPSTO:%d\r\n", at_net_config->server_timeout);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsto(int argc, const char **argv)
{
    int timeout;

    AT_CMD_PARSE_NUMBER(0, &timeout);
    if(timeout < 0 || timeout > 7200) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (!at_net_config->server_timeout) {
        at_net_poll_start(timeout * 1000);
    }
    at_net_config->server_timeout = (net_server_timeout)timeout;
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_savetranslink(int argc, const char **argv)
{
    int mode;
    char remote_host[128];
    int remote_port;
    char type[8];
    int type_valid = 0;
    int param, param_valid = 0;
    int keepalive = 0;
    int local_port = 0;

    AT_CMD_PARSE_NUMBER(0, &mode);
    if (mode == 0) {
        if (argc != 1) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
        }

        at_net_config->trans_link.enable = 0;
        at_net_config_save(AT_CONFIG_KEY_NET_TRANS_LINK);
        return AT_RESULT_CODE_OK;
    } else if (mode == 1) {
        AT_CMD_PARSE_STRING(1, remote_host, sizeof(remote_host));
        AT_CMD_PARSE_NUMBER(2, &remote_port);
        AT_CMD_PARSE_OPT_STRING(3, type, sizeof(type), type_valid);
        AT_CMD_PARSE_OPT_NUMBER(4, &param, param_valid);
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (remote_port < 0 || remote_port > 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (type_valid == 0) {
        strlcpy(type, "TCP", sizeof(type));
    }

    if (strcmp(type, "TCP") == 0 || strcmp(type, "SSL") == 0 ||
        strcmp(type, "TCPv6") == 0 || strcmp(type, "SSLv6") == 0) {
        if (param_valid) {
            keepalive = param;
        }
        if (keepalive < 0 || keepalive > 7200) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
    } else if (strcmp(type, "UDP") == 0 || strcmp(type, "UDPv6") == 0) {
        if (param_valid) {
            local_port = param;
        }
        if (local_port < 0 || local_port > 65535) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_net_config->trans_link.enable = 1;
    strlcpy(at_net_config->trans_link.type ,type, sizeof(at_net_config->trans_link.type));
    strlcpy(at_net_config->trans_link.remote_host ,remote_host, sizeof(at_net_config->trans_link.remote_host));
    at_net_config->trans_link.remote_port = (uint16_t)remote_port;
    at_net_config->trans_link.keep_alive = (uint16_t)keepalive;
    at_net_config->trans_link.local_port = (uint16_t)local_port;
    at_net_config_save(AT_CONFIG_KEY_NET_TRANS_LINK);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsntpcfg(int argc, const char **argv)
{
    int enable, servernum, timezone;

    enable = at_net_config->sntp_cfg.enable;
    servernum = at_net_config->sntp_cfg.servernum;
    timezone = at_net_config->sntp_cfg.timezone;
    if (timezone < -12 || timezone > 14) {
        timezone = timezone/100;
    }
    if (enable) {
        if (servernum == 1) {
            at_response_string("+CIPSNTPCFG:1,%d,\"%s\"\r\n", timezone,
                at_net_config->sntp_cfg.server1);
        } else if (servernum == 2) {
            at_response_string("+CIPSNTPCFG:1,%d,\"%s\",\"%s\"\r\n", timezone,
                at_net_config->sntp_cfg.server1,
                at_net_config->sntp_cfg.server2);
        } else if (servernum == 3) {
            at_response_string("+CIPSNTPCFG:1,%d,\"%s\",\"%s\",\"%s\"\r\n", timezone,
                at_net_config->sntp_cfg.server1,
                at_net_config->sntp_cfg.server2,
                at_net_config->sntp_cfg.server3);
        }
    } else {
        at_response_string("+CIPSNTPCFG:0\r\n");
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsntpcfg(int argc, const char **argv)
{
    int enable;
    int timezone;
    char server1[64];
    char server2[64];
    char server3[64];
    int timezone_valid = 0;
    int server1_valid = 0;
    int server2_valid = 0;
    int server3_valid = 0;

    AT_CMD_PARSE_NUMBER(0, &enable);
    AT_CMD_PARSE_OPT_NUMBER(1, &timezone, timezone_valid);
    AT_CMD_PARSE_OPT_STRING(2, server1, sizeof(server1), server1_valid);
    AT_CMD_PARSE_OPT_STRING(3, server2, sizeof(server2), server2_valid);
    AT_CMD_PARSE_OPT_STRING(4, server3, sizeof(server3), server3_valid);

    if (enable != 0 && enable != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (enable == 1 && argc < 2) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (timezone < -1259 || timezone > 1459) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_net_config->sntp_cfg.enable = enable;
    at_net_config->sntp_cfg.servernum = 0;
    if (timezone_valid) {
        at_net_config->sntp_cfg.timezone = timezone;
    } else {
        at_net_config->sntp_cfg.timezone = 0;
    }
    if (server1_valid == 0 && server2_valid == 0 && server3_valid == 0) {
        at_net_config->sntp_cfg.servernum = 3;
        strlcpy(at_net_config->sntp_cfg.server1, "cn.ntp.org.cn", sizeof(at_net_config->sntp_cfg.server1));
        strlcpy(at_net_config->sntp_cfg.server2, "ntp.sjtu.edu.cn", sizeof(at_net_config->sntp_cfg.server2));
        strlcpy(at_net_config->sntp_cfg.server3, "us.pool.ntp.org", sizeof(at_net_config->sntp_cfg.server3));
    } else {
        if (server1_valid) {
            strlcpy(at_net_config->sntp_cfg.server1, server1, sizeof(at_net_config->sntp_cfg.server1));
            at_net_config->sntp_cfg.servernum++;
        }
        if (server2_valid) {
            strlcpy(at_net_config->sntp_cfg.server2, server2, sizeof(at_net_config->sntp_cfg.server2));
            at_net_config->sntp_cfg.servernum++;
        }
        if (server3_valid) {
            strlcpy(at_net_config->sntp_cfg.server3, server3, sizeof(at_net_config->sntp_cfg.server3));
            at_net_config->sntp_cfg.servernum++;
        }
    }

    if (enable == 0) {
        at_net_sntp_stop();
    } else {
        at_net_sntp_stop();
        at_net_sntp_start();
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsntptime(int argc, const char **argv)
{
    struct timespec tp;
    char time_buf[32];

    at_net_sntp_gettime(&tp);

    ctime_r(&tp.tv_sec, time_buf);

    time_buf[strlen(time_buf)-1] = '\0'; // delete last 0x0A

    at_response_string("+CIPSNTPTIME:%s\r\n", time_buf);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsntpintv(int argc, const char **argv)
{
    at_response_string("+CIPSNTPINTV:%d\r\n", at_net_config->sntp_intv.interval);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsntpintv(int argc, const char **argv)
{
    int interval;

    AT_CMD_PARSE_NUMBER(0, &interval);

    if (interval < 15 || interval > 4294967) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_net_config->sntp_intv.interval = interval;
    if (at_net_sntp_is_start()) {
        at_net_sntp_stop();
        at_net_sntp_start();
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipreconnintv(int argc, const char **argv)
{
    at_response_string("+CIPRECONNINTV:%d\r\n", at_net_config->reconn_intv);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipreconnintv(int argc, const char **argv)
{
    int reconn_intv;

    AT_CMD_PARSE_NUMBER(0, &reconn_intv);
    if (reconn_intv < 1 || reconn_intv > 36000) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_net_config->reconn_intv = (net_reconn_intv)reconn_intv;
    if (at->store) {
        at_net_config_save(AT_CONFIG_KEY_NET_RECONN_INTV);
    }
    return AT_RESULT_CODE_OK;
}

static int _ping_callback(int ping_time)
{
    if (ping_time >= 0) {
        at_response_string("+PING:%dms\r\n", ping_time);
    } else {
        at_response_string("+PING:%s\r\n", "TIMEOUT");
    }
    return 0;
}

static int at_setup_cmd_ping(int argc, const char **argv)
{
    // TODO: FIXME
#if 1
    char hostname[128];
    struct hostent *hostinfo;
    struct ping_var *env;
    int len, count, interval, timeout;
    int len_valid = 0, count_valid = 0, interval_valid = 0, timeout_valid=0;

    AT_CMD_PARSE_STRING(0, hostname, sizeof(hostname));
    AT_CMD_PARSE_OPT_NUMBER(1, &len, len_valid);
    AT_CMD_PARSE_OPT_NUMBER(2, &count, count_valid);
    AT_CMD_PARSE_OPT_NUMBER(3, &interval, interval_valid);
    AT_CMD_PARSE_OPT_NUMBER(4, &timeout, timeout_valid);
    if (!len_valid) {
        len = 64;
    }
    if (!count_valid) {
        count = 4;
    }
    if (!interval_valid) {
        interval = 1000;
    }
    if (!timeout_valid) {
        timeout = 1000;
    }
    if (len <= 0 || len >= 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (interval <= 0 || interval >= 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (timeout <= 0 || timeout >= 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if(timeout<=interval) {
        timeout=interval;
    }
    hostinfo = gethostbyname(hostname);
    if (hostinfo) {
#if LWIP_IPV6
        if (IP_IS_V6((ip_addr_t *)hostinfo->h_addr) && !at_net_config->ipv6_enable) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
#endif
        env = ping_api_init(interval, len, count, timeout, (ip_addr_t *)hostinfo->h_addr, _ping_callback);
        if (env) {
            while (env->requests_count <= 0) //wait start
                vTaskDelay(1);
            //while (env->node_num > 0) //wait finish
            //    vTaskDelay(interval);
        } else {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }

    return AT_RESULT_CODE_OK;
#endif
}

static int at_query_cmd_ciupdate(int argc, const char **argv)
{
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ciupdate(int argc, const char **argv)
{
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_ciupdate(int argc, const char **argv)
{
    return AT_RESULT_CODE_OK;
}

static int at_iperf_redirect(void )
{
    if (NULL == fhost_iperf_msg_handle_get()) {
        return 0;
    }
    return (xTaskGetCurrentTaskHandle() == fhost_iperf_msg_handle_get() || (strcmp(pcTaskGetName(NULL), "RX") == 0));
}

static int at_setup_cmd_iperf(int argc, const char **argv)
{
    int is_server = 0;
    int is_udp = 0;
    int t, t_valid = 0;
    int p, p_valid = 0;
    int ip_valid = 0;
    char ipaddr[32] = {0};
    char type[4] = {0};
    char direct[3] = {0};
    char buffer[128];

    AT_CMD_PARSE_STRING(0, direct, sizeof(direct));
    AT_CMD_PARSE_STRING(1, type, sizeof(type));
    AT_CMD_PARSE_OPT_STRING(2, ipaddr, sizeof(ipaddr), ip_valid);
    AT_CMD_PARSE_OPT_NUMBER(3, &t, t_valid);
    AT_CMD_PARSE_OPT_NUMBER(4, &p, p_valid);

    if (strcmp(direct, "TX") == 0) {
        is_server = 0;
    } else if (strcmp(direct, "RX") == 0) {
        is_server = 1;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_TYPE_MISMATCH);
    }

    if (strcmp(type, "TCP") == 0) {
        is_udp = 0;
    } else if (strcmp(type, "UDP") == 0) {
        is_udp = 1;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_TYPE_MISMATCH);
    }

    if (!t_valid) {
        t = 10;
    }

    if (!p_valid) {
        p = 5001;
    }

    if (!is_server && !ip_valid) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    if (fhost_iperf_msg_handle_get() != NULL) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }

    snprintf(buffer, sizeof(buffer),
             "iperf "\
             "%s "\
             "%s "\
             "%s "\
             "%s "\
             "-i 1 "\
             "-t %d "\
             "-p %d\r\n",
             is_udp?"-u":"",
             is_server?"-s":"-c",
             is_server?"":ipaddr,
             (is_udp && !is_server)?"-b 200M":"",
             t,
             p);

    printf(buffer);
    at_output_redirect_register(at_iperf_redirect);
    shell_exe_cmd(buffer, strlen(buffer));

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_iperf_stop(int argc, const char **argv)
{
    shell_exe_cmd("iperf stop\r\n", strlen("iperf stop\r\n"));

    return AT_RESULT_CODE_OK;
}

#define AT_NET_SSL_NOT_AUTH         0
#define AT_NET_SSL_CLIENT_AUTH      1
#define AT_NET_SSL_SERVER_AUTH      2
#define AT_NET_SSL_BOTH_AUTH        3

static int at_setup_cmd_cipsslcconf(int argc, const char **argv)
{
    int linkid = 0, auth_mode;
    char cert_file[32] = {0};
    char key_file[32] = {0};
    char ca_file[32] = {0};
    int cert_file_valid = 0, key_file_valid = 0, ca_file_valid = 0;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        AT_CMD_PARSE_NUMBER(0, &auth_mode);
        AT_CMD_PARSE_OPT_STRING(1, cert_file, sizeof(cert_file), cert_file_valid);
        AT_CMD_PARSE_OPT_STRING(2, key_file, sizeof(key_file), key_file_valid);
        AT_CMD_PARSE_OPT_STRING(3, ca_file, sizeof(ca_file), ca_file_valid);
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
        AT_CMD_PARSE_NUMBER(1, &auth_mode);
        AT_CMD_PARSE_OPT_STRING(2, cert_file, sizeof(cert_file), cert_file_valid);
        AT_CMD_PARSE_OPT_STRING(3, key_file, sizeof(key_file), key_file_valid);
        AT_CMD_PARSE_OPT_STRING(4, ca_file, sizeof(ca_file), ca_file_valid);
    }

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (auth_mode < AT_NET_SSL_NOT_AUTH || auth_mode > AT_NET_SSL_BOTH_AUTH) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (auth_mode == AT_NET_SSL_NOT_AUTH) {
        at_net_ssl_path_set(linkid, NULL, NULL, NULL);
    } else if (auth_mode == AT_NET_SSL_CLIENT_AUTH) {
        if (cert_file_valid == 0 || key_file_valid == 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        if (!file_exists(cert_file) || !file_exists(key_file)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_ssl_path_set(linkid, NULL, cert_file, key_file);
    } else if (auth_mode == AT_NET_SSL_SERVER_AUTH) {
        if (ca_file_valid == 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        if (!file_exists(ca_file)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_ssl_path_set(linkid, ca_file, NULL, NULL);
    } else if (auth_mode == AT_NET_SSL_BOTH_AUTH) {
        if (cert_file_valid == 0 || key_file_valid == 0 || ca_file_valid == 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        if (!file_exists(cert_file) || !file_exists(key_file) || !file_exists(ca_file)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_ssl_path_set(linkid, ca_file, cert_file, key_file);
    }

    strlcpy(at_net_config->sslconf[linkid].ca_file, ca_file, sizeof(at_net_config->sslconf[linkid].ca_file));
    strlcpy(at_net_config->sslconf[linkid].cert_file, cert_file, sizeof(at_net_config->sslconf[linkid].cert_file));
    strlcpy(at_net_config->sslconf[linkid].key_file, key_file, sizeof(at_net_config->sslconf[linkid].key_file));

    if (at->store) {
        at_net_config_save(AT_CONFIG_KEY_NET_SSLCONF);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsslcconf(int argc, const char **argv)
{
    int linkid = 0, auth_mode = 0;
    char *ca, *cert, *key;

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
    } else {
        AT_CMD_PARSE_NUMBER(0, &linkid);
    }

    if (!at_net_client_id_is_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    at_net_ssl_path_get(linkid, &ca, &cert, &key);

    if (strlen(ca)) {
        auth_mode |= 2;
    }

    if (strlen(cert) && strlen(key)) {
        auth_mode |= 1;
    }

    if (at_net_config->mux_mode == NET_LINK_SINGLE) {
        at_response_string("+CIPSSLCCONF:%d,\"%s\",\"%s\",\"%s\"\r\n", auth_mode, cert, key, ca);
    } else {
        at_response_string("+CIPSSLCCONF:%d,%d,\"%s\",\"%s\",\"%s\"\r\n", linkid, auth_mode, cert, key, ca);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsslsconf(int argc, const char **argv)
{
    int linkid = 0, auth_mode;
    char cert_file[32] = {0};
    char key_file[32] = {0};
    char ca_file[32] = {0};
    int cert_file_valid = 0, key_file_valid = 0, ca_file_valid = 0;

    if (at_net_config->mux_mode != NET_LINK_MULT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    AT_CMD_PARSE_NUMBER(0, &auth_mode);
    AT_CMD_PARSE_OPT_STRING(1, cert_file, sizeof(cert_file), cert_file_valid);
    AT_CMD_PARSE_OPT_STRING(2, key_file, sizeof(key_file), key_file_valid);
    AT_CMD_PARSE_OPT_STRING(3, ca_file, sizeof(ca_file), ca_file_valid);

    if (auth_mode < AT_NET_SSL_NOT_AUTH || auth_mode > AT_NET_SSL_BOTH_AUTH) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (auth_mode == AT_NET_SSL_NOT_AUTH) {
        at_net_ssl_server_path_set(linkid, NULL, NULL, NULL);
    } else if (auth_mode == AT_NET_SSL_CLIENT_AUTH) {
        if (ca_file_valid == 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        if (!file_exists(ca_file)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_ssl_server_path_set(linkid, ca_file, NULL, NULL);
    } else if (auth_mode == AT_NET_SSL_SERVER_AUTH) {
        if (cert_file_valid == 0 || key_file_valid == 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        if (!file_exists(cert_file) || !file_exists(key_file)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_ssl_server_path_set(linkid, NULL, cert_file, key_file);
    } else if (auth_mode == AT_NET_SSL_BOTH_AUTH) {
        if (cert_file_valid == 0 || key_file_valid == 0 || ca_file_valid == 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        if (!file_exists(cert_file) || !file_exists(key_file) || !file_exists(ca_file)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        at_net_ssl_server_path_set(linkid, ca_file, cert_file, key_file);
    }

    return AT_RESULT_CODE_OK;
}


static int at_query_cmd_cipsslsconf(int argc, const char **argv)
{
    int linkid = 0, auth_mode = 0;
    char *ca, *cert, *key;

    at_net_ssl_server_path_get(linkid, &ca, &cert, &key);

    if (strlen(ca)) {
        auth_mode |= 1;
    }

    if (strlen(cert) && strlen(key)) {
        auth_mode |= 2;
    }

    at_response_string("+CIPSSLSCONF:%d,\"%s\",\"%s\",\"%s\"\r\n", auth_mode, cert, key, ca);

    return AT_RESULT_CODE_OK;
}

static const at_cmd_struct at_net_cmd[] = {
    {"+CIPSEND",           NULL, at_setup_cmd_cipsend, at_exe_cmd_cipsend, 1, 4},
    {"+CIPRECVDATA",       NULL, at_setup_cmd_ciprecvdata, NULL, 1, 2},
    {"+CIFSR",             NULL, NULL, at_exe_cmd_cifsr, 0, 0},
    {"+CIPV6",             at_query_cmd_cipv6, at_setup_cmd_cipv6, NULL, 1, 1},
    {"+CIPDNS",            at_query_cmd_cipdns, at_setup_cmd_cipdns, NULL, 1, 4},
    {"+CIPDOMAIN",         NULL, at_setup_cmd_cipdomain, NULL, 1, 2},
    {"+CIPSTATE",          at_query_cmd_cipstate, NULL, NULL, 0, 0},
    {"+CIPSTART",          at_query_cmd_cipstart, at_setup_cmd_cipstart, NULL, 3, 7},
    {"+CIPSTARTEX",        NULL, at_setup_cmd_cipstartex, NULL, 3, 6},
    {"+CIPTCPOPT",         at_query_cmd_ciptcport, at_setup_cmd_ciptcport, NULL, 1, 5},
    {"+CIPCLOSE",          NULL, at_setup_cmd_cipclose, at_exe_cmd_cipclose, 1, 1},
    {"+CIPSENDL",          NULL, at_setup_cmd_cipsendl, NULL, 1, 4},
    {"+CIPSENDLCFG",       at_query_cmd_cipsendlcfg, at_setup_cmd_cipsendlcfg, NULL, 2, 2},
    {"+CIPSENDEX",         NULL, at_setup_cmd_cipsendex, NULL, 1, 4},
    {"+CIPDINFO",          at_query_cmd_cipdinfo, at_setup_cmd_cipdinfo, NULL, 1, 1},
    {"+CIPEVT",            at_query_cmd_cipevt, at_setup_cmd_cipevt, NULL, 1, 1},
    {"+CIPMUX",            at_query_cmd_cipmux, at_setup_cmd_cipmux, NULL, 1, 1},
    {"+CIPRECVMODE",       at_query_cmd_ciprecvmode, at_setup_cmd_ciprecvmode, NULL, 1, 1},
    {"+CIPRECVBUF",        at_query_cmd_ciprecvbuf, at_setup_cmd_ciprecvbuf, NULL, 1, 2},
    {"+CIPRECVLEN",        at_query_cmd_ciprecvlen, NULL, NULL, 0, 0},
    {"+CIPSERVER",         at_query_cmd_cipserver, at_setup_cmd_cipserver, NULL, 1, 5},
    {"+CIPSERVERMAXCONN",  at_query_cmd_cipservermaxconn, at_setup_cmd_cipservermaxconn, NULL, 1, 1},
    {"+CIPSSLCCONF",       at_query_cmd_cipsslcconf, at_setup_cmd_cipsslcconf, NULL, 1, 5},
    {"+CIPSSLSCONF",       at_query_cmd_cipsslsconf, at_setup_cmd_cipsslsconf, NULL, 1, 4},
    //{"+CIPSSLCCN",       at_query_cmd_cipsslccn, at_setup_cmd_cipsslccn, NULL, 0, 0},
    {"+CIPSSLCSNI",        at_query_cmd_cipsslcsni, at_setup_cmd_cipsslcsni, NULL, 1, 2},
    {"+CIPSSLCALPN",       at_query_cmd_cipsslcalpn, at_setup_cmd_cipsslcalpn, NULL, 2, 8},
    {"+CIPSSLCPSK",        at_query_cmd_cipsslcpsk, at_setup_cmd_cipsslcpsk, NULL, 2, 3},
    {"+CIPSSLCPSKHEX",     at_query_cmd_cipsslcpskhex, at_setup_cmd_cipsslcpskhex, NULL, 2, 3},
    {"+CIPSTO",            at_query_cmd_cipsto, at_setup_cmd_cipsto, NULL,  1, 1},
    {"+SAVETRANSLINK",     NULL, at_setup_cmd_savetranslink, NULL, 1, 5},
    {"+CIPSNTPCFG",        at_query_cmd_cipsntpcfg, at_setup_cmd_cipsntpcfg, NULL, 1, 5},
    {"+CIPSNTPTIME",       at_query_cmd_cipsntptime, NULL, NULL, 0, 0},
    {"+CIPSNTPINTV",       at_query_cmd_cipsntpintv, at_setup_cmd_cipsntpintv, NULL, 1, 1},
    {"+CIPRECONNINTV",     at_query_cmd_cipreconnintv, at_setup_cmd_cipreconnintv, NULL, 1, 1},
    {"+PING",              NULL, at_setup_cmd_ping, NULL, 1, 5},
    {"+IPERF",             NULL, at_setup_cmd_iperf, NULL, 2, 5},
    {"+IPERFSTOP",         NULL, NULL, at_setup_cmd_iperf_stop, 0, 0},
    {NULL,              NULL, NULL, NULL, 0, 0},
};

bool at_net_cmd_regist(void)
{
    at_net_config_init();

    at_net_start();

    at_register_function(at_net_config_default, at_net_stop);

    if (at_cmd_register(at_net_cmd, sizeof(at_net_cmd) / sizeof(at_net_cmd[0])) == 0)
        return true;
    else
        return false;
}

