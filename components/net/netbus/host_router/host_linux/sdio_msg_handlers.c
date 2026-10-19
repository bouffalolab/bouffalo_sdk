#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include "main.h"
#include "sdio_msg_handlers.h"
#include "bl_sdio_eth.h"
#include "bl_sdio.h"
#include "sdio_msgs.h"
#include "ctl_port.h"
#include "tty.h"

static int bl_send_simple_rnm_cmd_sync(struct bl_eth_device *dev, uint16_t cmd_id)
{
    uint16_t tx_len;
    bl602_sdio_msg_t *sdio_msg;
    rnm_base_msg_t cmd = {
        .cmd = cmd_id,
    };
    tx_len = sizeof(*sdio_msg) + sizeof(cmd);

    sdio_msg = (bl602_sdio_msg_t *)dev->sdiocmd_buf;
    sdio_msg->type = BF1B_SDIO_MSG_TYPE_CMD;
    sdio_msg->len = tx_len;
    memcpy(sdio_msg->payload, &cmd, sizeof(cmd));

    if (bl_sdio_write(dev, sdio_msg, tx_len))
        return -1;
    return 0;
}

int bl_eth_get_mac_addr(struct bl_eth_device *dev)
{
    int i;
    int ret;
    u8 mac_addr[6] = { 0 };

    for (i = 0; i < ETH_ALEN; ++i) {
        ret = bl_read_reg(dev, BL_WIFI_MAC_ADDR_OFFSET + i, &mac_addr[i]);
        if (ret) {
            break;
        }
    }

    memcpy(dev->sta_mac, mac_addr, ETH_ALEN);
    // XXX
    /* mac_addr[5] ^= 1; */
    memcpy(dev->ap_mac, mac_addr, ETH_ALEN);

    return 0;
}

static int push_cmd(struct bl_eth_device *dev, const void *payload, size_t len)
{
    struct sk_buff *skb;
    bl602_sdio_msg_t *sdio_msg;
    const int smsg_len = sizeof(*sdio_msg) + len;

    if (!dev) {
        return -1;
    }

    if (!(skb = dev_alloc_skb(smsg_len))) {
        return -1;
    }

    sdio_msg = (bl602_sdio_msg_t *)skb_put(skb, smsg_len);
    sdio_msg->type = BF1B_SDIO_MSG_TYPE_CMD;
    sdio_msg->len = smsg_len;
    memcpy(sdio_msg->payload, payload, len);

    skb_queue_tail(&dev->cmd_list, skb);
    queue_work(dev->txworkqueue, &dev->tx_work);

    return 0;
}

int send_tty_frame(struct bl_eth_device *dev, uint16_t tty_id, const void *data, size_t len)
{
    uint16_t tx_len;
    const uint16_t subtype = BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT;
    bl602_sdio_msg_t *msg = (bl602_sdio_msg_t *)dev->sdiocmd_buf;
    sdiowifi_ext_frame_tty_msg_t *tty_msg;

    tx_len = sizeof(*msg) + 2 + sizeof(*tty_msg) + len;
    if (tx_len > 2040) {
        pr_err("SDIO tty frame msg too long");
        return -1;
    }
    msg->type = BF1B_SDIO_MSG_TYPE_ETH_FRAME;
    msg->len = tx_len;
    msg->payload[0] = subtype & 0xff;
    msg->payload[1] = (subtype >> 8) & 0xff;
    tty_msg = (sdiowifi_ext_frame_tty_msg_t *)(msg->payload + 2);
    tty_msg->hdr.type = SDIOWIFI_EXT_FRAME_TYPE_TTY;
    tty_msg->tty_id = tty_id;
    tty_msg->type = SDIOWIFI_EXT_TTY_DATA;
    memcpy(tty_msg->data, data, len);

    bl_sdio_write(dev, dev->sdiocmd_buf, tx_len);
    return 0;
}

int send_hello(struct bl_eth_device *dev)
{
    return bl_send_simple_rnm_cmd_sync(dev, BF1B_CMD_HELLO);
}

int push_host2device_msg(const uint8_t *payload, size_t len)
{
    int ret;

    mutex_lock(&gl_dev.mutex);
    ret = push_cmd(gl_dev.eth_dev, payload, len);
    mutex_unlock(&gl_dev.mutex);

    return ret;
}

