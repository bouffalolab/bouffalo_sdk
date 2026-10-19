#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/timer.h>

#include "main.h"
#include "bl_usb_eth.h"
#include "usb_msg_handlers.h"
#include "ctl_port.h"

#define USB_BL_VENDOR_ID             0x349B
#define USB_BL616_IOTWIFI_PRODUCT_ID 0x616F

static int bl_usb_submit_rx_urbs(struct bl_eth_device *dev);

static struct usb_device_id bl_usb_ids[] = {
    {USB_DEVICE_INTERFACE_CLASS(USB_BL_VENDOR_ID, USB_BL616_IOTWIFI_PRODUCT_ID, 0xff)},
    {}
};
MODULE_DEVICE_TABLE(usb, bl_usb_ids);

static int bl_eth_open(struct net_device *net)
{
    int retval = 0;
    struct bl_eth_device *dev = netdev_priv(net);
    if ((dev->mode == BL_MODE_STA && dev->sta_connected) ||
            dev->mode >= BL_MODE_AP) {
        netif_carrier_on(dev->net);
        netif_start_queue(net);
    }

    return retval;
}

static int bl_eth_close(struct net_device *net)
{
    struct bl_eth_device *dev = netdev_priv(net);

    netif_carrier_off(dev->net);
    netif_stop_queue(net);

    return 0;
}

static int bl_queue_skb(struct sk_buff *skb, struct bl_eth_device *dev)
{
    skb_queue_tail(&dev->sk_list, skb);

    if (skb_queue_len(&dev->sk_list) > BL_NDEV_FLOW_CTRL_STOP) {
        dev->status |= BL_TXQ_NDEV_FLOW_CTRL;
        netif_stop_queue(dev->net);
    }

    return 0;
}

static int bl_eth_xmit(struct sk_buff *skb, struct net_device *net)
{
    struct bl_eth_device *dev = netdev_priv(net);

    skb_tx_timestamp(skb);
    bl_queue_skb(skb, dev);

    queue_work(dev->workqueue, &dev->main_work);

    return NETDEV_TX_OK;
}

static void bl_eth_tx_timeout(struct net_device *net
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
        , unsigned int txqueue
#endif
)
{
    struct bl_eth_device *dev = netdev_priv(net);

    dev->stats.tx_errors++;
}

static
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
struct rtnl_link_stats64 *
#else
void
#endif
bl_get_stats64(struct net_device *net, struct rtnl_link_stats64 *stats)
{
    struct bl_eth_device *dev = netdev_priv(net);

    memcpy(stats, &dev->stats, sizeof(*stats));
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
    return stats;
#endif
}

static const struct net_device_ops bl_eth_netdev_ops = {
    .ndo_open = bl_eth_open,
    .ndo_stop = bl_eth_close,
    .ndo_start_xmit = bl_eth_xmit,
    .ndo_tx_timeout = bl_eth_tx_timeout,
    .ndo_get_stats64 = bl_get_stats64,
};

static void bl_usb_tx_complete(struct urb *urb)
{
    urb_ctx_t *ctx = urb->context;
    struct bl_eth_device *dev = ctx->dev;
    int status = urb->status;
    struct sk_buff *skb = ctx->skb;

    if (status != 0) {
        dev_warn(&dev->intf->dev, "%s: transfer skb %p error, status: %d\n", __func__, skb, status);
    } else {
        if (ctx->data_type == BL_URB_PKT) {
            dev->stats.tx_packets++;
            dev->stats.tx_bytes += ctx->pkt_len;
        }
    }

    atomic_dec(&dev->tx_data_urb_pending);
    dev_kfree_skb_any(ctx->skb);

    queue_work(dev->workqueue, &dev->main_work);
}

int bl_tx_push(struct bl_eth_device *dev, struct sk_buff *skb)
{
    int ret = -1;
    struct urb *urb = NULL;
    urb_ctx_t *urb_ctx;
    size_t offset;
    unsigned pkt_len = skb->len;
    unsigned tx_len;
    void *tx_buf;

    if (append_usb_hdr(dev, skb, USBWIFI_DATA_TYPE_PKT)) {
        goto err_build;
    }
    tx_len = skb->len;
    tx_buf = skb->data;

    offset = sizeof(*urb_ctx);
    offset += (uintptr_t)skb->data - ALIGN_LOW(sizeof(int *), skb->data);
    skb_push(skb, offset);
    urb_ctx = (urb_ctx_t *)skb->data;
    urb_ctx->skb = skb;
    urb_ctx->dev = dev;
    urb_ctx->pkt_len = pkt_len;
    urb_ctx->data_type = BL_URB_PKT;

    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb) {
        goto err_urb;
        return -1;
    }
    if (tx_len % 512 == 0) {
        ++tx_len;
    }
    usb_fill_bulk_urb(urb, dev->udev, usb_sndbulkpipe(dev->udev, dev->tx_ep),
            tx_buf, tx_len, bl_usb_tx_complete, urb_ctx);
    usb_anchor_urb(urb, &dev->tx_submitted);
    if ((ret = usb_submit_urb(urb, GFP_KERNEL))) {
        pr_err("submit tx urb failed, ret %d\n", ret);
        goto err_submit;
    }
    usb_free_urb(urb);

    atomic_inc(&dev->tx_data_urb_pending);

    ret = 0;
    goto ret;

