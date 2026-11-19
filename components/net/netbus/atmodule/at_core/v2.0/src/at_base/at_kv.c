#include <stdlib.h>
#include <string.h>
#include "at_kv.h"
#include "lfs_port.h"

/* On-flash record: header followed by length bytes of value data. */
typedef struct {
    char key[AT_KV_NAME_MAX + 1];
    uint32_t type;
    uint32_t length;
} at_kv_entry_t;

static lfs_t *kv_fs;
static struct lfs_context kv_ctx = { .partition_name = "user_kv" };
static struct lfs_config kv_cfg = {
    .read_size = 256,
    .prog_size = 256,
    .lookahead_size = 256,
    .cache_size = 512,
    .block_size = 4096,
    .block_cycles = 500,
};

int at_kv_init(void)
{
    kv_fs = lfs_xip_init(&kv_ctx, &kv_cfg);
    if (kv_fs == NULL) {
        return LFS_ERR_IO;
    }
    return 0;
}

int at_kv_deinit(void)
{
    int ret;

    if (kv_fs == NULL) {
        return 0;
    }
    ret = lfs_unmount(kv_fs);
    if (ret != 0) {
        return ret;
    }
    kv_fs = NULL;
    kv_ctx.lfs.cfg = NULL;
    vSemaphoreDelete(kv_ctx.fs_giant_lock);
    kv_ctx.fs_giant_lock = NULL;
    return 0;
}

static int at_kv_load(const char *namespace, uint8_t **data)
{
    lfs_file_t file;
    at_kv_entry_t entry;
    int size, ret;

    *data = NULL;
    if (kv_fs == NULL) {
        return LFS_ERR_IO;
    }
    ret = lfs_file_open(kv_fs, &file, namespace, LFS_O_RDONLY);
    if (ret != 0) {
        return ret;
    }
    size = lfs_file_size(kv_fs, &file);
    if (size < 0) {
        lfs_file_close(kv_fs, &file);
        return LFS_ERR_IO;
    }
    *data = malloc((size_t)size + 1);
    if (*data == NULL) {
        lfs_file_close(kv_fs, &file);
        return LFS_ERR_NOMEM;
    }
    ret = lfs_file_read(kv_fs, &file, *data, size);
    lfs_file_close(kv_fs, &file);
    if (ret != size) {
        free(*data);
        *data = NULL;
        return LFS_ERR_IO;
    }
    for (size_t pos = 0; pos < (size_t)size; pos += sizeof(entry) + entry.length) {
        if ((size_t)size - pos < sizeof(entry)) {
            goto invalid;
        }
        memcpy(&entry, *data + pos, sizeof(entry));
        if (entry.length > (size_t)size - pos - sizeof(entry) ||
            memchr(entry.key, '\0', sizeof(entry.key)) == NULL) {
            goto invalid;
        }
    }
    return size;

invalid:
    free(*data);
    *data = NULL;
    return LFS_ERR_INVAL;
}

static size_t at_kv_find(const uint8_t *data, size_t size, const char *key, at_kv_entry_t *entry)
{
    size_t pos = 0;

    while (pos < size) {
        memcpy(entry, data + pos, sizeof(*entry));
        if (strcmp(entry->key, key) == 0) {
            break;
        }
        pos += sizeof(*entry) + entry->length;
    }
    return pos;
}

int at_kv_get(const char *namespace, const char *key, uint8_t *type, void **value)
{
    uint8_t *data;
    at_kv_entry_t entry;
    int size = at_kv_load(namespace, &data);
    size_t pos;

    if (size < 0) {
        return size;
    }
    pos = at_kv_find(data, size, key, &entry);
    if (pos == (size_t)size) {
        free(data);
        return LFS_ERR_NOENT;
    }
    *type = entry.type;
    memmove(data, data + pos + sizeof(entry), entry.length);
    *value = data;
    return entry.length;
}

int at_kv_parse_number(int type, const char *text, int64_t *value)
{
    static const int64_t min_value[] = {0, INT8_MIN, 0, INT16_MIN, 0, INT32_MIN};
    static const int64_t max_value[] = {UINT8_MAX, INT8_MAX, UINT16_MAX, INT16_MAX, UINT32_MAX, INT32_MAX};
    char *end;
    long long number = strtoll(text, &end, 10);

    if (end == text || *end != '\0' || type < AT_KV_U8 || type > AT_KV_I32) {
        return LFS_ERR_INVAL;
    }
    if (number < min_value[type - 1] || number > max_value[type - 1]) {
        return LFS_ERR_INVAL;
    }
    *value = number;
    return 0;
}

int at_kv_set(const char *namespace, const char *key, uint8_t type,
              const void *value, size_t length)
{
    uint8_t *data, *updated;
    at_kv_entry_t entry;
    lfs_file_t file;
    int size = at_kv_load(namespace, &data);
    int ret, close_ret;
    size_t pos, removed;

    if (size == LFS_ERR_NOENT && type != 0) {
        size = 0;
    }
    if (size < 0) {
        return size;
    }
    if (key == NULL) {
        size = 0;
    } else {
        pos = at_kv_find(data, size, key, &entry);
        if (pos < (size_t)size) {
            removed = sizeof(entry) + entry.length;
            memmove(data + pos, data + pos + removed, size - pos - removed);
            size -= removed;
        } else if (type == 0) {
            free(data);
            return LFS_ERR_NOENT;
        }
        if (type != 0) {
            updated = realloc(data, size + sizeof(entry) + length);
            if (updated == NULL) {
                free(data);
                return LFS_ERR_NOMEM;
            }
            data = updated;
            memset(&entry, 0, sizeof(entry));
            strcpy(entry.key, key);
            entry.type = type;
            entry.length = length;
            memcpy(data + size, &entry, sizeof(entry));
            memcpy(data + size + sizeof(entry), value, length);
            size += sizeof(entry) + length;
        }
    }
    ret = lfs_file_open(kv_fs, &file, namespace, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (ret == 0) {
        ret = lfs_file_write(kv_fs, &file, data, size);
        close_ret = lfs_file_close(kv_fs, &file);
        if (ret == size) {
            ret = close_ret;
        } else if (ret >= 0) {
            ret = LFS_ERR_IO;
        }
    }
    free(data);
    return ret;
}

int at_kv_foreach(const char *namespace, at_kv_foreach_cb_t callback, void *arg)
{
    lfs_dir_t dir;
    struct lfs_info info;
    at_kv_entry_t entry;
    uint8_t *data;
    int size, ret;

    if (namespace == NULL) {
        if (kv_fs == NULL) {
            return LFS_ERR_IO;
        }
        ret = lfs_dir_open(kv_fs, &dir, "/");
        if (ret != 0) {
            return ret;
        }
        while ((ret = lfs_dir_read(kv_fs, &dir, &info)) > 0) {
            if (info.type == LFS_TYPE_REG) {
                ret = callback(info.name, 0, arg);
                if (ret != 0) {
                    break;
                }
            }
        }
        lfs_dir_close(kv_fs, &dir);
        return ret;
    }
    size = at_kv_load(namespace, &data);
    if (size < 0) {
        return size;
    }
    ret = 0;
    for (size_t pos = 0; pos < (size_t)size; pos += sizeof(entry) + entry.length) {
        memcpy(&entry, data + pos, sizeof(entry));
        ret = callback(entry.key, entry.type, arg);
        if (ret != 0) {
            break;
        }
    }
    free(data);
    return ret;
}
