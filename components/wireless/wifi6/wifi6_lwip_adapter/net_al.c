/**
 ****************************************************************************************
 *
 * @file net_al.c
 *
 * @brief Implementation of the networking stack abstraction layer using LwIP.
 *
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rtos_al.h"
#include "net_al.h"
#include "platform_al.h"
#include "lwip/tcpip.h"
#include "lwip/etharp.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "lwip/netbuf.h"
#include "netif/ethernet.h"

#include "macsw.h"

#include <assert.h>

#include "wifi_pkt_hooks.h"

#ifdef CFG_IPV6
#include <lwip/ethip6.h>
#endif

#include "os.h"
#include "eloop_rtos.h"
#include "eloop.h"

#include "lwip/tcpip.h"
#include "fhost.h"
#include "tx_buffer_copy.h"
#ifdef CONFIG_MAT
#include "mat.h"
#include "pbuf_custom_ref.h"
#include "netif/bridgeif.h"
#endif

int fhost_tx_start(net_al_if_t net_if, net_al_tx_t net_buf,
                   cb_fhost_tx cfm_cb, void *cfm_cb_arg);

int fhost_tx_req_do(net_al_if_t net_if, net_al_tx_t net_buf, int type,
                    void *cfm_cb, void *cfm_cb_arg, void* buf_rx,
                    int no_cck);

void fhost_tx_release_buf(net_al_tx_t net_buf, uint32_t tx_status,
                          void *cfm_cb, void *cfm_cb_arg,
                          void *buf_rx );

void fhost_rx_buf_push(void *net_buf);

uint8_t fhost_tx_get_staid(void *vif, struct mac_addr *dst_addr, bool mgmt_frame);

int is_sta_netif(struct netif *nif);

int is_ap_netif(struct netif *nif);

/// start of shared memory section
extern uint32_t _sshram[];
/// end of shared memory section
extern uint32_t _eshram[];
extern uint32_t _heap_wifi_start[];
extern uint32_t _heap_wifi_size[];
#define TST_SHRAM_PTR(ptr) (((((uint32_t)(ptr)) < (uint32_t)_sshram) ||                    \
                             (((uint32_t)(ptr)) >= (uint32_t)_eshram)) &&                  \
                            (((uint32_t)(ptr)) < (uint32_t)_heap_wifi_start ||             \
                             ((uint32_t)(ptr)) >= ((uint32_t)_heap_wifi_start + (uint32_t)_heap_wifi_size)))

#define TST_SHEAP_PTR(ptr)  (((uint32_t)(ptr)) < (uint32_t)_heap_wifi_start ||             \
                             ((uint32_t)(ptr)) >= ((uint32_t)_heap_wifi_start + (uint32_t)_heap_wifi_size))

#define MACSW_NB_L2_FILTER 2

#define L2_SEND_SW_RETRIES 7

struct l2_filter_tag
{
    struct netif *net_if;
    uint16_t ethertype;
};

static struct l2_filter_tag l2_filter[MACSW_NB_L2_FILTER];
static rtos_semaphore l2_semaphore;
static rtos_mutex     l2_mutex;

#if PBUF_LINK_ENCAPSULATION_HLEN < NET_AL_TX_HEADROOM
#error "PBUF_LINK_ENCAPSULATION_HLEN must be at least NET_AL_TX_HEADROOM"
#endif

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/// Fake function used to detected too small link encapsulation header length
void p_buf_link_encapsulation_hlen_too_small(void);

/// Declaration of the LwIP checksum computation function
u16_t lwip_standard_chksum(const void *dataptr, int len);

/**
 ****************************************************************************************
 * @brief Callback used by the networking stack to push a buffer for transmission by the
 * WiFi interface.
 *
 * @param[in] net_if Pointer to the network interface on which the TX is done
 * @param[in] p_buf  Pointer to the buffer to transmit
 *
 * @return ERR_OK upon successful pushing of the buffer, ERR_BUF otherwise
 ****************************************************************************************
 */
