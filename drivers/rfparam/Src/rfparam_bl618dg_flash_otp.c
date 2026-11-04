#include "rfparam_bl618dg_flash_otp.h"

#include "bflb_common.h"
#include "bflb_efuse.h"
#include "bflb_flash.h"
#include "bflb_flash_secreg.h"
#include "bflb_irq.h"
#include <stddef.h>
#include <string.h>

#define RFPARAM_FLASH_OTP_REGION_INDEX 1
#define RFPARAM_FLASH_OTP_MAGIC        0x43525750U
#define RFPARAM_FLASH_OTP_VERSION      1

struct rfparam_flash_otp_record {
    uint32_t magic;
    uint16_t version;
    uint16_t length;
    uint32_t reserved;
    uint32_t image[24];
    uint32_t crc32;
};

_Static_assert(sizeof(struct rfparam_flash_otp_record) == 112,
               "unexpected BL618DG Flash OTP record layout");
_Static_assert(offsetof(struct rfparam_flash_otp_record, image) == 0x0c,
               "unexpected BL618DG Flash OTP image offset");
_Static_assert(offsetof(struct rfparam_flash_otp_record, crc32) == 0x6c,
               "unexpected BL618DG Flash OTP CRC offset");

static bflb_flash_otp_config_t flash_otp_cfg;
static struct rfparam_flash_otp_record flash_otp_record;
static uint8_t record_loaded;
static uint8_t media_initialized;
static rfparam_bl618dg_media_t rf_param_media = RFPARAM_BL618DG_MEDIA_ERROR;

#if !defined(CFG_MFG_SDIO) && !defined(CFG_MFG_USB)
static int flash_otp_init(void)
{
    const bflb_flash_secreg_param_t *param;
    bflb_flash_secreg_region_info_t info;
    uint8_t *cfg;
    uint32_t len;

    if (bflb_flash_secreg_get_param(bflb_flash_get_jedec_id(), &param) != 0 ||
        RFPARAM_FLASH_OTP_REGION_INDEX >= param->region_count) {
        return RFPARAM_DG_MEDIA;
    }
    bflb_flash_get_cfg(&cfg, &len);
    if (cfg == NULL || len < sizeof(spi_flash_cfg_type)) {
        return RFPARAM_DG_MEDIA;
    }
    flash_otp_cfg.flash_cfg = (const spi_flash_cfg_type *)cfg;
    flash_otp_cfg.param = param;
    if (bflb_flash_secreg_get_info_by_idx(&flash_otp_cfg,
                                          RFPARAM_FLASH_OTP_REGION_INDEX, &info) != 0 ||
        info.secreg_size < sizeof(struct rfparam_flash_otp_record)) {
        return RFPARAM_DG_MEDIA;
    }
    return RFPARAM_DG_OK;
}
#endif

rfparam_bl618dg_media_t rfparam_bl618dg_get_rf_param_media(void)
{
    uintptr_t flags = bflb_irq_save();

    if (!media_initialized) {
        media_initialized = 1;
#if defined(CFG_MFG_SDIO) || defined(CFG_MFG_USB)
        rf_param_media = RFPARAM_BL618DG_MEDIA_EFUSE;
#else
        bflb_efuse_device_info_type info;
        bflb_efuse_get_device_info(&info);
        uint32_t id = bflb_flash_get_jedec_id();
        if ((id & 0xff) == 0 || (id & 0xff) == 0xff) {
            (void)bflb_flash_init();
            id = bflb_flash_get_jedec_id();
        }
        if ((id & 0xff) == 0 || (id & 0xff) == 0xff) {
            rf_param_media = info.flash_info == 0 ?
                                 RFPARAM_BL618DG_MEDIA_EFUSE :
                                 RFPARAM_BL618DG_MEDIA_ERROR;
        } else if (flash_otp_init() == 0) {
            rf_param_media = RFPARAM_BL618DG_MEDIA_FLASH_OTP;
        }
#endif
    }
    bflb_irq_restore(flags);
    return rf_param_media;
}

static int flash_otp_load(uint8_t reload)
{
    struct rfparam_flash_otp_record record;
    uint32_t crc_len = offsetof(struct rfparam_flash_otp_record, crc32) -
                       offsetof(struct rfparam_flash_otp_record, version);

    if (rfparam_bl618dg_get_rf_param_media() != RFPARAM_BL618DG_MEDIA_FLASH_OTP) {
        return RFPARAM_DG_MEDIA;
    }
    if (!reload && record_loaded) {
        return RFPARAM_DG_OK;
    }
    record_loaded = 0;
    if (bflb_flash_secreg_read_by_idx(&flash_otp_cfg,
                                      RFPARAM_FLASH_OTP_REGION_INDEX, 0,
                                      &record, sizeof(record)) != 0) {
        return RFPARAM_DG_IO;
    }
    const uint8_t *bytes = (const uint8_t *)&record;
    uint8_t empty_zero = 1;
    uint8_t empty_ff = 1;
    for (unsigned int i = 0; i < sizeof(record); i++) {
        empty_zero &= bytes[i] == 0;
        empty_ff &= bytes[i] == 0xff;
    }
    if (empty_zero || empty_ff) {
        return RFPARAM_DG_EMPTY;
    }
    if (record.magic != RFPARAM_FLASH_OTP_MAGIC ||
        record.version != RFPARAM_FLASH_OTP_VERSION ||
        record.length != sizeof(record) || record.reserved != 0) {
        return RFPARAM_DG_FORMAT;
    }
    if (record.crc32 != bflb_soft_crc32(&record.version, crc_len)) {
        return RFPARAM_DG_CRC;
    }
    flash_otp_record = record;
    record_loaded = 1;
    return RFPARAM_DG_OK;
}

int rfparam_bl618dg_flash_otp_read_poweroffset(rfparam_bl618dg_power_type_t type,
                                             int8_t *value, uint8_t reload,
                                             uint8_t *slot)
{
    /* Word offsets within the 96-byte image, in MFG type order. */
    static const uint8_t offsets[RFPARAM_DG_POWER_TYPE_MAX] = {
        0, 4, 8, 10, 12, 15, 18, 21,
    };

    if ((unsigned int)type >= RFPARAM_DG_POWER_TYPE_MAX || value == NULL) {
        return RFPARAM_DG_INVALID;
    }
    uintptr_t flags = bflb_irq_save();
    int ret = flash_otp_load(reload);
    if (ret == 0) {
        ret = rfparam_bl618dg_decode_poweroffset(type,
                                                 &flash_otp_record.image[offsets[type]],
                                                 value, slot);
    }
    bflb_irq_restore(flags);
    return ret;
}
