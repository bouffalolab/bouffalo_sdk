#include <stdio.h>

#include <lwip/tcpip.h>

#include <bflb_mtd.h>
#include <easyflash.h>
#include <nethub.h>
#include <rfparam_adapter.h>

#include <app_user.h>

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
    ret = easyflash_init();
    if (ret != 0) {
        LOG_E("easyflash init failed: %d\r\n", ret);
        return -1;
    }

    ret = nethub_bootstrap();
    if (ret != 0) {
        LOG_E("nethub bootstrap failed: %d\r\n", ret);
        return -1;
    }

    if (app_wifi_init() != 0) {
        return -1;
    }

#ifdef CONFIG_MR_VIRTUALCHAN
    app_vchan_init();
#endif

    return 0;
}
