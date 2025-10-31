 /**
 ****************************************************************************************
 *
 * @file fhost_ping.c
 *
 * @brief Definition of ping task for Fully Hosted firmware.
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup FHOST_PING
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "fhost.h"
#include "net_tg_al.h"
#include "net_ping_al_priv.h"
#include "lwip/sockets.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/igmp.h"
#include "lwip/inet_chksum.h"

/// Maximum length of PING payload
#define NET_MAX_PING_LEN 1500 - IP_HLEN
/// Ping identifier - must fit on a u16_t
#define NET_TG_PING_ID             0xAFAF
/// Ping ICMP ECHO header length : 8 bytes
#define NET_TG_PING_HDR_LEN        sizeof(struct icmp_echo_hdr)
/// Ping timestamp length : 8 bytes
#define NET_TG_PING_TIMESTAMP_LEN       8
/// Maximum ping time for sending process (340 s)
#define NET_TG_MAX_PING_TIME       340000000

#define co_ntohs ntohs
#define co_htons htons
#define co_ntohl ntohl
#define co_htonl htonl

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Table of ping streams
struct fhost_ping_stream p_streams[FHOST_PING_MAX_STREAMS];


/**
 ****************************************************************************************
 * @brief Configuration of PCB for ping process
 *
 * @param[in] pcb     Pointer to raw_pcb to which we send data
 * @param[in] stream  Pointer to ping stream which contains configuration information
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
static int net_ping_pcb_config(struct raw_pcb *pcb, struct fhost_ping_stream *stream)
{
    u32_t ip;
    ip_addr_t rip;

    if (raw_bind(pcb, IP_ADDR_ANY) != ERR_OK)
    {
        TRACE_APP(ERR, "[TG] Failed to bind RAW to 0.0.0.0");
        return 1;
    }

    ip = (stream->prof).dst_ip;
    IP4_ADDR(ip_2_ip4(&rip),  ip & 0xff, (ip >> 8) & 0xff,
            (ip >> 16) & 0xff, (ip >> 24) & 0xff);

    if (raw_connect(pcb, &rip) != ERR_OK)
    {
        TRACE_APP(ERR, "[TG] Failed to connect RAW socket to %x", rip);
        return 1;
    }

    pcb->tos = stream->prof.tos;
    return 0;
}

/**
 ****************************************************************************************
 * @brief Initialize ping process
 *
 * @param[in] stream     Pointer to ping stream which contains configuration information
 * @param[in] ping_recv  Callback function pointer used for @ref raw_recv
 *
 * @return pointer to raw_pcb configured on success and NULL if error occurred.
 ****************************************************************************************
 **/
static struct raw_pcb *net_ping_init(struct fhost_ping_stream *stream,
                                     raw_recv_fn ping_recv)
{
    struct raw_pcb *pcb;

    LOCK_TCPIP_CORE();
    pcb = raw_new(IP_PROTO_ICMP);
    if (pcb == NULL)
    {
        TRACE_APP(ERR, "[TG] Failed to allocate RAW control structure");
        UNLOCK_TCPIP_CORE();
        return NULL;
    }

    if (net_ping_pcb_config(pcb, stream))
    {
        UNLOCK_TCPIP_CORE();
        return NULL;
    }

    stream->arg = pcb;

    raw_recv(pcb, ping_recv, stream);
    UNLOCK_TCPIP_CORE();

    return pcb;
}

/**
 ****************************************************************************************
 * @brief Callback function for raw_recv() of @ref net_ping_send
 *
 * This callback will be called when receiving a datagram from the pcb.
 *
 * @param[in] arg   User supplied argument (raw_pcb.recv_arg)
 * @param[in] pcb   The raw_pcb which received data
 * @param[in] p     The packet buffer that was received
 * @param[in] addr  The remote IP address from which the packet was received
 *
 * @return 1 if the packet was 'eaten' (aka. deleted),
 *         0 if the packet lives on
 * If returning 1, the callback is responsible for freeing the pbuf
 * if it's not used any more.
 ****************************************************************************************
 **/
static u8_t net_ping_recv_cb(void *arg, struct raw_pcb *pcb,
                             struct pbuf *p, ip_addr_t *addr)
{
    struct fhost_ping_stream *stream = arg;
    struct icmp_echo_hdr *iecho;
    size_t hdr_len = NET_TG_PING_HDR_LEN;
    char *sec_p, *usec_p;
    int diff_time;
    struct tg_time send;

    // Sanity check - If callback function called, then p should never be NULL
    assert(p != NULL);

