
/**
 * @file hd_sdio3.c
 * @brief Host Driver SDIO3 Interface Driver Implementation
 * @author Host Driver Development Team
 * @date 2024
 * @details This module implements the SDIO3 interface driver for enhanced
 *          Host Driver wireless chipsets. It provides advanced SDIO communication
 *          features, improved performance characteristics, and extended functionality
 *          compared to SDIO2 with enhanced interrupt handling and data transfer
 *          optimizations.
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/jiffies.h>

#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include "hd_sdio_manage.h"
#include "hd_msg_ctrl.h"
#include "hd_eth_netdev.h"
#include "hd_tty.h"
#include "hd_sdio3.h"
#include "hd_debugfs.h"
#include "hd_speed_test.h"
#include "hd_bootrom.h"

static int hd_sdio3_info_update(struct hd_sdio_card *card);

/**
 * @brief Read data from SDIO card register
 * @param[in] card Pointer to SDIO card structure
 * @param[in] reg Register address to read from
 * @param[out] data Pointer to store read data
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function performs atomic register read operation
 *          with proper host claiming and error handling for SDIO3 interface.
 */
static int hd_sdio3_read_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t *data)
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
 *          with proper host claiming and error handling for SDIO3 interface.
 */
static int hd_sdio3_write_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t data)
{
    int ret;

    sdio_claim_host(card->func);
    sdio_writeb(card->func, data, reg, &ret);
    sdio_release_host(card->func);

    return ret;
}

/*
 * This function read from FUNC_0 register.
 */
static int hd_sdio3_read_fn0_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t *data)
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

/*
 * This function write data to FUNC_0 register.
 */
static int hd_sdio3_write_fn0_reg(struct hd_sdio_card *card, uint32_t reg, uint8_t data)
{
    int ret;

    sdio_claim_host(card->func);
    sdio_f0_writeb(card->func, data, reg, &ret);
    sdio_release_host(card->func);

    return ret;
}

/*
 *  read from custom register (FN0_CIS5).
 */
static int hd_sdio3_read_custom_reg(struct hd_sdio_card *card, uint16_t reg_offset, void *buff, uint16_t len)
{
    int i, ret;
    uint8_t val;

    sdio_claim_host(card->func);

    for (i = 0; i < len; i++) {
        val = sdio_f0_readb(card->func, SDIO3_HOST_CUSTOM_REG_ADDR + reg_offset + i, &ret);
        if (ret < 0) {
            break;
        }
        ((uint8_t *)buff)[i] = val;
    }

    sdio_release_host(card->func);

    return ret;
}

/*
 *  write to custom register (FN0_CIS5).
 */
static int hd_sdio3_write_custom_reg(struct hd_sdio_card *card, uint16_t reg_offset, void *buff, uint16_t len)
{
    int i, ret;

    sdio_claim_host(card->func);

    for (i = 0; i < len; i++) {
        sdio_f0_writeb(card->func, ((uint8_t *)buff)[i], SDIO3_HOST_CUSTOM_REG_ADDR + reg_offset + i, &ret);
        if (ret < 0) {
            break;
        }
    }

    sdio_release_host(card->func);
    return ret;
}

/**
 * @brief Initiate SDIO3 upload transmission (enhanced read operation)
 * @param[in] card Pointer to SDIO card structure
 * @param[in,out] upld_desc Upload transfer descriptor
 * @param[in] retry_cnt Maximum retry attempts
 * @retval 0 Success
 * @retval -EIO I/O error
 * @retval -EBUSY Device busy (timeout)
 * @retval -ENOMEM Buffer size insufficient
 * @details This function implements enhanced SDIO3 read operation with
 *          improved queue management, custom register handling, and
 *          optimized data transfer protocols for higher performance.
 */
