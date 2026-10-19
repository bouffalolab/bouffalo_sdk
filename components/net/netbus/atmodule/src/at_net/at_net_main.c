/**
  ******************************************************************************
  * @file    at_net_main.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <FreeRTOS.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
#include "lwip/ip_addr.h"
#if defined(CONFIG_AT_MDNS_ENABLE) && CONFIG_AT_MDNS_ENABLE
#include <lwip/apps/mdns.h>
#include <lwip/prot/dns.h>
#endif
#include <stream_buffer.h>
#include <at_net_sntp.h>
#include <wifi_mgmr_ext.h>
#include "at_pal.h"

#include "at_main.h"
#include "at_core.h"
#include "at_config.h"
#include "at_base_config.h"
#include "at_fs.h"
#include "at_net_main.h"
#include "at_net_config.h"
#include "at_net_ssl.h"
#include "at_wifi_config.h"
#include <lwip/dns.h>
#include "at_utils_crypto.h"
#include "utils_hex.h"

extern int at_wifi_sta_ip4_addr_get(uint32_t *ip, uint32_t *mask, uint32_t *gw, uint32_t *dns);

#define AT_LOCAL_LOOP_SOCKET_PORT  (9000)
#define AT_UDP_MAX_BUFFER_LEN      (1470)
#define AT_UDPV6_MAX_BUFFER_LEN    (1450)
#define AT_NET_TASK_STACK_SIZE     (1024)
#define AT_NET_TASK_PRIORITY_LOW   (27)
#define AT_NET_TASK_PRIORITY_HIGH  (28)
#define AT_NET_RECV_BUF_SIZE       (1536*3)
#define AT_NET_SEND_BUF_SIZE       (1024*8)
#define AT_NET_PRINTF              printf
#define AT_NET_DEBUG               printf

#define AT_NET_SSL_FILE_TAG        "ENC:"
#define AT_NET_SSL_FILE_TAG_LEN    4
#define AT_NET_SSL_FILE_IV_LEN     16
#define AT_NET_SSL_FILE_RSV_LEN    44
#define AT_NET_SSL_FILE_HEADER_LEN sizeof(at_net_ssl_file_header_t)

typedef struct {
    char    tag[AT_NET_SSL_FILE_TAG_LEN];
    uint8_t iv[AT_NET_SSL_FILE_IV_LEN];
    uint8_t reserved[AT_NET_SSL_FILE_RSV_LEN];
} __attribute__((packed)) at_net_ssl_file_header_t;

#define AT_NET_IPD_EVT_HEAD(s)      (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE)?s"\r\n":s","

#define CHECK_NET_CLIENT_ID_VALID(id) \
    if (!at_net_client_id_is_valid(id)) { \
        AT_NET_PRINTF("client id is invalid\r\n"); \
        return -1; \
    }

#define CHECK_NET_SERVER_ID_VALID(id) \
    if (!at_net_server_id_is_valid(id)) { \
        AT_NET_PRINTF("server id is invalid\r\n"); \
        return -1; \
    }

#define os_get_time_ms() ((xPortIsInsideInterrupt())?(xTaskGetTickCountFromISR()):(xTaskGetTickCount()))
#define udp_server_close(fd) udp_client_close(fd)

typedef enum {
    NET_CLIENT_TCP = 0,
    NET_CLIENT_UDP,
    NET_CLIENT_SSL,
} net_client_type;

typedef enum {
    NET_SERVER_TCP = 0,
	NET_SERVER_UDP,
    NET_SERVER_SSL,
} net_server_type;

typedef enum {
    NET_IPDINFO_CONNECTED = 0,
    NET_IPDINFO_DISCONNECTED,
    NET_IPDINFO_RECVDATA
} net_ipdinfo_type;

typedef struct {
    int valid;
    net_client_type type;
    int fd;
    void *priv;

    ip_addr_t remote_ip;
    uint16_t remote_port;
    uint16_t local_port;
    uint8_t udp_mode;
    uint8_t tetype;
    uint16_t recv_timeout;
    uint32_t recv_time;
    uint32_t disconnect_time;

    int keep_alive;
    int so_linger;
    int tcp_nodelay;
    int so_sndtimeo;
    StreamBufferHandle_t recv_buf;
    uint32_t recvbuf_size;

    char ca_path[32];
    char cert_path[32];
    char priv_key_path[32];
    char *ssl_hostname;
    char *ssl_alpn[6];
    int ssl_alpn_num;
    char ssl_psk[32];
    char ssl_pskhint[32];
    uint8_t ssl_psklen;
    uint8_t ssl_pskhint_len;
    uint8_t socket_accept;
    uint8_t retry_cnt;
}at_net_client_handle;

typedef struct {
    int valid;
    net_server_type type;
    int fd;

    uint16_t port;
    uint16_t recv_timeout;
    uint8_t ca_enable;
    int8_t client_max;
    int8_t client_num;
    int keepalive;

    uint8_t is_ipv6;
    char ca_path[32];
    char cert_path[32];
    char priv_key_path[32];
    char ssl_psk[32];
    char ssl_pskhint[32];
    uint8_t ssl_psklen;
    uint8_t ssl_pskhint_len;
}at_net_server_handle;

static at_net_client_handle *g_at_client_handle = NULL;
static at_net_server_handle *g_at_server_handle = NULL;
static SemaphoreHandle_t net_mutex;
static TimerHandle_t net_timer;
static uint8_t g_at_net_task_is_start = 0;
static uint8_t g_at_net_sntp_is_start = 0;
static char g_at_net_savelink_host[128];
static int wake_socket_fd = -1;

static int net_main_wakeup(void);
static int net_socket_accept(int fd, int type, uint16_t port, uint16_t timeout, uint8_t num, uint8_t max_conn, uint8_t is_ipv6, int keepalive, bool udp_force);

static void sockaddr_to_ipaddr(const struct sockaddr *sa, ip_addr_t *ipaddr)
{
    if (sa->sa_family == AF_INET) {
        const struct sockaddr_in *sa_in = (const struct sockaddr_in *)sa;
        ip4_addr_set_u32(ip_2_ip4(ipaddr), sa_in->sin_addr.s_addr);
        IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4);
    }
#if defined(CFG_IPV6) && CFG_IPV6
    else if (sa->sa_family == AF_INET6) {
        const struct sockaddr_in6 *sa_in6 = (const struct sockaddr_in6 *)sa;
        ip6_addr_set(ip_2_ip6(ipaddr), (const ip6_addr_t *)(&sa_in6->sin6_addr));
        IP_SET_TYPE(ipaddr, IPADDR_TYPE_V6);
    }
#endif
    else {
        AT_NET_PRINTF("Unsupported address family: %d\n", sa->sa_family);
    }
}

static int ipaddr_lookup(const ip_addr_t *addr, uint16_t port)
{
    int i;
    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        //printf("ipaddr_lookup id:%d %s:%d\r\n",i, ipaddr_ntoa(&g_at_client_handle[i].remote_ip), g_at_client_handle[i].remote_port);
        if (g_at_client_handle[i].valid && ip_addr_cmp(&g_at_client_handle[i].remote_ip, addr) && (port == g_at_client_handle[i].remote_port)) {
            return i;
        }
    }
    return -1;
}

static int ip_multicast_enable(int fd, ip_addr_t *ipaddr)
{
    if (fd < 0) {
        return -1;
    }
    if (!ip_addr_ismulticast(ipaddr)) {
        return -1;
    }
#if LWIP_IGMP
    char optval;
    /* Close multicast loop. */
    optval =0;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &optval, sizeof(optval))) {
        AT_NET_PRINTF("sock set multicast loop failed\r\n");
        return -1;
    }

    /* Set multicast interface. */
#if defined(CFG_IPV6) && CFG_IPV6
    if (IP_IS_V6(ipaddr)) {
        struct in6_addr addr6;
        memset((void *)&addr6, 0, sizeof(struct in6_addr));
        addr6 = in6addr_any;
        if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF,(char *)&addr6, sizeof(addr6))) {
            AT_NET_PRINTF("sock set multicast interface failed\r\n");
            return -1;
        }
    } else
#endif
    {
        struct in_addr addr;
        memset((void *)&addr, 0, sizeof(struct in_addr));
        addr.s_addr = htonl(INADDR_ANY);
        if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF,(char *)&addr, sizeof(addr))) {
            AT_NET_PRINTF("sock set multicast interface failed\r\n");
            return -1;
        }
    }

    /* Setup time-to-live. */
    optval = 10; /* Hop count */
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &optval, sizeof(optval))) {
        AT_NET_PRINTF("sock set multicast ttl failed\r\n");
        return -1;
    }

    /* Add membership to receiving socket. */
#if defined(CFG_IPV6) && CFG_IPV6
    if (IP_IS_V6(ipaddr)) {
        struct ipv6_mreq mreq6;
        memset(&mreq6, 0, sizeof(struct ipv6_mreq));
        mreq6.ipv6mr_interface = 0;
        memcpy(&mreq6.ipv6mr_multiaddr, ipaddr, sizeof(mreq6.ipv6mr_multiaddr));
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq6, sizeof(mreq6))) {
            AT_NET_PRINTF("sock set add membership failed\r\n");
            return -1;
        }
    } else
#endif
    {
        struct ip_mreq mreq;
        memset(&mreq, 0, sizeof(struct ip_mreq));
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        mreq.imr_multiaddr.s_addr = ip4_addr_get_u32(ip_2_ip4(ipaddr));
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, sizeof(mreq))) {
            AT_NET_PRINTF("sock set add membership failed\r\n");
            return -1;
        }
    }

#endif
    return 0;
}

static int so_keepalive_enable(int fd, int idle, int interval, int count)
{
    int keepAlive = 1;
    int keepIdle = idle;
    int keepInterval = interval;
    int keepCount = count;

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive))) {
        AT_NET_PRINTF("sock enable tcp keepalive failed\r\n");
        return -1;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle))) {
        AT_NET_PRINTF("sock set tcp keepidle failed\r\n");
        return -1;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval))) {
        AT_NET_PRINTF("sock set tcp keepintvl failed\r\n");
        return -1;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount))) {
        AT_NET_PRINTF("sock set tcp keepcnt failed\r\n");
        return -1;
    }

    return 0;
}

static int so_keepalive_disable(int fd)
{
    int keepAlive = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive))) {
        AT_NET_PRINTF("sock disable tcp keepalive failed\r\n");
        return -1;
    }

    return 0;
}

static int so_linger_enable(int fd, int linger)
{
    struct linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = linger;
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger))) {
        AT_NET_PRINTF("sock set enable linger failed\r\n");
        return -1;
    }

    return 0;
}

static int so_linger_disable(int fd)
{
    struct linger so_linger;
    so_linger.l_onoff = 0;
    so_linger.l_linger = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger))) {
        AT_NET_PRINTF("sock set disable linger failed\r\n");
        return -1;
    }

    return 0;
}

static int so_sndtimeo_enable(int fd, int timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = timeout_ms/1000;
    tv.tv_usec = (timeout_ms%1000)*1000;

    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*) &tv, sizeof(struct timeval))) {
        AT_NET_PRINTF("sock enable send timeout failed\r\n");
        return -1;
    }

    return 0;
}

static int so_localport_get(int fd)
{
    struct sockaddr_in local_addr;
    int n = sizeof(local_addr);

    getsockname(fd, (struct sockaddr *) &local_addr, (socklen_t *)&n);
    return ntohs(local_addr.sin_port);
}

static int __attribute__((unused)) so_recvsize_get(int fd)
{
    int bytes;

    if (ioctl(fd, FIONREAD, &bytes) != 0)
        return -1;
    return bytes;
}

static int tcp_nodelay_enable(int fd)
{
    int on = 1;

    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on))) {
        AT_NET_PRINTF("sock set enable tcp nodelay\r\n");
        return -1;
    }

    return 0;
}

static int tcp_nodelay_disable(int fd)
{
    int on = 0;

    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on))) {
        AT_NET_PRINTF("sock set disable tcp nodelay\r\n");
        return -1;
    }

    return 0;
}

#define UDP_LOCALPORT_BASE 50000
#define UDP_LOCALPORT_RANGE 10000
static uint16_t udp_localport_rand(void)
{
    uint16_t port = UDP_LOCALPORT_BASE + ((random()) % UDP_LOCALPORT_RANGE);
    return port;
}

static void net_timer_callback_func(TimerHandle_t xTimer)
{
    net_main_wakeup();
    at_net_poll_start(at_net_config->server_timeout * 1000);
}

static inline void poll_timeout_update(int id)
{
    g_at_client_handle[id].recv_time = os_get_time_ms();
    if (g_at_client_handle[id].valid && g_at_client_handle[id].recv_timeout > 0) {
        xTimerStop(net_timer, portMAX_DELAY);
        at_net_poll_start(at_net_config->server_timeout * 1000);
    }
}

static int tcp_client_connect(const ip_addr_t *ipaddr, uint16_t port, uint32_t timeout)
{
    int fd;
    int res;

    //struct hostent *hostinfo;
    struct sockaddr_in addr;

    AT_NET_PRINTF("tcp client connect %s:%d\r\n", ipaddr_ntoa(ipaddr), port);
    /*hostinfo = gethostbyname(host);
    if (!hostinfo) {
        AT_NET_PRINTF("gethostbyname failed\r\n");
        return -1;
    }*/

#if defined(CFG_IPV6) && CFG_IPV6
    struct sockaddr_in6 addr6 = {0};
    if(IP_IS_V6(ipaddr)) {
        if ( (fd =  socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6))  < 0) {
            AT_NET_PRINTF("socket create failed\r\n");
            return -AT_SUB_CMD_EXEC_FAIL;
        }

        memcpy(&addr6.sin6_addr, ipaddr, sizeof(addr6.sin6_addr));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
        if (ip6_addr_has_zone(ip_2_ip6(ipaddr))) {
            addr6.sin6_scope_id = ip6_addr_zone(ip_2_ip6(ipaddr));
        }
    } else
