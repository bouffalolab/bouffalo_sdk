/**
 ****************************************************************************************
 *
 * @file nuttx.c
 *
 * @brief Implementation of wl80211 networking stack abstraction layer for NuttX.
 *
 * This file provides the NuttX-specific implementation of the wl80211 networking
 * APIs, enabling WiFi functionality on the BL616 platform. It bridges the gap
 * between the wl80211 MAC layer and the NuttX networking stack using IOB buffers.
 *
 * Copyright (C) BouffaloLab 2017-2020
 *
 ****************************************************************************************
 */

/* INCLUDE FILES
 **************************************************************************************** */

#include <assert.h>
#include <debug.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/queue.h>
#include <sys/uio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <syslog.h>

#include <nuttx/net/netdev.h>
#include <nuttx/mm/iob.h>
#include <nuttx/irq.h>
#include "mbedtls/mbedtls_config.h"
#include <nuttx/net/ip.h>
#include <nuttx/net/tcp.h>
#include <nuttx/net/udp.h>

#include "wl80211.h"
#include "macsw/wl80211_mac.h"
#include <bflb_sec_trng.h>

/* DEFINES
 **************************************************************************************** */

#define NETIF_MAX_HWADDR_LEN          (6)
#define CONFIG_WL80211_TX_RETRY_COUNT (7)

/* Network header length constants for packet dump */
#define ETH_HDRLEN     14  /* Ethernet header length */
#define IPv4_HDRLEN    20  /* Minimum IPv4 header length */
#define TCP_HDRLEN     20  /* Minimum TCP header length */
#define UDP_HDRLEN      8  /* UDP header length */

/* TYPE DEFINITIONS
 **************************************************************************************** */

/* Type definitions for API compatibility */
typedef struct wl80211_netif net_if_t;
typedef struct iob_s net_buf_tx_t;
typedef void (*net_buf_free_fn)(void *buf);
typedef void (*rx_cb_type)(void *buf, void *addr, uint16_t len, void *free_fn);

/**
 * Structure for wl80211 raw packet free callback wrapper
 * Stores both the user callback and IOB buffer to ensure both are properly handled
 */
typedef struct wl80211_raw_free_data
{
  net_buf_tx_t *iob;  /**< IOB buffer to free */
  void (*cb)(void *); /**< User-provided callback function */
  void *opaque;       /**< User-provided opaque data */
} wl80211_raw_free_data_t;

/**
 * Network interface structure for wl80211 in NuttX
 */
struct wl80211_netif
{
  uint8_t hwaddr[NETIF_MAX_HWADDR_LEN]; /**< MAC address */
  uint8_t num;                          /**< Interface number */
  void *vif;                            /**< Pointer to virtual interface */
  struct wl80211_netif *next;           /**< Next interface in linked list */
};

/* GLOBAL VARIABLES
 **************************************************************************************** */

static struct wl80211_netif *g_netif_list = NULL;
static void *(*g_forward_cb)(void *netbuf, void *payload, int len, void *free_fn) = NULL;
static void (*g_rx_cb)(void *buf, void *addr, uint16_t len, void *free_fn) = NULL;

/* EXTERNAL VARIABLES
 **************************************************************************************** */

/* FUNCTION DECLARATIONS
 **************************************************************************************** */

_Static_assert(sizeof(struct wl80211_mac_tx_desc) <=
                 CONFIG_NET_LL_GUARDSIZE,
               "tx_desc too large");

/**
 ****************************************************************************************
 * @brief Allocate transmit buffer
 *
 * @param[in] length  Buffer length required
 *
 * @return Pointer to allocated buffer or NULL
 ****************************************************************************************
 */
net_buf_tx_t *net_buf_tx_alloc(uint32_t length)
{
  struct iob_s *iob = iob_tryalloc(false);

  if (!iob)
    {
      wlerr("Failed to allocate IOB buffer\n");
      return NULL;
    }

  /* Reserve space for link layer header */

  iob_reserve(iob, CONFIG_NET_LL_GUARDSIZE);

  return (net_buf_tx_t *)iob;
}

/**
 ****************************************************************************************
 * @brief Free transmit buffer
 *
 * @param[in] buf  Pointer to buffer to free
 ****************************************************************************************
 */
void net_buf_tx_free(net_buf_tx_t *buf)
{
  struct iob_s *iob = (struct iob_s *)buf;

  if (iob)
    {
      iob_free_chain(iob);
    }
}

