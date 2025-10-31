#ifndef LWIP_HDR_LWIPOPTS_H__
#define LWIP_HDR_LWIPOPTS_H__

#define LWIP_NETIF_API     1
#define LWIP_DEBUG         1
#define LWIP_STATS_DISPLAY 1
#define SOCKETS_DEBUG      LWIP_DBG_OFF
#ifdef BL616_DHCP_DEBUG
#define DHCP_DEBUG LWIP_DBG_ON
#else
#define DHCP_DEBUG LWIP_DBG_OFF
#endif
#define ICMP_DEBUG                    LWIP_DBG_OFF
#define ETHARP_DEBUG                  LWIP_DBG_OFF
#define LWIP_MULTICAST_PING           1
#define LWIP_BROADCAST_PING           1

#define LWIP_TCPIP_CORE_LOCKING       1

#define LWIP_ASSERT_CORE_LOCKED()
#define LWIP_NOASSERT

#define DNS_MAX_SERVERS               3
#define LWIP_NETIF_HOSTNAME           1
#define TCPIP_MBOX_SIZE               64
#define TCPIP_THREAD_STACKSIZE        512
#define TCPIP_THREAD_PRIO             28

#define DEFAULT_THREAD_STACKSIZE      1024
#define DEFAULT_THREAD_PRIO           1
#define DEFAULT_RAW_RECVMBOX_SIZE     32
#define DEFAULT_UDP_RECVMBOX_SIZE     64
#define DEFAULT_TCP_RECVMBOX_SIZE     64
#define DEFAULT_ACCEPTMBOX_SIZE       32

#define SNTP_SERVER_DNS               1
#define LWIP_NETIF_LOOPBACK           1
#define LWIP_HAVE_LOOPIF              1
#define LWIP_LOOPBACK_MAX_PBUFS       0
#define LWIP_DHCP_DOES_ACD_CHECK      0

//#define LWIP_ALTCP_TLS_MBEDTLS        1
//#define LWIP_ALTCP                    1
//#define LWIP_ALTCP_TLS                1
#define LWIP_CHKSUM_ALGORITHM         3
#define LWIP_TCPIP_CORE_LOCKING_INPUT 1

#define PBUF_LINK_ENCAPSULATION_HLEN  388

#define MEMP_NUM_NETBUF               28
#define MEMP_NUM_ALTCP_PCB            2
#define MEMP_NUM_UDP_PCB              6
#define MEMP_NUM_TCP_PCB              6
#define MEMP_NUM_TCP_PCB_LISTEN       1
#define MEMP_NUM_NETCONN              (MEMP_NUM_TCP_PCB + MEMP_NUM_TCP_PCB_LISTEN)

#define MAC_TXQ_DEPTH                 8
#define MAC_RXQ_DEPTH                 12

#define IP_REASS_MAX_PBUFS            (2 * MAC_RXQ_DEPTH - 2)
#define MEMP_NUM_REASSDATA            LWIP_MIN((IP_REASS_MAX_PBUFS), 5)

//#define TCP_OOSEQ_MAX_PBUFS           MAC_RXQ_DEPTH

#define TCP_MSS                       (1500 - 40)
#define TCP_WND                       ((2 * MAC_RXQ_DEPTH) * TCP_MSS)
#define TCP_SND_BUF                   (4 * MAC_TXQ_DEPTH * TCP_MSS)

#define TCP_QUEUE_OOSEQ               1
#define MEMP_NUM_TCP_SEG              ((4 * TCP_SND_BUF) / TCP_MSS)
#define MEMP_NUM_PBUF                 (TCP_SND_BUF / TCP_MSS)
#define PBUF_POOL_SIZE                0
#define LWIP_WND_SCALE                1
#define TCP_RCV_SCALE                 2
#define TCP_SNDLOWAT                  LWIP_MIN(LWIP_MAX(((TCP_SND_BUF) / 4), (2 * TCP_MSS) + 1), (TCP_SND_BUF)-1)

#define MEM_MIN_TCP                   (2300 + MEMP_NUM_PBUF * (100 + PBUF_LINK_ENCAPSULATION_HLEN))
#define MEM_MIN                       MEM_MIN_TCP
#define MEM_ALIGNMENT                 4

#define MEMP_NUM_SYS_TIMEOUT          (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 8 + 3)

#ifdef LWIP_HEAP_SIZE
#define MEM_SIZE LWIP_HEAP_SIZE
#else
#if MEM_MIN > 8192
#define MEM_SIZE (MEM_MIN)
#else
#define MEM_SIZE 8192
#endif
#endif

#if CONFIG_IPV6
#define LWIP_IPV6 1
#endif 

#define LWIP_HOOK_FILENAME        "lwiphooks.h"

#define LWIP_RAW                  1
#define LWIP_MULTICAST_TX_OPTIONS 1

#define LWIP_TIMEVAL_PRIVATE      0 // use sys/time.h for struct timeval

// #define LWIP_PROVIDE_ERRNO        1
#define LWIP_ERRNO_STDINCLUDE     1
#define LWIP_SOCKET_SET_ERRNO     1

#define LWIP_DHCP                 1
#define LWIP_DNS                  1
#define LWIP_SO_RCVTIMEO          1
#define LWIP_SO_SNDTIMEO          1
#define SO_REUSE                  1
#define LWIP_TCP_KEEPALIVE        1

#ifdef CONFIG_LWIP_LP
#define TCP_TIMER_PRECISE_NEEDED        1
#define DHCP_TIMER_PRECISE_NEEDED       1
#define ARP_TIMER_PRECISE_NEEDED        1
#define IP4_FRAG_TIMER_PRECISE_NEEDED   1
#define DNS_TIMER_PRECISE_NEEDED        1

#define LWIP_IGMP                       0
#else
#define LWIP_IGMP                       1
#endif

extern int *__errno(void);
#define errno                         (*__errno())
#define LWIP_NETIF_STATUS_CALLBACK    1
#define LWIP_NETIF_API                1

#define ETHARP_SUPPORT_STATIC_ENTRIES 1

#define LWIP_SUPPORT_CUSTOM_PBUF      1
#define LWIP_NETIF_TX_SINGLE_PBUF 1
#define LWIP_RAND()                                      ((u32_t)random())

#endif /* LWIP_HDR_LWIPOPTS_H__ */
