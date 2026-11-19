#include <stdlib.h>
#include <stdio.h>

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "wifi_mgmr_ext.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#define DBG_TAG "appuartwifi"
#include "log.h"

extern void app_wifi_init(void);

int app_uartwifi_init(void)
{
    app_wifi_init();

    return 0;
}