/**
 ****************************************************************************************
 * @brief Allocate buffer in WiFi RAM (WRAM)
 *
 * @param[in] size  Buffer size required
 *
 * @return Pointer to allocated buffer or NULL
 ****************************************************************************************
 */
void *wl80211_platform_malloc_wram(size_t size)
{
  struct iob_s *iob;

  if (size == 0)
    {
      return NULL;
    }

  if (size > CONFIG_IOB_BUFSIZE)
    {
      wlerr("Requested WRAM size %zu exceeds IOB buffer size %d\n", size, CONFIG_IOB_BUFSIZE);
      return NULL;
    }

  iob = iob_tryalloc(false);
  if (!iob)
    {
      return NULL;
    }

  return iob->io_data;
}

/**
 ****************************************************************************************
 * @brief Free buffer allocated in WiFi RAM (WRAM)
 *
 * @param[in] ptr   Pointer to buffer to free
 ****************************************************************************************
 */
void wl80211_platform_free_wram(void *ptr)
{
  DEBUGASSERT(ptr != NULL);

  struct iob_s *iob = container_of(ptr, struct iob_s, io_data);

  if (iob)
    {
      iob_free_chain(iob);
    }
}

/**
 ****************************************************************************************
 * @brief Register forward callback
 *
 * @param[in] cb  Pointer to forward callback function
 ****************************************************************************************
 */

void internal_register_forward_cb(void *cb)
{
  g_forward_cb = cb;
}

/**
 ****************************************************************************************
 * @brief Register receive callback
 *
 * @param[in] cb  Pointer to receive callback function
 ****************************************************************************************
 */

void internal_register_recv_cb(rx_cb_type cb)
{
  g_rx_cb = cb;
}

/**
 ****************************************************************************************
 * @brief Wrapper function for raw packet free callback
 *
 * This function is called by the MAC layer after transmission is complete.
 * It ensures both the user callback is called and the IOB buffer is freed.
 *
 * @param[in] data  Pointer to wl80211_raw_free_data_t structure
 ****************************************************************************************
 */
static void wl80211_raw_free_wrapper(void *data)
{
  wl80211_raw_free_data_t *free_data = (wl80211_raw_free_data_t *)data;

  if (free_data)
    {
      /* First call the user-provided callback if available */
      if (free_data->cb)
        {
          free_data->cb(free_data->opaque);
        }

      /* Then free the IOB buffer */
      if (free_data->iob)
        {
          net_buf_tx_free(free_data->iob);
        }

      /* Finally free the wrapper structure itself */
      free(free_data);
    }
}

/* WL80211 API IMPLEMENTATIONS
 **************************************************************************************** */

#if 0
/* WL80211 PACKET DUMP FUNCTIONS
 *****************************************************************************************
 * The following functions provide enhanced packet dumping for TCP/UDP packets.
 * - dump_ethhdr(): Main entry point, dispatches to protocol-specific handlers
 * - dump_tcp(): Parses and prints TCP header information (ports, seq, ack, payload)
 * - dump_udp(): Parses and prints UDP header information (ports, payload)
 * - dump_ethernet_only(): Fallback for non-IP or short packets
 *
 * Output format:
 *   TCP: "WLAN: TX TCP 192.168.1.100:12345 -> 192.168.1.1:443, Seq:xxx, Ack:yyy, Payload:zzz"
 *   UDP: "WLAN: TX UDP 192.168.1.100:12345 -> 192.168.1.1:53, Payload:zzz"
 *   Other: "WLAN: TX buf <addr> <len> bytes" (original format)
 *
 * Note: Set #if 1 to #if 0 to disable all packet dumping
 **************************************************************************************** */
/**
 * @brief Dump Ethernet header only (original behavior)
 */
static void dump_ethernet_only(const char *msg, unsigned char *buf, int buflen)
{
  syslog(LOG_INFO, "WLAN: %s buf %p %d bytes\n", msg, buf, buflen);
  syslog(LOG_INFO, "      %02x:%02x:%02x:%02x:%02x:%02x "
                   "%02x:%02x:%02x:%02x:%02x:%02x %02x%02x\n",
         buf[0], buf[1], buf[2], buf[3], buf[4], buf[5],
         buf[6], buf[7], buf[8], buf[9], buf[10], buf[11],
         buf[12], buf[13]);
}

