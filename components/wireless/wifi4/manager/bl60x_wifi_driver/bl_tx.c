/**
 ****************************************************************************************
 *
 * @file bl_tx.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <netif/etharp.h>
#include <bflb_os_private.h>

#include "bl_tx.h"
#include "bl_irqs.h"
#include "bl_utils.h"

#define RETRY_LIMIT_REACHED_BIT    (1 << 16)
#define LIFETIME_EXPIRED_BIT       (1 << 17)
#define FRAME_REPUSHABLE_PS_BIT    (1 << 19)
#define FRAME_REPUSHABLE_CHAN_BIT  (1 << 20)
#define FRAME_SUCCESSFUL_TX_BIT    (1 << 23)
#define DESC_DONE_TX_BIT           (1 << 31)
#define DESC_DONE_SW_TX_BIT        (1 << 30)

#define IS_BC_MC(byte)             ((byte) & 0x01)
#define BIT_VIF(idx)               (1 << (idx))
#define BIT_STA(idx)               (1 << (idx))

/* Trigger Flag */
static uint32_t tx_cntrl_sta_trigger = 0;
static uint32_t tx_cntrl_sta_trigger_pending = 0;
extern struct bl_hw wifi_hw;
int internel_cal_size_tx_hdr = sizeof(struct bl_txhdr);

void  bl_irq_handler(void);
#ifdef CFG_NETBUS_WIFI_ENABLE
int update_tx_pbuf_free_cnt_to_scratch_reg(void);
#endif

static void bl_tx_cntrl_purge_check(struct bl_sta *sta, uint8_t only_check)
{
    if (!utils_list_is_empty(&sta->pending_list) ||
        !utils_list_is_empty(&sta->waiting_list))
    {
        if (only_check)
        {
            /* Only check */
            bflb_os_printf("[TX] Have remaining packets when checking!\n\r");
        }
        else
        {
            /* Purging, trigger tx and let fw cfm to handle packets */
            bflb_os_enter_critical();
            bl_irq_handler();
            tx_cntrl_sta_trigger |= BIT_STA(sta->sta_idx);
            bflb_os_exit_critical();
        }
    }
}

static inline int bl_tx_cntrl_check_fc(struct bl_sta *sta)
{
    return ((0 == sta->fc_ps) &&
            (0 == wifi_hw.vif_table[sta->vif_idx].fc_chan));
}

static uint32_t bl_tx_cntrl_update_fc(struct ke_tx_fc *tx_fc_field)
{
    uint32_t sta_trigger = 0, fixed_sta_idx;

    /* FW fc  trigger: consider interface/sta_id/reason */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (tx_fc_field->vif_bits & BIT_VIF(i))
        {
            /* VIF_STA */
            if (i == 0)
            {
                fixed_sta_idx = wifi_hw.vif_table[BL_VIF_STA].fixed_sta_idx;
                if (tx_fc_field->sta.fc_chan)
                {
                    wifi_hw.vif_table[BL_VIF_STA].fc_chan = 0;
                }
                if (tx_fc_field->sta.fc_ps)
                {
                    wifi_hw.sta_table[fixed_sta_idx].fc_ps = 0;
                }
                sta_trigger |= BIT_STA(fixed_sta_idx);
            }
            /* VIF_AP (Broadcast + Remote STAs) */
            else
            {
                fixed_sta_idx = wifi_hw.vif_table[BL_VIF_AP].fixed_sta_idx;
                if (tx_fc_field->ap.fc_chan)
                {
                    wifi_hw.vif_table[BL_VIF_AP].fc_chan = 0;
                    sta_trigger |= BIT_STA(fixed_sta_idx);
                }
                sta_trigger |= tx_fc_field->ap.fc_ps_sta_bits;
            }
        }
    }

    return sta_trigger;
}

