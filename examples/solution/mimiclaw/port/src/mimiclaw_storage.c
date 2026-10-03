#include "mimiclaw_port.h"

#include "lfs_kv.h"

int mimiclaw_kv_set_blob(const char *key, const void *value, size_t len)
{
    if (!key || !value || len == 0) {
        return -1;
    }

    if (lfs_kv_set_blob(key, value, len) != LFS_KV_OK) {
        return -1;
    }

    /* every write is committed to the LittleFS backend immediately */
    return 0;
}

int mimiclaw_kv_get_blob(const char *key, void *buf, size_t buf_len, size_t *saved_len)
{
    size_t n;

    if (!key || !buf || buf_len == 0) {
        return -1;
    }

    n = lfs_kv_get_blob(key, buf, buf_len, saved_len);
    return (n > 0) ? 0 : -1;
}
