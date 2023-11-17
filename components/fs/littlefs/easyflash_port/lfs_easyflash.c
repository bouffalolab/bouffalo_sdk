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
#include "easyflash.h"

static lfs_t *lfs = NULL;
static char key_buffer[64];

#ifndef LFS_EF_NAMESPACE
#define LFS_EF_NAMESPACE "/_ef4_kvs_"
#endif

#ifdef CONFIG_FREERTOS
static SemaphoreHandle_t env_giant_lock = NULL;
#endif

/* littlefs init */
EfErrCode
easyflash_init(void)
{
    struct lfs_info stat;
    int32_t ret;

    lfs = lfs_xip_init();
    if (lfs == NULL) {
        LOG_E("littlefs backend init failed.\r\n");
        return EF_ENV_INIT_FAILED;
    }

#ifdef CONFIG_FREERTOS
#if configUSE_RECURSIVE_MUTEXES
    env_giant_lock = xSemaphoreCreateRecursiveMutex();
#else
    env_giant_lock = xSemaphoreCreateMutex();
#endif
#endif

    /* init namespace */
    ret = lfs_stat(lfs, LFS_EF_NAMESPACE, &stat);
    if (ret == LFS_ERR_OK) {
        if (stat.type == LFS_TYPE_DIR) {
            return EF_NO_ERR;
        } else if (stat.type == LFS_TYPE_REG) {
            LOG_E("namespace directory conflicts with standard file.\r\n");
            return EF_ENV_INIT_FAILED;
        }
    }

    ret = lfs_mkdir(lfs, LFS_EF_NAMESPACE);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        LOG_E("create namespace directory failed.\r\n");
        return EF_ENV_INIT_FAILED;
    }

    return EF_NO_ERR;
}

#ifdef CONFIG_FREERTOS
static void ef_giant_lock(void)
{
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreTakeRecursive(env_giant_lock, portMAX_DELAY);
#else
    xSemaphoreTake(env_giant_lock, portMAX_DELAY);
#endif
}

static void ef_giant_unlock(void)
{
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreGiveRecursive(env_giant_lock);
#else
    xSemaphoreGive(env_giant_lock);
#endif
}

#define EF_GIANT_LOCK() ef_giant_lock()
#define EF_GIANT_UNLOCK(ret) \
    ef_giant_unlock();       \
    return (ret)
#else
#define EF_GIANT_LOCK()
#define EF_GIANT_UNLOCK(ret) return (ret)
#endif

/* only supported on ef_env.c */
size_t
ef_get_env_blob(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    lfs_file_t file;
    int32_t ret, read_len;

    if (lfs == NULL) {
        return 0;
    }

    if (key == NULL || value_buf == NULL || buf_len == 0) {
        return 0;
    }

    EF_GIANT_LOCK();

    ret = snprintf(key_buffer, sizeof(key_buffer), "%s/%s", LFS_EF_NAMESPACE, key);
    if (ret >= sizeof(key_buffer)) {
        LOG_E("key name is too long to truncated.\r\n");
        EF_GIANT_UNLOCK(0);
    }

    ret = lfs_file_open(lfs, &file, key_buffer, LFS_O_RDONLY);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        EF_GIANT_UNLOCK(0);
    }

    ret = lfs_file_read(lfs, &file, value_buf, buf_len);
    if (ret < 0) {
        LOG_E("lfs_file_read failed with errno:%d.\r\n", ret);
        errno = -ret;
        lfs_file_close(lfs, &file);
        EF_GIANT_UNLOCK(0);
    }

    read_len = ret;

    if (saved_value_len != NULL) {
        ret = lfs_file_size(lfs, &file);
        if (ret < 0) {
            LOG_E("lfs_file_size failed with errno:%d.\r\n", ret);
            lfs_file_close(lfs, &file);
            errno = -ret;
            EF_GIANT_UNLOCK(0);
        }
        *saved_value_len = ret;
    }

    lfs_file_close(lfs, &file);

    EF_GIANT_UNLOCK(read_len);
}

