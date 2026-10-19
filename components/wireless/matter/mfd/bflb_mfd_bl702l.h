#ifndef __BFLB_MFD_BL702L_H

#include <stdbool.h>
#include <stdlib.h>
#include <bl702l.h>
#include <bl702l_ef_ctrl.h>
#include <bl702l_sec_eng.h>
#include <bl702l_romdriver.h>

#include <softcrc.h>
#include <hal_boot2.h>

#define MFD_XIP_BASE BL702L_FLASH_XIP_BASE
#define MFD_XIP_END BL702L_FLASH_XIP_END
#define BOOT2_PARTITION_ADDR_ACTIVE hal_boot2_partition_addr_active
#define MFD_RUNNING_MEMORY_CHECK() 

#define SEC_ENG_REQUEST_AES_ACCESS() Sec_Eng_Group0_Request_AES_Access()
#define SEC_ENG_RELEASE_AES_ACCESS() Sec_Eng_Group0_Release_AES_Access()
#define SF_CTRL_GET_FLASH_IMAGE_OFFSET() RomDriver_SF_Ctrl_Get_Flash_Image_Offset()

#endif