static err_t net_if_output(struct netif *net_if, struct pbuf *p_buf)
{
    err_t status = ERR_BUF;

    if (bl_wifi_pkt_eth_output_hook) {
        bool is_sta = true; // FIXME
        p_buf = bl_wifi_pkt_eth_output_hook(is_sta, p_buf, bl_wifi_pkt_eth_output_hook_arg);
        if (p_buf == NULL) {
            // hook ate the packet
            return ERR_BUF;
        }
    }
    // Increase the ref count so that the buffer is not freed by the networking stack
    // until it is actually sent over the WiFi interface
    pbuf_ref(p_buf);

    // Push the buffer and verify the status
    if (netif_is_up(net_if) && fhost_tx_start(net_if, p_buf, NULL, NULL) == 0)
    {
        status = ERR_OK;
    }
    else
    {
        // Failed to push message to TX task, call pbuf_free only to decrease ref count
        pbuf_free(p_buf);
    }

    return (status);
}

#ifdef CONFIG_MAT
static err_t net_if_forward(struct netif *netif, struct pbuf *p)
{
    int ret;
    int tx_copy = 0;
    struct pbuf *out = NULL;

    if (!netif_is_up(netif))
        return ERR_IF;

    if (is_sta_netif(netif)) {
        ret = mat_handle_egress(netif, p, &out);
        switch (ret) {
        case MAT_ERR_INVAL:
        case MAT_ERR_STATUS:
            return ERR_IF;

        case MAT_ERR_MEM:
            return ERR_MEM;

        /* The outgoing buffer is already allocated. */
        case MAT_ERR_OK:
            break;
        }

        /*
         * Perhaps this buffer is unknown protocol for MAT and MAT just
         * leaves it alone.
         */
        if (!out)
            tx_copy = 1;
    } else if (is_ap_netif(netif)) {
        tx_copy = 1;
    } else {
        printf("WARN: invalid netif?\r\n");
        out = p;
    }

    if (tx_copy) {
#ifdef CONFIG_MAT_FULL_COPY
        out = pbuf_clone(PBUF_RAW_TX, PBUF_RAM, p);
        if (!out)
            return ERR_MEM;
/* CONFIG_MAT_FULL_COPY is not defined. */
#else
        struct pbuf *ref;
        u16_t eth_hdr_size = SIZEOF_ETH_HDR;

        /* Reserve L2 head room for wifi tx. */
        out = pbuf_alloc(PBUF_RAW_TX, eth_hdr_size, PBUF_RAM);
        if (!out)
            return ERR_MEM;
        /* Construct the ethernet header. */
        memcpy(out->payload, p->payload, eth_hdr_size);

        /* Create a reference for the following data. */
        ref = pbuf_custom_ref_create(p, eth_hdr_size, p->tot_len - eth_hdr_size);
        if (!ref) {
            pbuf_free(out);
            return ERR_MEM;
        }
        pbuf_cat(out, ref);
/* CONFIG_MAT_FULL_COPY */
#endif
    }

    ret = fhost_tx_start(netif, out, NULL, NULL);
    if (ret) {
        printf("%s fhost_tx_start failed, %d\r\n", __func__, ret);
        pbuf_free(out);
        return ERR_IF;
    }
    return ERR_OK;
}

static err_t net_if_linkoutput(struct netif *net_if, struct pbuf *p_buf)
{
    struct eth_hdr *ethhdr;

    /* Do not touch local output traffic. */
    ethhdr = (struct eth_hdr *)p_buf->payload;
    if (!netif_bridged(net_if) ||
        !memcmp(ethhdr->src.addr, net_if->hwaddr, ETH_HWADDR_LEN)) {
        return net_if_output(net_if, p_buf);
    }

    return net_if_forward(net_if, p_buf);
}
/* CONFIG_MAT */
#endif