err_submit:
    usb_unanchor_urb(urb);
    usb_free_urb(urb);
err_urb:
err_build:
    dev_kfree_skb_any(skb);

ret:
    return ret;
}

static void bl_tx_data_process(struct bl_eth_device *dev)
{
    struct sk_buff *skb;

    while (atomic_read(&dev->tx_data_urb_pending) < TX_DATA_URB_THRESHOLD) {
        skb = skb_dequeue(&dev->sk_list);
        if (!skb) {
            break;
        }
        bl_tx_push(dev, skb);
    }

    if (unlikely(dev->status & BL_TXQ_NDEV_FLOW_CTRL) && skb_queue_len(&dev->sk_list) < BL_NDEV_FLOW_CTRL_RESTART) {
        dev->status &= ~BL_TXQ_NDEV_FLOW_CTRL;
        netif_wake_queue(dev->net);
    }
}

static void bl_rx_data_process(struct bl_eth_device *dev)
{
    struct sk_buff *skb;

    while ((skb = skb_dequeue(&dev->rx_sk_list))) {
        bl_handle_rx_data(dev, skb);
    }
}

void main_wq_hdlr(struct work_struct *work)
{
    struct bl_eth_device *dev = container_of(work, struct bl_eth_device, main_work);

    spin_lock_irq(&dev->main_proc_lock);
    if (dev->bl_processing) {
        dev->more_task_flag = true;
        spin_unlock_irq(&dev->main_proc_lock);
        goto exit_main_proc;
    } else {
        dev->bl_processing = true;
        spin_unlock_irq(&dev->main_proc_lock);
    }

process_start:

    bl_rx_data_process(dev);
    bl_tx_data_process(dev);

    spin_lock_irq(&dev->main_proc_lock);
    if (dev->more_task_flag) {
        dev->more_task_flag = false;
        spin_unlock_irq(&dev->main_proc_lock);
        goto process_start;
    }
    dev->bl_processing = false;
    spin_unlock_irq(&dev->main_proc_lock);

exit_main_proc:
    return;
}

static int bl_eth_get_mac_addr(struct bl_eth_device *dev)
{
    int tx_len;
    int actual_read;

    tx_len = build_simple_cmd_msg(dev->cmd_buf, BF1B_CMD_GET_MAC_ADDR);
    if (bl_write_data_sync(dev, dev->cmd_buf, tx_len, BL_USB_TIMEOUT) ||
        bl_read_data_sync(dev, dev->cmd_buf, CMD_BUF_LEN, &actual_read, BL_USB_TIMEOUT) ||
        parse_get_mac_rsp_msg(dev, dev->cmd_buf, actual_read)) {
        return -1;
    }
    bl_change_eth_mac(dev);
    return 0;
}

static int send_hello(struct bl_eth_device *dev)
{
    int tx_len;
    tx_len = build_simple_cmd_msg(dev->cmd_buf, BF1B_CMD_HELLO);
    if (bl_write_data_sync(dev, dev->cmd_buf, tx_len, BL_USB_TIMEOUT)) {
        return -1;
    }
    return 0;
}

static u32 bl_eth_ethtool_op_get_link(struct net_device *net)
{
    struct bl_eth_device *dev = netdev_priv(net);
    return netif_carrier_ok(dev->net);
}

static const struct ethtool_ops ops = {
    .get_link = bl_eth_ethtool_op_get_link
};

