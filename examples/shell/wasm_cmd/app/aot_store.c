#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aot_store.h"
#include "aot_xz.h"
#include "bflb_l1c.h"
#include "bflb_mtd.h"
#include "bflb_sf_ctrl.h"
#include "https_client.h"
#include "lfs.h"
#include "lfs_port.h"
#include "mm.h"
#include "shell.h"
#include "utils_crc.h"
#include "wasm_export.h"
#if defined(CONFIG_WL80211)
#include "wifi_mgmr.h"

extern int aot_wl80211_connect(const char *ssid, const char *password,
                               const char *bssid, uint8_t channel);
#endif

#define AOT_LFS_PARTITION       "media"
#define AOT_SLOT_PARTITION      "AOT"
#define AOT_SLOT_MAGIC          0x314f5441U /* "AOT1" */
#define AOT_SLOT_HEADER_SIZE    0x100U
#define AOT_SLOT_DATA_OFFSET    AOT_SLOT_HEADER_SIZE
#define AOT_DOWNLOAD_MAX_SIZE   AOT_IMAGE_MAX_SIZE
#define AOT_IO_CHUNK            256U
#define AOT_WASM_STACK_SIZE     (8U * 1024U)

typedef struct {
    lfs_file_t *file;
    lfs_t *lfs;
    uint32_t written;
    uint16_t status_code;
    bool failed;
} aot_download_ctx_t;

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t size;
    uint32_t crc32;
    uint16_t version;
    uint16_t reserved; /* AOT image e_type: 1 = REL, 4 = XIP */
    char name[32];
} aot_slot_header_t;

/* Result of probing a .aot/.aot.xz file in LittleFS. */
typedef struct {
    bool compressed;
    uint32_t image_size;
    uint16_t image_type;
} aot_file_info_t;

static struct lfs_context g_lfs_ctx = { .partition_name = AOT_LFS_PARTITION };
static struct lfs_config g_lfs_cfg = {
    .read_size = 256,
    .prog_size = 256,
    .lookahead_size = 256,
    .cache_size = 512,
    .block_size = 4096,
    .block_cycles = 500,
};
static lfs_t *g_lfs;
static bool g_lfs_init_attempted;

static const char *aot_type_name(uint16_t image_type)
{
    if (image_type == AOT_IMAGE_TYPE_XIP) {
        return "XIP";
    }
    if (image_type == AOT_IMAGE_TYPE_REL) {
        return "REL";
    }
    return "unknown";
}

static bool aot_has_suffix(const char *name, const char *suffix)
{
    size_t name_len = strlen(name);
    size_t suffix_len = strlen(suffix);

    return name_len > suffix_len
           && strcmp(name + name_len - suffix_len, suffix) == 0;
}

/* Accept both the raw .aot files and XZ-compressed .aot.xz files. */
static bool aot_has_aot_suffix(const char *name)
{
    return aot_has_suffix(name, ".aot") || aot_has_suffix(name, ".aot.xz");
}

static bool aot_is_xz_name(const char *name)
{
    return aot_has_suffix(name, ".xz");
}

static bool aot_lfs_ready(void)
{
    if (!g_lfs && !g_lfs_init_attempted) {
        g_lfs_init_attempted = true;
        (void)aot_store_init();
    }
    if (!g_lfs)
        printf("AOT: LittleFS is not mounted\r\n");
    return g_lfs != NULL;
}

static bool aot_name_valid(const char *name)
{
    size_t i, len;

    if (!name || !name[0])
        return false;
    len = strlen(name);
    if (len >= LFS_NAME_MAX || !strcmp(name, ".") || !strcmp(name, ".."))
        return false;
    for (i = 0; i < len; i++) {
        char c = name[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
              || (c >= '0' && c <= '9') || c == '_' || c == '-'
              || c == '.'))
            return false;
    }
    return true;
}