static int bl_tx_cntrl_get_sta_id(int is_sta, int is_broadcast, struct mac_addr *addr)
{
    struct bl_sta* sta;
    int ap_idx = -1, bcmc_sta_idx = -1, links_num;

    /* For STA */
    if (is_sta)
    {
        links_num = wifi_hw.vif_table[BL_VIF_STA].links_num;
        ap_idx    = wifi_hw.vif_table[BL_VIF_STA].fixed_sta_idx;
        sta       = &wifi_hw.sta_table[ap_idx];
        return ((links_num && sta->is_used) ? sta->sta_idx : -1);
    }

    /* For AP */
    links_num = wifi_hw.vif_table[BL_VIF_AP].links_num;
    if (0 == links_num)
    {
        return -1;
    }

    /* BC/MC */
    if (is_broadcast)
    {
        bcmc_sta_idx = wifi_hw.vif_table[BL_VIF_AP].fixed_sta_idx;
        sta          = &wifi_hw.sta_table[bcmc_sta_idx];
        return (sta->is_used ? sta->sta_idx : -1);
    }

    /* UC */
    for (int i = 0; i < NX_REMOTE_STA_STORE_MAX; i++)
    {
        /* Skip ap_idx and bcmc_sta_idx */
        if (i == bcmc_sta_idx || i == ap_idx)
        {
            continue;
        }

        sta = &wifi_hw.sta_table[i];
        if ((sta->is_used) &&
            (0 == memcmp(&sta->sta_addr, addr, sizeof(struct mac_addr))))
        {
            return i;
        }
    }

    return -1;
}

static int bl_tx_check_ret(uint8_t is_sta, uint8_t is_groupcast, uint32_t value)
{
    /* For handled packet, whether handled OK */
    if (is_sta) {
        if (value & FRAME_SUCCESSFUL_TX_BIT) {
            return 1;
        }
    } else {
        if ((!is_groupcast && (value & FRAME_SUCCESSFUL_TX_BIT)) ||
            (is_groupcast  && (value & DESC_DONE_TX_BIT)))
        {
            return 1;
        }
    }

    return 0;
}

static void bl_tx_push(struct bl_sta* sta, struct txdesc_host *txdesc_host, void *ptxbuf, struct bl_txhdr *txhdr)
{
    struct hostdesc *host = NULL;
    struct ethhdr *ethhdr = NULL;
    struct pbuf *p = NULL, *q = NULL;
    void* buf_ptr = NULL;
    uint16_t link_offset_len = 0, loop = 0;

    /* Get hostdesc */
    memset((void*)txdesc_host->pad_txdesc, 0, sizeof(txdesc_host->pad_txdesc));
    host = &(((struct txdesc_upper*)txdesc_host->pad_txdesc)->host);

#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
    struct txbuf_host *txbuf = (struct txbuf_host*)ptxbuf;
    if ((void*)txhdr->p != (void*)txbuf)
    {
        /* Get pbuf */
        p = (struct pbuf *)txhdr->p;
        /* Get ethernet header in pbuf */
        ethhdr = (struct ethhdr *)((uint8_t*)p->payload + PBUF_LINK_ENCAPSULATION_HLEN);
    }
    else
    {
        /* Get ethernet header in txbuf */
        ethhdr = (struct ethhdr *)((uint8_t*)txbuf->buf + PBUF_LINK_ENCAPSULATION_HLEN);
    }
#else
    /* Get pbuf */
    p = (struct pbuf *)txhdr->p;
    /* Get ethernet header in pbuf */
    ethhdr = (struct ethhdr *)((uint8_t*)p->payload + PBUF_LINK_ENCAPSULATION_HLEN);
#endif

    /* eth header is skipped in the header */
    link_offset_len = sizeof(struct ethhdr) + PBUF_LINK_ENCAPSULATION_HLEN;
    /* Fill-in the descriptor */
    memcpy(&host->eth_dest_addr, ethhdr->h_dest, ETH_ALEN);
    memcpy(&host->eth_src_addr, ethhdr->h_source, ETH_ALEN);
    host->ethertype   = ethhdr->h_proto;
    host->vif_type    = txhdr->vif_type;
    host->packet_len  = txhdr->len - link_offset_len;
    host->vif_idx     = wifi_hw.vif_table[sta->vif_idx].vif_idx;
    host->staid       = sta->sta_idx;
    host->tid         = (sta->qos ? 0 : 0xFF);
    host->packet_addr = (uint32_t)(0x11111111); // FIXME we use this magic for unvaild packet_addr
    host->flags       = 0;

    /* 
     * Buffer transfer and descriptor tranform
     */
#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
    /* Get right buffer pointer */
    struct bl_txhdr *new_bl_txhdr = NULL;
    uint16_t len = 0;
    void* ptr    = (void*)txbuf->buf;
    buf_ptr      = (void*)txbuf;

    /* Whether this packet is the first time */
    if ((void*)txhdr->p != (void*)txbuf)
    {
        /* Copy buf from pbuf to txdesc_host->eth_packet */
        uint16_t align_src = RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload);
        uint16_t align_dst = RWNX_HWTXHDR_ALIGN_PADS((uint32_t)ptr);
        new_bl_txhdr = (struct bl_txhdr*)((uint8_t*)ptr+align_dst);

        /* Copy txhdr */
        memcpy(ptr + align_dst, p->payload + align_src, sizeof(struct bl_txhdr));
        /* Copy packet */
        for (q = p; q != NULL; q = q->next)
        {
            if (0 == loop)
            {
                memcpy(ptr + PBUF_LINK_ENCAPSULATION_HLEN,
                       q->payload + PBUF_LINK_ENCAPSULATION_HLEN,
                       q->len - PBUF_LINK_ENCAPSULATION_HLEN);
            }
            else
            {
                memcpy(ptr + len, q->payload, q->len);
            }
            len += q->len;
            loop++;
        }

        memcpy(new_bl_txhdr, txhdr, sizeof(struct bl_txhdr));
        new_bl_txhdr->p = (uint32_t*)txbuf;

        /* Free pbuf, dont need it anymore */
        pbuf_free(p);
    }
    else
    {
        new_bl_txhdr = txhdr;
        len = new_bl_txhdr->len;
    }

    /* Packet to pbuf_chained_ptr */
    host->pbuf_chained_ptr[0] = (uint32_t)((uint8_t*)ptr + link_offset_len);
    host->pbuf_chained_len[0] = len - link_offset_len;
    host->status_addr = (uint32_t)(&(new_bl_txhdr->status));
