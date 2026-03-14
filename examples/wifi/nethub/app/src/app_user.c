
#include <stdio.h>

#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "rfparam_adapter.h"

#include <nethub.h>

#define DBG_TAG "appuser"
#include "log.h"

/* Forward declarations for external functions */
extern void tcpip_init(void (* tcpip_init_done_fn)(void *arg), void *arg);
extern int easyflash_init(void);
extern int app_wifi_init(void);

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

extern void app_atmodule_init(void);

#ifdef CONFIG_MR_VIRTUALCHAN
extern void app_vchan_init(void);
#endif

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
    easyflash_init();

    /* nethub init */
    nethub_bootstrap();

    /* wifi start */
    app_wifi_init();

#ifdef CONFIG_MR_VIRTUALCHAN
    /* vchan demo init */
    app_vchan_init();
#endif

    return 0;
}
