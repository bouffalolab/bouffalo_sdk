#include "rfparam_bl618dg_efuse.h"

#include "bflb_ef_ctrl.h"
#include "bflb_irq.h"
#include <stddef.h>

#define RFPARAM_BL618DG_TRIM_SLOT_COUNT 3
#define RFPARAM_BL618DG_TRIM_VALUE_COUNT 8

/* Non-A0 layout, matching MFG mfg_rfcal_trim_cfg. All bits are relative
 * to the start of a field; 5G values skip the enable/parity bits. */
struct rfparam_trim_slot_cfg {
    uint8_t en_addr;
    uint8_t parity_addr;
    uint8_t value_addr[RFPARAM_BL618DG_TRIM_VALUE_COUNT];
};

struct rfparam_trim_cfg {
    uint8_t slot_count;
    uint8_t value_len;
    uint8_t value_count;
    struct rfparam_trim_slot_cfg slot[RFPARAM_BL618DG_TRIM_SLOT_COUNT];
};

struct rfparam_efuse_power_cfg {
    uint16_t base;
    uint8_t words;
    const struct rfparam_trim_cfg *trim;
};

static const struct rfparam_trim_cfg poweroffset_5g_cfg = {
    .slot_count = 3,
    .value_len = 5,
    .value_count = 8,
    .slot = {
        { .en_addr = 30, .parity_addr = 31,
          .value_addr = { 0, 5, 10, 15, 20, 25, 32, 37 } },
        { .en_addr = 62, .parity_addr = 63,
          .value_addr = { 42, 47, 52, 57, 64, 69, 74, 79 } },
        { .en_addr = 94, .parity_addr = 95,
          .value_addr = { 84, 89, 96, 101, 106, 111, 116, 121 } },
    },
};

static const struct rfparam_trim_cfg poweroffset_2g_cfg = {
    .slot_count = 3,
    .value_len = 5,
    .value_count = 3,
    .slot = {
        { .en_addr = 47, .parity_addr = 48,
          .value_addr = { 0, 5, 10 } },
        { .en_addr = 49, .parity_addr = 50,
          .value_addr = { 16, 21, 26 } },
        { .en_addr = 51, .parity_addr = 52,
          .value_addr = { 32, 37, 42 } },
    },
};

static const struct rfparam_trim_cfg poweroffset_bz_cfg = {
    .slot_count = 3,
    .value_len = 5,
    .value_count = 5,
    .slot = {
        { .en_addr = 25, .parity_addr = 26,
          .value_addr = { 0, 5, 10, 15, 20 } },
        { .en_addr = 57, .parity_addr = 58,
          .value_addr = { 32, 37, 42, 47, 52 } },
        { .en_addr = 89, .parity_addr = 90,
          .value_addr = { 64, 69, 74, 79, 84 } },
    },
};

static const struct rfparam_trim_cfg capcode_cfg = {
    .slot_count = 3,
    .value_len = 8,
    .value_count = 1,
    .slot = {
        { .en_addr = 30, .parity_addr = 31, .value_addr = { 22 } },
        { .en_addr = 20, .parity_addr = 21, .value_addr = { 12 } },
        { .en_addr = 10, .parity_addr = 11, .value_addr = { 2 } },
    },
};

static const struct rfparam_efuse_power_cfg power_fields[RFPARAM_DG_POWER_TYPE_MAX] = {
    [RFPARAM_DG_WF_5G_HP] = { .base = 0x120, .words = 4, .trim = &poweroffset_5g_cfg },
    [RFPARAM_DG_WF_5G_LP] = { .base = 0x130, .words = 4, .trim = &poweroffset_5g_cfg },
    [RFPARAM_DG_WF_2G_HP] = { .base = 0x140, .words = 2, .trim = &poweroffset_2g_cfg },
    [RFPARAM_DG_WF_2G_LP] = { .base = 0x148, .words = 2, .trim = &poweroffset_2g_cfg },
    [RFPARAM_DG_WFBZ_HP] = { .base = 0x150, .words = 3, .trim = &poweroffset_bz_cfg },
    [RFPARAM_DG_WFBZ_LP] = { .base = 0x15c, .words = 3, .trim = &poweroffset_bz_cfg },
    [RFPARAM_DG_BZ_HP] = { .base = 0x168, .words = 3, .trim = &poweroffset_bz_cfg },
    [RFPARAM_DG_BZ_LP] = { .base = 0x174, .words = 3, .trim = &poweroffset_bz_cfg },
};