#else
    /* Get right buffer pointer, NO Copy */
    buf_ptr = (void*)p;

    /* Pbuf to pbuf_chained_ptr */
    for (q = p; q != NULL; q = q->next)
    {
        if (0 == loop)
        {
            /* The first pbuf */
            host->pbuf_chained_ptr[loop] = (uint32_t)((uint8_t*)p->payload + link_offset_len);
            host->pbuf_chained_len[loop] = q->len - link_offset_len;
        }
        else
        {
            /* Chained pbuf after */
            host->pbuf_chained_ptr[loop] = (uint32_t)(q->payload);
            host->pbuf_chained_len[loop] = q->len;
        }
        loop++;
    }
    if (loop >= 4)
    {
        /* exceed the limit for pbuf chained */
        bflb_os_log_warn("[TX] [PBUF] Please fix for bigger chained pbuf, total_len %d\r\n",
                        p->tot_len);
    }
    else if (loop > 2)
    {
        bflb_os_log_warn("[TX] [LOOP] Chain Used %d\r\n", loop);
    }
    host->status_addr = (uint32_t)(&(txhdr->status));

    /* No pbuf_free */
#endif

    /* Give right buf ptr */
    host->pbuf_addr   = (uint32_t)buf_ptr;

    /* IPC push */
    ipc_host_txdesc_push(wifi_hw.ipc_env, buf_ptr);

#ifdef CFG_BL_STATISTIC
    wifi_hw.stats.cfm_balance++;
#endif
}

int bl_tx_cfm(void *pthis, void *host_id)
{
    struct bl_txhdr *txhdr = NULL;
    struct ethhdr     *eth = NULL;
    struct bl_sta     *sta = NULL;
    struct bl_vif     *vif = NULL;
    struct bl_tx_cfm custom_cfm;
    int ret = 0, links_num;
    uint32_t value;

    /* ATTENTION: About return value
     * 1. return ( -1), only for unhandled pakcet (status==0)
     * 2. return (>=0), handled packet, if there is a cb
     *    a. unicast, MUST FRAME_SUCCESSFUL_TX_BIT, return (1), other to c.
     *    b. groupcast, MUST DESC_DONE_TX_BIT, return (1), other to c.
     *    c. return (0)
     */
#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
    /* Host_id is txdesc_host->eth_packet */
    void *buf = ((struct txbuf_host*)host_id)->buf;
    txhdr = (struct bl_txhdr*)(buf + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)buf));
    eth = (struct ethhdr *)((uint32_t)buf + PBUF_LINK_ENCAPSULATION_HLEN);
#else
    /* Host_id is pbuf */
    struct pbuf *p = (struct pbuf*)host_id;
    txhdr = (struct bl_txhdr*)(((uint32_t)p->payload) + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload));
    eth = (struct ethhdr *)(((uint32_t)p->payload) + PBUF_LINK_ENCAPSULATION_HLEN);
