/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bflb_nand_internal.h"

/* W25N01KVxxxR datasheet Rev. H. Timeouts exceed specified maxima. */
const struct nand_chip nand_w25n01kv = {
    .manufacturer = 0xef,
    .device = 0xae21,
    .page_shift = 11,
    .block_shift = 6,
    .blocks = 1024,
    .spare_bytes = 96,
    .configuration_mask = 0x18,
    .configuration_value = 0x18,
    .protection_mask = 0x7e,
    .ecc_error_mask = 0x30,
    .ecc_error_value = 0x20,
    .bad_marker_pages = 1,
    .bad_marker_bytes = 2,
    .read_timeout_us = 1000,
    .program_timeout_us = 5000,
    .erase_timeout_us = 50000,
};
