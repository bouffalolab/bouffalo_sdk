/**
 ****************************************************************************************
 *
 * @file net_tg_al.c
 *
 * @brief Definition of the networking stack abstraction layer used for traffic generator.
 *
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <assert.h>
#include <string.h>

#include "net_tg_al_priv.h"
#include "net_def.h"
#include "net_al.h"

#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/icmp.h"
#include "lwip/igmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/tcpip.h"

#define co_ntohs ntohs
#define co_htons htons
#define co_ntohl ntohl
#define co_htonl htonl
int get_time_SINCE_EPOCH(uint32_t *sec, uint32_t *usec);

/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// Maximum length of UDP payload
#define NET_MAX_UDP_LEN 1500 - UDP_HLEN - IP_HLEN
/// Multicast test rate is fixed at 50 frames/sec
#define NET_MCAST_FRATE            50
/// TG frame header size
#define NET_TG_HEADER_SIZE         20
/// TG frame total header size
#define NET_TG_HEADER_TOTAL_SIZE   LWIP_MEM_ALIGN_SIZE( \
        NET_TG_HEADER_SIZE + PBUF_LINK_ENCAPSULATION_HLEN + PBUF_LINK_HLEN + \
        PBUF_IP_HLEN + PBUF_TRANSPORT_HLEN)
/// Minimum transmission time for sending process (1 us)
#define NET_TG_MIN_SEND_TIME       1000000
/// Maximum transmission time for sending process (340 s)
#define NET_TG_MAX_SEND_TIME       340000000

/// Structure containing the information about the TG sending buffer
struct net_tg_send_info
{
    /// Pointer to TG stream
    struct fhost_tg_stream *stream;
    /// Flag to describe if this buffer is occupied : 1 for occupied, 0 for free
    int flag;
};
/// TG sending process environment structure
struct net_tg_send_buf_tag
{
    /// Network stack buffer element for TG payload
    struct pbuf_custom p;
    /// Structure containing the information about the TG sending buffer
    struct net_tg_send_info info;
    /// Network stack buffer element for TG header
    struct pbuf_custom h;
    /// TG header string
    char hdr[NET_TG_HEADER_TOTAL_SIZE];
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// Payload to send (no copy memory) which is pointed by pbuf->payload
static char net_udp_send_nocopy_buf[NET_MAX_UDP_LEN] __attribute__ ((section("SHAREDRAM")));
/// Pool of TG send buffers
static struct net_tg_send_buf_tag
net_tg_send_buf_mem[FHOST_TG_MAX_TRAFFIC_STREAMS][FHOST_TG_SEND_BUF_CNT] __attribute__ ((section("SHAREDRAM")));

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Calculate the sleep time for different frame rate
 *
 * It should be done according the device
 * This is just one way to adjust the packet delivery speed. If you find
 * you device does not meet the test requirements, you MUST re-adjust
 * the method.
 *
 * @param[in]  profile         Profile of stream
 * @param[in]  rate            Frame rate packets / sec
 * @param[out] sleep_time      Interval sleep time (in us)
 * @param[out] throttled_rate  Frames to send in each interval
 ****************************************************************************************
 **/
void net_sleep_time(enum profile_id prof, u32_t rate,
                           u32_t *sleep_time, u32_t *throttled_rate)
{
    u32_t throttled_rate_int = 0;
    // in microseconds
    *sleep_time = 0;
    // calculate the sleep time based on frame rate
    switch(prof)
    {
        case FHOST_PROF_MCAST :
        if(rate < 500 && rate >= 50)
        {
            // sleep for 100 ms
            *sleep_time = 100000;
            throttled_rate_int = NET_MCAST_FRATE;
        }
        else
        {   // rate < 50
            *sleep_time = 100000;
            throttled_rate_int = rate;
        }
        break;

        case FHOST_PROF_IPTV :
        case FHOST_PROF_FILE_TX :
        if (rate > 50)
        {
        /*
         * this sleep_time indeed is now being used for time period
         * to send packets in the throttled Rate.
         * The idea here is that in each fixed 20 millisecond period,
         * The device will send rate/50 (rate = packets / second),
         * then go sleep for rest of time.
         */
            *sleep_time = 20000; /* fixed 20 milliseconds */
            throttled_rate_int = rate / 50;
        }
        else if (rate == 0)
        {
            *sleep_time = 20000; /* fixed 20 milliseconds */
            throttled_rate_int = 10000 / 50;
        }
        else if (rate > 0 && rate <= 50) /* typically for voice */
        {
            *sleep_time = 1000000 / rate;
            throttled_rate_int = 1;
        }
        break;
        // Ping command profile @ref fhost_ping.h
        case FHOST_PROF_PING :
        throttled_rate_int = 1;
        if (rate > 50)
        {
            // We set the maximum rate at 50 pkts / s
            *sleep_time = 20000;
        }
        else if (rate > 0 && rate <= 50)
        {
            *sleep_time = 1000000 / rate;
        }
        else if (rate == 0)
        {
            *sleep_time = 1000000;
        }
        break;

        default:
            TRACE_APP(ERR, "[TG] Invalid profile %d", prof);
        break;
    }

    if (throttled_rate)
    {
        *throttled_rate = throttled_rate_int;
    }
}

