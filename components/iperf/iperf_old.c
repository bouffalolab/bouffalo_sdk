/**
* iperf-liked network performance tool
*
*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <lwip/sockets.h>
#include <lwip/udp.h>

#include <bflb_mtimer.h>
#include <utils_getopt.h>


#define IPERF_PORT_LOCAL    5002
#define IPERF_PORT          5001
#if defined(CFG_CHIP_BL808)
#define IPERF_BUFSZ         (16 * 1300)
#elif defined(CFG_CHIP_BL606P)
#define IPERF_BUFSZ         (16 * 1300)
#else
#define IPERF_BUFSZ         (4 * 1300)
#endif
#define IPERF_BUFSZ_UDP     (1 * 1300)
#define DEBUG_HEADER        "[NET] [IPC] "
#define DEFAULT_HOST_IP     "192.168.11.1"
#define IPERF_IP_LOCAL      "0.0.0.0"

#define TASK_PRIORITY_IPERF    (3)
static TaskHandle_t     iperf_task;
#define aos_task_new(name,task,para,stack_size)  xTaskCreate(task, name, stack_size, para, TASK_PRIORITY_IPERF, &iperf_task);

volatile int exit_flag = 0;

typedef struct UDP_datagram {
    uint32_t id;
    uint32_t tv_sec;
    uint32_t tv_usec;
} UDP_datagram;

typedef struct iperf_param {
    int port;
    char *host;
}iperf_param_t;

enum {
    /// TCP mode
    IPERF_TCP = 0,
    /// UDP mode
    IPERF_UDP
};

enum {
    /// Client mode
    IPERF_CLIENT = 0,
    /// Server mode
    IPERF_SERVER
};

/*
 * The server_hdr structure facilitates the server
 * report of jitter and loss on the client side.
 * It piggy_backs on the existing clear to close
 * packet.
 */
typedef struct server_hdr_v1 {
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     */
    int32_t flags;
    int32_t total_len1;
    int32_t total_len2;
    int32_t stop_sec;
    int32_t stop_usec;
    int32_t error_cnt;
    int32_t outorder_cnt;
    int32_t datagrams;
    int32_t jitter1;
    int32_t jitter2;
} server_hdr;

static void iperf_client_tcp(void *arg)
{
    int i;
    int sock;
    int ret;

    uint8_t *send_buf;
    int sentlen;
    uint32_t tick0, tick1, tick2;
    struct sockaddr_in addr;
    iperf_param_t *iperf_param = (iperf_param_t *)arg;
    uint64_t bytes_transfered = 0;

    char speed[64] = { 0 };
    float f_min = 8000.0, f_max = 0.0;

    exit_flag = 0;

    send_buf = (uint8_t *) pvPortMalloc (IPERF_BUFSZ);
    if (!send_buf) {
        goto __exit;
    }

    for (i = 0; i < IPERF_BUFSZ; i ++) {
        send_buf[i] = i & 0xff;
    }

    while (!exit_flag) {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0)
        {
            printf("create socket failed!\r\n");
            vTaskDelay(1000);
            continue;
        }

        addr.sin_family = PF_INET;
        addr.sin_port = htons(iperf_param->port);
        addr.sin_addr.s_addr = inet_addr(iperf_param->host);

        ret = connect(sock, (const struct sockaddr*)&addr, sizeof(addr));
        if (ret == -1)
        {
            printf("Connect failed!\r\n");
            closesocket(sock);

            vTaskDelay(1000);
            continue;
        }

        printf("Connect to iperf server successful!\r\n");

        {
            int flag = 1;

            setsockopt(sock,
                IPPROTO_TCP,     /* set option at TCP level */
                TCP_NODELAY,     /* name of option */
                (void *) &flag,  /* the cast is historical cruft */
                sizeof(int));    /* length of option value */
        }

        sentlen = 0;

        tick0 = xTaskGetTickCount();
        tick1 = tick0;
        while(!exit_flag) {
            tick2 = xTaskGetTickCount();
            if (tick2 - tick1 >= 1000 * 5)
            {
                float f_now, f_avg;

                f_now = (float)(sentlen)  / 125 / (((int32_t)tick2 - (int32_t)tick1)) * 1000;
                f_now /= 1000.0f;
                bytes_transfered += sentlen;
                f_avg = (float)(bytes_transfered)  / 125 / (((int32_t)tick2 - (int32_t)tick0)) * 1000;
                f_avg /= 1000.0f;

                if (f_now < f_min) {
                    f_min = f_now;
                }
                if (f_max < f_now) {
                    f_max = f_now;
                }
                snprintf(speed, sizeof(speed), "%.4f(%.4f %.4f %.4f) Mbps!\r\n",
                        f_now,
                        f_min,
                        f_avg,
                        f_max
                );
                printf("%s", speed);
                tick1 = tick2;
                sentlen = 0;
            }

            ret = send(sock, send_buf, IPERF_BUFSZ, 0);
            if (ret > 0)
            {
                sentlen += ret;
            }

            if (ret < 0) break;
        }

        closesocket(sock);

        vTaskDelay(1000*2);
        printf("disconnected!\r\n");
    }

