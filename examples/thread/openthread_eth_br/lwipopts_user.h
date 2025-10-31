/**
 * Copyright (c) 2022 Bouffalolab team
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 * Author: Adam Dunkels <adam@sics.se>
 ******************************************************************************
 */

#ifndef LWIP_HDR_LWIPOPTS_H__
#define LWIP_HDR_LWIPOPTS_H__

#include "arch/sys_arch.h"

#define LWIP_NETIF_API           1

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS 0

/**
 * LWIP_TCPIP_CORE_LOCKING, lwip thread safe protect options
 */
#define LWIP_TCPIP_CORE_LOCKING          1
#define LWIP_TCPIP_CORE_LOCKING_INPUT    0

#ifdef __cplusplus
extern "C" sys_mutex_t lock_tcpip_core;
extern "C" int sys_is_inside_interrupt(void);
extern "C" int sys_mutex_is_locked(sys_mutex_t *mutex);
extern "C" int sys_current_is_tcpip(void);
#else
extern sys_mutex_t lock_tcpip_core;
extern int sys_is_inside_interrupt(void);
extern int sys_mutex_is_locked(sys_mutex_t *mutex);
extern int sys_current_is_tcpip(void);
#endif

#define LWIP_ASSERT_CORE_LOCKED()       \
do {\
   if(lock_tcpip_core && (sys_is_inside_interrupt() || !sys_mutex_is_locked(&lock_tcpip_core))) {\
      printf("api must call with lwip core lock %s: %d\r\n", __FILE__, __LINE__); \
      asm("ebreak");\
   }\
} while(0)

/* ---------- OTBR options ---------- */
// #define OTBR_LOCK_TCPIP_CORE()         
// #define OTBR_UNLOCK_TCPIP_CORE()       
#define OTBR_LOCK_TCPIP_CORE()         LOCK_TCPIP_CORE()
#define OTBR_UNLOCK_TCPIP_CORE()       UNLOCK_TCPIP_CORE()

#define OTBR_MAX_RIO_ROUTE 20
#define OTBR_RIO_TIMEOUT 0xffffffff / (1000 * 4)

/* ---------- Memory options ---------- */
#define MEM_ALIGNMENT 4
#define MEM_SIZE (20 * 1024)
#define MEMP_NUM_PBUF   26
#define MEMP_NUM_NETBUF 16
#define MEMP_NUM_UDP_PCB 20
#define MEMP_NUM_TCP_PCB 10
#define MEMP_NUM_TCP_PCB_LISTEN 5
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_NUM_SYS_TIMEOUT_INTERNAL + OTBR_MAX_RIO_ROUTE)

/*
   ---------- LOOPIF options ----------
*/
#define LWIP_HAVE_LOOPIF           1

/* ---------- IPv4 options ---------- */
#define LWIP_IPV4 1
#define IP_FORWARD 1
#define IP_NAPT 1

/* ---------- IPv6 options ---------- */
#define LWIP_IPV6 1
#define LWIP_IPV6_SCOPES 0
#define LWIP_IPV6_FORWARD 1
#define LWIP_IPV6_NUM_ADDRESSES 20

/* ---------- Pbuf options ---------- */
#define PBUF_POOL_SIZE (TCP_WND / TCP_MSS)
#define PBUF_POOL_BUFSIZE 1524

/* ---------- ICMP options ---------- */
#define LWIP_ICMP 1

/* ---------- DHCP options ---------- */
#define LWIP_DHCP 1

/* ---------- DHCPv6 options ---------- */
#define LWIP_IPV6_DHCP6 1

/* ---------- UDP options ---------- */
#define LWIP_UDP 1
#define UDP_TTL  255

/* ---------- DNS options ---------- */
#define LWIP_DNS                        1
#define LWIP_DNS_SECURE                 0

/* ---------- Multi-cast options ---- */
#define LWIP_IGMP               1
#define LWIP_IPV6_MLD           1
#define MEMP_NUM_MLD6_GROUP 300
#define LWIP_MULTICAST_PING 1

#define LWIP_RAW                        1

/* ---------- Statistics options ---------- */
#define LWIP_STATS               1
#define LWIP_STATS_DISPLAY       0

#define LWIP_TIMEVAL_PRIVATE      0 // use sys/time.h for struct timeval