EfErrCode
ef_set_env_blob(const char *key, const void *value_buf, size_t buf_len)
{
    lfs_file_t file;
    int32_t ret;

    if (lfs == NULL) {
        return EF_ENV_INIT_FAILED;
    }

    if (key == NULL || value_buf == NULL || buf_len == 0) {
        return EF_ENV_ARG_ERR;
    }

    EF_GIANT_LOCK();

    ret = snprintf(key_buffer, sizeof(key_buffer), "%s/%s", LFS_EF_NAMESPACE, key);
    if (ret >= sizeof(key_buffer)) {
        LOG_E("key name is too long to truncated.\r\n");
        EF_GIANT_UNLOCK(EF_ENV_NAME_ERR);
    }

    ret = lfs_file_open(lfs, &file, key_buffer, LFS_O_RDWR | LFS_O_CREAT);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        LOG_E("lfs_file_open failed with errno:%d\r\n", ret);
        EF_GIANT_UNLOCK(EF_WRITE_ERR);
    }

    ret = lfs_file_write(lfs, &file, value_buf, buf_len);
    if (ret != buf_len) {
        errno = -ret;
        LOG_E("lfs_file_write failed with errno:%d.\r\n", ret);
        lfs_file_close(lfs, &file);
        EF_GIANT_UNLOCK(EF_WRITE_ERR);
    }

    ret = lfs_file_truncate(lfs, &file, buf_len);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        LOG_E("lfs_file_truncate failed with errno:%d\r\n", ret);
        lfs_file_close(lfs, &file);
        EF_GIANT_UNLOCK(EF_WRITE_ERR);
    }

    lfs_file_close(lfs, &file);

    EF_GIANT_UNLOCK(EF_NO_ERR);
}

EfErrCode
ef_del_env(const char *key)
{
    int32_t ret;

    if (lfs == NULL) {
        return EF_ENV_INIT_FAILED;
    }

    if (key == NULL) {
        return EF_ENV_ARG_ERR;
    }

    ret = snprintf(key_buffer, sizeof(key_buffer), "%s/%s", LFS_EF_NAMESPACE, key);
    if (ret >= sizeof(key_buffer)) {
        LOG_E("key name is too long to truncated.\r\n");
        EF_GIANT_UNLOCK(EF_ENV_NAME_ERR);
    }

    lfs_remove(lfs, key_buffer);

    return EF_ENV_ARG_ERR;
}

/****************************************************************************
 *   DEPRECATED DEPRECATED-DEPRECATED-DEPRECATED-DEPRECATED-DEPRECATED
 ***************************************************************************/
static bool ef_is_str(uint8_t *value, size_t len)
{
#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
    size_t i;

    for (i = 0; i < len; i++) {
        if (!__is_print(value[i])) {
            return false;
        }
    }
    return true;
}

/****************************************************************************
 *   DEPRECATED DEPRECATED-DEPRECATED-DEPRECATED-DEPRECATED-DEPRECATED
 ***************************************************************************/
char *
ef_get_env(const char *key)
{
#ifndef EF_STR_ENV_VALUE_MAX_SIZE
#define EF_STR_ENV_VALUE_MAX_SIZE (3979)
#endif
    static char value[EF_STR_ENV_VALUE_MAX_SIZE + 1];
    size_t get_size;

    printf("WARNING!!! ef_get_env is deprecated, use ef_get_env_blob instead.\r\n");

    get_size = ef_get_env_blob(key, value, EF_STR_ENV_VALUE_MAX_SIZE, NULL);
    value[get_size] = '\0';

    get_size = get_size > 0 ? strlen(value) : get_size;
    if (get_size > 0 && ef_is_str((uint8_t *)value, get_size)) {
        value[get_size] = '\0';
        return value;
    } else {
        LOG_W("The ENV value isn't string. Could not be returned\r\n");
        return NULL;
    }
}

EfErrCode
ef_set_env(const char *key, const char *value)
{
    return ef_set_env_blob(key, value, strlen(value) + 1);
}

EfErrCode
ef_save_env(void)
{
    return EF_NO_ERR;
}

/* clear all env */
EfErrCode
ef_env_set_default(void)
{
    return EF_ENV_ARG_ERR;
}

EfErrCode
ef_set_and_save_env(const char *key, const char *value)
{
    return ef_set_env(key, value);
}

EfErrCode
ef_del_and_save_env(const char *key)
{
    return ef_del_env(key);
}
