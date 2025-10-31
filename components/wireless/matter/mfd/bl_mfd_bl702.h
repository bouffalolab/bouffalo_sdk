#ifndef __BL_MFD_BL602_H

#include <stdbool.h>
#include <stdlib.h>
#include <bl702.h>
#include <bl702_ef_ctrl.h>
#include <bl702_sec_eng.h>
#include <bl702_romdriver.h>

#include <softcrc.h>
#include <hal_boot2.h>
#include <FreeRTOS.h>

#define MFD_XIP_BASE BL702_FLASH_XIP_BASE
#define MFD_XIP_END BL702_FLASH_XIP_END
#define MFD_WRAM_START BL702_WRAM_BASE
#define MFD_WRAM_END BL702_WRAM_END
#define BOOT2_PARTITION_ADDR_ACTIVE hal_boot2_partition_addr_active
#define MFD_RUNNING_MEMORY_CHECK() \
    configASSERT(MFD_WRAM_START <= (uint32_t) mfd_decrypt_buf && (uint32_t) mfd_decrypt_buf < MFD_WRAM_END);

#define SEC_ENG_REQUEST_AES_ACCESS() 
#define SEC_ENG_RELEASE_AES_ACCESS() 
#define SF_CTRL_GET_FLASH_IMAGE_OFFSET() RomDriver_SF_Ctrl_Get_Flash_Image_Offset()

bool bl_mfd_decrypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv);

#endif