#endif

    /* Read status in the TX control header */
    value = txhdr->status.value;
    if (value == 0) {
        bflb_os_printf("[TX] FW return status is NULL!!!\n\r");
    }

    /* Get tx result */
    ret = bl_tx_check_ret(txhdr->vif_type, IS_BC_MC(eth->h_dest[0]), value);

    /* Get sta id and links_num */
    sta = &wifi_hw.sta_table[txhdr->sta_id];
    links_num = wifi_hw.vif_table[sta->vif_idx].links_num;

    do {
        /* Try to repush it, if not handle OK, Check whether
         * can repush it (sta is still valid, ret,...)
         */
        if ((ret) || (txhdr->repush >= 3) || (links_num == 0) || (!sta->is_used))
        {
            break;
        }

        vif = &wifi_hw.vif_table[sta->vif_idx];

        /* If try repush, will cause more pakcet out-of-order but less loss
         * If not,        will cause less pakcet out-of-order but more loss
         */
        if (value & RETRY_LIMIT_REACHED_BIT)
        {
            tx_cntrl_sta_trigger_pending |= BIT_STA(sta->sta_idx);
        } else if (value & FRAME_REPUSHABLE_CHAN_BIT) {
            vif->fc_chan = 1;
        } else if (value & FRAME_REPUSHABLE_PS_BIT) {
            sta->fc_ps = 1;
        } else {
            break;
        }

        txhdr->repush++;
        utils_list_push_back(&sta->pending_list, &(txhdr->item));

        return 0;
    } while (0);

    /* Get customer cb param */
    custom_cfm = txhdr->custom_cfm;

    /* Release buf */
#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
    ipc_host_txbuf_free((struct txbuf_host *)host_id);
#else
    pbuf_free(p);
#endif

#ifdef CFG_NETBUS_WIFI_ENABLE
    update_tx_pbuf_free_cnt_to_scratch_reg();
#endif

    /* Re-trigger this sta tx */
    tx_cntrl_sta_trigger_pending |= BIT_STA(sta->sta_idx);

    /* Notify tx status */
    if (custom_cfm.cb) {
        custom_cfm.cb(custom_cfm.cb_arg, ret > 0);
    }

    return ret;
}

void bl_tx_try_flush(int param, struct ke_tx_fc *tx_fc_field)
{
    struct txdesc_host *txdesc_host = NULL;
    struct bl_txhdr          *txhdr = NULL;
    void                     *txbuf = NULL;
    struct bl_sta              *sta = NULL;
    uint32_t sta_trigger = 0;

    /* Whether need to manipulate flow control */
    if (param && tx_fc_field)
    {
        sta_trigger = bl_tx_cntrl_update_fc(tx_fc_field);
    }

    /* Get trigger flags and clear */
    sta_trigger |= tx_cntrl_sta_trigger_pending;
    tx_cntrl_sta_trigger_pending = 0;
    bflb_os_enter_critical();
    sta_trigger |= tx_cntrl_sta_trigger;
    tx_cntrl_sta_trigger = 0;
    bflb_os_exit_critical();

    // TX: Walking through all STAs
    for (uint8_t i = 0; i < NX_REMOTE_STA_STORE_MAX; i++)
    {
        /* Whether has trigger */
        if (!sta_trigger)
        {
            break;
        }

        /* whether can TX, only check flow control */
        sta = &wifi_hw.sta_table[i];
        if (!((sta_trigger & BIT_STA(i)) && bl_tx_cntrl_check_fc(sta)))
        {
            continue;
        }

        if (!utils_list_is_empty(&sta->pending_list))
        {
            while (1)
            {
                /* Get txdesc_host */
                txdesc_host = (struct txdesc_host *)ipc_host_txdesc_get(wifi_hw.ipc_env);
                if (!txdesc_host)
                {
                    bflb_os_log_warn("[TX] no more txdesc, wait!\n\r");
                    tx_cntrl_sta_trigger_pending |= BIT_STA(i);
                    break;
                }

                /* First get packet from pending list */
                txhdr = (struct bl_txhdr *)utils_list_pop_front(&sta->pending_list);
                if (!txhdr)
                {
                    tx_cntrl_sta_trigger_pending |= BIT_STA(i);
                    break;
                }

                /* Just push */
                bl_tx_push(sta, txdesc_host, (void*)txhdr->p, txhdr);
            }
        }

        if (!utils_list_is_empty(&sta->waiting_list))
        {
            while (1)
            {
                /* Get txdesc_host */
                txdesc_host = (struct txdesc_host *)ipc_host_txdesc_get(wifi_hw.ipc_env);
                if (!txdesc_host)
                {
                    bflb_os_log_warn("[TX] no more txdesc, wait!\n\r");
                    tx_cntrl_sta_trigger_pending |= BIT_STA(i);
                    break;
                }

#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
                /* Get txbuf_host */
                txbuf = (struct txbuf_host *)ipc_host_txbuf_get(wifi_hw.ipc_env);
                if (!txbuf)
                {
                    bflb_os_log_warn("[TX] no more txbuf, wait!\n\r");
                    tx_cntrl_sta_trigger_pending |= BIT_STA(i);
                    break;
                }
#endif
                /* Get packet */
                bflb_os_enter_critical();
                txhdr = (struct bl_txhdr *)utils_list_pop_front(&sta->waiting_list);
                bflb_os_exit_critical();
                if (!txhdr)
                {
#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
                    ipc_host_txbuf_free(txbuf);
#endif
                    break;
                }

                /* Packet push */
                bl_tx_push(sta, txdesc_host, (void*)txbuf, txhdr);
            }
        }
    }

    /* Reset trigger flags */
}

