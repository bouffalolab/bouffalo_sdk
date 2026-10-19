#include <stdio.h>
#include <lwip/inet.h>
#include <sys/types.h>
#include <lwip/sockets.h>
#include "list.h"
#include "util.h"
#include "log.h"
#include "rtsp_srv.h"
#include "rtsp_parser.h"
#include "delay_task.h"
#include "rtsp_method.h"
#include "sd_handler.h"
#include "rtp.h"


#define CRLF    "\r\n"
#define BUF_ALLOC_LIMIT     (500)
static uint32_t gAllocCnt = 0;
static uint8_t gAllocFailRateLimit = 0;

struct send_buf *create_send_buf(enum data_type type, unsigned int sz)
{
    struct send_buf *sendp = NULL;

    if (gAllocCnt >= BUF_ALLOC_LIMIT) {
        if (gAllocFailRateLimit++ < 5)
            printf("too many allocate buffer, check tcp throughput\r\n");
        return NULL;
    } else {
        gAllocFailRateLimit = 0;
    }

    sendp = mallocz(sizeof(*sendp));
    if (!sendp) {
        printf("Allocate memory for send buffer failed!\r\n");
        return NULL;
    }
    sendp->sz = sz;
    sendp->type = type;
    sendp->buf = mallocz(sz);
    if (!sendp->buf) {
        printf("Allocate memory for sendp->buf failed!\r\n");
        freez(sendp);
        return NULL;
    }
    gAllocCnt++;
//    printf("[%d]allocate 0x%x, 0x%x\r\n", gAllocCnt, (uint32_t)sendp, (uint32_t)sendp->buf);
    return sendp;
}

void destroy_send_buf(struct send_buf *sendp)
{
    gAllocCnt--;
//    printf("[%d]free 0x%x, 0x%x\r\n", gAllocCnt, (uint32_t)sendp, (uint32_t)sendp->buf);
    freez(sendp->buf);
    freez(sendp);

    return;
}

void add_send_buf(struct rtsp_sess *sessp, struct send_buf *sendp)
{
    /* Put it into send buffer queue. */
    list_add_tail(&sendp->entry, &sessp->send_queue);
    return;
}

void del_send_buf(struct send_buf *sendp)
{
    /* Delete from send buffer queue. */
    list_del(&sendp->entry);
    return;
}

static char *make_date_hdr(void)
{
    static char date[256];
    time_t tm;

    tm = time(NULL);
    strftime(date, sizeof(date),
            "Date: %a, %b %d %Y %H:%M:%S GMT"CRLF, gmtime(&tm));
    return date;
}

/**
 * Produce RTSP response message.
 * The buffer data produced here will be consumed
 * by function consume_send_buf().
 */