static int hd_sdio3_upld_transmit(struct hd_sdio_card *card, struct sdio_trans_desc *upld_desc, int retry_cnt)
{
    int ret, ret_1;
    int trans_len;
    int try_cnt = 0;
    uint16_t dev_upld_len = 0;
#if SDIO3_HOST_UPLD_RD_LEN_EN
    uint8_t reg_temp[2] = { 0 };
#endif

    while (card->curr_upld_in == card->curr_upld_out) {
        /* read upld_in from custom reg */
        ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN,
                                       &(card->curr_upld_in), 1);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "upld read_custom_reg failed, %d\n", ret);
            ret = -EIO;
            goto return_out;
        }

        if (card->curr_upld_in != card->curr_upld_out) {
            break;
        }

        if (try_cnt < retry_cnt) {
            try_cnt += 1;
            udelay(10);
        } else {
            SDIO_DRV_DBG(card, "sdio_upld timeout, in:%d, out:%d\n", card->curr_upld_in, card->curr_upld_out);
            ret = -EBUSY;
            goto return_out;
        }
    }

#if SDIO3_HOST_UPLD_RD_LEN_EN
    /* read frist byte */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE + (card->curr_upld_out & card->queue_mask) * 2,
                                   &reg_temp[0], 1);
    if (ret < 0) {
        goto return_out;
    }
    /* 2-byte mode, read second byte */
    if (reg_temp[0] & 0x01) {
        ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE + (card->curr_upld_out & card->queue_mask) * 2 + 1,
                                       &reg_temp[1], 1);
        if (ret < 0) {
            goto return_out;
        }
    }
    dev_upld_len = (uint16_t)reg_temp[1] << 8 | (uint16_t)reg_temp[0];
    SDIO_DRV_DBG(card, "upld len raw 0x%04X\n", dev_upld_len);

    if (dev_upld_len & 0x02) {
        /* block mode */
        dev_upld_len = (dev_upld_len >> 2) * card->func->cur_blksize;
    } else {
        /* byte mode */
        dev_upld_len = (dev_upld_len >> 2);
    }

#else
    /* trans_len = dnld_max_size */
    dev_upld_len = card->dnld_max_size;
#endif

    if (dev_upld_len <= SDIO3_CMD53_BYTE_MOD_SIZE_MAX(card->func->num)) {
        /* cmd53 byte mode, max size 511 */
        trans_len = dev_upld_len;
    } else {
        /* cmd53 block mode, align to block size */
        trans_len = (dev_upld_len + card->func->cur_blksize - 1) & ~(card->func->cur_blksize - 1);
    }
    SDIO_DRV_DBG(card, "upld_len %d, trans_len %d, rxbuff_len%d, upld_out %d\n", dev_upld_len, trans_len, upld_desc->buff_len, card->curr_upld_out);

    /* check len */
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

    /* start read data */
    sdio_claim_host(card->func);
    // ret = sdio_readsb(card->func, upld_desc->buff, 0, trans_len);
    ret = sdio_memcpy_fromio(card->func, upld_desc->buff, 0, trans_len);
    sdio_release_host(card->func);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sdio_memcpy_fromio error %d\n", ret);

        /* update info */
        SDIO_DRV_INFO(card, "try to update queue info\n");
        ret_1 = hd_sdio3_info_update(card);
        if (ret_1 < 0) {
            SDIO_DRV_ERR(card, "failed to update_queue_info\n");
            goto return_out;
        }

        goto return_out;
    }

#if SDIO3_HOST_UPLD_RD_LEN_EN
    upld_desc->data_len = dev_upld_len;
#else
    upld_desc->data_len = trans_len;
#endif

    card->curr_upld_out++;

return_out:
    return ret;
}

/**
 * @brief Initiate SDIO3 download transmission (enhanced write operation)
 * @param[in] card Pointer to SDIO card structure
 * @param[in] dnld_desc Download transfer descriptor
 * @param[in] retry_cnt Maximum retry attempts
 * @retval 0 Success
 * @retval -EIO I/O error
 * @retval -EBUSY Device busy (timeout)
 * @retval -ENOMEM Buffer size insufficient
 * @details This function implements enhanced SDIO3 write operation with
 *          advanced queue management, optimized transfer protocols, and
 *          improved error handling for maximum throughput performance.
 */
