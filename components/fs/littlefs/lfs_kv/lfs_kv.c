#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif

#define DBG_TAG "LFS"

#include "log.h"

#include "lfs.h"
#include "lfs_port.h"
#include "lfs_kv.h"

static lfs_t *lfs = NULL;
static char path_buffer[LFS_KV_KEY_MAX];

#ifndef LFS_KV_NAMESPACE
/* legacy on-flash name from the EasyFlash4 days -- DO NOT rename, or
 * existing devices lose access to all stored key-values after an OTA. */
#define LFS_KV_NAMESPACE "/_ef4_kvs_"
#endif

#ifdef CONFIG_FREERTOS
static SemaphoreHandle_t env_giant_lock = NULL;
#endif

static struct lfs_context lfs_ctx = { .partition_name = "PSM" };
static struct lfs_config lfs_cfg = { .read_size = 256,
                                     .prog_size = 256,
                                     .lookahead_size = 256,
                                     .cache_size = 512,
                                     .block_size = 4096,
                                     .block_cycles = 500
                                   };

/* littlefs init */
lfs_kv_err_t
lfs_kv_init(void)
{
    struct lfs_info stat;
    int32_t ret;

    lfs = lfs_xip_init(&lfs_ctx, &lfs_cfg);
    if (lfs == NULL) {
        LOG_E("littlefs backend init failed.\r\n");
        return LFS_KV_ERR_INIT_FAILED;
    }

#ifdef CONFIG_FREERTOS
#if configUSE_RECURSIVE_MUTEXES
    env_giant_lock = xSemaphoreCreateRecursiveMutex();
#else
    env_giant_lock = xSemaphoreCreateMutex();
#endif
#endif

    /* init namespace */
    ret = lfs_stat(lfs, LFS_KV_NAMESPACE, &stat);
    if (ret == LFS_ERR_OK) {
        if (stat.type == LFS_TYPE_DIR) {
            return LFS_KV_OK;
        } else if (stat.type == LFS_TYPE_REG) {
            LOG_E("namespace directory conflicts with standard file.\r\n");
            return LFS_KV_ERR_INIT_FAILED;
        }
    }

    ret = lfs_mkdir(lfs, LFS_KV_NAMESPACE);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        LOG_E("create namespace directory failed.\r\n");
        return LFS_KV_ERR_INIT_FAILED;
    }

    return LFS_KV_OK;
}

#ifdef CONFIG_FREERTOS
static void lfs_kv_lock(void)
{
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreTakeRecursive(env_giant_lock, portMAX_DELAY);
#else
    xSemaphoreTake(env_giant_lock, portMAX_DELAY);
#endif
}

static void lfs_kv_unlock(void)
{
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreGiveRecursive(env_giant_lock);
#else
    xSemaphoreGive(env_giant_lock);
#endif
}

#define LFS_KV_LOCK() lfs_kv_lock()
#define LFS_KV_UNLOCK(ret) \
    lfs_kv_unlock();       \
    return (ret)
#else
#define LFS_KV_LOCK()
#define LFS_KV_UNLOCK(ret) return (ret)
#endif

static int
kv_key2path(char *buf, size_t buf_len, const char *prefix, const char *path)
{
    int i, j = 0;
    int prefix_len = strlen(prefix);
    int path_len;

    i = prefix_len + 1; /* prefix + '/' */
    /* cal full path string length */
    for (j = 0; path[j] != 0; j++) {
        switch (path[j]) {
            case '}':
            case '/':
                i += 2;
                break;
            default:
                i++;
        }
    }
    path_len = j;

    /* oversize */
    if (i > buf_len - 1) {
        return i;
    }

    /* do strcat */
    memcpy(buf, prefix, prefix_len);
    buf[prefix_len] = '/';

    for (j = 0, i = 0; j < path_len; j++) {
        switch (path[j]) {
            case '}':
            case '/':
                (buf + prefix_len + 1)[i++] = '}';
                (buf + prefix_len + 1)[i++] = path[j] ^ 0x20;
                break;
            default:
                (buf + prefix_len + 1)[i++] = path[j];
        }
    }

    (buf + prefix_len + 1)[i] = 0;
    return prefix_len + 1 + i;
}

static int
kv_path2key(char *buf, size_t buf_len, const char *key)
{
    int i, j = 0;
    int key_len = strlen(key);

    i = 0;
    for (j = 0; j < key_len; j++) {
        switch (key[j]) {
            case '}':
                i++;
                assert(j < key_len);
                j++; /* skip next character */
                break;
            default:
                i++;
        }
    }

    /* oversize */
    if (i > buf_len - 1) {
        return i;
    }

    for (j = 0, i = 0; j < key_len; j++) {
        switch (key[j]) {
            case '}':
                (buf)[i++] = key[++j] ^ 0x20;
                break;
            default:
                (buf)[i++] = key[j];
        }
    }

    (buf)[i] = 0;
    return i;
}