/**
 ****************************************************************************************
 * @brief Callback used by the networking stack to setup the network interface.
 * This function should be passed as a parameter to netifapi_netif_add().
 *
 * @param[in] net_if Pointer to the network interface to setup
 * @param[in] p_buf  Pointer to the buffer to transmit
 *
 * @return ERR_OK upon successful setup of the interface, other status otherwise
 ****************************************************************************************
 */
static err_t net_if_init(struct netif *net_if)
{
    err_t status = ERR_OK;

    #if LWIP_NETIF_HOSTNAME
    {
        /* Initialize interface hostname */
        net_if->hostname = "wlan";
    }
    #endif /* LWIP_NETIF_HOSTNAME */

    net_if->name[ 0 ] = 'w';
    net_if->name[ 1 ] = 'l';

    net_if->output = etharp_output;

    #ifdef CFG_IPV6
    net_if->flags = NETIF_FLAG_LINK_UP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    net_if->output_ip6 = ethip6_output;
    #else
    net_if->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    #endif

    #if LWIP_IGMP
    net_if->flags |= NETIF_FLAG_IGMP;
    #endif
    net_if->hwaddr_len = ETHARP_HWADDR_LEN;
    // hwaddr is updated in net_if_add
    net_if->mtu = 1500;
#ifdef CONFIG_MAT
    net_if->linkoutput = net_if_linkoutput;
#else
    net_if->linkoutput = net_if_output;
#endif

    return status;
}

uint16_t net_ip_chksum(const void *dataptr, int len)
{
    // Simply call the LwIP function
    return lwip_standard_chksum(dataptr, len);
}

#ifdef CONFIG_ENABLE_IPV6_ADDR_CALLBACK
static void netif_nd6_callback(struct netif *netif, uint8_t ip_index)
{
    //platform_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP6);
}

void nd6_set_cb(struct netif *netif, void (*cb)(struct netif *netif, u8_t ip_index))
{
  if (netif != NULL) {
      netif->ipv6_addr_cb = cb;
  }
}
#endif

int net_if_add(net_al_if_t *net_if,
               const uint8_t *mac_addr,
               const uint32_t *ipaddr,
               const uint32_t *netmask,
               const uint32_t *gw,
               void *vif)
{
    err_t status;

    *net_if = (net_al_if_t)rtos_malloc(sizeof(inet_if_t));
    memset(*net_if, 0, sizeof(inet_if_t));
    status = netifapi_netif_add(*net_if,
                               (const ip4_addr_t *)ipaddr,
                               (const ip4_addr_t *)netmask,
                               (const ip4_addr_t *)gw,
                               vif,
                               net_if_init,
                               tcpip_input);

    // Init MAC addr here as we can't do it in net_if_init (without dereferencing vif)
    memcpy(((inet_if_t *)(*net_if))->hwaddr, mac_addr, ETHARP_HWADDR_LEN);

#ifdef CONFIG_ENABLE_IPV6_ADDR_CALLBACK
    nd6_set_cb(net_if, netif_nd6_callback);
#endif

    return (status == ERR_OK ? 0 : -1);
}

const uint8_t *net_if_get_mac_addr(net_al_if_t net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;
    return nif->hwaddr;
}

net_al_if_t net_if_find_from_name(const char *name)
{
    net_al_if_t net_if;

#if !LWIP_TCPIP_CORE_LOCKING
    #error To do add netif msg call
#endif
    LOCK_TCPIP_CORE();
    net_if = (net_al_if_t)netif_find(name);
    UNLOCK_TCPIP_CORE();

    return net_if;
}

int net_if_get_name(net_al_if_t net_if, char *buf, int len)
{
    inet_if_t *nif = (inet_if_t *)net_if;

    if (!nif) {
        if (buf && len) {
            buf[0] = '\0';
        }
        return -1;
    }

    if (len > 0)
        buf[0] = nif->name[0];
    if (len > 1)
        buf[1] = nif->name[1];
    if (len > 2)
        buf[2] = nif->num + '0';
    if ( len > 3)
        buf[3] = '\0';

    return 3;
}

