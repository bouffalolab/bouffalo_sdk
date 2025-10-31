#include <string.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include "list.h"
#include "rtsp_srv.h"
#include "rtsp_sess.h"
#include "rtsp_parser.h"
#include "delay_task.h"
#include "util.h"
#include "rtp.h"
#include "log.h"


#define CRLF    "\r\n"

//#define RTSP_SEND_SPS_PPS_IN_SETUP_RESPONSE

/* RTSP methods we support now.  */
static char *const supported_methods =
    "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER";

static char *const rtsp_method_token[] = {
    [RTSP_METHOD_OPTIONS]       = "OPTIONS",
    [RTSP_METHOD_DESCRIBE]      = "DESCRIBE",
    [RTSP_METHOD_SETUP]         = "SETUP",
    [RTSP_METHOD_PLAY]          = "PLAY",
    [RTSP_METHOD_PAUSE]         = "PAUSE",
    [RTSP_METHOD_GET_PARAMETER] = "GET_PARAMETER",
    [RTSP_METHOD_SET_PARAMETER] = "SET_PARAMETER",
    [RTSP_METHOD_TEARDOWN]      = "TEARDOWN",
};


int get_rtsp_method(const char *str)
{
    int i = 0;
    int method = -1;
    int num = sizeof(rtsp_method_token) / sizeof(*rtsp_method_token);

    for (i = 0; i < num; i++) {
        if (!strncmp(str, rtsp_method_token[i], strlen(rtsp_method_token[i]))) {
            method = i;
            break;
        }
    }
    return method;
}

char *rtsp_encodeBase64(char *buf, int buf_len,
                        const unsigned char *src, int src_len)
{
#ifndef UINT_MAX
#define UINT_MAX 0xFFFFFFFF
#endif
    static const char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *ret, *dst;
    unsigned i_bits = 0;
    int i_shift = 0;
    int bytes_remaining = src_len;

    if (src_len >= UINT_MAX / 4 ||
        buf_len < src_len * 4 / 3 + 12) {
        return NULL;
    }
    ret = dst = buf;
    while (bytes_remaining) {
        i_bits = (i_bits << 8) + *src++;
        bytes_remaining--;
        i_shift += 8;

        do {
            *dst++ = b64[(i_bits << 6 >> i_shift) & 0x3f];
            i_shift -= 6;
        } while (i_shift > 6 || (bytes_remaining == 0 && i_shift > 0));
    }
    while ((dst - ret) & 3) {
        *dst++ = '=';
    }
    *dst = '\0';

    return ret;
}

static char *make_sdp_info(struct rtsp_sess *sessp, struct frm_info *frmp)
{
    /* Use this MACRO to move forward while filling the response message. */
#define MOVE_FORWARD() do {                                         \
        left -= strlen(line);                                       \
        if (left < 0)  {                                            \
            printf("MOVE_FORWARD failed when parsing request.\r\n");  \
            return NULL;                                            \
        }                                                           \
        line += strlen(line);                                       \
    } while (0)

    static char sdp[2048];
    char *line = NULL;
    int left = 0;
    int audio_en = rtsp_get_audio_en();
    int video_en = rtsp_get_video_en();
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);
#ifdef RTSP_SEND_SPS_PPS_IN_SETUP_RESPONSE
    char *b64_sps = NULL;
    char *b64_pps = NULL;
    char *sps = NULL;           /* Begin of sequence parameter sets. */
    char *pps = NULL;           /* Begin of picture parameter sets. */
    char *ptr = NULL;
    char *end = NULL;
    int nalu_type = 0;
    /* Get sps & pps information. */
    ptr = frmp->frm_buf;
    end = ptr + frmp->frm_sz;
    while (!sps || !pps) {
        if (!(ptr = skip_nalu_start_code(ptr, end))) {
            printf("skip_nalu_start_code failed!\n");
            return NULL;
        }
        nalu_type = ptr[0] & 0x1F;
        if (nalu_type == NALU_TYPE_SPS) {
            sps = ptr;
            continue;
        }
        if (nalu_type == NALU_TYPE_PPS) {
            pps = ptr;
            continue;
        }
    }
