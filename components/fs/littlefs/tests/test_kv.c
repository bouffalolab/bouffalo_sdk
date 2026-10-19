/* Real LittleFS/KV code on an eight-block flash emulator, no credentials. */
#include "lfs_port.h"
#include "lfs_kv.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned char flash[8 * 4096];
static struct lfs_config *config;
static lfs_t *mounted;
static int sync_error;
static int persistent_sync_error;

static int read_flash(const struct lfs_config *cfg, lfs_block_t block,
                      lfs_off_t off, void *buffer, lfs_size_t size)
{
    assert(block < cfg->block_count && off + size <= cfg->block_size);
    memcpy(buffer, flash + block * cfg->block_size + off, size);
    return 0;
}

static int prog_flash(const struct lfs_config *cfg, lfs_block_t block,
                      lfs_off_t off, const void *buffer, lfs_size_t size)
{
    assert(block < cfg->block_count && off + size <= cfg->block_size);
    const unsigned char *input = buffer;
    unsigned char *output = flash + block * cfg->block_size + off;
    for (lfs_size_t i = 0; i < size; i++) {
        assert((output[i] & input[i]) == input[i]);
        output[i] &= input[i];
    }
    return 0;
}

static int erase_flash(const struct lfs_config *cfg, lfs_block_t block)
{
    assert(block < cfg->block_count);
    memset(flash + block * cfg->block_size, 0xff, cfg->block_size);
    return 0;
}

static int sync_flash(const struct lfs_config *cfg)
{
    (void)cfg;
    if (persistent_sync_error) return persistent_sync_error;
    int ret = sync_error;
    sync_error = 0;
    return ret;
}

lfs_t *lfs_xip_init(struct lfs_context *ctx, struct lfs_config *cfg)
{
    cfg->read = read_flash;
    cfg->prog = prog_flash;
    cfg->erase = erase_flash;
    cfg->sync = sync_flash;
    cfg->block_count = sizeof(flash) / cfg->block_size;
    memset(flash, 0xff, sizeof(flash));
    assert(lfs_format(&ctx->lfs, cfg) == 0);
    assert(lfs_mount(&ctx->lfs, cfg) == 0);
    config = cfg;
    mounted = &ctx->lfs;
    return mounted;
}

static void remount(void)
{
    assert(lfs_unmount(mounted) == 0);
    assert(lfs_mount(mounted, config) == 0);
}

static void assert_value(const char *key, const void *value, size_t length)
{
    unsigned char actual[1024];
    size_t saved = 0;
    assert(lfs_kv_get_blob(key, actual, sizeof(actual), &saved) == length);
    assert(saved == length && memcmp(actual, value, length) == 0);
}

int main(void)
{
    assert(lfs_kv_init() == LFS_KV_OK);
    assert(lfs_kv_set_blob("model", "long-model-name", 16) == LFS_KV_OK);
    assert(lfs_kv_set_blob("model", "small", 6) == LFS_KV_OK);
    remount();
    assert_value("model", "small", 6);

    /* For this inline value write/truncate succeed, but close must commit.
     * The old implementation ignored this error and returned success. */
    sync_error = LFS_ERR_IO;
    assert(lfs_kv_set_blob("model", "sync-failed", 12) == LFS_KV_ERR_WRITE);
    remount();
    persistent_sync_error = LFS_ERR_NOSPC;
    assert(lfs_kv_set_blob("model", "full-on-close", 14) == LFS_KV_ERR_FULL);
    persistent_sync_error = 0;
    remount();
    assert(lfs_kv_set_blob("model", "recovered", 10) == LFS_KV_OK);
    remount();
    assert_value("model", "recovered", 10);

    sync_error = LFS_ERR_IO;
    assert(lfs_kv_del("model") == LFS_KV_ERR_WRITE);
    remount();
    assert(lfs_kv_del("model") == LFS_KV_OK);
    assert(lfs_kv_del("already-absent") == LFS_KV_OK);

    /* Project keys exceed the inline threshold. Repeatedly replacing a
     * similarly sized synthetic value must survive reboot without truncation. */
    unsigned char fake_key[192];
    for (unsigned i = 0; i < 200; i++) {
        memset(fake_key, 'A' + i % 26, sizeof(fake_key));
        assert(lfs_kv_set_blob("synthetic-key", fake_key, sizeof(fake_key)) == LFS_KV_OK);
        remount();
        assert_value("synthetic-key", fake_key, sizeof(fake_key));
    }

    unsigned char payload[1024];
    memset(payload, 0x5a, sizeof(payload));
    int full = 0;
    for (unsigned i = 0; i < 32; i++) {
        char name[32];
        snprintf(name, sizeof(name), "fill-%u", i);
        lfs_kv_err_t ret = lfs_kv_set_blob(name, payload, sizeof(payload));
        if (ret == LFS_KV_ERR_FULL) { full = 1; break; }
        assert(ret == LFS_KV_OK);
    }
    assert(full);
    remount();
    assert_value("synthetic-key", fake_key, sizeof(fake_key));
    assert(lfs_unmount(mounted) == 0);
    puts("LittleFS KV commit errors, exhaustion, replacement and reboot tests passed");
    return 0;
}
