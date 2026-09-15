#include "rfparam_bl616cl_flash_otp.h"

#include "bflb_common.h"
#include "bflb_efuse.h"
#include "bflb_flash.h"
#include "bflb_flash_secreg.h"
#include "bflb_irq.h"
#include <stddef.h>
#include <string.h>

#define RFPARAM_FLASH_OTP_REGION_INDEX 1
#define RFPARAM_FLASH_OTP_MAGIC        0x31434652U
#define RFPARAM_FLASH_OTP_VERSION      1
#define RFPARAM_FLASH_OTP_EFUSE_BASE   0x1cU
#define RFPARAM_FLASH_OTP_EFUSE_END    0x60U
#define RFPARAM_FLASH_OTP_IMAGE_WORDS  \
    ((RFPARAM_FLASH_OTP_EFUSE_END - RFPARAM_FLASH_OTP_EFUSE_BASE) / \
     sizeof(uint32_t))
#define RFPARAM_BL616CL_TRIM_SLOT_COUNT 6
#define RFPARAM_BL616CL_MAC_SLOT_COUNT 2
#define EFUSE_BIT(address, bit) \
    (((address) - RFPARAM_FLASH_OTP_EFUSE_BASE) * 8U + (bit))

struct rfparam_flash_otp_record {
    uint32_t magic;
    uint16_t version;
    uint16_t length;
    uint32_t reserved;
    uint32_t image[RFPARAM_FLASH_OTP_IMAGE_WORDS];
    uint32_t crc32;
};

struct rfparam_flash_otp_trim_slot_cfg {
    uint16_t en_addr;
    uint16_t parity_addr;
    uint16_t value_addr;
    uint8_t value_len;
};

struct rfparam_flash_otp_trim_cfg {
    uint8_t slot_count;
    struct rfparam_flash_otp_trim_slot_cfg slot[RFPARAM_BL616CL_TRIM_SLOT_COUNT];
};

struct rfparam_flash_otp_mac_slot_cfg {
    uint16_t low_addr;
    uint16_t high_addr;
    uint16_t crc_addr;
};

static const struct rfparam_flash_otp_trim_cfg capcode_cfg = {
    .slot_count = 3,
    .slot = {
        { .en_addr = EFUSE_BIT(0x20, 31), .parity_addr = EFUSE_BIT(0x20, 30),
          .value_addr = EFUSE_BIT(0x20, 22), .value_len = 8 },
        { .en_addr = EFUSE_BIT(0x28, 31), .parity_addr = EFUSE_BIT(0x28, 30),
          .value_addr = EFUSE_BIT(0x28, 22), .value_len = 8 },
        { .en_addr = EFUSE_BIT(0x38, 5), .parity_addr = EFUSE_BIT(0x38, 4),
          .value_addr = EFUSE_BIT(0x3c, 24), .value_len = 8 },
    },
};

static const struct rfparam_flash_otp_trim_cfg hp_poweroffset_cfg = {
    .slot_count = 6,
    .slot = {
        { .en_addr = EFUSE_BIT(0x2c, 31), .parity_addr = EFUSE_BIT(0x2c, 30),
          .value_addr = EFUSE_BIT(0x2c, 15), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x30, 31), .parity_addr = EFUSE_BIT(0x30, 30),
          .value_addr = EFUSE_BIT(0x2c, 0), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x44, 31), .parity_addr = EFUSE_BIT(0x44, 30),
          .value_addr = EFUSE_BIT(0x40, 15), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x48, 31), .parity_addr = EFUSE_BIT(0x48, 30),
          .value_addr = EFUSE_BIT(0x48, 15), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x4c, 31), .parity_addr = EFUSE_BIT(0x4c, 30),
          .value_addr = EFUSE_BIT(0x48, 0), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x50, 31), .parity_addr = EFUSE_BIT(0x50, 30),
          .value_addr = EFUSE_BIT(0x4c, 15), .value_len = 15 },
    },
};

static const struct rfparam_flash_otp_trim_cfg lp_poweroffset_cfg = {
    .slot_count = 6,
    .slot = {
        { .en_addr = EFUSE_BIT(0x34, 31), .parity_addr = EFUSE_BIT(0x34, 30),
          .value_addr = EFUSE_BIT(0x30, 15), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x34, 29), .parity_addr = EFUSE_BIT(0x34, 28),
          .value_addr = EFUSE_BIT(0x30, 0), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x44, 29), .parity_addr = EFUSE_BIT(0x44, 28),
          .value_addr = EFUSE_BIT(0x40, 0), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x54, 29), .parity_addr = EFUSE_BIT(0x54, 28),
          .value_addr = EFUSE_BIT(0x4c, 0), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x54, 27), .parity_addr = EFUSE_BIT(0x54, 26),
          .value_addr = EFUSE_BIT(0x50, 15), .value_len = 15 },
        { .en_addr = EFUSE_BIT(0x54, 31), .parity_addr = EFUSE_BIT(0x54, 30),
          .value_addr = EFUSE_BIT(0x50, 0), .value_len = 15 },
    },
};

