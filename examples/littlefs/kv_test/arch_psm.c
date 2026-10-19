#include "lfs.h"

#include "arch_psm.h"

extern lfs_t *lfs;

#ifndef CONFIG_KV_PSM_PATH
#define CONFIG_KV_PSM_PATH ""
#endif

#ifndef CONFIG_KV_PSM_KEY_LEN
#define CONFIG_KV_PSM_KEY_LEN 64
#endif

#define PSM_KV_FORMAT "%s/%s.%s"
static uint8_t kvs_crc8(uint8_t *buf, uint16_t len)
{
    uint8_t crc = 0x00;
    uint8_t i = 0;
    while (len--) {
        crc ^= *buf++;
        for (i = 8; i > 0; i--) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}

int arch_psm_get_value(const char *name_space, const char *key, void *value, size_t length)
{
    char name[CONFIG_KV_PSM_KEY_LEN];
    int payload = -1;
    uint8_t crc;
    int fd, ret;
    struct lfs_info info;
    lfs_file_t file;

    snprintf(name, CONFIG_KV_PSM_KEY_LEN, PSM_KV_FORMAT, CONFIG_KV_PSM_PATH, name_space, key);

    fd = lfs_file_open(lfs, &file, name, LFS_O_RDONLY);
    if (fd < 0)
        return -1;

    ret = lfs_stat(lfs, name, &info);
    if (ret < 0)
        goto bail;

    payload = info.size - 1;

    if (payload > length)
        goto bail;

    ret = lfs_file_read(lfs, &file, value, payload);
    if (ret != payload)
        goto bail;

    ret = lfs_file_read(lfs, &file, &crc, sizeof(uint8_t));
    if (ret != sizeof(uint8_t))
        goto bail;

    if (crc != kvs_crc8(value, payload))
        ret = -1;

bail:
    lfs_file_close(lfs, &file);

    return ret > 0 ? payload : ret;
}

int arch_psm_get_str(const char *name_space, const char *key, char *str, size_t str_size)
{
    return arch_psm_get_value(name_space, key, str, str_size);
}

int arch_psm_set_value(const char *name_space, const char *key, const void *value, size_t length)
{
    char name[CONFIG_KV_PSM_KEY_LEN];
    uint8_t crc;
    int fd, ret, ret_close;
    int oflags;
    struct lfs_info info;
    lfs_file_t file;

    snprintf(name, CONFIG_KV_PSM_KEY_LEN, PSM_KV_FORMAT, CONFIG_KV_PSM_PATH, name_space, key);

    ret = lfs_stat(lfs, name, &info);
    if (ret < 0) {
        oflags = LFS_O_RDWR | LFS_O_CREAT;
    } else {
        oflags = LFS_O_RDWR | LFS_O_TRUNC;
    }

    fd = lfs_file_open(lfs, &file, name, oflags);
    if (fd < 0)
        return -1;

    ret = lfs_file_write(lfs, &file, value, length);
    if (ret != length)
        goto bail;

    crc = kvs_crc8((uint8_t *)value, length);

    ret = lfs_file_write(lfs, &file, &crc, sizeof(uint8_t));

bail:
    ret_close = lfs_file_close(lfs, &file);
    if ((ret_close < 0) && (oflags & LFS_O_CREAT)) {
        lfs_remove(lfs, name);
    }

    return ret < 0 ? ret : (ret_close < 0 ? ret_close : length);
}

int arch_psm_set_str(const char *name_space, const char *key, const char *str)
{
    return arch_psm_set_value(name_space, key, str, strlen(str) + 1);
}

int arch_psm_erase_key(const char *name_space, const char *key)
{
    char name[CONFIG_KV_PSM_KEY_LEN];

    snprintf(name, CONFIG_KV_PSM_KEY_LEN, PSM_KV_FORMAT, CONFIG_KV_PSM_PATH, name_space, key);

    lfs_remove(lfs, name);

    return 0;
}

int arch_psm_del_value(const char *name_space, const char *key)
{
    return arch_psm_erase_key(name_space, key);
}
