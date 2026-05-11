
#include <FreeRTOS.h>
#include <task.h>

#if defined (BL616)
#include <rfparam_adapter.h>
#include "bl616.h"
#elif defined (BL702L)
#include "bl702l.h"
#elif defined (BL702)
#include "bl702.h"
#endif

#include <bl_sys.h>

#include <bflb_mtd.h>
#include <lmac154.h>
#if defined (BL616) || defined (BL702L)
#include <lmac154_fpt.h>
#endif
#include <zb_timer.h>
#if CONFIG_LMAC154_DBG
#include <lmac154_dbg.h>
#include <shell.h>
#endif

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include <openthread_port.h>
#include <openthread/cli.h>
#if defined(CONFIG_THREAD_TPERF)
#include <ot_tperf.h>
#endif
#include "board.h"

static struct bflb_device_s *uart0;

extern void __libc_init_array(void);
extern void shell_init_with_task(struct bflb_device_s *shell);

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

static void lmac154_app_init(void)
{
    irq_callback lmac154_isr_callback;

    lmac154_init();
    lmac154_enableCoex();
    lmac154_setStd2015Extra(true);
    lmac154_setTxRetry(0);
    lmac154_fptClear();
    lmac154_setEnhAckWaitTime((LMAC154_AIFS + (6 + 39) * 2 + 20) << LMAC154_US_PER_SYMBOL_BITS);
    lmac154_setRxStateWhenIdle(true);

#if defined(BL702L)
    lmac154_setTxRxTransTime(0xA0);
#endif

#if OPENTHREAD_FTD && (defined (BL616) || defined(BL702L))
    lmac154_setFramePendingMode(LMAC154_FPT_ANY);
#endif

    zb_timer_cfg(bflb_mtimer_get_time_us() >> LMAC154_US_PER_SYMBOL_BITS);
    lmac154_disableRx();

    lmac154_isr_callback = (irq_callback)lmac154_getInterruptCallback();
    bflb_irq_attach(M154_INT_IRQn, lmac154_isr_callback, NULL);
    bflb_irq_enable(M154_INT_IRQn);
}

static void prvInitTask(void *pvParameters)
{
    lmac154_app_init();

#if CONFIG_LMAC154_DBG
    lmac154_dbg_trace_init();
#endif

    otrStart();

    vTaskDelete(NULL);
}

void otrInitUser(otInstance * instance)
{
    otAppCliInit(instance);

#if defined(CONFIG_THREAD_TPERF)
    if (tperf_init(instance) == 0) {
        printf("[tperf] Initialized\r\n");
    } else {
        printf("[tperf] WARNING: Initialization failed\r\n");
    }
#endif

    if (otDatasetIsCommissioned(otrGetInstance())) {
        otIp6SetEnabled(otrGetInstance(), true);
        otThreadSetEnabled(otrGetInstance(), true);
    }
    otSetStateChangedCallback(otrGetInstance(), ot_stateChangedCallback, NULL);
}

void vApplicationTickHook( void )
{
#ifdef BL616
    lmac154_monitor(10000);
#endif
#if CONFIG_LMAC154_DBG
    lmac154_dbg_output();
#endif
}

int main(void)
{
#if defined BL616 || defined BL702
    bl_sys_rstinfo_init();
#endif

    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    __libc_init_array();

    bflb_mtd_init();

    configASSERT((configMAX_PRIORITIES > 4));

#if defined(BL616)
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }
#endif

    xTaskCreate(prvInitTask, "init", 1024, NULL, 15, NULL);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}

#if CONFIG_LMAC154_DBG
SHELL_CMD_EXPORT_ALIAS(lmac154_dbg_cli, m154, m154 diagnostic cli.);
#endif

