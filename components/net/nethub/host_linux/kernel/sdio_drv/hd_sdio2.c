
/**
 * @file hd_sdio2.c
 * @brief Host Driver SDIO2 Interface Driver Implementation
 * @author Host Driver Development Team
 * @date 2024
 * @details This module implements the SDIO2 interface driver for Host Driver
 *          wireless chipsets. It provides low-level SDIO communication,
 *          register access, firmware download, and data transfer capabilities
 *          with interrupt handling and power management support.
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/ktime.h>

#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include "hd_sdio_manage.h"
#include "hd_msg_ctrl.h"
#include "hd_eth_netdev.h"
#include "hd_tty.h"
#include "hd_sdio2.h"
#include "hd_debugfs.h"
#include "hd_speed_test.h"
#include "hd_bootrom.h"

/**
 * @brief Read data from SDIO card register
 * @param[in] card Pointer to SDIO card structure
 * @param[in] reg Register address to read from
 * @param[out] data Pointer to store read data
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function performs atomic register read operation
 *          with proper host claiming and error handling.
 */
static int hd_sdio2_read_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t *data)
{
    int ret = -1;
    uint8_t val;

    sdio_claim_host(card->func);
    val = sdio_readb(card->func, reg, &ret);
    sdio_release_host(card->func);

    if (data) {
        *data = val;
    }

    return ret;
}

/**
 * @brief Write data to SDIO card register
 * @param[in] card Pointer to SDIO card structure
 * @param[in] reg Register address to write to
 * @param[in] data Data value to write
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function performs atomic register write operation
 *          with proper host claiming and error handling.
 */
static int hd_sdio2_write_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t data)
{
    int ret;

    sdio_claim_host(card->func);
    sdio_writeb(card->func, data, reg, &ret);
    sdio_release_host(card->func);

    return ret;
}

/**
 * @brief Read data from FUNC_0 register
 * @param[in] card Pointer to SDIO card structure
 * @param[in] reg Register address to read from
 * @param[out] data Pointer to store read data
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function reads from SDIO function 0 registers which
 *          contain common card configuration and status information.
 */
static int hd_sdio2_read_fn0_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t *data)
{
    int ret = -1;
    uint8_t val;

    sdio_claim_host(card->func);
    val = sdio_f0_readb(card->func, reg, &ret);
    sdio_release_host(card->func);

    if (data) {
        *data = val;
    }

    return ret;
}

/**
 * @brief Write data to FUNC_0 register
 * @param[in] card Pointer to SDIO card structure
 * @param[in] reg Register address to write to
 * @param[in] data Data value to write
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function writes to SDIO function 0 registers which
 *          control common card configuration and operational parameters.
 */
static int hd_sdio2_write_fn0_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t data)
{
    int ret;

    sdio_claim_host(card->func);
    sdio_f0_writeb(card->func, data, reg, &ret);
    sdio_release_host(card->func);

    return ret;
}

/**
 * @brief Initiate upload transmission (read operation)
 * @param[in] card Pointer to SDIO card structure
 * @param[in,out] upld_desc Upload transfer descriptor
 * @param[in] retry_cnt Maximum retry attempts
 * @retval 0 Success
 * @retval -EIO I/O error
 * @retval -EBUSY Device busy (timeout)
 * @retval -ENOMEM Buffer size insufficient
 * @details This function reads data from the SDIO device using port-based
 *          queuing mechanism. It handles bitmap checking, length negotiation,
 *          and automatic retry logic for robust data transfer.
 */