/**
 ****************************************************************************************
 * @brief Calculate the time difference
 *
 * @param[in] t1  TG time structure
 * @param[in] t2  TG time structure
 *
 * @return the time difference t2 - t1 in us (could be negative)
 ****************************************************************************************
 **/
int net_time_diff(struct tg_time *t1, struct tg_time *t2)
{
    int dtime;
    int sec = t2->sec - t1->sec;
    int usec = t2->usec - t1->usec;

    if (usec < 0)
    {
        sec -=1;
        usec += 1000000;
    }

    dtime = sec * 1000000 + usec;
    return dtime;
}

/**
 ****************************************************************************************
 * @brief Callback function called when we free the pbuf_custom of TG header
 *
 * Currently it does nothing.
 *
 * @param[in] p  Pointer to a pbuf_custom buffer to free
 ****************************************************************************************
 **/
static void net_tg_pbuf_custom_header_free_cb(void *p)
{
}

/**
 ****************************************************************************************
 * @brief Callback function called when we free the pbuf_custom of TG payload
 *
 * It will increase credit and set flag to 0.
 *
 * @param[in] p  Pointer to a pbuf_custom buffer to free
 ****************************************************************************************
 **/
static void net_tg_pbuf_custom_payload_free_cb(void *p)
{
    struct net_tg_send_buf_tag *buf = p;
    struct fhost_tg_stream *stream = buf->info.stream;
    rtos_mutex m = stream->tg_mutex;
    // Avoid sending process to modify it in the same time
    rtos_mutex_lock(m);
    buf->info.flag = 0;
    stream->credits += 1;
    rtos_mutex_unlock(m);
    // Send a signal to sending process that we have a new credit
    rtos_semaphore_signal(stream->net_tg_semaphore, false);
}

/**
 ****************************************************************************************
 * @brief Initialize the pool of TG send buffers according to TG stream
 *
 * It will also initialize part of TG header message.
 *
 * @param[in] stream  Pointer to TG stream for which we initialize its pool
 ****************************************************************************************
 **/
static void net_tg_send_buf_init(struct fhost_tg_stream *stream)
{
    int i, j;

    i = stream->id;
    memset(net_tg_send_buf_mem[i], 0, sizeof(struct net_tg_send_buf_tag)
            * FHOST_TG_SEND_BUF_CNT);
    for (j = 0; j < FHOST_TG_SEND_BUF_CNT; j++)
    {
        // Initialize callback function for its pbuf custom
        net_tg_send_buf_mem[i][j].h.custom_free_function =
                (pbuf_free_custom_fn)net_tg_pbuf_custom_header_free_cb;
        net_tg_send_buf_mem[i][j].p.custom_free_function =
                (pbuf_free_custom_fn)net_tg_pbuf_custom_payload_free_cb;
        // Pointer to TG stream
        net_tg_send_buf_mem[i][j].info.stream = stream;
        // Initialize header message
        memcpy(&net_tg_send_buf_mem[i][j].hdr, "1345678", 8);
    }
}

/**
 ****************************************************************************************
 * @brief Find a free TG send buffer in the pool according to TG stream
 *
 * We check if its flag is 0 or not.
 *
 * @param[in] stream  Pointer to TG stream for which we search its pool
 *
 * @return pointer to the free TG send buffer, NULL if all occupied
 ****************************************************************************************
 **/
