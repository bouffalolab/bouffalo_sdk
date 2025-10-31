/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt
 *
 */
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
#define IP6_DEBUG                     LWIP_DBG_OFF
#define ICMP_DEBUG                    LWIP_DBG_OFF
#define UDP_DEBUG                     LWIP_DBG_OFF
#define ETHARP_DEBUG                  LWIP_DBG_OFF

#define PBUF_DEBUG                    LWIP_DBG_OFF

#define TCPIP_MBOX_SIZE               64
#define TCPIP_THREAD_STACKSIZE        1024
#define TCPIP_THREAD_PRIO             28

#define DEFAULT_THREAD_STACKSIZE      1024
#define DEFAULT_THREAD_PRIO           1
#define DEFAULT_RAW_RECVMBOX_SIZE     32
#define DEFAULT_UDP_RECVMBOX_SIZE     64
#define DEFAULT_TCP_RECVMBOX_SIZE     64
#define DEFAULT_ACCEPTMBOX_SIZE       32

#define LWIP_CHKSUM_ALGORITHM         3
#define LWIP_TCPIP_CORE_LOCKING_INPUT 1

#define PBUF_LINK_ENCAPSULATION_HLEN  388

#define MEMP_NUM_NETBUF               32
#define MEMP_NUM_NETCONN              16

#define MAC_TXQ_DEPTH                 32
#define MAC_RXQ_DEPTH                 12

#define IP_REASS_MAX_PBUFS            (2 * MAC_RXQ_DEPTH - 2)
#define MEMP_NUM_REASSDATA            LWIP_MIN((IP_REASS_MAX_PBUFS), 5)

#define TCP_MSS                       (1500 - 40)
#define TCP_WND                       (2 * MAC_RXQ_DEPTH * TCP_MSS)
#define TCP_SND_BUF                   (4 * TCP_MSS)

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

#define LWIP_HEAP_SIZE                (40 * 1024)
#ifdef LWIP_HEAP_SIZE
#define MEM_SIZE LWIP_HEAP_SIZE
#else
#if MEM_MIN > 8192
#define MEM_SIZE MEM_MIN
#else
#define MEM_SIZE 8192
#endif
#endif
#define MEMP_NUM_UDP_PCB 20

#define LWIP_HOOK_INCLUDE         "lwiphooks.h"
#define LWIP_HOOK_FILENAME        "otbr_lwip_hooks.h"

#define LWIP_RAW                  1
#define LWIP_TIMEVAL_PRIVATE      0 // use sys/time.h for struct timeval

// #define LWIP_PROVIDE_ERRNO        1
#define LWIP_ERRNO_STDINCLUDE     1
#define LWIP_SOCKET_SET_ERRNO     1

#define LWIP_NETIF_HOSTNAME     1

/* ---------- Socket options ---------- */
#define LWIP_SOCKET 1
#define LWIP_SOCKET_MAX_MEMBERSHIPS 300
#define LWIP_SO_RCVTIMEO          1
#define LWIP_SO_SNDTIMEO          1
#define SO_REUSE                  1
#define LWIP_TCP_KEEPALIVE        1

/* ---------- LOOPIF options ---------- */
#define LWIP_HAVE_LOOPIF              1
#define LWIP_NETIF_LOOPBACK           1
#define LWIP_LOOPBACK_MAX_PBUFS       0

/* ---------- IPv4 options ---------- */
#define LWIP_IPV4 1
#define IP_FORWARD 1
#define IP_NAPT 1

/* ---------- IPv6 options ---------- */
#define LWIP_IPV6 1
#define LWIP_IPV6_SCOPES 0
#define LWIP_IPV6_FORWARD 1
#define LWIP_IPV6_NUM_ADDRESSES 20

/* ---------- ICMP options ---------- */
#define LWIP_ICMP 1

/* ---------- DHCP options ---------- */
#define LWIP_DHCP 1

/* ---------- DHCPv6 options ---------- */
#define LWIP_IPV6_DHCP6 1

/* ---------- DNS options ---------- */
#define LWIP_DNS                        1
#define LWIP_DNS_SECURE                 0
#define LWIP_DNS_SERVER                 0

/* ---------- Multi-cast options ---- */
#define LWIP_IGMP               1
#define LWIP_IPV6_MLD           1
#define MEMP_NUM_MLD6_GROUP 300
#define LWIP_MULTICAST_PING 1
#define LWIP_MULTICAST_TX_OPTIONS 1
#define LWIP_BROADCAST_PING           1


#define LWIP_NETIF_STATUS_CALLBACK    1
#define LWIP_NETIF_EXT_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1

/* ---------- OTBR options ---------- */
#define OTBR_LOCK_TCPIP_CORE()
#define OTBR_UNLOCK_TCPIP_CORE()

#define OTBR_MAX_RIO_ROUTE 20
#define OTBR_RIO_TIMEOUT 0xffffffff / (1000 * 4)

/* -- MEMP_NUM_SYS_TIMEOUT options --- */
#define MEMP_NUM_SYS_TIMEOUT          (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 8 + 3 + OTBR_MAX_RIO_ROUTE)

#ifdef __cplusplus
extern "C" int *__errno(void);
#else
extern int *__errno(void);
#endif
#define errno                         (*__errno())

#define ETHARP_SUPPORT_STATIC_ENTRIES 1
#define LWIP_HTTPD_FILE_EXTENSION 1

#define LWIP_SUPPORT_CUSTOM_PBUF      1
#ifdef HIGH_PERFORMANCE_PBUF
#define LWIP_NETIF_TX_SINGLE_PBUF 0
#else
#define LWIP_NETIF_TX_SINGLE_PBUF 1
#endif
#define LWIP_RAND()                                      ((u32_t)random())

#endif /* LWIP_HDR_LWIPOPTS_H__ */