/* Forward declarations */
static void dump_tcp(const char *msg, struct eth_hdr_s *eth,
                     struct ipv4_hdr_s *ip, int buflen);
static void dump_udp(const char *msg, struct eth_hdr_s *eth,
                     struct ipv4_hdr_s *ip, int buflen);

static void dump_ethhdr(const char *msg,
                        unsigned char *buf,
                        int buflen)
{
  struct eth_hdr_s *eth;
  struct ipv4_hdr_s *ip;

  /* Validate minimum packet length for IP parsing */
  if (buflen < ETH_HDRLEN + IPv4_HDRLEN) {
    goto eth_only;
  }

  eth = (struct eth_hdr_s *)buf;

  /* Check for IPv4 packets */
  if (ntohs(eth->type) != ETHTYPE_IP) {
    goto eth_only;
  }

  /* Parse IPv4 header */
  ip = (struct ipv4_hdr_s *)(buf + ETH_HDRLEN);

  /* Dispatch based on IP protocol */
  switch (ip->proto) {
    case IP_PROTO_TCP:
      dump_tcp(msg, eth, ip, buflen);
      break;

    case IP_PROTO_UDP:
      dump_udp(msg, eth, ip, buflen);
      break;

    default:
      /* Other protocols (ICMP, IGMP, etc.) use Ethernet-only dump */
      goto eth_only;
  }

  return;

eth_only:
  dump_ethernet_only(msg, buf, buflen);
}

/**
 * @brief Dump TCP packet information
 * @param msg Direction message ("TX" or "TX RAW")
 * @param eth Ethernet header pointer
 * @param ip IPv4 header pointer
 * @param buflen Total packet length
 */
static void dump_tcp(const char *msg,
                     struct eth_hdr_s *eth,
                     struct ipv4_hdr_s *ip,
                     int buflen)
{
  struct tcp_hdr_s *tcp;
  uint32_t seq, ack;
  uint16_t src_port, dest_port;
  uint16_t total_len, payload_len;

  /* Validate packet length */
  if (buflen < ETH_HDRLEN + IPv4_HDRLEN + TCP_HDRLEN) {
    goto eth_only;
  }

  tcp = (struct tcp_hdr_s *)((uint8_t *)ip + IPv4_HDRLEN);

  /* Extract and convert fields */
  src_port = ntohs(tcp->srcport);
  dest_port = ntohs(tcp->destport);
  /* TCP sequence numbers are stored as byte arrays */
  seq = (uint32_t)tcp->seqno[0] << 24 | (uint32_t)tcp->seqno[1] << 16 |
        (uint32_t)tcp->seqno[2] << 8 | (uint32_t)tcp->seqno[3];
  ack = (uint32_t)tcp->ackno[0] << 24 | (uint32_t)tcp->ackno[1] << 16 |
        (uint32_t)tcp->ackno[2] << 8 | (uint32_t)tcp->ackno[3];

  /* Calculate payload length */
  total_len = (ip->len[0] << 8) | ip->len[1];
  payload_len = total_len - IPv4_HDRLEN - TCP_HDRLEN;

  /* Print TCP packet info */
  syslog(LOG_INFO, "WLAN: %s TCP %u.%u.%u.%u:%d -> %u.%u.%u.%u:%d, "
                   "Seq:%lu, Ack:%lu, Payload:%d\n",
         msg,
         (uint8_t)(ntohs(ip->srcipaddr[0]) >> 8), (uint8_t)ntohs(ip->srcipaddr[0]),
         (uint8_t)(ntohs(ip->srcipaddr[1]) >> 8), (uint8_t)ntohs(ip->srcipaddr[1]),
         src_port,
         (uint8_t)(ntohs(ip->destipaddr[0]) >> 8), (uint8_t)ntohs(ip->destipaddr[0]),
         (uint8_t)(ntohs(ip->destipaddr[1]) >> 8), (uint8_t)ntohs(ip->destipaddr[1]),
         dest_port,
         (unsigned long)seq, (unsigned long)ack, payload_len);

  return;

eth_only:
  dump_ethernet_only(msg, (uint8_t *)eth, buflen);
}

/**
 * @brief Dump UDP packet information
 * @param msg Direction message ("TX" or "TX RAW")
 * @param eth Ethernet header pointer
 * @param ip IPv4 header pointer
 * @param buflen Total packet length
 */