static int hd_sdio2_upld_transmit(struct hd_sdio_card *card, struct sdio_trans_desc *upld_desc, int retry_cnt)
{
    int ret;
    int trans_len;
    int try_cnt = 0;
    uint8_t upld_port;
    uint8_t bitmap_byte;
    uint16_t dev_upld_len = 0;
#if SDIO2_HOST_UPLD_RD_LEN_EN
    uint8_t reg_temp[2] = { 0 };
#endif

    upld_port = (card->curr_upld_out & card->queue_mask);

    while ((card->sdio2_upld_bitmap & (0x01 << upld_port)) == 0) {
        /* Check bitmap register for available data */
        if (upld_port < 8) {
            /* Get bitmap lower byte */
            ret = hd_sdio2_read_reg(card, SDIO2_RD_BIT_MAP_OFFSET, &bitmap_byte);
            if (ret < 0) {
                SDIO_DRV_ERR(card, "upld read bitmap_l reg failed\n");
                ret = -EIO;
                goto return_out;
            }
            card->sdio2_upld_bitmap |= (uint16_t)bitmap_byte;
        } else {
            /* Get bitmap upper byte */
            ret = hd_sdio2_read_reg(card, SDIO2_RD_BIT_MAP_OFFSET + 1, &bitmap_byte);
            if (ret < 0) {
                SDIO_DRV_ERR(card, "upld read bitmap_h reg failed\n");
                ret = -EIO;
                goto return_out;
            }
            card->sdio2_upld_bitmap |= (uint16_t)bitmap_byte << 8;
        }

        if (card->sdio2_upld_bitmap & (0x01 << upld_port)) {
            break;
        }

        if (try_cnt < retry_cnt) {
            try_cnt += 1;
            udelay(10);
        } else {
            SDIO_DRV_DBG(card, "sdio_upld timeout\n");
            ret = -EBUSY;
            goto return_out;
        }
    }

#if SDIO2_HOST_UPLD_RD_LEN_EN
    if (card->rd_len_compress) {
        /* Read first byte */
        ret = hd_sdio2_read_reg(card, SDIO2_PORTn_UPLD_LEN_OFFSET(upld_port), &reg_temp[0]);
        if (ret < 0) {
            goto return_out;
        }
        /* 2-byte mode, read second byte */
        if (reg_temp[0] & 0x01) {
            ret = hd_sdio2_read_reg(card, SDIO2_PORTn_UPLD_LEN_OFFSET(upld_port) + 1, &reg_temp[1]);
            if (ret < 0) {
                goto return_out;
            }
        }
        dev_upld_len = (uint16_t)reg_temp[1] << 8 | (uint16_t)reg_temp[0];
        SDIO_DRV_DBG(card, "upld compress len reg raw 0x%04X\n", dev_upld_len);

        if (dev_upld_len & 0x02) {
            /* Block mode */
            dev_upld_len = (dev_upld_len >> 2) * card->func->cur_blksize;
        } else {
            /* Byte mode */
            dev_upld_len = (dev_upld_len >> 2);
        }
    } else {
        ret = hd_sdio2_read_reg(card, SDIO2_PORTn_UPLD_LEN_OFFSET(upld_port), &reg_temp[0]);
        if (ret < 0) {
            goto return_out;
        }
        ret = hd_sdio2_read_reg(card, SDIO2_PORTn_UPLD_LEN_OFFSET(upld_port) + 1, &reg_temp[1]);
        if (ret < 0) {
            goto return_out;
        }
        dev_upld_len = (uint16_t)reg_temp[1] << 8 | (uint16_t)reg_temp[0];
        SDIO_DRV_DBG(card, "upld len reg raw 0x%04X\n", dev_upld_len);
    }

#else
    /* Use default transfer length as dnld_max_size */
    dev_upld_len = card->dnld_max_size;
#endif

    if (dev_upld_len <= SDIO2_CMD53_BYTE_MOD_SIZE_MAX) {
        /* CMD53 byte mode, max size 511 */
        trans_len = dev_upld_len;
    } else {
        /* CMD53 block mode, align to block size */
        trans_len = (dev_upld_len + card->func->cur_blksize - 1) & ~(card->func->cur_blksize - 1);
    }
    SDIO_DRV_DBG(card, "upld_len %d, trans_len %d, rxbuff_len %d, curr_upld_out %d\n", dev_upld_len, trans_len, upld_desc->buff_len, card->curr_upld_out);

    /* Validate buffer constraints */
    if (trans_len > upld_desc->buff_len) {
        SDIO_DRV_ERR(card, "upld data size over buff\n");
        ret = -ENOMEM;
        goto return_out;
    }
    if (trans_len > card->upld_max_size) {
        SDIO_DRV_ERR(card, "upld data size over upld_max_size\n");
        ret = -ENOMEM;
        goto return_out;
    }

    /* Execute data read operation */
    sdio_claim_host(card->func);
    ret = sdio_readsb(card->func, upld_desc->buff, (card->sdio2_cmd53_ioport_offset + upld_port), trans_len);
    sdio_release_host(card->func);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sdio_readsb error %d", ret);
        goto return_out;
    }

#if SDIO2_HOST_UPLD_RD_LEN_EN
    upld_desc->data_len = dev_upld_len;
#else
    upld_desc->data_len = trans_len;
#endif

    /* Clear local bitmap and update port counter */
    card->sdio2_upld_bitmap &= ~(0x01 << upld_port);
    card->curr_upld_out += 1;