#endif
    {
        if ( (fd =  socket(AF_INET, SOCK_STREAM, 0))  < 0) {
            AT_NET_PRINTF("socket create failed\r\n");
            return -AT_SUB_CMD_EXEC_FAIL;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_len = sizeof(addr);
        addr.sin_port = htons(port);
        //addr.sin_addr.s_addr = ((struct in_addr *) hostinfo->h_addr)->s_addr;
        addr.sin_addr.s_addr = ip_addr_get_ip4_u32(ipaddr);
    }

    int on= 1;
    res = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    if (res != 0) {
        AT_NET_PRINTF("setsockopt failed, res:%d\r\n", res);
        res = -AT_SUB_CMD_EXEC_FAIL;
        goto _fail;
    }

    if (timeout) {

        int flag;
        flag = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
#if defined(CFG_IPV6) && CFG_IPV6
        if(IP_IS_V6(ipaddr)) {
            res = connect(fd, (struct sockaddr *)&addr6, sizeof(addr6));
        } else
#endif
        {
            res = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
        }

        if (res < 0 && errno != EINPROGRESS) {
            res = -AT_SUB_CMD_EXEC_FAIL;
            goto _fail;
        }

        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(fd, &writefds);

        struct timeval tv;
        tv.tv_sec = timeout/1000;
        tv.tv_usec = (timeout%1000)*1000;

        res = select(fd + 1, NULL, &writefds, NULL, &tv);
        if (res == 0) {
            res = -AT_SUB_TIMEOUT;
            goto _fail;
        }
        if (res < 0) {
            res = -AT_SUB_CMD_EXEC_FAIL;
            goto _fail;
        }

        socklen_t len = sizeof(res);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &res, &len);
        if (res != 0) {
            res = -AT_SUB_CMD_EXEC_FAIL;
            goto _fail;
        }
        fcntl(fd, F_SETFL, flag);
    } else {
#if defined(CFG_IPV6) && CFG_IPV6
        if(IP_IS_V6(ipaddr)) {
            res = connect(fd, (struct sockaddr *)&addr6, sizeof(addr6));
        } else
#endif
        {
            res = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
        }

        if (res < 0) {
            res = -AT_SUB_CMD_EXEC_FAIL;
            goto _fail;
        }
    }
    return fd;

_fail:
    AT_NET_PRINTF("connect failed:%d\r\n", res);
    close(fd);
    return res;
}

static int tcp_connected()
{
    int i;
    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (at_net_client_is_connected(i)) {
            if (g_at_client_handle[i].type != NET_CLIENT_UDP) {
                return 1;
            }
        }
    }
    return 0;
}

static int tcp_client_close(int fd)
{
    AT_NET_PRINTF("tcp client close\r\n");
    if (fd >= 0) {
        close(fd);
    }

    return 0;
}

static int tcp_client_send(int fd, void *buffer, int length)
{
    if (fd >= 0) {
        return send(fd, buffer, length, 0);
    }
    return 0;
}

static int udp_client_connect(uint16_t port, ip_addr_t *ipaddr)
{
    int fd;
    unsigned char loop= 0;
    int so_broadcast=1;
#if defined(CFG_IPV6) && CFG_IPV6
    if (IP_IS_V6(ipaddr)) {
        if ( (fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
            return -AT_SUB_CMD_EXEC_FAIL;
        }
    } else
#endif
    {
        if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            return -AT_SUB_CMD_EXEC_FAIL;
        }
    }
    AT_NET_PRINTF("udp client connect port %d\r\n", port);

    setsockopt(fd, SOL_SOCKET,SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

#if defined(CFG_IPV6) && CFG_IPV6
    if (IP_IS_V6(ipaddr)) {
        struct sockaddr_in6 addr6;
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        memcpy(&addr6.sin6_addr, ipaddr, sizeof(addr6.sin6_addr));
        addr6.sin6_port = htons(port);
        addr6.sin6_addr = in6addr_any;
        if (ip6_addr_has_zone(ip_2_ip6(ipaddr))) {
            addr6.sin6_scope_id = ip6_addr_zone(ip_2_ip6(ipaddr));
        }
        if(bind(fd, (struct sockaddr*)&addr6, sizeof(addr6)) < 0) {
            close(fd);
            return -AT_SUB_OP_ADDR_ERROR;
        }
    } else
#endif
    {
        struct sockaddr_in addr;
        memset(&addr, 0,  sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(fd);
            return -AT_SUB_OP_ADDR_ERROR;
        }

    }

    return fd;
}

static int udp_client_close(int fd)
{
    AT_NET_PRINTF("udp client close\r\n");
    if (fd >= 0) {
        close(fd);
    }

    return 0;
}

static int udp_client_send(int fd, void *buffer, int length, ip_addr_t *ipaddr, uint16_t port)
{
    int ret;
    uint32_t send_len = 0;

    if (fd >= 0) {
#if defined(CFG_IPV6) && CFG_IPV6
        if (IP_IS_V6(ipaddr)) {
            struct sockaddr_in6 toaddr6;
            memset(&toaddr6, 0, sizeof(toaddr6));
            toaddr6.sin6_family = AF_INET6;
            memcpy(&toaddr6.sin6_addr, ipaddr, sizeof(toaddr6.sin6_addr));
            toaddr6.sin6_port = htons(port);
            if (ip6_addr_has_zone(ip_2_ip6(ipaddr))) {
                toaddr6.sin6_scope_id = ip6_addr_zone(ip_2_ip6(ipaddr));
            }
            while (send_len < length) {

                ret = sendto(fd, ((uint8_t *)buffer) + send_len, (length - send_len > AT_UDPV6_MAX_BUFFER_LEN) ? AT_UDPV6_MAX_BUFFER_LEN : (length - send_len),
                        0, (struct sockaddr *)&toaddr6, sizeof(struct sockaddr_in6));
                if (ret <= 0) {
                    break;
                }
                send_len += ret;
            }
        } else
#endif
        {
            struct sockaddr_in toaddr;
            memset(&toaddr, 0, sizeof(toaddr));
            toaddr.sin_family = AF_INET;
            toaddr.sin_addr.s_addr = ip_addr_get_ip4_u32(ipaddr);
            toaddr.sin_port = htons(port);

            while (send_len < length) {

                ret = sendto(fd, ((uint8_t *)buffer) + send_len, (length - send_len > AT_UDP_MAX_BUFFER_LEN) ? AT_UDP_MAX_BUFFER_LEN : (length - send_len),
                        0, (struct sockaddr *)&toaddr, sizeof(struct sockaddr_in));
                if (ret <= 0) {
                    break;
                }
                send_len += ret;
            }
        }

        return send_len;
        //printf("sendto ret:%d len:%d\r\n", ret, length);
    }
    return 0;
}

static int ssl_client_connect(int id, ip_addr_t *ipaddr, uint16_t port, void **priv, uint32_t timeout)
{
    int fd;
    void *handle;
    ssl_conn_param_t ssl_param = {0};
    struct timeval tv;

    AT_NET_PRINTF("ssl client connect %s:%d\r\n", ipaddr_ntoa(ipaddr), port);

    fd = tcp_client_connect(ipaddr, port, timeout);
    if (fd < 0) {
        AT_NET_PRINTF("tcp_client_connect fd:%d\r\n", fd);
        return fd;
    }

    at_load_file(g_at_client_handle[id].ca_path, &ssl_param.ca_cert, &ssl_param.ca_cert_len);
    at_load_file(g_at_client_handle[id].cert_path, &ssl_param.own_cert, &ssl_param.own_cert_len);
    at_net_ssl_load_file_decrypt(g_at_client_handle[id].priv_key_path, &ssl_param.private_cert, &ssl_param.private_cert_len);

    ssl_param.alpn = at_net_ssl_alpn_get(id, &ssl_param.alpn_num);
    at_net_ssl_psk_get(id, &ssl_param.psk, &ssl_param.psk_len, &ssl_param.pskhint, &ssl_param.pskhint_len);
    ssl_param.sni = at_net_ssl_sni_get(id);

    tv.tv_sec = timeout/1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    handle = mbedtls_ssl_connect(fd, &ssl_param);
    if (handle == NULL) {
        AT_NET_PRINTF("mbedtls_ssl_connect handle NULL, fd:%d\r\n", fd);
        close(fd);
        at_free(ssl_param.ca_cert);
        at_free(ssl_param.own_cert);
        at_free(ssl_param.private_cert);
        return -AT_SUB_CMD_EXEC_FAIL;
    }

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    *priv = handle;
    at_free(ssl_param.ca_cert);
    at_free(ssl_param.own_cert);
    at_free(ssl_param.private_cert);

    return fd;
}

static int ssl_client_close(int fd, void *priv)
{
    AT_NET_PRINTF("ssl client close\r\n");
    if (priv)
        mbedtls_ssl_close(priv);
    if (fd >= 0)
        close(fd);

    return 0;
}

static int ssl_client_send(int fd, void *buffer, int length, void *priv)
{
    if (fd >= 0 && priv)
        return mbedtls_ssl_send(priv, buffer, length);

    return 0;
}

static int udp_server_create(uint16_t port, int listen, uint8_t is_ipv6)
{
    int fd;
    struct sockaddr_in servaddr;
    int on;
    struct timeval timeout = { 0 };

    AT_NET_PRINTF("udp server create port %d\r\n", port);

#if defined(CFG_IPV6) && CFG_IPV6
    struct sockaddr_in6 addr6 = {0};
    if (is_ipv6) {
        if ( (fd =  socket(AF_INET6, SOCK_DGRAM, 0))  < 0) {
            AT_NET_PRINTF("socket create failed\r\n");
            return -2;
        }

        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons(port);
        //TODO: By default, this is associated with the STA netif.
        //Note: This modification will no longer be applicable when IPv6 is supported in AP mode.
        addr6.sin6_scope_id = netif_get_index(netif_find("wl1"));

    } else
#endif
    {
        if ( (fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
            return -1;
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);
    }

#if defined(CFG_IPV6) && CFG_IPV6
    if (is_ipv6) {
        on= 0;
        setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
    }
#endif

    on= 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#if defined(CFG_IPV6) && CFG_IPV6
    if (is_ipv6) {
        if (bind(fd, (struct sockaddr*)&addr6, sizeof(addr6)) < 0) {
            close(fd);
            return -2;
        }
    } else
#endif
    {
        if (bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            close(fd);
            return -2;
        }
    }

    timeout.tv_sec = 5;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    return fd;
}

static int tcp_server_create(uint16_t port, int listen, uint8_t is_ipv6)
{
    int fd;
    struct sockaddr_in servaddr;
    int on;

    AT_NET_PRINTF("tcp server create port %d\r\n", port);

#if defined(CFG_IPV6) && CFG_IPV6
    struct sockaddr_in6 addr6 = {0};
    if (is_ipv6) {
        if ( (fd =  socket(AF_INET6, SOCK_STREAM, 0))  < 0) {
            AT_NET_PRINTF("socket create failed\r\n");
            return -2;
        }

        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons(port);
        //TODO: By default, this is associated with the STA netif.
        //Note: This modification will no longer be applicable when IPv6 is supported in AP mode.
        addr6.sin6_scope_id = netif_get_index(netif_find("wl1"));
    } else
#endif
    {
        if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return -1;
        }
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);
    }

    on= 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#if defined(CFG_IPV6) && CFG_IPV6
    if (is_ipv6) {
        if (bind(fd, (struct sockaddr*)&addr6, sizeof(addr6)) < 0) {
            close(fd);
            return -2;
        }
    } else
#endif
    {
        if (bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            close(fd);
            return -2;
        }
    }

    if (listen(fd, listen) < 0) {
        close(fd);
        return -3;
    }

    return fd;
}

static int tcp_server_close(int fd)
{
    AT_NET_PRINTF("tcp server close\r\n");
    if (fd >= 0) {
        close(fd);
    }

    return 0;
}

static inline int net_socket_fd_close(int fd, net_client_type type, bool tetype, void *priv)
{
    if (type == NET_CLIENT_TCP)
       tcp_client_close(fd);
    else if (type == NET_CLIENT_UDP && (!tetype))
       udp_client_close(fd);
    else if (type == NET_CLIENT_SSL)
       ssl_client_close(fd, priv);
    return 0;
}

static void net_lock(void)
{
    xSemaphoreTake(net_mutex, portMAX_DELAY);
}

static void net_unlock(void)
{
    xSemaphoreGive(net_mutex);
}

static int net_is_active(void)
{
    if (!at_wifi_config) {
        return 0;
    }

    if (at_wifi_config->wifi_mode == WIFI_STATION_MODE || at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
        uint32_t addr = 0;
    	at_wifi_sta_ip4_addr_get(&addr, NULL, NULL, NULL);
    	if (addr != IPADDR_ANY) {
    		return 1;
    	}
    } else if (at_wifi_config->wifi_mode == WIFI_SOFTAP_MODE) {

    }

    return 0;
}

