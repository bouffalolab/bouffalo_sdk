#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include "main.h"
#include "usb_msg_handlers.h"
#include "bl_usb_eth.h"
#include "usb_msgs.h"
#include "ctl_port.h"

size_t build_simple_cmd_msg(void *buf, uint16_t cmd)
{
    usb_data_t *usb_hdr = buf;
    rnm_base_msg_t *rnm_msg = buf + sizeof(*usb_hdr);

    memset(rnm_msg, 0, sizeof(*rnm_msg));
    rnm_msg->cmd = cmd;

    memset(usb_hdr, 0, sizeof(*usb_hdr));
    usb_hdr->type = USBWIFI_DATA_TYPE_CMD;
    usb_hdr->length = sizeof(*rnm_msg);
    usb_hdr->payload_offset = sizeof(*usb_hdr);

    return sizeof(*usb_hdr) + sizeof(*rnm_msg);
}

int parse_get_mac_rsp_msg(struct bl_eth_device *dev, void *buf, int buf_len)
{
    usb_data_t *usb_hdr = buf;
    rnm_mac_addr_ind_msg_t *rsp = buf + sizeof(*usb_hdr);

    if (buf_len != sizeof(*usb_hdr) + sizeof(*rsp)) {
        return -1;
    }
    if (usb_hdr->type != USBWIFI_DATA_TYPE_CMD || usb_hdr->length != sizeof(*rsp)) {
        return -1;
    }
    if (rsp->hdr.cmd != BF1B_CMD_GET_MAC_ADDR || !(rsp->hdr.flags & RNM_MSG_FLAG_ACK)) {
        return -1;
    }
    memcpy(dev->sta_mac, rsp->sta_mac, ETH_ALEN);
    memcpy(dev->ap_mac, rsp->ap_mac, ETH_ALEN);

    return 0;
}

int push_host2device_msg(const uint8_t *payload, size_t len)
{
    struct bl_eth_device *dev;
    struct sk_buff *skb = NULL;
    int ret = 0;

    mutex_lock(&gl_dev.mutex);
    dev = gl_dev.eth_dev;
    if (!dev) {
        mutex_unlock(&gl_dev.mutex);
        return -1;
    }
    skb = alloc_skb(NEEDED_HEADROOM_LEN + len, GFP_KERNEL);
    if (!skb) {
        mutex_unlock(&gl_dev.mutex);
        return ENOMEM;
    }
    skb_reserve(skb, NEEDED_HEADROOM_LEN);
    skb_put(skb, len);
    skb_copy_to_linear_data(skb, payload, len);

    if (append_usb_hdr(dev, skb, USBWIFI_DATA_TYPE_CMD)) {
        ret = -1;
        goto err_hdr;
    }

    ret = bl_write_data_sync(dev, skb->data, skb->len, BL_USB_TIMEOUT);

err_hdr:
    dev_kfree_skb_any(skb);

    mutex_unlock(&gl_dev.mutex);

    return ret;
}

int append_usb_hdr(struct bl_eth_device *dev, struct sk_buff *skb, uint16_t type)
{
    int headroom;
    usb_data_t hdr;

    if (skb->len > TX_LEN_LIMIT) {
        pr_err("skb too large\n");
        return -1;
    }

    headroom = skb_headroom(skb) - NEEDED_HEADROOM_LEN;

    if ((skb_header_cloned(skb) || headroom < 0) &&
         pskb_expand_head(skb, headroom < 0 ? -headroom : 0, 0, GFP_KERNEL)) {
        pr_err("%s: adjust failed\n", __func__);
        return -1;
    }

    memset(&hdr, 0, sizeof(hdr));
    hdr.type = type;
    hdr.length = skb->len;
    hdr.payload_offset = sizeof(hdr);
    if (type == USBWIFI_DATA_TYPE_PKT)
        if (dev->mode == BL_MODE_AP)
            hdr.flags = USB_DATA_FLAG_AP_PKT;

    skb_push(skb, sizeof(hdr));
    skb_copy_to_linear_data(skb, &hdr, sizeof(hdr));

    return 0;
}

static void handle_eth_frame_rx(struct bl_eth_device *dev, struct sk_buff *skb)
{
    skb->dev = dev->net;
    skb->protocol = eth_type_trans(skb, dev->net);
    dev->stats.rx_packets++;
    dev->stats.rx_bytes += skb->len;

    netif_rx(skb);
}

int bl_handle_rx_data(struct bl_eth_device *dev, struct sk_buff *skb)
{
    usb_data_t *hdr = (usb_data_t *)skb->data;
    uint16_t type = hdr->type;
    uint16_t length = hdr->length;
    uint16_t payload_offset = hdr->payload_offset;

    if (length + payload_offset != skb->len) {
        pr_warn("%s: length error", __func__);
        dev_kfree_skb_any(skb);
        return -1;
    }

    skb_pull(skb, payload_offset);

    if (type == USBWIFI_DATA_TYPE_CMD) {
        bl_handle_cmd(dev, skb->data, skb->len);
        dev_kfree_skb_any(skb);
    } else if (type == USBWIFI_DATA_TYPE_PKT) {
        handle_eth_frame_rx(dev, skb);
    } else {
        pr_warn("Unknown data type 0x%x\n", type);
        dev_kfree_skb_any(skb);
    }

    return 0;
}
