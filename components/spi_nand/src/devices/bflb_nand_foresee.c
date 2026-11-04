/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bflb_nand_internal.h"

/* F35SQA512M datasheet Rev. 1.2, sections 9, 10.3 and 11.2.
 * The 64-byte spare area is available to the host; ECC parity is hidden.
 * Timeouts include margin over the read/program/erase maxima in table 17.
 */
const struct nand_chip nand_f35sqa512m = {
    .manufacturer = 0xcd,
    .device = 0x7070,
    .page_shift = 11,
    .block_shift = 6,
    .blocks = 512,
    .spare_bytes = 64,
    /* Enable ECC, leave OTP access and select SIO; B0h bit 3 is reserved. */
    .configuration_mask = 0x51,
    .configuration_value = 0x10,
    .protection_mask = 0x7c,
    /* Both ECCS=10 and ECCS=11 indicate an uncorrectable read. */
    .ecc_error_mask = 0x20,
    .ecc_error_value = 0x20,
    /* A factory mark may appear only on the second page. */
    .bad_marker_pages = 2,
    .bad_marker_bytes = 1,
    .read_timeout_us = 1000,
    .program_timeout_us = 5000,
    .erase_timeout_us = 50000,
};
