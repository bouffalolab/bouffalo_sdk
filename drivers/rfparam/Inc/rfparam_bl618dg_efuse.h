#ifndef RFPARAM_BL618DG_EFUSE_H
#define RFPARAM_BL618DG_EFUSE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RFPARAM_DG_WF_5G_HP,
    RFPARAM_DG_WF_5G_LP,
    RFPARAM_DG_WF_2G_HP,
    RFPARAM_DG_WF_2G_LP,
    RFPARAM_DG_WFBZ_HP,
    RFPARAM_DG_WFBZ_LP,
    RFPARAM_DG_BZ_HP,
    RFPARAM_DG_BZ_LP,
    RFPARAM_DG_POWER_TYPE_MAX,
} rfparam_bl618dg_power_type_t;

enum {
    RFPARAM_DG_OK = 0,
    RFPARAM_DG_EMPTY = 1,
    RFPARAM_DG_INVALID = -1,
    RFPARAM_DG_PARITY = -2,
    RFPARAM_DG_IO = -3,
    RFPARAM_DG_FORMAT = -4,
    RFPARAM_DG_CRC = -5,
    RFPARAM_DG_MEDIA = -6,
};

/* Raw counts: 5G = 8, 2G = 3, BZ = 5; slots are numbered 0..2.
 * reload=0 reuses a private physical snapshot, never the MFG shadow.
 * Failed reads leave output and slot unchanged. */
int rfparam_bl618dg_efuse_read_poweroffset(rfparam_bl618dg_power_type_t type,
                                         int8_t *value, uint8_t reload,
                                         uint8_t *slot);
int rfparam_bl618dg_efuse_read_capcode(uint8_t *value, uint8_t reload,
                                     uint8_t *slot);

/* Shared read-only decoder for the identical eFuse and Flash image fields. */
int rfparam_bl618dg_decode_poweroffset(rfparam_bl618dg_power_type_t type,
                                     const uint32_t *words, int8_t *value,
                                     uint8_t *slot);

#ifdef __cplusplus
}
#endif

#endif
