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

net_config *at_net_config = NULL;

int at_net_config_init(void)
{
    int i;

    at_net_config = (net_config *)pvPortMalloc(sizeof(net_config));
    if (at_net_config == NULL) {
        return -1;
    }

    memset(at_net_config, 0, sizeof(net_config));
    at_net_config->mux_mode = NET_LINK_SINGLE;
    at_net_config->work_mode = NET_MODE_NORMAL;
    if (!at_config_read(AT_CONFIG_KEY_NET_TCP_OPT, &at_net_config->tcp_opt, sizeof(at_net_config->tcp_opt))) {
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
    if (!at_config_read(AT_CONFIG_KEY_NET_RECONN_INTV, &at_net_config->reconn_intv, sizeof(at_net_config->reconn_intv)))
        at_net_config->reconn_intv = 1;
    at_net_config->recv_mode = NET_RECV_MODE_ACTIVE;
    if (!at_config_read(AT_CONFIG_KEY_NET_TRANS_LINK, &at_net_config->trans_link, sizeof(at_net_config->trans_link))) {
        at_net_config->trans_link.enable = 0;
    }
    at_net_config->sendl_cfg.report_size = 1024;
    at_net_config->sendl_cfg.transmit_size = 2920;

    at_config_read(AT_CONFIG_KEY_NET_SSLCONF, &at_net_config->sslconf, sizeof(at_net_config->sslconf));
    
    if (!at_config_read(AT_CONFIG_KEY_NET_IPV6_ENABLE, &at_net_config->ipv6_enable, sizeof(at_net_config->ipv6_enable))) {
        at_net_config->ipv6_enable = 0;
    }
    if (!at_config_read(AT_CONFIG_KEY_NET_DNS, &at_net_config->dns, sizeof(at_net_config->dns))) {
        //ipaddr_aton(AT_CONFIG_DEFAULT_DNS1, &at_net_config->dns.dns[0]);
        //ipaddr_aton(AT_CONFIG_DEFAULT_DNS2, &at_net_config->dns.dns[1]);
    } 
    return 0;
}

int at_net_config_save(const char *key)
{
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
    at_config_delete(AT_CONFIG_KEY_NET_TCP_OPT);
    at_config_delete(AT_CONFIG_KEY_NET_RECONN_INTV);
    at_config_delete(AT_CONFIG_KEY_NET_TRANS_LINK);
    at_config_delete(AT_CONFIG_KEY_NET_SSLCONF);
    at_config_delete(AT_CONFIG_KEY_NET_IPV6_ENABLE);
    at_config_delete(AT_CONFIG_KEY_NET_DNS);
    return 0;
}

