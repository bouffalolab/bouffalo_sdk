/*
 * Key-value store API backed by LittleFS.
 */

#ifndef LFS_KV_H_
#define LFS_KV_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* error code */
typedef enum {
    LFS_KV_OK,
    LFS_KV_ERR_ERASE,
    LFS_KV_ERR_READ,
    LFS_KV_ERR_WRITE,
    LFS_KV_ERR_NAME,
    LFS_KV_ERR_NAME_EXIST,
    LFS_KV_ERR_FULL,
    LFS_KV_ERR_INIT_FAILED,
    LFS_KV_ERR_ARG,
} lfs_kv_err_t;

/* the key max name length must less then it */
#ifndef LFS_KV_KEY_MAX
#define LFS_KV_KEY_MAX                          (64)
#endif

/* lfs_kv.c */
lfs_kv_err_t lfs_kv_init(void);

size_t lfs_kv_get_blob(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len);
size_t lfs_kv_get_blob_offset(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len, int offset);
lfs_kv_err_t lfs_kv_set_blob(const char *key, const void *value_buf, size_t buf_len);

lfs_kv_err_t lfs_kv_del(const char *key);

/* clear all kv */
lfs_kv_err_t lfs_kv_clear(void);

lfs_kv_err_t lfs_kv_print(void);

/* Added by Bouffalolab */
typedef lfs_kv_err_t (*lfs_kv_foreach_cb_t)(const char *key, void *arg);
lfs_kv_err_t lfs_kv_foreach(lfs_kv_foreach_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* LFS_KV_H_ */