static const struct rfparam_flash_otp_trim_cfg bz_poweroffset_cfg = {
    .slot_count = 5,
    .slot = {
        { .en_addr = EFUSE_BIT(0x34, 27), .parity_addr = EFUSE_BIT(0x34, 26),
          .value_addr = EFUSE_BIT(0x34, 1), .value_len = 25 },
        { .en_addr = EFUSE_BIT(0x34, 0), .parity_addr = EFUSE_BIT(0x38, 31),
          .value_addr = EFUSE_BIT(0x38, 6), .value_len = 25 },
        { .en_addr = EFUSE_BIT(0x44, 2), .parity_addr = EFUSE_BIT(0x44, 1),
          .value_addr = EFUSE_BIT(0x44, 3), .value_len = 25 },
        { .en_addr = EFUSE_BIT(0x54, 0), .parity_addr = EFUSE_BIT(0x58, 31),
          .value_addr = EFUSE_BIT(0x54, 1), .value_len = 25 },
        { .en_addr = EFUSE_BIT(0x58, 30), .parity_addr = EFUSE_BIT(0x58, 29),
          .value_addr = EFUSE_BIT(0x58, 2), .value_len = 25 },
    },
};

static const struct rfparam_flash_otp_trim_cfg temperature_mp_cfg = {
    .slot_count = 6,
    .slot = {
        { .en_addr = EFUSE_BIT(0x38, 3), .parity_addr = EFUSE_BIT(0x38, 2),
          .value_addr = EFUSE_BIT(0x3c, 16), .value_len = 8 },
        { .en_addr = EFUSE_BIT(0x38, 1), .parity_addr = EFUSE_BIT(0x38, 0),
          .value_addr = EFUSE_BIT(0x3c, 8), .value_len = 8 },
        { .en_addr = EFUSE_BIT(0x40, 31), .parity_addr = EFUSE_BIT(0x40, 30),
          .value_addr = EFUSE_BIT(0x3c, 0), .value_len = 8 },
        { .en_addr = EFUSE_BIT(0x58, 28), .parity_addr = EFUSE_BIT(0x58, 27),
          .value_addr = EFUSE_BIT(0x5c, 16), .value_len = 8 },
        { .en_addr = EFUSE_BIT(0x58, 1), .parity_addr = EFUSE_BIT(0x58, 0),
          .value_addr = EFUSE_BIT(0x5c, 24), .value_len = 8 },
        { .en_addr = EFUSE_BIT(0x5c, 15), .parity_addr = EFUSE_BIT(0x5c, 14),
          .value_addr = EFUSE_BIT(0x5c, 6), .value_len = 8 },
    },
};

static const struct rfparam_flash_otp_mac_slot_cfg mac_slots[
    RFPARAM_BL616CL_MAC_SLOT_COUNT] = {
    { .low_addr = EFUSE_BIT(0x1c, 0), .high_addr = EFUSE_BIT(0x20, 0),
      .crc_addr = EFUSE_BIT(0x20, 16) },
    { .low_addr = EFUSE_BIT(0x24, 0), .high_addr = EFUSE_BIT(0x28, 0),
      .crc_addr = EFUSE_BIT(0x28, 16) },
};

_Static_assert(offsetof(struct rfparam_flash_otp_record, image) == 0x0c,
               "unexpected BL616CL Flash OTP image offset");
_Static_assert(offsetof(struct rfparam_flash_otp_record, crc32) == 0x50,
               "unexpected BL616CL Flash OTP CRC offset");
_Static_assert(sizeof(struct rfparam_flash_otp_record) == 84,
               "unexpected BL616CL Flash OTP record layout");

static bflb_flash_otp_config_t flash_otp_cfg;
static struct rfparam_flash_otp_record flash_otp_record;
static rfparam_bl616cl_media_t rf_param_media = RFPARAM_BL616CL_MEDIA_ERROR;
static uint8_t media_initialized;
static uint8_t flash_otp_initialized;
static uint8_t record_loaded;

