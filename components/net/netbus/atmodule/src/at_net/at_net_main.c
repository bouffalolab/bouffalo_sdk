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
#include <FreeRTOS.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include "lwip/ip_addr.h"
#include <stream_buffer.h>
#include <at_net_sntp.h>
#include <wifi_mgmr_ext.h>
#include <bflb_sec_trng.h>

#include "at_main.h"
#include "at_core.h"
#include "at_base_config.h"
#include "at_net_main.h"
#include "at_net_config.h"
#include "at_net_ssl.h"
#include "at_wifi_config.h"

#define AT_UDP_MAX_BUFFER_LEN      (1470)
#define AT_NET_TASK_STACK_SIZE     (1024)
#define AT_NET_TASK_PRIORITY_LOW   (27)
#define AT_NET_TASK_PRIORITY_HIGH  (28)
#define AT_NET_RECV_BUF_SIZE       (1536*3)
#define AT_NET_SEND_BUF_SIZE       (1024*8)
#define AT_NET_PRINTF              printf
#define AT_NET_DEBUG               printf

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
    uint8_t ssl_psklen;
    char ssl_pskhint[32];
    uint8_t ssl_pskhint_len;
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
}at_net_server_handle;

static at_net_client_handle *g_at_client_handle = NULL;
static at_net_server_handle *g_at_server_handle = NULL;
static SemaphoreHandle_t net_mutex;
static SemaphoreHandle_t net_reconnect_ref;
static uint8_t g_at_net_task_is_start = 0;
static uint8_t g_at_net_sntp_is_start = 0;
static char g_at_net_savelink_host[128];