static struct net_tg_send_buf_tag *net_tg_find_free_send_buf(struct fhost_tg_stream *stream)
{
    int i, j;

    i = stream->id;
    for (j = 0; j < FHOST_TG_SEND_BUF_CNT; j++)
    {
        if((net_tg_send_buf_mem[i][j]).info.flag == 0)
        {
            (net_tg_send_buf_mem[i][j]).info.flag = 1;
            return &net_tg_send_buf_mem[i][j];
        }
    }
    return NULL;
}

/**
 ****************************************************************************************
 * @brief Calculate the throughput of traffic sending
 *
 * @param[in] init      Start transmission timestamp
 * @param[in] actual    Actual transmission timestamp
 * @param[in] tx_bytes  Number of TX buffer bytes sent
 *
 * @return the throughput of traffic sending (bps), -1 if incorrect duration
 ****************************************************************************************
 **/
static int net_cal_throughput(struct tg_time *init, struct tg_time *actual,
                              u32_t tx_bytes)
{
    int diff_time;
    int throughput;

    diff_time = net_time_diff(init, actual);
    if (diff_time < NET_TG_MIN_SEND_TIME || diff_time > NET_TG_MAX_SEND_TIME)
    {
        // Duration too short or too long for sending process
        return -1;
    }
    // For better precision compute the throughput per ms, then multiply by 1000
    throughput = 8 * 1000 * (tx_bytes / (diff_time / 1000));
    return throughput;
}

/**
 ****************************************************************************************
 * @brief Callback function for udp_recv() of @ref net_tg_recv_start
 *
 * This callback will be called when receiving a datagram from the pcb.
 *
 * @attention Be aware that 'addr' might point into the pbuf 'p' so freeing this pbuf
 *            can make 'addr' invalid, too.
 *
 * @param[in] arg   User supplied argument (udp_pcb.recv_arg)
 * @param[in] pcb   The udp_pcb which received data
 * @param[in] p     The packet buffer that was received
 * @param[in] addr  The remote IP address from which the packet was received
 * @param[in] port  The remote port from which the packet was received
 ****************************************************************************************
 **/
static void net_tg_recv_cb(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                           const ip_addr_t *addr, u16_t port)
{
    if (p != NULL)
    {
        u32_t c;
        char *c_p;

        struct fhost_tg_stream *stream = arg;

        (stream->stats.rx_frames)++;
        (stream->stats.rx_bytes) += p->tot_len;
        (stream->stats.counter_recv)++;

        // Get the counter in the received buffer (normally equal to counter_send)
        // Pointer pointed to counter_send in payload
        c_p = p->payload + 8;
        // Copy 4 bytes of data (counter_send) and make it an unsigned int
        memcpy(&c, c_p, 4);
        // Convert network format to host format
        c = co_ntohl(c);

        if (stream->stats.counter_recv == c)
        {
            (stream->stats.expected)++;
        }
        else if (stream->stats.counter_recv < c)
        {
            (stream->stats.lost) += c - stream->stats.counter_recv;
            stream->stats.counter_recv = c;
        }
        else
        {
            /// counter_recv > c
            (stream->stats.unordered)++;
            (stream->stats.counter_recv)--;
        }

        pbuf_free(p);
    }
}

/**
 ****************************************************************************************
 * @brief Callback function for udp_recv() of @ref net_tg_recv_transc_start
 *
 * This callback will be called when receiving a datagram from the pcb. It will send a
 * response to the sender every time called.
 *
 * @attention Be aware that 'addr' might point into the pbuf 'p' so freeing this pbuf
 *            can make 'addr' invalid, too.
 *
 * @param[in] arg   User supplied argument (udp_pcb.recv_arg)
 * @param[in] pcb   The udp_pcb which received data
 * @param[in] p     The packet buffer that was received
 * @param[in] addr  The remote IP address from which the packet was received
 * @param[in] port  The remote port from which the packet was received
 ****************************************************************************************
 **/