static void dump_udp(const char *msg,
                     struct eth_hdr_s *eth,
                     struct ipv4_hdr_s *ip,
                     int buflen)
{
  struct udp_hdr_s *udp;
  uint16_t src_port, dest_port;
  uint16_t payload_len;

  /* Validate packet length */
  if (buflen < ETH_HDRLEN + IPv4_HDRLEN + UDP_HDRLEN) {
    goto eth_only;
  }

  udp = (struct udp_hdr_s *)((uint8_t *)ip + IPv4_HDRLEN);

  /* Extract and convert fields */
  src_port = ntohs(udp->srcport);
  dest_port = ntohs(udp->destport);
  payload_len = ntohs(udp->udplen) - UDP_HDRLEN;

  /* Print UDP packet info */
  syslog(LOG_INFO, "WLAN: %s UDP %u.%u.%u.%u:%d -> %u.%u.%u.%u:%d, "
                   "Payload:%d\n",
         msg,
         (uint8_t)(ntohs(ip->srcipaddr[0]) >> 8), (uint8_t)ntohs(ip->srcipaddr[0]),
         (uint8_t)(ntohs(ip->srcipaddr[1]) >> 8), (uint8_t)ntohs(ip->srcipaddr[1]),
         src_port,
         (uint8_t)(ntohs(ip->destipaddr[0]) >> 8), (uint8_t)ntohs(ip->destipaddr[0]),
         (uint8_t)(ntohs(ip->destipaddr[1]) >> 8), (uint8_t)ntohs(ip->destipaddr[1]),
         dest_port,
         payload_len);

  return;

eth_only:
  dump_ethernet_only(msg, (uint8_t *)eth, buflen);
}
#else
#define dump_ethhdr(msg, buf, buflen)
#endif

/**
 ****************************************************************************************
 * @brief Input packet from wl80211 MAC layer
 *
 * This function processes incoming packets from the wl80211 MAC layer and
 * forwards them to the NuttX networking stack.
 *
 * @param[in] vif       Virtual interface type (STA/AP)
 * @param[in] rxhdr     Pointer to receive header
 * @param[in] buf       Packet data buffer
 * @param[in] frm_len   Frame length
 * @param[in] status    Receive status flags
 ****************************************************************************************
 */
void wl80211_tcpip_input(enum wl80211_vif_type vif, void *rxhdr, void *buf, uint32_t frm_len, uint32_t status)
{
  struct eth_hdr_s *eth = buf;
  void *eth_buf = buf;
  UNUSED(vif);
  UNUSED(status);

  DEBUGASSERT(buf != NULL && frm_len > 0);

  switch (htons(eth->type))
    {
      case ETHTYPE_IP:
      case ETHTYPE_IP6:
      case ETHTYPE_ARP:
        if (g_forward_cb)
          eth_buf = g_forward_cb(rxhdr, buf, frm_len, (void *)wl80211_mac_rx_free);

        if (buf)
          g_rx_cb(rxhdr, eth_buf, frm_len,
                  g_forward_cb ? NULL : (void *)wl80211_mac_rx_free);
        break;

      default:
        wl80211_mac_rx_free(rxhdr);
    }

  return;
}

/**
 ****************************************************************************************
 * @brief Output packet through wl80211 interface
 *
 * This function handles packet transmission through the WiFi interface using
 * NuttX IOB buffers. Unlike lwip which uses reference counting, NuttX IOB
 * uses a copy-based approach or callback-based cleanup.
 *
 * @param[in] net_if  Pointer to network interface
 * @param[in] buf     Pointer to packet buffer
 *
 * @return OK on success, error code otherwise
 ****************************************************************************************
 */
