/*
 * This file is the header file that adapts LittleFS to EasyFlash4.
 */

#ifndef EASYFLASH_H_
#define EASYFLASH_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EasyFlash error code */
typedef enum {
    EF_NO_ERR,
    EF_ERASE_ERR,
    EF_READ_ERR,
    EF_WRITE_ERR,
    EF_ENV_NAME_ERR,
    EF_ENV_NAME_EXIST,
    EF_ENV_FULL,
    EF_ENV_INIT_FAILED,
    EF_ENV_ARG_ERR,
} EfErrCode;

/* easyflash.c */
EfErrCode easyflash_init(void);

/* only supported on ef_env.c */
size_t ef_get_env_blob(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len);
size_t ef_get_env_blob_offset(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len, int offset);
EfErrCode ef_set_env_blob(const char *key, const void *value_buf, size_t buf_len);

char *ef_get_env(const char *key) __attribute__((deprecated("Not recommended, use ef_get_env_blob instead")));
EfErrCode ef_set_env(const char *key, const char *value);
EfErrCode ef_del_env(const char *key);

EfErrCode ef_save_env(void);

/* clear all env */
EfErrCode ef_env_set_default(void);

EfErrCode ef_set_and_save_env(const char *key, const char *value);
EfErrCode ef_del_and_save_env(const char *key);

EfErrCode ef_print_env(void);

#ifdef __cplusplus
}
#endif

#endif /* EASYFLASH_H_ */