static int net_main_wakeup(void)
{
    ip_addr_t loopback_addr;
    IP_ADDR4(&loopback_addr, 127, 0, 0, 1);

    return udp_client_send(wake_socket_fd, "wake", 4, &loopback_addr, AT_LOCAL_LOOP_SOCKET_PORT);
}

static int select_wake_clear(void)
{
    struct sockaddr_in remote_addr;
    int len = sizeof(remote_addr);
    int ret = -1;
    uint8_t clear_buf[10] = {0};

    ret = recvfrom(wake_socket_fd,
    		clear_buf,
            sizeof(clear_buf),
            0,
            (struct sockaddr *)&remote_addr,
            (socklen_t *)(&len));
    return ret;
}

static int net_socket_ipd(net_ipdinfo_type ipd, int id, void *buffer, int length, ip_addr_t *ipaddr, uint16_t port, uint32_t timeout)
{
    char tmp[10];
    char ipd_evt[64] = {"+IPD:"};

    if (ipd == NET_IPDINFO_CONNECTED) {

        net_main_wakeup();

        if (tcp_connected()) {
            vTaskPrioritySet(NULL, AT_NET_TASK_PRIORITY_LOW);
        } else {
            vTaskPrioritySet(NULL, AT_NET_TASK_PRIORITY_HIGH);
        }

        if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT ||  at_base_config->sysmsg_cfg.bit.link_state_msg) {
            if (at_base_config->sysmsg_cfg.bit.link_msg_type) {
                char type[8] = {0};
                uint16_t local_port = 0;
                uint8_t tetype = 0;

                at_net_client_get_info(id, type, 8, NULL, NULL, &local_port, &tetype);
                if (at_net_config->wips_enable) {
                    at_response_string("+LINK_CONN:%d,%d,\"%s\",%d,\"%s\",%d,%d\r\n", 0, id, type, tetype, ipaddr_ntoa(ipaddr), port, local_port);
                }
            }
            else {
                if (at_net_config->wips_enable) {
                    if (at_net_config->mux_mode == NET_LINK_SINGLE)
                        at_response_string("+CIP:CONNECTED\r\n");
                    else
                        at_response_string("+CIP:%d,CONNECTED\r\n", id);
                }
            }
        }
    }
    else if (ipd == NET_IPDINFO_DISCONNECTED) {
        if (at_net_config->wips_enable) {
            if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT ||  at_base_config->sysmsg_cfg.bit.link_state_msg) {
                if (at_net_config->mux_mode == NET_LINK_SINGLE)
                    at_response_string("+CIP:DISCONNECTED\r\n");
                else
                    at_response_string("+CIP:%d,DISCONNECTED\r\n", id);
            }
        }
    }

    else if (ipd == NET_IPDINFO_RECVDATA) {
        if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT && at_net_config->wips_enable) {

            if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
                xStreamBufferSend(g_at_client_handle[id].recv_buf, buffer, length, timeout);
            }

            if (at_net_config->ipd_info == NET_IPDINFO_DISABLE_IPPORT) {
                if (at_net_config->mux_mode == NET_LINK_SINGLE) {
                    /* Use strncat + itoa instead of snprintf to improve performance. */
                    strncat(ipd_evt, itoa(length, tmp, 10), sizeof(ipd_evt) - strlen(ipd_evt));
                    strncat(ipd_evt, AT_NET_IPD_EVT_HEAD(""), sizeof(ipd_evt) - strlen(ipd_evt));
                    AT_CMD_DATA_SEND(ipd_evt, strlen(ipd_evt));
                } else {
                    /* Use strncat + itoa instead of snprintf to improve performance. */
                    strncat(ipd_evt, itoa(id, tmp, 10), sizeof(ipd_evt) - strlen(ipd_evt));
                    strncat(ipd_evt, ",", sizeof(ipd_evt) - strlen(ipd_evt));
                    strncat(ipd_evt, itoa(length, tmp, 10), sizeof(ipd_evt) - strlen(ipd_evt));
                    strncat(ipd_evt, AT_NET_IPD_EVT_HEAD(""), sizeof(ipd_evt) - strlen(ipd_evt));
                    AT_CMD_DATA_SEND(ipd_evt, strlen(ipd_evt));
                }
            } else {

                if (at_net_config->mux_mode == NET_LINK_SINGLE)
                    at_write(AT_NET_IPD_EVT_HEAD("+IPD:%d,\"%s\",%d"), length, ipaddr_ntoa(ipaddr), port);
                else
                    at_write(AT_NET_IPD_EVT_HEAD("+IPD:%d,%d,\"%s\",%d"), id, length, ipaddr_ntoa(ipaddr), port);
            }
            if (at_net_config->recv_mode != NET_RECV_MODE_PASSIVE) {
                memcpy((char *)buffer + length, "\r\n", 2);
                length += 2;
                AT_CMD_DATA_SEND(buffer, length);
            }
        } else if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
            xStreamBufferSend(g_at_client_handle[id].recv_buf, buffer, length, timeout);
        } else {
            if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT) {
                memcpy((char *)buffer + length, "\r\n", 2);
                length += 2;
            }
            AT_CMD_DATA_SEND(buffer, length);
        }
    }

    return 0;
}

static int net_socket_setopt(int fd, int keepalive, int so_linger, int tcp_nodelay, int sndtimeo)
{
    if (fd < 0)
        return -1;

    if (keepalive)
        so_keepalive_enable(fd, keepalive, 1, 3);
    else
        so_keepalive_disable(fd);

    if (so_linger>= 0)
        so_linger_enable(fd, so_linger);
    else
        so_linger_disable(fd);

    if (tcp_nodelay)
        tcp_nodelay_enable(fd);
    else
        tcp_nodelay_disable(fd);

    if (sndtimeo)
        so_sndtimeo_enable(fd, sndtimeo);

    return 0;
}

static int net_socket_connect(int id, net_client_type type, ip_addr_t *ipaddr, uint16_t port, int keepalive, uint16_t local_port, int mode, uint32_t timeout)
{
    int fd;
    void *priv = NULL;
    int keep_alive = 0;
    int so_linger = -1;
    int tcp_nodelay = 0;
    int so_sndtimeo = 0;

    if (g_at_client_handle[id].valid) {
        AT_NET_PRINTF("already connected\r\n");
        return -AT_SUB_HANDLE_INVALID;
    }

    if (type == NET_CLIENT_TCP) {
        fd = tcp_client_connect(ipaddr, port, timeout);
        AT_NET_PRINTF("tcp_client_connect fd:%d\r\n", fd);
        if (fd >= 0) {
            keep_alive = (keepalive>0)?keepalive:at_net_config->tcp_opt[id].keep_alive;
            so_linger = at_net_config->tcp_opt[id].so_linger;
            tcp_nodelay = at_net_config->tcp_opt[id].tcp_nodelay;
            so_sndtimeo = at_net_config->tcp_opt[id].so_sndtimeo;
            local_port = so_localport_get(fd);

            net_socket_setopt(fd, keep_alive, so_linger, tcp_nodelay, so_sndtimeo);
        }
    }
    else if (type == NET_CLIENT_UDP) {
        if (local_port == 0)
            local_port = udp_localport_rand();
        fd = udp_client_connect(local_port, ipaddr);
        ip_multicast_enable(fd, ipaddr);
    }
    else if (type == NET_CLIENT_SSL) {
        fd = ssl_client_connect(id, ipaddr, port, &priv, timeout);
        if (fd >= 0) {
            if (keepalive) {
                so_keepalive_enable(fd, keepalive, 1, 3);
                keep_alive = keepalive;
            }

            local_port = so_localport_get(fd);
        }
    }
    else {
        return -AT_SUB_PARA_PARSE_FAIL;
    }

    if (fd < 0) {
        AT_NET_PRINTF("net_socket_connect connect failed, fd:%d\r\n", fd);
        return (-fd);
    }

    net_lock();

    if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
        if (g_at_client_handle[id].recv_buf) {
            xStreamBufferReset(g_at_client_handle[id].recv_buf);
        } else {
            g_at_client_handle[id].recv_buf = xStreamBufferCreate(g_at_client_handle[id].recvbuf_size, 1);
            if (!g_at_client_handle[id].recv_buf) {
                net_unlock();
                net_socket_fd_close(fd, type, 0, priv);
                return -AT_SUB_NO_MEMORY;
            }
        }
    }
    g_at_client_handle[id].valid = 1;
    g_at_client_handle[id].type = type;
    g_at_client_handle[id].fd = fd;
    g_at_client_handle[id].socket_accept = 0;
    g_at_client_handle[id].retry_cnt = 0;
    g_at_client_handle[id].priv = priv;
    g_at_client_handle[id].remote_ip = *ipaddr;
    g_at_client_handle[id].remote_port = port;
    g_at_client_handle[id].local_port = local_port;
    g_at_client_handle[id].udp_mode = mode;
    g_at_client_handle[id].tetype = 0;
    g_at_client_handle[id].recv_timeout = 0;
    g_at_client_handle[id].recv_time = 0;
    g_at_client_handle[id].keep_alive = keep_alive;
    g_at_client_handle[id].so_linger = so_linger;
    g_at_client_handle[id].tcp_nodelay = tcp_nodelay;
    g_at_client_handle[id].so_sndtimeo = so_sndtimeo;

    net_unlock();
    net_socket_ipd(NET_IPDINFO_CONNECTED, id, NULL, 0, &g_at_client_handle[id].remote_ip, g_at_client_handle[id].remote_port, 0);
    return 0;
}

static void net_socket_close_sync(int evtid, void *arg)
{
	int id = (int)arg;
    int fd = g_at_client_handle[id].fd;
    net_client_type type = g_at_client_handle[id].type;
    void *priv = g_at_client_handle[id].priv;

    int valid = g_at_client_handle[id].valid;
    int servid = 0;

    if (!valid) {
        AT_NET_PRINTF("socket is not inited\r\n");
        return;
    }

    net_lock();
    if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT) {
        g_at_client_handle[id].valid = 0;
    }
    g_at_client_handle[id].fd = -1;
    g_at_client_handle[id].socket_accept = 0;
    g_at_client_handle[id].retry_cnt = 0;
    g_at_client_handle[id].priv = NULL;
    g_at_client_handle[id].disconnect_time = os_get_time_ms();

    if (g_at_client_handle[id].tetype && g_at_server_handle[servid].valid) {
        g_at_server_handle[servid].client_num--;
        if (g_at_server_handle[servid].client_num < 0)
            g_at_server_handle[servid].client_num = 0;
    }
    if (at_net_client_get_recvsize(id) == 0 && g_at_client_handle[id].recv_buf) {
        vStreamBufferDelete(g_at_client_handle[id].recv_buf);
        g_at_client_handle[id].recv_buf = NULL;
    }
    net_unlock();

    net_socket_fd_close(fd, type, g_at_client_handle[id].tetype, priv);
    net_socket_ipd(NET_IPDINFO_DISCONNECTED, id, NULL, 0, 0, 0, 0);
}

static int net_socket_close(int id)
{
    struct at_workq wq = {
        .pfunc = net_socket_close_sync,
        .arg = (void*)id,
    };

    at_workq_send(AT_EVENT_SOCKET_CLOSE, &wq, portMAX_DELAY);
    return 0;
}

static void wake_socket_close_sync(int evtid, void *arg)
{
    if (wake_socket_fd >= 0) {
        close(wake_socket_fd);
        wake_socket_fd = -1;
    }
}
static int wake_socket_close(void)
{
    struct at_workq wq = {
        .pfunc = wake_socket_close_sync,
        .arg = (void*)NULL,
    };

    at_workq_send(AT_EVENT_SOCKET_CLOSE, &wq, portMAX_DELAY);
    return 0;
}

static int net_socket_send(int id, void *buffer, int length)
{
    int ret = 0;

    int valid = g_at_client_handle[id].valid;
    int type = g_at_client_handle[id].type;
    int fd = g_at_client_handle[id].fd;
    void *priv = g_at_client_handle[id].priv;
    ip_addr_t remote_ip = g_at_client_handle[id].remote_ip;
    uint16_t remote_port = g_at_client_handle[id].remote_port;

    if (!valid)
        return -1;

    if (type == NET_CLIENT_TCP)
       ret = tcp_client_send(fd, buffer, length);
    else if (type == NET_CLIENT_UDP)
       ret = udp_client_send(fd, buffer, length, &remote_ip, remote_port);
    else if (type == NET_CLIENT_SSL)
       ret = ssl_client_send(fd, buffer, length, priv);

    poll_timeout_update(id);
    if (ret != length) {
        return -1;
    }
    return ret;
}