static int bl_usb_init(struct usb_interface *intf, struct bl_eth_device *dev)
{
    struct usb_device *udev = interface_to_usbdev(intf);
    struct usb_host_interface *iface_desc = intf->cur_altsetting;
    struct usb_endpoint_descriptor *epd;
    int ret = -1;
    int i;

    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        epd = &iface_desc->endpoint[i].desc;
        if (usb_endpoint_dir_in(epd) &&
            usb_endpoint_num(epd) == BL_USB_EP_IN &&
            usb_endpoint_xfer_bulk(epd)) {
            printk("info: EP IN: ep_num: %d, max pkt size: %d, addr: %d\n",
                 usb_endpoint_num(epd),
                 le16_to_cpu(epd->wMaxPacketSize),
                 epd->bEndpointAddress);
            dev->rx_ep = usb_endpoint_num(epd);
            atomic_set(&(dev->rx_data_urb_pending), 0);
        }

        if (usb_endpoint_dir_out(epd) &&
            usb_endpoint_num(epd) == BL_USB_EP_OUT &&
            usb_endpoint_xfer_bulk(epd)) {
            printk("info: EP OUT: ep_num: %d, max pkt size: %d, addr: %d\n",
                 usb_endpoint_num(epd),
                 le16_to_cpu(epd->wMaxPacketSize),
                 epd->bEndpointAddress);
            dev->tx_ep = usb_endpoint_num(epd);
            dev->tx_mps = le16_to_cpu(epd->wMaxPacketSize);
            atomic_set(&(dev->tx_data_urb_pending), 0);
        }
    }

    dev->intf = intf;
    dev->udev = udev;

    usb_set_intfdata(intf, dev);
    usb_get_dev(udev);

    ret = 0;

    return ret;
}

static void bl_usb_rx_complete(struct urb *urb)
{
    urb_ctx_t *ctx = urb->context;
    struct bl_eth_device *dev = ctx->dev;
    struct sk_buff *skb = ctx->skb;
    int status = urb->status;
    u32 rx_len  = urb->actual_length;

    if (status) {
        dev_kfree_skb_any(skb);
    } else {
        skb_put(skb, rx_len);
        skb_queue_tail(&dev->rx_sk_list, skb);
        queue_work(dev->workqueue, &dev->main_work);
    }

    atomic_dec(&dev->rx_data_urb_pending);

    if (status == 0 && atomic_read(&dev->rx_data_urb_pending) < RX_DATA_URB_THRESHOLD) {
        bl_usb_submit_rx_urbs(dev);
    }
}

static int bl_usb_submit_rx_urbs(struct bl_eth_device *dev)
{
    while (atomic_read(&dev->rx_data_urb_pending) < RX_DATA_URB_MAX) {
        struct urb *urb;
        struct sk_buff *skb;
        size_t offset;
        urb_ctx_t *ctx;

        urb = usb_alloc_urb(0, GFP_ATOMIC);
        if (!urb) {
            return -1;
        }
        skb = dev_alloc_skb(TX_LEN_LIMIT + NEEDED_HEADROOM_LEN + 8);
        if (!skb) {
            usb_free_urb(urb);
            return -1;
        }
        skb_reserve(skb, sizeof(urb_ctx_t) + sizeof(int *));
        offset = sizeof(urb_ctx_t);
        offset += (uintptr_t)skb->data - ALIGN_LOW(sizeof(int *), skb->data);
        ctx = (urb_ctx_t *)(skb->data - offset);
        ctx->skb = skb;
        ctx->dev = dev;

        usb_fill_bulk_urb(urb, dev->udev, usb_rcvbulkpipe(dev->udev, dev->rx_ep),
                skb->data, TX_LEN_LIMIT + sizeof(usb_data_t) + 4, bl_usb_rx_complete, ctx);

        usb_anchor_urb(urb, &dev->rx_submitted);
        if (usb_submit_urb(urb, GFP_ATOMIC)) {
            pr_err("submit rx urb failed\n");
            usb_unanchor_urb(urb);
            dev_kfree_skb_any(skb);
            usb_free_urb(urb);
            return -1;
        }

        usb_free_urb(urb);

        atomic_inc(&dev->rx_data_urb_pending);
    }

    return 0;
}

static int tx_rx_init(struct bl_eth_device *dev)
{
    init_usb_anchor(&dev->tx_submitted);
    init_usb_anchor(&dev->rx_submitted);

    if (bl_usb_submit_rx_urbs(dev)) {
        return -1;
    }
    return 0;
}

static void tx_rx_deinit(struct bl_eth_device *dev)
{
    usb_kill_anchored_urbs(&dev->tx_submitted);
    usb_kill_anchored_urbs(&dev->rx_submitted);
}

static void bl_usb_deinit(struct bl_eth_device *dev)
{
    usb_set_intfdata(dev->intf, NULL);
    usb_put_dev(interface_to_usbdev(dev->intf));
}

