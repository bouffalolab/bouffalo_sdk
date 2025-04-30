#ifndef __RTSP_SRV_H__
#define __RTSP_SRV_H__

#include "list.h"

#define MAX_FD_NUM      100000
#define BACKLOG         5       /* Maximum length of pending connections
                                 * for listening socket. */

#define RTSP_VERSION    "RTSP/1.0"
#define RTSP_SERVER     "BL-RTSP-Server"

#define EPOLL_MAX_EVENTS 128

/**
 * Epoll events of different socket descriptors.
 * Here doesn't include EPOLLOUT, we will add EPOLLOUT
 * when there's any data in send buffer queue.
 */
#define TIMER_SD_DFL_EV     (POLLIN | POLLOUT)
#define RTSP_LSN_SD_DFL_EV  (POLLIN | POLLOUT)
#define RTSP_SD_DFL_EV      (POLLIN | POLLOUT)
#define RTP_SD_DFL_EV       (POLLOUT)
#define RTCP_SD_DFL_EV      (POLLIN | POLLOUT)

/*
 * Max size of RTSP receiving buffer and response message,
 * these should be enough.
 */
#define RECV_BUF_SZ (100 * 1024)
#define RTSP_RESP_SZ (100 * 1024)


/* RTSP methods. */
enum rtsp_method {
    RTSP_METHOD_OPTIONS,
    RTSP_METHOD_DESCRIBE,
    RTSP_METHOD_SETUP,
    RTSP_METHOD_PLAY,
    RTSP_METHOD_PAUSE,
    RTSP_METHOD_GET_PARAMETER,
    RTSP_METHOD_SET_PARAMETER,
    RTSP_METHOD_TEARDOWN,
    RTSP_METHOD_MAX,
};

/*
 * The RTSP server state machine is defined
 * in RFC2326 Appendix A, Subsection 2.
 */
enum rtsp_state {
    RTSP_STATE_INIT,
    RTSP_STATE_READY,
    RTSP_STATE_PLAYING,
};

/*
 * RTSP server struct.
 */
struct rtsp_srv {
    void* rtsp_srv_tid;     /* Thread ID of RTSP server thread. */
    int rtsp_lsn_sd;            /* Socket for listening RTSP connections. */
    int http_lsn_sd;            /* Socket for listening HTTP connections. */
    unsigned short rtsp_port;
    unsigned short http_port;
    char thrd_running;          /* the RTSP server thread is running or not. */
    struct list_head rtsp_sess_list; /* A list holds all RTSP sessions. */
    struct list_head sd_handler_list; /* A queue keeps socket descripotrs' handler. */
    struct list_head delay_task_queue; /* A queue keeps delay tasks todo in the future. */
};

enum data_type {
    DATA_TYPE_RTSP_RESP     = 0x01, /* RTSP response. */
    DATA_TYPE_RTP_V_PKT     = 0x02, /* RTP packet of video. */
    DATA_TYPE_RTP_A_PKT     = 0x04, /* RTP packet of audio. */
    DATA_TYPE_RTCP_V_PKT    = 0x08, /* RTCP packet of video. */
    DATA_TYPE_RTCP_A_PKT    = 0x10, /* RTCP packet of audio. */
};

/*
 * Each RTSP session contains one buffer queue.
 * And each buffer queue links the nodes below.
 */
struct send_buf {
    struct list_head entry;          /* Entry of send queue. */
    enum data_type type;             /* RTSP, RTCP, RTP(I/P/A frame). */
    unsigned int sz;                 /* Buffer size. */
    char *buf;                       /* Buffer for storing raw data. */
};

extern struct rtsp_srv rtsp_srv;

int start_rtsp_srv(void);
int stop_rtsp_srv(void);

int monitor_sd_event(int fd, unsigned int ev);
int update_sd_event(int fd, unsigned int ev);


#endif /* __RTSP_SRV_H__ */
