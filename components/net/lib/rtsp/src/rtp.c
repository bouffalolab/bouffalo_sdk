#include <lwip/sockets.h>
#include <errno.h>
#include "rtsp_sess.h"
#include "rtp.h"
#include "delay_task.h"
#include "sd_handler.h"
#include "log.h"
#include "jpeg_parser.h"

struct jpegframe_parser jpeg_parser;

/* Update sequence number in RTP header. */
#define update_seq(seq) do {                    \
        seq = htons(ntohs(seq) + 1);            \
    } while (0)

/* Update timestamp in RTP header. */
#define update_ts(ts, media) do {               \
        if (media == MEDIA_TYPE_VIDEO) {        \
            ts = htonl(ntohl(ts) + 90000 / rtsp_get_video_fps()); \
        } else if (media == MEDIA_TYPE_AUDIO) { \
            ts = htonl(ntohl(ts) + 8000*5 / 50); \
        }                                       \
    } while (0)
// TODO: set ts increase by type
// Ex: AAC - ts = htonl(ntohl(ts) + 44100 * (1000 / 43) / 1000);

static void init_rtp_hdr(struct rtp_hdr *hdrp, int media)
{
    hdrp->v = 2;
    hdrp->p = 0;
    hdrp->x = 0;
    hdrp->cc = 0;
    hdrp->m = 0;
    hdrp->pt = (media == MEDIA_TYPE_VIDEO) ?
        rtsp_get_videoFmt() : rtsp_get_audioFmt();
    hdrp->seq = random();
    hdrp->ts = 0;//random();
    hdrp->ssrc = random();
    return;
}

/**
 * Depacketize the NALU into small enough slices.
 * And packetize them into RTP packets.
 */
static int produce_rtp_pkt(struct rtsp_sess *sessp, enum media_type media,
                           struct rtp_hdr *rtp_hdrp, struct rtp_pl *rtp_plp)
{
    struct send_buf *sendp = NULL;
    char *ptr = NULL;
    char nalu_pt = 0; /* Payload type. */
    int send_sz = 0;
    enum data_type data_type;
    struct intlvd intlvd = {
        .dollar = '$',
        .chn = sessp->rtp_rtcp[media].tcp.rtp_chn,
    };

    data_type = (media == MEDIA_TYPE_VIDEO) ? DATA_TYPE_RTP_V_PKT : DATA_TYPE_RTP_A_PKT;
    sendp = create_send_buf(data_type, MAX_RTP_PKT_SZ);
    if (!sendp) {
        printf("SendpBuf unavailable\r\n");
        return -1;
    }
    ptr = sessp->intlvd_mode ? (sendp->buf + sizeof(intlvd)) : sendp->buf;

    memcpy(ptr, rtp_hdrp, sizeof(*rtp_hdrp));