static uint32_t flash_otp_crc(const struct rfparam_flash_otp_record *record)
{
    const uint8_t *start = (const uint8_t *)&record->version;
    uint32_t len = offsetof(struct rfparam_flash_otp_record, crc32) -
                   offsetof(struct rfparam_flash_otp_record, version);

    return bflb_soft_crc32((void *)start, len);
}

static uint32_t flash_otp_get_bits(uint16_t bit, uint8_t len)
{
    uint32_t value = 0;

    for (uint8_t i = 0; i < len; i++) {
        if (flash_otp_record.image[(bit + i) / 32] &
            (1U << ((bit + i) % 32))) {
            value |= 1U << i;
        }
    }
    return value;
}

static uint8_t flash_otp_parity(uint32_t value, uint8_t len)
{
    uint8_t parity = 0;

    for (uint8_t i = 0; i < len; i++) {
        parity ^= (value >> i) & 1U;
    }
    return parity;
}

static int flash_otp_load(uint8_t reload);

static uint8_t mac_zero_count(const uint8_t mac[6])
{
    uint8_t count = 0;

    for (uint8_t i = 0; i < 6; i++) {
        uint8_t value = mac[i];

        for (uint8_t bit = 0; bit < 8; bit++) {
            count += ((value >> bit) & 1U) == 0;
        }
    }
    return count;
}

static int flash_otp_read_mac_slot(uint8_t mac[6], uint8_t reload,
                                   uint8_t *slot)
{
    uint8_t local_mac[6];
    uint32_t low;
    uint32_t high;
    uint8_t index;

    if (mac == NULL || flash_otp_load(reload) != 0) {
        return -1;
    }

    for (index = RFPARAM_BL616CL_MAC_SLOT_COUNT; index > 0; index--) {
        const struct rfparam_flash_otp_mac_slot_cfg *cfg =
            &mac_slots[index - 1];
        uint8_t crc = flash_otp_get_bits(cfg->crc_addr, 6);

        low = flash_otp_get_bits(cfg->low_addr, 32);
        high = flash_otp_get_bits(cfg->high_addr, 16);
        if (low != 0 || high != 0 || crc != 0) {
            memcpy(local_mac, &low, 4);
            memcpy(local_mac + 4, &high, 2);
            if (crc != (mac_zero_count(local_mac) & 0x3fU)) {
                return -1;
            }
            for (uint8_t i = 0; i < 6; i++) {
                mac[i] = local_mac[5 - i];
            }
            if (slot != NULL) {
                *slot = index + 1;
            }
            return 0;
        }
    }
    return RFPARAM_BL616CL_FLASH_OTP_MAC_EMPTY;
}

static int flash_otp_init(void)
{
    const bflb_flash_secreg_param_t *param;
    bflb_flash_secreg_region_info_t info;
    uint8_t *flash_cfg;
    uint32_t flash_cfg_len;
    uint32_t jedec_id;

    if (flash_otp_initialized) {
        return 0;
    }
    jedec_id = bflb_flash_get_jedec_id();
    if (jedec_id == 0 || bflb_flash_secreg_get_param(jedec_id, &param) != 0 ||
        RFPARAM_FLASH_OTP_REGION_INDEX >= param->region_count) {
        return -1;
    }
    bflb_flash_get_cfg(&flash_cfg, &flash_cfg_len);
    if (flash_cfg == NULL || flash_cfg_len < sizeof(spi_flash_cfg_type)) {
        return -1;
    }
    flash_otp_cfg.flash_cfg = (const spi_flash_cfg_type *)flash_cfg;
    flash_otp_cfg.param = param;
    if (bflb_flash_secreg_get_info_by_idx(&flash_otp_cfg,
                                           RFPARAM_FLASH_OTP_REGION_INDEX,
                                           &info) != 0 ||
        sizeof(struct rfparam_flash_otp_record) > info.secreg_size) {
        return -1;
    }
    flash_otp_initialized = 1;
    return 0;
}

