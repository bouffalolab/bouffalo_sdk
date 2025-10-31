#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <lfs.h>
#include <lfs_port.h>

#include <openthread_port.h>
#include <openthread/platform/settings.h>

#ifndef OPENTHREAD_LFS_NAMESPACE
#define OPENTHREAD_LFS_NAMESPACE "/_ot_"
#endif

#ifndef OPENTHREAD_LFS_WIPE_FLAG
#define OPENTHREAD_LFS_WIPE_FLAG "/_ot_wipe_"
#endif

#define OT_MAX_KEY_LEN              (12 + sizeof(OPENTHREAD_LFS_NAMESPACE) + 1)

#ifdef CONFIG_BL_SDK
    static struct lfs_context lfs_ctx = { .partition_name = "PSM" };
    struct lfs_config lfs_cfg = { .read_size = 256,
                                  .prog_size = 256,
                                  .lookahead_size = 256,
                                  .cache_size = 512,
                                  .block_size = 4096,
                                  .block_cycles = 500
                                };
    static lfs_t *lfs = NULL;
#else
    static lfs_t *lfs = NULL;
#endif


void otPlatSettingsInit(otInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength) 
{
    int ret;
    struct lfs_info stat;

    if (NULL == lfs) {
#ifdef CONFIG_BL_SDK
        lfs = lfs_xip_init(&lfs_ctx, &lfs_cfg);
#else
        lfs = lfs_xip_init();
#endif
    }

    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aSensitiveKeys);
    OT_UNUSED_VARIABLE(aSensitiveKeysLength);

    /* init namespace */
    ret = lfs_stat(lfs, OPENTHREAD_LFS_NAMESPACE, &stat);
    if (ret == LFS_ERR_OK) {
        configASSERT(stat.type == LFS_TYPE_DIR);

        if (LFS_ERR_OK == lfs_stat(lfs, OPENTHREAD_LFS_WIPE_FLAG, &stat)) {
            otPlatSettingsWipe(aInstance);
        }
        return;
    }

    ret = lfs_mkdir(lfs, OPENTHREAD_LFS_NAMESPACE);
    configASSERT(ret == LFS_ERR_OK);
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    char            key[OT_MAX_KEY_LEN];
    lfs_file_t      file;
    int             ret = LFS_ERR_OK;

    OT_UNUSED_VARIABLE(aInstance);
    if (0 != aIndex) {
        return OT_ERROR_NOT_FOUND;
    }

    configASSERT(lfs != NULL);

    if (aInstance) {
        snprintf(key, OT_MAX_KEY_LEN, "%s/ot-%04x", OPENTHREAD_LFS_NAMESPACE, aKey);
    }
    else {
        snprintf(key, OT_MAX_KEY_LEN, "%s/_nl%04x", OPENTHREAD_LFS_NAMESPACE, aKey);
    }

    lfs->cfg->lock(lfs->cfg);

    do {
        ret = lfs_file_open(lfs, &file, key, LFS_O_RDONLY);
        if (ret != LFS_ERR_OK) {
            break;
        }

        if (aValue && aValueLength) {
            ret = lfs_file_read(lfs, &file, aValue, *aValueLength);
            *aValueLength = ret;
        }
        lfs_file_close(lfs, &file);
    } while(0);

    lfs->cfg->unlock(lfs->cfg);

    return ret >= LFS_ERR_OK ? OT_ERROR_NONE: OT_ERROR_NOT_FOUND;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    char         key[OT_MAX_KEY_LEN];
    lfs_file_t   file;
    int          ret = LFS_ERR_OK;

    OT_UNUSED_VARIABLE(aInstance);

    configASSERT(lfs != NULL);

    if (aInstance) {
        snprintf(key, OT_MAX_KEY_LEN, "%s/ot-%04x", OPENTHREAD_LFS_NAMESPACE, aKey);
    }
    else {
        snprintf(key, OT_MAX_KEY_LEN, "%s/_nl%04x", OPENTHREAD_LFS_NAMESPACE, aKey);
    }

    lfs->cfg->lock(lfs->cfg);

    do {
        ret = lfs_file_open(lfs, &file, key, LFS_O_CREAT | LFS_O_RDWR);
        if (ret != LFS_ERR_OK) {
            break;
        }

        if (aValue && aValueLength) {
            ret = lfs_file_write(lfs, &file, aValue, aValueLength);
        }
        lfs_file_close(lfs, &file);
    } while(0);

    lfs->cfg->unlock(lfs->cfg);

    return ret >= LFS_ERR_OK ? OT_ERROR_NONE: OT_ERROR_FAILED;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    return otPlatSettingsSet(aInstance, aKey, aValue, aValueLength);
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    char         key[OT_MAX_KEY_LEN];
    int          ret;

    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aIndex);

    if (aInstance) {
        snprintf(key, OT_MAX_KEY_LEN, "%s/ot-%04x", OPENTHREAD_LFS_NAMESPACE, aKey);
    }
    else {
        snprintf(key, OT_MAX_KEY_LEN, "%s/_nl%04x", OPENTHREAD_LFS_NAMESPACE, aKey);
    }
    
    ret = lfs_remove(lfs, key);
    return (ret >= LFS_ERR_OK || ret == LFS_ERR_NOENT) ? OT_ERROR_NONE: OT_ERROR_FAILED;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    char             key[OT_MAX_KEY_LEN];
    lfs_file_t       file;
    int              ret = LFS_ERR_OK;
    lfs_dir_t        dir = {};
    struct lfs_info  info = {};

    OT_UNUSED_VARIABLE(aInstance);

    configASSERT(lfs != NULL);

    lfs->cfg->lock(lfs->cfg);

    do {
        ret = lfs_file_open(lfs, &file, OPENTHREAD_LFS_WIPE_FLAG, LFS_O_CREAT | LFS_O_RDWR);
        if (ret != LFS_ERR_OK) {
            break;
        }
        lfs_file_close(lfs, &file);

        ret = lfs_dir_open(lfs, &dir, OPENTHREAD_LFS_NAMESPACE);
        if (ret < LFS_ERR_OK) {
            break;
        }

        while (1) {
            ret = lfs_dir_read(lfs, &dir, &info);
            if (ret <= LFS_ERR_OK) {
                break;
            }

            if (info.type != LFS_TYPE_REG) {
                continue;
            }

            ret = snprintf(key, OT_MAX_KEY_LEN, "%s/%s", OPENTHREAD_LFS_NAMESPACE, info.name);
            if (ret > OT_MAX_KEY_LEN) {
                continue;
            }

            ret = lfs_remove(lfs, key); 
            if (ret < LFS_ERR_OK) {
                break;
            }
        }

        lfs_dir_close(lfs, &dir);

        lfs_remove(lfs, OPENTHREAD_LFS_WIPE_FLAG);
    } while(0);

    lfs->cfg->unlock(lfs->cfg);
}

void otPlatSettingsDeinit(otInstance *aInstance) {}