#endif
    /* Get IP address. */
    memset(&sa, 0, sizeof(sa));
    if (getsockname(sessp->rtsp_sd, (struct sockaddr *)&sa, &salen) < 0) {
        printf(ERR "getsockname of rtsp_sd error");
        return NULL;
    }

    /* Make up sdp information. */
    memset(sdp, 0, sizeof(sdp));
    line = sdp;
    left = sizeof(sdp);

    MOVE_FORWARD();
    snprintf(line, left, CRLF);
    MOVE_FORWARD();
    snprintf(line, left, "v=0"CRLF);
    MOVE_FORWARD();
    snprintf(line, left, "o=- %jd %d IN IP4 %s"CRLF,
             time(NULL), 1/*time(NULL) + 1*/, inet_ntoa(sa.sin_addr));
    MOVE_FORWARD();
    snprintf(line, left, "s=streamed by the BL MM RTSP server"CRLF);

    MOVE_FORWARD();
    snprintf(line, left, "t=0 0"CRLF);

    MOVE_FORWARD();
    snprintf(line, left, "a=control:*"CRLF);
    MOVE_FORWARD();
    snprintf(line, left, "a=type:broadcast"CRLF);

    if (video_en) {
        if (rtsp_get_videoFmt() == RTP_PT_H264) {
            MOVE_FORWARD();
            snprintf(line, left, "m=video 0 RTP/AVP %d"CRLF, RTP_PT_H264);
            MOVE_FORWARD();
            snprintf(line, left, "c=IN IP4 0.0.0.0"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=rtpmap:%d H264/90000"CRLF, RTP_PT_H264);
            MOVE_FORWARD();
            snprintf(line, left, "a=framerate:%d"CRLF, rtsp_get_video_fps());
            MOVE_FORWARD();
            snprintf(line, left, "a=control:track%d"CRLF, MEDIA_TYPE_VIDEO);
#ifdef RTSP_SEND_SPS_PPS_IN_SETUP_RESPONSE
            MOVE_FORWARD();
            if ((b64_sps = encode_base64(sps, get_nalu_sz(sps, end))) < 0) {
                return NULL;
            }
            if ((b64_pps = encode_base64(pps, get_nalu_sz(pps, end))) < 0) {
                freez(b64_sps);
                return NULL;
            }
            snprintf(line, left, "a=fmtp:%d packetization-mode=1;"
                    "profile-level-id=%02X%02X%02X;"
                    "sprop-parameter-sets=%s,%s"CRLF,
                    RTP_PT_H264,
                    (unsigned char)sps[1], (unsigned char)sps[2], (unsigned char)sps[3],
                    b64_sps, b64_pps);

            freez(b64_sps);
            freez(b64_pps);
#endif
        }else if (rtsp_get_videoFmt() == RTP_PT_JPEG) {
            MOVE_FORWARD();
            snprintf(line, left, "m=video 0 RTP/AVP %d"CRLF, RTP_PT_JPEG);
            MOVE_FORWARD();
            snprintf(line, left, "c=IN IP4 0.0.0.0"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=rtpmap:%d JPEG/90000"CRLF, RTP_PT_JPEG);
            MOVE_FORWARD();
            snprintf(line, left, "a=framerate:%d"CRLF, rtsp_get_video_fps());
            MOVE_FORWARD();
            snprintf(line, left, "a=control:track%d"CRLF, MEDIA_TYPE_VIDEO);
        }
    }
    
    if (audio_en) {
        if (rtsp_get_audioFmt() == RTP_PT_AAC) {
            MOVE_FORWARD();
            snprintf(line, left, "m=audio 0 RTP/AVP %d"CRLF, RTP_PT_AAC);
            MOVE_FORWARD();
            snprintf(line, left, "c=IN IP4 0.0.0.0"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=rtpmap:%d mpeg4-generic/44100/2"CRLF, RTP_PT_AAC);
            MOVE_FORWARD();
            snprintf(line, left, 
                    "a=fmtp:%d profile-level-id=1;"
                    "mode=AAC-hbr;"
                    "sizelength=13;indexlength=3;indexdeltalength=3;"
                    "config=%04u"CRLF,
                    RTP_PT_AAC,
                    1210);
            MOVE_FORWARD();
            snprintf(line, left, "a=control:track%d"CRLF, MEDIA_TYPE_AUDIO);
        } else if (rtsp_get_audioFmt() == RTP_PT_PCMA || rtsp_get_audioFmt() == RTP_PT_PCMU) {
            MOVE_FORWARD();
            snprintf(line, left, "m=audio 0 RTP/AVP %d"CRLF, rtsp_get_audioFmt());
            MOVE_FORWARD();
            snprintf(line, left, "c=IN IP4 0.0.0.0"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=rtpmap:%d %s/8000"CRLF, rtsp_get_audioFmt(),
                rtsp_get_audioFmt() == RTP_PT_PCMA ? "pcma" : "pcmu");
            MOVE_FORWARD();
            snprintf(line, left, "a=ptime:100"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=maxptime:100"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=control:track%d"CRLF, MEDIA_TYPE_AUDIO);
        } else if (rtsp_get_audioFmt() == RTP_PT_OPUS) {
            MOVE_FORWARD();
            snprintf(line, left, "m=audio 0 RTP/AVP %d"CRLF, rtsp_get_audioFmt());
            MOVE_FORWARD();
            snprintf(line, left, "c=IN IP4 0.0.0.0"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=rtpmap:%d opus/48000/2"CRLF, rtsp_get_audioFmt());
            MOVE_FORWARD();
            snprintf(line, left, "a=fmtp:%d maxplaybackrate=16000; sprop-maxcapturerate=16000;maxaveragebitrate=20000; stereo=1; useinbandfec=1; usedtx=0"CRLF, rtsp_get_audioFmt());
            MOVE_FORWARD();
            snprintf(line, left, "a=ptime:100"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=maxptime:100"CRLF);
            MOVE_FORWARD();
            snprintf(line, left, "a=control:track%d"CRLF, MEDIA_TYPE_AUDIO);
        }
    }

//    MOVE_FORWARD();
//    snprintf(line, left, "a=range:npt=0-"CRLF);

    return sdp;

#undef MOVE_FORWARD
}

void handle_method_options(struct rtsp_sess *sessp)
{
    sessp->resp->resp_hdr.public = strndup(supported_methods, strlen(supported_methods));
    sessp->handling_state = HANDLING_STATE_HANDLED;
    return;
}

void handle_method_describe(struct rtsp_sess *sessp)
{
    struct rtsp_resp *resp = sessp->resp;
#if 0
    if (sessp->strm.get_frm(&sessp->strm.strm_info, &sessp->strm.frm_info) < 0) {
        sessp->handling_state = HANDLING_STATE_FAILED;
        goto rtn;
    }

    /* Retry to get frame, until I frame appears. */
    if (sessp->strm.frm_info.frm_type != FRM_TYPE_I) {
        sessp->handling_state = HANDLING_STATE_RETRY;
        goto rtn;
    }
#endif
    /* Make up SDP information. */
    resp->resp_body.sdp_info = make_sdp_info(sessp, &sessp->strm.frm_info);
    if (!resp->resp_body.sdp_info) {
        sessp->handling_state = HANDLING_STATE_FAILED;
        goto rtn;
    }
    
    sessp->handling_state = HANDLING_STATE_HANDLED;
    
rtn:

    return;
}

void handle_method_setup(struct rtsp_sess *sessp)
{
    if (create_rtp_sess(sessp) < 0) {
        sessp->handling_state = HANDLING_STATE_FAILED;
        return;
    }

    if (sessp->rtsp_state != RTSP_STATE_PLAYING) {
        sessp->rtsp_state = RTSP_STATE_READY;
    }
    sessp->handling_state = HANDLING_STATE_HANDLED;
    return;
}

void handle_method_play(struct rtsp_sess *sessp)
{
    struct rtsp_resp *resp = sessp->resp;
    if (sessp->rtsp_state == RTSP_STATE_INIT) {
        resp->resp_line.status_code = METHOD_NOT_VALID_IN_THIS_STATE;
        sessp->handling_state = HANDLING_STATE_ERROR;
        return;
    } else {
        sessp->rtsp_state = RTSP_STATE_PLAYING;
    }
    create_delay_task((delay_task_proc_t)prefetch_frm, sessp, 0);
    sessp->handling_state = HANDLING_STATE_HANDLED;
    return;
}

void handle_method_pause(struct rtsp_sess *sessp)
{
    if (sessp->rtsp_state == RTSP_STATE_PLAYING) {
        sessp->rtsp_state = RTSP_STATE_READY;
    }

    sessp->handling_state = HANDLING_STATE_HANDLED;
    return;
}

void handle_method_get_parameter(struct rtsp_sess *sessp)
{
    sessp->handling_state = HANDLING_STATE_HANDLED;
    return;
}

void handle_method_set_parameter(struct rtsp_sess *sessp)
{
    sessp->handling_state = HANDLING_STATE_HANDLED;
    return;
}

void handle_method_teardown(struct rtsp_sess *sessp)
{
    sessp->rtsp_state = RTSP_STATE_INIT;

    /* Destroy RTSP session 10ms later, after handling current method. */
    create_delay_task((delay_task_proc_t)destroy_rtsp_sess, sessp, 10 * 1000);
    rtsp_strm_report_cb(RTSP_STRM_REPORT_CLIENT_EXIT);

    sessp->handling_state = HANDLING_STATE_HANDLED;
    return;
}

/**
 * Allocate resource to prepare handling RTSP method.
 */
static void handling_method_init(struct rtsp_sess *sessp)
{
    struct rtsp_req *req = NULL;
    struct rtsp_resp *resp = NULL;

    if (sessp->handling_state == HANDLING_STATE_RETRY) {
        return;
    }

    /* Request. */
    req = mallocz(sizeof(*req));
    if (!req) {
        printf("Allocate memory for RTSP request failed!\r\n");
        goto err;
    }

    /* Response. */
    resp = mallocz(sizeof(*resp));
    if (!resp) {
        printf("Allocate memory for RTSP response failed!\r\n");
        goto err;
    }
    /* Initialize the response struct. */
    resp->resp_line.status_code = OK;

    sessp->req = req;
    sessp->resp = resp;
    sessp->handling_state = HANDLING_STATE_READY;
    return;

err:
    freez(req);
    freez(resp);
    sessp->handling_state = HANDLING_STATE_FAILED;
    return;
}

/**
 * Free resource using in handling RTSP method.
 */
static void handling_method_done(struct rtsp_sess *sessp)
{
    struct rtsp_req *req = sessp->req;
    struct rtsp_resp *resp = sessp->resp;

    if (sessp->handling_state == HANDLING_STATE_RETRY) {
        return;
    }

    /* Request. */
    freez(req->req_line.uri);
    freez(req->req_line.ver);
    freez(req);

    /* Response. */
    freez(resp->resp_line.ver);
    freez(resp->resp_hdr.public);
    freez(resp->resp_hdr.server);
    freez(resp);

    sessp->handling_state = HANDLING_STATE_INIT;
    return;
}

int handle_rtsp_req(struct rtsp_sess *sessp)
{
    handling_method_init(sessp);

    /* Parse request message. */
    parse_rtsp_req(sessp);

    /* Run the RTSP state machine. */
    run_rtsp_state_machine(sessp);

    /* Produce RTSP response message and put it into send buffer queue. */
    produce_rtsp_resp(sessp);

    handling_method_done(sessp);

    if (sessp->handling_state == HANDLING_STATE_RETRY) {
        /* Usually generate one frame per 40ms, when frame rate = 25f/s. Return unit is us*/
        return 1*1000*1000/rtsp_get_video_fps();
    }

    /* Task finished! */
    return -1;
}