static void net_tg_recv_transc_cb(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                                  const ip_addr_t *addr, u16_t port)
{
    struct fhost_tg_stream *stream = arg;
    uint16_t tot_len;
    err_t err;
    struct pbuf *p_send, *h_send;
    char *hdr;
    u32_t sec_nl, usec_nl;
    struct tg_time actual;

    if (p == NULL)
        return;

    tot_len = p->tot_len;
    pbuf_free(p);
    get_time_SINCE_EPOCH(&actual.sec, &actual.usec);

    (stream->stats.rx_frames)++;
    (stream->stats.rx_bytes) += tot_len;

    // Allocation of response buffer
    h_send = pbuf_alloc(PBUF_TRANSPORT, NET_TG_HEADER_SIZE, PBUF_RAM);
    // If there's no more memory to allocate pbuf, we don't send the response to sender
    if (h_send == NULL)
    {
        return;
    }
    p_send = pbuf_alloc(PBUF_RAW, tot_len - NET_TG_HEADER_SIZE, PBUF_ROM);
    if (p_send == NULL)
    {
        pbuf_free(h_send);
        return;
    }

    pbuf_cat(h_send, p_send);
    p_send->payload = net_udp_send_nocopy_buf;

    hdr = h_send->payload;

    // For a transaction receiver, it just needs to send the local time back
    sec_nl = co_htonl(actual.sec);
    usec_nl = co_htonl(actual.usec);
    memcpy(&hdr[12], &sec_nl, 4);
    memcpy(&hdr[16], &usec_nl, 4);

    err = udp_send(pcb, h_send);

    if (err == ERR_OK)
    {
        (stream->stats.tx_frames)++;
        stream->stats.tx_bytes += tot_len;
    }
    else
    {
        TRACE_APP(ERR, "[TG] Failed to send response: %ld", err);
    }

    pbuf_free(h_send);

}

/**
 ****************************************************************************************
 * @brief Callback function for udp_recv() of @ref net_tg_send_transc_start
 *
 * This callback will be called when receiving a datagram from the pcb. It will calculate
 * the RTT and send a signal to TG send thread every time called.
 *
 * @attention Be aware that 'addr' might point into the pbuf 'p' so freeing this pbuf
 *            can make 'addr' invalid, too.
 *
 * @param[in] arg   User supplied argument (udp_pcb.recv_arg)
 * @param[in] pcb   The udp_pcb which received data
 * @param[in] p     The packet buffer that was received
 * @param[in] addr  The remote IP address from which the packet was received
 * @param[in] port  The remote port from which the packet was received
 ****************************************************************************************
 **/
static void net_tg_send_transc_cb(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                                  const ip_addr_t *addr, u16_t port)
{
    struct fhost_tg_stream *stream = arg;
    int diff_time;
    struct tg_time recv;

    if (p == NULL)
        return;

    get_time_SINCE_EPOCH(&recv.sec, &recv.usec);

    // Calculate RTT as diff_time
    diff_time = net_time_diff(&(stream->transc_timestamp), &recv);
    // Sanity check - diff_time should never be negative,
    // because we send before we receive the response
    assert(diff_time > 0);
    // Calculate the average to obtain the approximation of RTT
    stream->stats.rt_time = (stream->stats.rt_time + diff_time) / 2;

    (stream->stats.rx_frames)++;
    (stream->stats.rx_bytes) += p->tot_len;

    // Send a signal to net_tg_send_transc_start() to indicate we receive the response
    rtos_semaphore_signal(stream->net_tg_semaphore, false);

    pbuf_free(p);
}

