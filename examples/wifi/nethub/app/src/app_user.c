
#include <stdio.h>

#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "rfparam_adapter.h"

#include <transportsdio.h>
#include <nethub.h>
#include <nhsdiowifi.h>

#define DBG_TAG "appuser"
#include "log.h"

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

extern void app_atmodule_init(void);

int app_user_init(void)
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

    /* nethub */
    // nethub_init();

    /* nethubsdiowifi init */
    nhsdiowifi_init();
    nhsdiowifi_register_all_interfaces();

    /* AT moudle start */
    app_atmodule_init();

    return 0;
}