return_out:
    return ret;
}

/**
 * @brief Initiate download transmission (write operation)
 * @param[in] card Pointer to SDIO card structure
 * @param[in] dnld_desc Download transfer descriptor
 * @param[in] retry_cnt Maximum retry attempts
 * @retval 0 Success
 * @retval -EIO I/O error
 * @retval -EBUSY Device busy (timeout)
 * @retval -ENOMEM Buffer size insufficient
 * @details This function writes data to the SDIO device using port-based
 *          queuing mechanism. It handles bitmap checking, transfer mode
 *          selection, and automatic retry logic for reliable data transfer.
 */
static int hd_sdio2_dnld_transmit(struct hd_sdio_card *card, struct sdio_trans_desc *dnld_desc, int retry_cnt)
{
    int ret;
    int trans_len;
    int try_cnt = 0;
    uint8_t dnld_port;
    uint8_t bitmap_byte;

    dnld_port = (card->curr_dnld_out & card->queue_mask);

    while ((card->sdio2_dnld_bitmap & (0x01 << dnld_port)) == 0) {
        /* Check bitmap register for available space */
        if (dnld_port < 8) {
            /* Get bitmap lower byte */
            ret = hd_sdio2_read_reg(card, SDIO2_WR_BIT_MAP_OFFSET, &bitmap_byte);
            if (ret < 0) {
                SDIO_DRV_ERR(card, "dnld read bitmap_l reg failed\n");
                ret = -EIO;
                goto return_out;
            }
            card->sdio2_dnld_bitmap |= (uint16_t)bitmap_byte;
        } else {
            /* Get bitmap upper byte */
            ret = hd_sdio2_read_reg(card, SDIO2_WR_BIT_MAP_OFFSET + 1, &bitmap_byte);
            if (ret < 0) {
                SDIO_DRV_ERR(card, "dnld read bitmap_h reg failed\n");
                ret = -EIO;
                goto return_out;
            }
            card->sdio2_dnld_bitmap |= (uint16_t)bitmap_byte << 8;
        }

        if (card->sdio2_dnld_bitmap & (0x01 << dnld_port)) {
            break;
        }

        if (try_cnt < retry_cnt) {
            try_cnt += 1;
            udelay(10);
        } else {
            SDIO_DRV_DBG(card, "sdio_dnld timeout\n");
            ret = -EBUSY;
            goto return_out;
        }
    }

    /* Determine transfer length based on data size */
    if (dnld_desc->data_len >= SDIO2_CMD53_BYTE_MOD_SIZE_MAX) {
        /* Block mode, align to block size */
        trans_len = (dnld_desc->data_len + card->func->cur_blksize - 1) / card->func->cur_blksize * card->func->cur_blksize;
    } else {
        /* Byte mode, max size 512 */
        trans_len = dnld_desc->data_len;
    }

    SDIO_DRV_DBG(card, "dnld_len %d, trans_len %d, txbuff_len %d, dnld_port %d\n", dnld_desc->data_len, trans_len, dnld_desc->buff_len, dnld_port);

    /* Validate buffer constraints */
    if (trans_len > dnld_desc->buff_len) {
        SDIO_DRV_ERR(card, "dnld data size over buff\n");
        ret = -ENOMEM;
        goto return_out;
    }
    if (trans_len > card->dnld_max_size) {
        SDIO_DRV_ERR(card, "dnld data size over dnld_max_size\n");
        ret = -ENOMEM;
        goto return_out;
    }

    /* Execute data write operation */
    sdio_claim_host(card->func);
    ret = sdio_writesb(card->func, (card->sdio2_cmd53_ioport_offset + dnld_port), dnld_desc->buff, trans_len);
    sdio_release_host(card->func);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sdio_writesb error %d\n", ret);
        goto return_out;
    }

    /* Clear local bitmap and update port counter */
    card->sdio2_dnld_bitmap &= ~(0x01 << dnld_port);
    card->curr_dnld_out += 1;

return_out:
    return ret;
}

/**
 * @brief SDIO card-to-host interrupt handler
 * @param[in] func SDIO function structure
 * @details This function handles card-to-host interrupts triggered by
 *          upload events. It clears the interrupt status and invokes
 *          the registered callback for upload processing.
 */