void net_if_up_cb(net_al_if_t net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;
    netifapi_netif_set_up(nif);
}

void net_if_down_cb(net_al_if_t net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;
    netifapi_netif_set_down(nif);
}

static int net_if_input(net_al_rx_t net_buf, net_al_if_t net_if, void *addr, uint16_t len, net_buf_free_fn free_fn)
{
    struct pbuf *p;
    inet_if_t *nif = (inet_if_t *)net_if;
    inet_buf_rx_t *buf = net_buf;

    buf->custom_free_function = (pbuf_free_custom_fn)free_fn;
    p = pbuf_alloced_custom(PBUF_RAW, len, PBUF_REF | PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS, buf, addr, len);
    assert(p != NULL);

    /* MIOT616-238: Record the time when the last TCPIP packet was received */
    PLATFORM_HOOK(tcpip_rx);

    if (bl_wifi_pkt_eth_input_hook) {
        bool is_sta = is_sta_netif(nif);
        p = bl_wifi_pkt_eth_input_hook(is_sta, p, bl_wifi_pkt_eth_input_hook_arg);
        if (p == NULL) {
            // hook dropped the packet
            goto end;
        }
    }

#ifdef CONFIG_MAT
    if (netif_bridged(net_if) && is_sta_netif(net_if))
        mat_handle_ingress(nif, p);
#endif

    if (nif->input(p, nif))
    {
        free_fn(buf);
        return -1;
    }

    goto end; // In case of error that label end is defined but not used when bl_wifi_pkt_eth_input_hook is NULL
end:
    return 0;
}

void net_al_input(net_al_rx_t net_buf, void *payload,
                  net_al_if_t net_if,
                  uint16_t length,
                  uint8_t offset,
                  uint8_t skip_after_eth_hdr,
		  net_buf_free_fn free_fn)
{
        if (skip_after_eth_hdr != 0) {
            memcpy((char*)payload + skip_after_eth_hdr, payload, sizeof(struct mac_eth_hdr));
        }
        /* Drop my broadcast message forwarded by AP. */
        uint8_t from_us = 1;
        uint8_t *mac_eth_hdr = (uint8_t *)payload + offset;
        const uint8_t *src_addr = net_if_get_mac_addr(net_if);
        for (int i = 0; i < 6; i++) {
            if (mac_eth_hdr[6 + i] != src_addr[i]) {
                from_us = 0;
                break;
            }
        }
        if (from_us) {
            fhost_rx_buf_push(net_buf);
            return;
        }
        net_if_input(net_buf, net_if, (char*)payload + offset + skip_after_eth_hdr,
                     length - skip_after_eth_hdr, free_fn);
}

void *net_if_vif_info(net_al_if_t net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;
    return nif->state;
}

net_al_tx_t net_buf_tx_alloc(uint32_t length)
{
    struct pbuf *pbuf;

    pbuf = pbuf_alloc(PBUF_RAW_TX, length, PBUF_RAM);
    if (pbuf == NULL)
        return NULL;

    return pbuf;
}

net_al_tx_t net_buf_tx_alloc_fill(const uint8_t *frame, uint32_t length)
{
    struct pbuf *pbuf;

    pbuf = pbuf_alloc(PBUF_RAW_TX, length, PBUF_RAM);

    if (pbuf == NULL) {
        return NULL;
    }
    else {
        memcpy(pbuf->payload, frame, length);
        return pbuf;
    }
}

net_al_tx_t net_buf_tx_alloc_ref(uint32_t length)
{
    struct pbuf *pbuf;

    pbuf = pbuf_alloc(PBUF_RAW_TX, length, PBUF_REF);
    if (pbuf == NULL)
        return NULL;

    return pbuf;
}

bool net_buf_tx_all_shram(net_al_tx_t net_buf)
{
    bool all_shram = true;

    inet_buf_tx_t *p = (inet_buf_tx_t *)net_buf;

    for (; p; p = p->next) {
        if (TST_SHRAM_PTR(p->payload)) {
            all_shram = false;
            break;
        }
    }

    return all_shram;
}