static int aot_name_from_url(const char *url, char *name, size_t name_size)
{
    const char *start, *end;
    size_t len;

    if (!url || !name || name_size == 0)
        return -1;
    start = strrchr(url, '/');
    if (!start || !start[1])
        return -1;
    start++;
    end = start;
    while (*end && *end != '?' && *end != '#')
        end++;
    len = (size_t)(end - start);
    if (len == 0 || len + 1 > name_size)
        return -1;
    memcpy(name, start, len);
    name[len] = '\0';
    if (!aot_has_aot_suffix(name)) {
        /* Default to a compressed image when the URL has no AOT suffix. */
        if (aot_has_suffix(name, ".xz"))
            return -1;
        if (len + 8 > name_size)
            return -1;
        memcpy(name + len, ".aot.xz", 8);
    }
    return aot_name_valid(name) ? 0 : -1;
}

/*
 * Load one AOT image and call its exported entry.
 *
 * The image buffer must stay alive until the module is unloaded: with the
 * plain wasm_runtime_load() path the AOT loader keeps some module fields
 * (const strings, memory init data, ...) referencing the input buffer. For a
 * LittleFS image the caller therefore keeps the decompressed staging buffer
 * resident for the whole run and asks this helper to release it at the end
 * (free_image_at_end). Flash XIP images are permanent and pass false.
 */
static int aot_exec_image(uint8_t *image, uint32_t size, const char *entry,
                          const char *source, bool free_image_at_end)
{
    RuntimeInitArgs init_args;
    wasm_module_t module = NULL;
    wasm_module_inst_t instance = NULL;
    wasm_exec_env_t exec_env = NULL;
    wasm_function_inst_t func;
    uint32_t argv_wasm[1] = { 0 };
    char error[128] = { 0 };
    bool xip;
    int ret = -1;

    memset(&init_args, 0, sizeof(init_args));
    init_args.mem_alloc_type = Alloc_With_System_Allocator;
    if (!wasm_runtime_full_init(&init_args)) {
        printf("AOT: WAMR init failed\r\n");
        goto out;
    }
    xip = wasm_runtime_is_xip_file(image, size);
    printf("AOT: running %s from %s (%s), size=%lu\r\n", entry, source,
           xip ? "Flash XIP, not copied" : "copied to executable OCRAM",
           (unsigned long)size);

    module = wasm_runtime_load(image, size, error, sizeof(error));
    if (!module) {
        printf("AOT: load failed: %s\r\n", error);
        goto out_runtime;
    }
    instance = wasm_runtime_instantiate(module, AOT_WASM_STACK_SIZE, 0, error,
                                        sizeof(error));
    if (!instance) {
        printf("AOT: instantiate failed: %s\r\n", error);
        goto out_runtime;
    }
    func = wasm_runtime_lookup_function(instance, entry);
    if (!func) {
        printf("AOT: export not found: %s\r\n", entry);
        goto out_instance;
    }
    exec_env = wasm_runtime_create_exec_env(instance, AOT_WASM_STACK_SIZE);
    if (!exec_env) {
        printf("AOT: exec env failed\r\n");
        goto out_instance;
    }
    if (!wasm_runtime_call_wasm(exec_env, func, 0, argv_wasm)) {
        printf("AOT: call failed: %s\r\n",
               wasm_runtime_get_exception(instance));
        goto out_instance;
    }
    printf("AOT: %s returned %lu\r\n", entry, (unsigned long)argv_wasm[0]);
    ret = 0;

out_instance:
    if (exec_env)
        wasm_runtime_destroy_exec_env(exec_env);
    if (instance)
        wasm_runtime_deinstantiate(instance);
out_runtime:
    if (module)
        wasm_runtime_unload(module);
    wasm_runtime_destroy();
out:
    if (free_image_at_end)
        kfree(image);
    return ret;
}

