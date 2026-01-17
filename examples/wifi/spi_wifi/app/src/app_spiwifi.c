
#include <stdio.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bl616_glb.h"
#include "app_pm.h"

#include "rfparam_adapter.h"

#define DBG_TAG "appspiwifi"
#include "log.h"

int app_spiwifi_init(void)
{
    /* RF param init */
    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    /* TCP/IP stack init */
    tcpip_init(NULL, NULL);

    /* Enable easyflash(littlefs) */
    bflb_mtd_init();
    easyflash_init();

    app_wifi_init();
    return 0;
}

