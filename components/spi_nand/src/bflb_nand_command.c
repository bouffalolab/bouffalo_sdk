/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 *
 * Common SIO commands; device profiles describe configuration and ECC differences.
 */
#include <string.h>
#include "bflb_nand_internal.h"

static const struct nand_chip *const supported_chips[] = {
    &nand_w25n01kv,
    &nand_f35sqa512m,
};

static int feature_read(struct nand_phy *p, uint8_t reg, uint8_t *value)
{
    return nand_bus_command(p, 0x0f, reg, 1, 0, NULL, value, 1);
}

static int wait_ready(struct nand_phy *p, uint32_t timeout, uint8_t *status)
{
    uint64_t start = nand_time_us();

    for (;;) {
        int result = feature_read(p, 0xc0, status);

        if (result) {
            return result;
        }
        if (!(*status & 1)) {
            return BFLB_NAND_OK;
        }
        if (nand_time_us() - start >= timeout) {
            p->io_error = BFLB_NAND_ERR_TIMEOUT;
            return p->io_error;
        }
        nand_delay_us(10);
    }
}

static int feature_update(struct nand_phy *p, uint8_t reg, uint8_t mask, uint8_t bits)
{
    uint8_t value;
    uint8_t actual;
    uint8_t status;
    int result = feature_read(p, reg, &value);

    if (result) {
        return result;
    }

    value = (value & (uint8_t)~mask) | bits;
    result = nand_bus_command(p, 0x1f, reg, 1, 0, &value, NULL, 1);
    if (!result) {
        result = wait_ready(p, 1000, &status);
    }
    if (!result) {
        result = feature_read(p, reg, &actual);
    }
    if (!result && (actual & mask) != (value & mask)) {
        result = BFLB_NAND_ERR_INVALID_RESPONSE;
    }

    return result;
}

static int write_enable(struct nand_phy *p)
{
    uint8_t status;
    int result = nand_bus_command(p, 0x06, 0, 0, 0, NULL, NULL, 0);

    if (!result) {
        result = feature_read(p, 0xc0, &status);
    }
    if (!result && !(status & 2)) {
        p->io_error = BFLB_NAND_ERR_IO;
        result = p->io_error;
    }

    return result;
}

int nand_phy_open(struct nand_phy *p)
{
    uint8_t status;
    uint8_t id[3];
    int result;

    /* F35SQA512M requires 1 ms after power-up before full command access. */
    nand_delay_us(1000);
    result = nand_bus_command(p, 0xff, 0, 0, 0, NULL, NULL, 0);

    if (result) {
        return result;
    }

    nand_delay_us(1000); /* Includes reset with page-zero reload. */
    result = wait_ready(p, 10000, &status);
    if (!result) {
        result = nand_bus_command(p, 0x9f, 0, 0, 1, NULL, id, sizeof(id));
    }
    if (result) {
        return result;
    }

    const struct nand_chip *chip = NULL;
    uint16_t device_id = ((uint16_t)id[1] << 8) | id[2];

    for (size_t i = 0; i < sizeof(supported_chips) / sizeof(supported_chips[0]); i++) {
        if (id[0] == supported_chips[i]->manufacturer && device_id == supported_chips[i]->device) {
            chip = supported_chips[i];
            break;
        }
    }

    if (!chip) {
        return BFLB_NAND_ERR_INVALID_RESPONSE;
    }
    p->chip = chip;

    /* Apply the device's ECC/read mode before journal recovery, then unlock. */
    result = feature_update(p, 0xb0, chip->configuration_mask, chip->configuration_value);
    if (!result) {
        result = feature_update(p, 0xa0, chip->protection_mask, 0);
    }

    return result;
}