/* Probe a LittleFS file: compression, AOT image size and image type. */
static int aot_file_probe(lfs_t *lfs, const char *name, aot_file_info_t *info)
{
    lfs_file_t file;
    lfs_ssize_t file_size;
    lfs_ssize_t read_len;
    uint8_t head[AOT_IMAGE_HEADER_MAX];
    uint32_t head_len = 0;
    aot_xz_info_t xz_info;

    if (!lfs || !name || !info) {
        return -1;
    }
    memset(info, 0, sizeof(*info));
    if (lfs_file_open(lfs, &file, name, LFS_O_RDONLY) != LFS_ERR_OK) {
        printf("AOT: cannot open %s\r\n", name);
        return -1;
    }
    file_size = lfs_file_size(lfs, &file);
    if (file_size <= 0 || (uint32_t)file_size > AOT_DOWNLOAD_MAX_SIZE) {
        printf("AOT: invalid file size: %ld\r\n", (long)file_size);
        lfs_file_close(lfs, &file);
        return -1;
    }
    read_len = lfs_file_read(lfs, &file, head, sizeof(head));
    lfs_file_close(lfs, &file);
    if (read_len <= 0) {
        printf("AOT: cannot read %s\r\n", name);
        return -1;
    }
    head_len = (uint32_t)read_len;

    if (aot_xz_magic_matches(head, head_len)) {
        info->compressed = true;
        if (aot_xz_probe(lfs, name, &xz_info) != 0) {
            return -1;
        }
        info->image_size = xz_info.image_size;
        info->image_type = xz_info.image_type;
    } else {
        info->compressed = false;
        info->image_size = (uint32_t)file_size;
        if (aot_image_parse_type(head, head_len, &info->image_type) != 0) {
            printf("AOT: %s is neither an XZ stream nor a supported AOT "
                   "image\r\n",
                   name);
            return -1;
        }
    }
    printf("AOT: %s: %s image, %lu bytes, type=%s\r\n", name,
           info->compressed ? "XZ-compressed" : "raw",
           (unsigned long)info->image_size,
           aot_type_name(info->image_type));
    return 0;
}

/* Copy a LittleFS file into an in-memory buffer. */
static int aot_read_file_to_buffer(lfs_t *lfs, const char *name, uint8_t *dst,
                                   uint32_t expected_size)
{
    lfs_file_t file;
    lfs_ssize_t read_len;
    uint32_t pos = 0;

    if (lfs_file_open(lfs, &file, name, LFS_O_RDONLY) != LFS_ERR_OK) {
        printf("AOT: cannot open %s\r\n", name);
        return -1;
    }
    while (pos < expected_size) {
        read_len = lfs_file_read(lfs, &file, dst + pos, expected_size - pos);
        if (read_len < 0) {
            printf("AOT: LittleFS read failed (%ld)\r\n", (long)read_len);
            lfs_file_close(lfs, &file);
            return -1;
        }
        if (read_len == 0) {
            printf("AOT: %s is shorter than expected\r\n", name);
            lfs_file_close(lfs, &file);
            return -1;
        }
        pos += (uint32_t)read_len;
    }
    if (lfs_file_close(lfs, &file) != LFS_ERR_OK) {
        printf("AOT: cannot close %s\r\n", name);
        return -1;
    }
    return 0;
}

/* Append data to a padded 256-byte flash write stream. */
static int aot_flash_stream_write(bflb_mtd_handle_t handle, uint32_t *pos,
                                  uint8_t *wbuf, uint32_t *wfill,
                                  const uint8_t *data, uint32_t len)
{
    while (len > 0) {
        uint32_t space = AOT_IO_CHUNK - *wfill;
        uint32_t take = len < space ? len : space;

        memcpy(wbuf + *wfill, data, take);
        *wfill += take;
        data += take;
        len -= take;
        if (*wfill == AOT_IO_CHUNK) {
            if (bflb_mtd_write(handle, *pos, AOT_IO_CHUNK, wbuf) != 0) {
                return -1;
            }
            *pos += AOT_IO_CHUNK;
            *wfill = 0;
        }
    }
    return 0;
}