__exit:
    exit_flag = 0;
    if (send_buf) vPortFree(send_buf);
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    printf("iper stop\r\n");
}

static void iperf_client_tcp_entry(const char *name)
{
    int host_len;
    iperf_param_t *iperf_param;

    iperf_param = pvPortMalloc(sizeof(iperf_param_t));
    if (iperf_param == NULL) {
        printf("[IPC] Malloc error\r\n");
        goto _ERROUT;
    }

    memset(iperf_param, 0, sizeof(iperf_param_t));

    host_len = strlen(name) + 4;
    iperf_param->host = pvPortMalloc(host_len);//mem will be free in tcpc_entry
    if (iperf_param->host == NULL) {
        printf("[IPC] Malloc error\r\n");
        goto _ERROUT;
    }
    if(strlcpy(iperf_param->host, name, host_len) >= host_len)
        printf("[OS]: strlcpy truncated \r\n");

    iperf_param->port = IPERF_PORT;

    aos_task_new("ipc", iperf_client_tcp, (void *)iperf_param, 4096);
    return;

_ERROUT:
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    return;
}

static void iperf_client_udp(void *arg)
{
    int i;
    int sock = -1;
    int ret = -1;

    uint8_t *send_buf;
    int sentlen;
    uint64_t bytes_transfered = 0;
    uint32_t tick0, tick1, tick2;
    struct sockaddr_in laddr, raddr;
    UDP_datagram udp_header, *udp_header_buf;
    iperf_param_t *iperf_param = (iperf_param_t *)arg;

    char speed[64] = { 0 };
    float f_min = 8000.0, f_max = 0.0;

    exit_flag = 0;

    send_buf = (uint8_t *) pvPortMalloc (IPERF_BUFSZ_UDP);
    if (!send_buf) {
        goto __exit;
    }

    for (i = 0; i < IPERF_BUFSZ_UDP; i ++) {
        send_buf[i] = i & 0xff;
    }

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0)
    {
        printf("create socket failed!\r\n");
        goto __exit;
    }

    memset(&laddr, 0, sizeof(struct sockaddr_in));
    laddr.sin_family = PF_INET;
    laddr.sin_port = htons(IPERF_PORT_LOCAL);
    laddr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sock, (struct sockaddr*)&laddr, sizeof(laddr));
    if (ret == -1)
    {
        printf("Bind failed!\r\n");
        goto __exit;
    }

    printf("bind UDP socket successfully!\r\n");

    memset(&raddr, 0, sizeof(struct sockaddr_in));
    raddr.sin_family = PF_INET;
    raddr.sin_port = htons(iperf_param->port);
    raddr.sin_addr.s_addr = inet_addr(iperf_param->host);

    sentlen = 0;

    udp_header_buf = (UDP_datagram*)send_buf;
    udp_header.id = 0;
    udp_header.tv_sec = 0;
    udp_header.tv_usec = 0;

    tick0 = xTaskGetTickCount();
    tick1 = tick0;
    while (!exit_flag) {
        tick2 = xTaskGetTickCount();
        if (tick2 - tick1 >= 1000 * 5)
        {
            float f_now, f_avg;

            f_now = (float)(sentlen)  / 125 / (((int32_t)tick2 - (int32_t)tick1)) * 1000;
            f_now /= 1000.0f;
            bytes_transfered += sentlen;
            f_avg = (float)(bytes_transfered)  / 125 / (((int32_t)tick2 - (int32_t)tick0)) * 1000;
            f_avg /= 1000.0f;

            if (f_now < f_min) {
                f_min = f_now;
            }
            if (f_max < f_now) {
                f_max = f_now;
            }
            snprintf(speed, sizeof(speed), "%.4f(%.4f %.4f %.4f) Mbps!\r\n",
                    f_now,
                    f_min,
                    f_avg,
                    f_max
            );
            printf("%s", speed);
            tick1 = tick2;
            sentlen = 0;
        }

        udp_header.id++;
        udp_header_buf->id = htonl(udp_header.id);
        udp_header_buf->tv_sec = 0;
        udp_header_buf->tv_usec = 0;
retry:
        ret = sendto(sock, send_buf, IPERF_BUFSZ_UDP, 0, (const struct sockaddr*)&raddr, sizeof(raddr));
        if (ret > 0) {
            sentlen += ret;
        }

        if (ret < 0) {
            if (ERR_MEM == ret) {
                goto retry;
            }
            break;
        }
    }