int wl80211_output(net_buf_tx_t *buf)
{
  struct wl80211_tx_header *txhdr;
  struct iob_s *iob = (struct iob_s *)buf;
  struct iob_s *pkt;
  struct iovec txseg[TX_PBD_CNT];
  int seg_cnt;
  int idx;
  uint16_t length;
  int ret;

  DEBUGASSERT(buf != NULL);

  if (!iob)
    {
      wlerr("Invalid IOB buffer\n");
      return ERROR;
    }

  dump_ethhdr("TX", IOB_DATA(iob) - ETH_HDRLEN, iob->io_pktlen + ETH_HDRLEN);

  length = iob->io_pktlen + ETH_HDRLEN;

  /* Setup first segment */

  txseg[0].iov_base = (void *)(IOB_DATA(iob) - ETH_HDRLEN);
  txseg[0].iov_len = iob->io_len + ETH_HDRLEN;
  length -= txseg[0].iov_len;

  /* Get pointer to reserved headroom */

  txhdr = (void *)ALIGN4_HI((uint32_t)&iob->io_data);
  DEBUGASSERT(txhdr && (txseg[0].iov_base >= (void *)((uint32_t)txhdr + CONFIG_NET_LL_GUARDSIZE - ETH_HDRLEN)));

  /* Process additional segments */

  pkt = iob->io_flink;
  idx = 1;
  while (length && pkt && (idx < TX_PBD_CNT))
    {
      txseg[idx].iov_base = (void *)IOB_DATA(pkt);
      txseg[idx].iov_len = pkt->io_len;
      length -= txseg[idx].iov_len;
      idx++;
      pkt = pkt->io_flink;
    }

  seg_cnt = idx;

  DEBUGASSERT(length == 0);

  /* Transmit packet through MAC layer.
   * The MAC layer will call the provided callback to free the IOB buffer
   * after transmission is complete.
   */

  ret = wl80211_mac_tx(WL80211_VIF_STA, txhdr, 0, txseg, seg_cnt,
                       (void (*)(void *))net_buf_tx_free, iob);

  if (ret != 0)
    {
      net_buf_tx_free((net_buf_tx_t *)iob);
      wlerr("MAC transmission failed\n");
      return ERROR;
    }

  return OK;
}

/**
 ****************************************************************************************
 * @brief Output raw packet through wl80211 interface
 *
 * This function handles raw packet transmission (EAPOL, management frames)
 * through the WiFi interface.
 *
 * @param[in] vif       Virtual interface type (STA/AP)
 * @param[in] buffer    Packet data buffer
 * @param[in] len       Packet length
 * @param[in] flags     Transmission flags
 * @param[in] cb        Completion callback
 * @param[in] opaque    Opaque data for callback
 *
 * @return 0 on success, error code otherwise
 ****************************************************************************************
 */

int wl80211_output_raw(enum wl80211_vif_type vif, void *buffer, uint16_t len, unsigned int flags,
                       void (*cb)(void *), void *opaque)
{
  struct iob_s *iob;
  struct iob_s *pkt;
  struct iovec txseg[TX_PBD_CNT];
  uint16_t length;
  int seg_cnt = TX_PBD_CNT;
  int idx;
  void *txhdr;
  int ret;
  wl80211_raw_free_data_t *free_data;

  DEBUGASSERT(buffer != NULL && len > 0);

  dump_ethhdr("TX RAW", buffer, len);

  /* Allocate IOB buffer for raw transmission */
  iob = (struct iob_s *)net_buf_tx_alloc(len);
  if (!iob)
    {
      wlerr("Failed to allocate raw transmit buffer\n");
      return -1;
    }

  /* Allocate wrapper data structure to ensure both callback and IOB are handled */
  free_data = (wl80211_raw_free_data_t *)malloc(sizeof(wl80211_raw_free_data_t));
  if (!free_data)
    {
      wlerr("Failed to allocate free data structure\n");
      net_buf_tx_free((net_buf_tx_t *)iob);
      return -1;
    }

  /* Copy raw data to IOB buffer */
  if (iob_trycopyin(iob, buffer, len, -ETH_HDRLEN, false) != len)
    {
      wlerr("Failed to copy raw packet data, len=%u\n", len);
      net_buf_tx_free((net_buf_tx_t *)iob);
      free(free_data);
      return -2;
    }

  /* Initialize the wrapper data structure with callback and buffer information */
  free_data->iob = (net_buf_tx_t *)iob;
  free_data->cb = cb;
  free_data->opaque = opaque;

  /* Process buffer information directly (was net_buf_tx_info logic) */
  length = iob->io_pktlen + ETH_HDRLEN;

  /* Setup first segment */
  txseg[0].iov_base = (void *)(IOB_DATA(iob) - ETH_HDRLEN);
  txseg[0].iov_len = iob->io_len + ETH_HDRLEN;
  length -= txseg[0].iov_len;

  /* Get pointer to reserved headroom */
  txhdr = (void *)ALIGN4_HI((uint32_t)&iob->io_data);
  DEBUGASSERT(txhdr && (txseg[0].iov_base >= (void *)((uint32_t)txhdr + CONFIG_NET_LL_GUARDSIZE - ETH_HDRLEN)));

  /* Process additional segments */
  pkt = iob->io_flink;
  idx = 1;
  while (length && pkt && (idx < TX_PBD_CNT))
    {
      txseg[idx].iov_base = (void *)IOB_DATA(pkt);
      txseg[idx].iov_len = pkt->io_len;
      length -= txseg[idx].iov_len;
      idx++;
      pkt = pkt->io_flink;
    }

  seg_cnt = idx;

  if (length != 0)
    {
      wlerr("Not enough segments for complete raw packet\n");
      net_buf_tx_free((net_buf_tx_t *)iob);
      free(free_data);
      return -3;
    }

  if (!txhdr)
    {
      wlerr("Failed to get raw buffer information\n");
      net_buf_tx_free((net_buf_tx_t *)iob);
      free(free_data);
      return -3;
    }

  /* Transmit raw packet through MAC layer.
   * Use wrapper callback to ensure both the user callback is called
   * and the IOB buffer is properly freed.
   */
  ret = wl80211_mac_tx(vif, txhdr, flags, txseg, seg_cnt,
                       wl80211_raw_free_wrapper, free_data);

  if (ret != 0)
    {
      wlerr("Raw MAC transmission failed\n");
      /* On transmission failure, clean up resources manually */
      net_buf_tx_free((net_buf_tx_t *)iob);
      free(free_data);
      return -3;
    }

  return 0;
}