int nand_phy_read(struct nand_phy *p, uint32_t page, uint16_t column, void *data, size_t length, bool check_ecc)
{
    uint32_t bytes = 1U << p->chip->page_shift;

    if (page >= ((uint32_t)p->chip->blocks << p->chip->block_shift) || column > bytes + p->chip->spare_bytes ||
        length > bytes + p->chip->spare_bytes - column) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    uint8_t status;
    int result = nand_bus_command(p, 0x13, page, 3, 0, NULL, NULL, 0);

    if (!result) {
        result = wait_ready(p, p->chip->read_timeout_us, &status);
    }
    if (result) {
        return result;
    }

    if (check_ecc && (status & p->chip->ecc_error_mask) == p->chip->ecc_error_value) {
        return BFLB_NAND_ERR_ECC;
    }

    return nand_bus_command(p, 0x0b, column, 2, 1, NULL, data, length);
}

static int execute_program(struct nand_phy *p, uint32_t page)
{
    uint8_t status;
    int result = nand_bus_command(p, 0x10, page, 3, 0, NULL, NULL, 0);

    if (!result) {
        result = wait_ready(p, p->chip->program_timeout_us, &status);
    }
    if (!result && (status & 8)) {
        result = BFLB_NAND_ERR_BAD_BLOCK;
    }

    return result;
}

int nand_phy_program(struct nand_phy *p, uint32_t page, const uint8_t *data)
{
    uint32_t bytes = 1U << p->chip->page_shift;

    if (page >= ((uint32_t)p->chip->blocks << p->chip->block_shift)) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    int result = write_enable(p);

    /* 02h clears the entire cache to FF before loading data. 84h only adds
     * the two used-page bytes. There is exactly one program-execute per page.
     * Spare 0..1 stay FF to preserve the supported devices' bad-marker area;
     * spare 2..3 are the used marker.
     */
    const uint8_t used[2] = { 0, 0 };

    if (!result) {
        result = nand_bus_command(p, 0x02, 0, 2, 0, data, NULL, bytes);
    }
    if (!result) {
        result = nand_bus_command(p, 0x84, bytes + 2, 2, 0, used, NULL, sizeof(used));
    }
    if (!result) {
        result = execute_program(p, page);
    }

#if defined(CONFIG_NAND_FLASH_VERIFY_WRITE) && CONFIG_NAND_FLASH_VERIFY_WRITE
    uint8_t actual[64];

    for (uint32_t offset = 0; !result && offset < bytes; offset += sizeof(actual)) {
        result = nand_phy_read(p, page, offset, actual, sizeof(actual), true);
        if (!result && memcmp(actual, data + offset, sizeof(actual))) {
            result = BFLB_NAND_ERR_BAD_BLOCK;
        }
    }
#endif

    return result;
}

int nand_phy_erase(struct nand_phy *p, uint32_t block)
{
    if (block >= p->chip->blocks) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    uint8_t status;
    int result = write_enable(p);

    if (!result) {
        result = nand_bus_command(p, 0xd8, block << p->chip->block_shift, 3, 0, NULL, NULL, 0);
    }
    if (!result) {
        result = wait_ready(p, p->chip->erase_timeout_us, &status);
    }
    if (!result && (status & 4)) {
        result = BFLB_NAND_ERR_BAD_BLOCK;
    }

    return result;
}

int nand_phy_retire(struct nand_phy *p, uint32_t block)
{
    /* Dhara calls mark_bad after recovering the live data. Erase first to
     * avoid reprogramming page zero out of order. A failed block may reject
     * both erase and marker programming: persistence is necessarily best effort.
     */
    int result = nand_phy_erase(p, block);

    if (result && result != BFLB_NAND_ERR_BAD_BLOCK) {
        return result;
    }

    result = write_enable(p);
    const uint8_t marker[2] = { 0, 0 };

    if (!result) {
        result = nand_bus_command(p, 0x02, 1U << p->chip->page_shift, 2, 0, marker, NULL, p->chip->bad_marker_bytes);
    }
    if (!result) {
        result = execute_program(p, block << p->chip->block_shift);
    }

    return result;
}
