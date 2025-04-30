#ifndef __BL_MFD_BL602_H

#include <stdbool.h>
#include <stdlib.h>
#include <bl602.h>
#include <bl602_romdriver.h>
#include <bl602_ef_ctrl.h>
#include <bl602_sec_eng.h>

#include <softcrc.h>
#include <hal_boot2.h>
#include <FreeRTOS.h>

#define MFD_XIP_BASE BL602_FLASH_XIP_BASE
#define MFD_XIP_END BL602_FLASH_XIP_END
#define MFD_WRAM_START BL602_WRAM_BASE
#define MFD_WRAM_END BL602_WRAM_END
#define BOOT2_PARTITION_ADDR_ACTIVE hal_boot2_partition_addr_active

#define SEC_ENG_REQUEST_AES_ACCESS() 
#define SEC_ENG_RELEASE_AES_ACCESS() 
#define SF_CTRL_GET_FLASH_IMAGE_OFFSET() RomDriver_SF_Ctrl_Get_Flash_Image_Offset()
#define MFD_RUNNING_MEMORY_CHECK() \
    configASSERT(MFD_WRAM_START <= (uint32_t) mfd_decrypt_buf && (uint32_t) mfd_decrypt_buf < MFD_WRAM_END);

bool bl_mfd_decrypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv);

#endif
