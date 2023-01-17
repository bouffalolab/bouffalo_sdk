#ifndef _LFS_PORT_H
#define _LFS_PORT_H

#include "lfs.h"

extern int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, void *buffer, lfs_size_t size);
extern int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, const void *buffer, lfs_size_t size);
extern int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block);
extern int lfs_xip_flash_sync(const struct lfs_config *c);

#endif