/**
 ****************************************************************************************
 * @brief Get Network interface linked to a TG stream
 *
 * To be updated for now always return interface for 'wl1'
 *
 * @param[in] stream  Pointer to TG Stream
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
static net_al_if_t net_tg_get_netif(struct fhost_tg_stream *stream)
{
    return net_if_find_from_name("wl1");
}

/**
 ****************************************************************************************
 * @brief join or leave a multicast group
 *
 * If LWIP is not compiled with IGMP support this function does nothing.
 * Otherwise the interface will join (or leave) the multicast group (unless this is the
 * 'all systems' group 224.0.0.1).
 * The multicast address is the stream local address.
 *
 * @param[in] stream  Pointer to TG Stream
 * @param[in] join    Set to true to join the group and false to leave it.
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
static int net_tg_mcast_membership(struct fhost_tg_stream *stream, bool join)
{
    #if LWIP_IGMP
    ip_addr_t grp_ip;
    ip_addr_t allsystems;

    IP4_ADDR(&allsystems, 224, 0, 0, 1);
    IP4_ADDR(&grp_ip,  stream->prof.local_ip & 0xff, (stream->prof.local_ip >> 8) & 0xff,
             (stream->prof.local_ip >> 16) & 0xff, (stream->prof.local_ip >> 24) & 0xff);

    // LwIP already include the interface in the 'all system' group
    if (!ip_addr_cmp(&allsystems, &grp_ip))
    {
        err_t err;

        if (join)
            err = igmp_joingroup_netif(net_tg_get_netif(stream), ip_2_ip4(&grp_ip));
        else
            err = igmp_leavegroup_netif(net_tg_get_netif(stream), ip_2_ip4(&grp_ip));

        if (err != ERR_OK)
        {
            TRACE_APP(ERR, "[TG] Failed to update mcast group %x membership (join=%d)",
                      grp_ip, join);
            return 1;
        }
    }
    #endif  // LWIP_IGMP

    return 0;
}


/**
 ****************************************************************************************
 * @brief Configuration of PCB for TG sending process
 *
 * @param[in] pcb     Pointer to udp_pcb to which we send data
 * @param[in] stream  Pointer to TG stream which contains configuration information
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
static int net_tg_pcb_config(struct udp_pcb *pcb, struct fhost_tg_stream *stream)
{
    u32_t ip;
    ip4_addr_t rip;
    ip4_addr_t lip;

    ip = (stream->prof).local_ip;
    IP4_ADDR(&lip,  ip & 0xff, (ip >> 8) & 0xff,
            (ip >> 16) & 0xff, (ip >> 24) & 0xff);

    if (udp_bind(pcb, &lip, stream->prof.local_port) != ERR_OK)
    {
        TRACE_APP(ERR, "[TG] Failed to bind UDP socket to %x:%u",
                  lip, stream->prof.local_port);
        return 1;
    }

    ip = (stream->prof).remote_ip;
    IP4_ADDR(&rip,  ip & 0xff, (ip >> 8) & 0xff,
            (ip >> 16) & 0xff, (ip >> 24) & 0xff);

    if(stream->prof.prof_id == FHOST_PROF_MCAST)
    {
        if (stream->prof.direction == FHOST_TG_DIRECT_SEND)
            ip4_set_default_multicast_netif(net_tg_get_netif(stream));
        else if (net_tg_mcast_membership(stream, true))
            return 1;
    }

    if (udp_connect(pcb, &rip, stream->prof.remote_port) != ERR_OK)
    {
        TRACE_APP(ERR, "[TG] Failed to connect UDP socket to %x:%u",
                  rip, stream->prof.remote_port);
        return 1;
    }

    pcb->tos = stream->prof.tos;
    return 0;
}

/**
 ****************************************************************************************
 * @brief Initialize TG process
 *
 * @param[in] stream  Pointer to TG stream which contains configuration information
 * @param[in] recv    Callback function pointer used for @ref udp_recv
 *
 * @return pointer to udp_pcb configured on success and NULL if error occurred.
 ****************************************************************************************
 **/
static struct udp_pcb *net_tg_init(struct fhost_tg_stream *stream, udp_recv_fn recv)
{
    struct udp_pcb *pcb = NULL;

    LOCK_TCPIP_CORE();
    pcb = udp_new();
    if (pcb == NULL)
    {
        TRACE_APP(ERR, "[TG] Failed to allocate UDP control structure");
        UNLOCK_TCPIP_CORE();
        return NULL;
    }

    if (net_tg_pcb_config(pcb, stream))
    {
        UNLOCK_TCPIP_CORE();
        return NULL;
    }

    stream->arg = pcb;
    if (recv != NULL)
    {
        udp_recv(pcb, recv, stream);
    }
    UNLOCK_TCPIP_CORE();

    return pcb;
}

