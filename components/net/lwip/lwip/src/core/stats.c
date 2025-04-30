/**
 * @file
 * Statistics module
 *
 */

/*
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
 *
 */

#include "lwip/opt.h"

#if LWIP_STATS /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/debug.h"
#if LWIP_TCP
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#endif
#if LWIP_UDP
#include "lwip/udp.h"
#endif
#if LWIP_RAW
#include "lwip/raw.h"
#endif

#if IP_NAPT
#include "lwip/lwip_napt.h"
#endif

#include <string.h>
#include "lwip/api.h"

struct stats_ lwip_stats;

void
stats_init(void)
{
#ifdef LWIP_DEBUG
#if MEM_STATS
  lwip_stats.mem.name = "MEM";
#endif /* MEM_STATS */
#endif /* LWIP_DEBUG */
}

#if LWIP_STATS_DISPLAY
void
stats_display_proto(struct stats_proto *proto, const char *name)
{
  LWIP_PLATFORM_DIAG(("\n%s\n\t", name));
  LWIP_PLATFORM_DIAG(("xmit: %"STAT_COUNTER_F"\n\t", proto->xmit));
  LWIP_PLATFORM_DIAG(("recv: %"STAT_COUNTER_F"\n\t", proto->recv));
  LWIP_PLATFORM_DIAG(("fw: %"STAT_COUNTER_F"\n\t", proto->fw));
  LWIP_PLATFORM_DIAG(("drop: %"STAT_COUNTER_F"\n\t", proto->drop));
  LWIP_PLATFORM_DIAG(("chkerr: %"STAT_COUNTER_F"\n\t", proto->chkerr));
  LWIP_PLATFORM_DIAG(("lenerr: %"STAT_COUNTER_F"\n\t", proto->lenerr));
  LWIP_PLATFORM_DIAG(("memerr: %"STAT_COUNTER_F"\n\t", proto->memerr));
  LWIP_PLATFORM_DIAG(("rterr: %"STAT_COUNTER_F"\n\t", proto->rterr));
  LWIP_PLATFORM_DIAG(("proterr: %"STAT_COUNTER_F"\n\t", proto->proterr));
  LWIP_PLATFORM_DIAG(("opterr: %"STAT_COUNTER_F"\n\t", proto->opterr));
  LWIP_PLATFORM_DIAG(("err: %"STAT_COUNTER_F"\n\t", proto->err));
  LWIP_PLATFORM_DIAG(("cachehit: %"STAT_COUNTER_F"\n", proto->cachehit));
}

#if IGMP_STATS || MLD6_STATS
void
stats_display_igmp(struct stats_igmp *igmp, const char *name)
{
  LWIP_PLATFORM_DIAG(("\n%s\n\t", name));
  LWIP_PLATFORM_DIAG(("xmit: %"STAT_COUNTER_F"\n\t", igmp->xmit));
  LWIP_PLATFORM_DIAG(("recv: %"STAT_COUNTER_F"\n\t", igmp->recv));
  LWIP_PLATFORM_DIAG(("drop: %"STAT_COUNTER_F"\n\t", igmp->drop));
  LWIP_PLATFORM_DIAG(("chkerr: %"STAT_COUNTER_F"\n\t", igmp->chkerr));
  LWIP_PLATFORM_DIAG(("lenerr: %"STAT_COUNTER_F"\n\t", igmp->lenerr));
  LWIP_PLATFORM_DIAG(("memerr: %"STAT_COUNTER_F"\n\t", igmp->memerr));
  LWIP_PLATFORM_DIAG(("proterr: %"STAT_COUNTER_F"\n\t", igmp->proterr));
  LWIP_PLATFORM_DIAG(("rx_v1: %"STAT_COUNTER_F"\n\t", igmp->rx_v1));
  LWIP_PLATFORM_DIAG(("rx_group: %"STAT_COUNTER_F"\n\t", igmp->rx_group));
  LWIP_PLATFORM_DIAG(("rx_general: %"STAT_COUNTER_F"\n\t", igmp->rx_general));
  LWIP_PLATFORM_DIAG(("rx_report: %"STAT_COUNTER_F"\n\t", igmp->rx_report));
  LWIP_PLATFORM_DIAG(("tx_join: %"STAT_COUNTER_F"\n\t", igmp->tx_join));
  LWIP_PLATFORM_DIAG(("tx_leave: %"STAT_COUNTER_F"\n\t", igmp->tx_leave));
  LWIP_PLATFORM_DIAG(("tx_report: %"STAT_COUNTER_F"\n", igmp->tx_report));
}
#endif /* IGMP_STATS || MLD6_STATS */

