/*
 * XZ (LZMA2) support for AOT images stored in LittleFS.
 *
 * The SDK XZ decoder (components/lzma, CONFIG_XZ) works in multi-call mode
 * with a caller provided bump-allocator workspace, the same way boot2 uses
 * it. This file wraps it for streaming files out of LittleFS and for probing
 * the uncompressed AOT header before choosing an execution destination.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aot_xz.h"
#include "lfs.h"
#include "mm.h"
#include "xz.h"

#define AOT_XZ_FOOTER_SIZE  12U
#define AOT_XZ_INDEX_MAX    512U
#define AOT_XZ_STREAM_MAGIC "\xFD" "7zXZ\0"

static lfs_t *g_lfs;
static lfs_file_t g_file;
static bool g_file_open;

static struct xz_dec *g_dec;
static struct xz_buf g_buf;
static uint8_t g_in[AOT_XZ_IN_CHUNK];
static bool g_stream_end;

static uint8_t *g_workspace;

static uint16_t aot_xz_read16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t aot_xz_read32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16)
           | ((uint32_t)p[3] << 24);
}

static int aot_xz_error_message(enum xz_ret ret)
{
    switch (ret) {
        case XZ_MEM_ERROR:
            printf("AOT: XZ workspace too small for the stream\r\n");
            break;
        case XZ_MEMLIMIT_ERROR:
            printf("AOT: XZ dictionary too large, recompress with "
                   "--lzma2=dict=%uKiB or smaller\r\n",
                   (unsigned)(AOT_XZ_DICT_MAX / 1024U));
            break;
        case XZ_FORMAT_ERROR:
            printf("AOT: invalid XZ stream format\r\n");
            break;
        case XZ_OPTIONS_ERROR:
            printf("AOT: unsupported XZ options, use --check=crc32 and a "
                   "dictionary of at most %uKiB\r\n",
                   (unsigned)(AOT_XZ_DICT_MAX / 1024U));
            break;
        case XZ_DATA_ERROR:
            printf("AOT: corrupt XZ stream\r\n");
            break;
        case XZ_BUF_ERROR:
            printf("AOT: truncated XZ stream\r\n");
            break;
        default:
            printf("AOT: XZ decode failed (%d)\r\n", (int)ret);
            break;
    }
    return -1;
}

bool aot_xz_magic_matches(const uint8_t *head, uint32_t len)
{
    return len >= AOT_XZ_MAGIC_SIZE
           && memcmp(head, AOT_XZ_STREAM_MAGIC, AOT_XZ_MAGIC_SIZE) == 0;
}

int aot_image_parse_type(const uint8_t *buf, uint32_t len, uint16_t *image_type)
{
    uint32_t section_type;
    uint32_t section_size;

    if (!buf || !image_type || len < 22) {
        return -1;
    }
    /* AOT magic is 0x746f6100, stored little endian as "\0aot". */
    if (memcmp(buf, "\0" "aot", 4) != 0) {
        return -1;
    }
    section_type = aot_xz_read32(buf + 8);
    section_size = aot_xz_read32(buf + 12);
    /* The loader requires target_info (0) to be the first section. */
    if (section_type != 0 || section_size < 48) {
        return -1;
    }
    *image_type = aot_xz_read16(buf + 20);
    if (*image_type != AOT_IMAGE_TYPE_REL
        && *image_type != AOT_IMAGE_TYPE_XIP) {
        return -1;
    }
    return 0;
}

static bool aot_xz_vli(const uint8_t *p, uint32_t len, uint32_t *pos,
                       uint64_t *out)
{
    uint64_t value = 0;
    uint32_t i;

    for (i = 0; i < 9; i++) {
        uint8_t byte;

        if (*pos >= len) {
            return false;
        }
        byte = p[(*pos)++];
        value |= (uint64_t)(byte & 0x7FU) << (7U * i);
        if ((byte & 0x80U) == 0) {
            *out = value;
            return true;
        }
    }
    return false;
}

/*
 * Read the total uncompressed size from the XZ stream index. The file is a
 * single stream without stream padding, as produced by the xz command line
 * tool.
 */