    // Adjusts the ->payload pointer so that IP header disappears in the pbuf payload.
    if (pbuf_header(p, -PBUF_IP_HLEN) == 0)
    {
        int reply_num;

        iecho = (struct icmp_echo_hdr *)p->payload;
        reply_num = co_ntohs(iecho->seqno);

        if ((iecho->id == NET_TG_PING_ID + stream->id) &&
            (reply_num <= stream->ping_seq_num) &&
            ip_addr_cmp(addr, &pcb->remote_ip))
        {
            get_time_SINCE_EPOCH(&stream->stats.last_msg.sec,
                     &stream->stats.last_msg.usec);
            // Get the timestamp in the received buffer
            sec_p = (p->payload + hdr_len);
            usec_p = (p->payload + hdr_len + 4);
            memcpy(&send.sec, sec_p, 4);
            memcpy(&send.usec, usec_p, 4);
            // Convert network format to host format
            send.sec = co_ntohl(send.sec);
            send.usec = co_ntohl(send.usec);
            // Calculate RTT as diff_time
            diff_time = net_time_diff(&send, &stream->stats.last_msg);
            // Sanity check - diff_time should never be negative,
            // because we send before we receive the response
            assert(diff_time > 0);
            // Print traces at IPC layer
            printf("%d bytes from: %s: icmp_req=%d ttl=%d time=%d us\n",
                  p->tot_len, ipaddr_ntoa(&pcb->remote_ip),
                  reply_num, pcb->ttl, diff_time);

            // Calculate the cumulative value of RTT
            stream->stats.rt_time += diff_time;
            stream->stats.rx_frames++;
            stream->stats.rx_bytes += p->tot_len;
            stream->ping_reply_num = reply_num;

            pbuf_free(p);
            return 1; // eat the packet
        }
        else
        {   // Restore ->payload pointer position if this buffer is not
            // the corresponding response
            pbuf_header_force(p, PBUF_IP_HLEN);
        }
    }
    return 0; // don't eat the packet
}

/**
 ****************************************************************************************
 * @brief Configure ICMP ECHO packet to be sent to the remote IP address, used for
 * @ref net_ping_send
 *
 * Here is the structure of ICMP ECHO packet :
 * @verbatim
             | 8 bytes |    8 bytes     |            data_len           |
             ************************************************************
             |   hdr   |   timestamp    |             DATA              |
             ************************************************************
  @endverbatim
 * @param[in] iecho    Pointer to ICMP ECHO header
 * @param[in] len      Total length of packet to be sent
 * @param[in] stream   Pointer to ping stream which contains configuration information
 ****************************************************************************************
 **/
static void net_ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len,
                                  struct fhost_ping_stream *stream)
{
    u32_t sec_nl, usec_nl;
    size_t i;
    size_t timestamp_len = NET_TG_PING_TIMESTAMP_LEN;
    size_t hdr_len = NET_TG_PING_HDR_LEN;
    // 8 bytes for size of timestamp
    size_t data_len = len - hdr_len - timestamp_len;
    char *timestamp = (char *)iecho + hdr_len;

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = NET_TG_PING_ID + stream->id;
    iecho->seqno  = co_htons(++(stream->ping_seq_num));

    get_time_SINCE_EPOCH(&stream->stats.last_msg.sec, &stream->stats.last_msg.usec);
    // Fill in the payload with timestamp
    // Convert unsigned int of host format to network format
    sec_nl = co_htonl(stream->stats.last_msg.sec);
    usec_nl = co_htonl(stream->stats.last_msg.usec);
    memcpy(&timestamp[0], &sec_nl, 4);
    memcpy(&timestamp[4], &usec_nl, 4);

    if (stream->stats.tx_frames == 0)
        stream->stats.first_msg = stream->stats.last_msg;

