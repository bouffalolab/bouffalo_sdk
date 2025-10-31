#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "stdbool.h"

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    1

#define LWIP_NETIF_HOSTNAME     1
#define ETHARP_TRUST_IP_MAC     0
#define IP_REASSEMBLY           0
#define IP_FRAG                 0
#define ARP_QUEUEING            0
#define LWIP_NETIF_API          1

#define LWIP_MDNS_RESPONDER     1
#define LWIP_IGMP               1

#define LWIP_NUM_NETIF_CLIENT_DATA      1

#define LWIP_ALTCP                      1
#define LWIP_ALTCP_TLS                  1
/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  0

#define LWIP_TIMEVAL_PRIVATE    0

#define LWIP_HAVE_LOOPIF           1

/**
 * LWIP_TCPIP_CORE_LOCKING_INPUT: when LWIP_TCPIP_CORE_LOCKING is enabled,
 * this lets tcpip_input() grab the mutex for input packets as well,
 * instead of allocating a message and passing it to tcpip_thread.
 *
 * ATTENTION: this does not work when tcpip_input() is called from
 * interrupt context!
 */
#if defined(CFG_CHIP_BL808)
#define LWIP_TCPIP_CORE_LOCKING_INPUT   1
#elif defined(CFG_CHIP_BL606P)
#define LWIP_TCPIP_CORE_LOCKING_INPUT   1
#else
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0
#endif

/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#if defined(CFG_CHIP_BL808)
#define MEM_SIZE                (60*1024)
#elif defined(CFG_CHIP_BL606P)
#define MEM_SIZE                (60*1024)
#elif defined(CFG_SDIOWIFI)
#define MEM_SIZE                (8*1024)
#elif defined(CFG_NETBUS_WIFI_ENABLE)
#define MEM_SIZE                (12*1024)
#else
#define MEM_SIZE                (8*1024)
#endif


/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           26
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB        6
#endif

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        10
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 5
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#if defined(CFG_CHIP_BL808)
#define MEMP_NUM_TCP_SEG        100
#elif defined(CFG_CHIP_BL606P)
#define MEMP_NUM_TCP_SEG        100
#else
#define MEMP_NUM_TCP_SEG        32
#endif

/* NUM of sys_timeout pool*/
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 8 + 3)

#define MEMP_NUM_NETCONN    (MEMP_NUM_TCP_PCB + MEMP_NUM_UDP_PCB + MEMP_NUM_TCP_PCB_LISTEN)

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#if !defined PBUF_POOL_SIZE
#if defined(CFG_CHIP_BL808)
#define PBUF_POOL_SIZE          200
#elif defined(CFG_CHIP_BL606P)
#define PBUF_POOL_SIZE          200
#else

#if defined(CFG_ETHERNET_ENABLE)
#define PBUF_POOL_SIZE          12
#else
#define PBUF_POOL_SIZE          0
#endif
#endif /*CFG_ETHERNET_ENABLE*/
#endif

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#if defined(CFG_CHIP_BL808)||defined(CFG_ETHERNET_ENABLE)
#define PBUF_POOL_BUFSIZE       1600
#elif defined(CFG_CHIP_BL606P)
#define PBUF_POOL_BUFSIZE       1600
#else
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE       760
#endif
#endif /* CFG_ETHERNET_ENABLE */


/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define IP_DEFAULT_TTL          64

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
//#define TCP_MSS                 (1500 - 80)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
//#define TCP_MSS                 (800 - 40 - 80 + 8)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */


/* TCP sender buffer space (bytes). */
#if defined(CFG_CHIP_BL808)
#define TCP_SND_BUF             (12*TCP_MSS)
#elif defined(CFG_CHIP_BL606P)
#define TCP_SND_BUF             (12*TCP_MSS)
#else
#ifdef CFG_ETHERNET_ENABLE
#define TCP_SND_BUF             (11*TCP_MSS)
#else
#define TCP_SND_BUF             (3*TCP_MSS)
#endif
#endif

/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */

#define TCP_SND_QUEUELEN        ((2 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))

#define MEMP_NUM_TCPIP_MSG_INPKT        (32)

/**
 * TCP_SNDQUEUELOWAT: TCP writable bufs (pbuf count). This must be less
 * than TCP_SND_QUEUELEN. If the number of pbufs queued on a pcb drops below
 * this number, select returns writable (combined with TCP_SNDLOWAT).
 */
#define TCP_SNDQUEUELOWAT               ((TCP_SND_QUEUELEN)/2)

/* TCP receive window. */
#if defined(CFG_CHIP_BL808)
#define TCP_WND                 (30*TCP_MSS)
#elif defined(CFG_CHIP_BL606P)
#define TCP_WND                 (30*TCP_MSS)
#else
#ifdef CFG_ETHERNET_ENABLE
#define TCP_WND                 (6*TCP_MSS)
#else
#define TCP_WND                 (3*TCP_MSS)
#endif
#endif

#if defined(CFG_CHIP_BL808)
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)]
#endif

#if defined(CFG_CHIP_BL606P)
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)]
#endif

/**
 * TCP_WND_UPDATE_THRESHOLD: difference in window to trigger an
 * explicit window update
 */