// TODO: optimise me
int send_eth_frame(struct bl_eth_device *dev, struct sk_buff *skb)
{
    uint16_t tx_len;
    void *cp_dst;
    uint16_t subtype = get_tx_eth_frame_subtype(dev);
    bl602_sdio_msg_t *msg = (bl602_sdio_msg_t *)dev->sdiocmd_buf;

    tx_len = sizeof(*msg) + 2 + skb->len; // T, L, SUBTYPE, V
    if (tx_len > 2000) {
        printk("SDIO eth frame msg too long");
        return -1;
    }
    msg->type = BF1B_SDIO_MSG_TYPE_ETH_FRAME;
    msg->len = tx_len;
    msg->payload[0] = subtype & 0xff;
    msg->payload[1] = (subtype >> 8) & 0xff;
    cp_dst = msg->payload + 2;
    memcpy(cp_dst, skb->data, skb->len);

    bl_sdio_write(dev, dev->sdiocmd_buf, tx_len);
    return 0;
}

static void handle_eth_frame_rx(struct bl_eth_device *dev, const uint16_t subtype, const void *eth_hdr, const void *frame_p1, const uint16_t frame_len1, const void *frame_p2, const uint16_t frame_len2)
{
    struct sk_buff *skb;
    size_t frame_len = frame_len1 + frame_len2 + sizeof(struct eth_hdr);
    void *p;

    /* if (!(subtype == get_rx_eth_frame_subtype(dev))) { */
    /*     return; */
    /* } */
    skb = dev_alloc_skb(frame_len);
    if (!skb) {
        printk("alloc skb failed\n");
        return;
    }
    skb_put(skb, frame_len);
    p = skb->data;
    memcpy(p, eth_hdr, sizeof(struct eth_hdr));
    p += sizeof(struct eth_hdr);
    memcpy(p, frame_p1, frame_len1);
    p += frame_len1;
    if (frame_p2) {
        memcpy(p, frame_p2, frame_len2);
    }
    skb->dev = dev->net;
    skb->protocol = eth_type_trans(skb, dev->net);
    /* skb->ip_summed = CHECKSUM_UNNECESSARY; */
    dev->stats.rx_packets++;
    dev->stats.rx_bytes += skb->len;

    netif_rx(skb);
}

static void handle_ext_frame(struct bl_eth_device *dev, const uint8_t *frame, const uint16_t frame_len)
{
    sdiowifi_ext_frame_common_hdr_t *hdr;

    hdr = (sdiowifi_ext_frame_common_hdr_t *)frame;

    switch (hdr->type) {
    case SDIOWIFI_EXT_FRAME_TYPE_TTY:
        bl_handle_ext_frame_tty(dev, frame, frame_len);
        break;
    default:
        pr_warn("unknown ext type 0x%x\n", hdr->type);
        break;
    }
}

static void handle_eth_frame(struct bl_eth_device *dev, const uint16_t subtype, const void *eth_hdr, const void *frame_p1, const uint16_t frame_len1, const void *frame_p2, const uint16_t frame_len2, uint16_t len_orig)
{
    switch (subtype) {
    case BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_FROM_WIFI_RX:
        // fallthrough
    case BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_AP_FROM_WIFI_RX:
        handle_eth_frame_rx(dev, subtype, eth_hdr, frame_p1, frame_len1, frame_p2, frame_len2);
        break;
    case BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT:
        handle_ext_frame(dev, eth_hdr, len_orig);
        break;
    default:
        pr_warn("unknown eth subtype 0x%x\n", subtype);
        break;
    }
}

#define READ_U16_BE(x) ((((uint8_t *)(x))[0] << 8) | ((uint8_t *)(x))[1])

static void memcpy_two_parts(void *dst, const void *src1, size_t len1, const void *src2, size_t len2)
{
    if (src1) {
        memcpy(dst, src1, len1);
        dst += len1;
    }
    if (src2) {
        memcpy(dst, src2, len2);
    }
}