void *net_buf_tx_info(net_al_tx_t *net_buf, uint16_t *tot_len, int *seg_cnt,
                      uint32_t seg_addr[], uint16_t seg_len[], uint32_t *headroom_len)
{
    int idx, seg_cnt_max = *seg_cnt;
    inet_buf_tx_t *buf = (inet_buf_tx_t *)net_buf;
    uint16_t length;

    void *headroom;
    uint32_t start_payload, end_payload;

    length = buf->tot_len;
    *tot_len = length;

    // Sanity check - the payload shall be in shared RAM
    assert(!TST_SHRAM_PTR(buf->payload));
    seg_addr[0] = (uint32_t)buf->payload;
    seg_len[0] = buf->len;
    length -= buf->len;
    start_payload = (uint32_t)buf->payload;

    // Get pointer to reserved headroom
    /* The pbuf structure and payload used by sdio are not in a contiguous ram,
     * we use the pbuf_header_force api just for adjusts the payload pointer
     * to reveal headers in the payload. The pbuf_header api used for pbuf structure
     * and payload are in a contiguous ram.
     */
    if (buf->type_internal & PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS) {
        if (pbuf_header(buf, PBUF_LINK_ENCAPSULATION_HLEN)) {
            // Sanity check - we shall have enough space in the buffer
            assert(0);
            return NULL;
        }
    } else {
        if (pbuf_header_force(buf, PBUF_LINK_ENCAPSULATION_HLEN)) {
            // Sanity check - we shall have enough space in the buffer
            assert(0);
            return NULL;
        }
    }
#define ALIGN4_HI(val) (((val)+3)&~3)
    headroom = (void *)ALIGN4_HI((uint32_t)buf->payload);
    end_payload = (uint32_t)headroom;
    if (headroom_len) {
        *headroom_len = end_payload - start_payload;
    }

    // Get info of extra segments if any
    buf = buf->next;
    idx = 1;
    while (length && buf && (idx < seg_cnt_max))
    {
        // Sanity check - the payload shall be in shared RAM
        assert(!TST_SHRAM_PTR(buf->payload));

        seg_addr[idx] = (uint32_t)buf->payload;
        seg_len[idx] = buf->len;
        length -= buf->len;
        idx++;
        buf = buf->next;
    }

    *seg_cnt = idx;
    if (length != 0)
    {
        // The complete buffer must be included in all the segments
        assert(0);
        return NULL;
    }

    return headroom;
}

void net_buf_tx_free(net_al_tx_t net_buf)
{
    inet_buf_tx_t *buf = (inet_buf_tx_t *)net_buf;
    // Free the buffer
    pbuf_free(buf);
}

void net_buf_tx_cat(net_al_tx_t net_buf1, net_al_tx_t net_buf2)
{
    inet_buf_tx_t *buf1 = (inet_buf_tx_t *)net_buf1;
    inet_buf_tx_t *buf2 = (inet_buf_tx_t *)net_buf2;
    pbuf_cat(buf1, buf2);
}

void net_buf_rx_free(net_al_rx_t net_buf)
{
    inet_buf_rx_t *buf = (inet_buf_rx_t *)net_buf;
    // Free the buffer
    pbuf_free(&buf->pbuf);
}

