#ifndef RFPARAM_BL616CL_FLASH_OTP_H
#define RFPARAM_BL616CL_FLASH_OTP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RFPARAM_BL616CL_MEDIA_ERROR = -1,
    RFPARAM_BL616CL_MEDIA_EFUSE = 0,
    RFPARAM_BL616CL_MEDIA_FLASH_OTP = 1,
} rfparam_bl616cl_media_t;

#define RFPARAM_BL616CL_FLASH_OTP_MAC_EMPTY 1

rfparam_bl616cl_media_t rfparam_bl616cl_get_rf_param_media(void);
int rfparam_bl616cl_flash_otp_read_capcode(uint8_t *capcode, uint8_t reload);
int rfparam_bl616cl_flash_otp_read_hp_poweroffset(int8_t pwr_offset[14],
                                                  uint8_t reload);
int rfparam_bl616cl_flash_otp_read_lp_poweroffset(int8_t pwr_offset[14],
                                                  uint8_t reload);
int rfparam_bl616cl_flash_otp_read_bz_poweroffset(int8_t pwr_offset[5],
                                                  uint8_t reload);
int rfparam_bl616cl_flash_otp_read_temperature_mp(int8_t *temperature_mp,
                                                  uint8_t reload);
/* Select MAC3, MAC2, then MAC1; slot is numbered 3, 2, or 1. */
int rfparam_bl616cl_flash_otp_read_mac(uint8_t mac[6], uint8_t reload);
int rfparam_bl616cl_flash_otp_read_mac_slot(uint8_t mac[6], uint8_t reload,
                                             uint8_t *slot);

#ifdef __cplusplus
}
#endif

#endif
