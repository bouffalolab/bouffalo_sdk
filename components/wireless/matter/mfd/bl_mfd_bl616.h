#ifndef __BL_MFD_BL616_H

#include <stdbool.h>
#include <stdlib.h>
#include <bl616.h>
#include <bflb_boot2.h>
#include <bflb_flash.h>
#include <bflb_sec_aes.h>
#include <bl616_romdriver_e907.h>

#define MFD_XIP_BASE BL616_FLASH_XIP_BASE
#define MFD_XIP_END BL616_FLASH_XIP_END
#define SF_CTRL_GET_FLASH_IMAGE_OFFSET() bflb_sf_ctrl_get_flash_image_offset(0, SF_CTRL_FLASH_BANK0)
#define BOOT2_PARTITION_ADDR_ACTIVE bflb_boot2_partition_addr_active
#define MFD_RUNNING_MEMORY_CHECK() 

bool bl_mfd_decrypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv);

#endif