static int bl_usb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    int retval = -EINVAL;
    struct bl_eth_device *dev = NULL;
    struct net_device *netdev = NULL;

    mutex_lock(&gl_dev.mutex);
    if (gl_dev.eth_dev) {
        mutex_unlock(&gl_dev.mutex);
        dev_err(&intf->dev, "multiple device not supported\n");
        return -EINVAL;
    }

    netdev = alloc_etherdev(sizeof(struct bl_eth_device));
    if (!netdev) {
        retval = -ENOMEM;
        goto err_eth;
    }

    netdev->netdev_ops = &bl_eth_netdev_ops;
    netdev->watchdog_timeo = BL_ETH_TX_TIMEOUT;
    strcpy(netdev->name, "bleth%d");
    netdev->needed_headroom = NEEDED_HEADROOM_LEN;

    dev = netdev_priv(netdev);
    dev->net = netdev;

    if ((dev->cmd_buf = kmalloc(CMD_BUF_LEN, GFP_KERNEL)) == NULL) {
        retval = -ENOMEM;
        goto err_cmd_buf;
    }

    if (bl_usb_init(intf, dev)) {
        goto err_usb_init;
    }
    if (bl_eth_get_mac_addr(dev)) {
        dev_err(&intf->dev,"error getting MAC address\n");
        goto err_get_macaddr;
    }

    dev->workqueue = alloc_workqueue("BL_WORK_QUEUE", WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
    if (!dev->workqueue) {
        retval = -ENOMEM;
        goto err_create_wkq;
    }
    INIT_WORK(&dev->main_work, main_wq_hdlr);

    skb_queue_head_init(&dev->sk_list);
    skb_queue_head_init(&dev->rx_sk_list);

    spin_lock_init(&dev->main_proc_lock);

    netdev->ethtool_ops = &ops;

    retval = register_netdev(netdev);
    if (retval) {
        dev_err(&intf->dev, "error registering netdev: %d\n", retval);
        retval = -EIO;
        goto err_register_netdev;
    }

    netif_carrier_off(netdev);
    netif_tx_stop_all_queues(netdev);

    if (tx_rx_init(dev)) {
        goto err_txrx;
    }

    gl_dev.eth_dev = dev;
    mutex_unlock(&gl_dev.mutex);

    send_hello(dev);

    dev_info(&intf->dev, "DEVICE attached\n");

    return 0;

err_txrx:
    tx_rx_deinit(dev);
err_register_netdev:
    if (dev->workqueue) {
        destroy_workqueue(dev->workqueue);
    }
err_create_wkq:
err_get_macaddr:
    bl_usb_deinit(dev);
err_usb_init:
    if (dev) {
        kfree(dev->cmd_buf);
    }
err_cmd_buf:
    if (netdev) {
        free_netdev(netdev);
    }
err_eth:

    gl_dev.eth_dev = NULL;
    mutex_unlock(&gl_dev.mutex);

    return retval;
}

static void bl_usb_disconnect(struct usb_interface *intf)
{
    struct bl_eth_device *dev;

    mutex_lock(&gl_dev.mutex);
    dev = usb_get_intfdata(intf);

    netif_carrier_off(dev->net);
    netif_tx_stop_all_queues(dev->net);

    flush_workqueue(dev->workqueue);
    destroy_workqueue(dev->workqueue);

    unregister_netdev(dev->net);
    kfree(dev->cmd_buf);
    tx_rx_deinit(dev);

    bl_usb_deinit(dev);
    free_netdev(dev->net);

    gl_dev.eth_dev = NULL;
    mutex_unlock(&gl_dev.mutex);

    dev_info(&intf->dev, "Device disconnected\n");
}

static int bl_usb_suspend(struct usb_interface *intf, pm_message_t message)
{
    return 0;
}

static int bl_usb_resume(struct usb_interface *intf)
{
    return 0;
}

static struct usb_driver bl_usb_drv = {
    .name     = KBUILD_MODNAME,
    .id_table = bl_usb_ids,
    .probe    = bl_usb_probe,
    .disconnect = bl_usb_disconnect,
    .suspend = bl_usb_suspend,
    .resume = bl_usb_resume,
};

int bl_write_data_sync(struct bl_eth_device *dev, u8 *data, u32 len, u32 timeout)
{
    int actual_length, ret;

    ret = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->tx_ep), data,
            len, &actual_length, timeout);
    if (ret) {
        printk("usb_bulk_msg for tx failed: %d\n", ret);
        return ret;
    }

    return ret;
}

int bl_read_data_sync(struct bl_eth_device *dev, u8 *data, u32 len, int *actual_length, u32 timeout)
{
    int ret;

    ret = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->rx_ep), data,
            len, actual_length, timeout);
    if (ret) {
        printk("usb_bulk_msg for rx failed: %d\n", ret);
        return ret;
    }

    return ret;
}

int bl_register_eth_drv(void)
{
    return usb_register(&bl_usb_drv);
}

void bl_unregister_eth_drv(void)
{
    usb_deregister(&bl_usb_drv);
}