__exit:
    if (sock >= 0) lwip_close(sock);

    vTaskDelay(1000*2);
    exit_flag = 0;

    if (send_buf) vPortFree(send_buf);
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    printf("disconnected! ret %d\r\n",  ret);
    vTaskDelete(NULL);
}

struct iperf_server_udp_ctx {
    //SemaphoreHandle_t comp_sig_handle;
    volatile int exit_flag;
    uint32_t datagram_cnt;
    int32_t packet_id;
    uint32_t out_of_order_cnt, error_cnt, out_of_order_curr;
    uint32_t receive_start, period_start, current;
    uint64_t recv_total_len, recv_now;
    float f_min, f_max;
};

// Convert network byte order to host byte order, reading directly from memory
#define NTOHL_PTR(ptr)                     \
  ({                                       \
    uint32_t _tmp = 0;                     \
    _tmp |= *((uint8_t *)(ptr) + 0) << 24; \
    _tmp |= *((uint8_t *)(ptr) + 1) << 16; \
    _tmp |= *((uint8_t *)(ptr) + 2) << 8;  \
    _tmp |= *((uint8_t *)(ptr) + 3) << 0;  \
  })

// Convert host byte order to network byte order, writing directly to memory
#define HTONL_PTR(ptr, data)                       \
  {                                                \
    uint32_t _tmp = (uint32_t)(data);              \
    *((uint8_t *)(ptr) + 0) = (_tmp >> 24) & 0xff; \
    *((uint8_t *)(ptr) + 1) = (_tmp >> 16) & 0xff; \
    *((uint8_t *)(ptr) + 2) = (_tmp >> 8) & 0xff;  \
    *((uint8_t *)(ptr) + 3) = (_tmp >> 0) & 0xff;  \
  }

