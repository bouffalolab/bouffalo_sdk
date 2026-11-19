#include <stdlib.h>
#include <stdio.h>

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bl616_glb.h"
#include "app_pm.h"

#define DBG_TAG "appspiwifi"
#include "log.h"

int app_spiwifi_init(void)
{
    extern void app_wifi_init(void);
    app_wifi_init();
    return 0;
}

