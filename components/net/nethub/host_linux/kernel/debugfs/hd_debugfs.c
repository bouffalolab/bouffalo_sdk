/**
 * @file hd_debugfs.c
 * @brief Host Driver DebugFS Interface Implementation
 * @author Host Driver Development Team
 * @date 2024
 * @details This module provides DebugFS interfaces for debugging and testing
 *          SDIO communication functionality. It includes read/write operations
 *          for direct SDIO data access through the filesystem interface.
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include "hd_debugfs.h"

#if 0

/**
 * @brief DebugFS SDIO read operation
 * @param[in] file File structure pointer
 * @param[out] user_buf User space buffer for read data
 * @param[in] count Number of bytes to read
 * @param[in,out] ppos File position pointer
 * @retval >0 Number of bytes read
 * @retval <0 Error code
 * @details This function provides a DebugFS interface to read data
 *          directly from SDIO device for debugging purposes.
 */
static ssize_t hd_dbgfs_sdiord_read(struct file *file,
                                    char __user *user_buf,
                                    size_t count, loff_t *ppos)
{
    struct hd_sdio_card *card = file->private_data;
    ssize_t read;
    char recvbuf[256] = { 0 };
    int port1 = 0x10001;

    //TODO: read data from sdio
    hd_read_data_sync(card, recvbuf, 256, port1);

    read = simple_read_from_buffer(user_buf, count, ppos, recvbuf, sizeof(recvbuf));

    return read;
}

DEBUGFS_READ_FILE_OPS(sdiord);

/**
 * @brief DebugFS SDIO write operation
 * @param[in] file File structure pointer
 * @param[in] user_buf User space buffer containing data to write
 * @param[in] count Number of bytes to write
 * @param[in,out] ppos File position pointer
 * @retval >0 Number of bytes written
 * @retval <0 Error code
 * @details This function provides a DebugFS interface to write data
 *          directly to SDIO device for debugging purposes.
 */
static ssize_t hd_dbgfs_sdiowr_write(struct file *file,
                                     const char __user *user_buf,
                                     size_t count, loff_t *ppos)
{
    struct hd_sdio_card *card = file->private_data;
    char buf[32];
    char sendbuf[32] = { 0x5a, 0xa5, 0x5a, 0xa5 };
    int port;
    size_t len = min_t(size_t, count, sizeof(buf) - 1);

    if (copy_from_user(buf, user_buf, len))
        return -EFAULT;

    buf[len] = '\0';

    sscanf(buf, "%x", &port);

    hd_write_data_sync(card, sendbuf, 256, port);

    return count;
}
DEBUGFS_WRITE_FILE_OPS(sdiowr);

/**
 * @brief Unregister DebugFS interface
 * @param[in] card Pointer to SDIO card structure
 * @details This function removes the DebugFS directory and all associated
 *          debug files for the specified SDIO card.
 */
void hd_debugfs_unregister(struct hd_sdio_card *card)
{
    if (!card->dir)
        return;

    debugfs_remove_recursive(card->dir);

    card->dir = NULL;
}

/**
 * @brief Register DebugFS interface
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval -ENOMEM Memory allocation failed
 * @details This function creates DebugFS directory and files for SDIO
 *          debugging operations including read and write interfaces.
 */
int hd_debugfs_register(struct hd_sdio_card *card)
{
    struct dentry *dir_diags;

    if (!(dir_diags = debugfs_create_dir("sdio", NULL))) {
        printk("Create debugfs error!\n");
        return -ENOMEM;
    }

    DEBUGFS_ADD_FILE(sdiowr, dir_diags, S_IRUSR);
    DEBUGFS_ADD_FILE(sdiord, dir_diags, S_IRUSR);

    card->dir = dir_diags;

    return 0;

err:
    hd_debugfs_unregister(card);
    return -ENOMEM;
}

#endif