static void hd_sdio2_card2host_irq(struct sdio_func *func)
{
    int ret;
    struct hd_sdio_card *card = sdio_get_drvdata(func);
    if (!card) {
        return;
    }
    SDIO_DRV_DBG(card, "card2host_irq come");

    /* Clear interrupt status */
    ret = hd_sdio2_write_reg(card, SDIO2_HOST_INT_STATUS_OFFSET, 0);
    if (ret < 0) {
        return;
    }

    /* Invoke upload interrupt callback */
    if (card->upld_irq_cb) {
        card->upld_irq_cb(card->sdio_manage);
    }
}

/**
 * @brief Register/unregister upload interrupt callback
 * @param[in] card Pointer to SDIO card structure
 * @param[in] upld_irq_cb Upload interrupt callback function (NULL to disable)
 * @retval 0 Success
 * @retval <0 Error code from SDIO operations
 * @details This function enables or disables card-to-host interrupts
 *          for upload events. When enabled, it registers the interrupt
 *          handler and configures interrupt masks.
 */
static int hd_sdio2_upld_irq_register(struct hd_sdio_card *card, card2host_irq_cb upld_irq_cb)
{
    int ret;

    if (upld_irq_cb) {
        card->upld_irq_cb = upld_irq_cb;
        /* Enable card-to-host upload event */
        ret = hd_sdio2_write_reg(card, SDIO2_HOST_INT_MASK_OFFSET, SDIO2_HOST_INT_UMSK_UPLD);
        if (ret < 0) {
            return ret;
        }
        /* Enable card-to-host interrupt */
        sdio_claim_host(card->func);
        ret = sdio_claim_irq(card->func, hd_sdio2_card2host_irq);
        sdio_release_host(card->func);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "sdio_claim_irq failed: %d\n", ret);
            return ret;
        }
    } else {
        /* Disable interrupt */
        sdio_claim_host(card->func);
        ret = sdio_release_irq(card->func);
        sdio_release_host(card->func);
        card->upld_irq_cb = NULL;
        if (ret < 0) {
            SDIO_DRV_ERR(card, "sdio_release_irq failed: %d\n", ret);
            return ret;
        }

        /* Disable card-to-host upload event */
        ret = hd_sdio2_write_reg(card, SDIO2_HOST_INT_MASK_OFFSET, 0);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}

/**
 * @brief Enable SDIO function
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function enables the SDIO function for operation,
 *          making it ready for data transfer and register access.
 */
static int hd_sdio2_enable_func(struct hd_sdio_card *card)
{
    int ret;

    SDIO_DRV_INFO(card, "hd_sdio2_enable_func\n");

    /* enable func */
    sdio_claim_host(card->func);
    ret = sdio_enable_func(card->func);
    sdio_release_host(card->func);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to enable func\n");
        return ret;
    }

    return 0;
}

/**
 * @brief Disable SDIO function
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function disables the SDIO function, stopping
 *          data transfer capabilities and preparing for shutdown.
 */
static int hd_sdio2_disable_func(struct hd_sdio_card *card)
{
    int ret;
    SDIO_DRV_INFO(card, "hd_sdio2_disable_func\n");

    /* disable func */
    sdio_claim_host(card->func);
    ret = sdio_disable_func(card->func);
    sdio_release_host(card->func);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to disable func\n");
        return ret;
    }

    return 0;
}

/**
 * @brief Set SDIO block size
 * @param[in] card Pointer to SDIO card structure
 * @param[in] size Block size to set
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function configures the SDIO block size for
 *          block mode transfers, optimizing transfer efficiency.
 */
static int hd_sdio2_set_block_size(struct hd_sdio_card *card, uint16_t size)
{
    int ret;

    SDIO_DRV_INFO(card, "hd_sdio2_set_block_size  %d -> %d\n", card->func->cur_blksize, size);

    /* Set block size */
    sdio_claim_host(card->func);
    ret = sdio_set_block_size(card->func, size);
    sdio_release_host(card->func);

    if (ret < 0 || card->func->cur_blksize != size) {
        SDIO_DRV_ERR(card, "set block size error!, cur_blksize %d\n", card->func->cur_blksize);
        return ret;
    }

    return 0;
}

/**
 * @brief Dump SDIO2 card information
 * @param[in] card Pointer to SDIO card structure
 * @details This function outputs comprehensive card configuration
 *          and status information for debugging and monitoring purposes.
 */