static void iperf_server_udp_recv_fn(void *arg, struct udp_pcb *pcb, struct pbuf *p,
    const ip_addr_t *addr, u16_t port)
{
    struct iperf_server_udp_ctx *ctx = (struct iperf_server_udp_ctx *)arg;
    char speed[64] = { 0 };
    UDP_datagram udp_header;

    // Receive data and wait for the reception timeout
    if (p == NULL)
        return;

    ctx->current = bflb_mtimer_get_time_us();
    ctx->receive_start = ctx->receive_start ? : ctx->current;
    ctx->period_start = ctx->period_start ? : ctx->current;

    // Record the total number of bytes received so far: payload + Ethernet Link + IP header + UDP header
    ctx->recv_now += p->tot_len + PBUF_LINK_HLEN + PBUF_IP_HLEN + PBUF_TRANSPORT_HLEN;

    ctx->datagram_cnt ++;

    // Obtain the packet ID
    udp_header.id = NTOHL_PTR(p->payload);
    if ((signed)udp_header.id < 0) {    // Transmission completed, client waiting for response
        server_hdr *hdr = (server_hdr *)((UDP_datagram *)p->payload + 1);   // Server header follows the UDP datagram
        HTONL_PTR(&hdr->flags, 0x80000000u);
        HTONL_PTR(&hdr->total_len1, 0);
        HTONL_PTR(&hdr->total_len2, ctx->recv_total_len);   // Lower 32 bits
        HTONL_PTR(&hdr->stop_sec, 0);
        HTONL_PTR(&hdr->stop_usec, 0);
        HTONL_PTR(&hdr->error_cnt, ctx->error_cnt);
        HTONL_PTR(&hdr->outorder_cnt, ctx->out_of_order_cnt);
        HTONL_PTR(&hdr->datagrams, ctx->datagram_cnt);

        printf("iperf finish...\r\nreceive:%" PRId32 ",out of order:%" PRId32 "\r\n",
            ctx->datagram_cnt, ctx->out_of_order_cnt);
        udp_sendto(pcb, p, addr, port);

        //xSemaphoreGive(ctx->comp_sig_handle);
        ctx->exit_flag = 1;
    } else if (ctx->current - ctx->period_start >= 500*1000) {  // 500ms
        float f_now, f_avg;

        // f_now = (float)(ctx->recv_now)  / 125 / (((int32_t)ctx->current - (int32_t)ctx->period_start)) * 1000;
        // f_now /= 1000.0f;
        f_now = (float)(ctx->recv_now) * 1000000 / ((int32_t)ctx->current - (int32_t)ctx->period_start) / 131072;

        ctx->recv_total_len += ctx->recv_now;
        // f_avg = (float)(ctx->recv_total_len)  / 125 / (((int32_t)ctx->current - (int32_t)ctx->receive_start)) * 1000;
        // f_avg /= 1000.0f;
        f_avg = (float)(ctx->recv_total_len) * 1000000 / ((int32_t)ctx->current - (int32_t)ctx->receive_start) / 131072;

        if (f_now < ctx->f_min) {
            ctx->f_min = f_now;
        }
        if (ctx->f_max < f_now) {
            ctx->f_max = f_now;
        }
        snprintf(speed, sizeof(speed), "%.4f(%.4f %.4f %.4f) Mbps, out of order: %" PRId32 ".\r\n",
                f_now,
                ctx->f_min,
                f_avg,
                ctx->f_max,
                ctx->out_of_order_curr
        );
        printf("%s", speed);

        ctx->period_start = ctx->current;
        ctx->recv_now = 0;

        ctx->out_of_order_cnt += ctx->out_of_order_curr;
        ctx->out_of_order_curr = 0;
    }

    // Number of out-of-order packets and errors
    if ((signed)udp_header.id != ctx->packet_id + 1) {
      if ((signed)udp_header.id < ctx->packet_id + 1) {
        ctx->out_of_order_curr++;
      } else {
        ctx->error_cnt += (signed)udp_header.id  - ctx->packet_id - 1;
      }
    }

    if ((signed)udp_header.id > ctx->packet_id) {
      ctx->packet_id = (int32_t)udp_header.id;
    }

	pbuf_free(p);
}

static void iperf_server_udp(void *arg)
{
    iperf_param_t *iperf_param = (iperf_param_t *)arg;
    struct udp_pcb *server = NULL;
    err_t ret;
    ip_addr_t source_ip;
    struct iperf_server_udp_ctx context;

    configASSERT(arg != NULL);

    // FIXME bug here: Creating PCB control blocks outside lwIP thread context
    server = udp_new();
    if (!server) {
        printf("Create UDP Control block failed!\r\n");
        goto _exit;
    }

    ipaddr_aton(IPERF_IP_LOCAL, &source_ip);
    ret = udp_bind(server, &source_ip, iperf_param->port);
    if (ret != ERR_OK) {
        printf("Bind failed!\r\n");
        goto _exit;
    }

    printf("bind UDP socket successfully!\r\n");

    memset(&context, 0, sizeof context);
    context.f_min = 8000.0;
    context.packet_id = -1;
    udp_recv(server, iperf_server_udp_recv_fn, (void *)&context);

    // Wait for the reception of an exit signal
    while (!context.exit_flag) {
        vTaskDelay(1000);
    }

_exit:
    if (server) udp_remove(server);
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);  

    printf("ipus exit..\r\n");
}