static int aot_flash_stream_flush(bflb_mtd_handle_t handle, uint32_t *pos,
                                  uint8_t *wbuf, uint32_t *wfill)
{
    if (*wfill == 0) {
        return 0;
    }
    memset(wbuf + *wfill, 0xFF, AOT_IO_CHUNK - *wfill);
    if (bflb_mtd_write(handle, *pos, AOT_IO_CHUNK, wbuf) != 0) {
        return -1;
    }
    *pos += AOT_IO_CHUNK;
    *wfill = 0;
    return 0;
}

/*
 * Write an AOT image (raw or XZ-compressed) to the raw AOT slot, so that it
 * can be executed in place from Flash XIP. A slot header with the
 * uncompressed size, CRC and image type is written at offset 0.
 */
static int aot_slot_write_file(lfs_t *lfs, const char *name,
                               const aot_file_info_t *info)
{
    bflb_mtd_handle_t handle = NULL;
    bflb_mtd_info_t mtd_info;
    aot_slot_header_t header;
    uint8_t header_buf[AOT_SLOT_HEADER_SIZE];
    uint8_t chunk[AOT_IO_CHUNK];
    uint8_t wbuf[AOT_IO_CHUNK];
    struct crc32_stream_ctx crc;
    uint32_t written = 0;
    uint32_t pos = AOT_SLOT_DATA_OFFSET;
    uint32_t wfill = 0;
    uint32_t erased_size;
    int ret = -1;

    if (bflb_mtd_open(AOT_SLOT_PARTITION, &handle, 0) != 0) {
        printf("AOT: cannot open %s partition\r\n", AOT_SLOT_PARTITION);
        return -1;
    }
    bflb_mtd_info(handle, &mtd_info);
    if (info->image_size > (uint32_t)(mtd_info.size - AOT_SLOT_DATA_OFFSET)) {
        printf("AOT: image exceeds slot size (%lu > %lu)\r\n",
               (unsigned long)info->image_size,
               (unsigned long)(mtd_info.size - AOT_SLOT_DATA_OFFSET));
        goto out;
    }
    erased_size = (AOT_SLOT_DATA_OFFSET + info->image_size + 4095U) & ~4095U;
    if (bflb_mtd_erase(handle, 0, erased_size) != 0) {
        printf("AOT: slot erase failed\r\n");
        goto out;
    }

    utils_crc32_stream_init(&crc);
    if (info->compressed) {
        if (aot_xz_open(lfs, name) != 0) {
            goto out;
        }
        for (;;) {
            uint32_t got = 0;

            if (aot_xz_read(chunk, sizeof(chunk), &got) != 0) {
                aot_xz_close();
                goto out;
            }
            if (got == 0) {
                break;
            }
            written += got;
            if (written > info->image_size) {
                printf("AOT: image larger than expected\r\n");
                aot_xz_close();
                goto out;
            }
            utils_crc32_stream_feed_block(&crc, chunk, got);
            if (aot_flash_stream_write(handle, &pos, wbuf, &wfill, chunk, got)
                != 0) {
                printf("AOT: slot write failed\r\n");
                aot_xz_close();
                goto out;
            }
        }
        aot_xz_close();
    } else {
        lfs_file_t file;
        lfs_ssize_t read_len;

        if (lfs_file_open(lfs, &file, name, LFS_O_RDONLY) != LFS_ERR_OK) {
            printf("AOT: cannot open %s\r\n", name);
            goto out;
        }
        while ((read_len = lfs_file_read(lfs, &file, chunk, sizeof(chunk)))
               > 0) {
            written += (uint32_t)read_len;
            if (written > info->image_size) {
                printf("AOT: image larger than expected\r\n");
                lfs_file_close(lfs, &file);
                goto out;
            }
            utils_crc32_stream_feed_block(&crc, chunk, (uint32_t)read_len);
            if (aot_flash_stream_write(handle, &pos, wbuf, &wfill, chunk,
                                       (uint32_t)read_len)
                != 0) {
                printf("AOT: slot write failed\r\n");
                lfs_file_close(lfs, &file);
                goto out;
            }
        }
        if (read_len < 0) {
            printf("AOT: LittleFS read failed (%ld)\r\n", (long)read_len);
            lfs_file_close(lfs, &file);
            goto out;
        }
        lfs_file_close(lfs, &file);
    }
    if (written != info->image_size) {
        printf("AOT: image size mismatch (%lu != %lu)\r\n",
               (unsigned long)written, (unsigned long)info->image_size);
        goto out;
    }
    if (aot_flash_stream_flush(handle, &pos, wbuf, &wfill) != 0) {
        printf("AOT: slot write failed\r\n");
        goto out;
    }

    memset(header_buf, 0xFF, sizeof(header_buf));
    memset(&header, 0, sizeof(header));
    header.magic = AOT_SLOT_MAGIC;
    header.size = info->image_size;
    header.crc32 = utils_crc32_stream_results(&crc);
    header.version = 1;
    header.reserved = info->image_type;
    strlcpy(header.name, name, sizeof(header.name));
    memcpy(header_buf, &header, sizeof(header));
    if (bflb_mtd_write(handle, 0, sizeof(header_buf), header_buf) != 0) {
        printf("AOT: header write failed\r\n");
        goto out;
    }
    bflb_l1c_dcache_clean_invalidate_all();
    bflb_l1c_icache_invalid_all();
    printf("AOT: installed %s into slot: image=%lu crc=0x%08lx type=%s\r\n",
           name, (unsigned long)header.size, (unsigned long)header.crc32,
           aot_type_name(header.reserved));
    ret = 0;

out:
    bflb_mtd_close(handle);
    return ret;
}

