#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "stream_buffer.h"
#include <event_groups.h>

#include <utils_crc.h>
#include <spisync.h>
#include <spisync_config.h>
#include <ramsync.h>
#include <spisync_port.h>
#include <spisync_internal.h>

#include "mem.h"
#include <shell.h>

extern spisync_t *g_spisync_current;

int spisync_ps_enter(spisync_t *spisync)
{
    if ((NULL == spisync) && (NULL == g_spisync_current)) {
        return -1;
    } else if (NULL == spisync) {
        spisync = g_spisync_current;
    }

    spisync_log("========================== enter  ==========================\r\n");
    spisync->ps_status = 1;
    return 0;
}

int spisync_ps_exit(spisync_t *spisync)
{
    if ((NULL == spisync) && (NULL == g_spisync_current)) {
        return -1;
    } else if (NULL == spisync) {
        spisync = g_spisync_current;
    }

    spisync_log("========================== exit   ==========================\r\n");
    wakeup_pin_init(WAKEUP_PIN, WAKEUP_TRIGMODE);

    spisync->ps_status = 2;
    return 0;
}

int spisync_ps_wakeup(spisync_t *spisync, spisync_wakeuparg_t *arg)
{
    if ((NULL == spisync) && (NULL == g_spisync_current)) {
        return -1;
    } else if (NULL == spisync) {
        spisync = g_spisync_current;
    }

    if (arg) {
        if (arg->wakeup_reason == 0) { // 0-gpio
            // update keepactive
            g_spisync_current->ps_keepactive = 1;
#ifdef LP_APP
            pm_status_update(g_spisync_current->ps_keepactive);
#endif
        }
        spisync_log("========================== wakeup ========================== reason:%s\r\n",
                (arg->wakeup_reason==0)?"gpio":"other");
    } else {
        spisync_log("========================== wakeup ==========================\r\n");
    }

#if 1
    /* reconfig spi data_ready wakeup_pin */
    lramsync_reset(&spisync->hw);
    lramsync_start(&spisync->hw);
    spisync_trace("reconfig spi_pin wakeup_pin data_ready");
    if (spisync->reset_cb) {
        spisync->reset_cb(spisync->reset_arg);
    }
    {
        int wakeup_pin_init(void);
        wakeup_pin_init();
    }
    spisync->ps_status = 0;

    spisync_s2m_init(spisync->config->data_rdy_pin);
#endif

    if ((arg) && (arg->wakeup_reason == 0)) { // 0-gpio
        send_rasingedage_ack();
    }
    //spisync_setevent_internal(spisync, EVT_SPISYNC_RESET_BIT);

    return 0;
}

int spisync_ps_get(spisync_t *spisync)
{
    if ((NULL == spisync) && (NULL == g_spisync_current)) {
        return -1;
    } else if (NULL == spisync) {
        spisync = g_spisync_current;
    }

    return spisync->ps_gpioactive;
}

