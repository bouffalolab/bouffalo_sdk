#include <string.h>
#include "list.h"
#include "util.h"
#include "log.h"
#include "rtsp_parser.h"
#include "rtsp_sess.h"
#include "rtsp_srv.h"


#define MAX_URI_SZ 256

static const char *get_next_word(const char *str)
{
    const char *word = NULL;
    if (!str) {
        return NULL;
    }

    while (*str) {
        if (*str == ' ' && *(str + 1) != ' ') {
            word = str + 1;
            break;
        } else if (*str == '\r' || *str == '\n') {
            break;
        }
        str++;
    }
    return word;
}

/**
 * Return the next pointer.
 *
 * Refer to RFC2326 section 4:
 * Lines are terminated by CRLF, but we should be prepared to
 * also interpret CR and LF by themselves as line terminators.
 */
static const char *get_next_line(const char *str)
{
    const char *line = NULL;
    if (!str) {
        return NULL;
    }

    while (*str) {
        if ((*str == '\r' && *(str + 1) == '\n' && *(str + 2) == '\r' && *(str + 3) == '\n') ||
            (*str == '\r' && *(str + 1) == '\r') ||
            (*str == '\n' && *(str + 1) == '\n')) {
            break;
        } else if (*str == '\r' && *(str + 1) == '\n') {
            line = str + 2;
            break;
        } else if (*str == '\r' || *str == '\n') {
            line = str + 1;
            break;
        }
        str++;
    }
    return line;
}

/**
 * Count until a white space appears.
 */
static int get_word_sz(const char *word)
{
    int sz = 0;

    /* Count util hitting ' ' || '\r' || '\n'. */
    for (; *word && *word != ' ' && *word != '\r' && *word != '\n'; word++, sz++);
    return sz;
}

static void parse_rtsp_uri(struct rtsp_sess *sessp, struct rtsp_req *req, struct rtsp_resp *resp)
{
    struct strm_info *strmp = &sessp->strm.strm_info;
    const char *uri = req->req_line.uri;
    char file_name[MAX_URI_SZ] = {0};
    int chn_no = 0;
    int chn_type = 0;
    int media_type = 0;

    /* URI check. */
    if (strncasecmp(uri, "rtsp://", strlen("rtsp://"))) {
        printf("Illegal RTSP uri: [%s]\n", req->req_line.uri);
        resp->resp_line.status_code = NOT_FOUND;
        return;
    }
    if (strlen(uri) >= MAX_URI_SZ) {
        resp->resp_line.status_code = REQUEST_URI_TOO_LARGE;
        return;
    }

    /* Lookup stream resource */
    printf("uri: %s\r\n", uri);
    if (sscanf(uri, "rtsp://%*[^/]/av%d_%d", &chn_no, &chn_type) == 2) {
        printf(INFO "chn_no = %d, chn_type = %d\r\n", chn_no, chn_type);
        /* Sanity check. */
        if (chn_no < 0 || chn_no >= MAX_CHN_NUM ||
            chn_type < 0 || chn_type > 1) {
            printf(WARNING "Invalid range.\n");
            resp->resp_line.status_code = INVALID_RANGE;
            return;
        }
        strmp->chn_no = chn_no;
        strmp->chn_type = chn_type ? CHN_TYPE_MAIN : CHN_TYPE_MINOR;
    } else if (sscanf(uri, "rtsp://%*[^/]/%*[^/]/track%d", &media_type) == 1) {
        printf("=====uri: %s, media: %d\r\n", uri, media_type);
        if (media_type < 0 || media_type > 1) {
            printf(WARNING "Invalid range.\n");
            resp->resp_line.status_code = INVALID_RANGE;
            return;
        }
        req->media = media_type ? MEDIA_TYPE_AUDIO : MEDIA_TYPE_VIDEO;
    } else if (sscanf(uri, "rtsp://%*[^/]/%s", file_name) == 1) {
        printf(INFO "File resource.\r\n");
#if 0
        /* Check whether the request file exist. */
        if (access(file_name, F_OK) < 0) {
            printf(ERR "Access file error");
            resp->resp_line.status_code = INVALID_RANGE;
            return;
        }
        strmp->file_name = strndup(file_name, strlen(file_name));
#endif
    } else if (sscanf(uri, "rtsp://%*[^/]") == 0) {
        printf(INFO "Default resource.\r\n");
        /* Default resource. */
        strmp->chn_no = 0;
        strmp->chn_type = 0;
    } else {
        printf(WARNING "Invalid range.\r\n");
        resp->resp_line.status_code = INVALID_RANGE;
        return;
    }

    return;
}