/* Resolve the raw slot to an executable Flash XIP address and run it. */
static int aot_run_slot(const char *entry)
{
    bflb_mtd_handle_t handle = NULL;
    bflb_mtd_handle_t bus_handle = NULL;
    bflb_mtd_info_t info;
    bflb_mtd_info_t bus_info;
    aot_slot_header_t header;
    struct crc32_stream_ctx crc;
    uint8_t buf[256];
    uint32_t pos = 0, n;
    const uint8_t *aot = NULL;
    const uint8_t *linear_aot;
    const uint8_t *bus_aot = NULL;
    int ret = -1;

    if (bflb_mtd_open(AOT_SLOT_PARTITION, &handle, 0) != 0) {
        printf("AOT: cannot open slot partition\r\n");
        return -1;
    }
    bflb_mtd_info(handle, &info);
    if (bflb_mtd_read(handle, 0, sizeof(header), (uint8_t *)&header) != 0
        || header.magic != AOT_SLOT_MAGIC || header.size == 0
        || header.size > info.size - AOT_SLOT_DATA_OFFSET) {
        printf("AOT: slot is empty or invalid\r\n");
        goto out;
    }
    utils_crc32_stream_init(&crc);
    while (pos < header.size) {
        n = header.size - pos;
        if (n > sizeof(buf))
            n = sizeof(buf);
        if (bflb_mtd_read(handle, AOT_SLOT_DATA_OFFSET + pos, n, buf) != 0) {
            printf("AOT: slot read failed\r\n");
            goto out;
        }
        utils_crc32_stream_feed_block(&crc, buf, n);
        pos += n;
    }
    if (utils_crc32_stream_results(&crc) != header.crc32) {
        printf("AOT: slot CRC mismatch\r\n");
        goto out;
    }

    bflb_l1c_dcache_clean_invalidate_all();
    bflb_l1c_icache_invalid_all();
    linear_aot = (const uint8_t *)(BFLB_FLASH_XIP_BASE + info.offset
                                   + AOT_SLOT_DATA_OFFSET);
    if (bflb_mtd_open(AOT_SLOT_PARTITION, &bus_handle,
                      BFLB_MTD_OPEN_FLAG_BUSADDR)
        == 0) {
        bflb_mtd_info(bus_handle, &bus_info);
        if (bus_info.xip_addr)
            bus_aot = (const uint8_t *)bus_info.xip_addr
                      + AOT_SLOT_DATA_OFFSET;
        bflb_mtd_close(bus_handle);
    }

    if (linear_aot && memcmp(linear_aot, "\0aot", 4) == 0)
        aot = linear_aot;
    else if (bus_aot && memcmp(bus_aot, "\0aot", 4) == 0)
        aot = bus_aot;
    if (!aot) {
        printf("AOT: no valid AOT XIP mapping found\r\n");
        goto out;
    }
    ret = aot_exec_image((uint8_t *)aot, header.size, entry, "AOT slot",
                         false);
out:
    bflb_mtd_close(handle);
    return ret;
}

