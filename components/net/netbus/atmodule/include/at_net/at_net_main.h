/**
  ******************************************************************************
  * @file    at_net_main.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_NET_MAIN_H
#define AT_NET_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/ip_addr.h"

#define ip4_addr_eq(addr1, addr2) ((addr1)->addr == (addr2)->addr)

#if LWIP_IPV6
#define ip6_addr_zone_eq(ip6addr1, ip6addr2) ((ip6addr1)->zone == (ip6addr2)->zone)
    /** Compare IPv6 addresses, ignoring zone information. To be used sparingly! */
#define ip6_addr_zoneless_eq(addr1, addr2) (((addr1)->addr[0] == (addr2)->addr[0]) && \
                                    ((addr1)->addr[1] == (addr2)->addr[1]) && \
                                    ((addr1)->addr[2] == (addr2)->addr[2]) && \
                                    ((addr1)->addr[3] == (addr2)->addr[3]))
#define ip6_addr_eq(addr1, addr2) (ip6_addr_zoneless_eq((addr1), (addr2)) && \
                                    ip6_addr_zone_eq((addr1), (addr2)))
/** @ingroup ipaddr
 *  Check if two ip addresses are equal. */
#define ip_addr_eq(addr1, addr2)    ((IP_GET_TYPE(addr1) != IP_GET_TYPE(addr2)) ? 0 : (IP_IS_V6_VAL(*(addr1)) ? \
  ip6_addr_eq(ip_2_ip6(addr1), ip_2_ip6(addr2)) : \
  ip4_addr_eq(ip_2_ip4(addr1), ip_2_ip4(addr2))))
#else /* LWIP_IPV6 */
#define ip_addr_eq(addr1, addr2)                ip4_addr_eq(addr1, addr2)
#endif

#define AT_NET_CLIENT_HANDLE_MAX 5
#define AT_NET_SERVER_HANDLE_MAX 1

int at_net_start(void);

int at_net_stop(void);

int at_net_sock_is_build(void);

int at_net_throuthput_udp_linktype(int linkid);

int at_net_client_get_valid_id(void);

static inline int at_net_client_id_is_valid(int id)
{
    if (id < 0 || id >= AT_NET_CLIENT_HANDLE_MAX)
        return 0;
    else
        return 1;
}

int at_net_client_tcp_connect(int id, ip_addr_t *remote_ip, uint16_t remote_port, int keepalive, uint32_t timeout);

int at_net_client_udp_connect(int id, ip_addr_t *remote_ip, uint16_t remote_port, uint16_t local_port, int mode, uint32_t timeout);

int at_net_client_ssl_connect(int id, ip_addr_t *remote_ip, uint16_t remote_port, int keepalive, uint32_t timeout);

int at_net_client_is_connected(int id);

int at_net_client_set_remote(int id, ip_addr_t *ipaddr, uint16_t port);

int at_net_client_get_info(int id, char *type, uint16_t len, ip_addr_t *remote_ip, uint16_t *remote_port, uint16_t *local_port, uint8_t *tetype);

int at_net_client_get_recvsize(int id);

int at_net_recvbuf_delete(int id);

int at_net_client_send(int id, void * buffer, int length);

int at_net_client_close(int id);

int at_net_client_close_all(void);

int at_net_server_tcp_create(uint16_t port, int max_conn, int timeout, uint8_t is_ipv6, int keepalive);

int at_net_server_ssl_create(uint16_t port, int max_conn, int timeout, int ca_enable, uint8_t is_ipv6, int keepalive);

int at_net_server_is_created(uint16_t *port, char *type, int *ca_enable, int *keepalive);

int at_net_server_close(void);

int at_net_sntp_start(void);

int at_net_sntp_stop(void);

int at_net_sntp_is_start(void);

int at_net_recvbuf_size_set(int linkid, uint32_t size);

int at_net_recvbuf_size_get(int linkid);

int at_net_recvbuf_read(int linkid, ip_addr_t *remote_ipaddr, uint16_t *remote_port, uint8_t *buf, uint32_t size);

int at_net_ssl_path_set(int linkid, const char *ca, const char *cert, const char *key);

int at_net_ssl_path_get(int linkid, const char **ca, const char **cert, const char **key);

int at_net_ssl_sni_set(int linkid, const char *sni);

char *at_net_ssl_sni_get(int linkid);

int at_net_ssl_alpn_set(int linkid, int alpn_num, const char *alpn);

char **at_net_ssl_alpn_get(int linkid, int *alpn_num);

int at_net_ssl_psk_set(int linkid, char *psk, int psk_len, char *pskhint, int pskhint_len);

int at_net_ssl_psk_get(int linkid, char **psk, int *psk_len, char **pskhint, int *pskhint_len);

int at_string_host_to_ip(char *host, ip_addr_t *ip);

int at_net_dns_load(void);

int at_lwip_heap_free_size(void);

int at_net_poll_start(int interval_ms);

#ifdef __cplusplus
}
#endif

#endif/* AT_NET_MAIN_H */