static int hd_sdio3_dnld_transmit(struct hd_sdio_card *card, struct sdio_trans_desc *dnld_desc, int retry_cnt)
{
    int ret, ret_1;
    int trans_len;
    int try_cnt = 0;
#if SDIO3_HOST_DNLD_WR_LEN_EN
    uint16_t dnld_elem = 0;
#endif

    while (card->curr_dnld_in == card->curr_dnld_out) {
        /* read dnld_in from custom reg */
        ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN,
                                       &(card->curr_dnld_in), 1);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "dnld read_custom_reg failed, %d\n", ret);
            ret = -EIO;
            goto return_out;
        }

        if (card->curr_dnld_in != card->curr_dnld_out) {
            break;
        }

        if (try_cnt < retry_cnt) {
            try_cnt += 1;
            udelay(10);
        } else {
            SDIO_DRV_DBG(card, "sdio_dnld timeout, in:%d, out:%d\n", card->curr_dnld_in, card->curr_dnld_out);
            ret = -EBUSY;
            goto return_out;
        }
    }

    /* get trans len */
    if (dnld_desc->data_len >= SDIO3_CMD53_BYTE_MOD_SIZE_MAX(card->func->num)) {
        /* block mode, align to block size */
        trans_len = (dnld_desc->data_len + card->func->cur_blksize - 1) / card->func->cur_blksize * card->func->cur_blksize;
    } else {
        /* byte mode, max size 512 */
        trans_len = dnld_desc->data_len;
    }

    SDIO_DRV_DBG(card, "dnld_len %d, trans_len %d, txbuff_len %d, dnld_out %d\n", dnld_desc->data_len, trans_len, dnld_desc->buff_len, card->curr_dnld_out);

#if SDIO3_HOST_DNLD_WR_LEN_EN
    /* wirte length to slave */
    if (dnld_desc->data_len % card->func->cur_blksize) {
        if (dnld_desc->data_len < 64) {
            dnld_elem = (dnld_desc->data_len << 2) | 0x00; /* 1-byte, byte mode */
        } else {
            dnld_elem = (dnld_desc->data_len << 2) | 0x01; /* 2-byte, byte mode */
        }
    } else {
        if (dnld_desc->data_len / card->func->cur_blksize < 64) {
            dnld_elem = ((dnld_desc->data_len / card->func->cur_blksize) << 2) | 0x02; /* 1-byte, block mode */
        } else {
            dnld_elem = ((dnld_desc->data_len / card->func->cur_blksize) << 2) | 0x03; /* 2-byte, block mode */
        }
    }
    /* write reg */
    ret = hd_sdio3_write_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE + (card->curr_dnld_out & card->queue_mask) * 2,
                                    &dnld_elem, (dnld_elem & 0x01) ? 2 : 1);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "dnld data write_custom_reg error\n");
        goto return_out;
    }
#endif

    /* check len */
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

    /* start write data */
    sdio_claim_host(card->func);
    // ret = sdio_writesb(card->func, 0, dnld_desc->buff, trans_len);
    ret = sdio_memcpy_toio(card->func, 0, dnld_desc->buff, trans_len);
    sdio_release_host(card->func);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sdio_memcpy_toio error %d\n", ret);

        /* update info */
        SDIO_DRV_INFO(card, "try to update queue info\n");
        ret_1 = hd_sdio3_info_update(card);
        if (ret_1 < 0) {
            SDIO_DRV_ERR(card, "failed to update_queue_info\n");
            goto return_out;
        }

        goto return_out;
    }

    card->curr_dnld_out++;

return_out:
    return ret;
}

/**
 * @brief SDIO3 enhanced card-to-host interrupt handler
 * @param[in] func SDIO function structure
 * @details This function handles enhanced SDIO3 card-to-host interrupts
 *          with improved interrupt processing, custom register handling,
 *          and optimized callback mechanisms for better performance.
 */
static void hd_sdio3_card2host_irq(struct sdio_func *func)
{
    int ret;
    struct hd_sdio_card *card = sdio_get_drvdata(func);
    if (!card) {
        return;
    }
    SDIO_DRV_DBG(card, "card2host_irq come");

    /* read to clear */
    /* Clear SDIO3 interrupt status */
    ret = hd_sdio3_read_reg(card, 1, NULL);
    if (ret < 0) {
        return;
    }

    /* Invoke enhanced upload interrupt callback */
    if (card->upld_irq_cb) {
        card->upld_irq_cb(card->sdio_manage);
    }
}