    ptr += sizeof(*rtp_hdrp);
    switch (rtp_hdrp->pt) {
    case RTP_PT_H264:
        nalu_pt = *((char *)rtp_plp) & 0x1F; /* First byte in struct rtp_pl & 0x1F */
        switch (nalu_pt) {
        case 1 ... 23:            /* Single NALU Packet. */
            *ptr++ = rtp_plp->single.hdr;
            memcpy(ptr, rtp_plp->single.pl, rtp_plp->single.pl_sz);
            send_sz += rtp_plp->single.pl_sz +
                sizeof(rtp_plp->single.hdr);
            break;
        case NALU_PT_FU_A:
            *ptr++ = rtp_plp->fu_a.idr;
            *ptr++ = rtp_plp->fu_a.hdr;
            memcpy(ptr, rtp_plp->fu_a.pl, rtp_plp->fu_a.pl_sz);
            send_sz += rtp_plp->fu_a.pl_sz +
                sizeof(rtp_plp->fu_a.idr) +
                sizeof(rtp_plp->fu_a.hdr);
            break;
        case NALU_PT_FU_B:
            *ptr++ = rtp_plp->fu_b.idr;
            *ptr++ = rtp_plp->fu_b.hdr;
            *ptr++ = (char)(rtp_plp->fu_b.don >> 8);
            *ptr++ = (char)rtp_plp->fu_b.don;
            memcpy(ptr, rtp_plp->fu_b.pl, rtp_plp->fu_b.pl_sz);
            send_sz += rtp_plp->fu_b.pl_sz +
                sizeof(rtp_plp->fu_b.idr) +
                sizeof(rtp_plp->fu_b.hdr);
            break;
        default:
            printf("Unsupported or undefined NALU payload type[%d]\r\n", nalu_pt);
            return -1;
        }
        break;
    case RTP_PT_PCMA:
    case RTP_PT_PCMU:
    case RTP_PT_OPUS:
    case RTP_PT_AAC:
        memcpy(ptr, rtp_plp->audio.pl, rtp_plp->audio.pl_sz);
        send_sz += rtp_plp->audio.pl_sz;
        break;
    case RTP_PT_JPEG:
        ptr[0] = 0;
        ptr[1] = rtp_plp->jpeg.jpghdr.off>>16;
        ptr[2] = rtp_plp->jpeg.jpghdr.off>>8;
        ptr[3] = rtp_plp->jpeg.jpghdr.off>>0;
        ptr[4] = rtp_plp->jpeg.jpghdr.type;
        ptr[5] = rtp_plp->jpeg.jpghdr.q;
        ptr[6] = rtp_plp->jpeg.jpghdr.width;
        ptr[7] = rtp_plp->jpeg.jpghdr.height;
        ptr += sizeof(rtp_plp->jpeg.jpghdr);
        send_sz +=8;

        if (rtp_plp->jpeg.jpghdr.q >= 128 && rtp_plp->jpeg.jpghdr.off == 0) {
            // memcpy(ptr, &qtblhdr, sizeof(qtblhdr));//notice cpu endian
            ptr[0] = 0;
            ptr[1] = 0;
            ptr[2] = jpeg_parser.qTablesLength>>8;
            ptr[3] = jpeg_parser.qTablesLength&0xff;
            ptr += sizeof(rtp_plp->jpeg.qtblhdr);
            memcpy(ptr, jpeg_parser.qTables, jpeg_parser.qTablesLength);
            ptr += jpeg_parser.qTablesLength;
            send_sz +=jpeg_parser.qTablesLength;
            send_sz +=4;

        }
        rtp_plp->jpeg.jpgfrm.data_len = MAX_RTP_PKT_SZ - (ptr-sendp->buf);
        memcpy(ptr, rtp_plp->jpeg.jpgfrm.jpeg_data + rtp_plp->jpeg.jpghdr.off, rtp_plp->jpeg.jpgfrm.data_len);
        send_sz += rtp_plp->jpeg.jpgfrm.data_len;
        break;
    default:
        printf("Unsupported or undefined RTP payload type[%d]\r\n", rtp_hdrp->pt);
        return -1;
    }
    
    send_sz += sizeof(*rtp_hdrp);
    if (sessp->intlvd_mode) {
        intlvd.sz = htons(send_sz);
        send_sz += sizeof(intlvd);
        memcpy(sendp->buf, &intlvd, sizeof(intlvd));
    }
    sendp->sz = send_sz;
    add_send_buf(sessp, sendp);
    return 0;
}
static int send_h264_nalu(struct rtsp_sess *sessp,
                          char *nalu, unsigned int sz, int last, uint32_t timestamp)
{
    struct rtp_hdr *rtp_hdrp = &sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].rtp_hdr;
    struct rtp_pl rtp_pl;
    unsigned int left = sz;
    unsigned int num = 0;       /* RTP packet number. */
    int max_fu_sz = MAX_RTP_PL_SZ - 2; /* 2 = size of FU-A indicator and header. */
    int pl_sz = 0;                     /* Payload size. */
    char *ptr = nalu;
    int end = 0;                /* Last FU fragment. */
    int i = 0;

    if (sz <= MAX_RTP_PL_SZ) {
        rtp_hdrp->m = last;
        rtp_pl.single.hdr = nalu[0];
        rtp_pl.single.pl = nalu + 1;
        rtp_pl.single.pl_sz = sz - 1;
        rtp_hdrp->ts = htonl((uint32_t)timestamp * (90000/1000));
        produce_rtp_pkt(sessp, MEDIA_TYPE_VIDEO, rtp_hdrp, &rtp_pl);
        update_seq(rtp_hdrp->seq);
    } else {
        /* Skip header of NALU. */
        left--;
        ptr++;

        /* Calculate RTP packet number. */
        num = (left / max_fu_sz) + ((left % max_fu_sz) != 0);
        
        rtp_hdrp->m = 0;
        rtp_hdrp->ts = htonl((uint32_t)timestamp * (90000/1000));
        for (i = 0; i < num; i++) {
            end = (i == num - 1);
            /* RTP header */
            rtp_hdrp->m = end ? last : 0;

            /* FU indicator: data[0] = F | NRI | TYPE. */
            rtp_pl.fu_a.idr = (nalu[0] & 0xF0) | (nalu[0] & 0x60) | NALU_PT_FU_A;

            /* FU header: data[1] = S | E | R | TYPE, R = 0. */
            rtp_pl.fu_a.hdr = (i ? 0 : 0x80) | (end ? 0x40 : 0) | (nalu[0] & 0x1F);

            pl_sz = end ? left : max_fu_sz;
            rtp_pl.fu_a.pl = ptr;
            rtp_pl.fu_a.pl_sz = pl_sz;
            produce_rtp_pkt(sessp, MEDIA_TYPE_VIDEO, rtp_hdrp, &rtp_pl);
            update_seq(rtp_hdrp->seq);

            ptr += pl_sz;
            left -= pl_sz;
        }
    }