static int net_socket_recv(int id)
{
    int num = 0;
    struct sockaddr_storage remote_addr;
    uint16_t r_port;
    int len = sizeof(remote_addr);
    static char at_net_recv_buf[AT_NET_RECV_BUF_SIZE + 2];

    int type = g_at_client_handle[id].type;
    int fd = g_at_client_handle[id].fd;
    void *priv = g_at_client_handle[id].priv;
    ip_addr_t remote_ip = g_at_client_handle[id].remote_ip;
    uint16_t remote_port = g_at_client_handle[id].remote_port;
    int udp_mode = g_at_client_handle[id].udp_mode;

    if (type == NET_CLIENT_TCP) {
        num = recv(fd,
                at_net_recv_buf,
                AT_NET_RECV_BUF_SIZE,
                0);
        //num = so_recvsize_get(fd);
        //at_response_string("\r\n+IPD,%d:", num);
    }
    else if (type == NET_CLIENT_UDP) {
        num = recvfrom(fd,
                at_net_recv_buf,
                AT_NET_RECV_BUF_SIZE,
                0,
                (struct sockaddr *)&remote_addr,
                (socklen_t *)(&len));

        ip_addr_t ipaddr = {0};
        sockaddr_to_ipaddr((const struct sockaddr *)&remote_addr, &ipaddr);
        r_port = ntohs(((struct sockaddr_in *)&remote_addr)->sin_port);
        //printf("recvfd %d:%d id:%d udp_mode:%d remote_ip:%s remote_port:%d\r\n", fd, num, id, udp_mode, ipaddr_ntoa(&remote_ip), remote_port);
        //printf("remote_addr.sin_addr:%s remote_addr.sin_port:%d\r\n", ipaddr_ntoa(&ipaddr), ntohs(remote_addr.sin_port));

        if (udp_mode == 1) {
            id = ipaddr_lookup(&ipaddr, r_port);
            if (id < 0) {
                int i = 0;
                if ((id = net_socket_accept(fd, g_at_server_handle[i].type,
                            g_at_server_handle[i].port, g_at_server_handle[i].recv_timeout,
                            g_at_server_handle[i].client_num, g_at_server_handle[i].client_max,
                            g_at_server_handle[i].is_ipv6, g_at_server_handle[i].keepalive, 1)) >= 0) {
                    g_at_server_handle[i].client_num++;
                }
            }

            if (id < 0) {
                AT_NET_DEBUG("linkid is full, dorp the UDP packet\r\n");
                return -1;
            }

            remote_ip = g_at_client_handle[id].remote_ip;
            remote_port = g_at_client_handle[id].remote_port;
            udp_mode = g_at_client_handle[id].udp_mode;
        }
        //update remote addr
        if (udp_mode == 2) {
            if (!ip_addr_cmp(&remote_ip, &ipaddr) || remote_port != r_port) {
                net_lock();
                g_at_client_handle[id].remote_ip = ipaddr;
                g_at_client_handle[id].remote_port = r_port;
                g_at_client_handle[id].udp_mode = 1;
                net_unlock();
            }
        }
    }
    else if (type == NET_CLIENT_SSL) {
        num = mbedtls_ssl_recv(priv,
                at_net_recv_buf,
                AT_NET_RECV_BUF_SIZE);
    }
    else
        return 0;

    if (num <= 0) {
        if (type == NET_CLIENT_UDP) {
            return num;
        }
        net_socket_close(id);
    }
    else {
#if 0
        static uint32_t count = 0;
        static uint32_t sum = 0;

        uint32_t diff = xTaskGetTickCount() - count;

        sum += num;
        if (diff >= pdMS_TO_TICKS(1000)) {
            printf("RX:%.4f Mbps\r\n", (float)sum * 8 / 1000 / 1000 * diff / 1000);
            count = xTaskGetTickCount();
            sum = 0;
            //ps_cmd(0,0,0,0);
        }
#endif
        net_socket_ipd(NET_IPDINFO_RECVDATA,
                       id,
                       at_net_recv_buf,
                       num,
                       &g_at_client_handle[id].remote_ip,
                       g_at_client_handle[id].remote_port,
                       portMAX_DELAY);
        poll_timeout_update(id);
    }

    return num;
}

static int net_socket_accept(int fd, int type, uint16_t port, uint16_t timeout, uint8_t num, uint8_t max_conn, uint8_t is_ipv6, int keepalive, bool udp_force)
{
    int sock;
    struct sockaddr_in remote_addr;
#if defined(CFG_IPV6) && CFG_IPV6
    struct sockaddr_in6 remote_addr6;
#endif
    socklen_t len;
    int id;
    void *priv = NULL;

    if (type == NET_SERVER_UDP) {
    	for (int i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
    		if (!udp_force && g_at_client_handle[i].valid && g_at_client_handle[i].fd == fd) {
    			return -1;
    		}
    	}
    	sock = fd;
    } else {
#if defined(CFG_IPV6) && CFG_IPV6
        if (is_ipv6) {
            len = sizeof(struct sockaddr_in6);
    	    sock = accept(fd, (struct sockaddr*)&remote_addr6, &len);
        } else
#endif
        {
            len = sizeof(struct sockaddr_in);
    	    sock = accept(fd, (struct sockaddr*)&remote_addr, &len);
        }
    }

    if (sock >= 0) {
        if (num+1 > max_conn) {
            if (type != NET_SERVER_UDP) {
                close(sock);
            }
            printf("socket num %d more than max:%d\r\n", num, max_conn);
            return -1;
        }

        id = at_net_client_get_valid_id();
        if (id < 0) {
            close(sock);
            return -1;
        }

        if (type == NET_SERVER_SSL) {

            ssl_conn_param_t ssl_param = {0};

            at_load_file(g_at_server_handle[id].ca_path, &ssl_param.ca_cert, &ssl_param.ca_cert_len);
            at_load_file(g_at_server_handle[id].cert_path, &ssl_param.own_cert, &ssl_param.own_cert_len);
            at_net_ssl_load_file_decrypt(g_at_server_handle[id].priv_key_path, &ssl_param.private_cert, &ssl_param.private_cert_len);
            at_net_ssl_server_psk_get(id, &ssl_param.psk, &ssl_param.psk_len, &ssl_param.pskhint, &ssl_param.pskhint_len);

            priv = mbedtls_ssl_accept(sock, ssl_param.ca_cert, ssl_param.ca_cert_len,
                                      ssl_param.own_cert, ssl_param.own_cert_len,
                                      ssl_param.private_cert, ssl_param.private_cert_len,
                                      ssl_param.psk, ssl_param.psk_len,
                                      ssl_param.pskhint, ssl_param.pskhint_len);
            //priv = mbedtls_ssl_accept(sock, NULL, 0, NULL, 0, NULL, 0);
            if (priv == NULL) {
                AT_NET_PRINTF("mbedtls_ssl_accept priv NULL, fd:%d\r\n", fd);
                close(sock);
                at_free(ssl_param.ca_cert);
                at_free(ssl_param.own_cert);
                at_free(ssl_param.private_cert);
                return -1;
            }
            at_free(ssl_param.ca_cert);
            at_free(ssl_param.own_cert);
            at_free(ssl_param.private_cert);
        }
        if (type == NET_SERVER_SSL || type == NET_SERVER_TCP) {
            so_keepalive_enable(sock, keepalive, 1, 3);
        }

        net_lock();

        if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
            if (g_at_client_handle[id].recv_buf) {
                xStreamBufferReset(g_at_client_handle[id].recv_buf);
            } else {
                g_at_client_handle[id].recv_buf = xStreamBufferCreate(g_at_client_handle[id].recvbuf_size, 1);
                if (!g_at_client_handle[id].recv_buf) {
                    net_unlock();
                    net_socket_fd_close(fd, type, 1, priv);
                    return -AT_SUB_NO_MEMORY;
                }
            }
        }
        g_at_client_handle[id].valid = 1;
        g_at_client_handle[id].type = type;
        g_at_client_handle[id].fd = sock;
        g_at_client_handle[id].socket_accept = 1;
        g_at_client_handle[id].priv = priv;
#if defined(CFG_IPV6) && CFG_IPV6
        if (is_ipv6) {
            sockaddr_to_ipaddr((struct sockaddr *)&remote_addr6, &g_at_client_handle[id].remote_ip);
            g_at_client_handle[id].remote_port = ntohs(remote_addr6.sin6_port);
        } else
#endif
        {
            sockaddr_to_ipaddr((struct sockaddr *)&remote_addr, &g_at_client_handle[id].remote_ip);
            g_at_client_handle[id].remote_port = ntohs(remote_addr.sin_port);
        }
        g_at_client_handle[id].local_port = port;
        g_at_client_handle[id].udp_mode = 2;
        g_at_client_handle[id].tetype = 1;
        g_at_client_handle[id].recv_timeout = timeout;
        g_at_client_handle[id].recv_time = os_get_time_ms();
        g_at_client_handle[id].keep_alive = 0;
        g_at_client_handle[id].so_linger = -1;
        g_at_client_handle[id].tcp_nodelay = 0;
        g_at_client_handle[id].so_sndtimeo = 0;

        net_unlock();

        net_socket_ipd(NET_IPDINFO_CONNECTED, id, NULL, 0, &g_at_client_handle[id].remote_ip, g_at_client_handle[id].remote_port, 0);
        return id;
    }

    return -1;
}

static void net_poll_reconnect(void)
{
    int id = 0;
    int fd;
    void *priv;
    ip_addr_t ipaddr;
    uint16_t port;
    uint16_t local_port;

    if (g_at_client_handle[id].valid && g_at_client_handle[id].fd < 0) {

        if (at_net_config->work_mode == NET_MODE_TRANS && g_at_client_handle[id].retry_cnt >= at_net_config->trans_link.retry_cnt) {
            printf("Retry count exceeds %d, exiting throughput mode\r\n", at_net_config->trans_link.retry_cnt);
            at_net_config->work_mode = NET_MODE_NORMAL;
            g_at_client_handle[id].valid = 0;
            at_set_work_mode(AT_WORK_MODE_CMD);
            return;
        }
        if ((at_get_work_mode() != AT_WORK_MODE_THROUGHPUT) &&
            (at_get_work_mode() != AT_WORK_MODE_CMD_THROUGHPUT) &&
            (at_net_config->work_mode != NET_MODE_TRANS)) {
            net_lock();
            g_at_client_handle[id].valid = 0;
            net_unlock();
            return;
        }

        if (os_get_time_ms() - g_at_client_handle[id].disconnect_time <= at_net_config->reconn_intv*100) {
            vTaskDelay(at_net_config->reconn_intv*100);
            net_main_wakeup();
            return;
        }
        g_at_client_handle[id].disconnect_time = os_get_time_ms();

        if (!net_is_active()) {
            vTaskDelay(at_net_config->reconn_intv*100);
            net_main_wakeup();
            return;
        }

        if (ip_addr_isany(&g_at_client_handle[id].remote_ip)) {
            int res = at_string_host_to_ip(g_at_net_savelink_host, &ipaddr);
            if (res != 0) {
                return;
            }
        }
        else
            ipaddr = g_at_client_handle[id].remote_ip;
        port = g_at_client_handle[id].remote_port;
        local_port = g_at_client_handle[id].local_port;

        g_at_client_handle[id].retry_cnt++;
        if (g_at_client_handle[id].type == NET_CLIENT_TCP) {
            fd = tcp_client_connect(&ipaddr, port, 5000);
            if (fd >= 0) {
                net_socket_setopt(fd, g_at_client_handle[id].keep_alive, g_at_client_handle[id].so_linger, g_at_client_handle[id].tcp_nodelay, g_at_client_handle[id].so_sndtimeo);
                local_port = so_localport_get(fd);
            }
        }
        else if (g_at_client_handle[id].type == NET_CLIENT_UDP) {
            if (local_port == 0)
                local_port = udp_localport_rand();
            fd = udp_client_connect(local_port, &ipaddr);
            ip_multicast_enable(fd, &ipaddr);
        }
        else if (g_at_client_handle[id].type == NET_CLIENT_SSL) {
            fd = ssl_client_connect(id, &ipaddr, port, &priv, 5000);
            if (fd >= 0) {
                if (g_at_client_handle[id].keep_alive)
                    so_keepalive_enable(fd, g_at_client_handle[id].keep_alive, 1, 3);
                local_port = so_localport_get(fd);
            }
        }
        else
            return;

        if (fd < 0) {
            vTaskDelay(at_net_config->reconn_intv*100);
            net_main_wakeup();
            return;
        }

        net_lock();

        if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
            if (g_at_client_handle[id].recv_buf) {
                xStreamBufferReset(g_at_client_handle[id].recv_buf);
            } else {
                g_at_client_handle[id].recv_buf = xStreamBufferCreate(g_at_client_handle[id].recvbuf_size, 1);
                if (!g_at_client_handle[id].recv_buf) {
                    net_unlock();
                    net_socket_fd_close(fd, g_at_client_handle[id].type, 0, priv);
                    return;
                }
            }
        }
        g_at_client_handle[id].fd = fd;
        g_at_client_handle[id].socket_accept = 0;
        g_at_client_handle[id].priv = priv;
        g_at_client_handle[id].remote_ip = ipaddr;
        g_at_client_handle[id].remote_port = port;
        g_at_client_handle[id].local_port = local_port;

        if (at_net_config->work_mode == NET_MODE_TRANS) {
            at_set_work_mode(AT_WORK_MODE_THROUGHPUT);
            at_module_schedule_work();
        }
        net_main_wakeup();
        net_unlock();
    }
}

static void net_poll_recv(void)
{
    fd_set fdR;
    struct timeval timeout;
    int maxfd = -1;
    int i, fd;

    FD_ZERO(&fdR);
    if (wake_socket_fd >= 0) {
        FD_SET(wake_socket_fd, &fdR);
        maxfd = wake_socket_fd;
    }
    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (g_at_client_handle[i].valid && g_at_client_handle[i].fd >= 0) {
            FD_SET(g_at_client_handle[i].fd, &fdR);
            if (g_at_client_handle[i].fd > maxfd)
                maxfd = g_at_client_handle[i].fd;
        }
    }
    for (i = 0; i < AT_NET_SERVER_HANDLE_MAX; i++) {
        if (g_at_server_handle[i].valid) {
            FD_SET(g_at_server_handle[i].fd, &fdR);
            if (g_at_server_handle[i].fd > maxfd)
                maxfd = g_at_server_handle[i].fd;
        }
    }

    if (maxfd == -1) {
        return;
    }

    timeout.tv_sec= 0;
    timeout.tv_usec= 10000;
