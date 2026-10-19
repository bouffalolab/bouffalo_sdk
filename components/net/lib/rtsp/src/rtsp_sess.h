#ifndef __RTSP_SESS_H__
#define __RTSP_SESS_H__

#include <lwip/sockets.h>
#include "util.h"
#include "list.h"
#include "rtsp_srv.h"
#include "librtspsrv.h"
#include "rtp.h"


/* RTSP method handling state. */
enum handling_state {
    HANDLING_STATE_INIT,        /* When created. */
    HANDLING_STATE_READY,       /* It's ready to parse the request. */
    HANDLING_STATE_PARSED,      /* Request has been parsed. */
    HANDLING_STATE_HANDLED,     /* RTSP method has been performed. */
    HANDLING_STATE_RETRY,       /* Retry to handle RTSP method. */
    HANDLING_STATE_DONE,        /* Everything done, RTSP response has been sent out. */
    HANDLING_STATE_ERROR,       /* RTSP Error occured while handling. */
    HANDLING_STATE_FAILED,      /* Handling RTSP method failed, can't continue... */
};

/* Transport in request or response header fields. */
struct transport {
    char rtp_chn;
    char rtcp_chn;
    unsigned short rtp_cli_port;
    unsigned short rtcp_cli_port;
    unsigned short rtp_srv_port;
    unsigned short rtcp_srv_port;
} transport;

/* RTSP request. */
struct rtsp_req {
    struct req_line {           /* Request line. */
        enum rtsp_method method;   /* RTSP method ID. */
        char *uri;              /* RTSP uri. */
        char *ver;              /* RTSP version. */
    } req_line;
    struct req_hdr {                /* Response header fields. */
        unsigned long long sess_id; /* RTSP session ID. */
        unsigned int cseq;          /* RTSP sequence number. */
        struct transport transport;
    } req_hdr;
    enum media_type media;
};

/* RTSP response. */
struct rtsp_resp {
    struct resp_line {             /* Status line. */
        char *ver;                 /* RTSP version. */
        enum status_code status_code; /* RTSP status code. */
    } resp_line;
    struct resp_hdr {               /* Response header fields. */
        unsigned long long sess_id; /* Session. */
        unsigned int cseq;          /* CSeq. */
        char *server;               /* Server. */
        char *public;               /* Public. */
        struct transport transport;
    } resp_hdr;
    struct resp_body {
        char *sdp_info;             /* List hold all sdp info. */
    } resp_body;
};

/* RTSP session stream resource: video or audio stream. */
struct strm {
    struct strm_info strm_info; /* Stream information. */
    struct frm_info frm_info;   /* Frame information. */
    get_frm_t get_frm;          /* Callback function to get one frame. */
};

/* RTP information structure.. */
struct rtp_rtcp {
    struct rtp_hdr rtp_hdr;
    union {
        struct tcp {        /* Used in interleaved mode. */
            char rtp_chn;
            char rtcp_chn;
        } tcp;
        struct udp {
            int rtp_sd;              /* Server RTP socket descriptor. */
            int rtcp_sd;             /* Server RTCP socket descriptor. */
            struct sockaddr rtp_sa;  /* Client RTP socket address. */
            struct sockaddr rtcp_sa; /* Client RTCP socket address. */
        } udp;
    };
};

/* RTSP session struct. */
struct rtsp_sess {
    struct list_head entry;             /* Entry of session list. */
    unsigned long long sess_id;         /* Session ID. */
    unsigned int cseq;                  /* RTSP sequence number. */
    enum rtsp_state rtsp_state;         /* Current session state. */
    enum handling_state handling_state; /* RTSP method handling state. */
    int rtsp_sd;                        /* RTSP socket discriptor. */
    int intlvd_mode;                    /* Interleaved mode: [0, 1]. */
    struct list_head send_queue;        /* A queue keeps send buffers. */
    char *recv_buf;                     /* Buffer for receiving client data. */
    struct rtsp_req *req;               /* RTSP request. */
    struct rtsp_resp *resp;             /* RTSP response. */
    struct strm strm;                   /* Stream resource */
    struct rtp_rtcp rtp_rtcp[2];        /* RTP & RTCP information of video[0] & audio[1]. */
};


void init_rtsp_sess_list(void);
void deinit_rtsp_sess_list(void);

int create_rtsp_sess(int rtsp_sd);
int destroy_rtsp_sess(struct rtsp_sess *sessp);

int get_rtsp_method(const char *str);

struct send_buf *create_send_buf(enum data_type type, unsigned int sz);
void destroy_send_buf(struct send_buf *sendp);
void add_send_buf(struct rtsp_sess *sessp, struct send_buf *sendp);
void del_send_buf(struct send_buf *sendp);
void produce_rtsp_resp(struct rtsp_sess *sessp);
int consume_send_buf(struct rtsp_sess *sessp, enum data_type type);

void run_rtsp_state_machine(struct rtsp_sess *sessp);

void destroy_rtsp_sess_contain_sd(int sd);
int rtsp_sess_id_exist(unsigned long long sess_id);
int rtsp_sess_exist(struct rtsp_sess *sessp);
int check_send_queue(void);


#endif /* __RTSP_SESS_H__ */