size_t
lfs_kv_get_blob_offset(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len, int offset)
{
    lfs_file_t file;
    int32_t ret, read_len = 0;

    if (lfs == NULL || key == NULL) {
        return 0;
    }

    /* if key not exist, also set saved_value_len */
    if (saved_value_len != NULL) {
        *saved_value_len = 0;
    }

    LFS_KV_LOCK();

    ret = kv_key2path(path_buffer, sizeof(path_buffer), LFS_KV_NAMESPACE, key);
    if (ret >= sizeof(path_buffer)) {
        LOG_E("key name is too long to truncated.\r\n");
        LFS_KV_UNLOCK(0);
    }

    ret = lfs_file_open(lfs, &file, path_buffer, LFS_O_RDONLY);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        LFS_KV_UNLOCK(0);
    }

    if (value_buf != NULL && buf_len != 0) {
        ret = lfs_file_seek(lfs, &file, offset, LFS_SEEK_SET);
        if (ret < 0) {
            errno = -ret;
            lfs_file_close(lfs, &file);
            LFS_KV_UNLOCK(0);
        }

        ret = lfs_file_read(lfs, &file, value_buf, buf_len);
        if (ret < 0) {
            LOG_E("lfs_file_read failed with errno:%d.\r\n", ret);
            errno = -ret;
            lfs_file_close(lfs, &file);
            LFS_KV_UNLOCK(0);
        }

        read_len = ret;
    }

    if (saved_value_len != NULL) {
        ret = lfs_file_size(lfs, &file);
        if (ret < 0) {
            LOG_E("lfs_file_size failed with errno:%d.\r\n", ret);
            lfs_file_close(lfs, &file);
            errno = -ret;
            LFS_KV_UNLOCK(0);
        }
        *saved_value_len = ret;
    }

    lfs_file_close(lfs, &file);

    LFS_KV_UNLOCK(read_len);
}

size_t
lfs_kv_get_blob(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    return lfs_kv_get_blob_offset(key, value_buf, buf_len, saved_value_len, 0);
}

lfs_kv_err_t
lfs_kv_set_blob(const char *key, const void *value_buf, size_t buf_len)
{
    lfs_file_t file;
    int32_t ret;

    if (lfs == NULL) {
        return LFS_KV_ERR_INIT_FAILED;
    }

    if (key == NULL || value_buf == NULL || buf_len == 0) {
        return LFS_KV_ERR_ARG;
    }

    LFS_KV_LOCK();

    ret = kv_key2path(path_buffer, sizeof(path_buffer), LFS_KV_NAMESPACE, key);
    if (ret >= sizeof(path_buffer)) {
        LOG_E("key name is too long to truncated.\r\n");
        LFS_KV_UNLOCK(LFS_KV_ERR_NAME);
    }

    ret = lfs_file_open(lfs, &file, path_buffer, LFS_O_RDWR | LFS_O_CREAT);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        LOG_E("lfs_file_open failed with errno:%d\r\n", ret);
        LFS_KV_UNLOCK(LFS_KV_ERR_WRITE);
    }

    ret = lfs_file_write(lfs, &file, value_buf, buf_len);
    if (ret != buf_len) {
        errno = -ret;
        LOG_E("lfs_file_write failed with errno:%d.\r\n", ret);
        lfs_file_close(lfs, &file);
        LFS_KV_UNLOCK(LFS_KV_ERR_WRITE);
    }

    ret = lfs_file_truncate(lfs, &file, buf_len);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        LOG_E("lfs_file_truncate failed with errno:%d\r\n", ret);
        lfs_file_close(lfs, &file);
        LFS_KV_UNLOCK(LFS_KV_ERR_WRITE);
    }

    lfs_file_close(lfs, &file);

    LFS_KV_UNLOCK(LFS_KV_OK);
}

lfs_kv_err_t
lfs_kv_del(const char *key)
{
    int32_t ret;

    if (lfs == NULL) {
        return LFS_KV_ERR_INIT_FAILED;
    }

    if (key == NULL) {
        return LFS_KV_ERR_ARG;
    }

    LFS_KV_LOCK();
    ret = kv_key2path(path_buffer, sizeof(path_buffer), LFS_KV_NAMESPACE, key);
    if (ret >= sizeof(path_buffer)) {
        LOG_E("key name is too long to truncated.\r\n");
        LFS_KV_UNLOCK(LFS_KV_ERR_NAME);
    }

    lfs_remove(lfs, path_buffer);
    LFS_KV_UNLOCK(LFS_KV_OK);
}

