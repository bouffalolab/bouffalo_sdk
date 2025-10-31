/**
  ******************************************************************************
  * @file    at_net_config.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_NET_CONFIG_H
#define AT_NET_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CONFIG_KEY_NET_TCP_OPT "NETSOCK"
#define AT_CONFIG_KEY_NET_RECONN_INTV "NETRECONNINTV"
#define AT_CONFIG_KEY_NET_TRANS_LINK "NETTRANSLINK"
#define AT_CONFIG_KEY_NET_SSLCONF "NETSSLCONF"
#define AT_CONFIG_KEY_NET_IPV6_ENABLE "IPV6"
#define AT_CONFIG_KEY_NET_DNS "DNS"


#define AT_CONFIG_DEFAULT_DNS1 "208.67.222.222" 
#define AT_CONFIG_DEFAULT_DNS2 "8.8.8.8" 

typedef enum {
    NET_MODE_NORMAL = 0,
    NET_MODE_TRANS
} net_work_mode;

typedef enum {
    NET_LINK_SINGLE = 0,
    NET_LINK_MULT
} net_mux_mode;

typedef struct {
    int so_linger;
    int tcp_nodelay;
    int so_sndtimeo;
    int keep_alive;
} net_tcp_opt;

typedef int net_server_maxconn;

typedef uint16_t net_server_timeout;

typedef struct {
    uint8_t enable;
    uint8_t servernum;
    int16_t timezone;
    char server1[64];
    char server2[64];
    char server3[64];
} net_sntp_cfg;

typedef struct {
    uint32_t interval;
} net_sntp_intv;

typedef enum {
    NET_IPDINFO_DISABLE_IPPORT = 0,
    NET_IPDINFO_ENABLE_IPPORT
} net_ipd_info;

typedef uint16_t net_reconn_intv;

typedef enum {
    NET_RECV_MODE_ACTIVE = 0,
    NET_RECV_MODE_PASSIVE
} net_recv_mode;

typedef struct {
    uint8_t enable;
    char type[7];
    char remote_host[128];
    uint16_t remote_port;
    uint16_t keep_alive;
    uint16_t local_port;
} net_trans_link;

typedef struct {
    uint16_t report_size;
    uint16_t transmit_size;
} net_sendl_cfg;

typedef struct {
    char ca_file[32];
    char cert_file[32];
    char key_file[32];
} net_ssl_conf_t;

typedef struct {
    net_work_mode work_mode;
    net_mux_mode mux_mode;
    net_tcp_opt tcp_opt[AT_NET_CLIENT_HANDLE_MAX];
    net_server_maxconn server_maxconn;
    net_server_timeout server_timeout;
    net_sntp_cfg sntp_cfg;
    net_sntp_intv sntp_intv;
    net_ipd_info ipd_info;
    net_reconn_intv reconn_intv;
    net_recv_mode recv_mode;
    net_trans_link trans_link;
    net_sendl_cfg sendl_cfg;
    uint8_t wips_enable;
    uint8_t ipv6_enable;
    net_ssl_conf_t sslconf[AT_NET_CLIENT_HANDLE_MAX];
    struct {
        ip_addr_t dns[DNS_MAX_SERVERS];
        uint8_t dns_isset;
    } dns;
}net_config;

extern net_config *at_net_config;

int at_net_config_init(void);

int at_net_config_save(const char *key);

int at_net_config_default(void);

#ifdef __cplusplus
}
#endif

#endif/* AT_NET_CONFIG_H */