rfparam_bl616cl_media_t rfparam_bl616cl_get_rf_param_media(void)
{
    bflb_efuse_device_info_type device_info;
    uint32_t jedec_id;

    if (media_initialized) {
        return rf_param_media;
    }
    media_initialized = 1;
#if defined(CFG_MFG_SDIO) || defined(CFG_MFG_USB)
    rf_param_media = RFPARAM_BL616CL_MEDIA_EFUSE;
    return rf_param_media;
#else
    bflb_efuse_get_device_info(&device_info);
    jedec_id = bflb_flash_get_jedec_id();
    if ((jedec_id & 0xff) == 0 || (jedec_id & 0xff) == 0xff) {
        (void)bflb_flash_init();
        jedec_id = bflb_flash_get_jedec_id();
    }
    if ((jedec_id & 0xff) == 0 || (jedec_id & 0xff) == 0xff) {
        rf_param_media = device_info.flash_info == 0 ?
                             RFPARAM_BL616CL_MEDIA_EFUSE :
                             RFPARAM_BL616CL_MEDIA_ERROR;
        return rf_param_media;
    }
    rf_param_media = flash_otp_init() == 0 ?
                         RFPARAM_BL616CL_MEDIA_FLASH_OTP :
                         RFPARAM_BL616CL_MEDIA_ERROR;
    return rf_param_media;
#endif
}

static int flash_otp_load(uint8_t reload)
{
    struct rfparam_flash_otp_record record;

    if (!reload && record_loaded) {
        return 0;
    }
    if (rfparam_bl616cl_get_rf_param_media() !=
            RFPARAM_BL616CL_MEDIA_FLASH_OTP ||
        bflb_flash_secreg_read_by_idx(&flash_otp_cfg,
                                      RFPARAM_FLASH_OTP_REGION_INDEX, 0,
                                      &record, sizeof(record)) != 0 ||
        record.magic != RFPARAM_FLASH_OTP_MAGIC ||
        record.version != RFPARAM_FLASH_OTP_VERSION ||
        record.length != sizeof(record) || record.reserved != 0 ||
        record.crc32 != flash_otp_crc(&record)) {
        record_loaded = 0;
        return -1;
    }
    flash_otp_record = record;
    record_loaded = 1;
    return 0;
}

static int flash_otp_read_trim(const struct rfparam_flash_otp_trim_cfg *cfg,
                               uint32_t *value, uint8_t reload)
{
    if (cfg == NULL || value == NULL || flash_otp_load(reload) != 0) {
        return -1;
    }
    for (int i = cfg->slot_count - 1; i >= 0; i--) {
        const struct rfparam_flash_otp_trim_slot_cfg *slot = &cfg->slot[i];
        uint32_t slot_value = flash_otp_get_bits(slot->value_addr,
                                                  slot->value_len);
        uint8_t enable = flash_otp_get_bits(slot->en_addr, 1);
        uint8_t parity = flash_otp_get_bits(slot->parity_addr, 1);

        if (slot_value == 0 && enable == 0 && parity == 0) {
            continue;
        }
        if (enable != 1 || parity != flash_otp_parity(slot_value,
                                                       slot->value_len)) {
            return -1;
        }
        *value = slot_value;
        return 0;
    }
    return -1;
}

static void flash_otp_unpack_offsets(int8_t *pwr_offset, uint8_t count,
                                     uint32_t value)
{
    for (uint8_t i = 0; i < count; i++) {
        uint8_t raw = (value >> (i * 5)) & 0x1fU;
        pwr_offset[i] = raw >= 16 ? (int8_t)(raw - 32) : (int8_t)raw;
    }
}

static void flash_otp_expand_poweroffset(int8_t pwr_offset[14],
                                         const int8_t anchor[3])
{
    int32_t step;

    memset(pwr_offset, 0, 14);
    pwr_offset[0] = anchor[0];
    step = (anchor[1] - anchor[0]) * 100 / 6;
    for (uint8_t i = 1; i < 6; i++) {
        pwr_offset[i] = (step * i + 50) / 100 + anchor[0];
    }
    pwr_offset[6] = anchor[1];
    step = (anchor[2] - anchor[1]) * 100 / 6;
    for (uint8_t i = 1; i < 6; i++) {
        pwr_offset[6 + i] = (step * i + 50) / 100 + anchor[1];
    }
    pwr_offset[12] = anchor[2];
    step = (anchor[2] - anchor[1]) * 100 / 5;
    pwr_offset[13] = (step * 2 + 50) / 100 + anchor[2];
}

static int flash_otp_read_wlan_poweroffset(
    const struct rfparam_flash_otp_trim_cfg *cfg, int8_t pwr_offset[14],
    uint8_t reload)
{
    int8_t anchor[3];
    uint32_t value;

    if (pwr_offset == NULL || flash_otp_read_trim(cfg, &value, reload) != 0) {
        return -1;
    }
    flash_otp_unpack_offsets(anchor, 3, value);
    flash_otp_expand_poweroffset(pwr_offset, anchor);
    return 0;
}

