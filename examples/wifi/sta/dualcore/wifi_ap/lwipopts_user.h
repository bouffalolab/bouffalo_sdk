#ifndef LWIP_HDR_LWIPOPTS_H__
#define LWIP_HDR_LWIPOPTS_H__

/* Use the platform fd_set for application socket descriptors. */
#include <sys/select.h>

/* Socket types/options for AP; socket traffic is handled by NP. */
#define MEM_ALIGNMENT             4
#define LWIP_TIMEVAL_PRIVATE      0
#define LWIP_ERRNO_STDINCLUDE      1
#define LWIP_SOCKET_SET_ERRNO      1
#define LWIP_DNS                  1
#define LWIP_RAND()               ((u32_t)random())
#define LWIP_SO_RCVTIMEO           1
#define LWIP_SO_SNDTIMEO           1
#define SO_REUSE                  1
#define LWIP_TCP_KEEPALIVE        1

/* Keep iperf's reported TCP window consistent with NP. */
#define TCP_MSS                   (1500 - 40)
#define TCP_WND                   (24 * TCP_MSS)
#define PBUF_POOL_SIZE            0

#endif /* LWIP_HDR_LWIPOPTS_H__ */