static void aot_download_response(struct http_response *rsp,
                                  enum http_final_call final_data,
                                  void *user_data)
{
    aot_download_ctx_t *ctx = (aot_download_ctx_t *)user_data;
    lfs_ssize_t written;

    (void)final_data;
    if (rsp->http_status_code) {
        ctx->status_code = rsp->http_status_code;
        if (rsp->http_status_code != 200)
            ctx->failed = true;
    }
    if (ctx->failed || rsp->body_frag_len == 0)
        return;
    if (ctx->written + rsp->body_frag_len > AOT_DOWNLOAD_MAX_SIZE) {
        printf("AOT: download exceeds %u bytes\r\n", AOT_DOWNLOAD_MAX_SIZE);
        ctx->failed = true;
        return;
    }
    written = lfs_file_write(ctx->lfs, ctx->file, rsp->body_frag_start,
                             rsp->body_frag_len);
    if (written != (lfs_ssize_t)rsp->body_frag_len) {
        printf("AOT: LittleFS write failed (%ld/%lu)\r\n", (long)written,
               (unsigned long)rsp->body_frag_len);
        ctx->failed = true;
        return;
    }
    ctx->written += (uint32_t)written;
}

static int aot_download_cancel(void *user_data)
{
    return ((aot_download_ctx_t *)user_data)->failed;
}

static int aot_list(int argc, char **argv)
{
    lfs_dir_t dir;
    struct lfs_info info;
    int ret;

    (void)argc;
    (void)argv;
    if (!aot_lfs_ready())
        return -1;
    ret = lfs_dir_open(g_lfs, &dir, "/");
    if (ret != LFS_ERR_OK) {
        printf("AOT: cannot open LittleFS root: %d\r\n", ret);
        return -1;
    }
    printf("AOT files:\r\n");
    while ((ret = lfs_dir_read(g_lfs, &dir, &info)) > 0) {
        if (info.type == LFS_TYPE_REG && aot_has_aot_suffix(info.name))
            printf("  %-32s %lu bytes%s\r\n", info.name,
                   (unsigned long)info.size,
                   aot_is_xz_name(info.name) ? " (xz)" : "");
    }
    lfs_dir_close(g_lfs, &dir);
    return ret < 0 ? -1 : 0;
}