int rfparam_bl616cl_flash_otp_read_capcode(uint8_t *capcode, uint8_t reload)
{
    uintptr_t flags;
    uint32_t value;
    int ret;

    if (capcode == NULL) {
        return -1;
    }
    flags = bflb_irq_save();
    ret = flash_otp_read_trim(&capcode_cfg, &value, reload);
    bflb_irq_restore(flags);
    if (ret == 0) {
        *capcode = value;
    }
    return ret;
}

int rfparam_bl616cl_flash_otp_read_hp_poweroffset(int8_t pwr_offset[14],
                                                  uint8_t reload)
{
    uintptr_t flags = bflb_irq_save();
    int ret = flash_otp_read_wlan_poweroffset(&hp_poweroffset_cfg, pwr_offset,
                                               reload);
    bflb_irq_restore(flags);
    return ret;
}

int rfparam_bl616cl_flash_otp_read_lp_poweroffset(int8_t pwr_offset[14],
                                                  uint8_t reload)
{
    uintptr_t flags = bflb_irq_save();
    int ret = flash_otp_read_wlan_poweroffset(&lp_poweroffset_cfg, pwr_offset,
                                               reload);
    bflb_irq_restore(flags);
    return ret;
}

int rfparam_bl616cl_flash_otp_read_bz_poweroffset(int8_t pwr_offset[5],
                                                  uint8_t reload)
{
    uintptr_t flags;
    uint32_t value;
    int ret;

    if (pwr_offset == NULL) {
        return -1;
    }
    flags = bflb_irq_save();
    ret = flash_otp_read_trim(&bz_poweroffset_cfg, &value, reload);
    bflb_irq_restore(flags);
    if (ret == 0) {
        flash_otp_unpack_offsets(pwr_offset, 5, value);
    }
    return ret;
}

int rfparam_bl616cl_flash_otp_read_temperature_mp(int8_t *temperature_mp,
                                                  uint8_t reload)
{
    uintptr_t flags;
    int ret = -1;

    if (temperature_mp == NULL) {
        return -1;
    }
    flags = bflb_irq_save();
    if (flash_otp_load(reload) == 0) {
        for (int i = temperature_mp_cfg.slot_count - 1; i >= 0; i--) {
            const struct rfparam_flash_otp_trim_slot_cfg *slot =
                &temperature_mp_cfg.slot[i];
            uint32_t value = flash_otp_get_bits(slot->value_addr,
                                                 slot->value_len);
            uint8_t enable = flash_otp_get_bits(slot->en_addr, 1);
            uint8_t parity = flash_otp_get_bits(slot->parity_addr, 1);

            if (enable == 1 && parity == flash_otp_parity(value,
                                                           slot->value_len)) {
                *temperature_mp = value;
                ret = 0;
                break;
            }
        }
    }
    bflb_irq_restore(flags);
    return ret;
}

int rfparam_bl616cl_flash_otp_read_mac_slot(uint8_t mac[6], uint8_t reload,
                                             uint8_t *slot)
{
    rfparam_bl616cl_media_t media;
    uintptr_t flags;
    int ret = -1;

    if (mac == NULL) {
        return -1;
    }

    flags = bflb_irq_save();
    media = rfparam_bl616cl_get_rf_param_media();
    if (media == RFPARAM_BL616CL_MEDIA_FLASH_OTP) {
        ret = flash_otp_read_mac_slot(mac, reload, slot);
        if (ret == RFPARAM_BL616CL_FLASH_OTP_MAC_EMPTY) {
            if (!bflb_efuse_is_mac_address_slot_empty(0, reload)) {
                ret = bflb_efuse_read_mac_address_opt(0, mac, reload);
                if (ret == 0 && slot != NULL) {
                    *slot = 1;
                }
            }
        }
    } else if (media == RFPARAM_BL616CL_MEDIA_EFUSE) {
        for (int candidate = 2; candidate >= 0; candidate--) {
            if (bflb_efuse_is_mac_address_slot_empty(candidate, reload)) {
                continue;
            }
            ret = bflb_efuse_read_mac_address_opt(candidate, mac, reload);
            if (ret == 0 && slot != NULL) {
                *slot = (uint8_t)candidate + 1;
            }
            break;
        }
    }
    bflb_irq_restore(flags);
    return ret;
}

int rfparam_bl616cl_flash_otp_read_mac(uint8_t mac[6], uint8_t reload)
{
    return rfparam_bl616cl_flash_otp_read_mac_slot(mac, reload, NULL);
}