int net_al_tx_req(struct net_al_tx_req req)
{
    inet_buf_tx_t *ibuf = req.net_buf;
    net_al_if_t net_if = req.net_if;
    int type = req.type;
    void *cfm_cb = req.cfm_cb;
    void *cfm_cb_arg = req.cfm_cb_arg;
    void *buf_rx = req.buf_rx;
    int no_cck = req.no_cck;

    if (net_buf_tx_all_shram(ibuf)) {
        return fhost_tx_req_do(net_if, ibuf, type, cfm_cb, cfm_cb_arg, buf_rx, no_cck);
    }

#ifdef FHOST_BUFFER_COPY
    struct bc_tx_buf_tag *buf;
    // should be RXed to be forwarded buf
    buf = bc_tx_buf_alloc();
    if (buf) {
        struct pbuf *p = (struct pbuf *)&buf->c_pbuf;
        if (pbuf_copy(p, ibuf) != ERR_OK) {
            assert(0);
        }
        p->len = ibuf->tot_len;
        p->tot_len = ibuf->tot_len;

        // Since we copied net_buf to p, net_buf no more depends on buf_rx
        if (buf_rx) {
            fhost_rx_buf_push(buf_rx);
            buf_rx = NULL;
        }
        net_buf_tx_free(ibuf);
        return fhost_tx_req_do(net_if, (net_al_tx_t)p, type, cfm_cb, cfm_cb_arg, NULL, no_cck);
    } else {
        void *vif;
        uint8_t staid;
        struct mac_addr *da = (struct mac_addr *)ibuf->payload;

        vif = net_if_vif_info(net_if);
        staid = fhost_tx_get_staid(vif, da, false);
        if (staid == 0xff) {
            fhost_tx_release_buf(ibuf, 0, cfm_cb, cfm_cb_arg, buf_rx);
        } else {
            if (!(ibuf->type_internal & PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS &&
                CO_ALIGN4_LO((uintptr_t)ibuf->payload - sizeof(struct bc_tx_info_tag)) >= (uintptr_t)ibuf + sizeof(*ibuf))) {
                assert(0);
            }
            bc_tx_enqueue((struct bc_tx_info_tag *)CO_ALIGN4_LO((uintptr_t)ibuf->payload - sizeof(struct bc_tx_info_tag)), staid, req);
        }
    }
    return 0;
#else
    return -1;
#endif // FHOST_BUFFER_COPY
}

typedef void (*bl_custom_tx_callback_t)(void *cb_arg, bool tx_ok);

struct bl_custom_tx_cfm {
    bl_custom_tx_callback_t cb;
    void *cb_arg;
};

static void cfm_cb(uint32_t frame_id, bool acknowledged, void *arg)
{
    bl_custom_tx_callback_t cb = arg;
    if (cb) {
        cb(NULL, acknowledged);
    }
}

int bl_wifi_eth_tx(struct pbuf *p, bool is_sta, struct bl_custom_tx_cfm *custom_cfm)
{
    net_al_if_t *net_if;

    if (is_sta) {
        // FIXME
        net_if = fhost_env.vif[0].net_if;
    } else {
        net_if = fhost_env.vif[1].net_if;
    }

    pbuf_ref(p);

    return fhost_tx_start(net_if, p, cfm_cb, custom_cfm->cb);
}

void net_al_tx_cfm()
{
#ifdef FHOST_BUFFER_COPY
    bc_try_tx_queued_pkts();
#endif
}

void net_al_tx_do_sta_del(uint8_t sta_id, release_buf_cb release_buf)
{
#ifdef FHOST_BUFFER_COPY
    bc_tx_queue_release_sta(sta_id, release_buf);
#endif
}

void net_al_tx_init()
{
#ifdef FHOST_BUFFER_COPY
    bc_tx_init_tx_pool();
#endif
}

