#ifndef __BFLB_MTD_H__
#define __BFLB_MTD_H__

#include "bflb_core.h"

typedef void *bflb_mtd_handle_t;
typedef struct {
    char name[16];
    unsigned int offset;
    unsigned int size;
    void *xip_addr;
} bflb_mtd_info_t;

#define BFLB_MTD_OPEN_FLAG_NONE           (0)
/* open backup partition */
#define BFLB_MTD_OPEN_FLAG_BACKUP        (1 << 0)
#define BFLB_MTD_OPEN_FLAG_BUSADDR       (1 << 1)

void bflb_mtd_init(void);
int bflb_mtd_open(const char *name, bflb_mtd_handle_t *handle, unsigned int flags);
int bflb_mtd_close(bflb_mtd_handle_t handle);
int bflb_mtd_info(bflb_mtd_handle_t handle, bflb_mtd_info_t *info);
int bflb_mtd_erase(bflb_mtd_handle_t handle, unsigned int addr, unsigned int size);
int bflb_mtd_erase_all(bflb_mtd_handle_t handle);
int bflb_mtd_write(bflb_mtd_handle_t handle, unsigned int addr, unsigned int size, const uint8_t *data);
int bflb_mtd_read(bflb_mtd_handle_t handle,  unsigned int addr, unsigned int size, uint8_t *data);
int bflb_mtd_size(bflb_mtd_handle_t handle, unsigned int *size);

#define BFLB_MTD_PARTITION_NAME_PSM               "PSM"
#define BFLB_MTD_PARTITION_NAME_FW_DEFAULT        "FW"
#define BFLB_MTD_PARTITION_NAME_ROMFS             "media"

#endif
