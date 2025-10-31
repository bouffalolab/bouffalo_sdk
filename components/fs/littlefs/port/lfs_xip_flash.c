#include <assert.h>
#include <errno.h>

#include "lfs.h"
#include "bflb_flash.h"
#include "bflb_l1c.h"
#include "bflb_mtd.h"

#ifdef CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif
#include "lfs_port.h"

#define DBG_TAG "LFS"

#include "log.h"

/*!< use memory area after 512k */
#ifdef CONFIG_LITTLEFS_FLASH_ADDRESS
#ifndef LITTLEFS_FLASH_SIZE
#error not def LITTLEFS_FLASH_SIZE
#endif
#endif

#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
static int lfs_xip_giant_lock(const struct lfs_config *c);
static int lfs_xip_giant_unlock(const struct lfs_config *c);
#endif

lfs_t *lfs_xip_init(struct lfs_context *lfs_xip_ctx, struct lfs_config *cfg)
{
    int ret;
    lfs_t *lfs = &lfs_xip_ctx->lfs;

    if (lfs->cfg == cfg) {
        return lfs;
    } else {
        cfg->context = lfs_xip_ctx;
        cfg->read = lfs_xip_flash_read;
        cfg->prog = lfs_xip_flash_prog;
        cfg->erase = lfs_xip_flash_erase;
        cfg->sync = lfs_xip_flash_sync;
#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
        cfg->lock = lfs_xip_giant_lock;
        cfg->unlock = lfs_xip_giant_unlock;
#endif
    }

#ifndef CONFIG_LITTLEFS_FLASH_ADDRESS
    bflb_mtd_info_t info;
    bflb_mtd_handle_t handle;

    ret = bflb_mtd_open(lfs_xip_ctx->partition_name, &handle, BFLB_MTD_OPEN_FLAG_BUSADDR);
    if (ret < 0) {
        LOG_E("no valid PSM partition found\r\n");
        errno = LFS_ERR_IO;
        return NULL;
    }

    memset(&info, 0, sizeof(info));
    bflb_mtd_info(handle, &info);
    LOG_I("Found valid PSM partition, XIP addr %08x, flash addr %08x, size %d\r\n",
          info.xip_addr,
          info.offset,
          info.size);
    lfs_xip_ctx->flash_addr = info.offset;
    cfg->block_count = info.size / cfg->block_size;
    bflb_mtd_close(handle);
#else
    lfs_xip_ctx->flash_addr = CONFIG_LITTLEFS_FLASH_ADDRESS;
    cfg.block_count = LITTLEFS_FLASH_SIZE / cfg->block_size;
#endif

#ifdef CONFIG_FREERTOS
#if configUSE_RECURSIVE_MUTEXES
    lfs_xip_ctx->fs_giant_lock = xSemaphoreCreateRecursiveMutex();
#else
    lfs_xip_ctx->fs_giant_lock = xSemaphoreCreateMutex();
#endif
#endif

    // mount the filesystem
    ret = lfs_mount(lfs, cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (ret == LFS_ERR_CORRUPT) {
        LOG_W("try to reformat \r\n");
        ret = lfs_format(lfs, cfg);
        if (ret) {
            LOG_F("reformat fail\r\n");
            errno = LFS_ERR_CORRUPT;
            return NULL;
        }

        LOG_I("reformat success\r\n");
        ret = lfs_mount(lfs, cfg);
        if (ret) {
            errno = ret;
            return NULL;
        }
    } else if (ret != LFS_ERR_OK) {
        errno = ret;
        return NULL;
    }

    LOG_I("mount success\r\n");

    return lfs;
}

#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
static int lfs_xip_giant_lock(const struct lfs_config *c)
{
    struct lfs_context *ctx = c->context;
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreTakeRecursive(ctx->fs_giant_lock, portMAX_DELAY);
#else
    xSemaphoreTake(ctx->fs_giant_lock, portMAX_DELAY);
#endif
    return 0;
}

static int lfs_xip_giant_unlock(const struct lfs_config *c)
{
    struct lfs_context *ctx = c->context;
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreGiveRecursive(ctx->fs_giant_lock);
#else
    xSemaphoreGive(ctx->fs_giant_lock);
#endif
    return 0;
}
#endif

/*****************************************************************************
* @brief        Read a region in a block. Negative error codes are propagated
*               to the user.
* @param[in]    c
* @param[in]    block
* @param[in]    off
* @param[out]   buffer
* @param[in]    size
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct lfs_context *ctx = c->context;
    return bflb_flash_read(ctx->flash_addr + block * c->block_size + off,
                           (uint8_t *)buffer, size);
}

/*****************************************************************************
* @brief        Program a region in a block. The block must have previously
*               been erased. Negative error codes are propagated to the user.
*               May return LFS_ERR_CORRUPT if the block should be considered bad.
* @param[in]    c
* @param[in]    block
* @param[in]    off
* @param[in]    buffer
* @param[in]    size
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct lfs_context *ctx = c->context;
    return bflb_flash_write(ctx->flash_addr + block * c->block_size + off,
                            (uint8_t *)buffer, size);
}

/*****************************************************************************
* @brief        Erase a block. A block must be erased before being programmed.
*               The state of an erased block is undefined. Negative error codes
*               are propagated to the user.
*               May return LFS_ERR_CORRUPT if the block should be considered bad.
* @param[in]    c
* @param[in]    block
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block)
{
    struct lfs_context *ctx = c->context;
    return bflb_flash_erase(ctx->flash_addr + block * c->block_size, c->block_size);
}

/*****************************************************************************
* @brief        Sync the state of the underlying block device. Negative error
*               codes are propagated to the user.
* @param[in]    c
*
* @retval int
*****************************************************************************/
int lfs_xip_flash_sync(const struct lfs_config *c)
{
    /*!< if use xip, may need to clean cache */
    return 0;
}