void produce_rtsp_resp(struct rtsp_sess *sessp)
{
    /* Use this MACRO to move forward while filling the response message. */
#define MOVE_FORWARD() do {                                         \
        left -= strlen(line);                                       \
        if (left < 0)  {                                            \
            sessp->handling_state = HANDLING_STATE_FAILED;          \
            printf("MOVE_FORWARD failed when parsing request.\n");  \
            return;                                                 \
        }                                                           \
        line += strlen(line);                                       \
    } while (0)

    struct send_buf *sendp = NULL;
    char *line = NULL;
    int left = 0;
    struct rtsp_req *req = sessp->req;
    struct rtsp_resp *resp = sessp->resp;

    if (!rtsp_sess_exist(sessp)) {
        return;
    }

    if (sessp->handling_state != HANDLING_STATE_ERROR &&
        sessp->handling_state != HANDLING_STATE_HANDLED) {
        return;
    }

    sendp = create_send_buf(DATA_TYPE_RTSP_RESP, RTSP_RESP_SZ);
    if (!sendp) {
        sessp->handling_state = HANDLING_STATE_FAILED;
        return;
    }
    line = sendp->buf;
    left = sendp->sz;

    /* Status line of response message. */
    MOVE_FORWARD();
    snprintf(line, left, "%s %d %s"CRLF, resp->resp_line.ver,
             resp->resp_line.status_code,
             get_status_reason(resp->resp_line.status_code));

    /* Common response header fields. */
    MOVE_FORWARD();
    snprintf(line, left, "CSeq: %u"CRLF, resp->resp_hdr.cseq);
    MOVE_FORWARD();
    snprintf(line, left, "%s", make_date_hdr());
    MOVE_FORWARD();
    snprintf(line, left, "Server: %s"CRLF, resp->resp_hdr.server);

    /* If we are at state HANDLING_STATE_FAILED, don't do following stuff. */
    if (sessp->handling_state == HANDLING_STATE_HANDLED) {
        if (resp->resp_hdr.sess_id) {
            MOVE_FORWARD();
            snprintf(line, left, "Session: %llu"CRLF, resp->resp_hdr.sess_id);
        }

        /* Response header fields of different method. */
        switch (req->req_line.method) {
        case RTSP_METHOD_OPTIONS:
            MOVE_FORWARD();
            snprintf(line, left, "Public: %s"CRLF, resp->resp_hdr.public);
            break;
        case RTSP_METHOD_DESCRIBE:
            MOVE_FORWARD();
            snprintf(line, left, "Content-type: application/sdp"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "Content-length: %d"CRLF,
                     (int)strlen(resp->resp_body.sdp_info));
            MOVE_FORWARD();
            snprintf(line, left, "%s", resp->resp_body.sdp_info);
            break;
        case RTSP_METHOD_SETUP:
            MOVE_FORWARD();
            if (sessp->intlvd_mode) {
                snprintf(line, left, "Transport: RTP/AVP/TCP;unicast;"
                         "interleaved=%hhd-%hhd"CRLF,
                         resp->resp_hdr.transport.rtp_chn,
                         resp->resp_hdr.transport.rtcp_chn);
            } else {
                snprintf(line, left, "Transport: RTP/AVP/UDP;unicast;"
                         "client_port=%hu-%hu;server_port=%hu-%hu"CRLF,
                         resp->resp_hdr.transport.rtp_cli_port,
                         resp->resp_hdr.transport.rtcp_cli_port,
                         resp->resp_hdr.transport.rtp_srv_port,
                         resp->resp_hdr.transport.rtcp_srv_port);
            }
            break;
        case RTSP_METHOD_PLAY:
            break;
        case RTSP_METHOD_PAUSE:
            break;
        case RTSP_METHOD_GET_PARAMETER:
            break;
        case RTSP_METHOD_SET_PARAMETER:
            break;
        case RTSP_METHOD_TEARDOWN:
            break;
        default:
            break;
        }
    }

    /* End of response message. */
    MOVE_FORWARD();
    snprintf(line, left, CRLF);

    /* Get the actual message size which will be sent. */
    MOVE_FORWARD();
    sendp->sz -= left;

    add_send_buf(sessp, sendp);

    printf("S->C %s\r\n", sendp->buf);

    sessp->handling_state = HANDLING_STATE_DONE;
    return;

#undef MOVE_FORWARD
}

void run_rtsp_state_machine(struct rtsp_sess *sessp)
{
    static void (*handle_method[])(struct rtsp_sess *) = {
        [RTSP_METHOD_OPTIONS]       = handle_method_options,
        [RTSP_METHOD_DESCRIBE]      = handle_method_describe,
        [RTSP_METHOD_SETUP]         = handle_method_setup,
        [RTSP_METHOD_PLAY]          = handle_method_play,
        [RTSP_METHOD_PAUSE]         = handle_method_pause,
        [RTSP_METHOD_GET_PARAMETER] = handle_method_get_parameter,
        [RTSP_METHOD_SET_PARAMETER] = handle_method_set_parameter,
        [RTSP_METHOD_TEARDOWN]      = handle_method_teardown,
    };

    if (sessp->handling_state != HANDLING_STATE_PARSED &&
        sessp->handling_state != HANDLING_STATE_RETRY) {
        return;
    }
    
    /* Handle the RTSP method of requested. */
    handle_method[sessp->req->req_line.method](sessp);

    return;
}

static ssize_t send_buf_data(struct rtsp_sess *sessp, struct send_buf *sendp)
{
    ssize_t ns = 0;             /* Bytes sent.  */
    int sd = -1;
    struct sockaddr *sap = NULL;
    int next_ev = 0;

    if (sessp->intlvd_mode || sendp->type == DATA_TYPE_RTSP_RESP) {
        sd = sessp->rtsp_sd;
        next_ev = RTSP_SD_DFL_EV;
        ns = send(sd, sendp->buf, sendp->sz, 0);
        if (ns < 0 || ns != sendp->sz) {
            printf("send_buf_data ns = %zd\r\n", ns);
        }
    } else {
        switch (sendp->type) {
        case DATA_TYPE_RTP_V_PKT:
            sd = sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].udp.rtp_sd;
            sap = &sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].udp.rtp_sa;
            next_ev = RTP_SD_DFL_EV;
            break;
        case DATA_TYPE_RTP_A_PKT:
            sd = sessp->rtp_rtcp[MEDIA_TYPE_AUDIO].udp.rtp_sd;
            sap = &sessp->rtp_rtcp[MEDIA_TYPE_AUDIO].udp.rtp_sa;
            next_ev = RTP_SD_DFL_EV;
            break;
        case DATA_TYPE_RTCP_V_PKT:
            sd = sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].udp.rtcp_sd;
            next_ev = RTCP_SD_DFL_EV;
            sap = &sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].udp.rtcp_sa;
            break;
        case DATA_TYPE_RTCP_A_PKT:
            sd = sessp->rtp_rtcp[MEDIA_TYPE_AUDIO].udp.rtcp_sd;
            next_ev = RTCP_SD_DFL_EV;
            sap = &sessp->rtp_rtcp[MEDIA_TYPE_AUDIO].udp.rtcp_sa;
            break;
        default:
            return -1;
        }
        ns = sendto(sd, sendp->buf, sendp->sz, 0,
                    sap, sizeof(struct sockaddr));
    }

    update_sd_event(sd, next_ev);
    return ns;
}