int net_tg_send_transc(struct fhost_tg_stream *stream)
{
    struct udp_pcb * pcb = NULL;
    net_al_tx_t h = NULL;
    net_al_tx_t p = NULL;
    err_t err;
    struct tg_time send, init;
    u32_t pack_len = stream->prof.pksize;
    int duration;
    // current duration of TG sending process
    int duration_cur = 0;

    if (pack_len > NET_MAX_UDP_LEN)
        pack_len = NET_MAX_UDP_LEN;

    if (stream->prof.duration == 0)
        duration = 30 * 1000000;
    else
        duration = stream->prof.duration * 1000000;

    if (duration > NET_TG_MAX_SEND_TIME)
    {
        duration = NET_TG_MAX_SEND_TIME;
    }

    pcb = net_tg_init(stream, net_tg_send_transc_cb);
    if (pcb == NULL)
        return 1;

    // estimated value : 2000 us for RTT to initialize
    stream->stats.rt_time = 2000;

    get_time_SINCE_EPOCH(&init.sec, &init.usec);
    while (stream->active && duration_cur < duration)
    {
        h = pbuf_alloc(PBUF_TRANSPORT, NET_TG_HEADER_SIZE, PBUF_RAM);
        if (h == NULL)
        {
            TRACE_APP(ERR, "[TG] Failed to allocate header buffer");
            rtos_task_suspend(1);
            continue;
        }
        p = pbuf_alloc(PBUF_RAW, pack_len - NET_TG_HEADER_SIZE, PBUF_ROM);
        if (p == NULL)
        {
            pbuf_free(h);
            TRACE_APP(ERR, "[TG] Failed to allocate data buffer");
            rtos_task_suspend(1);
            continue;
        }
        // Sanity check - If buf_tag is not NULL, then h and p should never be NULL
        assert((h != NULL) && (p != NULL));

        pbuf_cat(h, p);
        ((struct pbuf *)p)->payload = net_udp_send_nocopy_buf;

        get_time_SINCE_EPOCH(&send.sec, &send.usec);
        // Store the timestamp of sending
        stream->transc_timestamp = send;

        LOCK_TCPIP_CORE();
        err = udp_send(pcb, h);
        UNLOCK_TCPIP_CORE();

        pbuf_free(h);

        // Calculate the current duration of TG transaction sending process
        duration_cur = net_time_diff(&init, &send);

        if (err == ERR_OK)
        {
            (stream->stats.tx_frames)++;
            stream->stats.tx_bytes += pack_len;
        }
        else
        {
            TRACE_APP(ERR, "[TG] Failed to send data packet");
            return 1;
        }

        if (rtos_semaphore_wait(stream->net_tg_semaphore, 20))
        {   // 20 ms timeout expired without the semaphore becoming available
            // TG send thread has sent a packet without receiving the response
            (stream->stats.lost)++;
        }
        // else the response was received
    }

    LOCK_TCPIP_CORE();
    udp_remove(pcb);
    UNLOCK_TCPIP_CORE();

    return 0;
}