/**
 * @brief Register/unregister SDIO3 enhanced upload interrupt callback
 * @param[in] card Pointer to SDIO card structure
 * @param[in] upld_irq_cb Upload interrupt callback function (NULL to disable)
 * @retval 0 Success
 * @retval <0 Error code from SDIO operations
 * @details This function manages enhanced SDIO3 interrupt registration
 *          with advanced interrupt handling capabilities and improved
 *          performance characteristics over SDIO2.
 */
static int hd_sdio3_upld_irq_register(struct hd_sdio_card *card, card2host_irq_cb upld_irq_cb)
{
    int ret;

    if (upld_irq_cb) {
        card->upld_irq_cb = upld_irq_cb;
        /* enable card2host interrupt */
        sdio_claim_host(card->func);
        ret = sdio_claim_irq(card->func, hd_sdio3_card2host_irq);
        sdio_release_host(card->func);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "sdio_claim_irq failed: %d\n", ret);
            return ret;
        }
    } else {
        /* disable irq */
        sdio_claim_host(card->func);
        ret = sdio_release_irq(card->func);
        sdio_release_host(card->func);
        card->upld_irq_cb = NULL;
        if (ret < 0) {
            SDIO_DRV_ERR(card, "sdio_release_irq failed: %d\n", ret);
            return ret;
        }
    }

    return 0;
}

/**
 * @brief Enable SDIO3 function
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function enables the SDIO3 function for operation,
 *          making it ready for enhanced data transfer and register access.
 */
static int hd_sdio3_enable_func(struct hd_sdio_card *card)
{
    int ret;

    SDIO_DRV_INFO(card, "hd_sdio3_enable_func\n");

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
 * @brief Disable SDIO3 function
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from SDIO operation
 * @details This function disables the SDIO3 function, stopping
 *          enhanced data transfer capabilities and preparing for shutdown.
 */
static int hd_sdio3_disable_func(struct hd_sdio_card *card)
{
    int ret;
    SDIO_DRV_INFO(card, "hd_sdio3_disable_func\n");

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
 * @brief Set SDIO3 block size with enhanced configuration
 * @param[in] card Pointer to SDIO card structure
 * @param[in] size Block size to set
 * @retval 0 Success
 * @retval <0 Error code from SDIO operations
 * @details This function configures the SDIO3 block size for enhanced
 *          block mode transfers and synchronizes the setting with the
 *          device through custom register interface.
 */
static int hd_sdio3_set_block_size(struct hd_sdio_card *card, uint16_t size)
{
    int ret;
    SDIO_DRV_INFO(card, "hd_sdio3_set_block_size  %d -> %d\n", card->func->cur_blksize, size);

    /* set block size */
    sdio_claim_host(card->func);
    ret = sdio_set_block_size(card->func, size);
    sdio_release_host(card->func);

    if (ret < 0 || card->func->cur_blksize != size) {
        SDIO_DRV_ERR(card, "set block size error!, cur_blksize %d\n", card->func->cur_blksize);
        return ret;
    }

    /* write custom_reg */
    ret = hd_sdio3_write_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE,
                                    &size, 2);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "set block size error!, write_custom_reg error\n");
        return -EIO;
    }

    return 0;
}

/**
 * @brief Dump SDIO3 card information
 * @param[in] card Pointer to SDIO card structure
 * @details This function outputs comprehensive SDIO3 card configuration
 *          and status information including enhanced queue management
 *          details for debugging and monitoring purposes.
 */
