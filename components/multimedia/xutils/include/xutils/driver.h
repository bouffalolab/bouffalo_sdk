/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __XUTILS_DRIVER_H__
#define __XUTILS_DRIVER_H__

#include <stdint.h>
#include <string.h>

#include <xutils/list.h>

#include <xutils/device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _msp_driver_obj msp_driver_t;

struct _msp_driver_obj {
    msp_slist_t node;
    char   *name;
    char   *type;
    int16_t ref;
    int16_t device_id;

    msp_dev_t * (*init)(msp_driver_t *drv, void *config, int id);
    void    (*uninit)(msp_dev_t *dev);
    int     (*lpm)(msp_dev_t *dev, int state);
    int     (*open)(msp_dev_t *dev);
    int     (*close)(msp_dev_t *dev);
};

#define DRIVER(dev)  ((msp_driver_t*)(dev->drv))

/**
  \brief       register device object into device driver framework.
  \param[in]   drv      device driver object
  \param[in]   count    device number
  \return      -1 error, 0 success.
*/
int msp_driver_register(msp_driver_t *drv, void *config, int idx);

/**
  \brief       remove device object from device driver framework.
  \param[in]   name     device name
  \return      -1 error, 0 success
*/
int msp_driver_unregister(const char *name);

/**
  \brief       get device object by device name
  \param[in]   name     device name
  \param[in]   id       device index
  \return      null on error, device object pointer on success
*/
msp_dev_t *msp_device_find(const char *name, int id);

/**
  \brief       allocate new node for new device driver
  \param[in]   drv      device driver object
  \param[in]   size     device driver object size
  \param[in]   id       device node index
  \retrun      null on error, device object pointer on success
*/
msp_dev_t *msp_device_new(msp_driver_t *drv, int size, int id);

/**
  \brief       free a device node
  \param[in]   dev      device driver object
  \return      none
*/
void msp_device_free(msp_dev_t *dev);

/**
  \brief       lock device to prevent to access
  \param[in]   dev      device driver object
  \return      none
*/
int msp_device_lock(msp_dev_t *dev);

/**
  \brief       unlock device to allow to access
  \param[in]   dev      device driver object
  \return      none
*/
int msp_device_unlock(msp_dev_t *dev);

/**
  \brief       valid a device 's name
  \param[in]   dev      device driver object
  \param[in]   name     device name
  \return      =0 success other is error
*/
int msp_device_valid(msp_dev_t *dev, const char *name);

/**
  \brief       get a device 's id
  \param[in]   dev      device driver object
  \return      device 's id
*/
int msp_device_tags(msp_dev_t *dev);

/**
  \brief       device enter(leave)lpm
  \param[in]   dev      device driver object
  \param[in]   state    1:enter lpm 0:leave lpm
  \return      =0 success other is error
*/
int msp_device_lpm(msp_dev_t *dev, int pm_state);

#ifdef __cplusplus
}
#endif

#endif
