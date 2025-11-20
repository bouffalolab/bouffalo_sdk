/**
  ******************************************************************************
  * @file    at_net_config.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>
//#ifdef EASYFLASH_ENABLE
#include <easyflash.h>
//#endif
#include "at_config.h"
#include "at_net_main.h"
#include "at_net_config.h"
#include "at_pal.h"

net_config *at_net_config = NULL;

int at_net_config_init(void)
{
    int i;

    at_net_config = (net_config *)at_malloc(sizeof(net_config));
    if (at_net_config == NULL) {
        AT_CMD_PRINTF("Failed to allocate memory for at_net_config\r\n");
        return -1;
    }

    memset(at_net_config, 0, sizeof(net_config));
    at_net_config->mux_mode = NET_LINK_SINGLE;
    at_net_config->work_mode = NET_MODE_NORMAL;
    if (!at_config_read(AT_CONFIG_KEY_NET_TCP_OPT, &at_net_config->tcp_opt, sizeof(at_net_config->tcp_opt))) {
        AT_CMD_PRINTF("TCP OPT config read failed, using defaults\r\n");
        for (i = 0; i < sizeof(at_net_config->tcp_opt)/sizeof(net_tcp_opt); i++) {
            at_net_config->tcp_opt[i].so_linger = -1;
            at_net_config->tcp_opt[i].tcp_nodelay = 0;
            at_net_config->tcp_opt[i].so_sndtimeo = 0;
            at_net_config->tcp_opt[i].keep_alive = 0;
        }
    }
    at_net_config->server_maxconn = AT_NET_CLIENT_HANDLE_MAX;
    at_net_config->server_timeout = 0;
    memset(&at_net_config->sntp_cfg, 0, sizeof(net_sntp_cfg));
    at_net_config->sntp_intv.interval = 300;
    at_net_config->ipd_info = NET_IPDINFO_DISABLE_IPPORT;
    at_net_config->wips_enable = 1;
    if (!at_config_read(AT_CONFIG_KEY_NET_RECONN_INTV, &at_net_config->reconn_intv, sizeof(at_net_config->reconn_intv))) {
        AT_CMD_PRINTF("RECONN INTV config read failed, using default\r\n");
        at_net_config->reconn_intv = 1;
    }
    at_net_config->recv_mode = NET_RECV_MODE_ACTIVE;
    if (!at_config_read(AT_CONFIG_KEY_NET_TRANS_LINK, &at_net_config->trans_link, sizeof(at_net_config->trans_link))) {
        AT_CMD_PRINTF("TRANS LINK config read failed, using default\r\n");
        at_net_config->trans_link.enable = 0;
    }
    at_net_config->sendl_cfg.report_size = 1024;
    at_net_config->sendl_cfg.transmit_size = 2920;
    if (!at_config_read(AT_CONFIG_KEY_NET_SSLCONF, &at_net_config->sslconf, sizeof(at_net_config->sslconf))) {
        AT_CMD_PRINTF("SSL CONF config read failed\r\n");
    }
    if (!at_config_read(AT_CONFIG_KEY_NET_IPV6_ENABLE, &at_net_config->ipv6_enable, sizeof(at_net_config->ipv6_enable))) {
        AT_CMD_PRINTF("IPV6 ENABLE config read failed, using default\r\n");
        at_net_config->ipv6_enable = 0;
    }
    if (!at_config_read(AT_CONFIG_KEY_NET_DNS, &at_net_config->dns, sizeof(at_net_config->dns))) {
        AT_CMD_PRINTF("DNS config read failed\r\n");
        //ipaddr_aton(AT_CONFIG_DEFAULT_DNS1, &at_net_config->dns.dns[0]);
        //ipaddr_aton(AT_CONFIG_DEFAULT_DNS2, &at_net_config->dns.dns[1]);
    } 
    return 0;
}

int at_net_config_save(const char *key)
{
    if (!at_net_config || !key) {
        AT_CMD_PRINTF("Invalid arguments to at_net_config_save\r\n");
        return -1;
    }
    if (strcmp(key, AT_CONFIG_KEY_NET_TCP_OPT) == 0)
        return at_config_write(key, &at_net_config->tcp_opt, sizeof(at_net_config->tcp_opt));
    else if (strcmp(key, AT_CONFIG_KEY_NET_RECONN_INTV) == 0)
        return at_config_write(key, &at_net_config->reconn_intv, sizeof(at_net_config->reconn_intv));
    else if (strcmp(key, AT_CONFIG_KEY_NET_TRANS_LINK) == 0)
        return at_config_write(key, &at_net_config->trans_link, sizeof(at_net_config->trans_link));
    else if (strcmp(key, AT_CONFIG_KEY_NET_SSLCONF) == 0)
        return at_config_write(key, &at_net_config->sslconf, sizeof(at_net_config->sslconf));
    else if (strcmp(key, AT_CONFIG_KEY_NET_IPV6_ENABLE) == 0) {
        return at_config_write(key, &at_net_config->ipv6_enable, sizeof(at_net_config->ipv6_enable));
    } else if (strcmp(key, AT_CONFIG_KEY_NET_DNS) == 0) {
        return at_config_write(key, &at_net_config->dns, sizeof(at_net_config->dns));
    }
        return -1;
}

int at_net_config_default(void)
{
    ef_del_env(AT_CONFIG_KEY_NET_TCP_OPT);
    ef_del_env(AT_CONFIG_KEY_NET_RECONN_INTV);
    ef_del_env(AT_CONFIG_KEY_NET_TRANS_LINK);
    ef_del_env(AT_CONFIG_KEY_NET_SSLCONF);
    ef_del_env(AT_CONFIG_KEY_NET_IPV6_ENABLE);
    ef_del_env(AT_CONFIG_KEY_NET_DNS);
    if (at_net_config) {
        memset(&at_net_config->tcp_opt, 0, sizeof(at_net_config->tcp_opt));
        memset(&at_net_config->reconn_intv, 0, sizeof(at_net_config->reconn_intv));
        memset(&at_net_config->trans_link, 0, sizeof(at_net_config->trans_link));
        memset(&at_net_config->sslconf, 0, sizeof(at_net_config->sslconf));
        memset(&at_net_config->ipv6_enable, 0, sizeof(at_net_config->ipv6_enable));
        memset(&at_net_config->dns, 0, sizeof(at_net_config->dns));
    }
    return 0;
}