static void hd_sdio3_info_dump(struct hd_sdio_card *card)
{
    /* dump queue info */
    SDIO_DRV_INFO(card, "sdio3_info dump:\n");
    SDIO_DRV_INFO(card, "sdio_boot mode: %s\n", (card->sdio_boot) ? "true" : "false");
    SDIO_DRV_INFO(card, "app_run mode: %s\n", (card->app_run) ? "true" : "false");
    SDIO_DRV_INFO(card, "rd_len_compress mode: %s\n", (card->rd_len_compress) ? "true" : "false");
    SDIO_DRV_INFO(card, "dnld_size_max: %d\n", card->dnld_max_size);
    SDIO_DRV_INFO(card, "upld_size_max: %d\n", card->upld_max_size);
    SDIO_DRV_INFO(card, "queue_depth: %d\n", card->queue_depth);
    SDIO_DRV_INFO(card, "queue_mask: 0x%04X\n", card->queue_mask);
    SDIO_DRV_INFO(card, "curr_dnld_in: %d\n", card->curr_dnld_in);
    SDIO_DRV_INFO(card, "curr_dnld_out: %d\n", card->curr_dnld_out);
    SDIO_DRV_INFO(card, "curr_upld_in: %d\n", card->curr_upld_in);
    SDIO_DRV_INFO(card, "curr_upld_out: %d\n", card->curr_upld_out);
}

/**
 * @brief Update SDIO3 card information with enhanced configuration
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from custom register operations
 * @details This function reads and updates enhanced SDIO3 card configuration
 *          including transfer size limits, queue states, status flags, and
 *          synchronizes bidirectional transfer parameters through custom
 *          register interface for optimal enhanced mode operation.
 */
static int hd_sdio3_info_update(struct hd_sdio_card *card)
{
    int ret;
    uint8_t status_flags;
    SDIO_DRV_INFO(card, "hd_sdio3_info_update\n");

    /* 1. read dnld max size (slave buff size) */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE,
                                   &card->dnld_max_size, 2);
    if (ret < 0) {
        goto exit;
    }

    /* 2. write upld max size (host buff size) */
    if (card->func->num == 1) {
        card->upld_max_size = HD_SDIO3_HOST_FN1_UPLD_SIZE_MAX;
    } else if (card->func->num == 2) {
        card->upld_max_size = HD_SDIO3_HOST_FN2_UPLD_SIZE_MAX;
    }
    ret = hd_sdio3_write_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_MAX_SIZE,
                                    &card->upld_max_size, 2);
    if (ret < 0) {
        goto exit;
    }

    /* 3.read dnld queue in */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN,
                                   &card->curr_dnld_in, 1);
    if (ret < 0) {
        goto exit;
    }
    /* 4.read dnld queue out */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT,
                                   &card->curr_dnld_out, 1);
    if (ret < 0) {
        goto exit;
    }
    /* 5.read upld queue in */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN,
                                   &card->curr_upld_in, 1);
    if (ret < 0) {
        goto exit;
    }
    /* 6.read upld queue out */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT,
                                   &card->curr_upld_out, 1);
    if (ret < 0) {
        goto exit;
    }

    /* 7. read queue depth */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH,
                                   &card->queue_depth, 1);
    if (ret < 0) {
        goto exit;
    }
    if (card->queue_depth) {
        card->queue_mask = card->queue_depth - 1;
    } else {
        ret = -EIO;
        goto exit;
    }

    /* 8. read sdio status flags */
    ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_STA_FLAG,
                                   &status_flags, 1);
    if (ret < 0) {
        goto exit;
    }
    card->sdio_boot = (status_flags & SDIO3_STA_FLAG_SDIO_BOOT) ? true : false;
    card->app_run = (status_flags & SDIO3_STA_FLAG_APP_RUN) ? true : false;
    card->rd_len_compress = (status_flags & SDIO3_STA_FLAG_RD_LEN_COMPRESS_SUP) ? true : false;

exit:
    if (ret < 0) {
        SDIO_DRV_ERR(card, "update_queue_info error\n");
    }

    hd_sdio3_info_dump(card);
    return ret;
}

/**
 * @brief Wait for SDIO3 card to become ready with enhanced polling
 * @param[in] card Pointer to SDIO card structure
 * @param[in] timeout_ms Timeout in milliseconds
 * @retval 0 Success - card is ready
 * @retval -ETIME Timeout occurred
 * @retval <0 Other error code from custom register operations
 * @details This function polls the enhanced card ready status through
 *          custom registers until the card signals readiness or timeout
 *          occurs. Uses efficient sleep intervals for enhanced mode.
 */