extern void fhost_rx_do_resend_req(void *buf, net_al_tx_t net_buf_tx, bool forward);
extern void fhost_rx_buf_push(void *);
void net_al_rx_resend(bool rx_buf_forward, void *buf, void *payload, int length,
                      int offset,
                      int mac_hdr_len)
{
    inet_buf_tx_t *net_buf_tx_1, *net_buf_tx_2;

    // Allocation of the 2 buffer Tx: Ethernet header + payload @ref Rx payload
    net_buf_tx_1 = net_buf_tx_alloc(mac_hdr_len);
    net_buf_tx_2 = net_buf_tx_alloc_ref(length - mac_hdr_len);

    if ((net_buf_tx_1 == NULL) || (net_buf_tx_2 == NULL))
    {
        if (net_buf_tx_1)
            net_buf_tx_free(net_buf_tx_1);
        if (net_buf_tx_2)
            net_buf_tx_free(net_buf_tx_2);

        if (!rx_buf_forward)
        {
            fhost_rx_buf_push(buf);
        }
    }
    else
    {
        // Update the ref of the payload
        net_buf_tx_2->payload = (uint8_t *)payload + mac_hdr_len + offset;

        // Copy the ethernet header from Rx
        memcpy(net_buf_tx_1->payload, (uint8_t*)payload + offset, mac_hdr_len);

        //concatenate the 2 buffers Tx
        net_buf_tx_cat(net_buf_tx_1, net_buf_tx_2);

        fhost_rx_do_resend_req(buf, (net_al_tx_t)net_buf_tx_1, rx_buf_forward);
    }
}

int net_init(void)
{
    int i;

    for (i = 0; i < MACSW_NB_L2_FILTER; i++)
    {
        l2_filter[i].net_if = NULL;
    }

    if (rtos_semaphore_create(&l2_semaphore, 1, 0))
    {
        assert(0);
    }

    if (rtos_mutex_create(&l2_mutex))
    {
        assert(0);
    }

    return 0;
}

static void net_l2_send_cfm(uint32_t frame_id, bool acknowledged, void *arg)
{
    if (arg)
        *((bool *)arg) = acknowledged;
    rtos_semaphore_signal(l2_semaphore, false);
}

int net_l2_send_do(net_al_if_t net_if, const uint8_t *data, int data_len, uint16_t ethertype,
                   const uint8_t *dst_addr, bool *ack)
{
    struct pbuf *pbuf;
    int res;
    inet_if_t *nif = (inet_if_t *)net_if;

    if (nif == NULL || data == NULL || data_len >= nif->mtu || !netif_is_up(nif))
        return -1;

    pbuf = pbuf_alloc(PBUF_LINK, data_len, PBUF_RAM);
    if (pbuf == NULL)
        return -1;

    memcpy(pbuf->payload, data, data_len);

    if (dst_addr)
    {
        // Need to add ethernet header as fhost_tx_start is called directly
        struct eth_hdr* ethhdr;
        if (pbuf_header(pbuf, SIZEOF_ETH_HDR))
        {
            pbuf_free(pbuf);
            return -1;
        }
        ethhdr = (struct eth_hdr*)pbuf->payload;
        ethhdr->type = htons(ethertype);
        memcpy(&ethhdr->dest, dst_addr, sizeof(struct eth_addr));
        memcpy(&ethhdr->src, nif->hwaddr, sizeof(struct eth_addr));
    }

    // Ensure no other thread will program a L2 transmission while this one is waiting
    // for its confirmation
    rtos_mutex_lock(l2_mutex);

    // In order to implement this function as blocking until the completion of the frame
    // transmission, directly call fhost_tx_start with a confirmation callback.
    res = fhost_tx_start((net_al_if_t)nif, pbuf, net_l2_send_cfm, ack);

    // Wait for the transmission completion
    rtos_semaphore_wait(l2_semaphore, -1);

    // Now new L2 transmissions are possible
    rtos_mutex_unlock(l2_mutex);

    return res;
}

int net_l2_send(net_al_if_t net_if, const uint8_t *data, int data_len, uint16_t ethertype,
                const uint8_t *dst_addr, bool *ack)
{
    size_t tries = L2_SEND_SW_RETRIES + 1;

    while (tries-- > 0) {
        bool ack_;

        if (net_l2_send_do(net_if, data, data_len, ethertype, dst_addr, &ack_)) {
            return -1;
        }
        if (ack_) {
            *ack = true;
            return 0;
        }
    }

    *ack = false;
    return 0;
}