#ifdef CONFIG_LPAPP
    (void)timeout;
    if(select(maxfd+1, &fdR, NULL, NULL, NULL) > 0) {
#else
    if(select(maxfd+1, &fdR, NULL, NULL, &timeout) > 0) {
#endif
         for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
            fd = g_at_client_handle[i].fd;
            if (g_at_client_handle[i].valid && fd >= 0 && FD_ISSET(fd, &fdR)) {
                FD_CLR(fd, &fdR);
                net_socket_recv(i);
            }
        }
        for (i = 0; i < AT_NET_SERVER_HANDLE_MAX; i++) {
            fd = g_at_server_handle[i].fd;
            if (g_at_server_handle[i].valid && FD_ISSET(fd, &fdR)) {

                if (net_socket_accept(fd, g_at_server_handle[i].type,
                            g_at_server_handle[i].port, g_at_server_handle[i].recv_timeout,
                            g_at_server_handle[i].client_num, g_at_server_handle[i].client_max,
                            g_at_server_handle[i].is_ipv6, g_at_server_handle[i].keepalive, 0) >= 0) {
                    g_at_server_handle[i].client_num++;
                }
            }
        }
        if ((wake_socket_fd >= 0) && FD_ISSET(wake_socket_fd, &fdR)) {
        	select_wake_clear();
        }
    }
}

static void net_poll_timeout(void)
{
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (g_at_client_handle[i].valid && g_at_client_handle[i].recv_timeout > 0) {
            if ((os_get_time_ms() - g_at_client_handle[i].recv_time)/1000 >= g_at_client_handle[i].recv_timeout)
                net_socket_close(i);
        }
    }
}

static void net_init_save_link(void)
{
    net_client_type type;
    int id;

    if (at_net_config->trans_link.enable) {
        at_net_config->recv_mode = NET_RECV_MODE_ACTIVE;
        if (strstr(at_net_config->trans_link.type, "TCP") != NULL)
            type = NET_CLIENT_TCP;
        else if (strstr(at_net_config->trans_link.type, "UDP") != NULL)
            type = NET_CLIENT_UDP;
        else if (strstr(at_net_config->trans_link.type, "SSL") != NULL)
            type = NET_CLIENT_SSL;
        else
            return;

        at_net_config->work_mode = NET_MODE_TRANS;

        id = at_net_client_get_valid_id();
        net_lock();
        g_at_client_handle[id].valid = 1;
        g_at_client_handle[id].type = type;
        g_at_client_handle[id].fd = -1;
        g_at_client_handle[id].socket_accept = 0;
        g_at_client_handle[id].retry_cnt = 0;
        ip_addr_set_any(
#if LWIP_IPV6
            1,
#else
            0,
#endif
            &g_at_client_handle[id].remote_ip);
        strlcpy(g_at_net_savelink_host, at_net_config->trans_link.remote_host, sizeof(g_at_net_savelink_host));
        g_at_client_handle[id].remote_port = at_net_config->trans_link.remote_port;
        g_at_client_handle[id].local_port = at_net_config->trans_link.local_port;
        g_at_client_handle[id].udp_mode = 0;
        g_at_client_handle[id].tetype = 0;
        g_at_client_handle[id].recv_timeout = 0;
        g_at_client_handle[id].recv_time = 0;
        g_at_client_handle[id].keep_alive = at_net_config->trans_link.keep_alive;
        g_at_client_handle[id].so_linger = -1;
        g_at_client_handle[id].tcp_nodelay = 0;
        g_at_client_handle[id].so_sndtimeo = 0;
        net_unlock();
    }
}

static void net_main_task(void *pvParameters)
{
    g_at_net_task_is_start = 1;

    while(1) {
        net_poll_reconnect();

        net_poll_recv();

        net_poll_timeout();

        if (g_at_net_task_is_start == 2)
            break;
    }

    g_at_net_task_is_start = 0;
    vTaskDelete(NULL);
}

static int at_net_init(void)
{
    char buffer[255];
    if (g_at_client_handle)
        return 0;

    g_at_client_handle = (at_net_client_handle *)at_malloc(sizeof(at_net_client_handle) * AT_NET_CLIENT_HANDLE_MAX);
    if (!g_at_client_handle) {
        AT_NET_PRINTF("at_net malloc failed!\r\n");
        return -1;
    }

    g_at_server_handle = (at_net_server_handle *)at_malloc(sizeof(at_net_server_handle) * AT_NET_SERVER_HANDLE_MAX);
    if (!g_at_server_handle) {
        AT_NET_PRINTF("at_net malloc failed!\r\n");
        at_free(g_at_client_handle);
        g_at_client_handle = NULL;
        return -1;
    }
    net_mutex = xSemaphoreCreateMutex();

    memset(g_at_client_handle, 0, sizeof(at_net_client_handle) * AT_NET_CLIENT_HANDLE_MAX);
    memset(g_at_server_handle, 0, sizeof(at_net_server_handle) * AT_NET_SERVER_HANDLE_MAX);

    for (int id = 0; id < AT_NET_CLIENT_HANDLE_MAX; id++) {
        g_at_client_handle[id].recvbuf_size = AT_NET_RECV_BUF_SIZE;
        at_net_ssl_path_set(id, at_net_config->sslconf[id].ca_file,
                            at_net_config->sslconf[id].cert_file,
                            at_net_config->sslconf[id].key_file);
        memset(buffer, 0, sizeof(buffer));
        if (at_config_read_with_id(AT_CONFIG_KEY_NET_SNI, id, buffer, sizeof(buffer))) {
            g_at_client_handle[id].ssl_hostname = strdup(buffer);
        }
    }

    for (int id = 0; id < AT_NET_SERVER_HANDLE_MAX; id++) {
        at_net_ssl_server_path_set(id, at_net_config->sslsconf[id].ca_file,
                                   at_net_config->sslsconf[id].cert_file,
                                   at_net_config->sslsconf[id].key_file);
    }

    wake_socket_fd =  socket(AF_INET, SOCK_DGRAM, 0);
    if (wake_socket_fd < 0) {
        AT_NET_PRINTF("loopback socket create failed\r\n");
        return -1;
    }
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(AT_LOCAL_LOOP_SOCKET_PORT),
        .sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK),
    };
    bind(wake_socket_fd, (struct sockaddr *)&addr, sizeof(addr));

    net_timer = xTimerCreate("net_poll",
                              portMAX_DELAY,
                              pdFALSE, NULL, net_timer_callback_func);
    return 0;
}

static int at_net_deinit(void)
{
    if (g_at_client_handle) {
        at_free(g_at_client_handle);
        g_at_client_handle = NULL;

        at_free(g_at_server_handle);
        g_at_server_handle = NULL;
    }

    if (net_mutex) {
        vSemaphoreDelete(net_mutex);
        net_mutex = NULL;
    }
    if (net_timer) {
        xTimerDelete(net_timer, portMAX_DELAY);
        net_timer = NULL;
    }

    return 0;
}

int at_net_sock_is_build(void)
{
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (g_at_client_handle[i].valid)
            return 1;
    }

    for (i = 0; i < AT_NET_SERVER_HANDLE_MAX; i++) {
        if (g_at_server_handle[i].valid)
            return 1;
    }

    return 0;
}

int at_net_client_get_valid_id(void)
{
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (g_at_client_handle[i].valid == 0)
            return i;
    }
    return -1;
}

int at_net_client_tcp_connect(int id, ip_addr_t *remote_ip, uint16_t remote_port, int keepalive, uint32_t timeout)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    return net_socket_connect(id, NET_CLIENT_TCP, remote_ip, remote_port, keepalive, 0, 0, timeout);
}

int at_net_client_udp_connect(int id, ip_addr_t *remote_ip, uint16_t remote_port, uint16_t local_port, int mode, uint32_t timeout)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    return net_socket_connect(id, NET_CLIENT_UDP, remote_ip, remote_port, 0, local_port, mode, timeout);
}

int at_net_client_ssl_connect(int id, ip_addr_t *remote_ip, uint16_t remote_port, int keepalive, uint32_t timeout)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    return net_socket_connect(id, NET_CLIENT_SSL, remote_ip, remote_port, keepalive, 0, 0, timeout);
}

int at_net_client_is_connected(int id)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    if (g_at_client_handle[id].valid && g_at_client_handle[id].fd >= 0)
        return 1;

    return 0;
}

int at_net_client_set_remote(int id, ip_addr_t *ipaddr, uint16_t port)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    if (!g_at_client_handle[id].valid)
        return -1;

    if (g_at_client_handle[id].type != NET_CLIENT_UDP)
        return -1;

    g_at_client_handle[id].remote_ip = *ipaddr;
    g_at_client_handle[id].remote_port = port;
    //AT_NET_PRINTF("net set id: %d remote_ip: %s remote_port: %d\r\n", id, ipaddr_ntoa(ipaddr), port);
    return 0;
}

int at_net_client_get_info(int id, char *type, uint16_t len, ip_addr_t *remote_ipaddr, uint16_t *remote_port, uint16_t *local_port, uint8_t *tetype)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    if (g_at_client_handle[id].valid && (g_at_client_handle[id].fd >= 0)) {
        if (type) {
            if (g_at_client_handle[id].type == NET_CLIENT_TCP) {
#if defined(CFG_IPV6) && CFG_IPV6
                if (IP_IS_V6(&g_at_client_handle[id].remote_ip)) {
                    strlcpy(type, "TCPv6", len);
                } else
#endif
                    strlcpy(type, "TCP", len);
            } else if (g_at_client_handle[id].type == NET_CLIENT_UDP) {
#if defined(CFG_IPV6) && CFG_IPV6
                if (IP_IS_V6(&g_at_client_handle[id].remote_ip)) {
                    strlcpy(type, "UDPv6", len);
                } else
#endif
                    strlcpy(type, "UDP", len);
            } else if (g_at_client_handle[id].type == NET_CLIENT_SSL) {
#if defined(CFG_IPV6) && CFG_IPV6
                if (IP_IS_V6(&g_at_client_handle[id].remote_ip)) {
                    strlcpy(type, "SSLv6", len);
                } else
#endif
                    strlcpy(type, "SSL", len);
            }
        }
        if (remote_ipaddr)
            *remote_ipaddr = g_at_client_handle[id].remote_ip;
        if (remote_port)
            *remote_port = g_at_client_handle[id].remote_port;
        if (local_port)
            *local_port = g_at_client_handle[id].local_port;
        if (tetype)
            *tetype = g_at_client_handle[id].tetype;
        return 0;
    }
    return -1;
}

int at_net_client_get_recvsize(int id)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    if (g_at_client_handle[id].recv_buf)
        return xStreamBufferBytesAvailable(g_at_client_handle[id].recv_buf);
    else
        return 0;
}

int at_net_recvbuf_delete(int id)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    net_lock();
    if (g_at_client_handle[id].recv_buf) {
        vStreamBufferDelete(g_at_client_handle[id].recv_buf);
        g_at_client_handle[id].recv_buf = NULL;
    }
    net_unlock();
    return 0;
}

int at_net_client_send(int id, void * buffer, int length)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    return net_socket_send(id, buffer, length);
}

int at_net_client_close(int id)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    return net_socket_close(id);
}

int at_net_client_close_all(void)
{
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (g_at_client_handle[i].valid)
            net_socket_close(i);
    }

    return 0;
}

int at_net_server_sockets_close_all(void)
{
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (g_at_client_handle[i].valid && g_at_client_handle[i].socket_accept) {
            net_socket_close(i);
            printf("server_sockets_close_all:%d\r\n", i);
        }
    }

    return 0;
}

int at_net_server_id_is_valid(int id)
{
    if (id < 0 || id >= AT_NET_SERVER_HANDLE_MAX)
        return 0;
    else
        return 1;
}

int at_net_server_udp_create(uint16_t port, int max_conn,  int timeout, uint8_t is_ipv6)
{
    int id = 0;
    int fd;

    if (g_at_server_handle[id].valid) {
        AT_NET_PRINTF("already create\r\n");
        return -1;
    }

    fd = udp_server_create(port, max_conn, is_ipv6);
    if (fd < 0) {
        AT_NET_PRINTF("create failed\r\n");
        return -1;
    }

    net_lock();
    g_at_server_handle[id].valid = 1;
    net_main_wakeup();
    g_at_server_handle[id].type = NET_SERVER_UDP;
    g_at_server_handle[id].fd = fd;
    g_at_server_handle[id].is_ipv6 = is_ipv6;
    g_at_server_handle[id].port = port;
    g_at_server_handle[id].recv_timeout = timeout;
    g_at_server_handle[id].ca_enable = 0;
    g_at_server_handle[id].client_max = max_conn;
    g_at_server_handle[id].client_num = 0;
    g_at_server_handle[id].keepalive = 0;
    net_unlock();
    return 0;
}

