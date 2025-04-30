#include <linux/module.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/firmware.h>

#include "bl_sdio_eth.h"
#include "bl_sdio.h"

void bl_get_rd_len(struct bl_eth_device *dev, u32 reg_l, u32 reg_u, u32 *len)
{
    u8 rd_len_l = 0;
    u8 rd_len_u = 0;

    bl_read_reg(dev, reg_l, &rd_len_l);
    bl_read_reg(dev, reg_u, &rd_len_u);

    *len = (rd_len_u << 8) + rd_len_l;
}
/*
 * This function gets the read port.
 *
 * If control port bit is set in MP read bitmap, the control port
 * is returned, otherwise the current read port is returned and
 * the value is increased (provided it does not reach the maximum
 * limit, in which case it is reset to 1)
 */
int bl_get_rd_port(struct bl_eth_device *dev, u32 *port)
{
    u32 rd_bitmap = dev->mp_rd_bitmap;

    if (!(rd_bitmap & (CTRL_PORT_MASK | dev->reg->data_port_mask))) {
        return -EBUSY;
    }

    if ((dev->has_control_mask) &&  (dev->mp_rd_bitmap & CTRL_PORT_MASK)) {
        dev->mp_rd_bitmap &= (u32) (~CTRL_PORT_MASK);
        *port = CTRL_PORT;
#if 0
        printk("ctrl: port=%d rd_bitmap=0x%08x -> 0x%08x\n",
        *port, rd_bitmap, dev->mp_rd_bitmap);
#endif
        return 0;
    }

    if (!(dev->mp_rd_bitmap & (1 << dev->curr_rd_port)))
        return -1;

    /* We are now handling the SDIO data ports */
    dev->mp_rd_bitmap &= (u32)(~(1 << dev->curr_rd_port));
    *port = dev->curr_rd_port;

    if (++(dev->curr_rd_port) == MAX_PORT_NUM)
        dev->curr_rd_port = dev->reg->start_rd_port;

#if 0
    printk("data: port=%d rd_bitmap=0x%08x -> 0x%08x\n",
        *port, rd_bitmap, dev->mp_rd_bitmap);
#endif

    return 0;
}

/*
 * This function gets the write port for data.
 *
 * The current write port is returned if available and the value is
 * increased (provided it does not reach the maximum limit, in which
 * case it is reset to 1)
 */
int bl_get_wr_port(struct bl_eth_device *dev, u32 *port)
{
    u32 wr_bitmap = dev->mp_wr_bitmap;

    if (!(wr_bitmap & dev->reg->data_port_mask)) {
        //printk("no available port\n");
        return -EBUSY;
    }

    if (wr_bitmap & (1 << dev->curr_wr_port)) {
        dev->mp_wr_bitmap &= (u32) (~(1 << dev->curr_wr_port));
        *port = dev->curr_wr_port;
        if (++(dev->curr_wr_port) == MAX_PORT_NUM)
            dev->curr_wr_port = dev->reg->start_wr_port;
    } else {
        //printk("no available port\n");
        return -EBUSY;
    }

    //printk("data: port=%d wr_bitmap=0x%08x -> 0x%08x\n",
    //  *port, wr_bitmap, dev->mp_wr_bitmap);

    return 0;
}

#define BL_SDIO_WRITE_MAX_TRYOUTS    3
/*
 * This function writes multiple data into SDIO card memory.
 *
 * This does not work in suspended mode.
 */
int bl_write_data_sync(struct bl_eth_device *dev, u8 *buffer, u32 pkt_len, u32 port)
{
    int i, ret;
    u8 blk_mode = (pkt_len < BL_SDIO_BLOCK_SIZE) ? BYTE_MODE : BLOCK_MODE;
    u32 blk_size = (blk_mode == BLOCK_MODE) ? BL_SDIO_BLOCK_SIZE : 1;
    u32 blk_cnt = (blk_mode == BLOCK_MODE) ? ((pkt_len + BL_SDIO_BLOCK_SIZE -1)/BL_SDIO_BLOCK_SIZE) : pkt_len;

    u32 ioport = (port & BL_SDIO_IO_PORT_MASK);

    sdio_claim_host(dev->func);
    i = 0;
    do {
        i++;
        ret = sdio_writesb(dev->func, ioport, buffer, blk_cnt * blk_size);
        if (ret < 0) {
            pr_err("sdio_writesb failed, %d\n", ret);
            if (i < BL_SDIO_WRITE_MAX_TRYOUTS) {
                dev->sdio_write_retries++;
            } else {
                break;
            }
        }
    } while (ret < 0);
    sdio_release_host(dev->func);

    if (ret) {
        pr_err("finally sdio_writesb failed, %d\n", ret);
        dev->sdio_io_fail_cnt++;
    }
    return ret;
}