/*
 * Parse RTSP request line.
 */
static void parse_req_line(struct rtsp_sess *sessp, const char *line)
{
    struct rtsp_req *req = sessp->req;
    struct rtsp_resp *resp = sessp->resp;
    const char *ptr = line;

    /* Get method ID. */
    req->req_line.method = get_rtsp_method(ptr);
    if (req->req_line.method == -1 || req->req_line.method >= RTSP_METHOD_MAX) {
        printf("RTSP method is not implemented!\n");
        resp->resp_line.status_code = NOT_IMPLEMENTED;
        return;
    }
    
    /* Get and check RTSP uri. */
    ptr = get_next_word(ptr);
    if (!ptr) {
        sessp->handling_state = HANDLING_STATE_FAILED;
        printf("get_next_word failed!\n");
        return;
    }
    req->req_line.uri = strndup(ptr, get_word_sz(ptr));
    parse_rtsp_uri(sessp, req, resp);

    /* Check RTSP version. */
    ptr = get_next_word(ptr);
    if (!ptr) {
        sessp->handling_state = HANDLING_STATE_FAILED;
        printf("get_next_word failed!\n");
        return;
    }
    req->req_line.ver = strndup(ptr, get_word_sz(ptr));
    if (strcmp(req->req_line.ver, RTSP_VERSION)) {
        printf("Unsupported RTSP version: [%s]!\n", req->req_line.ver);
        resp->resp_line.status_code = VERSION_NOT_SUPPORTED;
        return;
    }

    return;
}

static int parse_hdr_transport(struct rtsp_sess *sessp, const char *line)
{
    const char *ptr = line;
    struct rtsp_req *req = sessp->req;
    
    if (strncmp(ptr, "Transport: ", strlen("Transport: "))) {
        return -1;              /* Not a transport field! */
    }
    ptr += strlen("Transport: ");

    while (*ptr) {
        if (!strncmp(ptr, "RTP/AVP/TCP", strlen("RTP/AVP/TCP"))) {
            sessp->intlvd_mode = 1;
        } else if (!strncmp(ptr, "RTP/AVP/UDP", strlen("RTP/AVP/UDP")) ||
                   !strncmp(ptr, "RTP/AVP", strlen("RTP/AVP"))) {
            sessp->intlvd_mode = 0;
        } else if (!strncmp(ptr, "unicast", strlen("unicast"))) {
        } else if (!strncmp(ptr, "mode", strlen("mode"))) {
        } else if (!strncmp(ptr, "client_port", strlen("client_port"))) {
            if (sscanf(ptr, "client_port=%hu-%hu",
                       &req->req_hdr.transport.rtp_cli_port,
                       &req->req_hdr.transport.rtcp_cli_port) != 2) {
                return -1;
            }
        } else if (!strncmp(ptr, "interleaved", strlen("interleaved"))) {
            if (sscanf(ptr, "interleaved=%d-%d",
                       &req->req_hdr.transport.rtp_chn,
                       &req->req_hdr.transport.rtcp_chn) != 2) {
                return -1;
            }
        }

        ptr = strpbrk(ptr, ";\r\n");
        while (*ptr == ';') {   /* Skip over separating ';' chars. */
            ptr++;
        }
        if (*ptr == '\0' || *ptr == '\r' || *ptr == '\n') {
            break;
        }
    }
    return 0;
}


 unsigned long long match_sscanf64(const char *line)
{
    char string_buf[1024];
    unsigned long long sess_id_1 = 0, sess_id_2 = 0, sess_id_3 = 0, sess_id;    
	        
    memcpy(string_buf, line + 9, 7);
    sscanf(string_buf, "%u", &sess_id_1);

    memcpy(string_buf, line + 16, 7);
    sscanf(string_buf, "%u", &sess_id_2);

    memset(string_buf, '\0', 1024);
    memcpy(string_buf, line + 23, 6);
    sscanf(string_buf, "%u", &sess_id_3);
    
    if (sess_id_3 > 99999 || string_buf[0] == '0' && sess_id_3 > 9999) {
        sess_id = ((unsigned long long)sess_id_1 * 10000000 + (unsigned long long)sess_id_2) * 1000000 + sess_id_3;
    } else if (sess_id_3 > 9999 || string_buf[0] == '0' && sess_id_3 > 999) {
        sess_id = ((unsigned long long)sess_id_1 * 10000000 + (unsigned long long)sess_id_2) * 100000 + sess_id_3;
    } else if(sess_id_3 > 999 || string_buf[0] == '0' && sess_id_3 > 99) {
        sess_id = ((unsigned long long)sess_id_1 * 10000000 + (unsigned long long)sess_id_2) * 10000 + sess_id_3;
    } else {
        sess_id = ((unsigned long long)sess_id_1 * 10000000 + (unsigned long long)sess_id_2) * 1000 + sess_id_3;
    }

    return sess_id;
}

