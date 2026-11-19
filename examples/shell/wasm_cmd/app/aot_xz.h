#ifndef WASM_CMD_AOT_XZ_H
#define WASM_CMD_AOT_XZ_H

#include <stdbool.h>
#include <stdint.h>

#include "lfs.h"

/*
 * WAMR AOT ELF e_type values, see
 * components/wamr/upstream/core/iwasm/aot/aot_loader.c:
 *   E_TYPE_REL = 1 : relocatable AOT, the runtime copies and relocates its
 *                    text section into executable OCRAM before running it.
 *   E_TYPE_XIP = 4 : execution-in-place AOT (wamrc --xip or
 *                    --enable-indirect-mode), the runtime runs the text
 *                    section from the buffer itself.
 */
#define AOT_IMAGE_TYPE_REL 1U
#define AOT_IMAGE_TYPE_XIP 4U

/* Same limit as the downloader, also used to bound XZ index parsing. */
#define AOT_IMAGE_MAX_SIZE (128U * 1024U)

/* Enough to inspect an AOT target-info section (e_type at offset 20). */
#define AOT_IMAGE_HEADER_MAX 64U
#define AOT_XZ_MAGIC_SIZE    6U

/*
 * Maximum LZMA2 dictionary accepted by this app.
 *
 * Recommended host command:
 *   xz --check=crc32 --lzma2=preset=9,dict=8KiB -k foo.aot
 *
 * The decoder workspace is AOT_XZ_DICT_MAX + 32 KiB. Raising the dictionary
 * to 32 KiB gives a slightly smaller file but needs a 64 KiB workspace.
 * The XZ stream must use CRC32 (or no check); CRC64 is not compiled in.
 */
#ifndef AOT_XZ_DICT_MAX
#define AOT_XZ_DICT_MAX (8U * 1024U)
#endif

#ifndef AOT_XZ_WORKSPACE_SIZE
#define AOT_XZ_WORKSPACE_SIZE (AOT_XZ_DICT_MAX + 32U * 1024U)
#endif

/* LFS input chunk fed into the streaming decoder. */
#define AOT_XZ_IN_CHUNK (2U * 1024U)

typedef struct {
    /* Uncompressed AOT image size, taken from the XZ stream index. */
    uint32_t image_size;
    /* AOT_IMAGE_TYPE_REL or AOT_IMAGE_TYPE_XIP. */
    uint16_t image_type;
} aot_xz_info_t;

/* Check the six-byte XZ stream header magic. */
bool aot_xz_magic_matches(const uint8_t *head, uint32_t len);

/*
 * Parse the AOT header: magic, target-info section and e_type.
 * Returns 0 on success and stores AOT_IMAGE_TYPE_REL/XIP in image_type.
 */
int aot_image_parse_type(const uint8_t *buf, uint32_t len, uint16_t *image_type);

/*
 * Probe a compressed AOT file: uncompressed size from the XZ index and the
 * AOT image type from the beginning of the decompressed stream.
 */
int aot_xz_probe(lfs_t *lfs, const char *name, aot_xz_info_t *info);

/* Streaming decoder for one compressed AOT file (single instance). */
int aot_xz_open(lfs_t *lfs, const char *name);

/*
 * Read the next uncompressed chunk. Returns 0 on success; *got == 0 means the
 * stream has ended. Returns -1 on LittleFS/XZ errors.
 */
int aot_xz_read(uint8_t *buf, uint32_t cap, uint32_t *got);

void aot_xz_close(void);

/*
 * Decode a whole compressed AOT into dst. expected_size is the size reported
 * by aot_xz_probe(); trailing or missing bytes are reported as an error.
 */
int aot_xz_decode_to_buffer(lfs_t *lfs, const char *name, uint8_t *dst,
                            uint32_t expected_size, uint32_t *out_size);

#endif
