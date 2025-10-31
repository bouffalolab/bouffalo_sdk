#include <lwip/sockets.h>
#include <lwip/inet.h>
#include "list.h"
#include "log.h"
#include "rtsp_parser.h"
#include "util.h"
#include "rtsp_srv.h"
#include "rtsp_sess.h"
#include "delay_task.h"
#include "rtsp_method.h"
#include "sd_handler.h"
#include "rtcp.h"


/**
 * Handle listening socket.
 * @arg: Should be the struct rtsp_srv.
 */
int handle_lsn_sd(struct sd_handler *hp, unsigned int event)
{
    struct rtsp_srv *srvp = (struct rtsp_srv *)hp->arg;
    struct sockaddr_in rtsp_sa;     /* Address of RTSP socket. */
    socklen_t rtsp_salen;           /* Address length of RTSP listen socket. */
    int rtsp_sd = -1;               /* RTSP socket, accepted from client. */
    socklen_t nodelay = 1;

    memset(&rtsp_sa, 0, sizeof(rtsp_sa));
    rtsp_salen = sizeof(rtsp_sa);
    rtsp_sd = accept(srvp->rtsp_lsn_sd, (struct sockaddr *)&rtsp_sa, &rtsp_salen);
    if (rtsp_sd < 0) {
        printf(ERR "Accept RTSP connection error");
        return -1;
    }
    printf("Accept one connection from [%s:%d] ^_^\r\n",
           inet_ntoa(rtsp_sa.sin_addr), ntohs(rtsp_sa.sin_port));

    /* Set rtsp_sd non-blocking. */
    if (set_block_mode(rtsp_sd, 0) < 0) {
        close(rtsp_sd);
        return -1;
    }

    if (setsockopt(rtsp_sd, IPPROTO_TCP, TCP_NODELAY,
                   &nodelay, sizeof(nodelay)) < 0) {
        printf("Set RTSP socket option [TCP_NODELAY] error");
        return -1;
    }

    monitor_sd_event(rtsp_sd, RTSP_SD_DFL_EV);

    create_rtsp_sess(rtsp_sd);
    return 0;
}

/**
 * Consider about the interleaved mode, we have to
 * filter out the RTCP packet.
 */
static int filter_data_from_rtsp_sd(struct rtsp_sess *sessp)
{
    ssize_t nr = 0;
    int offset = 0;
try:
    /* '$' must be the byte received last time,
     * we should continue to receive completed RTCP packet. */
    offset = (sessp->recv_buf[0] == '$') ? 1 : 0;
    nr = recv(sessp->rtsp_sd,
              sessp->recv_buf + offset, RECV_BUF_SZ - offset, 0);

    if (nr <= 0) {
    // printf("Receive data from rtsp_sd error, errno = %d \r\n", errno);
        if (sessp->recv_buf[0] == 0) {
            vTaskDelay(10);
            goto try;
        } 
        return -1;
    } else if (nr == RECV_BUF_SZ - offset) {
        printf(WARNING "Receive buffer seems not enough!\r\n");
        return 0;
    }
    
    if ((sessp->recv_buf[0] == '$') && (nr == 1)) {
        /* Continue to receive completed RTCP packet. */
        return 0;
    } else if (sessp->recv_buf[0] == '$') { /* Completed RTCP packet. */
        handle_rtcp_pkt(sessp);
        sessp->recv_buf[0] = '\0';
    } else {                    /* RTSP request message. */
        /* Handle the RTSP method. */
        create_delay_task((delay_task_proc_t)handle_rtsp_req, sessp, 0);
    }

    return 0;
}

/**
 * Handle socket of RTSP.
 * @arg: Should be the struct rtsp_sess.
 *
 * NOTE:
 * This function will also handle RTSP/RTCP over TCP
 * when it's in the RTSP interleaved mode.
 */
int handle_rtsp_sd(struct sd_handler *hp, unsigned int event)
{
    struct rtsp_sess *sessp = (struct rtsp_sess *)hp->arg;
    enum data_type type = 0;

    if (event & POLLIN) {      /* Readable. */
        /* Receive data from RTSP client. */
        if (filter_data_from_rtsp_sd(sessp) < 0) {
            destroy_rtsp_sess(sessp);
            return 0;
        }
    }
    if (event & POLLOUT) { /* Writable. */
        type = sessp->intlvd_mode   ?
            (DATA_TYPE_RTSP_RESP    |
             DATA_TYPE_RTP_V_PKT    |
             DATA_TYPE_RTP_A_PKT    |
             DATA_TYPE_RTCP_V_PKT   |
             DATA_TYPE_RTCP_A_PKT)  :
            DATA_TYPE_RTSP_RESP;
        if (consume_send_buf(sessp, type) < 0) {
            destroy_rtsp_sess(sessp);
        }
    }
    return 0;
}

/**
 * Handle socket of RTP.
 * @arg: Should be the struct rtsp_sess.
 *
 * NOTE:
 * Strictly speaking, handling non-interleaved mode only.
 */
int handle_rtp_sd(struct sd_handler *hp, unsigned int event)
{
    struct rtsp_sess *sessp = (struct rtsp_sess *)hp->arg;
    enum data_type type = 0;
    if (hp->sd == sessp->rtp_rtcp[0].udp.rtp_sd) {
        type = DATA_TYPE_RTP_V_PKT;
    } else {
        type = DATA_TYPE_RTP_A_PKT;
    }
    if (event & POLLOUT) {     /* Writable. */
        if (consume_send_buf(sessp, type) < 0) {
            destroy_rtsp_sess(sessp);
        }
    }

    return 0;
}

/**
 * Handle socket of RTCP.
 * @arg: Should be the struct rtsp_sess.
 *
 * NOTE:
 * Strictly speaking, handling non-interleaved mode only.
 */
int handle_rtcp_sd(struct sd_handler *hp, unsigned int event)
{
    return 0;
}

void do_sd_handler(int sd, unsigned int event)
{
    struct sd_handler *hp = NULL;    /* Used as loop curser. */

    /* Lookup current sd in sd handler list. */
    list_for_each_entry(hp, &rtsp_srv.sd_handler_list, entry) {
        if (hp->sd == sd) { /* Matched. */
            hp->proc(hp, event);
            break;
        }
    }

    return;
}

/**
 * Register sd handler and add it to the handler list.
 */
int register_sd_handler(int sd, sd_handler_proc_t proc, void *arg)
{
    struct sd_handler *hp = NULL;

    hp = mallocz(sizeof(*hp));
    if (!hp) {
        printf(ERR "Allocate memory for struct sd_handler failed!\r\n");
        return -1;
    }

    hp->sd = sd;
    hp->arg = arg;
    hp->proc = proc;
    list_add_tail(&hp->entry, &rtsp_srv.sd_handler_list);
    return 0;
}

/**
 * Unregister sd handler and delete it from the handler list.
 */
void deregister_sd_handler(int sd)
{
    struct sd_handler *hp = NULL;

    unmonitor_sd_event(sd);

    list_for_each_entry(hp, &rtsp_srv.sd_handler_list, entry) {
        if (hp->sd == sd) {
            list_del(&hp->entry);
            freez(hp);
            break;
        }
    }
    return;
}

void init_sd_handler_list(void)
{
    INIT_LIST_HEAD(&rtsp_srv.sd_handler_list);
    return;
}

void deinit_sd_handler_list(void)
{
    struct sd_handler *hp = NULL;
    struct sd_handler *tmp = NULL;

    list_for_each_entry_safe(hp, tmp, &rtsp_srv.sd_handler_list, entry) {
        list_del(&hp->entry);
        freez(hp);
    }
    return;
}