//    if (last) {
        //update_ts(rtp_hdrp->ts, MEDIA_TYPE_VIDEO);
       // rtp_hdrp->ts = htonl(timestamp * (90000/1000));
        //printf("[V][%d]rtp_hdrp->ts = %u\r\n", rtp_hdrp->seq, (int)ntohl(rtp_hdrp->ts));
//    }

    return 0;
}
static int send_mjpeg_frm(struct rtsp_sess *sessp,
                          char *fb, unsigned int sz, uint64_t timestamp)
{
    struct rtp_hdr *rtp_hdrp = &sessp->rtp_rtcp[MEDIA_TYPE_VIDEO].rtp_hdr;
    struct rtp_pl rtp_pl;
    int bytes_left = sz;
    unsigned char *ptr = (unsigned char*)fb;
    rtp_pl.jpeg.jpgfrm.jpeg_data= (unsigned char*)fb;
    if (sz <= MAX_RTP_PL_SZ) {
        rtp_hdrp->m = 0;
        rtp_pl.single.hdr = ptr[0];
        //rtp_pl.single.pl = ptr + 1;
        rtp_pl.single.pl_sz = sz - 1;
        rtp_hdrp->ts = htonl((uint32_t)timestamp * (90000/1000));
        produce_rtp_pkt(sessp, MEDIA_TYPE_VIDEO, rtp_hdrp, &rtp_pl);
        update_seq(rtp_hdrp->seq);
    } else {
        parse_jpeg_req(ptr, sz, &jpeg_parser);
        jpeg_parser.qFactor = 255;
        int dri = jpeg_parser.driFound;

        /* Initialize RTP header
         */
        rtp_hdrp->v = 2;
        rtp_hdrp->p = 0;
        rtp_hdrp->x = 0;
        rtp_hdrp->cc = 0;
        rtp_hdrp->m = 0;
        rtp_hdrp->pt = RTP_PT_JPEG;
        rtp_hdrp->ts =  htonl((uint32_t)timestamp * (90000/1000));
        rtp_hdrp->ssrc = 0;

        /* Initialize JPEG header
         */

        rtp_pl.jpeg.jpghdr.tspec = 0;
        rtp_pl.jpeg.jpghdr.off = 0;
        rtp_pl.jpeg.jpghdr.type = jpeg_parser.type | ((dri != 0) ? RTP_JPEG_RESTART : 0);
        rtp_pl.jpeg.jpghdr.q = jpeg_parser.qFactor;
        rtp_pl.jpeg.jpghdr.width = jpeg_parser.width;
        rtp_pl.jpeg.jpghdr.height = jpeg_parser.height;

        /* Initialize DRI header
         */
        if (dri != 0) {
                rtp_pl.jpeg.rsthdr.dri = dri;
                rtp_pl.jpeg.rsthdr.f = 1;        /* This code does not align RIs */
                rtp_pl.jpeg.rsthdr.l = 1;
                rtp_pl.jpeg.rsthdr.count = 0x3fff;
        }

        /* Initialize quantization table header
         */
        if ( rtp_pl.jpeg.jpghdr.q >= 128) {
                rtp_pl.jpeg.qtblhdr.mbz = 0;
                rtp_pl.jpeg.qtblhdr.precision = 0; /* This code uses 8 bit tables only */
                rtp_pl.jpeg.qtblhdr.length = 128;  /* 2 64-byte tables */
        }

        rtp_pl.jpeg.jpgfrm.data_len = 0;
        while (bytes_left > 0) {
            if (rtp_pl.jpeg.jpgfrm.data_len >= bytes_left) {
                    rtp_pl.jpeg.jpgfrm.data_len = bytes_left;
                    rtp_hdrp->m = 1;
            }
            produce_rtp_pkt(sessp, MEDIA_TYPE_VIDEO, rtp_hdrp, &rtp_pl);
            rtp_pl.jpeg.jpghdr.off += rtp_pl.jpeg.jpgfrm.data_len;
            bytes_left -= rtp_pl.jpeg.jpgfrm.data_len;
            update_seq(rtp_hdrp->seq);
        }
    }
        return 0;
}