static void iperf_client_udp_entry(const char *name)
{
    int host_len;
    iperf_param_t *iperf_param;

    iperf_param = pvPortMalloc(sizeof(iperf_param_t));
    if (iperf_param == NULL) {
        printf("[IPC] Malloc error\r\n");
        goto _ERROUT;
    }

    memset(iperf_param, 0, sizeof(iperf_param_t));

    host_len = strlen(name) + 4;
    iperf_param->host = pvPortMalloc(host_len);//mem will be free in tcpc_entry
    if (iperf_param->host == NULL) {
        printf("[IPC] Malloc error\r\n");
        goto _ERROUT;
    }
    if(strlcpy(iperf_param->host, name, host_len) >= host_len);
        printf("[OS]: strlcpy truncated \r\n");

    iperf_param->port = IPERF_PORT;

    aos_task_new("ipu", iperf_client_udp, (void *)iperf_param, 4096);
    return;

_ERROUT:
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    return;
}

static void iperf_server_udp_entry(const char *name)
{

    iperf_param_t *iperf_param;

    iperf_param = pvPortMalloc(sizeof(iperf_param_t));
    if (iperf_param == NULL) {
        printf("[IPC] Malloc error\r\n");
        goto _ERROUT;
    }

    memset(iperf_param, 0, sizeof(iperf_param_t));

    iperf_param->port = IPERF_PORT;

    aos_task_new("ipus", iperf_server_udp, (void *)iperf_param, 4096);
    return;

_ERROUT:
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    return;
}

static void iperf_server(void *arg)
{
    uint8_t *recv_data;
    uint32_t sin_size;
    uint32_t tick0, tick1, tick2;
    int sock = -1, connected, bytes_received, recvlen;
    struct sockaddr_in server_addr, client_addr;
    char speed[64] = { 0 };
    iperf_param_t *iperf_param = (iperf_param_t *)arg;
    uint64_t bytes_transfered = 0;
    float f_min = 8000.0, f_max = 0.0;
    exit_flag = 0;

    recv_data = (uint8_t *)pvPortMalloc(IPERF_BUFSZ);
    if (recv_data == NULL)
    {
        printf("No memory\r\n");
        goto __exit;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket error\r\n");
        goto __exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(iperf_param->port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Unable to bind\r\n");
        goto __exit;
    }

    if (listen(sock, 5) == -1) {
        printf("Listen error\r\n");
        goto __exit;
    }

    while (!exit_flag) {
        sin_size = sizeof(struct sockaddr_in);

        connected = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *)&sin_size);

        printf("new client connected from (%s, %d)\r\n",
                  inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

        {
            int flag = 1;

            setsockopt(connected,
                IPPROTO_TCP,     /* set option at TCP level */
                TCP_NODELAY,     /* name of option */
                (void *) &flag,  /* the cast is historical cruft */
                sizeof(int));    /* length of option value */
        }

        recvlen = 0;
        tick0 = xTaskGetTickCount();
        tick1 = tick0;
        while (!exit_flag) {
            bytes_received = recv(connected, recv_data, IPERF_BUFSZ, 0);
            if (bytes_received <= 0) break;

            recvlen += bytes_received;

            tick2 = xTaskGetTickCount();
            if (tick2 - tick1 >= 1000 * 5)
            {
                float f_now, f_avg;

                f_now = (float)(recvlen)  / 125 / (((int32_t)tick2 - (int32_t)tick1)) * 1000;
                f_now /= 1000.0f;
                bytes_transfered += recvlen;
                f_avg = (float)(bytes_transfered)  / 125 / (((int32_t)tick2 - (int32_t)tick0)) * 1000;
                f_avg /= 1000.0f;

                if (f_now < f_min) {
                    f_min = f_now;
                }
                if (f_max < f_now) {
                    f_max = f_now;
                }
                snprintf(speed, sizeof(speed), "%.4f(%.4f %.4f %.4f) Mbps!\r\n",
                        f_now,
                        f_min,
                        f_avg,
                        f_max
                );
                printf("%s", speed);
                tick1 = tick2;
                recvlen = 0;
            }
        }

        if (connected >= 0) closesocket(connected);
        connected = -1;
    }

__exit:
    if (sock >= 0) closesocket(sock);
    if (recv_data) vPortFree(recv_data);
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    exit_flag = 0;
    printf("ips exit..\r\n");
}

