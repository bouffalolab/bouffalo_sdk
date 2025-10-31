#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <mem.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include <bl_fw_api.h>
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#include <bflb_irq.h>
#include <bl616_glb.h>
#include <bflb_mtd.h>

#include <rfparam_adapter.h>

#define DBG_TAG "MAIN"
#include <log.h>

int app_usbwifi_init(void)
{
    /* RF param init */
    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    /* TCP/IP stack init */
    tcpip_init(NULL, NULL);

    /* enable wifi clock */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* Enable wifi irq */
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    /* Enable easyflash(littlefs) */
    bflb_mtd_init();
    easyflash_init();
 
    /* romsfs init mount use media factory*/
    // romfs_mount(0x378000);
   
    at_module_init();

    return 0;
}

