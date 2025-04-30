#ifndef MAIN_H_UPSXCZ5H
#define MAIN_H_UPSXCZ5H

#include "config.h"
#include "ctl_port.h"
#include <linux/mutex.h>

#define BL_DRV_NAME "blwnet"

#define BL_DEVICE_STATUS_DRV_REMOVING  (1 << 0)
#define BL_DEVICE_STATUS_CARD_REMOVING (1 << 1)
#define BL_DEVICE_STATUS_CARD_PRESENT  (1 << 2)
#define BL_DEVICE_STATUS_CARD_DEAD     (1 << 3)

struct bl_device {
    struct mutex mutex;
    u8 status;
    struct bl_eth_device *eth_dev;
};

extern struct bl_device gl_dev;

int bl_register_eth_drv(void);
void bl_unregister_eth_drv(void);

bool bl_card_ok(void);

#endif /* end of include guard: MAIN_H_UPSXCZ5H */
