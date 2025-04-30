#ifndef _LFS_PORT_H
#define _LFS_PORT_H

#include "lfs.h"

#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif

struct lfs_context {
    lfs_t lfs;
    uint32_t flash_addr;
#ifdef CONFIG_FREERTOS
    SemaphoreHandle_t fs_giant_lock;
#endif
#ifndef CONFIG_LITTLEFS_FLASH_ADDRESS
    char *partition_name;
#endif
};

lfs_t *lfs_xip_init(struct lfs_context *lfs_xip_ctx, struct lfs_config *cfg);

int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size);
int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size);
int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block);
int lfs_xip_flash_sync(const struct lfs_config *c);

#endif
