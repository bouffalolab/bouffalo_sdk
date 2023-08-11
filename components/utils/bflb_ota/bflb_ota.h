#ifndef __HOSAL_OTA_H__
#define __HOSAL_OTA_H__
#include <stdint.h>

#define HOSAL_OTA_FILE_SIZE_MAX    0x100000  // 1M

#define BL_MTD_PARTITION_NAME_FW_DEFAULT    "FW"

int bflb_ota_start(uint32_t file_size);
int bflb_ota_update(uint32_t filesize, uint32_t offset, uint8_t *buf, uint32_t buf_len);
int bflb_ota_finish(uint8_t check_hash);
int bflb_ota_read(uint32_t offset, uint8_t *buf, uint32_t buf_len);
int bflb_ota_check(void);
int bflb_ota_apply(void);
void bflb_ota_abort(void);
#endif