int net_tg_send(struct fhost_tg_stream *stream)
{
    struct udp_pcb * pcb = NULL;
    net_al_tx_t h = NULL;
    net_al_tx_t p = NULL;
    char *hdr;
    struct net_tg_send_buf_tag *buf_tag = NULL;
    int status = 0;
    err_t err;
    struct tg_time before, after, actual, init;
    int diff_time;
    u32_t sleep_time = 0;
    u32_t throttled_rate = 0;
    // estimated value : 500 us
    int adj_latency = 500;
    u32_t sec_nl, usec_nl, counter_nl;
    u32_t pack_len = stream->prof.pksize;
    int duration;
    // current duration of TG sending process
    int duration_cur = 0;

    if (pack_len > NET_MAX_UDP_LEN)
        pack_len = NET_MAX_UDP_LEN;

    if (stream->prof.duration == 0)
        duration = 30 * 1000000;
    else
        duration = stream->prof.duration * 1000000;

    if (duration > NET_TG_MAX_SEND_TIME)
    {
        duration = NET_TG_MAX_SEND_TIME;
    }

    pcb = net_tg_init(stream, NULL);
    if (pcb == NULL)
        return 1;

    stream->stats.counter_send = 0;
    // Initialize TG send buffer pool
    net_tg_send_buf_init(stream);

    net_sleep_time(stream->prof.prof_id, stream->prof.rate,
            &sleep_time, &throttled_rate);

    get_time_SINCE_EPOCH(&init.sec, &init.usec);

    while (stream->active && duration_cur < duration)
    {
        // At least 1 credit
        if (stream->credits > 0)
        {
            // Avoid callback function of pbuf_custom to modify it in the same time
            rtos_mutex_lock(stream->tg_mutex);
            // Retrieve a free buffer from buffer pool
            buf_tag = net_tg_find_free_send_buf(stream);

            if (buf_tag == NULL)
            {
                TRACE_APP(ERR, "[TG] no more TX buffer");
                rtos_mutex_unlock(stream->tg_mutex);
                return 1;
            }
            // Set the pbuf custom flag on
            h = pbuf_alloced_custom(PBUF_TRANSPORT, NET_TG_HEADER_SIZE, PBUF_RAM,
                    &(buf_tag->h), buf_tag->hdr, NET_TG_HEADER_TOTAL_SIZE);

            p = pbuf_alloced_custom(PBUF_RAW, pack_len - NET_TG_HEADER_SIZE, PBUF_ROM,
                    &(buf_tag->p), net_udp_send_nocopy_buf, pack_len - NET_TG_HEADER_SIZE);
            // Sanity check - If buf_tag is not NULL, then h and p should never be NULL
            assert((h != NULL) && (p != NULL));

            stream->credits--;

            rtos_mutex_unlock(stream->tg_mutex);
        } // We have to wait at least one free token if all are used
        else
        {
            rtos_semaphore_wait(stream->net_tg_semaphore, -1);
            continue;
        }

        pbuf_cat(h, p);
        hdr = ((struct pbuf *)h)->payload;

        (stream->stats.counter_send)++;

        if (stream->stats.counter_send == 1)
        {
            get_time_SINCE_EPOCH(&before.sec, &before.usec);

            before.usec += sleep_time;
            if (before.usec > 1000000)
            {
                before.usec -= 1000000;
                before.sec +=1;
            }
        }

        // Fill in the payload of header by counter and time
        // Convert unsigned int of host format to network format
        counter_nl = co_htonl(stream->stats.counter_send);
        // Store unsigned int in the table of char
        memcpy(&hdr[8], &counter_nl, 4);
        get_time_SINCE_EPOCH(&actual.sec, &actual.usec);
        sec_nl = co_htonl(actual.sec);
        usec_nl = co_htonl(actual.usec);
        memcpy(&hdr[12], &sec_nl, 4);
        memcpy(&hdr[16], &usec_nl, 4);

        LOCK_TCPIP_CORE();
        err = udp_send(pcb, h);
        UNLOCK_TCPIP_CORE();
        pbuf_free(h);

        // Calculate the current duration of TG sending process
        duration_cur = net_time_diff(&init, &actual);

        if (err == ERR_OK)
        {
            (stream->stats.tx_frames)++;
            stream->stats.tx_bytes += pack_len;
            status = 0;
        }
        else
        {
            TRACE_APP(ERR, "[TG] Failed to send data: %ld", err);
            status = 1;
            break;
        }

        if (throttled_rate != 0)
        {
            if (stream->stats.counter_send % throttled_rate == 0)
            {
                get_time_SINCE_EPOCH(&after.sec, &after.usec);
                diff_time = net_time_diff(&after, &before);
                // It means that the rate is too high (> 2000 pkt/s)
                if (diff_time > adj_latency)
                {
                    // too much time left, go sleep
                    rtos_task_suspend((diff_time - adj_latency) / 1000);
                }

                before.usec += sleep_time;
                if (before.usec > 1000000)
                {
                    before.usec -= 1000000;
                    before.sec +=1;
                }
            }
        } // otherwise, it floods
    }
    // Wait until all of pbuf are freed
    while (stream->credits < FHOST_TG_SEND_BUF_CNT)
    {
        rtos_semaphore_wait(stream->net_tg_semaphore, -1);
    }

    stream->stats.throughput = net_cal_throughput(&init, &actual,
            (stream->stats.tx_bytes));

    LOCK_TCPIP_CORE();
    udp_remove(pcb);
    UNLOCK_TCPIP_CORE();

    return status;
}

int net_tg_recv_start(struct fhost_tg_stream *stream)
{
    if (net_tg_init(stream, net_tg_recv_cb) == NULL)
        return 1;
    else
        return 0;
}

int net_tg_recv_transc_start(struct fhost_tg_stream *stream)
{
    if (net_tg_init(stream, net_tg_recv_transc_cb) == NULL)
        return 1;
    else
        return 0;
}

void net_tg_recv_stop(struct fhost_tg_stream *stream)
{
    LOCK_TCPIP_CORE();
    udp_remove(stream->arg);
    if (stream->prof.prof_id == FHOST_PROF_MCAST)
        net_tg_mcast_membership(stream, false);
    UNLOCK_TCPIP_CORE();
}
