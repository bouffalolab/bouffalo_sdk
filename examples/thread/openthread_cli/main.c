
#if defined (BL616)
#include <rfparam_adapter.h>
#endif

#include <bl_sys.h>

#include <bflb_mtd.h>
#if defined (CONFIG_LITTLEFS)
#include <easyflash.h>
#endif

#include <lmac154.h>
#include <zb_timer.h>

#include <openthread_port.h>
#include <openthread/cli.h>

#include "board.h"

static struct bflb_device_s *uart0;

extern void __libc_init_array(void);
#if defined(OT_SERIAL_SHELL)
extern void shell_init_with_task(struct bflb_device_s *shell);
#endif

static void ot_stateChangedCallback(otChangedFlags aFlags, void *aContext) 
{
    char string[OT_IP6_ADDRESS_STRING_SIZE];

    if (OT_CHANGED_THREAD_ROLE & aFlags) {
        otDeviceRole role = otThreadGetDeviceRole(otrGetInstance());
        const otNetifAddress *unicastAddrs = otIp6GetUnicastAddresses(otrGetInstance());

        APP_PRINT ("state_role_changed %ld %s, partition id %d\r\n", zb_timer_get_current_time(), 
                otThreadDeviceRoleToString(role), otThreadGetPartitionId(otrGetInstance()));

        for (const otNetifAddress *addr = unicastAddrs; addr; addr = addr->mNext) {
            otIp6AddressToString(&addr->mAddress, string, sizeof(string));
            APP_PRINT ("%s\r\n", string);
        }
    }

    if (OT_CHANGED_IP6_ADDRESS_ADDED & aFlags) {
        otDeviceRole role = otThreadGetDeviceRole(otrGetInstance());
        const otNetifAddress *unicastAddrs = otIp6GetUnicastAddresses(otrGetInstance());

        APP_PRINT ("state_address_changed %ld %s, partition id %d\r\n", zb_timer_get_current_time(), 
                otThreadDeviceRoleToString(role), otThreadGetPartitionId(otrGetInstance()));

        for (const otNetifAddress *addr = unicastAddrs; addr; addr = addr->mNext) {
            otIp6AddressToString(&addr->mAddress, string, sizeof(string));
            APP_PRINT ("%s\r\n", string);
        }
    }

    if (OT_CHANGED_THREAD_PARTITION_ID & aFlags) {
        APP_PRINT ("st_part %ld %s, partition id %d\r\n", zb_timer_get_current_time(), 
                otThreadDeviceRoleToString(otThreadGetDeviceRole(otrGetInstance())), otThreadGetPartitionId(otrGetInstance()));
    }
}

void otrInitUser(otInstance * instance)
{
    otAppCliInit(instance);

    if (otDatasetIsCommissioned(otrGetInstance())) {
        otIp6SetEnabled(otrGetInstance(), true);
        otThreadSetEnabled(otrGetInstance(), true);
    }
    otSetStateChangedCallback(otrGetInstance(), ot_stateChangedCallback, NULL);
}

void vApplicationTickHook( void )
{
#ifdef BL616
    lmac154_monitor();
#endif
#if CONFIG_LMAC154_LOG
    lmac154_logs_output();
#endif
}

int main(void)
{
    otRadio_opt_t opt;
    
#if !defined(BL702L)
    bl_sys_rstinfo_init();
#endif

    board_init();
    
    bflb_mtd_init();
#if defined (CONFIG_LITTLEFS)
    easyflash_init();
#endif

    configASSERT((configMAX_PRIORITIES > 4));

#if defined(BL616)
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }
#endif
    
    __libc_init_array();

#if CONFIG_LMAC154_LOG
    lmac154_log_init();
#endif

    uart0 = bflb_device_get_by_name("uart0");
#if defined(OT_SERIAL_SHELL)
    shell_init_with_task(uart0);
#elif defined (OT_SERIAL_UART)
    ot_uart_init(uart0);
#else
    #error "No serial interface specified."
#endif

    opt.byte = 0;

    opt.bf.isCoexEnable = false;
#if OPENTHREAD_FTD
    opt.bf.isFtd = true;
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    opt.bf.isLinkMetricEnable = true;
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    opt.bf.isCSLReceiverEnable = true;
#endif
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    opt.bf.isTimeSyncEnable = true;
#endif

    otrStart(opt);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}