static void hd_sdio2_info_dump(struct hd_sdio_card *card)
{
    /* dump queue info */
    SDIO_DRV_INFO(card, "sdio2_info dump:\n");
    SDIO_DRV_INFO(card, "sdio_boot mode: %s\n", (card->sdio_boot) ? "true" : "false");
    SDIO_DRV_INFO(card, "app_run mode: %s\n", (card->app_run) ? "true" : "false");
    SDIO_DRV_INFO(card, "rd_len_compress mode: %s\n", (card->rd_len_compress) ? "true" : "false");
    SDIO_DRV_INFO(card, "dnld_size_max: %d\n", card->dnld_max_size);
    SDIO_DRV_INFO(card, "upld_size_max: %d\n", card->upld_max_size);
    SDIO_DRV_INFO(card, "queue_depth: %d\n", card->queue_depth);
    SDIO_DRV_INFO(card, "queue_mask: 0x%04X\n", card->queue_mask);
    SDIO_DRV_INFO(card, "sdio2_cmd53_ioport_offset: 0x%04X\n", card->sdio2_cmd53_ioport_offset);
    SDIO_DRV_INFO(card, "curr_dnld_out: 0x%04X\n", card->curr_dnld_out);
    SDIO_DRV_INFO(card, "sdio2_dnld_bitmap: 0x%04X\n", card->sdio2_dnld_bitmap);
    SDIO_DRV_INFO(card, "curr_upld_out: 0x%04X\n", card->curr_upld_out);
    SDIO_DRV_INFO(card, "sdio2_upld_bitmap: 0x%04X\n", card->sdio2_upld_bitmap);
}

/**
 * @brief Update SDIO2 card information and configuration
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from register operations
 * @details This function reads and updates card configuration including
 *          I/O port offset, transfer size limits, status flags, and
 *          queue parameters. It configures the card for optimal operation.
 */
static int hd_sdio2_info_update(struct hd_sdio_card *card)
{
    int ret = 0;
    uint8_t io_reg;
    uint32_t ioport_base = 0;

    SDIO_DRV_INFO(card, "hd_sdio2_info_update\n");

    /* 1. read io port offset */
    /* io port byte 0 */
    ret = hd_sdio2_read_reg(card, SDIO2_IO_PORT_OFFSET, &io_reg);
    if (ret < 0) {
        goto return_out;
    }
    ioport_base |= (uint32_t)io_reg;
    /* io port byte 1 */
    ret = hd_sdio2_read_reg(card, SDIO2_IO_PORT_OFFSET + 1, &io_reg);
    if (ret < 0) {
        goto return_out;
    }
    ioport_base |= (uint32_t)io_reg << 8;
    /* io port byte 2 */
    ret = hd_sdio2_read_reg(card, SDIO2_IO_PORT_OFFSET + 2, &io_reg);
    if (ret < 0) {
        goto return_out;
    }
    ioport_base |= (uint32_t)io_reg << 16;
    /* */
    card->sdio2_cmd53_ioport_offset = ioport_base;

    /* 2. read dnld max size (slave buff size) */
    ret = hd_sdio2_read_reg(card, SDIO2_SCRATCH0_1_OFFSET, &io_reg);
    if (ret < 0) {
        goto return_out;
    }
    if (io_reg) {
        card->dnld_max_size = (uint32_t)io_reg * SDIO2_SIZE_CONSULT_MULTIPLE;
    } else {
        SDIO_DRV_ERR(card, "dnld_size_max is zero, use default values, continue...\n");
        card->dnld_max_size = SDIO2_HOST_DNLD_SIZE_MAX_DEFAULT;
    }

    /* 3. read sdio status flag */
    ret = hd_sdio2_read_reg(card, SDIO2_SCRATCH0_2_OFFSET, &io_reg);
    if (ret < 0) {
        goto return_out;
    }
    SDIO_DRV_INFO(card, "status flag 0x%02X\n", io_reg);
    if (io_reg & (SDIO2_STA_FLAG_APP_RUN | SDIO2_STA_FLAG_NOT_SDIO_BOOT)) {
        card->app_run = true;
    } else {
        card->app_run = false;
        card->sdio_boot = true;
    }
    card->rd_len_compress = (io_reg & SDIO2_STA_FLAG_RD_LEN_COMPRESS_SUP) ? true : false;

    /* 4. update port num and mask */
    if (card->app_run) {
        card->queue_depth = SDIO2_HOST_PORT_NUM_MAX;
        card->queue_mask = SDIO2_HOST_PORT_NUM_MAX - 1;
    } else {
        card->queue_depth = SDIO2_HOST_BOOTROM_PORT_NUM_MAX;
        card->queue_mask = SDIO2_HOST_BOOTROM_PORT_NUM_MAX - 1;
    }

    /* 5. update dnld/upld queue out ptr */
    ret = hd_sdio2_read_reg(card, SDIO2_SCRATCH0_3_OFFSET, &io_reg);
    if (ret < 0) {
        goto return_out;
    }
    card->curr_dnld_out = (io_reg >> 4) & card->queue_mask;
    card->curr_dnld_in = card->curr_dnld_out;
    card->curr_upld_out = io_reg & card->queue_mask;
    card->curr_upld_in = card->curr_upld_out;

    card->upld_max_size = SDIO2_HOST_UPLD_SIZE_MAX;

return_out:
    if (ret < 0) {
        SDIO_DRV_ERR(card, "update_queue_info error\n");
    }

    hd_sdio2_info_dump(card);
    return ret;
}

