#ifndef _LFS_PORT_H
#define _LFS_PORT_H

#include "lfs.h"

lfs_t *lfs_xip_init(void);

int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size);
int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size);
int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block);
int lfs_xip_flash_sync(const struct lfs_config *c);

#endif