static int hd_sdio3_wait_card_ready(struct hd_sdio_card *card, int timeout_ms)
{
    int ret;
    uint8_t card_ready = 0;
    ktime_t time_s;

    time_s = ktime_get_boottime();

    while (1) {
        ret = hd_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_CARD_READY,
                                       &card_ready, 1);
        if (ret < 0) {
            goto return_out;
        }

        if (card_ready) {
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
 * @brief Signal SDIO3 function host readiness
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from custom register write operation
 * @details This function signals host readiness to the SDIO3 card through
 *          enhanced custom register interface to establish enhanced mode
 *          communication handshake.
 */
static int hd_sdio3_func_host_ready(struct hd_sdio_card *card)
{
    int ret;
    uint8_t host_ready = 1;

    ret = hd_sdio3_write_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_HOST_READY,
                                    &host_ready, 1);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "set host_ready error\n");
        return ret;
    }

    return 0;
}

/**
 * @brief Clean SDIO3 function host and card readiness status
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from custom register write operations
 * @details This function clears both host and card ready status through
 *          enhanced custom register interface to reset the enhanced mode
 *          communication handshake state during shutdown or reinitialization.
 */
static int hd_sdio3_func_clean_ready(struct hd_sdio_card *card)
{
    int ret;
    uint8_t ready = 0;

    ret = hd_sdio3_write_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_HOST_READY,
                                    &ready, 1);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "clean host_ready error\n");
        return ret;
    }

    ret = hd_sdio3_write_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_CARD_READY,
                                    &ready, 1);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "clean card_ready error\n");
        return ret;
    }

    return 0;
}

/**
 * @brief Initialize SDIO3 function with enhanced capabilities
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from initialization operations
 * @details This function performs complete SDIO3 enhanced function initialization
 *          including quirks setup, function enabling, block size configuration,
 *          enhanced handshake synchronization, and enhanced operation callback
 *          registration. Establishes full enhanced driver interface.
 */
static int hd_sdio3_func_init(struct hd_sdio_card *card)
{
    int ret;

    /* Set the MMC_QUIRK_LENIENT_FN0 bit to allow access to non-vendor registers. */
    card->func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
    /* Use block mode for transferring over one block size of data */
    card->func->card->quirks |= MMC_QUIRK_BLKSZ_FOR_BYTE_MODE;

    /* sdio3.0 */
    card->sdio_hw_ver = 3;

    /* 1. enable or reset func */
    ret = hd_sdio3_enable_func(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to enable func\n");
        goto exit;
    }

    /* 2. set block size, and write to custom_reg */
    if (card->func->num == 1) {
        ret = hd_sdio3_set_block_size(card, SDIO3_HOST_FN1_BLOCK_SIZE);
    } else if (card->func->num == 2) {
        ret = hd_sdio3_set_block_size(card, SDIO3_HOST_FN2_BLOCK_SIZE);
    }
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to set block size\n");
        goto exit;
    }

    /* 3. waiting card ready */
    ret = hd_sdio3_wait_card_ready(card, 200);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to wait_card_ready\n");
        goto exit;
    }

    /* 4. update queue info */
    ret = hd_sdio3_info_update(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to update_queue_info\n");
        goto exit;
    }

    /* 5. host ready */
    ret = hd_sdio3_func_host_ready(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to set host_ready\n");
        goto exit;
    }

    /* 6. sdio3 ops register */
    /* */
    card->sdio_read_reg = hd_sdio3_read_reg;
    card->sdio_write_reg = hd_sdio3_write_reg;
    /* */
    card->sdio_read_fn0_reg = hd_sdio3_read_fn0_reg;
    card->sdio_write_fn0_reg = hd_sdio3_write_fn0_reg;
    /* */
    card->sdio_read_custom_reg = hd_sdio3_read_custom_reg;
    card->sdio_write_custom_reg = hd_sdio3_write_custom_reg;
    /* */
    card->sdio_upld_transf = hd_sdio3_upld_transmit;
    card->sdio_dnld_transf = hd_sdio3_dnld_transmit;
    /* */
    card->sdio_upld_irq_register = hd_sdio3_upld_irq_register;

exit:
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to init function\n");
    }
    return ret;
}

/**
 * @brief Reinitialize SDIO3 function after bootrom phase
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from reinitialization operations
 * @details This function reinitializes the enhanced SDIO3 function after
 *          firmware has been loaded and started. It waits for enhanced card
 *          readiness, updates enhanced configuration, and reestablishes the
 *          enhanced communication handshake for application mode operation.
 */
