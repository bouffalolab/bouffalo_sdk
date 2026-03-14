#include "mimiclaw_port.h"

#include <easyflash.h>

int mimiclaw_kv_set_blob(const char *key, const void *value, size_t len)
{
    if (!key || !value || len == 0) {
        return -1;
    }

    if (ef_set_env_blob(key, value, len) != EF_NO_ERR) {
        return -1;
    }

    return (ef_save_env() == EF_NO_ERR) ? 0 : -1;
}

int mimiclaw_kv_get_blob(const char *key, void *buf, size_t buf_len, size_t *saved_len)
{
    size_t n;

    if (!key || !buf || buf_len == 0) {
        return -1;
    }

    n = ef_get_env_blob(key, buf, buf_len, saved_len);
    return (n > 0) ? 0 : -1;
}
