/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __XUTILS_DEVICE_H__
#define __XUTILS_DEVICE_H__

#include <stdint.h>
#include <xutils/list.h>
#include <msp/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_PM_ACTIVE_STATE          1
#define DEVICE_PM_LOW_POWER_STATE       2
#define DEVICE_PM_SUSPEND_STATE         3
#define DEVICE_PM_OFF_STATE             4

typedef struct _msp_dev_obj {
    msp_slist_t     node;
    uint8_t     id;
    uint8_t     busy;
    uint16_t    ref;
    msp_mutex_t mutex;
    void        *drv;
    void        *config;
} msp_dev_t;

/**
  \brief       open one device
  \param[in]   name     device name
  \param[in]   id       device index to be register
  \return      null on error, device object pointer on success
*/
msp_dev_t *msp_device_open(const char *name);

msp_dev_t *msp_device_open_id(const char *name, int id);

/**
  \brief       close one device
  \param[in]   dev      device name
  \return      <0 error, >0 return device close error
*/
int msp_device_close(msp_dev_t *dev);

int msp_device_is_busy(void);
void msp_device_manage_power(int pm_state);

#ifdef __cplusplus
}
#endif

#endif
