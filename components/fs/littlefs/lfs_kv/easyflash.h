/*
 * Deprecated EasyFlash4 compatibility layer on top of the lfs_kv API.
 *
 * Every ef_* entry point below is a static inline wrapper around the
 * equivalent lfs_kv_* function and is marked __attribute__((deprecated)),
 * so each call site gets a compile-time warning.  New code should include
 * lfs_kv.h and use the lfs_kv_* API directly; this header will be removed
 * in a future release.
 *
 * NOTE for maintainers: the wrappers must call lfs_kv_* functions only.
 * GCC emits -Wdeprecated-declarations for calls to a deprecated function
 * even when the call sits inside an inline function that is never used,
 * so a wrapper calling another ef_* wrapper would warn on every inclusion
 * of this header.
 */

#ifndef EASYFLASH_H_
#define EASYFLASH_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "lfs_kv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* error code */
typedef lfs_kv_err_t EfErrCode;

enum {
    EF_NO_ERR          = LFS_KV_OK,
    EF_ERASE_ERR       = LFS_KV_ERR_ERASE,
    EF_READ_ERR        = LFS_KV_ERR_READ,
    EF_WRITE_ERR       = LFS_KV_ERR_WRITE,
    EF_ENV_NAME_ERR    = LFS_KV_ERR_NAME,
    EF_ENV_NAME_EXIST  = LFS_KV_ERR_NAME_EXIST,
    EF_ENV_FULL        = LFS_KV_ERR_FULL,
    EF_ENV_INIT_FAILED = LFS_KV_ERR_INIT_FAILED,
    EF_ENV_ARG_ERR     = LFS_KV_ERR_ARG,
};

/* the ENV max name length must less then it */
#ifndef EF_ENV_NAME_MAX
#define EF_ENV_NAME_MAX                          LFS_KV_KEY_MAX
#endif

#if defined(__GNUC__)
#define EF_DEPRECATED \
    __attribute__((deprecated("EasyFlash KV API is deprecated, use the lfs_kv API (lfs_kv.h) instead")))
#else
#define EF_DEPRECATED
#endif

/* lfs_kv.c */
EF_DEPRECATED static inline EfErrCode
easyflash_init(void)
{
    return lfs_kv_init();
}

EF_DEPRECATED static inline size_t
ef_get_env_blob(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    return lfs_kv_get_blob(key, value_buf, buf_len, saved_value_len);
}

EF_DEPRECATED static inline size_t
ef_get_env_blob_offset(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len, int offset)
{
    return lfs_kv_get_blob_offset(key, value_buf, buf_len, saved_value_len, offset);
}

EF_DEPRECATED static inline EfErrCode
ef_set_env_blob(const char *key, const void *value_buf, size_t buf_len)
{
    return lfs_kv_set_blob(key, value_buf, buf_len);
}

EF_DEPRECATED static inline EfErrCode
ef_del_env(const char *key)
{
    return lfs_kv_del(key);
}

/* clear all env */
EF_DEPRECATED static inline EfErrCode
ef_env_set_default(void)
{
    return lfs_kv_clear();
}

EF_DEPRECATED static inline EfErrCode
ef_print_env(void)
{
    return lfs_kv_print();
}

/* Added by Bouffalolab */
typedef lfs_kv_foreach_cb_t ef_foreach_cb_t;
EF_DEPRECATED static inline EfErrCode
ef_foreach_env(ef_foreach_cb_t cb, void *arg)
{
    return lfs_kv_foreach(cb, arg);
}

/****************************************************************************
 *   String-style API.  ef_save_env is a no-op: every write is committed
 *   to the LittleFS backend immediately.
 ***************************************************************************/
static inline bool
ef_is_str(uint8_t *value, size_t len)
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

#ifndef EF_STR_ENV_VALUE_MAX_SIZE
#define EF_STR_ENV_VALUE_MAX_SIZE (3979)
#endif

EF_DEPRECATED static inline char *
ef_get_env(const char *key)
{
    static char value[EF_STR_ENV_VALUE_MAX_SIZE + 1];
    size_t get_size;

    printf("WARNING!!! ef_get_env is deprecated, use ef_get_env_blob instead.\r\n");

    get_size = lfs_kv_get_blob(key, value, EF_STR_ENV_VALUE_MAX_SIZE, NULL);
    value[get_size] = '\0';

    get_size = get_size > 0 ? strlen(value) : get_size;
    if (get_size > 0 && ef_is_str((uint8_t *)value, get_size)) {
        value[get_size] = '\0';
        return value;
    } else {
        printf("WARNING!!! The ENV value isn't string. Could not be returned\r\n");
        return NULL;
    }
}

EF_DEPRECATED static inline EfErrCode
ef_set_env(const char *key, const char *value)
{
    if (value == NULL) {
        return EF_ENV_ARG_ERR;
    }
    return lfs_kv_set_blob(key, value, strlen(value) + 1);
}

EF_DEPRECATED static inline EfErrCode
ef_save_env(void)
{
    return EF_NO_ERR;
}

EF_DEPRECATED static inline EfErrCode
ef_set_and_save_env(const char *key, const char *value)
{
    if (value == NULL) {
        return EF_ENV_ARG_ERR;
    }
    return lfs_kv_set_blob(key, value, strlen(value) + 1);
}

EF_DEPRECATED static inline EfErrCode
ef_del_and_save_env(const char *key)
{
    return lfs_kv_del(key);
}

#ifdef __cplusplus
}
#endif

#endif /* EASYFLASH_H_ */