#if MEM_STATS || MEMP_STATS
void
stats_display_mem(struct stats_mem *mem, const char *name)
{
  LWIP_PLATFORM_DIAG(("\nMEM %s\n\t", name));
  LWIP_PLATFORM_DIAG(("avail: %"MEM_SIZE_F"\n\t", mem->avail));
  LWIP_PLATFORM_DIAG(("used: %"MEM_SIZE_F"\n\t", mem->used));
  LWIP_PLATFORM_DIAG(("max: %"MEM_SIZE_F"\n\t", mem->max));
  LWIP_PLATFORM_DIAG(("err: %"STAT_COUNTER_F"\n", mem->err));
}

#if MEMP_STATS
void
stats_display_memp(struct stats_mem *mem, int idx)
{
  if (idx < MEMP_MAX) {
    stats_display_mem(mem, mem->name);
  }
}
#endif /* MEMP_STATS */
#endif /* MEM_STATS || MEMP_STATS */

#if SYS_STATS
void
stats_display_sys(struct stats_sys *sys)
{
  LWIP_PLATFORM_DIAG(("\nSYS\n\t"));
  LWIP_PLATFORM_DIAG(("sem.used:  %"STAT_COUNTER_F"\n\t", sys->sem.used));
  LWIP_PLATFORM_DIAG(("sem.max:   %"STAT_COUNTER_F"\n\t", sys->sem.max));
  LWIP_PLATFORM_DIAG(("sem.err:   %"STAT_COUNTER_F"\n\t", sys->sem.err));
  LWIP_PLATFORM_DIAG(("mutex.used: %"STAT_COUNTER_F"\n\t", sys->mutex.used));
  LWIP_PLATFORM_DIAG(("mutex.max:  %"STAT_COUNTER_F"\n\t", sys->mutex.max));
  LWIP_PLATFORM_DIAG(("mutex.err:  %"STAT_COUNTER_F"\n\t", sys->mutex.err));
  LWIP_PLATFORM_DIAG(("mbox.used:  %"STAT_COUNTER_F"\n\t", sys->mbox.used));
  LWIP_PLATFORM_DIAG(("mbox.max:   %"STAT_COUNTER_F"\n\t", sys->mbox.max));
  LWIP_PLATFORM_DIAG(("mbox.err:   %"STAT_COUNTER_F"\n", sys->mbox.err));
}
#endif /* SYS_STATS */

#if IP_NAPT_STATS
void stats_display_ip_napt(struct stats_ip_napt *napt)
{
  LWIP_PLATFORM_DIAG(("\nIP NAPT max %"STAT_COUNTER_F"\n\t", IP_NAPT_MAX));
  LWIP_PLATFORM_DIAG(("nr_active_tcp:        %"STAT_COUNTER_F"\n\t", napt->nr_active_tcp));
  LWIP_PLATFORM_DIAG(("nr_active_udp:        %"STAT_COUNTER_F"\n\t", napt->nr_active_udp));
  LWIP_PLATFORM_DIAG(("nr_active_icmp:       %"STAT_COUNTER_F"\n\t", napt->nr_active_icmp));
  LWIP_PLATFORM_DIAG(("nr_forced_evictions:  %"STAT_COUNTER_F"\n\t", napt->nr_forced_evictions));
#if BL_IP_FORWARD
  LWIP_PLATFORM_DIAG(("mem_alloc_err:        %"STAT_COUNTER_F"\n\t", napt->mem_alloc_err));
  LWIP_PLATFORM_DIAG(("output_err:           %"STAT_COUNTER_F"\n\t", napt->output_err));
#endif
}
#endif /* IP_NAPT_STATS */