void dump_buf(const void *buf, const size_t len);
// pdu(s) passed in are sure to be consumed
static int handle_amsdu_pdus(struct bl_eth_device *dev, void *pdu1, void *pdu2)
{
    struct sdio_top_msg *msg;
    uint16_t msg_len;
    struct amsdu_subframe *subfrm;
    //void *p, *q;
    void *tail;
    void *first_part_tail;
    //uint16_t pdu_tot_len;
    uint16_t len_processed = 0;
    uint16_t first_part_len;
    struct amsdu_subframe subfrm_header_copy, *subfrm_header_ptr;
    struct eth_hdr eth_header;
    bool pdu2_consumed;

    if (pdu1 == NULL) {
        return -1;
    }

    msg = (struct sdio_top_msg *)pdu1;
    msg_len = msg->len_lsb | (msg->len_msb << 8);
    first_part_len = msg->first_part_len_lsb | (msg->first_part_len_msb << 8);
    first_part_tail = pdu1 + msg->pld_off + first_part_len;

    // sanity check
    if (unlikely(!(((msg->has_twin && pdu2 != NULL) || (!msg->has_twin && pdu2 == NULL))) ||
                 !(msg_len <= SD_RX_BUF_LEN * 2 - sizeof(struct sdio_top_msg)) ||
                 !(first_part_len <= SD_RX_BUF_LEN - sizeof(struct sdio_top_msg))
                 )) {
        printk("amsdu data sanity check failed\n");
        printk("msg->has_twin %d, pdu2 %p\n", msg->has_twin, pdu2);
        printk("msg_len %u\n", msg_len);
        printk("first-part_len %u\n", first_part_len);
        return -1;
    }

    subfrm = (struct amsdu_subframe *)((void *)msg + msg->pld_off);
    tail = first_part_tail;

    pdu2_consumed = false;
    while (1) {
        uint16_t msdu_len;
        uint16_t subfrm_len;
        void *eth_frm_pld[2] = { NULL };
        uint16_t eth_frm_pld_len[2] = { 0 };

        // if subframe header is across pdu
        if (unlikely((void *)subfrm + sizeof(struct amsdu_subframe) > tail)) {
            int len[2];
            if (unlikely(pdu2_consumed)) {
                /* printk("error: amsdu subframe header across pdu but pdu2 is consumed\n"); */
                return -1;
            }
            pdu2_consumed = true;
            len[0] = tail - (void *)subfrm;
            len[1] = sizeof(struct amsdu_subframe) - len[0];
            memcpy_two_parts(&subfrm_header_copy, subfrm, len[0], pdu2, len[1]);
            subfrm_header_ptr = &subfrm_header_copy;

            eth_frm_pld[0] = pdu2 + len[1];
            msdu_len = READ_U16_BE(&subfrm_header_ptr->length);
            if (msdu_len > 1508) {
#if 1
                printk("msdu_len %d too large\n", msdu_len);
                printk("dump:\n");
                printk("msg_len %u\n", msg_len);
                printk("len_processed %u\n", len_processed);
                printk("pdu1:\n");
                dump_buf((void *)msg, SD_RX_BUF_LEN);
                if (pdu2) {
                    printk("pdu2:\n");
                    dump_buf((void *)msg + SD_RX_BUF_LEN, SD_RX_BUF_LEN);
                }
#endif
                return -1;
            }
            eth_frm_pld_len[0] = msdu_len - sizeof(struct llc_snap);
            tail = pdu2 + msg_len - first_part_len;

            subfrm_len = sizeof(struct amsdu_subframe) - sizeof(struct llc_snap) + msdu_len;
            subfrm = (struct amsdu_subframe *)(eth_frm_pld[0] + eth_frm_pld_len[0] + (4 - (subfrm_len & 3)));
        } else {
            int len;
            subfrm_header_ptr = subfrm;
            msdu_len = READ_U16_BE(&subfrm_header_ptr->length);
            eth_frm_pld[0] = subfrm->payload;
            if (msdu_len > 1508) {
                pr_err("illegal msdu_len > 1508\n");
                return -1;
            }

            // if IP payload is across pdu
            len = sizeof(struct amsdu_subframe) - sizeof(struct llc_snap) + msdu_len;
            if ((void *)subfrm + len > tail) {
                if (unlikely(pdu2_consumed)) {
                    printk("error: amsdu subframe IP payload across pdu but pdu2 is consumed\n");
                    return -1;
                }
                pdu2_consumed = true;
                eth_frm_pld_len[0] = tail - eth_frm_pld[0];
                eth_frm_pld[1] = pdu2;
                eth_frm_pld_len[1] = msdu_len - sizeof(struct llc_snap) - eth_frm_pld_len[0];

                subfrm_len = sizeof(struct amsdu_subframe) - sizeof(struct llc_snap) + msdu_len;
                subfrm = (struct amsdu_subframe *)(eth_frm_pld[1] + eth_frm_pld_len[1] + (4 - (subfrm_len & 3)));
            } else {
                eth_frm_pld_len[0] = msdu_len - sizeof(struct llc_snap);
                subfrm_len = sizeof(struct amsdu_subframe) - sizeof(struct llc_snap) + msdu_len;
                subfrm = (struct amsdu_subframe *)(eth_frm_pld[0] + eth_frm_pld_len[0] + (4 - (subfrm_len & 3)));
            }
        }

        memcpy(eth_header.da, subfrm_header_ptr->da, 6);
        memcpy(eth_header.sa, subfrm_header_ptr->sa, 6);
        memcpy(&eth_header.proto, &subfrm_header_ptr->llc.proto_id, 2);

        /* printk("msdu frame length %u, da %pM, sa %pM, proto 0x%02x%02x\n", msdu_len, da, sa, proto[0], proto[1]); */
        // XXX type
        handle_eth_frame(dev, BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_FROM_WIFI_RX, &eth_header, eth_frm_pld[0], eth_frm_pld_len[0], eth_frm_pld[1], eth_frm_pld_len[1], 0);

        if (subfrm_len + len_processed >= msg_len) {
            /* printk("all processed. subfrm_len+len_processed %u vs msg_len %u\n", subfrm_len + len_processed, msg_len); */
            break;
        }
        subfrm_len = (subfrm_len + 3) & ~3; // pad len to multiples of 4 bytes
        len_processed += subfrm_len;
    }

    return 0;
}