static void sockaddr_to_ipaddr(struct sockaddr *sa, ip_addr_t *ipaddr) 
{
    if (sa->sa_family == AF_INET) {
        struct sockaddr_in *sa_in = (struct sockaddr_in *)sa;
        ip4_addr_set_u32(ip_2_ip4(ipaddr), sa_in->sin_addr.s_addr);
    }
#if CFG_IPV6
    else if (sa->sa_family == AF_INET6) {
        struct sockaddr_in6 *sa_in6 = (struct sockaddr_in6 *)sa;
        ip6_addr_set(ip_2_ip6(ipaddr), (const ip6_addr_t *)(&sa_in6->sin6_addr));
    } 
#endif
    else {
        printf("Unsupported address family: %d\n", sa->sa_family);
    }
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
#if CFG_IPV6
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
#if CFG_IPV6
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
    tv.tv_usec = (timeout_ms%1000)*10000;

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

static int so_recvsize_get(int fd)
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

static uint16_t udp_localport_rand(void)
{
    uint16_t port = 50000 + ((random())%10000);
    return port;
}

static int tcp_client_connect(ip_addr_t *ipaddr, uint16_t port, uint32_t timeout)
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

#if CFG_IPV6
    struct sockaddr_in6 addr6 = {0};
    if(IP_IS_V6(ipaddr)) {
        if ( (fd =  socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6))  < 0) {
            AT_NET_PRINTF("socket create failed\r\n");
            return -AT_SUB_CMD_EXEC_FAIL;
        }

        memcpy(&addr6.sin6_addr, ipaddr, sizeof(addr6.sin6_addr));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
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
#if CFG_IPV6
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
#if CFG_IPV6
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
#if CFG_IPV6
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

#if CFG_IPV6
    if (IP_IS_V6(ipaddr)) {
        struct sockaddr_in6 addr6;
        memset(&addr6, 0, sizeof(addr6));	
        addr6.sin6_family = AF_INET6;
        memcpy(&addr6.sin6_addr, ipaddr, sizeof(addr6.sin6_addr));
        addr6.sin6_port = htons(port);
        addr6.sin6_addr = in6addr_any;	
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
#if CFG_IPV6
        if (IP_IS_V6(ipaddr)) {
            struct sockaddr_in6 toaddr6;
            memset(&toaddr6, 0, sizeof(toaddr6));	
            toaddr6.sin6_family = AF_INET6;
            memcpy(&toaddr6.sin6_addr, ipaddr, sizeof(toaddr6.sin6_addr));
            toaddr6.sin6_port = htons(port);
            
            while (send_len < length) {

                ret = sendto(fd, ((uint8_t *)buffer) + send_len, (length - send_len > AT_UDP_MAX_BUFFER_LEN) ? AT_UDP_MAX_BUFFER_LEN : (length - send_len),
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

    AT_NET_PRINTF("ssl client connect %s:%d\r\n", ipaddr_ntoa(ipaddr), port);

    fd = tcp_client_connect(ipaddr, port, timeout);
    if (fd < 0) {
        AT_NET_PRINTF("tcp_client_connect fd:%d\r\n", fd);
        return fd;
    }

    at_load_file(g_at_client_handle[id].ca_path, &ssl_param.ca_cert, &ssl_param.ca_cert_len);
    at_load_file(g_at_client_handle[id].cert_path, &ssl_param.own_cert, &ssl_param.own_cert_len);
    at_load_file(g_at_client_handle[id].priv_key_path, &ssl_param.private_cert, &ssl_param.private_cert_len);
    
    ssl_param.alpn = at_net_ssl_alpn_get(id, &ssl_param.alpn_num);
    at_net_ssl_psk_get(id, &ssl_param.psk, &ssl_param.psk_len, &ssl_param.pskhint, &ssl_param.pskhint_len);
    ssl_param.sni = at_net_ssl_sni_get(id);

    handle = mbedtls_ssl_connect(fd, &ssl_param);
    if (handle == NULL) {
        AT_NET_PRINTF("mbedtls_ssl_connect handle NULL, fd:%d\r\n", fd);
        close(fd);
        free(ssl_param.ca_cert);
        free(ssl_param.own_cert);
        free(ssl_param.private_cert);
        return -AT_SUB_CMD_EXEC_FAIL;
    }

    *priv = handle;
    free(ssl_param.ca_cert);
    free(ssl_param.own_cert);
    free(ssl_param.private_cert);

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

#if CFG_IPV6
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

    on= 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#if CFG_IPV6
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

#if CFG_IPV6
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
#if CFG_IPV6
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
    if (at_wifi_config->wifi_mode == WIFI_STATION_MODE || at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
        uint32_t addr = 0;
    	wifi_sta_ip4_addr_get(&addr, NULL, NULL, NULL);
    	if (addr != IPADDR_ANY) {
    		return 1;
    	}
    } else if (at_wifi_config->wifi_mode == WIFI_SOFTAP_MODE) {

    }
    
    return 0;
}

static int net_socket_ipd(net_ipdinfo_type ipd, int id, void *buffer, int length, ip_addr_t *ipaddr, uint16_t port, uint32_t timeout)
{
    char tmp[10];
    char ipd_evt[64] = {"+IPD:"};

    if (ipd == NET_IPDINFO_CONNECTED) {
        
        if (tcp_connected()) {
            vTaskPrioritySet(NULL, AT_NET_TASK_PRIORITY_LOW);
        } else {
            vTaskPrioritySet(NULL, AT_NET_TASK_PRIORITY_HIGH);
        }

        if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT ||  at_base_config->sysmsg_cfg.bit.link_state_msg) {
            if (at_base_config->sysmsg_cfg.bit.link_msg_type) {
                char type[8];
                uint16_t local_port;
                uint8_t tetype;

                at_net_client_get_info(id, type, NULL, NULL, &local_port, &tetype);
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
                    //at_write(AT_NET_IPD_EVT_HEAD("+IPD:%d"), length);
                } else {
                    /* Use strncat + itoa instead of snprintf to improve performance. */
                    strncat(ipd_evt, itoa(id, tmp, 10), sizeof(ipd_evt) - strlen(ipd_evt));
                    strncat(ipd_evt, ",", sizeof(ipd_evt) - strlen(ipd_evt));
                    strncat(ipd_evt, itoa(length, tmp, 10), sizeof(ipd_evt) - strlen(ipd_evt));
                    strncat(ipd_evt, AT_NET_IPD_EVT_HEAD(""), sizeof(ipd_evt) - strlen(ipd_evt));
                    AT_CMD_DATA_SEND(ipd_evt, strlen(ipd_evt));
                    //at_write(AT_NET_IPD_EVT_HEAD("+IPD:%d,%d"), id, length);
                }
            } else {

                if (at_net_config->mux_mode == NET_LINK_SINGLE)
                    at_write(AT_NET_IPD_EVT_HEAD("+IPD:%d,\"%s\",%d"), length, ipaddr_ntoa(ipaddr), port);
                else
                    at_write(AT_NET_IPD_EVT_HEAD("+IPD:%d,%d,\"%s\",%d"), id, length, ipaddr_ntoa(ipaddr), port);
            }
            if (at_net_config->recv_mode != NET_RECV_MODE_PASSIVE) {
                memcpy(buffer + length, "\r\n", 2);
                length += 2;
                AT_CMD_DATA_SEND(buffer, length);
            }
        } else if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
            xStreamBufferSend(g_at_client_handle[id].recv_buf, buffer, length, timeout);
        } else {
            if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT) {
                memcpy(buffer + length, "\r\n", 2);
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
    g_at_client_handle[id].valid = 1;
    xSemaphoreGive(net_reconnect_ref);
    g_at_client_handle[id].type = type;
    g_at_client_handle[id].fd = fd;
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

    if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
        if (g_at_client_handle[id].recv_buf) {
            xStreamBufferReset(g_at_client_handle[id].recv_buf);
        } else {
            g_at_client_handle[id].recv_buf = xStreamBufferCreate(g_at_client_handle[id].recvbuf_size, 1);
        }
    }
    net_unlock();
    net_socket_ipd(NET_IPDINFO_CONNECTED, id, NULL, 0, &g_at_client_handle[id].remote_ip, g_at_client_handle[id].remote_port, 0);
    return 0;
}

static int net_socket_close_sync(int evtid, void *arg)
{
	int id = (int)arg;

    int valid = g_at_client_handle[id].valid;
    net_client_type type = g_at_client_handle[id].type;
    int fd = g_at_client_handle[id].fd;
    void *priv = g_at_client_handle[id].priv;
    int servid = 0;
    
    if (!valid) {
        AT_NET_PRINTF("socket is not inited\r\n");
        return -1;
    }

    net_lock();
    if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT) {
        g_at_client_handle[id].valid = 0;
        xSemaphoreTake(net_reconnect_ref, 0);
    }
    g_at_client_handle[id].fd = -1;
    g_at_client_handle[id].priv = NULL;
    g_at_client_handle[id].disconnect_time = os_get_time_ms();
  
    if (g_at_client_handle[id].tetype && g_at_server_handle[servid].valid) {
        g_at_server_handle[servid].client_num--;
        if (g_at_server_handle[servid].client_num < 0)
            g_at_server_handle[servid].client_num = 0;
    }
    net_unlock();

    if (type == NET_CLIENT_TCP)
       tcp_client_close(fd);
    else if (type == NET_CLIENT_UDP)
       udp_client_close(fd);
    else if (type == NET_CLIENT_SSL)
       ssl_client_close(fd, priv);

    net_socket_ipd(NET_IPDINFO_DISCONNECTED, id, NULL, 0, 0, 0, 0);
    return 0;
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

    return ret;
}

static int net_socket_recv(int id)
{
    int num = 0;
    struct sockaddr_in remote_addr;
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

        //update remote addr
        if (udp_mode == 1 || udp_mode == 2) {
            if (ip_2_ip4(&remote_ip) != remote_addr.sin_addr.s_addr || remote_port != ntohs(remote_addr.sin_port)) {
                net_lock();
                ip4_addr_set_u32(ip_2_ip4(&g_at_client_handle[id].remote_ip), remote_addr.sin_addr.s_addr);
                g_at_client_handle[id].remote_port = ntohs(remote_addr.sin_port);
                if (udp_mode == 1)
                    g_at_client_handle[id].udp_mode = 0;
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

    if (num <= 0 && type != NET_CLIENT_UDP) {
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
        g_at_client_handle[id].recv_time = os_get_time_ms();
    }

    return num;
}

static int net_socket_accept(int fd, int type, uint16_t port, uint16_t timeout, uint8_t num, uint8_t max_conn, uint8_t is_ipv6, int keepalive)
{
    int sock;
    struct sockaddr_in remote_addr;
#if CFG_IPV6
    struct sockaddr_in6 remote_addr6;
#endif
    socklen_t len;
    int id;
    void *priv = NULL;

    if (type == NET_SERVER_UDP) {
    	for (int i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
    		if (g_at_client_handle[id].valid && g_at_client_handle[id].fd == fd) {
    			return 0;
    		}
    	}
    	sock = fd;
    } else {
#if CFG_IPV6
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
            close(sock);
            return 0;
        }

        id = at_net_client_get_valid_id();
        if (id < 0) {
            close(sock);
            return 0;
        }

        if (type == NET_SERVER_SSL) {

            ssl_conn_param_t ssl_param = {0};

            at_load_file(g_at_server_handle[id].ca_path, &ssl_param.ca_cert, &ssl_param.ca_cert_len);
            at_load_file(g_at_server_handle[id].cert_path, &ssl_param.own_cert, &ssl_param.own_cert_len);
            at_load_file(g_at_server_handle[id].priv_key_path, &ssl_param.private_cert, &ssl_param.private_cert_len);
    
            priv = mbedtls_ssl_accept(sock, ssl_param.ca_cert, ssl_param.ca_cert_len, 
                                      ssl_param.own_cert, ssl_param.own_cert_len, 
                                      ssl_param.private_cert, ssl_param.private_cert_len);
            //priv = mbedtls_ssl_accept(sock, NULL, 0, NULL, 0, NULL, 0);
            if (priv == NULL) {
                close(sock);
                return 0;
            }
        }
        if (type == NET_SERVER_SSL || type == NET_SERVER_TCP) {
            so_keepalive_enable(sock, keepalive, 1, 3);
        }

        net_lock();
        g_at_client_handle[id].valid = 1;
        xSemaphoreGive(net_reconnect_ref);
        g_at_client_handle[id].type = type;
        g_at_client_handle[id].fd = sock;
        g_at_client_handle[id].priv = priv;
#if CFG_IPV6
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

        if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
            if (g_at_client_handle[id].recv_buf) {
                xStreamBufferReset(g_at_client_handle[id].recv_buf);
            } else {
                g_at_client_handle[id].recv_buf = xStreamBufferCreate(g_at_client_handle[id].recvbuf_size, 1);
            }
        }
        net_unlock();

        net_socket_ipd(NET_IPDINFO_CONNECTED, id, NULL, 0, &g_at_client_handle[id].remote_ip, g_at_client_handle[id].remote_port, 0);
        return 1;
    }

    return 0;
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
        if ((at_get_work_mode() != AT_WORK_MODE_THROUGHPUT) && (at_get_work_mode() != AT_WORK_MODE_CMD_THROUGHPUT)) {
            net_lock();
            g_at_client_handle[id].valid = 0;
            xSemaphoreTake(net_reconnect_ref, 0);
            net_unlock();
            return;
        }

        if (os_get_time_ms() - g_at_client_handle[id].disconnect_time <= at_net_config->reconn_intv*100) {
            vTaskDelay(at_net_config->reconn_intv*100);
            return;
        }
        g_at_client_handle[id].disconnect_time = os_get_time_ms();

        if (!net_is_active()) {
            vTaskDelay(at_net_config->reconn_intv*100);
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

        if (fd < 0)
            return;

        net_lock();
        g_at_client_handle[id].fd = fd;
        g_at_client_handle[id].priv = priv;
        g_at_client_handle[id].remote_ip = ipaddr;
        g_at_client_handle[id].remote_port = port;
        g_at_client_handle[id].local_port = local_port;
        if (at_net_config->recv_mode == NET_RECV_MODE_PASSIVE) {
            if (g_at_client_handle[id].recv_buf) {
                xStreamBufferReset(g_at_client_handle[id].recv_buf);
            } else {
                g_at_client_handle[id].recv_buf = xStreamBufferCreate(g_at_client_handle[id].recvbuf_size, 1);
            }
        }
        net_unlock();
    }
}

static void net_poll_recv(void)
{
    fd_set fdR;
    struct timeval timeout;
    int maxfd = -1;
    int i;

    FD_ZERO(&fdR);
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
#ifdef LP_APP
    if(select(maxfd+1, &fdR, NULL, NULL, NULL) > 0) {
#else
    if(select(maxfd+1, &fdR, NULL, NULL, &timeout) > 0) {
#endif
         for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
            if (g_at_client_handle[i].valid && g_at_client_handle[i].fd >= 0 && FD_ISSET(g_at_client_handle[i].fd, &fdR)) {
                net_socket_recv(i);
            }
        }
        for (i = 0; i < AT_NET_SERVER_HANDLE_MAX; i++) {
            if (g_at_server_handle[i].valid && FD_ISSET(g_at_server_handle[i].fd, &fdR)) {
                if (net_socket_accept(g_at_server_handle[i].fd, g_at_server_handle[i].type,
                            g_at_server_handle[i].port, g_at_server_handle[i].recv_timeout,
                            g_at_server_handle[i].client_num, g_at_server_handle[i].client_max, 
                            g_at_server_handle[i].is_ipv6, g_at_server_handle[i].keepalive) ) {
                    g_at_server_handle[i].client_num++;
                }
            }
        }
    }
}

static void net_poll_timeout(void)
{
    int i;

    for (i = 0; i < AT_NET_CLIENT_HANDLE_MAX; i++) {
        if (g_at_client_handle[i].valid && g_at_client_handle[i].recv_timeout > 0) {
            if ((os_get_time_ms() - g_at_client_handle[i].recv_time)/1000 > g_at_client_handle[i].recv_timeout)
                net_socket_close(i);
        }
    }
}

static void net_init_save_link(void)
{
    net_client_type type;
    int id;

    if (at_net_config->trans_link.enable) {
        if (strstr(at_net_config->trans_link.type, "TCP") != NULL)
            type = NET_CLIENT_TCP;
        else if (strstr(at_net_config->trans_link.type, "UDP") != NULL)
            type = NET_CLIENT_UDP;
        else if (strstr(at_net_config->trans_link.type, "SSL") != NULL)
            type = NET_CLIENT_SSL;
        else
            return;

        at_set_work_mode(AT_WORK_MODE_THROUGHPUT);
        at_net_config->work_mode = NET_MODE_TRANS;

        id = at_net_client_get_valid_id();
        net_lock();
        g_at_client_handle[id].valid = 1;
        xSemaphoreGive(net_reconnect_ref);
        g_at_client_handle[id].type = type;
        g_at_client_handle[id].fd = -1;
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
        if (!uxSemaphoreGetCount(net_reconnect_ref)) {
            xSemaphoreTake(net_reconnect_ref, portMAX_DELAY);
            xSemaphoreGive(net_reconnect_ref);
        }

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
    if (g_at_client_handle)
        return 0;

    g_at_client_handle = (at_net_client_handle *)pvPortMalloc(sizeof(at_net_client_handle) * AT_NET_CLIENT_HANDLE_MAX);
    if (!g_at_client_handle) {
        AT_NET_PRINTF("at_net malloc failed!\r\n");
        return -1;
    }

    g_at_server_handle = (at_net_server_handle *)pvPortMalloc(sizeof(at_net_server_handle) * AT_NET_SERVER_HANDLE_MAX);
    if (!g_at_server_handle) {
        AT_NET_PRINTF("at_net malloc failed!\r\n");
        vPortFree(g_at_client_handle);
        g_at_client_handle = NULL;
        return -1;
    }
    net_mutex = xSemaphoreCreateMutex();
    net_reconnect_ref = xSemaphoreCreateCounting(AT_NET_SERVER_HANDLE_MAX + AT_NET_CLIENT_HANDLE_MAX, 0);

    memset(g_at_client_handle, 0, sizeof(at_net_client_handle) * AT_NET_CLIENT_HANDLE_MAX);
    memset(g_at_server_handle, 0, sizeof(at_net_server_handle) * AT_NET_SERVER_HANDLE_MAX);

    for (int id = 0; id < AT_NET_CLIENT_HANDLE_MAX; id++) {
        g_at_client_handle[id].recvbuf_size = AT_NET_RECV_BUF_SIZE;
        at_net_ssl_path_set(id, at_net_config->sslconf[id].ca_file, 
                            at_net_config->sslconf[id].cert_file, 
                            at_net_config->sslconf[id].key_file); 
    }

    return 0;
}

static int at_net_deinit(void)
{
    if (g_at_client_handle) {
        vPortFree(g_at_client_handle);
        g_at_client_handle = NULL;

        vPortFree(g_at_server_handle);
        g_at_server_handle = NULL;
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

int at_net_client_get_info(int id, char *type, ip_addr_t *remote_ipaddr, uint16_t *remote_port, uint16_t *local_port, uint8_t *tetype)
{
    CHECK_NET_CLIENT_ID_VALID(id);

    if (g_at_client_handle[id].valid) {
        if (type) {
            if (g_at_client_handle[id].type == NET_CLIENT_TCP) {
#if CFG_IPV6
                if (IP_IS_V6(&g_at_client_handle[id].remote_ip)) {
                    strlcpy(type, "TCPv6", sizeof(type));
                } else 
#endif
                    strlcpy(type, "TCP", sizeof(type));
            } else if (g_at_client_handle[id].type == NET_CLIENT_UDP) {
#if CFG_IPV6
                if (IP_IS_V6(&g_at_client_handle[id].remote_ip)) {
                    strlcpy(type, "UDPv6", sizeof(type));
                } else 
#endif
                    strlcpy(type, "UDP", sizeof(type));
            } else if (g_at_client_handle[id].type == NET_CLIENT_SSL) {
#if CFG_IPV6
                if (IP_IS_V6(&g_at_client_handle[id].remote_ip)) {
                    strlcpy(type, "SSLv6", sizeof(type));
                } else 
#endif
                    strlcpy(type, "SSL", sizeof(type));
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
    xSemaphoreGive(net_reconnect_ref);
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
    xSemaphoreGive(net_reconnect_ref);
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
    xSemaphoreGive(net_reconnect_ref);
    g_at_server_handle[id].type = NET_SERVER_SSL;
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
    if (ca_enable) {
        *ca_enable = g_at_server_handle[id].ca_enable;
    }
    if (keepalive) {
        *keepalive = g_at_server_handle[id].keepalive;
    }
    return 1;
}

int at_net_server_close(void)
{
    int id = 0;
    int valid = g_at_server_handle[id].valid;
    net_server_type type = g_at_server_handle[id].type;
    int fd = g_at_server_handle[id].fd;;
    
    if (!valid) {
        AT_NET_PRINTF("socket is not inited\r\n");
        return -1;
    }

    net_lock();
    g_at_server_handle[id].valid = 0;
    xSemaphoreTake(net_reconnect_ref, 0);
    net_unlock();

    if (type == NET_SERVER_TCP || type == NET_SERVER_SSL)
       tcp_server_close(fd);

    return 0;
}

int at_net_throuthput_udp_linktype(int linkid)
{
    if ((!at_net_client_id_is_valid(linkid)) || (!at_net_client_is_connected(linkid))) {
        return 0;
    }
    return (g_at_client_handle[linkid].type == NET_CLIENT_UDP);
}

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
    at_net_client_close_all();
    if (g_at_net_task_is_start) {
        xSemaphoreGive(net_reconnect_ref);
        g_at_net_task_is_start = 2;
        while(g_at_net_task_is_start != 0)
            vTaskDelay(100);
    }
    at_net_deinit();
    return 0;
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

int at_net_ssl_path_get(int linkid, const char **ca, const char **cert, const char **key)
{
    *ca = g_at_client_handle[linkid].ca_path;
    *cert = g_at_client_handle[linkid].cert_path;
    *key = g_at_client_handle[linkid].priv_key_path;
    return 0;
}

int at_net_ssl_sni_set(int linkid, const char *sni)
{
    if (g_at_client_handle[linkid].ssl_hostname) {
        vPortFree(g_at_client_handle[linkid].ssl_hostname);
    }
    g_at_client_handle[linkid].ssl_hostname = strdup(sni);
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
        vPortFree(g_at_client_handle[linkid].ssl_alpn[alpn_num]);
        g_at_client_handle[linkid].ssl_alpn[alpn_num] = NULL;
    } else {
        if (g_at_client_handle[linkid].ssl_alpn[alpn_num]) {
            vPortFree(g_at_client_handle[linkid].ssl_alpn[alpn_num]);
        }
        g_at_client_handle[linkid].ssl_alpn[alpn_num] = strdup(alpn);
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

int at_string_host_to_ip(char *host, ip_addr_t *ip)
{
    struct hostent *hostinfo = gethostbyname(host);
    if (hostinfo) {
        *ip = *(ip_addr_t *)hostinfo->h_addr;
        return 0;
    }

    return -1;
}

int at_net_dns_load(void)
{
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

int at_lwip_heap_free_size(void)
{
    int lwip_heap = 0;
#if MEM_STATS 
    extern struct stats_ lwip_stats;
    lwip_heap = lwip_stats.mem.avail - lwip_stats.mem.used;
#endif 
    return lwip_heap;
}