static int send_video_frm(struct rtsp_sess *sessp,
                          char *fb, unsigned int sz, uint64_t timestamp)
{
    char *end = fb + sz;
    char *ptr = fb;
    unsigned int nalu_sz = 0;
    int last = 0;               /* Last NALU of the frame. */

    while (ptr != end) {
        ptr = skip_nalu_start_code(ptr, end);
        if (!ptr) {
            break;
        }
        
#if 0
        /* We've send sps & pps NALU to RTSP client
         * in sdp when responsing RTSP method 'DESCRIBE',
         * needn't repeat this again. */
        if ((ptr[0] & 0x1F) == NALU_TYPE_SPS ||
            (ptr[0] & 0x1F) == NALU_TYPE_PPS) {
            continue;
        }
#endif

        nalu_sz = get_nalu_sz(ptr, end);
        last = (ptr + nalu_sz == end) ? 1 : 0;
        send_h264_nalu(sessp, ptr, nalu_sz, last, timestamp);
        ptr += nalu_sz;
    }

    return 0;
}

static int send_audio_frm(struct rtsp_sess *sessp,
                          char *fb, unsigned int sz, uint32_t timestamp)
{
    char *ptr = fb;
    struct rtp_hdr *rtp_hdrp = &sessp->rtp_rtcp[MEDIA_TYPE_AUDIO].rtp_hdr;
    struct rtp_pl rtp_pl;

    if (sz <= MAX_RTP_PL_SZ) {
        rtp_hdrp->m = 1;
        if (rtp_hdrp->pt == RTP_PT_AAC) {
            uint32_t frm_size = ((ptr[3] & 0x3) << 11) | (ptr[4] << 3) | (ptr[5] >> 5);
            rtp_pl.audio.pl = ptr+7-4;
            frm_size = frm_size - 7;
            rtp_pl.audio.pl_sz = frm_size+4;
            rtp_pl.audio.pl[0] = 0x00;
            rtp_pl.audio.pl[1] = 0x10;
            rtp_pl.audio.pl[2] = (frm_size & 0x1FE0) >> 5;
            rtp_pl.audio.pl[3] = (frm_size & 0x1F) << 3;
        } else {
            rtp_pl.audio.pl = fb;
            rtp_pl.audio.pl_sz = sz;
        }

        rtp_hdrp->ts = htonl((uint32_t)timestamp * (8000 / 1000));
        produce_rtp_pkt(sessp, MEDIA_TYPE_AUDIO, rtp_hdrp, &rtp_pl);
        update_seq(rtp_hdrp->seq);
        //rtp_hdrp->ts = htonl(timestamp * 8000 / 1000);
        //update_ts(rtp_hdrp->ts, MEDIA_TYPE_AUDIO);
        //printf("[A][%d]rtp_hdrp->ts = %u\r\n", rtp_hdrp->seq, (int)ntohl(rtp_hdrp->ts));
    } else {
        printf("Audio frame size (%d) larger than single packet\r\n", sz);
    }

    return 0;
}

/**
 * Prefetch one frame which is pure data encoded in H.264,
 * with NALU start code.
 */
