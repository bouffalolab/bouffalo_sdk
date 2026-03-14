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

/*============================================================================
 * Multi_bins Descriptor Parsing
 *============================================================================*/

/* Multi_bins descriptor format (16 bytes) */
typedef struct {
    char name[8];         /* Descriptor name (e.g., "CERTS\0\0\0") */
    uint32_t start_addr;  /* Flash offset relative to firmware start */
    uint32_t end_addr;    /* End offset */
} __attribute__((packed)) multi_bin_desc_t;

/* Linker symbols for multi_bins array */
extern const uint8_t __multi_bins__[];

/* Firmware base address in XIP space */
#ifdef BL616
#define FW_HEADER_OFFSET  0x1000
#define XIP_BASE_ADDR     0xA0000000
#elif defined(BL602)
#define FW_HEADER_OFFSET  0x1000
#define XIP_BASE_ADDR     0x23000000
#endif

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
    const uint8_t *array_ptr = __multi_bins__;
    const multi_bin_desc_t *desc;

    if (!start_addr || !end_addr) {
        return -1;
    }

    /* Iterate through descriptors until terminator (16 bytes each) */
    for (int i = 0; i < 16; i++) {
        desc = (const multi_bin_desc_t*)(array_ptr + i * 16);

        /* Check for terminator (all 0xFFFFFFFF) */
        if (desc->start_addr == 0xFFFFFFFF) {
            break;
        }

        /* Check if this is CERTS descriptor (8-byte name) */
        if (memcmp(desc->name, "CERTS", 5) == 0) {
            /* Convert firmware-relative offset to XIP mapped address */
            /* Formula: (flash_offset - 0x1000) + 0xA0000000 */
            uint32_t bundle_flash_addr = desc->start_addr;
            uint32_t bundle_xip_addr = (bundle_flash_addr - FW_HEADER_OFFSET) + XIP_BASE_ADDR;

            *start_addr = (const uint8_t*)bundle_xip_addr;
            *end_addr = (const uint8_t*)(bundle_xip_addr + (desc->end_addr - desc->start_addr));

            return 0;  /* Found */
        }
    }

    /* CERTS descriptor not found */
    *start_addr = NULL;
    *end_addr = NULL;
    return -1;
}
