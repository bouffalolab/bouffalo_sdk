/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bflb_nand_internal.h"

int nand_bus_command(struct nand_phy *phy, uint8_t opcode, uint32_t address, unsigned address_bytes,
                     unsigned dummy_bytes, const void *tx, void *rx, size_t length)
{
    if (phy->io_error) {
        return phy->io_error;
    }

    if (address_bytes > 3 || dummy_bytes > 1 || (tx && rx) || (length && !tx && !rx)) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    uint8_t header[5] = { opcode };

    for (unsigned i = 0; i < address_bytes; ++i) {
        header[1 + i] = (uint8_t)(address >> (8 * (address_bytes - i - 1)));
    }

    bflb_spi_nand_bus_t *bus = &phy->bus;

    if (bus->lock) {
        bus->lock(bus->ctx);
    }

    bus->chip_select(bus->ctx, true);
    int failed = bus->transfer(bus->ctx, header, NULL, 1 + address_bytes + dummy_bytes);
    if (!failed && length) {
        failed = bus->transfer(bus->ctx, tx, rx, length);
    }
    bus->chip_select(bus->ctx, false);

    if (bus->unlock) {
        bus->unlock(bus->ctx);
    }

    if (failed) {
        phy->io_error = BFLB_NAND_ERR_IO;
    }

    return phy->io_error;
}