/**
 * @brief Wait for SDIO card to become ready
 * @param[in] card Pointer to SDIO card structure
 * @param[in] timeout_ms Timeout in milliseconds
 * @retval 0 Success - card is ready
 * @retval -ETIME Timeout occurred
 * @retval <0 Other error code from register operations
 * @details This function polls the card ready status register until
 *          the card signals readiness or timeout occurs. It uses
 *          efficient sleep intervals to avoid busy waiting.
 */
static int hd_sdio2_wait_card_ready(struct hd_sdio_card *card, int timeout_ms)
{
    int ret;
    uint8_t io_reg;
    ktime_t time_s;

    time_s = ktime_get_boottime();

    while (1) {
        ret = hd_sdio2_read_reg(card, SDIO2_SCRATCH0_1_OFFSET, &io_reg);
        if (ret < 0) {
            goto return_out;
        }

        if (io_reg) {
            break;
        } else if ((ktime_get_boottime() - time_s) / (1000 * 1000) < timeout_ms) {
            usleep_range(100, 100);
        } else {
            ret = -ETIME;
            goto return_out;
        }
    }

return_out:
    if (ret < 0) {
        SDIO_DRV_ERR(card, "wait_card_ready error\n");
    }
    return ret;
}

/**
 * @brief Signal host readiness to SDIO card
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from register write operation
 * @details This function writes the host maximum upload size capability
 *          to the scratch register to inform the card that host is ready
 *          for communication with specified transfer parameters.
 */
static int hd_sdio2_host_ready(struct hd_sdio_card *card)
{
    int ret;

    ret = hd_sdio2_write_reg(card, SDIO2_SCRATCH0_0_OFFSET, SDIO2_HOST_UPLD_SIZE_MAX / SDIO2_SIZE_CONSULT_MULTIPLE);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "set host_ready error\n");
        return ret;
    }

    return 0;
}

/**
 * @brief Clean host and card readiness status
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from register write operations
 * @details This function clears both host and card ready status registers
 *          to reset the communication handshake state. Used during
 *          shutdown or reinitialization sequences.
 */
static int hd_sdio2_clean_ready(struct hd_sdio_card *card)
{
    int ret;

    ret = hd_sdio2_write_reg(card, SDIO2_SCRATCH0_0_OFFSET, 0);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "clean host_ready error\n");
        return ret;
    }

    ret = hd_sdio2_write_reg(card, SDIO2_SCRATCH0_1_OFFSET, 0);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "clean card_ready error\n");
        return ret;
    }

    return 0;
}

/**
 * @brief Initialize SDIO2 card and driver interface
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from initialization operations
 * @details This function performs complete SDIO2 initialization including
 *          quirks setup, function enabling, block size configuration,
 *          handshake synchronization, and operation callback registration.
 *          It establishes the complete driver interface for data transfer.
 */
