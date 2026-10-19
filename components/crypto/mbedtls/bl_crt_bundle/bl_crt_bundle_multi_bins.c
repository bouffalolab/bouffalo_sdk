/**
 * @file bl_crt_bundle_multi_bins.c
 * @brief X.509 certificate bundle multi_bins descriptor parsing
 *
 * This module parses the multi_bins descriptor table to locate the
 * certificate bundle embedded in the firmware image.
 */

#include <stdint.h>
#include <string.h>
#include "bl_crt_bundle.h"
#include "bflb_flash.h"
#include "multi_bins.h"

/**
 * @brief Get certificate bundle location from multi_bins descriptor
 *
 * Parses the multi_bins descriptor table to find the CERTS entry.
 * Returns the start and end addresses in XIP-mapped space.
 *
 * @param start_addr [out] Pointer to store bundle start address (XIP)
 * @param end_addr   [out] Pointer to store bundle end address (XIP)
 * @return 0 on success, -1 if CERTS descriptor not found
 */
int bl_crt_bundle_get_addr(const uint8_t **start_addr, const uint8_t **end_addr)
{
    if (!start_addr || !end_addr) {
        return -1;
    }

    *start_addr = multi_bins_get_start("CERTS");
    *end_addr = multi_bins_get_end("CERTS");
    return (*start_addr != NULL && *end_addr != NULL) ? 0 : -1;
}