/**
 * Parse RTSP request header fields.
 */

static void parse_req_hdrs(struct rtsp_sess *sessp, const char *line)
{
    struct rtsp_req *req = sessp->req;
    struct rtsp_resp *resp = sessp->resp;
    int cseq_found = 0;
  
    do {
        if (!strncmp(line, "CSeq", strlen("CSeq"))) {
            if (sscanf(line, "CSeq: %d", &req->req_hdr.cseq) != 1) {
                resp->resp_line.status_code = INTERNAL_SERVER_ERROR;
                printf(ERR "Parse RTSP header <CSeq> failed!\n");
                break;
            }
            cseq_found = 1;
            resp->resp_hdr.cseq = req->req_hdr.cseq;
        } else if (!strncmp(line, "Session", strlen("Session"))) {
            if (sscanf(line, "Session: %llu", &req->req_hdr.sess_id) == 1) {
                req->req_hdr.sess_id = match_sscanf64(line);
                if (rtsp_sess_id_exist(req->req_hdr.sess_id)) {
                    resp->resp_hdr.sess_id = req->req_hdr.sess_id;
                } else {
                    printf(WARNING "Session not found!\r\n");
                    resp->resp_hdr.sess_id = 0;
                    resp->resp_line.status_code = SESSION_NOT_FOUND;
                    break;
                }
            } else {
                resp->resp_line.status_code = INTERNAL_SERVER_ERROR;
                printf(ERR "Parse RTSP header <Session> failed!\r\n");
                break;
            }
        } else if (!strncmp(line, "User-Agent", strlen("User-Agent"))) {
        } else if (!strncmp(line, "Transport", strlen("Transport"))) {
            if (parse_hdr_transport(sessp, line) < 0) {
                resp->resp_line.status_code = INTERNAL_SERVER_ERROR;
                printf(ERR "Parse RTSP header <Transport> failed!\r\n");
                break;
            }
        }

        line = get_next_line(line);
    } while (line);

    if (!cseq_found) {
        resp->resp_line.status_code = INTERNAL_SERVER_ERROR;
        printf(WARNING "CSeq not found!\r\n");
    }
    return;
}

/**
 * We assume that the request message is complete,
 * and do not handle the un-complete request message.
 */
void parse_rtsp_req(struct rtsp_sess *sessp)
{
    const char *line = sessp->recv_buf;
    struct rtsp_req *req = sessp->req;
    struct rtsp_resp *resp = sessp->resp;

    if (sessp->handling_state != HANDLING_STATE_READY) {
        return;
    }

    /* Prepare RTSP response message. */
    resp->resp_line.ver = strndup(RTSP_VERSION, strlen(RTSP_VERSION));
    resp->resp_hdr.server = strndup(RTSP_SERVER, strlen(RTSP_SERVER));
    resp->resp_hdr.cseq = req->req_hdr.cseq;
    resp->resp_hdr.sess_id = req->req_hdr.sess_id;

    print_req_msg(sessp->recv_buf);
    printf("C->S %s\r\n", sessp->recv_buf);

    /* Parse RTSP request line. */
    parse_req_line(sessp, line);

    /* Parse RTSP request header fields. */
    line = get_next_line(line);
    if (line) {
        parse_req_hdrs(sessp, line);
    }

    if (sessp->handling_state != HANDLING_STATE_FAILED) {
        sessp->handling_state = (resp->resp_line.status_code == OK) ?
            HANDLING_STATE_PARSED : HANDLING_STATE_ERROR;
    }
    return;
}
