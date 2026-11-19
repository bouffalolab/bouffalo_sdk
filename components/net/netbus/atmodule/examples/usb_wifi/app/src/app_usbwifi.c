#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include <bl_fw_api.h>
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#include <bflb_irq.h>

#define DBG_TAG "MAIN"
#include <log.h>

/* External function declarations */
extern void app_wifi_init(void);

int app_usbwifi_init(void)
{
    /* romsfs init mount use media factory*/
    // romfs_mount(0x378000);

    app_wifi_init();

    return 0;
}

