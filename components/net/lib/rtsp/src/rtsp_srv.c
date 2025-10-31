#include "FreeRTOS.h"
#include <task.h>
#include <lwip/sockets.h>
#include <errno.h>
#include "list.h"
#include "log.h"
#include "librtspsrv.h"
#include "rtsp_srv.h"
#include "util.h"
#include "rtsp_parser.h"
#include "rtsp_sess.h"
#include "rtp.h"
#include "sd_handler.h"
#include "delay_task.h"

#define MAX_POLL_FDS              (32)
#define RTSP_SRV_TASK_PRI         (0) // lower than venc get stream task
#define RTSP_SRV_TASK_STACK_SIZE  (4096)

struct rtsp_srv rtsp_srv;
static struct pollfd g_poll_table[MAX_POLL_FDS];
static int g_poll_table_cnt = 0;
static TaskHandle_t gRTSPTaskHdl;
/**
 * This will add specified @ev to the epoll events.
 * 
 * @ev: Generally, we provide EPOLLIN, EPOLLOUT, EPOLLET for the caller.
 */
int monitor_sd_event(int fd, unsigned int ev)
{
    if (g_poll_table_cnt +1 >= MAX_POLL_FDS) {
        printf("Poll fds exceeds\r\n");
        return -1;
    }
    g_poll_table[g_poll_table_cnt].fd = fd;
    g_poll_table[g_poll_table_cnt].events = ev;
    g_poll_table_cnt++;
    return 0;
}

int unmonitor_sd_event(int fd)
{
    int i;
    for (i = 0; i < g_poll_table_cnt; i++) {
        if (g_poll_table[i].fd == fd) {
            if (i < g_poll_table_cnt - 1) {
                g_poll_table[i].fd = g_poll_table[g_poll_table_cnt - 1].fd;
                g_poll_table[i].events = g_poll_table[g_poll_table_cnt - 1].events;
            }
            g_poll_table_cnt--;
            break;
        }
    }
    return 0;
}

void cleanup_sd_event(void)
{
    int i;
    for (i = 0; i < g_poll_table_cnt; i++) {
        if (g_poll_table[i].fd >= 0)
            close(g_poll_table[i].fd);
    }
    g_poll_table_cnt = 0;
}


/**
 * This will modify specified @ev to the epoll events.
 * 
 * @ev: Generally, we provide EPOLLIN, EPOLLOUT, EPOLLET for the caller.
 */
int update_sd_event(int fd, unsigned int ev)
{
    int i;
    for (i = 0; i < g_poll_table_cnt; i++) {
        if (g_poll_table[i].fd == fd)
            g_poll_table[i].events = ev;
    }

    return 0;
}
#define RTSP_SRV_KEEP_ALIVE (5*1000)

static void rtsp_srv_thrd(void *arg)
{
    struct rtsp_srv *srvp = (struct rtsp_srv *)arg;
    int i = 0;
    int rtsp_lsn_sd = -1;           /* RTSP listen socket. */
    int nfds = 0;
    int timeout = 0;
    int min_delay_time = 0;
    TickType_t tick_start = xTaskGetTickCount();

    entering_thread();

    while (1) {
        rtsp_lsn_sd = -1;           /* RTSP listen socket. */
        nfds = 0;
        timeout = 0;
        min_delay_time = 0;

        srvp->rtsp_lsn_sd = -1;
        srvp->http_lsn_sd = -1;
        srvp->rtsp_port = DFL_RTSP_PORT;
        srvp->http_port = DFL_HTTP_PORT;
        srvp->thrd_running = 1;

        init_rtsp_sess_list();
        init_sd_handler_list();
        init_delay_task_queue();

        /* Create listen socket. */
        rtsp_lsn_sd = create_and_bind(srvp->rtsp_port);
        if (rtsp_lsn_sd < 0) {
            printf("rtsp_srv_thrd bind fail\r\n");
            goto rtn;
        }

        if (listen(rtsp_lsn_sd, BACKLOG) < 0) {
            printf("Start listening connections error\r\n");
            goto rtn;
        }
        srvp->rtsp_lsn_sd = rtsp_lsn_sd;
        if (monitor_sd_event(srvp->rtsp_lsn_sd, RTSP_LSN_SD_DFL_EV) < 0) {
            printf("monitor sd event fail\r\n");
            goto rtn;
        }

        /* Register sd handler. */
        if (register_sd_handler(rtsp_lsn_sd, handle_lsn_sd, srvp) < 0) {
            printf("register rtsp_lsn_sd handler failed!\r\n");
            goto rtn;
        }

        /* Accept connections, then create a new thread for it. */
        printf("\r\nStart listening RTSP connections ...\r\n");
        while (srvp->thrd_running) {
#if 0
            if (xTaskGetTickCount() - tick_start > RTSP_SRV_KEEP_ALIVE) {
                tick_start = xTaskGetTickCount();
                printf("[%ld]xPortGetFreeHeapSize = %zd\r\n", tick_start, xPortGetFreeHeapSize());
            }
#endif
            /* Check send buffer queue in each RTSP session,
             * then update the event of socket descriptors. */
            if (check_send_queue() < 0) {
                continue;
            }
            /* Reset the value of epoll_wait timeout. */
            min_delay_time = get_min_delay_time();
            timeout = (min_delay_time < 0) ? -1 : (min_delay_time / THOUSAND);

            /* Wait event notifications. */
            do {
                nfds = poll(g_poll_table, g_poll_table_cnt, timeout);
            } while (nfds == 0 && errno == EINTR);
            if (nfds < 0) {
                printf("epoll_wait for listen socket error %d, %d\r\n", nfds, errno);
                goto rtn;
            }

            /* Handle the readable sockets. */
            for (i = 0; i < g_poll_table_cnt; i++) {
                if (g_poll_table[i].revents == 0)
                    continue;

                if (g_poll_table[i].revents == POLLERR) {
                    /* Error occured on this descriptor. */
                    printf("epoll_wait error occured on this fd[%d]\n", g_poll_table[i].fd);
                    destroy_rtsp_sess_contain_sd(g_poll_table[i].fd);
                    continue;
                }

                do_sd_handler(g_poll_table[i].fd, g_poll_table[i].revents);

            }
            /* Process the delayed task whose time is up. */
            do_delay_task();
        }

    rtn:
        deregister_sd_handler(srvp->rtsp_lsn_sd);
        close(srvp->rtsp_lsn_sd);
        close(srvp->http_lsn_sd);
        srvp->rtsp_lsn_sd = -1;
        srvp->http_lsn_sd = -1;
        cleanup_sd_event();
        rtsp_strm_report_cb(RTSP_STRM_REPORT_CLIENT_EXIT);
    }
    leaving_thread();
    vTaskDelete(NULL);
}

int start_rtsp_srv(void)
{
#if 0
    int ret;

    if ((ret = pthread_create(&rtsp_srv.rtsp_srv_tid, NULL,
                              rtsp_srv_thrd, &rtsp_srv)) != 0) {
        printf("Create thread rtsp_srv_thrd error: %s\n", strerror(ret));
        return -1;
    }
#else
    xTaskCreate(rtsp_srv_thrd, "RTSP Server", RTSP_SRV_TASK_STACK_SIZE, &rtsp_srv, RTSP_SRV_TASK_PRI, &gRTSPTaskHdl);
#endif

    return 0;
}

int stop_rtsp_srv(void)
{
    rtsp_srv.thrd_running = 0;
    deinit_delay_task_queue();
    deinit_sd_handler_list();
    deinit_rtsp_sess_list();

    return 0;
}