/**
 ****************************************************************************************
 * @brief Initialize wl80211 station interface
 *
 * This function initializes the wl80211 network interface for station mode
 * operation in NuttX.
 *
 * @return 0 on success, error code otherwise
 ****************************************************************************************
 */
int wl80211_sta_init(void)
{
  struct wl80211_netif *netif;
  irqstate_t flags;
  int ret;

  /* Allocate network interface structure */
  netif = (struct wl80211_netif *)malloc(sizeof(struct wl80211_netif));
  if (!netif)
    {
      wlerr("Failed to allocate network interface\n");
      return ERROR;
    }

  /* Initialize network interface */
  memset(netif, 0, sizeof(struct wl80211_netif));

  /* Set interface name */
  netif->num = g_netif_list ? g_netif_list->num + 1 : 0;

  /* Set virtual interface - use pointer to MAC layer vif array entry */
  netif->vif = &wl80211_mac_vif[WL80211_VIF_STA];

  /* Add to global interface list */
  flags = enter_critical_section();
  netif->next = g_netif_list;
  g_netif_list = netif;
  leave_critical_section(flags);

  return 0;
}

/**
 ****************************************************************************************
 * @brief Initialize wl80211 with NuttX native network stack
 *
 * This function initializes the wl80211 interface for NuttX native networking.
 * It serves as adapter for wl80211 stack which expects lwip initialization.
 *
 * @return 0 on success, error code otherwise
 ****************************************************************************************
 */
int wl80211_lwip_init(void)
{
  /* For NuttX native network stack, this simply calls wl80211_sta_init */
  return wl80211_sta_init();
}

/* SYSTEM API IMPLEMENTATIONS */

/**
 * ****************************************************************************************
 * @brief Hardware entropy poll function for mbedTLS
 * @param data       Pointer to user data (not used)
 * @param output     Pointer to output
 * @param len        Length of output
 * @param olen       Pointer to output length
 * @return 0 on success
 * ****************************************************************************************
 */
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
int mbedtls_hardware_poll(void *data,
                          unsigned char *output,
                          size_t len,
                          size_t *olen)
{
  irqstate_t flags;
  flags = enter_critical_section();
  bflb_trng_readlen(output, len);
  leave_critical_section(flags);

  *olen = len;

  return (0);
}
#endif
#endif /* MBEDTLS_ENTROPY_C */

/**
 ****************************************************************************************
 * @brief System log function for wl80211
 *
 * This function logs messages if the WiFi log flag is enabled.
 *
 * @param[in] fmt  Format string
 * @param[in] ...  Variable arguments
 ****************************************************************************************
 */

extern void *__attribute__((weak)) _wifi_log_flag;

void wifi_syslog(int priority, const char *fmt, ...)
{
  if (&_wifi_log_flag)
    {
      va_list arg;

      va_start(arg, fmt);
      vsyslog(LOG_DEBUG, fmt, arg);
      va_end(arg);
    }
}