int at_net_server_tcp_create(uint16_t port, int max_conn,  int timeout, uint8_t is_ipv6, int keep_alive)
{
    int id = 0;
    int fd;

    if (g_at_server_handle[id].valid) {
        AT_NET_PRINTF("already create\r\n");
        return -1;
    }

    fd = tcp_server_create(port, max_conn, is_ipv6);
    if (fd < 0) {
        AT_NET_PRINTF("create failed\r\n");
        return -1;
    }

    net_lock();
    g_at_server_handle[id].valid = 1;
    net_main_wakeup();
    g_at_server_handle[id].type = NET_SERVER_TCP;
    g_at_server_handle[id].fd = fd;
    g_at_server_handle[id].is_ipv6 = is_ipv6;
    g_at_server_handle[id].port = port;
    g_at_server_handle[id].recv_timeout = timeout;
    g_at_server_handle[id].ca_enable = 0;
    g_at_server_handle[id].client_max = max_conn;
    g_at_server_handle[id].client_num = 0;
    g_at_server_handle[id].keepalive = keep_alive;
    net_unlock();
    return 0;
}

int at_net_server_ssl_create(uint16_t port, int max_conn,  int timeout, int ca_enable, uint8_t is_ipv6, int keep_alive)
{
    int id = 0;
    int fd;

    if (g_at_server_handle[id].valid) {
        AT_NET_PRINTF("already create\r\n");
        return -1;
    }

    fd = tcp_server_create(port, max_conn, is_ipv6);
    if (fd < 0) {
        AT_NET_PRINTF("create failed\r\n");
        return -1;
    }


    net_lock();
    g_at_server_handle[id].valid = 1;
    net_main_wakeup();
    g_at_server_handle[id].type = NET_SERVER_SSL;
    g_at_server_handle[id].fd = fd;
    g_at_server_handle[id].is_ipv6 = is_ipv6;
    g_at_server_handle[id].port = port;
    g_at_server_handle[id].recv_timeout = timeout;
    g_at_server_handle[id].ca_enable = ca_enable;
    g_at_server_handle[id].client_max = max_conn;
    g_at_server_handle[id].client_num = 0;
    g_at_server_handle[id].keepalive = keep_alive;

    net_unlock();
    return 0;
}

int at_net_server_is_created(uint16_t *port, char *type, int *ca_enable, int *keepalive)
{
    int id = 0;
    CHECK_NET_SERVER_ID_VALID(id);

    if (!g_at_server_handle[id].valid)
        return 0;

    if (port) {
        *port = g_at_server_handle[id].port;
    }
    if (type) {
        if ( g_at_server_handle[id].type == NET_SERVER_TCP)
            strlcpy(type, "TCP", sizeof(type));
        else if ( g_at_server_handle[id].type == NET_SERVER_SSL)
            strlcpy(type, "SSL", sizeof(type));
        else if ( g_at_server_handle[id].type == NET_SERVER_UDP)
            strlcpy(type, "UDP", sizeof(type));
    }
    if (g_at_server_handle[id].is_ipv6) {
        strncat(type, "v6", 2);
    }
    if (ca_enable) {
        *ca_enable = g_at_server_handle[id].ca_enable;
    }
    if (keepalive) {
        *keepalive = g_at_server_handle[id].keepalive;
    }
    return 1;
}

static void _net_server_close(int evtid, void *arg)
{
    int id = 0;
    int valid = g_at_server_handle[id].valid;
    net_server_type type = g_at_server_handle[id].type;
    int fd = g_at_server_handle[id].fd;

    if (!valid) {
        AT_NET_PRINTF("socket is not inited\r\n");
        return;
    }

    net_lock();
    g_at_server_handle[id].valid = 0;
    net_unlock();

    if (type == NET_SERVER_TCP || type == NET_SERVER_SSL)
       tcp_server_close(fd);

    if (type == NET_SERVER_UDP)
       udp_server_close(fd);
}

int at_net_server_close(void)
{
    struct at_workq wq = {
        .pfunc = _net_server_close,
        .arg = (void*)0,
    };

    if (g_at_server_handle[0].type == NET_SERVER_UDP) {
        at_net_server_sockets_close_all();
    }
    at_workq_send(AT_EVENT_SOCKET_CLOSE, &wq, portMAX_DELAY);
    return 0;
}

int at_net_throuthput_udp_linktype(int linkid)
{
    if ((!at_net_client_id_is_valid(linkid)) || (!at_net_client_is_connected(linkid))) {
        return 0;
    }
    return (g_at_client_handle[linkid].type == NET_CLIENT_UDP);
}

#if defined(CONFIG_AT_MDNS_ENABLE) && CONFIG_AT_MDNS_ENABLE
typedef struct {
    uint8_t running;
    struct netif *netif;
    s8_t service_slot;
    at_net_mdns_state_t info;
    uint8_t txt_count;
    char (*txt_items)[AT_NET_MDNS_TXT_ITEM_MAX_LEN];
} at_net_mdns_handle_t;

typedef struct {
    SemaphoreHandle_t done;
    at_net_mdns_result_t *results;
    int max_results;
    int result_count;
    char service[AT_NET_MDNS_LABEL_MAX_LEN + 1];
    char proto[5];
    uint8_t current_valid;
} at_net_mdns_search_t;

static uint8_t g_at_net_mdns_is_init;
static at_net_mdns_handle_t *g_at_net_mdns;

static int at_net_mdns_label_valid(const char *label)
{
    size_t len;

    if (!label) {
        return 0;
    }

    len = strlen(label);
    return len > 0 && len <= AT_NET_MDNS_LABEL_MAX_LEN;
}

static int at_net_mdns_proto_parse(const char *proto, enum mdns_sd_proto *out)
{
    if (strcmp(proto, "_tcp") == 0) {
        *out = DNSSD_PROTO_TCP;
        return 0;
    }

    if (strcmp(proto, "_udp") == 0) {
        *out = DNSSD_PROTO_UDP;
        return 0;
    }

    return -1;
}

static struct netif *at_net_mdns_get_netif(void)
{
    return netif_find("wl1");
}

static const char *at_net_mdns_get_netif_hostname(struct netif *netif)
{
#if LWIP_NETIF_HOSTNAME
    if (netif && netif->hostname && netif->hostname[0] != '\0') {
        return netif->hostname;
    }
#else
    (void)netif;
#endif

    return "atmdns";
}

static int at_net_mdns_domain_to_string(const struct mdns_domain *domain, char *out, size_t out_len)
{
    const uint8_t *src;
    size_t pos = 0;
    uint16_t read_len = 0;

    if (!domain || !out || out_len == 0) {
        return -1;
    }

    src = domain->name;
    out[0] = '\0';
    while (read_len < domain->length && src[0] != 0) {
        uint8_t label_len = src[0];

        src++;
        read_len++;
        if (label_len == 0 || read_len + label_len > domain->length ||
            pos + label_len + 2 > out_len) {
            return -1;
        }

        if (pos > 0) {
            out[pos++] = '.';
        }
        memcpy(&out[pos], src, label_len);
        pos += label_len;
        src += label_len;
        read_len += label_len;
    }

    out[pos] = '\0';
    return 0;
}

static void at_net_mdns_txt_fn(struct mdns_service *service, void *txt_userdata)
{
    at_net_mdns_handle_t *handle = (at_net_mdns_handle_t *)txt_userdata;
    int i;

    for (i = 0; i < handle->txt_count; i++) {
        mdns_resp_add_service_txtitem(service, handle->txt_items[i], strlen(handle->txt_items[i]));
    }
}

static void at_net_mdns_init_once(void)
{
    if (!g_at_net_mdns_is_init) {
        mdns_resp_init();
        g_at_net_mdns_is_init = 1;
    }
}

static int at_net_mdns_err_to_sub_code(err_t err)
{
    if (err == ERR_OK) {
        return 0;
    }
    if (err == ERR_MEM) {
        return AT_SUB_NO_MEMORY;
    }

    return AT_SUB_CMD_EXEC_FAIL;
}

static int at_net_mdns_add_netif(struct netif *netif, const char *hostname)
{
    err_t err;

    if (mdns_resp_netif_active(netif)) {
        err = mdns_resp_rename_netif(netif, hostname);
    } else {
        err = mdns_resp_add_netif(netif, hostname);
    }

    return at_net_mdns_err_to_sub_code(err);
}

static void at_net_mdns_free_handle(void)
{
    if (g_at_net_mdns) {
        if (g_at_net_mdns->txt_items) {
            at_free(g_at_net_mdns->txt_items);
        }
        at_free(g_at_net_mdns);
        g_at_net_mdns = NULL;
    }
}

static void at_net_mdns_give_done(at_net_mdns_search_t *ctx)
{
    if (ctx->done) {
        xSemaphoreGive(ctx->done);
    }
}

static at_net_mdns_result_t *at_net_mdns_current_result(at_net_mdns_search_t *ctx, uint8_t ip_type)
{
    at_net_mdns_result_t *result;

    if (ctx->result_count >= ctx->max_results) {
        return NULL;
    }

    if (!ctx->current_valid) {
        result = &ctx->results[ctx->result_count];
        memset(result, 0, sizeof(*result));
        result->netif = 1;
        result->ip_type = ip_type;
        ctx->current_valid = 1;
    } else {
        result = &ctx->results[ctx->result_count];
    }

    return result;
}

static void at_net_mdns_finish_current_result(at_net_mdns_search_t *ctx)
{
    at_net_mdns_result_t *result;

    if (!ctx->current_valid || ctx->result_count >= ctx->max_results) {
        return;
    }

    result = &ctx->results[ctx->result_count];
    if (result->ptr[0] || result->srv[0] || result->txt_num ||
        result->a_num || result->aaaa_num) {
        ctx->result_count++;
    }
    ctx->current_valid = 0;
}

static int at_net_mdns_copy_ptr(at_net_mdns_result_t *result, const char *domain,
                                const char *service, const char *proto)
{
    char suffix[AT_NET_MDNS_LABEL_MAX_LEN * 2 + 8];
    const char *suffix_pos;
    size_t len;

    if (!result || !domain || domain[0] == '\0') {
        return -1;
    }

    if (snprintf(suffix, sizeof(suffix), ".%s.%s", service, proto) >= (int)sizeof(suffix)) {
        return -1;
    }

    suffix_pos = strstr(domain, suffix);
    if (!suffix_pos || suffix_pos == domain) {
        return -1;
    }

    if (suffix_pos[strlen(suffix)] != '\0' &&
        strcmp(&suffix_pos[strlen(suffix)], ".local") != 0) {
        return -1;
    }

    len = suffix_pos - domain;
    if (len > AT_NET_MDNS_LABEL_MAX_LEN) {
        len = AT_NET_MDNS_LABEL_MAX_LEN;
    }
    memcpy(result->ptr, domain, len);
    result->ptr[len] = '\0';
    return 0;
}

static void at_net_mdns_add_txt(at_net_mdns_result_t *result, const char *txt, int txt_len)
{
    int copy_len;

    if (!result || !txt || txt_len <= 0 ||
        result->txt_num >= AT_NET_MDNS_QUERY_RECORD_MAX) {
        return;
    }

    copy_len = txt_len;
    if (copy_len >= AT_NET_MDNS_TXT_ITEM_MAX_LEN) {
        copy_len = AT_NET_MDNS_TXT_ITEM_MAX_LEN - 1;
    }
    memcpy(result->txt[result->txt_num], txt, copy_len);
    result->txt[result->txt_num][copy_len] = '\0';
    result->txt_num++;
}

static void at_net_mdns_parse_txt(at_net_mdns_result_t *result, const char *varpart, int varlen)
{
    int pos = 0;

    while (pos < varlen) {
        uint8_t len = (uint8_t)varpart[pos++];

        if (len == 0) {
            continue;
        }
        if (pos + len > varlen) {
            break;
        }

        at_net_mdns_add_txt(result, &varpart[pos], len);
        pos += len;
    }
}

static void at_net_mdns_search_result(struct mdns_answer *answer, const char *varpart,
                                      int varlen, int flags, void *arg)
{
    at_net_mdns_search_t *ctx = (at_net_mdns_search_t *)arg;
    at_net_mdns_result_t *result;
    char domain[AT_NET_MDNS_DOMAIN_MAX_LEN];

    if (!answer || !varpart || !ctx) {
        return;
    }

    result = at_net_mdns_current_result(ctx, answer->info.type == DNS_RRTYPE_AAAA ? 1 : 0);
    if (!result) {
        at_net_mdns_give_done(ctx);
        return;
    }

    switch (answer->info.type) {
        case DNS_RRTYPE_PTR: {
            const struct mdns_domain *ptr = (const struct mdns_domain *)varpart;

            if (at_net_mdns_domain_to_string(ptr, domain, sizeof(domain)) == 0) {
                if (at_net_mdns_copy_ptr(result, domain, ctx->service, ctx->proto) != 0) {
                    ctx->current_valid = 0;
                    return;
                }
            }
        } break;

        case DNS_RRTYPE_SRV: {
            uint16_t port;
            const struct mdns_domain *target;

            if (varlen < 6) {
                break;
            }

            memcpy(&port, varpart + 4, sizeof(port));
            target = (const struct mdns_domain *)(varpart + 6);
            if (at_net_mdns_domain_to_string(target, domain, sizeof(domain)) == 0) {
                strlcpy(result->srv, domain, sizeof(result->srv));
                result->port = lwip_ntohs(port);
            }
        } break;

        case DNS_RRTYPE_TXT:
            at_net_mdns_parse_txt(result, varpart, varlen);
            break;

        case DNS_RRTYPE_A: {
            ip4_addr_t addr;

            if (varlen == sizeof(addr) && result->a_num < AT_NET_MDNS_QUERY_RECORD_MAX) {
                memcpy(&addr, varpart, sizeof(addr));
                ip4addr_ntoa_r(&addr, result->a[result->a_num], sizeof(result->a[result->a_num]));
                result->a_num++;
            }
        } break;

#if LWIP_IPV6
        case DNS_RRTYPE_AAAA: {
            ip6_addr_t addr;

            if (varlen == sizeof(addr) && result->aaaa_num < AT_NET_MDNS_QUERY_RECORD_MAX) {
                memcpy(&addr, varpart, sizeof(addr));
                ip6addr_ntoa_r(&addr, result->aaaa[result->aaaa_num],
                                sizeof(result->aaaa[result->aaaa_num]));
                result->aaaa_num++;
            }
        } break;
#endif

        default:
            break;
    }

    if (flags & MDNS_SEARCH_RESULT_LAST) {
        at_net_mdns_finish_current_result(ctx);
    }
    if (ctx->result_count >= ctx->max_results) {
        at_net_mdns_give_done(ctx);
    }
}