/* clear all kv */
lfs_kv_err_t
lfs_kv_clear(void)
{
    lfs_dir_t dir = {};
    int ret;

    if (lfs == NULL) {
        return LFS_KV_ERR_INIT_FAILED;
    }

    LFS_KV_LOCK();

    ret = lfs_dir_open(lfs, &dir, LFS_KV_NAMESPACE);
    if (ret != LFS_ERR_OK) {
      errno = -ret;
      LFS_KV_UNLOCK(LFS_KV_ERR_READ);
    }

    struct lfs_info info = {};

    while (1) {
        ret = lfs_dir_read(lfs, &dir, &info);
        if (ret < 0) {
            errno = -ret;
            ret = LFS_KV_ERR_READ;
            break;
        } else if (ret == 0) {
            ret = LFS_KV_OK;
            break;
        }

        if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
            continue;
        }

        if (info.type != LFS_TYPE_REG) {
            LOG_E("Unexpected file type! name:%s, size: %d, type: %d\r\n", info.name, info.size, info.type);
            ret = LFS_KV_ERR_NAME;
            break;
        }

        ret = snprintf(path_buffer, sizeof(path_buffer), "%s/%s", LFS_KV_NAMESPACE, info.name);
        assert(ret <= sizeof(path_buffer) - 1);

        ret = lfs_remove(lfs, path_buffer);
        if (ret < 0) {
            errno = -ret;
            ret = LFS_KV_ERR_WRITE;
            break;
        }
    }

    lfs_dir_close(lfs, &dir);
    LFS_KV_UNLOCK(ret);
}

lfs_kv_err_t
lfs_kv_print(void)
{
    lfs_dir_t dir = {};
    int ret;

    if (lfs == NULL) {
        return LFS_KV_ERR_INIT_FAILED;
    }

    LFS_KV_LOCK();

    ret = lfs_dir_open(lfs, &dir, LFS_KV_NAMESPACE);
    if (ret != LFS_ERR_OK) {
      errno = -ret;
      LFS_KV_UNLOCK(LFS_KV_ERR_READ);
    }

    struct lfs_info info = {};

    while (1) {
        ret = lfs_dir_read(lfs, &dir, &info);
        if (ret < 0) {
            errno = -ret;
            ret = LFS_KV_ERR_READ;
            break;
        } else if (ret == 0) {
            ret = LFS_KV_OK;
            break;
        }

        if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
            continue;
        }

        kv_path2key(path_buffer, sizeof(path_buffer), info.name);
        printf("key: %s, size: %d, type: %d\r\n", path_buffer, info.size, info.type);
    }

    lfs_dir_close(lfs, &dir);
    LFS_KV_UNLOCK(LFS_KV_OK);
}

lfs_kv_err_t lfs_kv_foreach(lfs_kv_foreach_cb_t cb, void *arg) {
    lfs_dir_t dir = {};
    int ret;

    if (lfs == NULL) {
        return LFS_KV_ERR_INIT_FAILED;
    }

    if (cb == NULL) {
        return LFS_KV_ERR_ARG;
    }

    LFS_KV_LOCK();

    ret = lfs_dir_open(lfs, &dir, LFS_KV_NAMESPACE);
    if (ret != LFS_ERR_OK) {
      errno = -ret;
      LFS_KV_UNLOCK(LFS_KV_ERR_READ);
    }

    struct lfs_info info = {};
    char buf_p[LFS_KV_KEY_MAX];

    while (1) {
        ret = lfs_dir_read(lfs, &dir, &info);
        if (ret < 0) {
            errno = -ret;
            ret = LFS_KV_ERR_READ;
            break;
        } else if (ret == 0) {
            ret = LFS_KV_OK;
            break;
        }

        if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
            continue;
        }

        /* do not use path_buffer here! cb may call lfs_kv_get_blob,
         * reentrancy issues may occur.*/
        memset(buf_p, 0, sizeof(buf_p));
        kv_path2key(buf_p, sizeof(buf_p), info.name);
        ret = cb(buf_p, arg);
        if (ret != LFS_KV_OK) {
            LFS_KV_UNLOCK(ret);
        }
    }

    lfs_dir_close(lfs, &dir);
    LFS_KV_UNLOCK(LFS_KV_OK);
}
