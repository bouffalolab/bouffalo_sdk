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

#define NO_SYS                                           0
#define SYS_LIGHTWEIGHT_PROT                             1
#define LWIP_COMPAT_MUTEX                                0

/* Run socket/netconn operations and RX processing under the lwIP core lock. */
#define LWIP_TCPIP_CORE_LOCKING                          1
#define LWIP_TCPIP_CORE_LOCKING_INPUT                    1

/* Core protocol support. */
#define LWIP_IPV4                                        1
#define LWIP_TCP                                         1
#define LWIP_UDP                                         1
#define LWIP_RAW                                         1
#define LWIP_ICMP                                        1
#define LWIP_DHCP                                        1
#define LWIP_DNS                                         1
#define LWIP_DNS_SERVER                                  0
#define LWIP_IGMP                                        0
#define LWIP_MULTICAST_TX_OPTIONS                        1
#define LWIP_MULTICAST_PING                              1
#define LWIP_BROADCAST_PING                              1
#define LWIP_RAND()                                      ((u32_t)random())

/* Heap and fixed-size pool configuration. */
#define MEM_ALIGNMENT                                    4
#define MEM_SIZE                                         (32 * 1024)
#define MEMP_MEM_MALLOC                                  0
#define MEM_LIBC_MALLOC                                  0

/* This application must not place lwIP pools in the Wi-Fi RAM region. */
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)]

#define MEMP_NUM_NETBUF                                  32
#define MEMP_NUM_NETCONN                                 16
#define MEMP_NUM_UDP_PCB                                 16
#define MEMP_NUM_TCP_PCB                                 8
#define MEMP_NUM_TCP_PCB_LISTEN                          8
#define MEMP_NUM_TCPIP_MSG_API                           16

/* Limit the IPv4 reassembly resources used by lwIP. */
#define IP_REASS_MAX_PBUFS                               12
#define MEMP_NUM_REASSDATA                               LWIP_MIN(IP_REASS_MAX_PBUFS, 5)

/* High-throughput TCP settings. */
#define TCP_MSS                                          (1500 - 40)
#define TCP_WND                                          (16 * TCP_MSS)
#define TCP_SND_BUF                                      (16 * TCP_MSS)
#define TCP_SND_QUEUELEN                                 ((3 * TCP_SND_BUF) / TCP_MSS)
#define MEMP_NUM_TCP_SEG                                 TCP_SND_QUEUELEN
#define MEMP_NUM_PBUF                                    ((TCP_SND_BUF / TCP_MSS) + 8)

#define TCP_QUEUE_OOSEQ                                  1
#define TCP_OOSEQ_MAX_PBUFS                              10
#define LWIP_WND_SCALE                                   1
#define TCP_RCV_SCALE                                    2
#define TCP_SNDLOWAT                                     LWIP_MIN((16 * TCP_MSS), (TCP_SND_BUF / 2))

/* RX packets use PBUF_RAM; the netif driver supports chained TX pbufs. */
#define PBUF_POOL_SIZE                                   0
#define PBUF_LINK_ENCAPSULATION_HLEN                     0
#define LWIP_NETIF_TX_SINGLE_PBUF                        0

/* Netif and socket features used by the USB ECM host demo. */
#define LWIP_NETIF_API                                   1
#define LWIP_NETIF_HOSTNAME                              1
#define LWIP_NETIF_STATUS_CALLBACK                       1
#define LWIP_NETIF_LINK_CALLBACK                         1
#define ETHARP_SUPPORT_STATIC_ENTRIES                    1

#define LWIP_NETIF_LOOPBACK                              1
#define LWIP_HAVE_LOOPIF                                 1
#define LWIP_LOOPBACK_MAX_PBUFS                          0

#define LWIP_NETCONN                                     1
#define LWIP_SOCKET                                      1
#define LWIP_SO_RCVTIMEO                                 1
#define LWIP_SO_SNDTIMEO                                 1

#define SO_REUSE                                         1
#define LWIP_TCP_KEEPALIVE                               1
#define LWIP_TIMEVAL_PRIVATE                             0
#define LWIP_ERRNO_STDINCLUDE                            1
#define LWIP_SOCKET_SET_ERRNO                            1

/* The USB ECM host performs checksums in software. */
#define LWIP_CHKSUM_ALGORITHM                            3
#define LWIP_CHECKSUM_ON_COPY                            0
#define CHECKSUM_GEN_IP                                  1
#define CHECKSUM_GEN_UDP                                 1
#define CHECKSUM_GEN_TCP                                 1
#define CHECKSUM_GEN_ICMP                                1
#define CHECKSUM_CHECK_IP                                1
#define CHECKSUM_CHECK_UDP                               1
#define CHECKSUM_CHECK_TCP                               1
#define CHECKSUM_CHECK_ICMP                              1

/* TCP/IP thread and sequential API mailboxes. RX packets bypass tcpip_mbox. */
#define osPriorityIdle                                   2
#define osPriorityLow                                    5
#define osPriorityBelowNormal                            10
#define osPriorityNormal                                 15
#define osPriorityAboveNormal                            20
#define osPriorityHigh                                   25
#define osPriorityRealtime                               29

#define TCPIP_THREAD_NAME                                "TCP/IP"
#define TCPIP_THREAD_STACKSIZE                           1024
#define TCPIP_THREAD_PRIO                                osPriorityHigh
#define TCPIP_MBOX_SIZE                                  64
#define DEFAULT_THREAD_STACKSIZE                         1024
#define DEFAULT_THREAD_PRIO                              1
#define DEFAULT_RAW_RECVMBOX_SIZE                        32
#define DEFAULT_UDP_RECVMBOX_SIZE                        64
#define DEFAULT_TCP_RECVMBOX_SIZE                        64
#define DEFAULT_ACCEPTMBOX_SIZE                          32

/* Allow concurrent read/write/close on one socket. */
#define LWIP_NETCONN_FULLDUPLEX                          1
#define LWIP_NETCONN_SEM_PER_THREAD                      1

void *sys_thread_sem_get(void);
void sys_thread_sem_init(void);
void sys_thread_sem_deinit(void);
#define LWIP_NETCONN_THREAD_SEM_GET()   sys_thread_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_thread_sem_init()
#define LWIP_NETCONN_THREAD_SEM_FREE()  sys_thread_sem_deinit()

/* Keep statistics available while leaving verbose protocol logs disabled. */
#define LWIP_STATS                      1
#define LWIP_STATS_DISPLAY              1
#define LWIP_DEBUG                      1
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
#define UDP_DEBUG                       LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#define API_LIB_DEBUG                   LWIP_DBG_OFF
#define PBUF_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                        LWIP_DBG_OFF

#endif /* LWIP_HDR_LWIPOPTS_H__ */