/**
 * @brief: Find the specified type buffer in send queue, and send it out.
 *         Then remove the buffer from the queue.
 *
 * @type: The buffer type wanted to be consumed.
 *
 * NOTE:
 * To guarantee the order of sending RT(C)P packet(including video & audio),
 * we stop iterating the queue when found one kind of RT(C)P packet, no matter
 * it's kind of video or audio.
 * Maybe it's hard to understand, just reading the code.
 */
int consume_send_buf(struct rtsp_sess *sessp, enum data_type type)
{
    struct send_buf *sendp = NULL; /* Send buffer pointer. */

    if (!list_empty(&sessp->send_queue)) {
        sendp = list_first_entry(&sessp->send_queue, struct send_buf, entry);
        if (sendp->type & type) {
            if (send_buf_data(sessp, sendp) < 0) {
//                return -1;
            }
            del_send_buf(sendp);
            destroy_send_buf(sendp);
        }
    }

    return 0;
}

/**
 * Create one session for it
 * and add it to the session list.
 */
int create_rtsp_sess(int rtsp_sd)
{
    struct rtsp_sess *sessp = NULL;  /* RTSP session information. */

    /* Allocate a rtsp_sess struct for this session. */
    sessp = mallocz(sizeof(*sessp));
    if (!sessp) {
        printf(ERR "Allocate memory for RTSP session failed!\r\n");
        goto err;
    }

    /* Register sd handler. */
    if (register_sd_handler(rtsp_sd, handle_rtsp_sd, sessp) < 0) {
        printf(ERR "register rtsp_sd handler failed!\r\n");
        goto err;
    }
    sessp->rtsp_sd = rtsp_sd;

    /* Allocate memory for receive buffer. */
    sessp->recv_buf = mallocz(RECV_BUF_SZ);
    if (!sessp->recv_buf) {
        printf("ERR Allocate memory for receive buffer failed!\r\n");
        goto err;
    }

    /* Allocate memory for frame buffer. */
    sessp->strm.frm_info.frm_buf = mallocz(MAX_FRM_SZ);
    if (!sessp->strm.frm_info.frm_buf) {
        printf("ERR Allocate memory for frame buffer failed!\r\n");
        goto err;
    }

    /* Initialize the stream information. */
    get_strm_cb(&sessp->strm.get_frm);

    /* Other initializations. */
    sessp->rtsp_state = RTSP_STATE_INIT;
    sessp->handling_state = HANDLING_STATE_INIT;
    INIT_LIST_HEAD(&sessp->send_queue);

    /* Add to session list. */
    list_add(&sessp->entry, &rtsp_srv.rtsp_sess_list);
    return 0;

err:
    deregister_sd_handler(rtsp_sd);
    freez(sessp->strm.frm_info.frm_buf);
    freez(sessp->recv_buf);
    freez(sessp);
    return -1;
}

/**
 * Delete the session from session list,
 * and destroy its relevant resource.
 *
 * Return -1, means task finished.
 * Return >= 0, means task will be delay, and perform it later.
 */