int at_net_mdns_stop(void)
{
    at_net_mdns_handle_t *handle = g_at_net_mdns;

    if (!handle) {
        return 0;
    }

    if (handle->netif && handle->running) {
        if (handle->service_slot >= 0) {
            mdns_resp_del_service(handle->netif, handle->service_slot);
        }
    }

    at_net_mdns_free_handle();
    return 0;
}

int at_net_mdns_start(const char *hostname, const char *service, uint16_t port,
                      const char *instance, const char *proto,
                      const char txt_items[][AT_NET_MDNS_TXT_ITEM_MAX_LEN], uint8_t txt_count)
{
    enum mdns_sd_proto mdns_proto;
    struct netif *netif;
    at_net_mdns_handle_t *handle;
    char (*new_txt_items)[AT_NET_MDNS_TXT_ITEM_MAX_LEN] = NULL;
    int ret;
    s8_t slot;

    if (!at_net_mdns_label_valid(hostname) || !at_net_mdns_label_valid(service) ||
        !at_net_mdns_label_valid(instance) || service[0] != '_' || port == 0 ||
        txt_count > AT_NET_MDNS_TXT_ITEM_MAX_NUM) {
        return AT_SUB_PARA_VALUE_INVALID;
    }

    if (at_net_mdns_proto_parse(proto, &mdns_proto) != 0) {
        return AT_SUB_PARA_VALUE_INVALID;
    }

    netif = at_net_mdns_get_netif();
    if (!netif) {
        return AT_SUB_NOT_INIT;
    }

    handle = at_malloc(sizeof(*handle));
    if (!handle) {
        return AT_SUB_NO_MEMORY;
    }
    memset(handle, 0, sizeof(*handle));
    handle->service_slot = -1;

    if (txt_count) {
        new_txt_items = at_malloc(txt_count * AT_NET_MDNS_TXT_ITEM_MAX_LEN);
        if (!new_txt_items) {
            at_free(handle);
            return AT_SUB_NO_MEMORY;
        }
        memcpy(new_txt_items, txt_items, txt_count * AT_NET_MDNS_TXT_ITEM_MAX_LEN);
    }

    at_net_mdns_init_once();
    at_net_mdns_stop();
    handle->txt_count = txt_count;
    handle->txt_items = new_txt_items;

    ret = at_net_mdns_add_netif(netif, hostname);
    if (ret != 0) {
        goto fail;
    }

    slot = mdns_resp_add_service(netif, instance, service, mdns_proto, port,
                                 handle->txt_count ? at_net_mdns_txt_fn : NULL,
                                 handle);
    if (slot < 0) {
        ret = AT_SUB_NO_RESOURCE;
        goto fail;
    }

    handle->running = 1;
    handle->netif = netif;
    handle->service_slot = slot;
    handle->info.running = 1;
    handle->info.port = port;
    strlcpy(handle->info.hostname, hostname, sizeof(handle->info.hostname));
    strlcpy(handle->info.service, service, sizeof(handle->info.service));
    strlcpy(handle->info.instance, instance, sizeof(handle->info.instance));
    strlcpy(handle->info.proto, proto, sizeof(handle->info.proto));
    g_at_net_mdns = handle;

    return 0;

fail:
    if (new_txt_items) {
        at_free(new_txt_items);
    }
    at_free(handle);
    return ret;
}

int at_net_mdns_query(const char *service, const char *proto, int timeout_ms,
                      at_net_mdns_result_t *results, int max_results, int *result_count)
{
    enum mdns_sd_proto mdns_proto;
    at_net_mdns_search_t ctx;
    struct netif *netif;
    uint8_t request_id;
    int elapsed_ms = 0;
    err_t err;
    int ret;

    if (!at_net_mdns_label_valid(service) || service[0] != '_' ||
        at_net_mdns_proto_parse(proto, &mdns_proto) != 0 ||
        timeout_ms < AT_NET_MDNS_QUERY_TIMEOUT_MIN_MS ||
        timeout_ms > AT_NET_MDNS_QUERY_TIMEOUT_MAX_MS || max_results <= 0 ||
        max_results > AT_NET_MDNS_QUERY_RESULT_MAX || !results || !result_count) {
        return AT_SUB_PARA_VALUE_INVALID;
    }

    netif = at_net_mdns_get_netif();
    if (!netif) {
        return AT_SUB_NOT_INIT;
    }

    *result_count = 0;
    at_net_mdns_init_once();
    memset(results, 0, sizeof(*results) * max_results);
    memset(&ctx, 0, sizeof(ctx));
    ctx.done = xSemaphoreCreateBinary();
    if (!ctx.done) {
        return AT_SUB_NO_MEMORY;
    }
    ctx.results = results;
    ctx.max_results = max_results;
    strlcpy(ctx.service, service, sizeof(ctx.service));
    strlcpy(ctx.proto, proto, sizeof(ctx.proto));

    LOCK_TCPIP_CORE();
    if (!mdns_resp_netif_active(netif)) {
        ret = at_net_mdns_add_netif(netif, at_net_mdns_get_netif_hostname(netif));
        if (ret != 0) {
            UNLOCK_TCPIP_CORE();
            vSemaphoreDelete(ctx.done);
            return ret;
        }
    }
    UNLOCK_TCPIP_CORE();

    while (elapsed_ms < timeout_ms) {
        int wait_ms = LWIP_MIN(1000, timeout_ms - elapsed_ms);

        LOCK_TCPIP_CORE();
        err = mdns_search_service(NULL, service, mdns_proto, netif,
                                  at_net_mdns_search_result, &ctx, &request_id);
        UNLOCK_TCPIP_CORE();
        if (err != ERR_OK) {
            vSemaphoreDelete(ctx.done);
            return at_net_mdns_err_to_sub_code(err);
        }

        BaseType_t query_done = xSemaphoreTake(ctx.done, pdMS_TO_TICKS(wait_ms));
        LOCK_TCPIP_CORE();
        mdns_search_stop(request_id);
        UNLOCK_TCPIP_CORE();
        if (query_done == pdTRUE) {
            break;
        }

        elapsed_ms += wait_ms;
    }
    vSemaphoreDelete(ctx.done);

    at_net_mdns_finish_current_result(&ctx);
    if (ctx.result_count == 0) {
        return AT_SUB_TIMEOUT;
    }

    *result_count = ctx.result_count;
    return 0;
}

int at_net_mdns_get_state(at_net_mdns_state_t *state)
{
    if (!state) {
        return AT_SUB_PARA_VALUE_INVALID;
    }

    memset(state, 0, sizeof(*state));
    if (g_at_net_mdns) {
        memcpy(state, &g_at_net_mdns->info, sizeof(*state));
    }
    return 0;
}
#endif

static void at_net_sntp_sync(void)
{
    uint64_t time_stamp, time_stamp_ms;
    uint32_t seconds = 0, frags = 0;

    sntp_get_time(&seconds, &frags);
    time_stamp = seconds;
    time_stamp_ms = time_stamp*1000 + frags/1000;
    at_base_config->systime_stamp = time_stamp_ms - at_current_ms_get();
}

int at_net_sntp_gettime(struct timespec * tp)
{
    time_t ut;
    int zone = (int)at_net_config->sntp_cfg.timezone;

    if (tp == NULL) {
        return -1;
    }
    if (zone >= -12 && zone <= 14)
        zone = zone*3600;
    else {
        int zone_h = zone/100;
        int zone_m = zone%100;
        zone = zone_h * 3600 + zone_m * 60;
    }

    ut = (time_t)(at_base_config->systime_stamp + at_current_ms_get());

    tp->tv_sec = ut / 1000 + zone;
    tp->tv_nsec = (ut % 1000) * 1000000;

    return 0;
}

int at_net_sntp_start(void)
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    if (at_net_config->sntp_cfg.servernum >= 1)
        sntp_setservername(0, at_net_config->sntp_cfg.server1);
    else
        sntp_setservername(0, NULL);
    if (at_net_config->sntp_cfg.servernum >= 2)
        sntp_setservername(1, at_net_config->sntp_cfg.server2);
    else
        sntp_setservername(1, NULL);
    if (at_net_config->sntp_cfg.servernum >= 3)
        sntp_setservername(2, at_net_config->sntp_cfg.server3);
    else
        sntp_setservername(2, NULL);
    sntp_settimesynccb(at_net_sntp_sync);
    sntp_setupdatedelay(at_net_config->sntp_intv.interval);
    sntp_init();
    g_at_net_sntp_is_start = 1;
    return 0;
}

int at_net_sntp_stop(void)
{
    sntp_stop();
    g_at_net_sntp_is_start = 0;
    return 0;
}

int at_net_sntp_is_start(void)
{
    return g_at_net_sntp_is_start;
}

int at_net_start(void)
{
    int ret;

    at_net_init();

    net_init_save_link();

    ret = xTaskCreate(net_main_task, (char*)"net_main_task", AT_NET_TASK_STACK_SIZE, NULL, AT_NET_TASK_PRIORITY_LOW, NULL);
    if (ret != pdPASS) {
        AT_NET_PRINTF("ERROR: create net_main_task failed, ret = %d\r\n", ret);
        return -1;
    }

    return 0;
}

int at_net_stop(void)
{
    int ret = 0;

#if LWIP_IPV6
    if (wifi_sta_ipv6_enable(0) != 0) {
        ret = -1;
    }
#endif

    at_net_client_close_all();

    wake_socket_close();
    if (g_at_net_task_is_start) {
        g_at_net_task_is_start = 2;
        while(g_at_net_task_is_start != 0) {
            vTaskDelay(100);
            net_main_wakeup();
        }
    }
    at_net_deinit();
    return ret;
}

int at_net_recvbuf_size_set(int linkid, uint32_t size)
{
    net_lock();
    g_at_client_handle[linkid].recvbuf_size = size;
    net_unlock();
    return 0;
}

int at_net_recvbuf_size_get(int linkid)
{
    return g_at_client_handle[linkid].recvbuf_size;
}

int at_net_recvbuf_read(int linkid, ip_addr_t *remote_ipaddr, uint16_t *remote_port, uint8_t *buf, uint32_t size)
{
    int read_size = 0;

    if (!g_at_client_handle[linkid].recv_buf) {
        return 0;
    }

    if (remote_ipaddr) {
        *remote_ipaddr = g_at_client_handle[linkid].remote_ip;
    }
    if (remote_port) {
        *remote_port = g_at_client_handle[linkid].remote_port;
    }
    if (buf == NULL) {
        return 0;
    }
    read_size = xStreamBufferReceive(g_at_client_handle[linkid].recv_buf, buf, size, 0);

    return (read_size >= 0) ? read_size : 0;
}

int at_net_ssl_path_set(int linkid, const char *ca, const char *cert, const char *key)
{
    if (ca) {
        strlcpy(g_at_client_handle[linkid].ca_path, ca, sizeof(g_at_client_handle[linkid].ca_path));
    } else {
        g_at_client_handle[linkid].ca_path[0] = '\0';
    }

    if (cert) {
        strlcpy(g_at_client_handle[linkid].cert_path, cert, sizeof(g_at_client_handle[linkid].cert_path));
    } else {
        g_at_client_handle[linkid].cert_path[0] = '\0';
    }

    if (key) {
        strlcpy(g_at_client_handle[linkid].priv_key_path, key, sizeof(g_at_client_handle[linkid].priv_key_path));
    } else {
        g_at_client_handle[linkid].priv_key_path[0] = '\0';
    }
    return 0;
}
int at_net_ssl_server_path_set(int linkid, const char *ca, const char *cert, const char *key)
{
    if (linkid != 0) {
        return -1;
    }
    if (ca) {
        strlcpy(g_at_server_handle[linkid].ca_path, ca, sizeof(g_at_server_handle[linkid].ca_path));
    } else {
        g_at_server_handle[linkid].ca_path[0] = '\0';
    }

    if (cert) {
        strlcpy(g_at_server_handle[linkid].cert_path, cert, sizeof(g_at_server_handle[linkid].cert_path));
    } else {
        g_at_server_handle[linkid].cert_path[0] = '\0';
    }

    if (key) {
        strlcpy(g_at_server_handle[linkid].priv_key_path, key, sizeof(g_at_server_handle[linkid].priv_key_path));
    } else {
        g_at_server_handle[linkid].priv_key_path[0] = '\0';
    }
    return 0;
}

int at_net_ssl_path_get(int linkid, const char **ca, const char **cert, const char **key)
{
    *ca = g_at_client_handle[linkid].ca_path;
    *cert = g_at_client_handle[linkid].cert_path;
    *key = g_at_client_handle[linkid].priv_key_path;
    return 0;
}
int at_net_ssl_server_path_get(int linkid, const char **ca, const char **cert, const char **key)
{
    if (linkid != 0) {
        return -1;
    }
    *ca = g_at_server_handle[linkid].ca_path;
    *cert = g_at_server_handle[linkid].cert_path;
    *key = g_at_server_handle[linkid].priv_key_path;
    return 0;
}

