#ifndef __BL602_MFG_MEDIA_H__
#define __BL602_MFG_MEDIA_H__

#include "stdint.h"
#include "bl602_mfg_efuse.h"

int8_t mfg_media_read_macaddr_need_lock(uint8_t mac[6],uint8_t reload);
int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload);
int mfg_media_read_macaddr(uint8_t mac[6], uint8_t reload);

#endif/*__BL602_MFG_MEDIA_H__*/
