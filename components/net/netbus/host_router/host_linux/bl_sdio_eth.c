#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/timer.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/proc_fs.h>
#include <linux/completion.h>

#include "main.h"
#include "bl_sdio_eth.h"
#include "bl_sdio.h"
#include "sdio_msg_handlers.h"
#include "ctl_port.h"
#include "tty.h"
#include "version.h"

static void send_cmds(struct bl_eth_device *dev);

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
        dev->netif_stopped_ts = jiffies;
    }

    return 0;
}

static int bl_eth_xmit(struct sk_buff *skb, struct net_device *net)
{
    struct bl_eth_device *dev = netdev_priv(net);

    skb_tx_timestamp(skb);
    bl_queue_skb(skb, dev);

    queue_work(dev->txworkqueue, &dev->tx_work);

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

static const struct sdio_device_id bl_sdio_ids[] = {
    { SDIO_DEVICE(SDIO_VENDOR_ID, SD_DEVICE_ID) },
    { 0 }
};
MODULE_DEVICE_TABLE(sdio, bl_sdio_ids);

void bl_sdio_deinit(struct bl_eth_device *dev)
{
    sdio_claim_host(dev->func);
    sdio_release_irq(dev->func);
    sdio_disable_func(dev->func);
    sdio_release_host(dev->func);

    kfree(dev->mp_regs);
}

#if USE_POLLING_RX
static struct task_struct *rx_polling_task;
static int bl_rx_process(struct bl_eth_device *dev, struct sk_buff *skb, u32 port);
int rx_task(void *data)
{
    u8 rd_bitmap_l;
    u8 rd_bitmap_u;
    u32 bitmap;
    struct bl_eth_device *dev;

    dev = (struct bl_eth_device*)data;

    while(1){
        if(kthread_should_stop()) {
            break;
        }

        bl_read_reg(dev, dev->reg->rd_bitmap_l, &rd_bitmap_l);
        bl_read_reg(dev, dev->reg->rd_bitmap_u, &rd_bitmap_u);

        bitmap = rd_bitmap_l;
        bitmap |= rd_bitmap_u << 8;

        if (bitmap == 1) {
            bl_rx_process(dev, NULL, 0x10000);
        }

        usleep_range(400, 600);
    }

    return 0;
}
#endif /* USE_POLLING_RX */

static int bl_sdio_get_ready(struct bl_eth_device *dev)
{
    u8 val;
    int ret;
    int retry = 0;
    int got_clamp = 0;
    int got_sndnxt = 0;

    do {
        if (!got_clamp) {
            val = sdio_readb(dev->func, dev->reg->scratch_0_0, &ret);
            if (ret) {
                pr_err("%s: sdio_readb failed, %d\n", __func__, ret);
            } else {
                dev->seq_clamp = val;
                got_clamp = 1;
            }
        }

        if (!got_sndnxt) {
            val = sdio_readb(dev->func, dev->reg->scratch_1_3, &ret);
            if (ret) {
                pr_err("%s: sdio_readb failed, %d\n", __func__, ret);
            } else {
                dev->snd_nxt = val;
                got_sndnxt = 1;
            }
        }

        if (got_clamp && got_sndnxt) {
            pr_info("resync done, seq_clamp %u, snd_nxt %u\n",
                    dev->seq_clamp, dev->snd_nxt);
            return 0;
        }
        msleep(100);
    } while (retry++ < 10);

    return -1;
}

static int bl_sdio_init(struct bl_eth_device *dev)
{
    int ret = 0;
    u8 sdio_ireg;

    dev->reg = &bl_reg_sd606;
    dev->max_ports = 1;
    dev->has_control_mask = true;
    dev->func->card->quirks |= MMC_QUIRK_BLKSZ_FOR_BYTE_MODE;

    sdio_claim_host(dev->func);
    /*1. enable func*/
    ret = sdio_enable_func(dev->func);
    if (ret) {
        pr_err("sdio_enable_func failed, %d\n", ret);
        return ret;
    }

    /* wait for sdio device ready */
    ret = bl_sdio_get_ready(dev);
    if (ret < 0) {
        pr_err("sdio device is not ready");
        goto err_out;
    }

    /*
     * 2. Read the host_int_status_reg for ACK the first interrupt got
     * from the bootloader. If we don't do this we get a interrupt
     * as soon as we register the irq.
     */
    sdio_readb(dev->func, dev->reg->host_int_status_reg, &ret);
    if (ret) {
        pr_err("sdio_readb failed, %d, can't get host interrupt status\n", ret);
        goto err_out;
    }

    /* 3. Get SDIO ioport */
    dev->io_port = (sdio_readb(dev->func, dev->reg->io_port_0_reg, &ret) & 0xff);
    if (ret) {
        pr_err("sdio_readb failed, %d, can't get io port0\n", ret);
        goto err_out;
    }
    dev->io_port |= ((sdio_readb(dev->func, dev->reg->io_port_1_reg, &ret) & 0xff) << 8);
    if (ret) {
        pr_err("sdio_readb failed, %d, can't get io port1\n", ret);
        goto err_out;
    }
    dev->io_port |= ((sdio_readb(dev->func, dev->reg->io_port_2_reg, &ret) & 0xff) << 16);
    if (ret) {
        pr_err("sdio_readb failed, %d, can't get io port2\n", ret);
        goto err_out;
    }
    /* printk("SDIO FUNC1 IO port: %#x\n", dev->io_port); */

    /* Set Host interrupt reset to read to clear */
    sdio_ireg = sdio_readb(dev->func, dev->reg->host_int_rsr_reg, &ret);
    if (ret) {
        pr_err("sdio_readb failed, %d, can't get host int rsr\n", ret);
        goto err_out;
    }
    sdio_writeb(dev->func, sdio_ireg | dev->reg->sdio_int_mask, dev->reg->host_int_rsr_reg, &ret);
    if (ret) {
        pr_err("sdio_writeb failed, %d, can't write host int rsr\n", ret);
        goto err_out;
    }

    /* Dnld/Upld ready set to auto reset */
    sdio_ireg = sdio_readb(dev->func, dev->reg->card_misc_cfg_reg, &ret);
    if (ret) {
        pr_err("sdio_readb failed, %d, can't get card misc cfg reg\n", ret);
        goto err_out;
    }
    sdio_writeb(dev->func, sdio_ireg | AUTO_RE_ENABLE_INT, dev->reg->card_misc_cfg_reg, &ret);
    if (ret) {
        pr_err("sdio_writeb failed, %d, can't set card misc cfg reg\n", ret);
        goto err_out;
    }

    /* 4. set block size*/
    sdio_set_block_size(dev->func, BL_SDIO_BLOCK_SIZE);

    /* 5. request irq */
#if !USE_POLLING_RX
    ret = sdio_claim_irq(dev->func, bl_irq_hdlr);
    if (ret) {
        pr_err("sdio_claim_irq failed: ret=%d\n", ret);
        goto err_out;
    }
#else
    rx_polling_task = kthread_create(rx_task, dev, "bl_rx");
    if(IS_ERR(rx_polling_task)){
        printk("Unable to start kernel thread.");
        ret = PTR_ERR(rx_polling_task);
        rx_polling_task = NULL;
        return ret;
    }

    wake_up_process(rx_polling_task);
#endif /* !USE_POLLING_RX */

    /* Simply write the mask to the register */
    sdio_writeb(dev->func, UP_LD_HOST_INT_MASK | DN_LD_HOST_INT_MASK, dev->reg->host_int_mask_reg, &ret);
    if (ret) {
        pr_err("sdio_writeb failed, %d, can't write host int mask\n", ret);
        goto err_irq;
    }

    sdio_ireg = sdio_readb(dev->func, dev->reg->host_int_mask_reg, &ret);
    if(ret) {
        pr_err("sdio_readb failed, %d, can't get host int mask\n", ret);
        goto err_irq;
    }

    /* printk("irq_enable=0x%x\n", sdio_ireg); */

    sdio_release_host(dev->func);

    /* 6. Initialize SDIO variables in card */
    dev->mp_rd_bitmap = 0;
    dev->mp_wr_bitmap = 0;
    dev->curr_rd_port = dev->reg->start_rd_port;
    dev->curr_wr_port = dev->reg->start_wr_port;
    dev->curr_wr_port = 0;
    dev->my_curr_rd_port = 0;
    dev->max_tx_loop_elapsed_ms = 0;
    dev->max_netif_stopped_ms = 0;
    dev->sdio_write_timeo_cnt = 0;
    dev->sdio_write_retries = 0;
    dev->sdio_io_fail_cnt = 0;

    dev->mp_regs = kzalloc(dev->reg->max_mp_regs, GFP_KERNEL);
    if (!dev->mp_regs) {
        pr_err("kzalloc mp_regs failed!\n");
        sdio_claim_host(dev->func);
        ret = -ENOMEM;
        goto err_irq;
    }

    return ret;

err_irq:
    sdio_release_irq(dev->func);
err_out:
    sdio_disable_func(dev->func);
    sdio_release_host(dev->func);
    return ret;
}

static void bl_get_interrupt_status(struct bl_eth_device *dev)
{
    u8 sdio_ireg;

    bl_read_reg(dev, dev->reg->host_int_status_reg, &sdio_ireg);
    if(sdio_ireg) {
        spin_lock(&dev->int_lock);
        dev->int_status |= sdio_ireg;
        spin_unlock(&dev->int_lock);
    }
}

int bl_sdio_write(struct bl_eth_device *dev, const void *data_ptr, const uint16_t data_len)
{
    u8 wr_bitmap_l = 0;
    u8 wr_bitmap_u = 0;
    unsigned long start_tick;
    u8 *ptx_data = dev->xfr.ptx_data;


    //printk("===== bl_sdio_write dnld port:%d, bitmap:%X\r\n", dev->curr_wr_port, dev->mp_wr_bitmap);
    start_tick = jiffies;
    while ((dev->mp_wr_bitmap & (1 << dev->curr_wr_port)) == 0) {
        if (time_after(jiffies, start_tick + msecs_to_jiffies(SDIO_TX_TIMEOUT))) {
            dev->sdio_write_timeo_cnt++;
            pr_warn("%s: timeout! port:%d, bitmap:%X\n",
                    __func__, dev->curr_wr_port, dev->mp_wr_bitmap);
            return -1;
        }

        bl_read_reg(dev, dev->reg->wr_bitmap_l, &wr_bitmap_l);
        bl_read_reg(dev, dev->reg->wr_bitmap_u, &wr_bitmap_u);
        dev->mp_wr_bitmap = wr_bitmap_l;
        dev->mp_wr_bitmap |= wr_bitmap_u << 8;
    }
    dev->mp_wr_bitmap &= ~(1 << dev->curr_wr_port);

    ptx_data[0] = data_len & 0xff;
    ptx_data[1] = (data_len >> 8) & 0xff;

    memcpy(&ptx_data[2], data_ptr, data_len);

    bl_write_data_sync(dev, ptx_data, 2048, 0x10000 + dev->curr_wr_port);

    dev->curr_wr_port++;
    if (dev->curr_wr_port == SDIO_DNLD_MAX_PORTS) {
        dev->curr_wr_port = 0;
    }

    return 0;
}

int bl_sdio_read(struct bl_eth_device *dev, void **data)
{
    u8 rd_bitmap_l = 0;
    u8 rd_bitmap_u = 0;
    u32 bitmap;
    unsigned long start_tick;
    u8 *rx_buf = dev->xfr.prx_data;

    start_tick = jiffies;

    while (1) {
        if (time_after(jiffies, start_tick + msecs_to_jiffies(SDIO_TX_TIMEOUT))) {
            printk("%s: timeout!", __func__);
            return -1;
        }
        bl_read_reg(dev, dev->reg->rd_bitmap_l, &rd_bitmap_l);
        bl_read_reg(dev, dev->reg->rd_bitmap_u, &rd_bitmap_u);
        bitmap = rd_bitmap_l;
        bitmap |= rd_bitmap_u << 8;

        if (bitmap & (1 << dev->my_curr_rd_port))
            break;
    }
    //printk("===== bl_sdio_read upld bitmap:%X\r\n", bitmap);
    if (bl_read_data_sync(dev, rx_buf, 2048, 0x10000 + dev->my_curr_rd_port))
        return -1;
    dev->my_curr_rd_port = (dev->my_curr_rd_port + 1) % 16;
    *data = &rx_buf[0];
    return 0;
}

int bl_sdio_read_cb_register(struct bl_eth_device *dev, bl_sdio_read_cb_t cb, void *cb_arg)
{
    if (cb == NULL) {
        return -1;
    }

    dev->xfr.read_process = cb;
    dev->xfr.read_process_cb_arg = cb_arg;

    return 0;
}

static int rx_data(struct bl_eth_device *dev, u8 *rx_buf, u32 port, u32 len)
{
    if (bl_read_data_sync(dev, rx_buf, len, port)) {
        return -1;
    }
    return 0;
}

#if 0
static int bl_rx_process(struct bl_eth_device *dev, struct sk_buff *skb, u32 port)
{
    uint16_t len;
    u8 *prx_data;
    struct xfr_ctx *xfr;

    xfr = &dev->xfr;
    prx_data = xfr->prx_data;
    if (rx_data(dev, prx_data, port, 2048)) {
        return -1;
    }

    if (xfr->read_process != NULL) {
        /* len = prx_data[0] | (prx_data[1] << 8); */
        len = 0xff; // fake
        xfr->read_process(xfr->read_process_cb_arg, &prx_data[0], len);
    }

    return 0;
}
#endif

static int bl_rx_noprocess(struct bl_eth_device *dev, struct sk_buff *skb, u32 port)
{
    u8 *prx_data;
    struct xfr_ctx *xfr;

    xfr = &dev->xfr;
    prx_data = xfr->prx_data;
    if (rx_data(dev, prx_data, port, 1)) {
        return -1;
    }

    return 0;
}

static int gen_sd_agg_port(uint16_t bitmap, uint32_t *port, int *num, uint8_t curr_rd_port)
{
    int ffs = -1;
    int i;
    int n = 0;

    ffs = curr_rd_port;
    ++n;
    for (i = 1; i < 16; ++i) {
        u8 ii = (ffs + i) % 16;
        if (bitmap & (1 << ii)) {
            ++n;
        } else {
            break;
        }
    }
    if (n > 8) {
        n = 8;
    }
    *port = 0x11000 + (((1 << n) - 1) << 4) + ffs;
    /* printk("gen_sd_agg_port: bitmap 0x%x, curr_rd_port %d, num %d, generated port 0x%x\n", bitmap, curr_rd_port, n, *port); */
    *num = n;
    return 0;
}

static int bl_rx_process_agg(struct bl_eth_device *dev, u16 bitmap)
{
    uint16_t len;
    u8 *prx_data;
    struct xfr_ctx *xfr;
    u32 port;
    int num_of_pdu;

    xfr = &dev->xfr;
    prx_data = xfr->prx_data;
    if (!(bitmap & (1 << dev->my_curr_rd_port))) {
        pr_err("read bitmap 0x%x, read port %d is not set, need resync?\n",
                bitmap, dev->my_curr_rd_port);
    }

    if (gen_sd_agg_port(bitmap, &port, &num_of_pdu, dev->my_curr_rd_port))
        return -1;

    dev->my_curr_rd_port = (dev->my_curr_rd_port + num_of_pdu) % 16;
    /* printk("curr_rd_port updated to %u\n", dev->my_curr_rd_port); */
    /* printk("Read %d pdus, bitmap 0x%x, port 0x%x\n", num_of_pdu, bitmap, port); */
    if (rx_data(dev, prx_data, port, SD_RX_BUF_LEN * num_of_pdu))
        return -1;

    if (xfr->read_process != NULL)
        xfr->read_process(xfr->read_process_cb_arg, &prx_data[0], num_of_pdu, len);

    return 0;
}

static int bl_upload_hdl(struct bl_eth_device *dev)
{
    u8 rd_bitmap_l = 0;
    u8 rd_bitmap_u = 0;
    u32 bitmap;
    int loop_cnt = 1;

    bl_read_reg(dev, dev->reg->rd_bitmap_l, &rd_bitmap_l);
    bl_read_reg(dev, dev->reg->rd_bitmap_u, &rd_bitmap_u);
    bitmap = rd_bitmap_l;
    bitmap |= rd_bitmap_u << 8;

    //printk("bl_upload_hdl: bitmap %x, cur_rd_port %u\n", bitmap, dev->my_curr_rd_port);
    if (!bitmap) {
        u32 fake_rd_port;

        /* TODO why? */
        /* pr_err("sdio read bitmap is 0?!\n"); */
        fake_rd_port = 0x10000 + (dev->my_curr_rd_port - 1 + 16) % 16;
        bl_rx_noprocess(dev, NULL, fake_rd_port);
        return 0;
    }

    do {
        if (loop_cnt > 1) {
            /* printk("bl_upload_hdl: bitmap %x\n", bitmap); */
            /* printk("Process loop %d\n", loop_cnt); */
            if (loop_cnt > 16)
                break;
        }
        bl_rx_process_agg(dev, bitmap);
        bl_read_reg(dev, dev->reg->rd_bitmap_l, &rd_bitmap_l);
        bl_read_reg(dev, dev->reg->rd_bitmap_u, &rd_bitmap_u);
        bitmap = rd_bitmap_l;
        bitmap |= rd_bitmap_u << 8;
        loop_cnt++;
    } while (bitmap);

#if 0
    if (bitmap & (1 << dev->my_curr_rd_port)) {
        if (bl_rx_process(dev, NULL, 0x10000 + dev->my_curr_rd_port)) {
            printk("bl_rx_process error\n");
            return -1;
        }
    }
#endif
#if 0
    do {
        if (loop_cnt > 1) {
            printk("Process loop %d\n", loop_cnt);
        }
        for (i = 0; i < 16; ++i) {
            if (bitmap & (1 << i)) {
                if (bl_rx_process(dev, NULL, 0x10000 + i)) {
                    printk("bl_rx_process error\n");
                    return -1;
                }
            }
        }
        bl_read_reg(dev, dev->reg->rd_bitmap_l, &rd_bitmap_l);
        bl_read_reg(dev, dev->reg->rd_bitmap_u, &rd_bitmap_u);
        bitmap = rd_bitmap_l;
        bitmap |= rd_bitmap_u << 8;
        loop_cnt++;
    } while (bitmap);
#endif

    return 0;
}

int bl_tx_push(struct bl_eth_device *dev, struct sk_buff *skb)
{
    send_eth_frame(dev, skb);
    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;
    dev_kfree_skb_any(skb);

    return 0;
}

static inline int bl_tx_credit_available(struct bl_eth_device *dev)
{
    return (s8)(dev->seq_clamp - dev->snd_nxt) > 0;
}

#define BL_TX_CREDIT_TIMEOUT_MS 2500

static bool bl_eth_tx_data_process(struct bl_eth_device *dev)
{
    struct sk_buff *skb;
    size_t txed = 0;
    bool restart = false;
    unsigned long start_jiffies, elapsed, delta, check_point;

    start_jiffies = jiffies;
    while ((skb = skb_dequeue(&dev->sk_list)) != NULL) {
        check_point = jiffies + msecs_to_jiffies(BL_TX_CREDIT_TIMEOUT_MS);
        while (1) {
            if (!bl_card_ok()) {
                dev_kfree_skb_any(skb);
                break;
            }

            if (bl_tx_credit_available(dev)) {
                bl_tx_push(dev, skb);
                dev->snd_nxt++;
                break;
            } else {
                bl_read_reg(dev, FREE_TX_PBUF_CNT_OFFSET, &dev->seq_clamp);
                if (!bl_tx_credit_available(dev)) {
                    usleep_range(100, 200);
                    if (time_after(jiffies, check_point)) {
                        dev_kfree_skb_any(skb);
                        dev->net->stats.tx_dropped++;
                        pr_warn("the ethernet frame is dropped because we "
                                "don't have credits for transmission for more "
                                "than %dms, snd_nxt %u, seq_clamp %u, "
                                "please check firmware's status\n",
                                BL_TX_CREDIT_TIMEOUT_MS, dev->snd_nxt,
                                dev->seq_clamp);
                        break;
                    }
                }
            }
        }
        ++txed;
        if (txed == TX_ETH_FRAME_BULK) {
            restart = true;
            break;
        }
    }
    elapsed = jiffies_to_msecs(jiffies - start_jiffies);
    if (elapsed > dev->max_tx_loop_elapsed_ms) {
        dev->max_tx_loop_elapsed_ms = elapsed;
        pr_info("%s tx loop max elapsed %lums\n",
                __func__, dev->max_tx_loop_elapsed_ms);
    }

    if (!bl_card_ok()) {
        return restart;
    }

    if (unlikely(dev->status & BL_TXQ_NDEV_FLOW_CTRL) && skb_queue_len(&dev->sk_list) < BL_NDEV_FLOW_CTRL_RESTART) {
        dev->status &= ~BL_TXQ_NDEV_FLOW_CTRL;
        netif_wake_queue(dev->net);

        delta = jiffies - dev->netif_stopped_ts;
        delta = jiffies_to_msecs(delta);
        if (delta > dev->max_netif_stopped_ms) {
            dev->max_netif_stopped_ms = delta;
            pr_info("netif is stopped for %lums\n", dev->max_netif_stopped_ms);
        }
    }

    return restart;
}

static void send_tty(struct bl_eth_device *dev)
{
    struct sk_buff *skb;

    while ((skb = skb_dequeue(&dev->tty_msg_list)) != NULL) {
        struct bl_tty_queued_data *qd;
        u16 tty_id;
        unsigned skb_len;
        int avail_len;
        unsigned tx_len;

        if (!bl_card_ok()) {
            dev_kfree_skb_any(skb);
            continue;
        }
        skb_len = skb->len;

        avail_len = atomic_read(&dev->tty_credit);
        if (avail_len == 0) {
            skb_queue_head(&dev->tty_msg_list, skb);
            break;
        }
        qd = (struct bl_tty_queued_data *)skb->cb;
        tty_id = qd->tty_id;

        tx_len = min_t(size_t, avail_len, skb_len);
        tx_len = min_t(size_t, tx_len, 2000);
        send_tty_frame(dev, tty_id, skb->data, tx_len);
        atomic_sub(tx_len, &dev->tty_credit);

        skb_pull(skb, tx_len);
        if (skb->len == 0) {
            dev_kfree_skb_any(skb);
        } else {
            skb_queue_head(&dev->tty_msg_list, skb);
        }
    }
}

void tx_wq_hdlr(struct work_struct *work)
{
    struct bl_eth_device *dev = container_of(work, struct bl_eth_device, tx_work);

    spin_lock_irq(&dev->tx_proc_lock);
    if (dev->bl_tx_processing) {
        printk("bl_main_process: tx_more_task_flag = true\n");
        dev->tx_more_task_flag = true;
        spin_unlock_irq(&dev->tx_proc_lock);
        goto exit_main_proc;
    } else {
        dev->bl_tx_processing = true;
        spin_unlock_irq(&dev->tx_proc_lock);
    }

process_start:
    send_cmds(dev);
    send_tty(dev);
    if (bl_eth_tx_data_process(dev)) {
        goto process_start;
    }

    spin_lock_irq(&dev->tx_proc_lock);
    if (dev->tx_more_task_flag) {
        dev->tx_more_task_flag = false;
        spin_unlock_irq(&dev->tx_proc_lock);
        goto process_start;
    }
    dev->bl_tx_processing = false;
    spin_unlock_irq(&dev->tx_proc_lock);

exit_main_proc:
    return;
}

void dump_buf(const void *buf, const size_t len)
{
    size_t i;

    for (i = 0; i < len; ++i) {
        printk(KERN_CONT "%02X ", ((uint8_t *)buf)[i]);
        if ((i + 1) % 16 == 0) {
            printk("\n");
        }
    }
    if (i % 16 != 0) {
        printk("\n");
    }
}

static void send_cmds(struct bl_eth_device *dev)
{
    struct sk_buff *skb;

    while ((skb = skb_dequeue(&dev->cmd_list))) {
        if (!bl_card_ok()) {
            dev_kfree_skb_any(skb);
            continue;
        }
        bl_sdio_write(dev, skb->data, skb->len);
        dev_kfree_skb_any(skb);
    }
}

void main_wq_hdlr(struct work_struct *work)
{
    u8 sdio_ireg;
    int ret = 0;

    struct bl_eth_device *dev = container_of(work, struct bl_eth_device, main_work);

    spin_lock_irq(&dev->main_proc_lock);
    if (dev->bl_processing) {
        printk("bl_main_process: more_task_flag = true\n");
        dev->more_task_flag = true;
        spin_unlock_irq(&dev->main_proc_lock);
        goto exit_main_proc;
    } else {
        dev->bl_processing = true;
        spin_unlock_irq(&dev->main_proc_lock);
    }

process_start:
    if (!bl_card_ok()) {
        spin_lock_irq(&dev->main_proc_lock);
        dev->bl_processing = false;
        spin_unlock_irq(&dev->main_proc_lock);
        return;
    }

#if CARD_DEAD_CHECK
    if (dev->check_card_flag) {
        struct timer_list *t = &dev->check_card_timer;

        dev->check_card_flag = false;
        if (time_after(jiffies, dev->last_ping_recv_time + msecs_to_jiffies(CARD_DEAD_THRESHOLD_MS))) {
            mutex_lock(&gl_dev.mutex);
            gl_dev.status |= BL_DEVICE_STATUS_CARD_DEAD;
            mutex_unlock(&gl_dev.mutex);
            netif_carrier_off(dev->net);
            netif_tx_stop_all_queues(dev->net);
            printk("Card dead! Notifying daemon\n");
            notify_daemon_simple_event(dev, CTL_PORT_MSG_CARD_DEAD);
        } else {
            t->expires = jiffies + msecs_to_jiffies(CHECK_CARD_DEAD_INTERVAL_MS);
            add_timer(t);
        }
    }
#endif

    spin_lock_irq(&dev->int_lock);
    sdio_ireg = dev->int_status;
    dev->int_status = 0;
    spin_unlock_irq(&dev->int_lock);

    if (sdio_ireg & UP_LD_HOST_INT_STATUS) {
        ret = bl_upload_hdl(dev);
        if(ret)
            goto exit_main_proc;
    }

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

void bl_irq_hdlr(struct sdio_func *func)
{
    struct bl_eth_device *dev;

    dev = sdio_get_drvdata(func);
    if (!dev) {
        return;
    }

    bl_get_interrupt_status(dev);

    spin_lock_irq(&dev->main_proc_lock);
    if (dev->bl_processing) {
        dev->more_task_flag = true;
        spin_unlock_irq(&dev->main_proc_lock);
    } else {
        spin_unlock_irq(&dev->main_proc_lock);
        queue_work(dev->workqueue, &dev->main_work);
    }

    return;
}

static u32 bl_eth_ethtool_op_get_link(struct net_device *net)
{
    struct bl_eth_device *dev = netdev_priv(net);
    return netif_carrier_ok(dev->net);
}

static const struct ethtool_ops ops = {
    .get_link = bl_eth_ethtool_op_get_link
};

static int init_xfr_ctx(struct bl_eth_device *dev)
{
    memset(&dev->xfr, 0, sizeof(dev->xfr));
    dev->xfr.prx_data = kmalloc(SD_RX_BUF_LEN * 8, GFP_KERNEL);
    dev->xfr.ptx_data = kmalloc(2048, GFP_KERNEL);
    if (!(dev->xfr.prx_data && dev->xfr.ptx_data)) {
        return -1;
    }
    return 0;
}

static void deinit_xfr_ctx(struct bl_eth_device *dev)
{
    kfree(dev->xfr.prx_data);
    kfree(dev->xfr.ptx_data);
    memset(&dev->xfr, 0, sizeof(dev->xfr));
}

#if CARD_DEAD_CHECK
static void check_card_timer_cb(struct timer_list *t)
{
    struct bl_eth_device *dev = from_timer(dev, t, check_card_timer);

    dev->check_card_flag = true;
    queue_work(dev->workqueue, &dev->main_work);
}
#endif

#ifdef BL_PROCFS_ENABLE
static int bl_stats_proc_open(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
    file->private_data = PDE_DATA(inode);
#else
    file->private_data = pde_data(inode);
#endif
    return 0;
}

static ssize_t bl_stats_proc_read(struct file *file, char __user *buffer,
                                size_t count, loff_t *ppos)
{
    char *str;
    size_t len;
    ssize_t ret = 0;
    struct bl_eth_device *dev = file->private_data;

    str = kasprintf(GFP_KERNEL, "max_tx_loop_elapsed_ms\t%lu\n"
            "max_netif_stopped_ms\t%lu\n"
            "sdio_write_timeo_cnt\t%lu\n"
            "sdio_write_retries\t%lu\n"
            "sdio_io_fail_cnt\t%lu\n",
            dev->max_tx_loop_elapsed_ms, dev->max_netif_stopped_ms,
            dev->sdio_write_timeo_cnt, dev->sdio_write_retries,
            dev->sdio_io_fail_cnt);
    if (str) {
        len = strlen(str) + 1;
        ret = simple_read_from_buffer(buffer, len, ppos, str, len);
        kfree_const(str);
    }
    return ret;
}

static const struct proc_ops stats_proc_ops = {
    .proc_open = bl_stats_proc_open,
    .proc_read = bl_stats_proc_read,
};

#define BL_PROC_STATS_NAME    "stats"

extern struct proc_dir_entry *bl_proc_dir;
/* BL_PROCFS_ENABLE */
#endif

static int bl_query_version(struct bl_eth_device *dev)
{
    uint16_t tx_len;
    uint8_t buf[256];
    rnm_base_msg_t *hdr;
    struct bl_version_query *query;

    tx_len = sizeof(rnm_base_msg_t) + sizeof(struct bl_version_query);
    hdr = (rnm_base_msg_t *)buf;
    hdr->cmd = BF1B_CMD_GET_DEV_VERSION;
    hdr->flags = 0;
    hdr->msg_id = 0;
    hdr->status = 0;
    hdr->session_id = 0;
    hdr->msg_id_replying = 0;

    query = (void *)&buf[sizeof(rnm_base_msg_t)];
    query->minor_version = BL_WNET_MINOR_VERSION;
    query->major_version = BL_WNET_MAJOR_VERSION;
    memset(query->reserved, 0, sizeof(query->reserved));
    /* call this api instead of bl_sdio_write to avoid race condition */
    return push_host2device_msg(buf, tx_len);
}

static int bl_do_handshake(struct bl_eth_device *dev)
{
    int err;
    int tryouts = 3;
    unsigned long long ms;

    init_completion(&dev->req_done);
    do {
        err = bl_query_version(dev);
        if (err) {
            pr_warn("failed to query verison %d\n", err);
            msleep(50);
            continue;
        }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
        reinit_completion(&dev->req_done);
#else
        INIT_COMPLETION(dev->req_done);
#endif
        ms = wait_for_completion_timeout(&dev->req_done, msecs_to_jiffies(500));
        if (ms) {
            pr_info("firmware version 0x%08x\n", dev->fw_version);
            return 0;
        }
    } while (--tryouts > 0);

    pr_warn("failed to retrieve firmware version\n");
    return -1;
}

static int bl_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
    int retval = 0;
    struct bl_eth_device *dev = NULL;
    struct net_device *netdev = NULL;

    mutex_lock(&gl_dev.mutex);
    if (gl_dev.eth_dev) {
        mutex_unlock(&gl_dev.mutex);
        printk("%s: multiple device not supported\n", BL_DRV_NAME);
        return -EINVAL;
    }
    mutex_unlock(&gl_dev.mutex);
    printk("vendor=0x%4.04X device=0x%4.04X class=%d func=%d\n", func->vendor, func->device, func->class, func->num);

    /*init net device*/
    netdev = alloc_etherdev(sizeof(struct bl_eth_device));
    if (!netdev) {
        retval = -ENOMEM;
        goto err_mem;
    }

    netdev->netdev_ops = &bl_eth_netdev_ops;
    netdev->watchdog_timeo = BL_ETH_TX_TIMEOUT;
    strcpy(netdev->name, "bleth%d");

    dev = netdev_priv(netdev);
    dev->func = func;
    dev->net = netdev;

    if (init_xfr_ctx(dev)) {
        retval = -ENOMEM;
        goto err_mem;
    }

    if ((dev->sdiocmd_buf = kmalloc(SDIOCMD_BUF_LEN, GFP_KERNEL)) == NULL) {
        printk(KERN_ERR "alloc sdiocmd_buf\n");
        retval = -ENOMEM;
        goto err_mem;
    }

    dev->workqueue = alloc_workqueue("BL_WORK_QUEUE", WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
    if (!dev->workqueue) {
        printk("creat workqueue failed\n");
        retval = -ENOMEM;
        goto err_create_wkq;
    }
    INIT_WORK(&dev->main_work, main_wq_hdlr);

    dev->txworkqueue = alloc_workqueue("BL_TX_WORK_QUEUE", WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
    if (!dev->txworkqueue) {
        printk("creat workqueue failed\n");
        retval = -ENOMEM;
        goto err_create_wkq;
    }
    INIT_WORK(&dev->tx_work, tx_wq_hdlr);

    skb_queue_head_init(&dev->sk_list);
    skb_queue_head_init(&dev->cmd_list);

    spin_lock_init(&dev->int_lock);
    spin_lock_init(&dev->main_proc_lock);
    spin_lock_init(&dev->tx_proc_lock);

    bl_sdio_read_cb_register(dev, sdio_read_callback, dev);
    /*init sdio device*/
    if ((retval = bl_sdio_init(dev))) {
        goto err_sdio_init;
    }

    retval = bl_eth_get_mac_addr(dev);
    if (retval)
        goto err_get_macaddr;
    bl_change_eth_mac(dev);

    sdio_set_drvdata(func, dev);
    SET_NETDEV_DEV(netdev, &func->dev);
    netdev->ethtool_ops = &ops;

    atomic_set(&dev->tty_credit, 0);
    skb_queue_head_init(&dev->tty_msg_list);

    retval = register_netdev(netdev);
    if (retval) {
        dev_err(&func->dev, "error registering netdev: %d\n", retval);
        retval = -EIO;
        goto err_register_netdev;
    }

    //carrier down and transmit queues stopped until packet from device
    netif_carrier_off(netdev);
    netif_tx_stop_all_queues(netdev);
    dev_info(&func->dev, "Device attached\n");

    mutex_lock(&gl_dev.mutex);
    gl_dev.status &= ~BL_DEVICE_STATUS_CARD_REMOVING;
#if CARD_DEAD_CHECK
    gl_dev.status &= ~BL_DEVICE_STATUS_CARD_DEAD;
#endif
    gl_dev.status |= BL_DEVICE_STATUS_CARD_PRESENT;

    gl_dev.eth_dev = dev;
    mutex_unlock(&gl_dev.mutex);

    send_hello(dev);
    /* device responds after it has received hello from host */
    bl_do_handshake(dev);

#if CARD_DEAD_CHECK
    timer_setup(&dev->check_card_timer, check_card_timer_cb, 0);
    dev->check_card_timer.expires = jiffies + msecs_to_jiffies(CHECK_CARD_DEAD_INTERVAL_MS);
    add_timer(&dev->check_card_timer);
#endif

#ifdef BL_PROCFS_ENABLE
    proc_create_data(BL_PROC_STATS_NAME, S_IFREG | 0644, bl_proc_dir,
                     &stats_proc_ops, dev);
#endif
    notify_daemon_simple_event(dev, CTL_PORT_MSG_CARD_INSERTED);

    return 0;

err_register_netdev:
err_get_macaddr:
    bl_sdio_deinit(dev);
err_sdio_init:
err_create_wkq:
    if (dev->workqueue) {
        destroy_workqueue(dev->workqueue);
    }
    if (dev->txworkqueue) {
        destroy_workqueue(dev->txworkqueue);
    }
err_mem:
    if (dev) {
        kfree(dev->sdiocmd_buf);
        deinit_xfr_ctx(dev);
    }
    if (netdev) {
        free_netdev(netdev);
    }

    return retval;
}

static void bl_sdio_remove(struct sdio_func *func)
{
    struct bl_eth_device *dev;

#ifdef BL_PROCFS_ENABLE
    remove_proc_entry(BL_PROC_STATS_NAME, bl_proc_dir);
#endif
    mutex_lock(&gl_dev.mutex);
    dev = sdio_get_drvdata(func);

    gl_dev.status |= BL_DEVICE_STATUS_CARD_REMOVING;
    gl_dev.status &= ~BL_DEVICE_STATUS_CARD_PRESENT;
#if CARD_DEAD_CHECK
    gl_dev.status &= ~BL_DEVICE_STATUS_CARD_DEAD;

    del_timer_sync(&dev->check_card_timer);
#endif
    /* stop netif tx flow */
    netif_carrier_off(dev->net);
    netif_tx_stop_all_queues(dev->net);
    /* stop sdio rx, then rx irq handler won't be called anymore */
    bl_sdio_deinit(dev);
    sdio_set_drvdata(func, NULL);
    /* flush pending work if any */
    flush_workqueue(dev->workqueue);
    flush_workqueue(dev->txworkqueue);
    destroy_workqueue(dev->workqueue);
    destroy_workqueue(dev->txworkqueue);
    /* there is no flow in the net device, release the net device */
    unregister_netdev(dev->net);
    deinit_xfr_ctx(dev);
    kfree(dev->sdiocmd_buf);
    free_netdev(dev->net);

    gl_dev.eth_dev = NULL;
    mutex_unlock(&gl_dev.mutex);

    dev_info(&func->dev, "Device disconnected\n");
}

static struct sdio_driver bl_sdio_drv = {
    .name     = "bl_eth",
    .id_table = bl_sdio_ids,
    .probe    = bl_sdio_probe,
    .remove   = bl_sdio_remove
};

int bl_register_eth_drv(void)
{
    return sdio_register_driver(&bl_sdio_drv);
}

void bl_unregister_eth_drv(void)
{
    sdio_unregister_driver(&bl_sdio_drv);
}