int destroy_rtsp_sess(struct rtsp_sess *sessp)
{
    struct send_buf *sendp = NULL;
    struct send_buf *tmp_sendp = NULL;

    if (sessp->handling_state != HANDLING_STATE_INIT) {
        return 10 * 1000;       /* Retry 10ms later. */
    }
    sessp->rtsp_state = RTSP_STATE_INIT;

    /* Loopup RTSP session in list, maybe it has been deleted. */
    if (!rtsp_sess_exist(sessp)) {
        return -1;
    }

    list_del(&sessp->entry);

    freez(sessp->strm.strm_info.file_name);

    /* Close and deregister socket descriptors. */
    deregister_sd_handler(sessp->rtsp_sd);
    close(sessp->rtsp_sd);

    /* Free send buffers in queue. */
    list_for_each_entry_safe(sendp, tmp_sendp, &sessp->send_queue, entry) {
        del_send_buf(sendp);
        destroy_send_buf(sendp);
    }

    /* Destroy delayed task in queue. */
    destroy_sess_task(sessp);

    /* Destroy RTP session. */
    destroy_rtp_sess(sessp);

    /* Free frame buffer. */
    freez(sessp->strm.frm_info.frm_buf);

    /* Free receive buffer. */
    freez(sessp->recv_buf);
    freez(sessp);
    
    /* Task finished. */
    return -1;
}

void destroy_rtsp_sess_contain_sd(int sd)
{
    struct rtsp_sess *sessp = NULL; /* Used as loop curser. */

    list_for_each_entry(sessp, &rtsp_srv.rtsp_sess_list, entry) {
        if (sessp->rtsp_sd == sd ||
            sessp->rtp_rtcp[0].udp.rtp_sd == sd ||
            sessp->rtp_rtcp[0].udp.rtcp_sd == sd ||
            sessp->rtp_rtcp[1].udp.rtp_sd == sd ||
            sessp->rtp_rtcp[1].udp.rtcp_sd == sd) {
            destroy_rtsp_sess(sessp);
            return;
        }
    }
    return;
}

int rtsp_sess_id_exist(unsigned long long sess_id)
{
    struct rtsp_sess *sessp = NULL; /* Used as loop curser. */

    list_for_each_entry(sessp, &rtsp_srv.rtsp_sess_list, entry) {
        if (sessp->sess_id == sess_id) {
            return 1;
        }
    }
    return 0;
}

int rtsp_sess_exist(struct rtsp_sess *sessp)
{
    struct rtsp_sess *tmp = NULL; /* Used as loop curser. */

    list_for_each_entry(tmp, &rtsp_srv.rtsp_sess_list, entry) {
        if (tmp == sessp) {
            return 1;
        }
    }
    return 0;
}

/**
 * Check send buffer queue in each RTSP session in list.
 * If there's any data to be sent, we add EPOLLOUT event to
 * corresponding socket descriptor.
 *
 * NOTE: The added EPOLLOUT event will be removed after the
 * data is sent out.
 */
int check_send_queue(void)
{
    struct rtsp_sess *sessp = NULL;
    struct send_buf *sendp = NULL;
    int sd = -1;
    int ev = POLLOUT;

    list_for_each_entry(sessp, &rtsp_srv.rtsp_sess_list, entry) {
        if (!list_empty(&sessp->send_queue)) {
            sendp = list_first_entry(&sessp->send_queue, struct send_buf, entry);
            if (sessp->intlvd_mode || sendp->type == DATA_TYPE_RTSP_RESP) {
                sd = sessp->rtsp_sd;
                ev |= RTSP_SD_DFL_EV;
            } else {
                switch (sendp->type) {
                case DATA_TYPE_RTP_V_PKT:
                    sd = sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].udp.rtp_sd;
                    ev |= RTP_SD_DFL_EV;
                    break;
                case DATA_TYPE_RTP_A_PKT:
                    sd = sessp->rtp_rtcp[MEDIA_TYPE_AUDIO].udp.rtp_sd;
                    ev |= RTP_SD_DFL_EV;
                    break;
                case DATA_TYPE_RTCP_V_PKT:
                    sd = sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].udp.rtcp_sd;
                    ev |= RTCP_SD_DFL_EV;
                    break;
                case DATA_TYPE_RTCP_A_PKT:
                    sd = sessp->rtp_rtcp[MEDIA_TYPE_AUDIO].udp.rtcp_sd;
                    ev |= RTCP_SD_DFL_EV;
                    break;
                default:
                    break;
                }
            }
            if (update_sd_event(sd, ev) < 0) {
                return -1;
            }
        }
    }
    return 0;
}

void init_rtsp_sess_list(void)
{
    INIT_LIST_HEAD(&rtsp_srv.rtsp_sess_list);
    return;
}

void deinit_rtsp_sess_list(void)
{
    struct rtsp_sess *sessp = NULL;
    struct rtsp_sess *tmp = NULL;

    list_for_each_entry_safe(sessp, tmp, &rtsp_srv.rtsp_sess_list, entry) {
        destroy_rtsp_sess(sessp);
    }
    return;
}