static int aot_xz_index_size(lfs_t *lfs, lfs_file_t *file, uint32_t file_size,
                             uint32_t *image_size)
{
    uint8_t footer[AOT_XZ_FOOTER_SIZE];
    uint8_t index[AOT_XZ_INDEX_MAX];
    uint32_t backward_size;
    uint32_t index_size;
    uint32_t index_offset;
    uint32_t pos = 1;
    uint64_t records;
    uint64_t total = 0;
    uint64_t value;
    lfs_ssize_t read_len;
    uint64_t i;

    if (file_size < AOT_XZ_FOOTER_SIZE + 12U) {
        printf("AOT: XZ file too short\r\n");
        return -1;
    }
    if (lfs_file_seek(lfs, file, (lfs_soff_t)(file_size - AOT_XZ_FOOTER_SIZE),
                      LFS_SEEK_SET)
        < 0) {
        printf("AOT: LittleFS seek failed\r\n");
        return -1;
    }
    read_len = lfs_file_read(lfs, file, footer, sizeof(footer));
    if (read_len != (lfs_ssize_t)sizeof(footer)) {
        printf("AOT: cannot read XZ footer\r\n");
        return -1;
    }
    if (footer[10] != 'Y' || footer[11] != 'Z') {
        printf("AOT: unsupported XZ file, expected a single stream without "
               "padding\r\n");
        return -1;
    }
    backward_size = aot_xz_read32(footer + 4);
    index_size = (backward_size + 1U) * 4U;
    if (index_size == 0 || index_size > AOT_XZ_INDEX_MAX
        || index_size + AOT_XZ_FOOTER_SIZE > file_size) {
        printf("AOT: unsupported XZ index size\r\n");
        return -1;
    }
    index_offset = file_size - AOT_XZ_FOOTER_SIZE - index_size;
    if (lfs_file_seek(lfs, file, (lfs_soff_t)index_offset, LFS_SEEK_SET) < 0) {
        printf("AOT: LittleFS seek failed\r\n");
        return -1;
    }
    read_len = lfs_file_read(lfs, file, index, index_size);
    if (read_len != (lfs_ssize_t)index_size) {
        printf("AOT: cannot read XZ index\r\n");
        return -1;
    }
    if (index[0] != 0x00) {
        printf("AOT: invalid XZ index\r\n");
        return -1;
    }
    if (!aot_xz_vli(index, index_size, &pos, &records)) {
        printf("AOT: invalid XZ index records\r\n");
        return -1;
    }
    for (i = 0; i < records; i++) {
        if (!aot_xz_vli(index, index_size, &pos, &value)
            || !aot_xz_vli(index, index_size, &pos, &value)) {
            printf("AOT: invalid XZ index record\r\n");
            return -1;
        }
        total += value;
        if (total > AOT_IMAGE_MAX_SIZE) {
            printf("AOT: uncompressed image exceeds %u bytes\r\n",
                   (unsigned)AOT_IMAGE_MAX_SIZE);
            return -1;
        }
    }
    if (total == 0) {
        printf("AOT: empty XZ stream\r\n");
        return -1;
    }
    *image_size = (uint32_t)total;
    return 0;
}

static int aot_xz_decoder_open(void)
{
    g_workspace = kmalloc(AOT_XZ_WORKSPACE_SIZE,
                          MM_FLAG_HEAP_PSRAM_0 | MM_FLAG_ALIGN_32BYTE);
    if (!g_workspace) {
        g_workspace = kmalloc(AOT_XZ_WORKSPACE_SIZE,
                              MM_FLAG_HEAP_ANY | MM_FLAG_ALIGN_32BYTE);
    }
    if (!g_workspace) {
        printf("AOT: cannot allocate %u bytes of XZ workspace\r\n",
               (unsigned)AOT_XZ_WORKSPACE_SIZE);
        return -1;
    }
    simple_malloc_init(g_workspace, AOT_XZ_WORKSPACE_SIZE);
    xz_crc32_init();
    g_dec = xz_dec_init(XZ_DYNALLOC, AOT_XZ_DICT_MAX);
    if (!g_dec) {
        printf("AOT: XZ decoder init failed (workspace %u bytes)\r\n",
               (unsigned)AOT_XZ_WORKSPACE_SIZE);
        kfree(g_workspace);
        g_workspace = NULL;
        return -1;
    }
    memset(&g_buf, 0, sizeof(g_buf));
    g_stream_end = false;
    return 0;
}

static void aot_xz_decoder_close(void)
{
    if (g_dec) {
        xz_dec_end(g_dec);
        g_dec = NULL;
    }
    if (g_workspace) {
        kfree(g_workspace);
        g_workspace = NULL;
    }
}

int aot_xz_open(lfs_t *lfs, const char *name)
{
    int ret;

    if (g_dec || g_file_open) {
        return -1;
    }
    ret = lfs_file_open(lfs, &g_file, name, LFS_O_RDONLY);
    if (ret != LFS_ERR_OK) {
        printf("AOT: cannot open %s (%d)\r\n", name, ret);
        return -1;
    }
    g_lfs = lfs;
    g_file_open = true;
    if (aot_xz_decoder_open() != 0) {
        lfs_file_close(lfs, &g_file);
        g_file_open = false;
        return -1;
    }
    return 0;
}