static int hd_sdio2_init(struct hd_sdio_card *card)
{
    int ret;

    /* Set the MMC_QUIRK_LENIENT_FN0 bit to allow access to non-vendor registers. */
    card->func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
    /* Use block mode for transferring over one block size of data */
    card->func->card->quirks |= MMC_QUIRK_BLKSZ_FOR_BYTE_MODE;
    /**
     * @brief Manual block size setting for 606 SDU func1
     * @details 606 SDU func1 lacks cistpl_funce block which prevents automatic
     *          setting, so manual configuration is required here
     */
    card->func->max_blksize = 512;
    card->func->enable_timeout = 200;

    /* sdio2.0 */
    card->sdio_hw_ver = 2;

    /* 1. enable or reset func */
    ret = hd_sdio2_enable_func(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to enable func\n");
        goto exit;
    }

    /* 2. set block size */
    ret = hd_sdio2_set_block_size(card, SDIO2_HOST_BLOCK_SIZE);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to set block size\n");
        goto exit;
    }

    /* 3. waiting card ready */
    ret = hd_sdio2_wait_card_ready(card, 100);
    if (ret == -ETIME) {
        SDIO_DRV_ERR(card, "failed to wait_card_ready, But continue.\n");
    } else if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to wait_card_ready\n");
        goto exit;
    }

    /* 4. update queue info */
    ret = hd_sdio2_info_update(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to update_queue_info\n");
        goto exit;
    }

    /* 5. host ready */
    ret = hd_sdio2_host_ready(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to set host_ready\n");
        goto exit;
    }

    /* 6. sdio2 ops register */
    /* */
    card->sdio_read_reg = hd_sdio2_read_reg;
    card->sdio_write_reg = hd_sdio2_write_reg;
    /* */
    card->sdio_read_fn0_reg = hd_sdio2_read_fn0_reg;
    card->sdio_write_fn0_reg = hd_sdio2_write_fn0_reg;
    /* */
    card->sdio_read_custom_reg = NULL;
    card->sdio_write_custom_reg = NULL;
    /* */
    card->sdio_upld_transf = hd_sdio2_upld_transmit;
    card->sdio_dnld_transf = hd_sdio2_dnld_transmit;
    /* */
    card->sdio_upld_irq_register = hd_sdio2_upld_irq_register;

exit:
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to init function\n");
    }
    return ret;
}

/**
 * @brief Reinitialize SDIO2 interface after bootrom phase
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from reinitialization operations
 * @details This function reinitializes the SDIO2 interface after firmware
 *          has been loaded and started. It resets queue counters, waits for
 *          card readiness, updates configuration, and reestablishes the
 *          communication handshake for application mode operation.
 */
static int hd_sdio2_boot_reinit(struct hd_sdio_card *card)
{
    int ret;

    /* 0. reset sdio2 dnld/upld queue int/out */
    card->curr_dnld_in = 0;
    card->curr_dnld_out = 0;
    card->curr_upld_in = 0;
    card->curr_upld_out = 0;

    /* 1. waiting card ready */
    ret = hd_sdio2_wait_card_ready(card, 200);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to wait_card_ready\n");
        goto exit;
    }

    /* 2. update queue info */
    ret = hd_sdio2_info_update(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to update_queue_info\n");
        goto exit;
    }

    /* 3. host ready */
    ret = hd_sdio2_host_ready(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to set host_ready\n");
        goto exit;
    }

exit:
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to reinit sdio after bootrom\n");
    }
    return ret;
}

/**
 * @brief SDIO2 device removal function
 * @param[in] func SDIO function structure
 * @details This function handles cleanup when SDIO2 device is removed.
 *          It destroys associated components, releases resources, and
 *          performs proper driver cleanup sequence.
 */
static void hd_sdio2_remove(struct sdio_func *func)
{
    struct hd_sdio_card *card;

    pr_notice("[sdio fn%d]: hd_sdio_remove\n", func->num);

    card = sdio_get_drvdata(func);
    if (!card) {
        pr_notice("[sdio fn%d]: card is NULL\n", func->num);
        return;
    }

    if (card->netdev) {
        /* stop netdev */
        hd_sdio_eth_netdev_deinit(card->netdev);
        card->netdev = NULL;
    }

    if (card->tty_msg) {
        /* stop tty devices */
        hd_tty_deinit(card->tty_msg);
        card->tty_msg = NULL;
    }

    /* stop speed test thread */
    hd_speed_test_thread_destroy_all();

    if (card->sdio_manage) {
        /* deinit msg ctrl */
        if (card->sdio_manage->msg_ctrl) {
            hd_sdio_msg_ctrl_deinit(card->sdio_manage->msg_ctrl);
            card->sdio_manage->msg_ctrl = NULL;
        }

        /* deinit sdio manage */
        hd_sdio_manage_deinit(card->sdio_manage);
        card->sdio_manage = NULL;
    }

    /* disable function */
    hd_sdio2_disable_func(card);

    /* dump queue info */
    hd_sdio2_info_dump(card);

    sdio_set_drvdata(func, NULL);
    kfree(card);

    /* unregister */
    // hd_debugfs_unregister(card);
}

/**
 * @brief SDIO2 device probe function
 * @param[in] func SDIO function structure
 * @param[in] id SDIO device ID
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function handles SDIO2 device initialization including
 *          function enabling, buffer allocation, interrupt configuration,
 *          and registration of all driver components.
 */