int net_l2_socket_create(net_al_if_t net_if, uint16_t ethertype)
{
    struct l2_filter_tag *filter = NULL;
    int i;
    inet_if_t *nif = (inet_if_t *)net_if;

    /* First find free filter and check that socket for this ethertype/net_if couple
       doesn't already exists */
    for (i = 0; i < MACSW_NB_L2_FILTER; i++)
    {
        if ((l2_filter[i].net_if == nif) &&
            (l2_filter[i].ethertype == ethertype))
        {
            return -1;
        }
        else if ((filter == NULL) && (l2_filter[i].net_if == NULL))
        {
            filter = &l2_filter[i];
        }
    }

    if (!filter)
        return -1;

    /* Note: we create DGRAM socket here but in practice we don't care, net_eth_receive
       will use the socket as a L2 raw socket */
    filter->net_if = nif;
    filter->ethertype = ethertype;

    return 0;
}

int net_l2_socket_delete(void)
{
    int i;
    for (i = 0; i < MACSW_NB_L2_FILTER; i++)
    {
        if (l2_filter[i].net_if != NULL)
        {
            l2_filter[i].net_if = NULL;
            return 0;
        }
    }

    return -1;
}

__attribute__((weak)) err_t bridgeif_has_port(struct netif *bridgeif, struct netif *netif)
{
    return 0;
}

err_t net_eth_receive(struct pbuf *pbuf, struct netif *netif)
{
    struct l2_filter_tag *filter = NULL;
    struct eth_hdr* ethhdr = pbuf->payload;
    uint16_t ethertype = ntohs(ethhdr->type);
#if BL_ENABLE_PACKET_SOCKET
    struct netconn *conn;
    struct netbuf *buf;
#endif
    int i;

    for (i = 0; i < MACSW_NB_L2_FILTER; i++) {
        /*
         * For WPA EAPOL(Extensible Authentication Protocol over LAN) Debug.
         * printf("---> name:%s-%d, l2_filter[%d].ethertype:0x%04X, ethertype:0x%04X, netif:%p-%p\r\n",
         *      netif->name, netif->num, i, l2_filter[i].ethertype, ethertype, netif, l2_filter[i].net_if);
         */

        if ((l2_filter[i].ethertype == ethertype) &&
            ((l2_filter[i].net_if == netif) || (bridgeif_has_port(netif, l2_filter[i].net_if)))) {
            /*
             * For WPA EAPOL(Extensible Authentication Protocol over LAN) Debug.
             *  printf("===> name:%s-%d, l2_filter[%d].ethertype:0x%04X, ethertype:0x%04X, netif:%p-%p\r\n",
             *      netif->name, netif->num, i, l2_filter[i].ethertype, ethertype, netif, l2_filter[i].net_if);
             */
            filter = &l2_filter[i];
            break;
        }
    }

    if (!filter)
        return ERR_VAL;

    if (eloop_event_commit(ELOOP_EVT_WPA_L2_DATA, pbuf->payload, pbuf->tot_len)) {
        printf("failed to commit L2 data\r\n");
    }
    pbuf_free(pbuf);
    return ERR_OK;

/* this macro is never defined */
#if BL_ENABLE_PACKET_SOCKET
    buf = (struct netbuf *)memp_malloc(MEMP_NETBUF);
    if (buf == NULL)
    {
        return ERR_MEM;
    }

    buf->p = pbuf;
    buf->ptr = pbuf;
    conn = filter->conn;

    if (sys_mbox_trypost(&conn->recvmbox, buf) != ERR_OK)
    {
        netbuf_delete(buf);
        return ERR_OK;
    }
    else
    {
        #if LWIP_SO_RCVBUF
        SYS_ARCH_INC(conn->recv_avail, pbuf->tot_len);
        #endif /* LWIP_SO_RCVBUF */
        /* Register event with callback */
        API_EVENT(conn, NETCONN_EVT_RCVPLUS, pbuf->tot_len);
    }

    return ERR_OK;
#endif
}

void lock_net_tcpip_core(void)
{
    LOCK_TCPIP_CORE();
}

void unlock_net_tcpip_core(void)
{
    UNLOCK_TCPIP_CORE();
}
