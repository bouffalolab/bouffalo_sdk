#ifndef __BFLB_MFD_H
#define __BFLB_MFD_H

#include <stdbool.h>
#include <stdlib.h>
#if defined BL616
#include <bl616.h>
#include <bl616_romdriver_e907.h>
#elif defined BL616CL
#include <bl616cl.h>
#include <bl616cl_romdriver_e907.h>
#endif

#include <bflb_boot2.h>
#include <bflb_flash.h>
#include <bflb_sec_aes.h>

#if defined BL616
#define MFD_XIP_BASE BL616_FLASH_XIP_BASE
#define MFD_XIP_END BL616_FLASH_XIP_END
#elif defined BL616CL
#define MFD_XIP_BASE BL616CL_FLASH_XIP_BASE
#define MFD_XIP_END BL616CL_FLASH_XIP_END
#endif
#define SF_CTRL_GET_FLASH_IMAGE_OFFSET() bflb_sf_ctrl_get_flash_image_offset(0, SF_CTRL_FLASH_BANK0)
#define BOOT2_PARTITION_ADDR_ACTIVE bflb_boot2_partition_addr_active
#define MFD_RUNNING_MEMORY_CHECK() 

#endif