static uint32_t power_cache[RFPARAM_DG_POWER_TYPE_MAX][4];
static uint8_t power_cached[RFPARAM_DG_POWER_TYPE_MAX];
static uint32_t capcode_cache;
static uint8_t capcode_cached;

static uint32_t efuse_get_bits(const uint32_t *words, uint8_t bit, uint8_t len)
{
    uint32_t value = 0;

    for (uint8_t i = 0; i < len; i++) {
        value |= ((words[(bit + i) / 32] >> ((bit + i) % 32)) & 1U) << i;
    }

    return value;
}

int rfparam_bl618dg_decode_poweroffset(rfparam_bl618dg_power_type_t type,
                                     const uint32_t *words, int8_t *value,
                                     uint8_t *slot)
{
    int ret = RFPARAM_DG_EMPTY;

    if ((unsigned int)type >= RFPARAM_DG_POWER_TYPE_MAX ||
        words == NULL || value == NULL) {
        return RFPARAM_DG_INVALID;
    }
    const struct rfparam_trim_cfg *cfg = power_fields[type].trim;

    for (int s = cfg->slot_count - 1; s >= 0; s--) {
        const struct rfparam_trim_slot_cfg *trim_slot = &cfg->slot[s];
        uint8_t parity = 0;
        int8_t decoded[RFPARAM_BL618DG_TRIM_VALUE_COUNT];

        if (!efuse_get_bits(words, trim_slot->en_addr, 1)) {
            continue;
        }
        for (uint8_t i = 0; i < cfg->value_count; i++) {
            uint32_t raw = efuse_get_bits(words, trim_slot->value_addr[i], cfg->value_len);
            parity ^= bflb_ef_ctrl_get_trim_parity(raw, cfg->value_len);
            decoded[i] = raw >= 16 ? (int)raw - 32 : raw;
        }
        if (parity != efuse_get_bits(words, trim_slot->parity_addr, 1)) {
            ret = RFPARAM_DG_PARITY;
            continue;
        }
        for (uint8_t i = 0; i < cfg->value_count; i++) {
            value[i] = decoded[i];
        }
        if (slot != NULL) {
            *slot = s;
        }
        return RFPARAM_DG_OK;
    }
    return ret;
}

int rfparam_bl618dg_efuse_read_poweroffset(rfparam_bl618dg_power_type_t type,
                                         int8_t *value, uint8_t reload,
                                         uint8_t *slot)
{
    if ((unsigned int)type >= RFPARAM_DG_POWER_TYPE_MAX || value == NULL) {
        return RFPARAM_DG_INVALID;
    }
    uintptr_t flags = bflb_irq_save();
    if (reload || !power_cached[type]) {
        /* Always load physical eFuse, not unsaved controller shadow data. */
        bflb_ef_ctrl_read_direct(NULL, power_fields[type].base,
                                 power_cache[type], power_fields[type].words, 1);
        power_cached[type] = 1;
    }
    int ret = rfparam_bl618dg_decode_poweroffset(type, power_cache[type], value, slot);
    bflb_irq_restore(flags);
    return ret;
}

int rfparam_bl618dg_efuse_read_capcode(uint8_t *value, uint8_t reload,
                                     uint8_t *slot)
{
    int ret = RFPARAM_DG_EMPTY;

    if (value == NULL) {
        return RFPARAM_DG_INVALID;
    }
    uintptr_t flags = bflb_irq_save();
    if (reload || !capcode_cached) {
        bflb_ef_ctrl_read_direct(NULL, 0x1f8, &capcode_cache, 1, 1);
        capcode_cached = 1;
    }
    for (int s = capcode_cfg.slot_count - 1; s >= 0; s--) {
        const struct rfparam_trim_slot_cfg *trim_slot = &capcode_cfg.slot[s];
        uint8_t raw = efuse_get_bits(&capcode_cache, trim_slot->value_addr[0],
                                     capcode_cfg.value_len);

        if (!efuse_get_bits(&capcode_cache, trim_slot->en_addr, 1)) {
            continue;
        }
        if (efuse_get_bits(&capcode_cache, trim_slot->parity_addr, 1) !=
            bflb_ef_ctrl_get_trim_parity(raw, capcode_cfg.value_len)) {
            ret = RFPARAM_DG_PARITY;
            continue;
        }
        *value = raw;
        if (slot != NULL) {
            *slot = s;
        }
        ret = RFPARAM_DG_OK;
        break;
    }
    bflb_irq_restore(flags);
    return ret;
}