int prefetch_frm(struct rtsp_sess *sessp)
{
    int next_delay = 0;
    struct frm_info *frmp = &sessp->strm.frm_info;

//    elapsed_time();

    /* Wait until RTSP state is RTSP_STATE_PLAYING,
     * and last response message has been add to send buffer queue. */
    if (sessp->rtsp_state != RTSP_STATE_PLAYING ||
        sessp->handling_state != HANDLING_STATE_INIT) {
        return -1;
    }

    /* Get frame and send it out. (Actually, just add to the send queue.) */
    if (sessp->strm.get_frm(&sessp->strm.strm_info, &sessp->strm.frm_info) <= 0) {
        /* Retry 5ms later. */
        next_delay = 1 * 1000;
    } else {
        switch (frmp->frm_type) {
        case FRM_TYPE_I:
        case FRM_TYPE_B:
        case FRM_TYPE_P:
            send_video_frm(sessp, frmp->frm_buf, frmp->frm_sz, frmp->timestamp);
            break;
        case FRM_TYPE_A:
            send_audio_frm(sessp, frmp->frm_buf, frmp->frm_sz, frmp->timestamp);
            break;
        case FRM_TYPE_M:
            send_mjpeg_frm(sessp, frmp->frm_buf, frmp->frm_sz, frmp->timestamp);
            break;
        default:
            printf("Unsupport frame type[%d]!\n", frmp->frm_type);
            return -1;
        }
        /* Do this again after 10ms later. */
        next_delay = 20 * 1000;
    }

    return next_delay;
}

/**
 * Setup interleaved mode.
 * Just initialize the intlvd struct in the beginning of RTP packet.
 */
static int setup_intlvd_mode(struct rtsp_sess *sessp)
{
    struct rtsp_req *req = sessp->req;
    struct rtsp_resp *resp = sessp->resp;
    int media = req->media;

    resp->resp_hdr.transport.rtp_chn = req->req_hdr.transport.rtp_chn;
    resp->resp_hdr.transport.rtcp_chn = req->req_hdr.transport.rtcp_chn;

    sessp->rtp_rtcp[media].tcp.rtp_chn = resp->resp_hdr.transport.rtp_chn;
    sessp->rtp_rtcp[media].tcp.rtcp_chn = resp->resp_hdr.transport.rtcp_chn;
    
    return 0;
}

/**
 * Setup non-interleaved mode.
 * Create two UDP sockets for RTSP server side,
 * then bind them to the local ports.
 * Generally, the two local ports are continuous number pair,
 * first port for RTP<transmitting media data>,
 * and second port for RTCP<transmitting control information>.
 */