int at_net_ssl_sni_set(int linkid, const char *sni)
{
    if (g_at_client_handle[linkid].ssl_hostname) {
        at_free(g_at_client_handle[linkid].ssl_hostname);
        g_at_client_handle[linkid].ssl_hostname = NULL;
    }
    if (sni == NULL || sni[0] == '\0') {
        at_config_delete_with_id(AT_CONFIG_KEY_NET_SNI, linkid);
        return 0;
    }
    g_at_client_handle[linkid].ssl_hostname = strdup(sni);
    if (g_at_client_handle[linkid].ssl_hostname == NULL) {
        return -1;
    }

    if (at->store) {
        at_config_write_with_id(AT_CONFIG_KEY_NET_SNI,
                                linkid,
                                g_at_client_handle[linkid].ssl_hostname,
                                strlen(g_at_client_handle[linkid].ssl_hostname));
    }
    return 0;
}

char *at_net_ssl_sni_get(int linkid)
{
    return g_at_client_handle[linkid].ssl_hostname;
}

int at_net_ssl_alpn_set(int linkid, int alpn_num, const char *alpn)
{
    net_lock();

    if (alpn == NULL) {
        at_free(g_at_client_handle[linkid].ssl_alpn[alpn_num]);
        g_at_client_handle[linkid].ssl_alpn[alpn_num] = NULL;
    } else {
        if (g_at_client_handle[linkid].ssl_alpn[alpn_num]) {
            at_free(g_at_client_handle[linkid].ssl_alpn[alpn_num]);
        }
        g_at_client_handle[linkid].ssl_alpn[alpn_num] = strdup(alpn);
        if (g_at_client_handle[linkid].ssl_alpn[alpn_num] == NULL) {
            net_unlock();
            return -1;
        }
    }

    g_at_client_handle[linkid].ssl_alpn_num = 0;
    for (int i = 0; i < 6; i++) {
        if (g_at_client_handle[linkid].ssl_alpn[i]) {
            g_at_client_handle[linkid].ssl_alpn_num++;
        }
    }
    net_unlock();
    return 0;
}

char **at_net_ssl_alpn_get(int linkid, int *alpn_num)
{
    if (alpn_num) {
        *alpn_num = g_at_client_handle[linkid].ssl_alpn_num;
    }
    return g_at_client_handle[linkid].ssl_alpn;
}

int at_net_ssl_psk_set(int linkid, char *psk, int psk_len, char *pskhint, int pskhint_len)
{
    strlcpy(g_at_client_handle[linkid].ssl_psk, psk, sizeof(g_at_client_handle[linkid].ssl_psk));
    g_at_client_handle[linkid].ssl_psklen = psk_len;

    strlcpy(g_at_client_handle[linkid].ssl_pskhint, pskhint, sizeof(g_at_client_handle[linkid].ssl_pskhint));
    g_at_client_handle[linkid].ssl_pskhint_len = pskhint_len;
    return 0;
}

int at_net_ssl_psk_get(int linkid, char **psk, int *psk_len, char **pskhint, int *pskhint_len)
{
    if (psk) {
        *psk = g_at_client_handle[linkid].ssl_psk;
    }
    if (psk_len) {
        *psk_len = g_at_client_handle[linkid].ssl_psklen;
    }

    if (pskhint) {
        *pskhint = g_at_client_handle[linkid].ssl_pskhint;
    }
    if (pskhint_len) {
        *pskhint_len = g_at_client_handle[linkid].ssl_pskhint_len;
    }
    return 0;
}

int at_net_ssl_server_psk_set(int linkid, char *psk, int psk_len, char *pskhint, int pskhint_len)
{
    strlcpy(g_at_server_handle[linkid].ssl_psk, psk, sizeof(g_at_server_handle[linkid].ssl_psk));
    g_at_server_handle[linkid].ssl_psklen = psk_len;

    strlcpy(g_at_server_handle[linkid].ssl_pskhint, pskhint, sizeof(g_at_server_handle[linkid].ssl_pskhint));
    g_at_server_handle[linkid].ssl_pskhint_len = pskhint_len;
    return 0;
}

int at_net_ssl_server_psk_get(int linkid, char **psk, int *psk_len, char **pskhint, int *pskhint_len)
{
    if (linkid < 0 || linkid >= AT_NET_SERVER_HANDLE_MAX) {
        return -1;
    }

    if (psk) {
        *psk = g_at_server_handle[linkid].ssl_psk;
    }

    if (psk_len) {
        *psk_len = g_at_server_handle[linkid].ssl_psklen;
    }

    if (pskhint) {
        *pskhint = g_at_server_handle[linkid].ssl_pskhint;
    }

    if (pskhint_len) {
        *pskhint_len = g_at_server_handle[linkid].ssl_pskhint_len;
    }

    return 0;
}

int at_string_host_to_ip(char *host, ip_addr_t *ip)
{
    struct hostent *hostinfo = gethostbyname(host);
    if (hostinfo) {
        *ip = *(ip_addr_t *)hostinfo->h_addr;

        #if defined(CFG_IPV6) && CFG_IPV6
        if (IP_IS_V6(ip) && ip_addr_islinklocal(ip)) {
            printf("linkloacl address:%s\r\n", host);
            //TODO: By default, this is associated with the STA netif.
            //Note: This modification will no longer be applicable when IPv6 is supported in AP mode.
            ip6_addr_assign_zone(ip_2_ip6(ip), IP6_UNICAST, netif_find("wl1"));
        }
        #endif
        return 0;
    }

    return -1;
}

int at_net_dns_load(void)
{
    if(at_net_config->dns.dns_isset==1){
        if (!ip_addr_isany(&at_net_config->dns.dns[0])) {
            dns_setserver(0, &at_net_config->dns.dns[0]);
        }
        if (!ip_addr_isany(&at_net_config->dns.dns[1])) {
            dns_setserver(1, &at_net_config->dns.dns[1]);
        }
        if (!ip_addr_isany(&at_net_config->dns.dns[2])) {
            dns_setserver(2, &at_net_config->dns.dns[2]);
        }
        return 0;
    }

    const ip_addr_t *current_dns1 = dns_getserver(0);
    const ip_addr_t *current_dns2 = dns_getserver(1);
    const ip_addr_t *current_dns3 = dns_getserver(2);

    at_net_config->dns.dns[0] = *current_dns1;
    at_net_config->dns.dns[1] = *current_dns2;
    at_net_config->dns.dns[2] = *current_dns3;

    int valid_dns_count = 0;
    if (!ip_addr_isany(current_dns1)) valid_dns_count++;
    if (!ip_addr_isany(current_dns2)) valid_dns_count++;
    if (!ip_addr_isany(current_dns3)) valid_dns_count++;

    if (valid_dns_count == 0) {
        ipaddr_aton(AT_CONFIG_DEFAULT_DNS1, &at_net_config->dns.dns[0]);
        ipaddr_aton(AT_CONFIG_DEFAULT_DNS2, &at_net_config->dns.dns[1]);
        ipaddr_aton("0.0.0.0", &at_net_config->dns.dns[2]);
    } else if (valid_dns_count == 1) {
        ipaddr_aton(AT_CONFIG_DEFAULT_DNS1, &at_net_config->dns.dns[1]);
        ipaddr_aton(AT_CONFIG_DEFAULT_DNS2, &at_net_config->dns.dns[2]);
    } else if (valid_dns_count == 2) {
        ipaddr_aton(AT_CONFIG_DEFAULT_DNS1, &at_net_config->dns.dns[2]);
    }

    dns_setserver(0, &at_net_config->dns.dns[0]);
    dns_setserver(1, &at_net_config->dns.dns[1]);
    dns_setserver(2, &at_net_config->dns.dns[2]);

    return 0;
}

int at_lwip_heap_free_size(void)
{
    int lwip_heap = 0;
#if MEM_STATS
    extern struct stats_ lwip_stats;
    lwip_heap = lwip_stats.mem.avail - lwip_stats.mem.used;
#endif
    return lwip_heap;
}

int at_net_poll_start(int interval_ms)
{
    int ret;

    if (interval_ms <= 0 || net_timer == NULL) {
       return -1;
    }

    g_at_server_handle[0].recv_timeout = interval_ms / 1000;

    xTimerChangePeriod(net_timer, interval_ms / portTICK_PERIOD_MS, portMAX_DELAY);

    ret = xTimerStart(net_timer, portMAX_DELAY);
    return (ret == pdPASS) ? 0 : -1;
}

int at_net_ssl_enc_storage_init(void)
{
    at_dir_t dir = at_fs_opendir("/");
    if (!dir) {
        return 0;
    }

    struct at_direct entry;
    while (at_fs_readdir(dir, &entry) == 0) {
        int name_len = strlen(entry.d_name);

        if ((name_len > 4 && strcmp(entry.d_name + name_len - 4, ".key") == 0) ||
            (name_len > 4 && strcmp(entry.d_name + name_len - 4, ".pem") == 0) ||
            strstr(entry.d_name, "_private.") != NULL) {
            if (at_net_ssl_save_file_encrypt(entry.d_name) != 0) {
                AT_NET_PRINTF("ssl key encrypt failed: %s\r\n", entry.d_name);
            }
        }
    }
    at_fs_closedir(dir);
    return 0;
}

int at_net_ssl_save_file_encrypt(const char *path)
{
    char *file_data = NULL;
    uint8_t *output_buf = NULL;
    int file_len = 0;
    uint32_t cipher_len = 0;
    int ret = -1;
    at_net_ssl_file_header_t header = {0};

    if (at_load_file(path, &file_data, &file_len) != 0) {
        goto exit;
    }

    if (memcmp(file_data, AT_NET_SSL_FILE_TAG, AT_NET_SSL_FILE_TAG_LEN) == 0) {
        ret = 0;
        goto exit;
    }

    if (mbedtls_ssl_verify_credential(file_data, file_len) != 0) {
        goto exit;
    }

    memcpy(header.tag, AT_NET_SSL_FILE_TAG, AT_NET_SSL_FILE_TAG_LEN);
    at_utils_crypto_get_random_iv(header.iv);
    memset(header.reserved, 0, AT_NET_SSL_FILE_RSV_LEN);

    output_buf = at_malloc(AT_NET_SSL_FILE_HEADER_LEN + ((file_len / AT_UTILS_CRYPTO_AES_BLOCK_SIZE) + 1) * AT_UTILS_CRYPTO_AES_BLOCK_SIZE);
    if (!output_buf) {
        goto exit;
    }

    if (at_utils_crypto_aes_cbc_encrypt_pad((uint8_t *)file_data, file_len,
                                            output_buf + AT_NET_SSL_FILE_HEADER_LEN,
                                            &cipher_len, header.iv) != 0) {
        goto exit;
    }

    memcpy(output_buf, &header, AT_NET_SSL_FILE_HEADER_LEN);

    int fd = at_fs_open(path, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd < 0) {
        goto exit;
    }
    at_fs_write(fd, output_buf, AT_NET_SSL_FILE_HEADER_LEN + cipher_len);
    at_fs_close(fd);
    ret = 0;

exit:
    at_free(file_data);
    at_free(output_buf);
    return ret;
}

int at_net_ssl_load_file_decrypt(const char *path, char **buf, int *len)
{
    char *file_data = NULL;
    int file_len = 0;

    if (at_load_file(path, &file_data, &file_len) != 0) {
        *buf = NULL;
        *len = 0;
        return -1;
    }

    if (file_len < AT_NET_SSL_FILE_HEADER_LEN ||
        memcmp(file_data, AT_NET_SSL_FILE_TAG, AT_NET_SSL_FILE_TAG_LEN) != 0) {
        *buf = file_data;
        *len = file_len;
        return 0;
    }

    uint8_t *cipher_data = (uint8_t *)file_data + AT_NET_SSL_FILE_HEADER_LEN;
    int cipher_len = file_len - AT_NET_SSL_FILE_HEADER_LEN;
    uint32_t plain_len = 0;

    if (at_utils_crypto_aes_cbc_decrypt_unpad(cipher_data, cipher_len,
                                             cipher_data, &plain_len,
                                             ((at_net_ssl_file_header_t *)file_data)->iv) != 0) {
        at_free(file_data);
        *buf = NULL;
        *len = 0;
        return -1;
    }

    memmove(file_data, cipher_data, plain_len);
    file_data[plain_len] = '\0';

    *buf = file_data;
    *len = plain_len;
    return 0;
}

int at_net_ssl_verify_credential_file(const char *path)
{
    char *data = NULL;
    int len = 0;
    int ret = -1;

    at_load_file(path, &data, &len);

    ret = mbedtls_ssl_verify_credential(data, len);
    at_free(data);
    return ret;
}

int at_net_ssl_verify_cert_key_pair(const char *cert_path, const char *key_path)
{
    char *cert_buf = NULL;
    char *key_buf = NULL;
    int cert_len = 0;
    int key_len = 0;
    int ret = -1;

    at_load_file(cert_path, &cert_buf, &cert_len);
    at_net_ssl_load_file_decrypt(key_path, &key_buf, &key_len);

    ret = mbedtls_ssl_verify_cert_key_match(cert_buf, cert_len, key_buf, key_len);

    at_free(cert_buf);
    at_free(key_buf);
    return ret;
}