#ifdef CFG_NETBUS_WIFI_ENABLE
err_t bl_output(struct bl_hw *bl_hw, int is_sta, struct pbuf *p, struct bl_tx_cfm *custom_cfm, uint8_t from_local)
#else
err_t bl_output(struct bl_hw *bl_hw, int is_sta, struct pbuf *p, struct bl_tx_cfm *custom_cfm)
#endif
{
    struct ethhdr  *ethhdr;
    struct bl_txhdr *txhdr;
    struct bl_sta    *sta;
    int sta_id;
    uint16_t align_offset, link_desc_len;

    /* NULL protection */
    if (!bl_hw || !p)
    {
        bflb_os_printf("[TX] NULL parameters!\r\n");
        return ERR_CONN;
    }

    /* Get ethernet header first */
    ethhdr = (struct ethhdr *)(p->payload);

    /* Get sta_id */
    sta_id = bl_tx_cntrl_get_sta_id(is_sta, IS_BC_MC(ethhdr->h_dest[0]), (struct mac_addr*)ethhdr->h_dest);
    if (sta_id < 0)
    {
        bflb_os_printf("[TX] Cant find valid sta_id, drop! (is_sta: %d, is_bc_mc: %d, proto: %04x)\r\n",
                     is_sta, IS_BC_MC(ethhdr->h_dest[0]), ethhdr->h_proto);
        return ERR_IF;
    }
    sta = &wifi_hw.sta_table[sta_id];

    /* Make room in the header for tx */
    if (pbuf_header(p, PBUF_LINK_ENCAPSULATION_HLEN)) {
        bflb_os_printf("[TX] Reserve room failed for header\r\n");
        return ERR_IF;
    }

    /* Check reserved len for link 
     *           PBUF_LINK_ENCAPSULATION_HLEN (48)
     * | align (8) | struct bl_txhdr (24) | reserved (16) |
     */
    align_offset = RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload);
    link_desc_len = align_offset + sizeof(struct bl_txhdr) + 16;
    if (link_desc_len > PBUF_LINK_ENCAPSULATION_HLEN) {
        bflb_os_printf("[TX] link_header size is %ld vs header %u\r\n",
                     link_desc_len, PBUF_LINK_ENCAPSULATION_HLEN);
        return ERR_BUF;
    }

    /* Use aligned link_header */
    txhdr = (struct bl_txhdr *)(p->payload + align_offset);
    memset(txhdr, 0, sizeof(struct bl_txhdr));

    /* Fill-in txhdr */
    if (custom_cfm) {
        memcpy(&txhdr->custom_cfm, custom_cfm, sizeof(*custom_cfm));
    }
    txhdr->p        = (uint32_t*)p; // XXX pattention to this filed
    txhdr->len      = p->tot_len;
    txhdr->vif_type = is_sta;
    txhdr->sta_id   = sta_id;

    /* Ref this pbuf to avoid pbuf release */
#ifdef CFG_NETBUS_WIFI_ENABLE
    if (from_local) {
        pbuf_ref(p);
    }
#else
    pbuf_ref(p);
#endif

    /* Push packet into waiting_list */
    bflb_os_enter_critical();
    utils_list_push_back(&sta->waiting_list, &(txhdr->item));
    tx_cntrl_sta_trigger |= BIT_STA(sta_id);
    /* Whether trigger irq */
    if (bl_tx_cntrl_check_fc(sta))
    {
        bl_irq_handler();
    }
    bflb_os_exit_critical();

    return ERR_OK;
}

void bl_tx_cntrl_link_up(struct bl_sta *sta)
{
    /* Check pending_list and waiting_list */
    bl_tx_cntrl_purge_check(sta, 1);
}

void bl_tx_cntrl_link_down(struct bl_sta *sta)
{
    /* Purge pending_list and waiting_list */
    bl_tx_cntrl_purge_check(sta, 0);
}