    /* fill the additional data buffer with some data */
    for(i = 0; i < data_len; i++) {
        ((char *)iecho)[hdr_len + timestamp_len + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

int net_ping_send(struct fhost_ping_stream *stream)
{
    struct raw_pcb * pcb = NULL;
    inet_buf_tx_t *p = NULL;
    struct icmp_echo_hdr *iecho;
    // Duration unit : microseconds
    int duration;
    err_t err;
    u32_t sleep_time = 0;
    size_t pack_len;
    int nb_skipped = 0;

    // Set configuration information to default value if it's 0
    if (stream->prof.pksize == 0 ||
            stream->prof.pksize < NET_TG_PING_HDR_LEN + NET_TG_PING_TIMESTAMP_LEN)
        pack_len = 100;
    else
        pack_len = stream->prof.pksize;

    if (stream->prof.duration == 0)
        duration = 30 * 1000000;
    else
        duration = stream->prof.duration * 1000000;

    if (stream->prof.rate == 0)
        stream->prof.rate = 1;

    if (pack_len > NET_MAX_PING_LEN)
    {
        pack_len = NET_MAX_PING_LEN;
    }
    if (duration > NET_TG_MAX_PING_TIME)
    {
        duration = NET_TG_MAX_PING_TIME;
    }

    pcb = net_ping_init(stream, (raw_recv_fn)net_ping_recv_cb);
    if (pcb == NULL)
        return -1;

    // Initialize the ping sequence number and RTT
    stream->ping_seq_num = 0;
    stream->ping_reply_num = 0;
    stream->stats.rt_time = 0;

	// net_sleep_time(FHOST_PROF_PING, stream->prof.rate, &sleep_time, NULL);
	if (stream->prof.rate > 50)
	{
		// We set the maximum rate at 50 pkts / s
		sleep_time = 20000;
	}
	else if (stream->prof.rate > 0 && stream->prof.rate <= 50)
	{
		sleep_time = 1000000 / stream->prof.rate;
	}
	else if (stream->prof.rate == 0)
	{
		sleep_time = 1000000;
	}

    while (stream->active && duration > 0)
    {
        p = pbuf_alloc(PBUF_IP, (u16_t)pack_len, PBUF_RAM);
        if (p == NULL)
        {
            nb_skipped++;
        }
        else
        {
            iecho = (struct icmp_echo_hdr *)p->payload;
            // Configure ICMP ECHO packet before sending it
            net_ping_prepare_echo(iecho, (u16_t)pack_len, stream);

            LOCK_TCPIP_CORE();
            err = raw_send(pcb, p);
            UNLOCK_TCPIP_CORE();

            pbuf_free(p);

            if (err == ERR_OK)
            {
                (stream->stats.tx_frames)++;
                stream->stats.tx_bytes += pack_len;
            }
        }

        // Update the remaining time
        duration -= sleep_time;
        rtos_task_suspend(sleep_time / 1000);
    }

    if (stream->stats.rt_time)
    {
        uint16_t missing_reply = stream->ping_seq_num - stream->ping_reply_num;
        if (missing_reply)
        {
            // let's wait for missing reply, using average round trip delay.
            int cur_rtt = stream->stats.rt_time / 1000;
            cur_rtt = cur_rtt / stream->stats.rx_frames;
            rtos_task_suspend(missing_reply * cur_rtt);
        }
        stream->stats.rt_time /= stream->stats.rx_frames;
    }

    stream->stats.time = net_time_diff(&stream->stats.first_msg,
                                       &stream->stats.last_msg);

    LOCK_TCPIP_CORE();
    raw_remove(pcb);
    UNLOCK_TCPIP_CORE();

    return nb_skipped;
}

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/// main function of ping send task
static RTOS_TASK_FCT(ping_send_main)
{
    struct fhost_ping_stream *ping_stream = env;
    ip4_addr_t rip;
    u32_t ip = ping_stream->prof.dst_ip;
    int packet_loss = 0;
    int res;

    // Wait for semaphore from fhost_ping_start to ensure the flag active is at 1
    rtos_semaphore_wait(ping_stream->ping_semaphore, -1);

    // run the ping command
    res = net_ping_send(ping_stream);

    // Wait for semaphore to lock the statistics print for background execution
    // Background task: semaphore signalled in fhost_ping_stop
    // Foreground task: semaphore signalled in fhost_ping_start
    rtos_semaphore_wait(ping_stream->ping_semaphore, -1);
    ping_stream->active = false;

    if (res >= 0)
    {
        struct fhost_ping_stats* stats;
        int ms, us;

        stats = &ping_stream->stats;
        IP4_ADDR(&rip,  ip & 0xff, (ip >> 8) & 0xff,
                 (ip >> 16) & 0xff, (ip >> 24) & 0xff);
        fhost_printf("--- %s ping statistics ---\n",
                     ipaddr_ntoa(&rip));
        if (stats->tx_frames)
        {
            packet_loss = ((stats->tx_frames - stats->rx_frames) * 100) / stats->tx_frames;
        }

        ms = stats->rt_time / 1000;
        us = stats->rt_time - (ms * 1000);
        fhost_printf("%d packets transmitted, %d received, "
                     "%d%% packet loss, time %d ms\n"
                     "%d bytes transmitted, %d bytes received, rtt avg %d.%03d ms\n",
                     stats->tx_frames, stats->rx_frames,
                     packet_loss, stats->time / 1000, stats->tx_bytes, stats->rx_bytes,
                     ms, us);
        if (res > 0)
        {
            fhost_printf("%d packets skipped\n", res);
        }
    }
    else
    {
        fhost_printf("ping internal error\n");
        TRACE_APP(ERR, "PING: PING_SEND_MAIN ERR net ping send");
    }

    rtos_semaphore_delete(ping_stream->ping_semaphore);
    //fhost_ipc_send_cmd_cfm(rtos_get_task_handle(), cmd_ret);
    rtos_task_delete(NULL);
}

/**
 ****************************************************************************************
 * @brief Search the stream id of a free ping stream
 * It will return the first free one in the table.
 *
 * @return ping stream id, -1 if error
 ****************************************************************************************
 **/
static int fhost_ping_find_free_stream_id(void)
{
    int stream_id;
    for (stream_id = 0; stream_id < FHOST_PING_MAX_STREAMS; stream_id++)
    {
        if (p_streams[stream_id].active == false)
        {
            return stream_id;
        }
    }
    return -1;
}

struct fhost_ping_stream *fhost_ping_find_stream_profile(uint32_t stream_id)
{
    if (stream_id < FHOST_PING_MAX_STREAMS)
    {
        return &p_streams[stream_id];
    }
    else
    {
        return NULL;
    }
}

rtos_task_handle fhost_ping_start(void *args)
{
    struct fhost_ping_task_args *ping_args = (struct fhost_ping_task_args *) args;
    struct fhost_ping_stream *ping_stream = NULL;
    int stream_id = fhost_ping_find_free_stream_id();
    char fmt[] = "Send ping ID %d \n";

    if (stream_id == -1)
    {
        fhost_printf("Couldn't find free stream");
        return RTOS_TASK_NULL;
    }

    ping_stream = fhost_ping_find_stream_profile(stream_id);

    memset(ping_stream, 0, sizeof(struct fhost_ping_stream));

    ping_stream->id = stream_id;
    ping_stream->prof.dst_ip = ping_args->rip;
    ping_stream->prof.rate = ping_args->rate;
    ping_stream->prof.pksize = ping_args->pksize;
    ping_stream->prof.duration = ping_args->duration;
    ping_stream->prof.tos = ping_args->tos;
    ping_stream->background = ping_args->background;

    if (rtos_semaphore_create(&ping_stream->ping_semaphore, 2, 0))
        return RTOS_TASK_NULL;

    if (rtos_task_create(ping_send_main, "PING_SEND", PING_SEND_TASK,
                         FHOST_PING_SEND_STACK_SIZE, ping_stream, fhost_ping_priority,
                         &ping_stream->ping_handle))
    {
        rtos_semaphore_delete(ping_stream->ping_semaphore);
        return RTOS_TASK_NULL;
    }
    ping_stream->active = true;
    // Signal the semaphore to ping_send_main to ensure the flag active is at 1
    rtos_semaphore_signal(ping_stream->ping_semaphore, false);
    if (!ping_stream->background)
    {
        // Signal the ping semaphore to print ping statistics
        rtos_semaphore_signal(ping_stream->ping_semaphore, false);
    }

    fhost_printf(fmt, ping_stream->id);
    return ping_stream->ping_handle;
}

void fhost_ping_stop(struct fhost_ping_stream *ping_stream)
{
    // Cannot stop a stream which is inactive
    if (!ping_stream->active)
    {
        fhost_printf("Cannot stop the stream %d because it is inactive", ping_stream->id);
       return;
    }

    fhost_printf("Stop ping ID : %d\n", ping_stream->id);

    if (ping_stream->background)
    {
        // Signal the ping semaphore to print ping statistics
        rtos_semaphore_signal(ping_stream->ping_semaphore, false);
    }
    ping_stream->active = false;
}

int fhost_ping_sigkill_handler(rtos_task_handle ping_handle)
{
    int stream_id, ret;
    struct fhost_ping_stream *ping_stream = NULL;

    // Search ping stream
    for (stream_id = 0; stream_id < FHOST_PING_MAX_STREAMS; stream_id++)
    {
        if (p_streams[stream_id].ping_handle == ping_handle)
        {
            ping_stream = &p_streams[stream_id];
            break;
        }
    }

    if (stream_id == FHOST_PING_MAX_STREAMS)
    {
        fhost_printf("Stream id %d not valid\n", stream_id);
        return 1;//FHOST_IPC_ERROR;
    }

    if (ping_stream->background)
        ret = 2;//FHOST_IPC_NO_RESP;
    else
        ret = 0;//FHOST_IPC_SUCCESS;

    fhost_ping_stop(ping_stream);

    return ret;
}

/// @}