static int hd_sdio2_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
    int ret = 0;
    struct hd_sdio_card *card = NULL;
    struct hd_sdio_manage *sdio_manage = NULL;
    struct hd_msg_ctrl *msg_ctrl = NULL;

    pr_notice("[sdio fn%d]: hd_sdio_probe\n", func->num);
    pr_notice("[sdio fn%d]: VID:0x%04X PID:0x%04X Class:%d Func:%d\n",
              func->num, func->vendor, func->device, func->class, func->num);
    pr_notice("[sdio fn%d]: max_blksize %d, timeout %d\n",
              func->num, func->max_blksize, func->enable_timeout);

    /* malloc hd_sdio_card struct */
    card = kzalloc(sizeof(struct hd_sdio_card), GFP_KERNEL);
    if (!card) {
        ret = -ENOMEM;
        goto exit;
    }

    card->func = func;
    sdio_set_drvdata(func, card);

    /* reset func, init sdio dev bitmap, and set host ready */
    ret = hd_sdio2_init(card);
    if (ret < 0) {
        goto exit;
    }
    msleep(2);

    /* TODO: download fw */
    if (card->sdio_boot) {
        /* 1. download firmware */
        ret = hd_bootrom_download_firmware(card, NULL);
        if (ret < 0) {
            goto exit;
        }
        /* 2. clean ready status */
        ret = hd_sdio2_clean_ready(card);
        if (ret < 0) {
            goto exit;
        }
        /* 3. run firmware */
        ret = hd_bootrom_firmware_run(card);
        if (ret < 0) {
            goto exit;
        }
        msleep(5);
        /* 4. reinit sdio */
        ret = hd_sdio2_boot_reinit(card);
        if (ret < 0) {
            goto exit;
        }
    }

    /* async sdio manage init */
    ret = hd_sdio_manage_init(&sdio_manage, card);
    if (ret < 0) {
        goto exit;
    }

    /* msg_ctrl init */
    ret = hd_sdio_msg_ctrl_init(&msg_ctrl, sdio_manage);
    if (ret < 0) {
        goto sdio_manage_exit;
    }

    /* do something */

    /* speed test */
    // ret = hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_1, false);
    // if (ret < 0) {
    //     goto msg_ctrl_exit;
    // }

    /* Multithreaded speed test */
    hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_1, true);
    hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_2, true);
    hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_3, true);

    /* create netdev */
    card->netdev = hd_sdio_eth_netdev_init(msg_ctrl, HD_MSG_TAG_ETH);
    if (card->netdev == NULL) {
        SDIO_DRV_ERR(card, "failed to create netdev\n");
        ret = -ENOMEM;
        goto msg_ctrl_exit;
    }

    /* create tty devices */
    card->tty_msg = hd_tty_init(msg_ctrl, HD_MSG_TAG_TTY);
    if (card->tty_msg == NULL) {
        SDIO_DRV_ERR(card, "failed to create tty devices\n");
        /* Continue without TTY - not critical */
    }

#if 0
    ret = hd_debugfs_register(card);
    if (ret) {
        goto exit;
    }
#endif

msg_ctrl_exit:
    if (ret < 0) {
        hd_sdio_msg_ctrl_deinit(msg_ctrl);
    }

sdio_manage_exit:
    if (ret < 0) {
        hd_sdio_manage_deinit(sdio_manage);
    }
exit:
    if (ret < 0) {
        pr_err("[sdio fn%d]: probe failed\n", func->num);
        /*  */
        sdio_set_drvdata(func, NULL);
        /* remove card2host int */
        hd_sdio2_upld_irq_register(card, NULL);
        /* disable func */
        hd_sdio2_disable_func(card);
        kfree(card);
    }
    pr_notice("[sdio fn%d]: hd_sdio_probe exit: %d\n", func->num, ret);
    return ret;
}

static const struct sdio_device_id hd_sdio2_ids[] = {
    { SDIO_DEVICE(0x424c, 0x606) }, /* bl602 bl616 bl616L */
    { /* end: all zeroes */ },
};
MODULE_DEVICE_TABLE(sdio, hd_sdio2_ids);

struct sdio_driver hd_sdio2 = {
    .name = "hd_sdio2",
    .id_table = hd_sdio2_ids,
    .probe = hd_sdio2_probe,
    .remove = hd_sdio2_remove,
};
