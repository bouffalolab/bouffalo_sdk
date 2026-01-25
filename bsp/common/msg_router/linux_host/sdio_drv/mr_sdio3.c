
/**
 * @file mr_sdio3.c
 * @brief SDIO3 Interface Driver Implementation
 * @details This module implements the SDIO3 interface driver.
 *          It provides advanced SDIO communication features, 
 *          improved performance characteristics, and extended functionality
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

#include "mr_sdio_manage.h"
#include "mr_sdio3.h"
#include "mr_msg_ctrl.h"
#include "mr_debugfs.h"

#ifdef CONFIG_MR_NETDEV
#include "mr_netdev.h"
#endif
#ifdef CONFIG_MR_NETLINK
#include "mr_netlink.h"
#endif
#ifdef CONFIG_MR_SPEED_TEST
#include "mr_speed_test.h"
#endif
#ifdef CONFIG_MR_TTY
#include "mr_tty.h"
#endif
#ifdef CONFIG_MR_BOOTROM
#include "mr_bootrom.h"
#endif

static int mr_sdio3_info_update(struct mr_sdio_card *card);

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
static int mr_sdio3_read_reg(struct mr_sdio_card *card, uint32_t reg, uint8_t *data)
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
static int mr_sdio3_write_reg(struct mr_sdio_card *card, uint32_t reg, uint8_t data)
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
static int mr_sdio3_read_fn0_reg(struct mr_sdio_card *card, uint32_t reg, uint8_t *data)
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
static int mr_sdio3_write_fn0_reg(struct mr_sdio_card *card, uint32_t reg, uint8_t data)
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
static int mr_sdio3_read_custom_reg(struct mr_sdio_card *card, uint16_t reg_offset, void *buff, uint16_t len)
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
static int mr_sdio3_write_custom_reg(struct mr_sdio_card *card, uint16_t reg_offset, void *buff, uint16_t len)
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
static int mr_sdio3_upld_transmit(struct mr_sdio_card *card, struct sdio_trans_desc *upld_desc, int retry_cnt)
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
        ret = mr_sdio3_read_custom_reg(
            card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN,
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
    ret = mr_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE + (card->curr_upld_out & card->queue_mask) * 2,
                                   &reg_temp[0], 1);
    if (ret < 0) {
        goto return_out;
    }
    /* 2-byte mode, read second byte */
    if (reg_temp[0] & 0x01) {
        ret = mr_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE + (card->curr_upld_out & card->queue_mask) * 2 + 1,
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
    SDIO_DRV_DBG(card, "upld_len %d, trans_len %d, rxbuff_len%d, upld_out %d\n", dev_upld_len, trans_len,
                 upld_desc->buff_len, card->curr_upld_out);

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
        ret_1 = mr_sdio3_info_update(card);
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
static int mr_sdio3_dnld_transmit(struct mr_sdio_card *card, struct sdio_trans_desc *dnld_desc, int retry_cnt)
{
    int ret, ret_1;
    int trans_len;
    int try_cnt = 0;
#if SDIO3_HOST_DNLD_WR_LEN_EN
    uint16_t dnld_elem = 0;
#endif

    while (card->curr_dnld_in == card->curr_dnld_out) {
        /* read dnld_in from custom reg */
        ret = mr_sdio3_read_custom_reg(
            card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN,
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
        trans_len =
            (dnld_desc->data_len + card->func->cur_blksize - 1) / card->func->cur_blksize * card->func->cur_blksize;
    } else {
        /* byte mode, max size 512 */
        trans_len = dnld_desc->data_len;
    }

    SDIO_DRV_DBG(card, "dnld_len %d, trans_len %d, txbuff_len %d, dnld_out %d\n", dnld_desc->data_len, trans_len,
                 dnld_desc->buff_len, card->curr_dnld_out);

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
    ret = mr_sdio3_write_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE + (card->curr_dnld_out & card->queue_mask) * 2,
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
        ret_1 = mr_sdio3_info_update(card);
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
static void mr_sdio3_card2host_irq(struct sdio_func *func)
{
    int ret;
    struct mr_sdio_card *card = sdio_get_drvdata(func);
    if (!card) {
        return;
    }
    SDIO_DRV_DBG(card, "card2host_irq come");

    /* read to clear */
    /* Clear SDIO3 interrupt status */
    ret = mr_sdio3_read_reg(card, 1, NULL);
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
static int mr_sdio3_upld_irq_register(struct mr_sdio_card *card, card2host_irq_cb upld_irq_cb)
{
    int ret;

    if (upld_irq_cb) {
        card->upld_irq_cb = upld_irq_cb;
        /* enable card2host interrupt */
        sdio_claim_host(card->func);
        ret = sdio_claim_irq(card->func, mr_sdio3_card2host_irq);
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
static int mr_sdio3_enable_func(struct mr_sdio_card *card)
{
    int ret;

    SDIO_DRV_INFO(card, "mr_sdio3_enable_func\n");

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
static int mr_sdio3_disable_func(struct mr_sdio_card *card)
{
    int ret;
    SDIO_DRV_INFO(card, "mr_sdio3_disable_func\n");

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
static int mr_sdio3_set_block_size(struct mr_sdio_card *card, uint16_t size)
{
    int ret;
    SDIO_DRV_INFO(card, "mr_sdio3_set_block_size  %d -> %d\n", card->func->cur_blksize, size);

    /* set block size */
    sdio_claim_host(card->func);
    ret = sdio_set_block_size(card->func, size);
    sdio_release_host(card->func);

    if (ret < 0 || card->func->cur_blksize != size) {
        SDIO_DRV_ERR(card, "set block size error!, cur_blksize %d\n", card->func->cur_blksize);
        return ret;
    }

    /* write custom_reg */
    ret = mr_sdio3_write_custom_reg(
        card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE, &size, 2);
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
static void mr_sdio3_info_dump(struct mr_sdio_card *card)
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
static int mr_sdio3_info_update(struct mr_sdio_card *card)
{
    int ret;
    uint8_t status_flags;
    SDIO_DRV_INFO(card, "mr_sdio3_info_update\n");

    /* 1. read dnld max size (slave buff size) */
    ret = mr_sdio3_read_custom_reg(card,
                                   SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE,
                                   &card->dnld_max_size, 2);
    if (ret < 0) {
        goto exit;
    }

    /* 2. write upld max size (host buff size) */
    if (card->func->num == 1) {
        card->upld_max_size = MR_SDIO3_HOST_FN1_UPLD_SIZE_MAX;
    } else if (card->func->num == 2) {
        card->upld_max_size = MR_SDIO3_HOST_FN2_UPLD_SIZE_MAX;
    }
    ret = mr_sdio3_write_custom_reg(card,
                                    SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_MAX_SIZE,
                                    &card->upld_max_size, 2);
    if (ret < 0) {
        goto exit;
    }

    /* 3.read dnld queue in */
    ret = mr_sdio3_read_custom_reg(card,
                                   SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN,
                                   &card->curr_dnld_in, 1);
    if (ret < 0) {
        goto exit;
    }
    /* 4.read dnld queue out */
    ret = mr_sdio3_read_custom_reg(card,
                                   SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT,
                                   &card->curr_dnld_out, 1);
    if (ret < 0) {
        goto exit;
    }
    /* 5.read upld queue in */
    ret = mr_sdio3_read_custom_reg(card,
                                   SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN,
                                   &card->curr_upld_in, 1);
    if (ret < 0) {
        goto exit;
    }
    /* 6.read upld queue out */
    ret = mr_sdio3_read_custom_reg(card,
                                   SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT,
                                   &card->curr_upld_out, 1);
    if (ret < 0) {
        goto exit;
    }

    /* 7. read queue depth */
    ret =
        mr_sdio3_read_custom_reg(card,
                                 SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH,
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
    ret = mr_sdio3_read_custom_reg(card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_STA_FLAG,
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

    mr_sdio3_info_dump(card);
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
static int mr_sdio3_wait_card_ready(struct mr_sdio_card *card, int timeout_ms)
{
    int ret;
    uint8_t card_ready = 0;
    ktime_t time_s;

    time_s = ktime_get_boottime();

    while (1) {
        ret = mr_sdio3_read_custom_reg(
            card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_CARD_READY, &card_ready, 1);
        if (ret < 0) {
            goto return_out;
        }

        if (card_ready) {
            break;
        } else if (ktime_to_ns(ktime_sub(ktime_get_boottime(), time_s)) < timeout_ms * NSEC_PER_MSEC) {
            usleep_range(100, 200);
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
static int mr_sdio3_func_host_ready(struct mr_sdio_card *card)
{
    int ret;
    uint8_t host_ready = 1;

    ret = mr_sdio3_write_custom_reg(
        card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_HOST_READY, &host_ready, 1);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "set host_ready error\n");
        return ret;
    }

    return 0;
}

#ifdef CONFIG_MR_BOOTROM
/**
 * @brief Clean SDIO3 function host and card readiness status
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code from custom register write operations
 * @details This function clears both host and card ready status through
 *          enhanced custom register interface to reset the enhanced mode
 *          communication handshake state during shutdown or reinitialization.
 */
static int mr_sdio3_func_clean_ready(struct mr_sdio_card *card)
{
    int ret;
    uint8_t ready = 0;

    ret = mr_sdio3_write_custom_reg(
        card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_HOST_READY, &ready, 1);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "clean host_ready error\n");
        return ret;
    }

    ret = mr_sdio3_write_custom_reg(
        card, SDIO3_CUSTOM_REG_FUNC_OFFSET(card->func->num) + SDIO3_CUSTOM_REG_FUNC_CARD_READY, &ready, 1);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "clean card_ready error\n");
        return ret;
    }

    return 0;
}
#endif

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
static int mr_sdio3_func_init(struct mr_sdio_card *card)
{
    int ret;

    /* Set the MMC_QUIRK_LENIENT_FN0 bit to allow access to non-vendor registers. */
    card->func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
    /* Use block mode for transferring over one block size of data */
    card->func->card->quirks |= MMC_QUIRK_BLKSZ_FOR_BYTE_MODE;

    /* sdio3.0 */
    card->sdio_hw_ver = 3;

    /* 1. enable or reset func */
    ret = mr_sdio3_enable_func(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to enable func\n");
        goto exit;
    }

    /* 2. set block size, and write to custom_reg */
    if (card->func->num == 1) {
        ret = mr_sdio3_set_block_size(card, SDIO3_HOST_FN1_BLOCK_SIZE);
    } else if (card->func->num == 2) {
        ret = mr_sdio3_set_block_size(card, SDIO3_HOST_FN2_BLOCK_SIZE);
    }
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to set block size\n");
        goto exit;
    }

    /* 3. waiting card ready */
    ret = mr_sdio3_wait_card_ready(card, 200);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to wait_card_ready\n");
        goto exit;
    }

    /* 4. update queue info */
    ret = mr_sdio3_info_update(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to update_queue_info\n");
        goto exit;
    }

    /* 5. host ready */
    ret = mr_sdio3_func_host_ready(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to set host_ready\n");
        goto exit;
    }

    /* 6. sdio3 ops register */
    /* */
    card->sdio_read_reg = mr_sdio3_read_reg;
    card->sdio_write_reg = mr_sdio3_write_reg;
    /* */
    card->sdio_read_fn0_reg = mr_sdio3_read_fn0_reg;
    card->sdio_write_fn0_reg = mr_sdio3_write_fn0_reg;
    /* */
    card->sdio_read_custom_reg = mr_sdio3_read_custom_reg;
    card->sdio_write_custom_reg = mr_sdio3_write_custom_reg;
    /* */
    card->sdio_upld_transf = mr_sdio3_upld_transmit;
    card->sdio_dnld_transf = mr_sdio3_dnld_transmit;
    /* */
    card->sdio_upld_irq_register = mr_sdio3_upld_irq_register;

exit:
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to init function\n");
    }
    return ret;
}

#ifdef CONFIG_MR_BOOTROM
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
static int mr_sdio3_func_boot_reinit(struct mr_sdio_card *card)
{
    int ret;

    /* 1. waiting card ready */
    ret = mr_sdio3_wait_card_ready(card, 200);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to wait_card_ready\n");
        goto exit;
    }

    /* 2. update queue info */
    ret = mr_sdio3_info_update(card);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "failed to update_queue_info\n");
        goto exit;
    }

    /* 3. host ready */
    ret = mr_sdio3_func_host_ready(card);
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
#endif

/**
 * @brief SDIO3 device removal function
 * @param[in] func SDIO function structure
 * @details This function handles cleanup when SDIO3 device is removed.
 *          It destroys associated components, releases resources, and
 *          performs proper driver cleanup sequence for enhanced SDIO3 features.
 */
static void mr_sdio3_remove(struct sdio_func *func)
{
    struct mr_sdio_card *card;

    pr_notice("[sdio fn%d]: mr_sdio_remove\n", func->num);

    card = sdio_get_drvdata(func);
    if (!card) {
        pr_notice("[sdio fn%d]: card is NULL\n", func->num);
        return;
    }

#ifdef CONFIG_MR_NETDEV
    if (card->netdev) {
        /* stop netdev */
        mr_sdio_eth_netdev_deinit(card->netdev);
        card->netdev = NULL;
    }
#endif

#ifdef CONFIG_MR_NETLINK
    if (card->netlink) {
        /* stop netlink */
        mr_netlink_deinit(card->netlink);
        card->netlink = NULL;
    }
#endif

#ifdef CONFIG_MR_TTY
#ifdef CONFIG_MR_TTY_CMD
    if (card->tty_msg_cmd) {
        /* stop tty devices */
        mr_tty_deinit(card->tty_msg_cmd);
        card->tty_msg_cmd = NULL;
    }
#endif
#ifdef CONFIG_MR_TTY_USER_1
    if (card->tty_msg_user_1) {
        /* stop tty devices */
        mr_tty_deinit(card->tty_msg_user_1);
        card->tty_msg_user_1 = NULL;
    }
#endif
#ifdef CONFIG_MR_TTY_USER_2
    if (card->tty_msg_user_2) {
        /* stop tty devices */
        mr_tty_deinit(card->tty_msg_user_2);
        card->tty_msg_user_2 = NULL;
    }
#endif
#ifdef CONFIG_MR_TTY_USER_3
    if (card->tty_msg_user_3) {
        /* stop tty devices */
        mr_tty_deinit(card->tty_msg_user_3);
        card->tty_msg_user_3 = NULL;
    }
#endif
#endif

#ifdef CONFIG_MR_SPEED_TEST
    /* stop speed test thread */
    mr_speed_test_thread_destroy_all();
#endif

    if (card->sdio_manage) {
        /* deinit msg ctrl */
        if (card->sdio_manage->msg_ctrl) {
            mr_sdio_msg_ctrl_deinit(card->sdio_manage->msg_ctrl);
            card->sdio_manage->msg_ctrl = NULL;
        }

        /* deinit sdio manage */
        mr_sdio_manage_deinit(card->sdio_manage);
        card->sdio_manage = NULL;
    }

    /* disable function */
    mr_sdio3_disable_func(card);

    /* dump queue info */
    mr_sdio3_info_dump(card);

    sdio_set_drvdata(func, NULL);
    kfree(card);

    /* unregister */
    // mr_debugfs_unregister(card);
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
static int mr_sdio3_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
    int ret = 0;
    struct mr_sdio_card *card = NULL;
    struct mr_sdio_manage *sdio_manage = NULL;
    struct mr_msg_ctrl *msg_ctrl = NULL;

    pr_notice("[sdio fn%d]: mr_sdio_probe\n", func->num);
    pr_notice("[sdio fn%d]: VID:0x%04X PID:0x%04X Class:%d Func:%d\n", func->num, func->vendor, func->device,
              func->class, func->num);
    pr_notice("[sdio fn%d]: max_blksize %d, timeout %d\n", func->num, func->max_blksize, func->enable_timeout);

    /* malloc mr_sdio_card struct */
    card = kzalloc(sizeof(struct mr_sdio_card), GFP_KERNEL);
    if (!card) {
        ret = -ENOMEM;
        goto exit;
    }

    card->func = func;
    sdio_set_drvdata(func, card);

    /* reset func, init sdio dev queue, and set host ready */
    ret = mr_sdio3_func_init(card);
    if (ret < 0) {
        goto exit;
    }
    msleep(2);

    /* TODO: download fw */
    if (card->sdio_boot) {
#ifdef CONFIG_MR_BOOTROM
        /* 1. download firmware */
        ret = mr_bootrom_download_firmware(card, NULL);
        if (ret < 0) {
            goto exit;
        }
        /* 2. clean ready status */
        ret = mr_sdio3_func_clean_ready(card);
        if (ret < 0) {
            goto exit;
        }
        /* 3. run firmware */
        ret = mr_bootrom_firmware_run(card);
        if (ret < 0) {
            goto exit;
        }
        msleep(5);
        /* 4. reinit sdio */
        ret = mr_sdio3_func_boot_reinit(card);
        if (ret < 0) {
            goto exit;
        }
#else
        pr_err("[sdio fn%d]: no bootrom support, cannot download fw\n", func->num);
        ret = -EIO;
        goto exit;
#endif
    }

    /* async sdio manage init */
    ret = mr_sdio_manage_init(&sdio_manage, card);
    if (ret < 0) {
        goto exit;
    }

    /* msg_ctrl init */
    ret = mr_sdio_msg_ctrl_init(&msg_ctrl, sdio_manage);
    if (ret < 0) {
        goto sdio_manage_exit;
    }

    /* do something */

#ifdef CONFIG_MR_SPEED_TEST
    /* speed test */
    // ret = mr_speed_test_run(msg_ctrl, MR_MSG_TAG_TEST_1, false);
    // if (ret < 0) {
    //     goto msg_ctrl_exit;
    // }

    /* Multithreaded speed test */
    mr_speed_test_run(msg_ctrl, MR_MSG_TAG_TEST_1, true);
    mr_speed_test_run(msg_ctrl, MR_MSG_TAG_TEST_2, true);
    mr_speed_test_run(msg_ctrl, MR_MSG_TAG_TEST_3, true);
#endif

#ifdef CONFIG_MR_NETDEV
    /* create netdev */
    card->netdev = mr_sdio_eth_netdev_init(msg_ctrl, MR_MSG_TAG_NETDEV_ETH);
    if (card->netdev == NULL) {
        SDIO_DRV_ERR(card, "failed to create netdev\n");
        ret = -ENOMEM;
        goto msg_ctrl_exit;
    }
#endif

#ifdef CONFIG_MR_NETLINK
    /* create netlink */
    card->netlink = mr_netlink_init(msg_ctrl, MR_MSG_TAG_NETLINK);
    if (card->netlink == NULL) {
        SDIO_DRV_ERR(card, "failed to create netlink\n");
        ret = -ENOMEM;
        goto msg_ctrl_exit;
    }
#endif

#ifdef CONFIG_MR_TTY

#ifdef CONFIG_MR_TTY_CMD
    /* create tty devices */
    card->tty_msg_cmd = mr_tty_init(msg_ctrl, MR_MSG_TAG_TTY_CMD, "ttyMR_cmd", 0);
    if (card->tty_msg_cmd == NULL) {
        SDIO_DRV_ERR(card, "failed to create tty devices\n");
        goto msg_ctrl_exit;
    }
#endif

#ifdef CONFIG_MR_TTY_USER_1
    /* create tty devices */
    card->tty_msg_user_1 = mr_tty_init(msg_ctrl, MR_MSG_TAG_TTY_USER_1, "ttyMR_user", 1);
    if (card->tty_msg_user_1 == NULL) {
        SDIO_DRV_ERR(card, "failed to create tty devices\n");
        goto msg_ctrl_exit;
    }
#endif

#ifdef CONFIG_MR_TTY_USER_2
    /* create tty devices */
    card->tty_msg_user_2 = mr_tty_init(msg_ctrl, MR_MSG_TAG_TTY_USER_2, "ttyMR_user", 2);
    if (card->tty_msg_user_2 == NULL) {
        SDIO_DRV_ERR(card, "failed to create tty devices\n");
        goto msg_ctrl_exit;
    }
#endif

#ifdef CONFIG_MR_TTY_USER_1
    /* create tty devices */
    card->tty_msg_user_3 = mr_tty_init(msg_ctrl, MR_MSG_TAG_TTY_USER_3, "ttyMR_user", 3);
    if (card->tty_msg_user_3 == NULL) {
        SDIO_DRV_ERR(card, "failed to create tty devices\n");
        goto msg_ctrl_exit;
    }
#endif

#endif

#if 0
    ret = mr_debugfs_register(card);
    if (ret) {
        goto exit;
    }
#endif

msg_ctrl_exit:
    if (ret < 0) {
        mr_sdio_msg_ctrl_deinit(msg_ctrl);
    }

sdio_manage_exit:
    if (ret < 0) {
        mr_sdio_manage_deinit(sdio_manage);
    }
exit:
    if (ret < 0) {
        pr_err("[sdio fn%d]: probe failed\n", func->num);
        /*  */
        sdio_set_drvdata(func, NULL);
        /* remove card2host int */
        mr_sdio3_upld_irq_register(card, NULL);
        /* disable func */
        mr_sdio3_disable_func(card);
        kfree(card);
    }
    pr_notice("[sdio fn%d]: mr_sdio_probe exit: %d\n", func->num, ret);
    return ret;
}

static const struct sdio_device_id mr_sdio3_ids[] = {
    { SDIO_DEVICE(0x424c, 0x6281) },
    { SDIO_DEVICE(0x424c, 0x6282) },
    { SDIO_DEVICE(0x0000, 0x0000) },
    { /* end: all zeroes */ },
};
MODULE_DEVICE_TABLE(sdio3, mr_sdio3_ids);

struct sdio_driver mr_sdio3 = {
    .name = "mr_sdio3",
    .id_table = mr_sdio3_ids,
    .probe = mr_sdio3_probe,
    .remove = mr_sdio3_remove,
};