/*
 * This function reads multiple data from SDIO card memory.
 */
int bl_read_data_sync_claim0(struct bl_eth_device *dev, u8 *buffer, u32 len, u32 port)
{
    int ret;
    u8 blk_mode = (len < BL_SDIO_BLOCK_SIZE) ? BYTE_MODE : BLOCK_MODE;
    u32 blk_size = (blk_mode == BLOCK_MODE) ? BL_SDIO_BLOCK_SIZE : 1;
    u32 blk_cnt = (blk_mode == BLOCK_MODE) ? ((len + BL_SDIO_BLOCK_SIZE -1)/BL_SDIO_BLOCK_SIZE) : len;
    u32 ioport = (port & BL_SDIO_IO_PORT_MASK);

    ret = sdio_readsb(dev->func, buffer, ioport, blk_cnt * blk_size);
    if (ret) {
        pr_err("sdio_readsb failed, %d\n", ret);
        dev->sdio_io_fail_cnt++;
    }
    return ret;
}

/*
 * This function reads multiple data from SDIO card memory.
 */
int bl_read_data_sync(struct bl_eth_device *dev, u8 *buffer, u32 len, u32 port)
{
    int ret;
    u8 blk_mode = (len < BL_SDIO_BLOCK_SIZE) ? BYTE_MODE : BLOCK_MODE;
    u32 blk_size = (blk_mode == BLOCK_MODE) ? BL_SDIO_BLOCK_SIZE : 1;
    u32 blk_cnt = (blk_mode == BLOCK_MODE) ? ((len + BL_SDIO_BLOCK_SIZE -1)/BL_SDIO_BLOCK_SIZE) : len;
    u32 ioport = (port & BL_SDIO_IO_PORT_MASK);

    sdio_claim_host(dev->func);
    ret = sdio_readsb(dev->func, buffer, ioport, blk_cnt * blk_size);
    sdio_release_host(dev->func);

    if (ret) {
        pr_err("sdio_readsb failed, %d\n", ret);
        dev->sdio_io_fail_cnt++;
    }
    return ret;
}

/*
 * This function reads data from SDIO card register.
 */
int bl_read_reg(struct bl_eth_device *dev, u32 reg, u8 *data)
{
    int ret = -1;
    u8 val;

    sdio_claim_host(dev->func);
    val = sdio_readb(dev->func, reg, &ret);
    sdio_release_host(dev->func);

    if (ret) {
        pr_err("sdio_readb failed, ret %d, reg %u\n", ret, reg);
        dev->sdio_io_fail_cnt++;
    }
    *data = val;
    return ret;
}

int bl_read16(struct bl_eth_device *dev, u32 reg, u16 *data)
{
    int ret = -1;
    u16 val;

    sdio_claim_host(dev->func);
    val = sdio_readw(dev->func, reg, &ret);
    sdio_release_host(dev->func);

    if (ret) {
        pr_err("sdio_readw failed, %d\n", ret);
        dev->sdio_io_fail_cnt++;
    }
    *data = val;
    return ret;
}

int bl_read32(struct bl_eth_device *dev, u32 reg, u32 *data)
{
    int ret = -1;
    u32 val;

    sdio_claim_host(dev->func);
    val = sdio_readl(dev->func, reg, &ret);
    sdio_release_host(dev->func);

    if (ret) {
        pr_err("sdio_readl failed, %d\n", ret);
        dev->sdio_io_fail_cnt++;
    }
    *data = val;
    return ret;
}

/*
 * This function write data to SDIO card register.
 */
int bl_write_reg(struct bl_eth_device *dev, u32 reg, u8 data)
{
    int ret;

    sdio_claim_host(dev->func);
    sdio_writeb(dev->func, data, reg, &ret);
    sdio_release_host(dev->func);

    if (ret) {
        pr_err("sdio_writeb failed, %d\n", ret);
        dev->sdio_io_fail_cnt++;
    }
    return ret;
}

