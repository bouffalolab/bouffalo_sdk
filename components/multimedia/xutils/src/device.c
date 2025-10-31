/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <msp/kernel.h>
#include <xutils/driver.h>
#include <xutils/debug.h>

#ifndef CONFIG_MAX_DEV_COUNT
#define CONFIG_MAX_DEV_COUNT 8
#endif

static msp_slist_t device_list;

#define CONFIG_DEVICE_THREAD_SAFE 1

#ifdef CONFIG_DEVICE_THREAD_SAFE
static msp_mutex_t device_mutex;

#define LIST_LOCK()                                                                                \
    do {                                                                                           \
        if (!msp_mutex_is_valid(&device_mutex))                                                    \
            msp_mutex_new(&device_mutex);                                                          \
        msp_mutex_lock(&device_mutex, MSP_WAIT_FOREVER);                                           \
    } while (0)

#define LIST_UNLOCK()                                                                              \
    do {                                                                                           \
        msp_mutex_unlock(&device_mutex);                                                           \
    } while (0)

#else

#define LIST_LOCK()
#define LIST_UNLOCK()

#endif

msp_dev_t *msp_device_new(msp_driver_t *drv, int size, int id)
{
    msp_dev_t *dev = (msp_dev_t*)msp_zalloc(size);

    if (dev) {
        dev->id = id;
        dev->drv = drv;

        if (msp_mutex_new(&dev->mutex) != 0) {
            msp_free(dev);
            return NULL;
        }
    }

    return dev;
}

void msp_device_free(msp_dev_t *dev)
{
    msp_mutex_free(&dev->mutex);
    msp_free(dev);
}

int msp_device_lock(msp_dev_t *dev)
{
    return msp_mutex_lock(&dev->mutex, MSP_WAIT_FOREVER);
}

int msp_device_unlock(msp_dev_t *dev)
{
    return msp_mutex_unlock(&dev->mutex);
}

static int msp_alloc_device_id()
{
    uint32_t index = rand() & 0xFFFF;

    LIST_LOCK();
    while (1) {
        index++;

        int found = 0;
        msp_dev_t *node;
        msp_slist_for_each_entry(&device_list, node,  msp_dev_t, node) {
            msp_driver_t *drv = (msp_driver_t *)node->drv;
            msp_check_param(drv);
            if (drv->device_id == index << 16) {
                found = 1;
                break;
            }
        }

        if (found == 0)
            break;
    }
    LIST_UNLOCK();

    return index << 16;
}

int msp_driver_register(msp_driver_t *drv, void *config, int idx)
{
    msp_check_param(drv && drv->init);

    /* name exist check */
    if (msp_device_find(drv->name, idx) != NULL)
        return -1;

    msp_dev_t *dev = drv->init(drv, config, idx);

    if (dev) {
        dev->id = idx;
        ((msp_driver_t *)(dev->drv))->device_id = msp_alloc_device_id();
        LIST_LOCK();
        msp_slist_add_tail(&dev->node, &device_list);
        LIST_UNLOCK();

        return 0;
    }

    return -1;
}

msp_dev_t *msp_device_find(const char *name, int id)
{
    msp_dev_t *node;
    LIST_LOCK();
    msp_slist_for_each_entry(&device_list, node,  msp_dev_t, node) {
        msp_driver_t *drv = (msp_driver_t *)node->drv;
        msp_assert(drv);
        if (strcmp(drv->name, name) == 0 && node->id == id)
            break;
    }
    LIST_UNLOCK();

    return node;
}

static char *msp_dev_name_parse(const char *name, int *id)
{
    char *n = strdup(name);

    if (n == NULL)
        return NULL;

    int len = strlen(n);

    for (int i = 0; i < len; i++) {
        if (n[i] >= '0' && n[i] <= '9') {
            *id = atoi(n + i);
            n[i] = '\0';
            break;
        }
    }

    return n;
}

int msp_driver_unregister(const char *name)
{
    int id = 0;

    char *n = msp_dev_name_parse(name, &id);
    if (n == NULL)
        return -ENOMEM;

    msp_dev_t *dev = msp_device_find(n, id);
    msp_free(n);

    if (dev) {
        msp_driver_t *drv = (msp_driver_t *)dev->drv;
        msp_assert(drv);
        if (drv->uninit)
            drv->uninit(dev);

        LIST_LOCK();
        msp_slist_del(&dev->node, &device_list);
        LIST_UNLOCK();

        return 0;
    }

    return -1;
}

msp_dev_t *msp_device_open(const char *name)
{
    int id = 0;

    char *n = msp_dev_name_parse(name, &id);
    if (n != NULL) {
        msp_dev_t *dev = msp_device_open_id(n, id);
        msp_free(n);
        return dev;
    }

    return NULL;
}

msp_dev_t *msp_device_open_id(const char *name, int id)
{
    msp_dev_t *dev = msp_device_find(name, id);

    if (dev) {
        msp_device_lock(dev);
        if (dev->ref == 0) {
            if (DRIVER(dev)->open(dev) != 0) {
                msp_device_unlock(dev);
                return NULL;
            }
        }
        dev->ref++;
        msp_device_unlock(dev);
    }

    return dev;
}

int msp_device_close(msp_dev_t *dev)
{
    int ret = -EBADF;

    if (dev) {
        msp_device_lock(dev);
        if (dev->ref > 0) {
            dev->ref--;
            if (dev->ref == 0)
                ret = DRIVER(dev)->close(dev);
        }
        msp_device_unlock(dev);
    }

    return ret;
}

int msp_device_valid(msp_dev_t *dev, const char *name)
{
    if (dev && dev->drv) {
        const char *n = NULL;
        msp_driver_t *drv = (msp_driver_t*)dev->drv;
        if (drv->type)
            n = drv->type;
        else
            n = drv->name;

        if (n)
            return strcmp(n, name);
    }

    return -1;
}

int msp_device_tags(msp_dev_t *dev)
{
    msp_driver_t *drv = (msp_driver_t*)dev->drv;
    msp_check_param(drv);

    return drv->device_id;
}

int msp_device_is_busy(void)
{
    msp_dev_t *node;
    int busy = 0;

    LIST_LOCK();
    msp_slist_for_each_entry(&device_list, node,  msp_dev_t, node) {
        if (node->busy == 1) {
            busy = 1;
            break;
        }
    }
    LIST_UNLOCK();

    return busy;
}

int msp_device_lpm(msp_dev_t *dev, int pm_state)
{
    int ret = -EBADF;
#if 0
    if (dev) {
        msp_device_lock(dev);
        msp_driver_t *drv = (msp_driver_t*)dev->drv;

        msp_check_param(drv);
        ret = DRIVER(dev)->lpm(dev, pm_state);
        msp_device_unlock(dev);
    }
#endif
    return ret;
}

void msp_device_manage_power(int pm_state)
{
    msp_dev_t *node;

    LIST_LOCK();
    msp_slist_for_each_entry(&device_list, node,  msp_dev_t, node) {
        msp_driver_t *drv = (msp_driver_t *)node->drv;
        msp_check_param(drv);
        if (drv->lpm) {
            drv->lpm(node, pm_state);
        }
    }
    LIST_UNLOCK();
}