void
stats_display(void)
{
  s16_t i;

  LINK_STATS_DISPLAY();
  ETHARP_STATS_DISPLAY();
  IPFRAG_STATS_DISPLAY();
  IP6_FRAG_STATS_DISPLAY();
  IP_STATS_DISPLAY();
  ND6_STATS_DISPLAY();
  IP6_STATS_DISPLAY();
  IGMP_STATS_DISPLAY();
  MLD6_STATS_DISPLAY();
  ICMP_STATS_DISPLAY();
  ICMP6_STATS_DISPLAY();
  UDP_STATS_DISPLAY();
  TCP_STATS_DISPLAY();
  MEM_STATS_DISPLAY();
  for (i = 0; i < MEMP_MAX; i++) {
    MEMP_STATS_DISPLAY(i);
  }
  SYS_STATS_DISPLAY();
  IP_NAPT_STATS_DISPLAY();
}

#endif /* LWIP_STATS_DISPLAY */

#if LWIP_TCP
static void
netstat_tcp_listen_pcbs_dump(union tcp_listen_pcbs_t *pcb_head)
{
  char ip_str[50];
  char str_laddr[64];
  uint32_t txq_size = 0;
  uint32_t rxq_size = 0;
  struct tcp_pcb_listen *lpcb;
  const char *str_raddr = "-:*";

  if (!pcb_head)
    return;

  /*
   * NB: do not touch members after lpcb->local_port, because you don't
   * know if this is a listen_pcb or a full tcp_pcb.
   */
  for (lpcb = pcb_head->listen_pcbs; lpcb != NULL; lpcb = lpcb->next) {
    ipaddr_ntoa_r(&lpcb->local_ip, ip_str, sizeof(ip_str));
    snprintf(str_laddr, sizeof(str_laddr) - 1,
             "%s:%"U16_F, ip_str, lpcb->local_port);
    LWIP_PLATFORM_DIAG(("%-10s%-16"U32_F"%-8"U32_F"%-25s%-25s%-15s\r\n",
                IP_IS_V4(&lpcb->local_ip) ? "tcp" : "tcp6",
                rxq_size, txq_size, str_laddr, str_raddr,
                tcp_debug_state_str(lpcb->state)));
  }
}

static void
netstat_tcp_pcbs_dump(struct tcp_pcb *pcb_head)
{
  char ip_str[50];
  char str_rxq[32];
  uint32_t txq_size;
  struct tcp_pcb *pcb;
  uint32_t rxq_size, rxmem_size;
  char str_laddr[64], str_raddr[64];

  for (pcb = pcb_head; pcb != NULL; pcb = pcb->next) {
    rxq_size = (pcb->refused_data ? pcb->refused_data->tot_len : 0);
#if LWIP_NETCONN
    if (tcp_owner_is_netconn(pcb)) {
#if LWIP_SO_RCVBUF
      struct netconn *netconn = pcb->callback_arg;
      /* received in sequence, but not processed by application yet */
      rxq_size += (netconn ? netconn->recv_avail : 0);
#endif /* LWIP_SO_RCVBUF */
    }
#endif /* LWIP_NETCONN */
#if TCP_QUEUE_OOSEQ
    rxmem_size = rxq_size + (pcb->ooseq ? pcb->ooseq->len : 0);
#else
    rxmem_size = rxq_size;
#endif

    txq_size = (pcb->unsent ? pcb->unsent->len : 0) +
        (pcb->unacked ? pcb->unacked->len : 0);

    snprintf(str_rxq, sizeof(str_rxq) - 1, "%"U32_F"(%"U32_F")",
             rxmem_size, rxq_size);
    ipaddr_ntoa_r(&pcb->local_ip, ip_str, sizeof(ip_str));
    snprintf(str_laddr, sizeof(str_laddr) - 1,
             "%s:%"U16_F, ip_str, pcb->local_port);
    ipaddr_ntoa_r(&pcb->remote_ip, ip_str, sizeof(ip_str));
    snprintf(str_raddr, sizeof(str_raddr) - 1,
             "%s:%"U16_F, ip_str, pcb->remote_port);
    LWIP_PLATFORM_DIAG(("%-10s%-16s%-8"U32_F"%-25s%-25s%-15s\r\n",
                IP_IS_V4(&pcb->local_ip) ? "tcp" : "tcp6",
                str_rxq, txq_size, str_laddr, str_raddr,
                tcp_debug_state_str(pcb->state)));
  }
}
#endif /* LWIP_TCP */