void sdio_read_callback(void *cb_arg, const void *data_ptr, uint8_t num_of_pdu, const uint16_t data_len)
{
    struct bl_eth_device *dev = (struct bl_eth_device *)cb_arg;

    // kept values used
    static uint8_t saved_pdu[SD_RX_BUF_LEN];
    static bool saved_pdu_used = false;

    struct sdio_top_msg *msg;
    uint16_t type;
    uint16_t subtype;
    uint8_t *pld;
    uint16_t pld_len;

    void *dp1, *dp2;

    /* printk("sdio_read_callback: num_of_pdu %u\n", num_of_pdu); */
    while (num_of_pdu > 0) {
        if (saved_pdu_used) {
            /* printk("saved_pdu_used is true\n"); */
            saved_pdu_used = false;
            dp1 = &saved_pdu[0];
            dp2 = (void *)data_ptr;
            data_ptr += SD_RX_BUF_LEN;
            handle_amsdu_pdus(dev, dp1, dp2);
            num_of_pdu--;
        } else {
            /* printk("saved_pdu_used is false\n"); */
            msg = (struct sdio_top_msg *)data_ptr;

            type = (msg->type_msb << 8) | msg->type_lsb;
            pld = (uint8_t *)msg + msg->pld_off;
            pld_len = (msg->len_msb << 8) | msg->len_lsb;

            if (!msg->is_amsdu) {
                if (! (pld_len + msg->pld_off <= SD_RX_BUF_LEN) ) {
                    pr_err("pld length check failed! pld_len %u, pld_off %u\n",
                            pld_len, msg->pld_off);
                    return;
                }
                if (type == BF1B_SDIO_MSG_TYPE_ETH_FRAME) {
                    subtype = (msg->subtype_msb << 8) | msg->subtype_lsb;
                    handle_eth_frame(dev, subtype, pld, pld + sizeof(struct eth_hdr), pld_len - sizeof(struct eth_hdr), NULL, 0, pld_len);
                } else if (type == BF1B_SDIO_MSG_TYPE_CMD) {
                    bl_handle_cmd(dev, pld, pld_len);
                } else {
                    pr_err("Unknown SIDO_MSG type\n");
                }
                data_ptr += SD_RX_BUF_LEN;
                num_of_pdu--;
            } else {
                /* printk("got amsdu\n"); */
                // is amsdu
                if (msg->has_twin) {
                    /* printk("has twin\n"); */
                    if (num_of_pdu >= 2) {
                        /* printk("pdu left >= 2\n"); */
                        dp1 = (void *)data_ptr;
                        dp2 = (void *)data_ptr + SD_RX_BUF_LEN;
                        handle_amsdu_pdus(dev, dp1, dp2);
                        data_ptr += 2 * SD_RX_BUF_LEN;
                        num_of_pdu -= 2;
                    } else {
                        /* printk("pdu left == %d\n", num_of_pdu); */
                        memcpy(saved_pdu, data_ptr, SD_RX_BUF_LEN);
                        /* printk("set saved_pdu_used to true\n"); */
                        saved_pdu_used = true;
                        num_of_pdu--;
                    }
                } else {
                    /* printk("no has twin\n"); */
                    dp1 = (void *)data_ptr;
                    data_ptr += SD_RX_BUF_LEN;
                    num_of_pdu--;
                    handle_amsdu_pdus(dev, dp1, NULL);
                }
            }
        }
    }
}