#define LWIP_ERRNO_STDINCLUDE     1
#define LWIP_SOCKET_SET_ERRNO     1
#ifdef __cplusplus
extern "C" int *__errno(void);
#else
extern int *__errno(void);
#endif
#define errno                         (*__errno())

/* ---------- link callback options ---------- */
/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK 1

#define LWIP_NETIF_STATUS_CALLBACK     1
#define LWIP_NETIF_API                 1
#define LWIP_NETIF_EXT_STATUS_CALLBACK 1
/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/
/* LWIP_CHECKSUM_ON_COPY==1: Calculate checksum when copying data from application buffers to pbufs. */
#define LWIP_CHECKSUM_ON_COPY 0

#ifdef CHECKSUM_BY_HARDWARE
/* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
#define CHECKSUM_GEN_IP 0
/* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP 0
/* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP 0
/* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
#define CHECKSUM_CHECK_IP 0
/* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP 0
/* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP 0
/* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP 0
#else
/* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
#define CHECKSUM_GEN_IP     1
/* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP    1
/* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP    1
/* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
#define CHECKSUM_CHECK_IP   1
/* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP  1
/* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP  1
/* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP   1
#define CHECKSUM_CHECK_ICMP 1
#endif

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN 1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
#define LWIP_SOCKET 1
#define LWIP_SOCKET_MAX_MEMBERSHIPS 300

/*
   ---------------------------------------
   ---------- Hook options ---------------
   ---------------------------------------
*/
#define LWIP_HOOK_FILENAME "otbr_lwip_hooks.h"

/*
   ----------------------------------------
   ---------- Lwip Debug options ----------
   ----------------------------------------
*/
#define LWIP_DEBUG       LWIP_DBG_OFF
#define SYS_DEBUG        LWIP_DBG_OFF
#define TIMERS_DEBUG     LWIP_DBG_OFF
#define ETHARP_DEBUG     LWIP_DBG_OFF
#define NETIF_DEBUG      LWIP_DBG_OFF
#define ICMP_DEBUG       LWIP_DBG_OFF
#define DNS_DEBUG        LWIP_DBG_OFF
#define TCPIP_DEBUG      LWIP_DBG_OFF
#define TCP_DEBUG        LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG LWIP_DBG_OFF
#define UDP_DEBUG        LWIP_DBG_OFF
#define SOCKET_DEBUG     LWIP_DBG_OFF
#define API_LIB_DEBUG    LWIP_DBG_OFF
#define HTTPD_DEBUG      LWIP_DBG_OFF
#define PBUF_DEBUG       LWIP_DBG_OFF
#define IP_DEBUG         LWIP_DBG_OFF
#define DHCP_DEBUG       LWIP_DBG_OFF
#define NAPT_DEBUG       LWIP_DBG_OFF
#define RAW_DEBUG        LWIP_DBG_OFF
#define IP6_DEBUG        LWIP_DBG_OFF
#define DHCP6_DEBUG      LWIP_DBG_OFF

/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/

#define osPriorityIdle                                   2  //< priority: idle (lowest)
#define osPriorityLow                                    5  //< priority: low
#define osPriorityBelowNormal                            10 //< priority: below normal
#define osPriorityNormal                                 15 //< priority: normal (default)
#define osPriorityAboveNormal                            20 //< priority: above normal
#define osPriorityHigh                                   25 //< priority: high
#define osPriorityRealtime                               29 //< priority: realtime (highest)

#define TCPIP_THREAD_NAME         "TCP/IP"
#define TCPIP_THREAD_STACKSIZE    2048
#define TCPIP_MBOX_SIZE           50
#define DEFAULT_UDP_RECVMBOX_SIZE 100
#define DEFAULT_TCP_RECVMBOX_SIZE 100
#define DEFAULT_RAW_RECVMBOX_SIZE 100
#define DEFAULT_ACCEPTMBOX_SIZE   100
#define DEFAULT_THREAD_STACKSIZE  512
#define TCPIP_THREAD_PRIO         osPriorityHigh


#define EMAC_DHCP_STACK_SIZE      1024

#define LWIP_RAND() ((u32_t)random())
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[size] __attribute__((aligned(4)))

#define LWIP_DNS_SERVER                                  0

#endif /* LWIP_HDR_LWIPOPTS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
