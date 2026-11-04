#ifndef RFPARAM_BL618DG_FLASH_OTP_H
#define RFPARAM_BL618DG_FLASH_OTP_H

#include "rfparam_bl618dg_efuse.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RFPARAM_BL618DG_MEDIA_ERROR = -1,
    RFPARAM_BL618DG_MEDIA_EFUSE = 0,
    RFPARAM_BL618DG_MEDIA_FLASH_OTP = 1,
} rfparam_bl618dg_media_t;

rfparam_bl618dg_media_t rfparam_bl618dg_get_rf_param_media(void);
int rfparam_bl618dg_flash_otp_read_poweroffset(rfparam_bl618dg_power_type_t type,
                                             int8_t *value, uint8_t reload,
                                             uint8_t *slot);

#ifdef __cplusplus
}
#endif

#endif