static int aot_download(int argc, char **argv)
{
    struct https_client_request req = { 0 };
    lfs_file_t file;
    aot_download_ctx_t ctx = { 0 };
    char name[LFS_NAME_MAX];
    uint8_t head[AOT_XZ_MAGIC_SIZE];
    lfs_ssize_t read_len;
    int ret;
    const char *url;
    bool is_xz;

    if (argc < 2 || argc > 3) {
        printf("Usage: aot_download <http(s)-url> [file.aot|file.aot.xz]\r\n");
        return -1;
    }
    if (!aot_lfs_ready())
        return -1;
    url = argv[1];
    if (strncmp(url, "http://", 7) != 0
        && strncmp(url, "https://", 8) != 0) {
        printf("AOT: only http:// and https:// URLs are supported\r\n");
        return -1;
    }
    if (argc == 3) {
        if (!aot_name_valid(argv[2]) || !aot_has_aot_suffix(argv[2])) {
            printf("AOT: invalid .aot/.aot.xz filename\r\n");
            return -1;
        }
        strlcpy(name, argv[2], sizeof(name));
    } else if (aot_name_from_url(url, name, sizeof(name)) != 0) {
        printf("AOT: cannot derive filename, pass it explicitly\r\n");
        return -1;
    }

    ret = lfs_file_open(g_lfs, &file, name,
                        LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (ret != LFS_ERR_OK) {
        printf("AOT: cannot create %s: %d\r\n", name, ret);
        return -1;
    }
    req.method = HTTP_GET;
    req.url = url;
    req.protocol = "HTTP/1.1";
    req.response = aot_download_response;
    req.cancel = aot_download_cancel;
    req.buffer_size = 4096;
    ctx.file = &file;
    ctx.lfs = g_lfs;
    ret = https_client_request(&req, 30000, &ctx);
    if (ret < 0)
        printf("AOT: HTTP download failed: %d\r\n", ret);
    if (ctx.failed || ctx.written == 0
        || (ctx.status_code && ctx.status_code != 200))
        ret = -1;
    if (lfs_file_close(g_lfs, &file) != LFS_ERR_OK)
        ret = -1;
    if (ret < 0) {
        lfs_remove(g_lfs, name);
        return -1;
    }

    /* Check that the payload really is an XZ stream or a raw AOT image. */
    ret = lfs_file_open(g_lfs, &file, name, LFS_O_RDONLY);
    if (ret != LFS_ERR_OK) {
        printf("AOT: cannot reopen %s: %d\r\n", name, ret);
        lfs_remove(g_lfs, name);
        return -1;
    }
    read_len = lfs_file_read(g_lfs, &file, head, sizeof(head));
    lfs_file_close(g_lfs, &file);
    is_xz = aot_xz_magic_matches(head, (uint32_t)(read_len > 0 ? read_len : 0));
    if (!is_xz && (read_len < 4 || memcmp(head, "\0" "aot", 4) != 0)) {
        printf("AOT: downloaded data is neither XZ nor AOT, removing %s\r\n",
               name);
        lfs_remove(g_lfs, name);
        return -1;
    }
    if (aot_is_xz_name(name) && !is_xz) {
        printf("AOT: %s is not an XZ file, removing it\r\n", name);
        lfs_remove(g_lfs, name);
        return -1;
    }
    printf("AOT: downloaded %s (%lu bytes, %s)\r\n", name,
           (unsigned long)ctx.written,
           is_xz ? "XZ-compressed AOT" : "raw AOT");
    return 0;
}

static int aot_install(int argc, char **argv)
{
    aot_file_info_t info;

    if (argc != 2) {
        printf("Usage: aot_install <file.aot|file.aot.xz>\r\n");
        return -1;
    }
    if (!aot_lfs_ready())
        return -1;
    if (!aot_name_valid(argv[1]) || !aot_has_aot_suffix(argv[1])) {
        printf("AOT: invalid .aot/.aot.xz filename\r\n");
        return -1;
    }
    if (aot_file_probe(g_lfs, argv[1], &info) != 0)
        return -1;
    return aot_slot_write_file(g_lfs, argv[1], &info);
}

static int aot_run(int argc, char **argv)
{
    if (argc > 2) {
        printf("Usage: aot_run [entry]\r\n");
        return -1;
    }
    return aot_run_slot(argc > 1 ? argv[1] : "main");
}

static int aot_run_lfs(int argc, char **argv)
{
    aot_file_info_t info;
    const char *entry;
    uint8_t *image = NULL;
    uint32_t decoded = 0;
    int ret;

    if (argc < 2 || argc > 3) {
        printf("Usage: aot_run_lfs <file.aot|file.aot.xz> [entry]\r\n");
        return -1;
    }
    if (!aot_lfs_ready())
        return -1;
    if (!aot_name_valid(argv[1]) || !aot_has_aot_suffix(argv[1])) {
        printf("AOT: invalid .aot/.aot.xz filename\r\n");
        return -1;
    }
    if (aot_file_probe(g_lfs, argv[1], &info) != 0)
        return -1;
    entry = argc > 2 ? argv[2] : "main";

    if (info.image_type == AOT_IMAGE_TYPE_XIP) {
        printf("AOT: XIP image, %s to the raw AOT slot and executing from "
               "Flash\r\n",
               info.compressed ? "decompressing" : "copying");
        if (aot_slot_write_file(g_lfs, argv[1], &info) != 0)
            return -1;
        return aot_run_slot(entry);
    }

    printf("AOT: REL image, %s into OCRAM\r\n",
           info.compressed ? "decompressing" : "loading");
    image = kmalloc(info.image_size, MM_FLAG_HEAP_OCRAM_0 | MM_FLAG_ALIGN_4BYTE);
    if (!image) {
        printf("AOT: OCRAM allocation failed, falling back to the default "
               "heap\r\n");
        image = kmalloc(info.image_size,
                        MM_FLAG_HEAP_ANY | MM_FLAG_ALIGN_4BYTE);
    }
    if (!image) {
        printf("AOT: cannot allocate %lu bytes for the image\r\n",
               (unsigned long)info.image_size);
        return -1;
    }
    if (info.compressed) {
        if (aot_xz_decode_to_buffer(g_lfs, argv[1], image, info.image_size,
                                    &decoded)
            != 0) {
            kfree(image);
            return -1;
        }
        if (decoded != info.image_size) {
            printf("AOT: decoded size mismatch\r\n");
            kfree(image);
            return -1;
        }
    } else if (aot_read_file_to_buffer(g_lfs, argv[1], image,
                                       info.image_size) != 0) {
        kfree(image);
        return -1;
    }
    /* The staging buffer must stay alive while the module runs: the AOT
     * loader keeps pointers (strings, memory init data) into it, so
     * aot_exec_image() releases it after wasm_runtime_unload(). */
    ret = aot_exec_image(image, info.image_size, entry, "OCRAM", true);
    return ret;
}

#if defined(CONFIG_WL80211)
static int aot_wifi_connect(int argc, char **argv)
{
    int ret;
    const char *bssid = NULL;
    uint8_t channel = 0;

    if (argc < 3 || argc > 5) {
        printf("Usage: aot_wifi_connect <ssid> <password> [bssid] [channel]\r\n");
        return -1;
    }
    if (argc >= 4)
        bssid = argv[3];
    if (argc >= 5)
        channel = (uint8_t)strtoul(argv[4], NULL, 0);

    ret = aot_wl80211_connect(argv[1], argv[2], bssid, channel);
    if (ret < 0) {
        printf("AOT: WiFi connect request failed: %d\r\n", ret);
        return -1;
    }
    printf("AOT: WiFi connect request submitted for %s\r\n", argv[1]);
    return 0;
}
#endif

int aot_store_init(void)
{
    bflb_mtd_init();
    g_lfs = lfs_xip_init(&g_lfs_ctx, &g_lfs_cfg);
    if (!g_lfs) {
        printf("AOT: LittleFS mount failed\r\n");
        return -1;
    }
    printf("AOT: LittleFS mounted on %s; slot=%s\r\n", AOT_LFS_PARTITION,
           AOT_SLOT_PARTITION);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(aot_list, aot_list, List AOT files in LittleFS);
SHELL_CMD_EXPORT_ALIAS(aot_download, aot_download,
                       Download an AOT file (optionally XZ) into LittleFS);
SHELL_CMD_EXPORT_ALIAS(aot_install, aot_install,
                       Install a LittleFS AOT file into the raw AOT slot);
SHELL_CMD_EXPORT_ALIAS(aot_run, aot_run, Run an AOT from the raw XIP slot);
SHELL_CMD_EXPORT_ALIAS(aot_run_lfs, aot_run_lfs,
                       Decompress and run an AOT file stored in LittleFS);
#if defined(CONFIG_WL80211)
SHELL_CMD_EXPORT_ALIAS(aot_wifi_connect, aot_wifi_connect,
                       Connect to a WiFi AP through WL80211);
#endif