int aot_xz_read(uint8_t *buf, uint32_t cap, uint32_t *got)
{
    if (!got) {
        return -1;
    }
    *got = 0;
    if (!g_dec || !g_file_open || !buf || cap == 0) {
        return -1;
    }
    if (g_stream_end) {
        return 0;
    }
    g_buf.out = buf;
    g_buf.out_pos = 0;
    g_buf.out_size = cap;
    for (;;) {
        enum xz_ret ret;

        if (g_buf.in_pos == g_buf.in_size) {
            lfs_ssize_t read_len = lfs_file_read(g_lfs, &g_file, g_in,
                                                 sizeof(g_in));

            if (read_len < 0) {
                printf("AOT: LittleFS read failed (%ld)\r\n",
                       (long)read_len);
                return -1;
            }
            if (read_len == 0) {
                printf("AOT: truncated XZ stream\r\n");
                return -1;
            }
            g_buf.in = g_in;
            g_buf.in_pos = 0;
            g_buf.in_size = (uint32_t)read_len;
        }
        ret = xz_dec_run(g_dec, &g_buf);
        if (ret == XZ_STREAM_END) {
            g_stream_end = true;
            *got = g_buf.out_pos;
            return 0;
        }
        if (ret != XZ_OK) {
            return aot_xz_error_message(ret);
        }
        if (g_buf.out_pos == cap) {
            *got = g_buf.out_pos;
            return 0;
        }
    }
}

void aot_xz_close(void)
{
    if (g_file_open) {
        lfs_file_close(g_lfs, &g_file);
        g_file_open = false;
    }
    aot_xz_decoder_close();
    g_lfs = NULL;
    g_stream_end = false;
    memset(&g_buf, 0, sizeof(g_buf));
}

int aot_xz_probe(lfs_t *lfs, const char *name, aot_xz_info_t *info)
{
    lfs_file_t file;
    lfs_ssize_t file_size;
    lfs_ssize_t read_len;
    uint8_t head[AOT_IMAGE_HEADER_MAX];
    uint32_t image_size = 0;
    uint32_t header_len = 0;
    uint16_t image_type = 0;
    int ret = -1;

    if (!lfs || !name || !info) {
        return -1;
    }
    memset(info, 0, sizeof(*info));
    if (lfs_file_open(lfs, &file, name, LFS_O_RDONLY) != LFS_ERR_OK) {
        printf("AOT: cannot open %s\r\n", name);
        return -1;
    }
    file_size = lfs_file_size(lfs, &file);
    if (file_size <= 0 || (uint32_t)file_size > AOT_IMAGE_MAX_SIZE) {
        printf("AOT: invalid compressed size: %ld\r\n", (long)file_size);
        goto out_file;
    }
    read_len = lfs_file_read(lfs, &file, head, AOT_XZ_MAGIC_SIZE);
    if (read_len < (lfs_ssize_t)AOT_XZ_MAGIC_SIZE
        || !aot_xz_magic_matches(head, (uint32_t)read_len)) {
        printf("AOT: %s is not an XZ stream\r\n", name);
        goto out_file;
    }
    if (aot_xz_index_size(lfs, &file, (uint32_t)file_size, &image_size) != 0) {
        goto out_file;
    }
    lfs_file_close(lfs, &file);

    if (aot_xz_open(lfs, name) != 0) {
        return -1;
    }
    if (aot_xz_read(head, sizeof(head), &header_len) != 0) {
        goto out_decoder;
    }
    if (header_len < 22) {
        printf("AOT: decompressed header too short\r\n");
        goto out_decoder;
    }
    if (aot_image_parse_type(head, header_len, &image_type) != 0) {
        printf("AOT: %s does not contain a supported AOT image\r\n", name);
        goto out_decoder;
    }
    info->image_size = image_size;
    info->image_type = image_type;
    ret = 0;

out_decoder:
    aot_xz_close();
    return ret;
out_file:
    lfs_file_close(lfs, &file);
    return ret;
}

int aot_xz_decode_to_buffer(lfs_t *lfs, const char *name, uint8_t *dst,
                            uint32_t expected_size, uint32_t *out_size)
{
    uint32_t pos = 0;
    uint8_t dummy;

    if (!dst || !out_size || expected_size == 0) {
        return -1;
    }
    *out_size = 0;
    if (aot_xz_open(lfs, name) != 0) {
        return -1;
    }
    while (pos < expected_size) {
        uint32_t got = 0;

        if (aot_xz_read(dst + pos, expected_size - pos, &got) != 0) {
            goto fail;
        }
        if (got == 0) {
            printf("AOT: XZ stream shorter than expected\r\n");
            goto fail;
        }
        pos += got;
    }
    {
        uint32_t got = 0;

        if (aot_xz_read(&dummy, sizeof(dummy), &got) != 0) {
            goto fail;
        }
        if (got != 0) {
            printf("AOT: XZ stream longer than expected\r\n");
            goto fail;
        }
    }
    aot_xz_close();
    *out_size = pos;
    return 0;

fail:
    aot_xz_close();
    return -1;
}