static int setup_non_intlvd_mode(struct rtsp_sess *sessp)
{
    int first_sd = -1;
    int second_sd = -1;
    unsigned short first_port = 0;
    unsigned short second_port = 0;
    struct sockaddr_in rtsp_sa;
    struct sockaddr_in sa;      /* Use for creating RTP & RTCP socket. */
    socklen_t salen = 0;
    struct rtsp_req *req = sessp->req;
    struct rtsp_resp *resp = sessp->resp;
    int media = req->media;
    struct rtp_rtcp *rtp_rtcp = &sessp->rtp_rtcp[media];
  
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /* Create first socket and get first port. */
    if ((first_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf(ERR "Create first UDP socket error");
        goto err;
    }
    if (bind(first_sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        printf(ERR "Bind first UDP port error");
        goto err;
    }
    salen = sizeof(sa);
    if (getsockname(first_sd, &sa, &salen) < 0) {
        printf(ERR "Getsockname of first UDP socket error");
        goto err;
    }
    first_port = ntohs(sa.sin_port);
    second_port = (first_port % 2) ? (first_port - 1) : (first_port + 1);
    printf("==========RTP Port: %d, %d=========\r\n", first_port, second_port);
    /* Create second socket and get second port. */
    sa.sin_port = htons(second_port);
    if ((second_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf(ERR "Create second UDP socket error, errno: %d\r\n", errno);
        goto err;
    }
    if (bind(second_sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
//        if (errno == EADDRINUSE) { /* Address already in use. */
            sa.sin_port = 0;       /* Give up to use continuous number. */
            if (bind(second_sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
                printf(ERR "Bind second UDP sockaddr error");
                goto err;
            }
//        } else {
//            printf(ERR "Bind second UDP sockaddr error");
//            goto err;
//        }
    }
    if (getsockname(second_sd, &sa, &salen) < 0 ) {
        printf(ERR "Getsockname of second UDP socket error");
        goto err;
    }
    second_port = ntohs(sa.sin_port);

    /* Assign UDP socket to server for RTP & RTCP. */
    if (first_port % 2) {  /* Odd port. */
        rtp_rtcp->udp.rtp_sd = second_sd;
        rtp_rtcp->udp.rtcp_sd = first_sd;
    } else {               /* Even port. */
        rtp_rtcp->udp.rtp_sd = first_sd;
        rtp_rtcp->udp.rtcp_sd = second_sd;
    }

    /* Add rtp_sd & rtcp_sd to epoll events. */
    if ((monitor_sd_event(rtp_rtcp->udp.rtp_sd, RTP_SD_DFL_EV) < 0) ||
        (monitor_sd_event(rtp_rtcp->udp.rtcp_sd, RTCP_SD_DFL_EV) < 0)) {
        printf(ERR "monitor rtp_sd or rtcp_sd handler failed!\n");
        goto err;
    }

    /* Register RTP & RTCP sd handler. */
    if ((register_sd_handler(rtp_rtcp->udp.rtp_sd, handle_rtp_sd, sessp) < 0) ||
        (register_sd_handler(rtp_rtcp->udp.rtcp_sd, handle_rtcp_sd, sessp) < 0)) {
        printf(ERR "register rtp_sd or rtcp_sd handler failed!\n");
        goto err;
    }
    
    /* Get UDP socket address information of client. */
    salen = sizeof(sa);
    if (getpeername(sessp->rtsp_sd, &rtsp_sa, &salen) < 0) {
        printf(ERR "Getpeername of RTSP client socket");
        goto err;
    }
    memcpy(&rtp_rtcp->udp.rtp_sa, &rtsp_sa, sizeof(sa));
    memcpy(&rtp_rtcp->udp.rtcp_sa, &rtsp_sa, sizeof(sa));
    ((struct sockaddr_in *)&rtp_rtcp->udp.rtp_sa)->sin_port =
        htons(req->req_hdr.transport.rtp_cli_port);
    ((struct sockaddr_in *)&rtp_rtcp->udp.rtcp_sa)->sin_port =
        htons(req->req_hdr.transport.rtcp_cli_port);

    /* Prepare RTSP response. */
    resp->resp_hdr.transport.rtp_cli_port = req->req_hdr.transport.rtp_cli_port;
    resp->resp_hdr.transport.rtcp_cli_port = req->req_hdr.transport.rtcp_cli_port;
    resp->resp_hdr.transport.rtp_srv_port = (first_port % 2) ? second_port : first_port;
    resp->resp_hdr.transport.rtcp_srv_port = (first_port % 2) ? first_port : second_port;

    return 0;

err:
    close(first_sd);
    close(second_sd);
    return -1;
}

int create_rtp_sess(struct rtsp_sess *sessp)
{
    int media = sessp->req->media; /* Media type: video, audio. */
    struct rtp_rtcp *rtp_rtcp = &sessp->rtp_rtcp[media];

    if (sessp->rtsp_state == RTSP_STATE_INIT) {
        sessp->sess_id = random64();
        sessp->resp->resp_hdr.sess_id = sessp->sess_id;
    }

    init_rtp_hdr(&rtp_rtcp->rtp_hdr, media);

    if (sessp->intlvd_mode) {
        if (setup_intlvd_mode(sessp) < 0) {
            return -1;
        }
    } else {
        if (setup_non_intlvd_mode(sessp) < 0) {
            return -1;
        }
    }
   
    return 0;
}

void destroy_rtp_sess(struct rtsp_sess *sessp)
{
    int i = 0;

    if (sessp->intlvd_mode) {
        /* TODO: Is there anything to be done? */
    } else {
        if (rtsp_get_video_en()) {
            deregister_sd_handler(sessp->rtp_rtcp[i].udp.rtp_sd);
            close(sessp->rtp_rtcp[i].udp.rtp_sd);
            sessp->rtp_rtcp[i].udp.rtp_sd = -1;
            
            deregister_sd_handler(sessp->rtp_rtcp[i].udp.rtcp_sd);
            close(sessp->rtp_rtcp[i].udp.rtcp_sd);
            sessp->rtp_rtcp[i].udp.rtcp_sd = -1;
        }
        i++;
        if (rtsp_get_audio_en()) {
            deregister_sd_handler(sessp->rtp_rtcp[i].udp.rtp_sd);
            close(sessp->rtp_rtcp[i].udp.rtp_sd);
            sessp->rtp_rtcp[i].udp.rtp_sd = -1;
            
            deregister_sd_handler(sessp->rtp_rtcp[i].udp.rtcp_sd);
            close(sessp->rtp_rtcp[i].udp.rtcp_sd);
            sessp->rtp_rtcp[i].udp.rtcp_sd = -1;
        }
    }
    return;
}