#if defined(CFG_CHIP_BL808)
#define TCP_WND_UPDATE_THRESHOLD   LWIP_MIN((TCP_WND / 2), (TCP_MSS * 16))
#elif defined(CFG_CHIP_BL606P)
#define TCP_WND_UPDATE_THRESHOLD   LWIP_MIN((TCP_WND / 2), (TCP_MSS * 16))
#else
#define TCP_WND_UPDATE_THRESHOLD   LWIP_MIN((TCP_WND / 2), (TCP_MSS * 6))
#endif

/**
 * By default, TCP socket/netconn close waits 20 seconds max to send the FIN
 */
#define LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT 5000

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#define LWIP_SO_SNDTIMEO                1
/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1


/* ---------- ICMP options ---------- */
#define LWIP_ICMP                       1


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#define LWIP_DHCP               1


/* ---------- UDP options ---------- */
#define LWIP_UDP                1


/* ---------- Statistics options ---------- */
#define LWIP_STATS 1
#ifdef OPENTHREAD_BORDER_ROUTER
#define LWIP_ERRNO_STDINCLUDE 1
#else
#define LWIP_PROVIDE_ERRNO 1
#endif

/* ---------- link callback options ---------- */
/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK        1

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

#define LWIP_CHECKSUM_ON_COPY            1
#ifdef CFG_ETHERNET_ENABLE
#define LWIP_NETIF_TX_SINGLE_PBUF    0
#else
#define LWIP_NETIF_TX_SINGLE_PBUF    1
#endif /* CFG_ETHERNET_ENABLE */

#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               1
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
#define LWIP_CHKSUM_ALGORITHM 3

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     1

/*
   -----------------------------------
   ---------- DEBUG options ----------
   -----------------------------------
*/

//#define LWIP_DEBUG                      0

/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/

#define TCPIP_THREAD_NAME              "TCP/IP"
#ifdef CFG_ETHERNET_ENABLE
#define TCPIP_THREAD_STACKSIZE          1536
#elif defined(CFG_SDIOWIFI)
#define TCPIP_THREAD_STACKSIZE          512
#elif defined(CFG_NETBUS_WIFI_ENABLE)
#define TCPIP_THREAD_STACKSIZE          1536
#elif defined(CFG_TCPIP_STACKSIZE)
#define TCPIP_THREAD_STACKSIZE          CFG_TCPIP_STACKSIZE
#else
#define TCPIP_THREAD_STACKSIZE          4000
#endif /* CFG_ETHERNET_ENABLE */
#define TCPIP_MBOX_SIZE                 50
#define DEFAULT_UDP_RECVMBOX_SIZE       50
#define DEFAULT_TCP_RECVMBOX_SIZE       50
#define DEFAULT_ACCEPTMBOX_SIZE         50
#define DEFAULT_THREAD_STACKSIZE        500
#define TCPIP_THREAD_PRIO               (configMAX_PRIORITIES - 2)

#define LWIP_COMPAT_MUTEX               0
#if defined(CFG_CHIP_BL808)
#define LWIP_TCPIP_CORE_LOCKING         1
#elif defined(CFG_CHIP_BL606P)
#define LWIP_TCPIP_CORE_LOCKING         1
#else
#define LWIP_TCPIP_CORE_LOCKING         0
#endif
#define LWIP_SOCKET_SET_ERRNO           1
#define SO_REUSE                        1
#define LWIP_TCP_KEEPALIVE              1

/*Enable Status callback and link callback*/
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK        1
/*Enable dns*/
#define LWIP_DNS                        1
#define LWIP_DNS_SECURE                 0

#define MEMP_MEM_MALLOC                 0
#define LWIP_SUPPORT_CUSTOM_PBUF        1

#define PBUF_LINK_ENCAPSULATION_HLEN    48u

#define LWIP_RAW                        1

#ifdef MATTER_SUPPORT
#include "lwip/arch.h"
#include <lwip/mem.h>
#define LWIP_PBUF_CUSTOM_DATA           mem_size_t pool;
#endif

/*
   ---------------------------------
   ---------- MISC. options ----------
   ---------------------------------
*/

#if defined(__cplusplus)
extern "C" int bl_rand(void);
extern "C" int * __errno(void);
#else
extern int bl_rand(void);
extern int * __errno(void);
#endif

#define errno (*__errno())

/**
 * LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS==1: randomize the local port for the first
 * local TCP/UDP pcb (default==0). This can prevent creating predictable port
 * numbers after booting a device.
 */

#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 1
#define LWIP_RAND() ((u32_t)bl_rand())

#ifdef LWIP_NETCONN_DUPLEX_SWITCH
#define LWIP_NETCONN_FULLDUPLEX 1
#define LWIP_NETCONN_SEM_PER_THREAD     1

void *sys_thread_sem_get(void);
void sys_thread_sem_init(void);
void sys_thread_sem_deinit(void);
#define LWIP_NETCONN_THREAD_SEM_GET() sys_thread_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_thread_sem_init()
#define LWIP_NETCONN_THREAD_SEM_FREE() sys_thread_sem_deinit()
#endif

#endif /* __LWIPOPTS_H__ */
