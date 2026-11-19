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
#if defined(CONFIG_AT_MDNS_ENABLE) && CONFIG_AT_MDNS_ENABLE
#include "lwip/apps/mdns.h"
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

int at_net_server_sockets_close_all(void);

int at_net_server_udp_create(uint16_t port, int max_conn, int timeout, uint8_t is_ipv6);

int at_net_sntp_gettime(struct timespec *tp);

int at_net_sntp_start(void);

int at_net_sntp_stop(void);

int at_net_sntp_is_start(void);

#if defined(CONFIG_AT_MDNS_ENABLE) && CONFIG_AT_MDNS_ENABLE
#define AT_NET_MDNS_LABEL_MAX_LEN 63
#define AT_NET_MDNS_TXT_ITEM_MAX_NUM 5
#define AT_NET_MDNS_TXT_ITEM_MAX_LEN 64
#define AT_NET_MDNS_DOMAIN_MAX_LEN MDNS_DOMAIN_MAXLEN
#define AT_NET_MDNS_QUERY_RESULT_MAX 255
#define AT_NET_MDNS_QUERY_TIMEOUT_DEFAULT_MS 5000
#define AT_NET_MDNS_QUERY_TIMEOUT_MIN_MS     1000
#define AT_NET_MDNS_QUERY_TIMEOUT_MAX_MS     180000
#define AT_NET_MDNS_QUERY_RECORD_MAX         4

typedef enum {
    AT_NET_MDNS_QUERY_PTR = 0,
} at_net_mdns_query_type_t;

typedef struct {
    uint8_t running;
    char hostname[AT_NET_MDNS_LABEL_MAX_LEN + 1];
    char service[AT_NET_MDNS_LABEL_MAX_LEN + 1];
    char instance[AT_NET_MDNS_LABEL_MAX_LEN + 1];
    char proto[5];
    uint16_t port;
} at_net_mdns_state_t;

typedef struct {
    uint8_t netif;
    uint8_t ip_type;
    char ptr[AT_NET_MDNS_LABEL_MAX_LEN + 1];
    char srv[AT_NET_MDNS_DOMAIN_MAX_LEN];
    uint16_t port;
    uint8_t txt_num;
    char txt[AT_NET_MDNS_QUERY_RECORD_MAX][AT_NET_MDNS_TXT_ITEM_MAX_LEN];
    uint8_t a_num;
    char a[AT_NET_MDNS_QUERY_RECORD_MAX][16];
    uint8_t aaaa_num;
    char aaaa[AT_NET_MDNS_QUERY_RECORD_MAX][40];
} at_net_mdns_result_t;

int at_net_mdns_start(const char *hostname, const char *service, uint16_t port,
                      const char *instance, const char *proto,
                      const char txt_items[][AT_NET_MDNS_TXT_ITEM_MAX_LEN], uint8_t txt_count);

int at_net_mdns_stop(void);

int at_net_mdns_query(const char *service, const char *proto, int timeout_ms,
                      at_net_mdns_result_t *results, int max_results, int *result_count);

int at_net_mdns_get_state(at_net_mdns_state_t *state);
#endif

int at_net_recvbuf_size_set(int linkid, uint32_t size);

int at_net_recvbuf_size_get(int linkid);

int at_net_recvbuf_read(int linkid, ip_addr_t *remote_ipaddr, uint16_t *remote_port, uint8_t *buf, uint32_t size);

int at_net_ssl_path_set(int linkid, const char *ca, const char *cert, const char *key);

int at_net_ssl_path_get(int linkid, const char **ca, const char **cert, const char **key);

int at_net_ssl_server_path_set(int linkid, const char *ca, const char *cert, const char *key);

int at_net_ssl_server_path_get(int linkid, const char **ca, const char **cert, const char **key);

int at_net_ssl_sni_set(int linkid, const char *sni);

char *at_net_ssl_sni_get(int linkid);

int at_net_ssl_alpn_set(int linkid, int alpn_num, const char *alpn);

char **at_net_ssl_alpn_get(int linkid, int *alpn_num);

int at_net_ssl_psk_set(int linkid, char *psk, int psk_len, char *pskhint, int pskhint_len);

int at_net_ssl_psk_get(int linkid, char **psk, int *psk_len, char **pskhint, int *pskhint_len);

int at_net_ssl_server_psk_set(int linkid, char *psk, int psk_len, char *pskhint, int pskhint_len);

int at_net_ssl_server_psk_get(int linkid, char **psk, int *psk_len, char **pskhint, int *pskhint_len);

int at_string_host_to_ip(char *host, ip_addr_t *ip);

int at_net_dns_load(void);

int at_lwip_heap_free_size(void);

int at_net_poll_start(int interval_ms);

int at_net_ssl_enc_storage_init(void);

int at_net_ssl_save_file_encrypt(const char *path);

int at_net_ssl_load_file_decrypt(const char *path, char **buf, int *len);

int at_net_ssl_verify_credential_file(const char *path);

int at_net_ssl_verify_cert_key_pair(const char *cert_path, const char *key_path);

#ifdef __cplusplus
}
#endif

#endif/* AT_NET_MAIN_H */