#ifdef LWIP_UDP
static void
netstat_udp_pcbs_dump(struct udp_pcb *pcb_head)
{
  char ip_str[50];
  char str_laddr[64];
  char str_raddr[64];
  struct udp_pcb *pcb = pcb_head;

  for (pcb = pcb_head; pcb != NULL; pcb = pcb->next) {
    /**
     * TODO an UDP connection is established if we saw data flows in two
     * directions.
     */
    const char *state = "-";
    uint32_t txq_size = 0, rxq_size = 0;

#if LWIP_NETCONN
    if (udp_owner_is_netconn(pcb)) {
#if LWIP_SO_RCVBUF
      struct netconn *netconn = pcb->recv_arg;
      /* received but not processed by application yet */
      rxq_size = (netconn ? netconn->recv_avail : 0);
#endif /* LWIP_SO_RCVBUF */
    }
#endif /* LWIP_NETCONN */
    ipaddr_ntoa_r(&pcb->local_ip, ip_str, sizeof(ip_str));
    snprintf(str_laddr, sizeof(str_laddr) - 1, "%s:%"U16_F,
             ip_str, pcb->local_port);
    ipaddr_ntoa_r(&pcb->remote_ip, ip_str, sizeof(ip_str));
    snprintf(str_raddr, sizeof(str_raddr) - 1, "%s:%"U16_F,
            ip_str, pcb->remote_port);
    LWIP_PLATFORM_DIAG(("%-10s%-8"U32_F"%-8"U32_F"%-25s%-25s%-15s\r\n",
                IP_IS_V4(&pcb->local_ip) ? "udp" : "udp6",
                rxq_size, txq_size, str_laddr, str_raddr, state));
  }
}
#endif /* LWIP_UDP */

#if LWIP_RAW
static void
netstat_raw_pcbs_dump(struct raw_pcb *pcb_head)
{
  struct raw_pcb *pcb;
  char str_laddr[50], str_raddr[50];

  for (pcb = pcb_head; pcb != NULL; pcb = pcb->next) {
    ipaddr_ntoa_r(&pcb->local_ip, str_laddr, sizeof(str_laddr));
    ipaddr_ntoa_r(&pcb->remote_ip, str_raddr, sizeof(str_raddr));
    LWIP_PLATFORM_DIAG(("%-10s%-25s%-25s%-10d",
                IP_IS_V4(&pcb->local_ip) ? "raw" : "raw6",
                str_laddr, str_raddr, pcb->protocol));
  }
}
#endif /* LWIP_RAW */

void
stats_netstat(void *ctx)
{
  LWIP_UNUSED_ARG(ctx);
  LWIP_ASSERT_CORE_LOCKED();

  /* title strings */
  const char *ts_proto = "Proto";
  const char *ts_rxq = "Recv-Q";
  const char *ts_txq = "Send-Q";
  const char *ts_laddr = "Local Address";
  const char *ts_raddr = "Foreign Address";
  const char *ts_state = "State";

#if LWIP_TCP
  LWIP_PLATFORM_DIAG(("%-10s%-16s%-8s%-25s%-25s%-15s\r\n",
              ts_proto, ts_rxq, ts_txq, ts_laddr, ts_raddr, ts_state));
  netstat_tcp_listen_pcbs_dump(&tcp_listen_pcbs);
  netstat_tcp_pcbs_dump(tcp_bound_pcbs);
  netstat_tcp_pcbs_dump(tcp_active_pcbs);
  netstat_tcp_pcbs_dump(tcp_tw_pcbs);
  LWIP_PLATFORM_DIAG(("\r\nNote: tcp Recv-Q A(B), A - recv buffer size, "
                      "B - recv queue size\r\n"));
  LWIP_PLATFORM_DIAG(("\r\n\r\n"));
#endif /* LWIP_TCP */

#if LWIP_UDP
  LWIP_PLATFORM_DIAG(("%-10s%-8s%-8s%-25s%-25s%-15s\r\n",
              ts_proto, ts_rxq, ts_txq, ts_laddr, ts_raddr, ts_state));
  netstat_udp_pcbs_dump(udp_pcbs);
  LWIP_PLATFORM_DIAG(("\r\n\r\n"));
#endif

#if LWIP_RAW
  LWIP_PLATFORM_DIAG(("%-10s%-25s%-25s%-10s\r\n",
              ts_proto, ts_laddr, ts_raddr, "protocol"));
  netstat_raw_pcbs_dump(raw_get_pcbs());
  LWIP_PLATFORM_DIAG(("\r\n\r\n"));
#endif /* LWIP_RAW */
}

#endif /* LWIP_STATS */