static void iperf_server_entry(const char *name)
{
    iperf_param_t *iperf_param;

    iperf_param = pvPortMalloc(sizeof(iperf_param_t));
    if (iperf_param == NULL) {
        printf("[IPC] Malloc error\r\n");
        goto _ERROUT;
    }

    memset(iperf_param, 0, sizeof(iperf_param_t));

    iperf_param->port = IPERF_PORT;

    aos_task_new("ips", iperf_server, (void *)iperf_param, 4096);
    return;

_ERROUT:
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    return;
}

static void ipc_test_cmd(/*char *buf, int len, */int argc, char **argv)
{
    if (1 == argc) {
        printf(DEBUG_HEADER "[IPC] Connecting with default address " DEFAULT_HOST_IP "\r\n");
        iperf_client_tcp_entry(DEFAULT_HOST_IP);
    } else if (2 == argc) {
        printf(DEBUG_HEADER "[IPC] Connecting with default address %s\r\n", argv[1]);
        iperf_client_tcp_entry(argv[1]);
    } else {
        printf(DEBUG_HEADER  "[IPC] illegal address\r\n");
    }
}

static void ips_test_cmd(/*char *buf, int len, */int argc, char **argv)
{
    if (1 == argc) {
        puts(DEBUG_HEADER "[IPS] Starting iperf server on 0.0.0.0\r\n");
        iperf_server_entry(IPERF_IP_LOCAL);
    } else if (2 == argc) {
        iperf_server_entry(argv[1]);
    } else {
        printf(DEBUG_HEADER  "[IPS] illegal address\r\n");
    }
}

static void ipu_test_cmd(/*char *buf, int len,*/ int argc, char **argv)
{
    if (1 == argc) {
        printf(DEBUG_HEADER "[IPU] Connecting with default address " DEFAULT_HOST_IP "\r\n");
        iperf_client_udp_entry(DEFAULT_HOST_IP);
    } else if (2 == argc) {
        printf(DEBUG_HEADER "[IPU] Connecting with default address %s\r\n", argv[1]);
        iperf_client_udp_entry(argv[1]);
    } else {
        printf(DEBUG_HEADER  "[IPU] illegal address\r\n");
    }
}

// iperf UDP Server
static void ipus_test_cmd(/*char *buf, int len, */int argc, char **argv)
{
    if (1 == argc) {
        printf(DEBUG_HEADER "[IPUS] Connecting with default address 0.0.0.0\r\n");
        iperf_server_udp_entry(IPERF_IP_LOCAL);
    } else if (2 == argc) {
        iperf_server_udp_entry(argv[1]);
    } else {
        printf(DEBUG_HEADER  "[IPUS] illegal address\r\n");
    }
}

static void iperf_exit_cmd(/*char *buf, int len, */int argc, char **argv)
{
    exit_flag = 1;
}