static int hd_sdio3_func_boot_reinit(struct hd_sdio_card *card)
{
    int ret;

    /* 1. waiting card ready */
    ret = hd_sdio3_wait_card_ready(card, 200);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to wait_card_ready\n");
        goto exit;
    }

    /* 2. update queue info */
    ret = hd_sdio3_info_update(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to update_queue_info\n");
        goto exit;
    }

    /* 3. host ready */
    ret = hd_sdio3_func_host_ready(card);
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
 * @brief SDIO3 device removal function
 * @param[in] func SDIO function structure
 * @details This function handles cleanup when SDIO3 device is removed.
 *          It destroys associated components, releases resources, and
 *          performs proper driver cleanup sequence for enhanced SDIO3 features.
 */
static void hd_sdio3_remove(struct sdio_func *func)
{
    struct hd_sdio_card *card;

    pr_notice("[sdio fn%d]: hd_sdio_remove\n", func->num);

    card = sdio_get_drvdata(func);
    if (!card) {
        pr_notice("[sdio fn%d]: card is NULL\n", func->num);
        return;
    }

    /* dump queue info */
    hd_sdio3_info_dump(card);

    /* stop speed test thread */
    hd_speed_test_thread_destroy_all();

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
    hd_sdio3_disable_func(card);

    sdio_set_drvdata(func, NULL);
    kfree(card);

    /* unregister */
    // hd_debugfs_unregister(card);
}

/**
 * @brief SDIO3 device probe function
 * @param[in] func SDIO function structure
 * @param[in] id SDIO device ID
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function handles SDIO3 device initialization including
 *          enhanced function enabling, optimized buffer allocation,
 *          advanced interrupt configuration, and registration of all
 *          enhanced SDIO3 driver components.
 */
static int hd_sdio3_probe(struct sdio_func *func, const struct sdio_device_id *id)
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

    /* reset func, init sdio dev queue, and set host ready */
    ret = hd_sdio3_func_init(card);
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
        ret = hd_sdio3_func_clean_ready(card);
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
        ret = hd_sdio3_func_boot_reinit(card);
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
        goto async_queue_exit;
    }

    /* do something */

#if 0
    /* speed test */
    ret = hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_1, false);
    if (ret < 0) {
        goto async_queue_exit;
    }
#endif

    /* Multithreaded speed test */
    hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_1, true);
    hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_2, true);
    hd_speed_test_run(msg_ctrl, HD_MSG_TAG_TEST_3, true);

    /* create netdev */
    card->netdev = hd_sdio_eth_netdev_init(msg_ctrl, HD_MSG_TAG_ETH);
    if (card->netdev == NULL) {
        SDIO_DRV_ERR(card, "failed to create netdev\n");
        /* Continue without netdev - not critical */
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

async_queue_exit:
    if (ret < 0) {
        hd_sdio_manage_deinit(sdio_manage);
    }
exit:
    if (ret < 0) {
        pr_err("[sdio fn%d]: probe failed\n", func->num);
        /*  */
        sdio_set_drvdata(func, NULL);
        /* remove card2host int */
        hd_sdio3_upld_irq_register(card, NULL);
        /* disable func */
        hd_sdio3_disable_func(card);
        kfree(card);
    }
    pr_notice("[sdio fn%d]: hd_sdio_probe exit: %d\n", func->num, ret);
    return ret;
}

static const struct sdio_device_id hd_sdio3_ids[] = {
    { SDIO_DEVICE(0x424c, 0x6281) }, /* bl628 fn1 */
    { SDIO_DEVICE(0x424c, 0x6282) }, /* bl628 fn2 */
    { SDIO_DEVICE(0x0000, 0x0000) }, /* bl616D */
    { /* end: all zeroes */ },
};
MODULE_DEVICE_TABLE(sdio, hd_sdio3_ids);

struct sdio_driver hd_sdio3 = {
    .name = "hd_sdio3",
    .id_table = hd_sdio3_ids,
    .probe = hd_sdio3_probe,
    .remove = hd_sdio3_remove,
};
