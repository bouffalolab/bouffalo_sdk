#include <stdio.h>

#include <lwip/tcpip.h>

#include <bflb_mtd.h>
#include "lfs_kv.h"
#include <nethub.h>
#include <rfparam_adapter.h>

#include <app_user.h>

#if defined(CONFIG_NETHUB_PROFILE_USB)
#include <app_usb_composite.h>
#endif

#define DBG_TAG "appuser"
#include <log.h>

int app_user_init(void)
{
    int ret;

    if (rfparam_init(0, NULL, 0) != 0) {
        LOG_E("PHY RF init failed!\r\n");
        return -1;
    }

    LOG_I("PHY RF init success!\r\n");

    tcpip_init(NULL, NULL);

    bflb_mtd_init();
    ret = lfs_kv_init();
    if (ret != 0) {
        LOG_E("lfs_kv init failed: %d\r\n", ret);
        return -1;
    }

#if defined(CONFIG_NETHUB_PROFILE_USB)
    ret = app_usb_composite_configure();
    if (ret != 0) {
        LOG_E("USB composite configure failed: %d\r\n", ret);
        return -1;
    }
#endif

    ret = nethub_bootstrap();
    if (ret != 0) {
        LOG_E("nethub bootstrap failed: %d\r\n", ret);
        return -1;
    }

#if defined(CONFIG_NETHUB_PROFILE_USB)
    ret = app_usb_composite_start();
    if (ret != 0) {
        LOG_E("USB composite start failed: %d\r\n", ret);
        return -1;
    }
#endif

    if (app_wifi_init() != 0) {
        return -1;
    }

#ifdef CONFIG_MR_VIRTUALCHAN
    app_vchan_init();
#endif

    return 0;
}