static void iperf_cmd(/*char *buf, int len, */int argc, char **argv)
{
    iperf_param_t *iperf_param;
    getopt_env_t getopt_env;
    int opt, host_len, server_client = IPERF_CLIENT, tcp_udp = IPERF_TCP;

    iperf_param = pvPortMalloc(sizeof(iperf_param_t));
    if (iperf_param == NULL) {
        printf("[IPC] Malloc error\r\n");
        goto _ERROUT;
    }

    memset(iperf_param, 0, sizeof(iperf_param_t));
    utils_getopt_init(&getopt_env, 0);

    while ((opt = utils_getopt(&getopt_env, argc, argv, "c:p:su")) != -1) {
        switch (opt) {
        case 'c':
            host_len = strlen(getopt_env.optarg) + 4;
            iperf_param->host = pvPortMalloc(host_len);//mem will be free in tcpc_entry
            if (NULL == iperf_param->host) {
                printf("[TCPC] run out of mem for host alloc\r\n");
                goto _ERROUT;
            }
            if(strlcpy(iperf_param->host, getopt_env.optarg, host_len) >= host_len)
                printf("[OS]: strlcpy truncated \r\n");
            break;

        case 'p':
            iperf_param->port = atoi(getopt_env.optarg);
            break;

        case 's':
            server_client = IPERF_SERVER;
            break;

        case 'u':
            tcp_udp = IPERF_UDP;
            break;

        case '?':
            printf("unknow option: %c\r\n", getopt_env.optopt);
            goto _ERROUT;
        }
    }

    if ((NULL != iperf_param->host) && (server_client == IPERF_SERVER)) {
        printf("server and client can't be set at the time\r\n");
        goto _ERROUT;
    }

    if (iperf_param->port == 0) {
        iperf_param->port = IPERF_PORT;
    }

    if (server_client == IPERF_CLIENT) {
        if (NULL == iperf_param->host) {
            host_len = strlen(DEFAULT_HOST_IP) + 4;
            iperf_param->host = pvPortMalloc(host_len);//mem will be free in tcpc_entry
            if (NULL == iperf_param->host) {
                printf("[TCPC] run out of mem for host alloc\r\n");
                goto _ERROUT;
            }
            if(strlcpy(iperf_param->host, DEFAULT_HOST_IP, host_len) >= host_len)
                printf("[OS]: strlcpy truncated \r\n");
        }

        if (tcp_udp == IPERF_TCP) {
            printf(DEBUG_HEADER "[IPC] Connecting with default address %s port %d\r\n", iperf_param->host ? iperf_param->host: DEFAULT_HOST_IP, iperf_param->port);
            aos_task_new("ipc", iperf_client_tcp, (void *)iperf_param, 4096);
        } else if (tcp_udp == IPERF_UDP) {
            printf(DEBUG_HEADER "[IPU] Connecting with default address %s port %d\r\n", iperf_param->host ? iperf_param->host: DEFAULT_HOST_IP, iperf_param->port);
            aos_task_new("ipu", iperf_client_udp, (void *)iperf_param, 4096);
        }
        return;
    } else if (server_client == IPERF_SERVER) {
        if (tcp_udp == IPERF_TCP) {
            printf(DEBUG_HEADER "[IPS] Starting iperf server on %s port %d\r\n", IPERF_IP_LOCAL, iperf_param->port);
            aos_task_new("ips", iperf_server, (void *)iperf_param, 4096);
        } else if (tcp_udp == IPERF_UDP) {
            printf(DEBUG_HEADER "[IPUS] Starting iperf server on %s port %d\r\n", IPERF_IP_LOCAL, iperf_param->port);
            aos_task_new("ipus", iperf_server_udp, (void *)iperf_param, 4096);
        }
        return;
    }
    
_ERROUT:
    if (iperf_param->host) vPortFree(iperf_param->host);
    if (iperf_param) vPortFree(iperf_param);
    printf("[USAGE]: %s [-s] [-c <host>] [-p <port>] [-u]\r\n", argv[0]);
    return;
}

#ifdef CONFIG_SHELL
#include <shell.h>
SHELL_CMD_EXPORT_ALIAS(ipc_test_cmd, ipc, iperf TCP client);
SHELL_CMD_EXPORT_ALIAS(ips_test_cmd, ips, iperf TCP server);
SHELL_CMD_EXPORT_ALIAS(ipu_test_cmd, ipu, iperf UDP client);
SHELL_CMD_EXPORT_ALIAS(ipus_test_cmd, ipus, iperf UDP server);
SHELL_CMD_EXPORT_ALIAS(iperf_exit_cmd, iperf_stop, stop iperf);
SHELL_CMD_EXPORT_ALIAS(iperf_cmd, iperf, iperf command);
#endif
