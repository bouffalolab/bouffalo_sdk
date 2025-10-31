
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <nxspi.h>
#include <nxspi_log.h>

#include <utils_list.h>
#include <shell.h>

#define DBG_TAG "NXSPI"
#include <log.h>
#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include <bflb_core.h>
#include "bflb_dma.h"
#include "bl616_glb.h"
#include "bflb_clock.h"
#include <bflb_gpio.h>

#define NXSPI_ALIGN_BYTES         (4)
#define NXSPI_ALIGN_MASK          (NXSPI_ALIGN_BYTES - 1)
#define NXSPI_GETMAX_LEN(a,b)     (((((a)>(b))?(a):(b)) + NXSPI_ALIGN_MASK) & (~NXSPI_ALIGN_MASK))

#if (NXSPI_BUFMALLOC)
char *dn_buf = NULL;
char *up_buf = NULL;
#else
static uint8_t g_dn_buf[NXBD_ITEMS * NXBD_MTU] __attribute__((section(".ram_lp")));
static uint8_t g_up_buf[NXBD_ITEMS * NXBD_MTU] __attribute__((section(".ram_lp")));
char *dn_buf = g_dn_buf;
char *up_buf = g_up_buf;
#endif

extern struct bflb_device_s *sgpio;

trans_desc_t dnmsg_desc[NXBD_ITEMS] __attribute__((section(".nocache_ram")));// up+dn
trans_desc_t upmsg_desc[NXBD_ITEMS] __attribute__((section(".nocache_ram")));// up+dn

struct bflb_dma_channel_lli_pool_s txlli_pool[4] __attribute__((section(".nocache_ram")));
struct bflb_dma_channel_lli_pool_s rxlli_pool[4] __attribute__((section(".nocache_ram")));
spi_header_t dn_header __attribute__((section(".wifi_ram")));
spi_header_t up_header __attribute__((section(".wifi_ram")));

nxspi_desc_t g_nxspi  __attribute__((section(".nocache_ram")));
volatile uint8_t g_hd_received = 0;
volatile uint8_t g_cs_rising   = 0;

extern struct bflb_device_s *dma0_ch0;
extern struct bflb_device_s *dma0_ch1;

// Handles timeout if SPI fails to reach complete state within 1 second of start
void spi_start_timeout_handler(void *arg)
{
    NX_LOGW("wait timeout after start.\r\n");

    return;//todo: fixme
    /* Set bit */
    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_START_TIMEOUT, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xTaskNotify(g_nxspi.task_hdl, NTF_START_TIMEOUT, eSetBits);
    }
}

// cb when write
void dataready_update_cb(void)
{
    /* arg check */
    if (NULL == g_nxspi.task_hdl) {
        NX_LOGW("hdr:%p\r\n", g_nxspi.task_hdl);
        return;
    }

    /* Set bit */
    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_DATA_READY, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xTaskNotify(g_nxspi.task_hdl, NTF_DATA_READY, eSetBits);
    }
}

void nxspi_state_machine(void)
{
    xTaskNotify(g_nxspi.task_hdl, NTF_DATA_READY, eSetBits);
}

void __spihddelay_cb_isr(int irq, void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    bool status;

    // set gpio low
    status = bflb_timer_get_compint_status(g_nxspi.timer0, TIMER_COMP_ID_0);
    if (status) {
        bflb_timer_compint_clear(g_nxspi.timer0, TIMER_COMP_ID_0);
    }
    bflb_irq_disable(g_nxspi.timer0->irq_num);
    bflb_timer_stop(g_nxspi.timer0);

    g_nxspi.time_isr_cnt += 1;

    // set received
    g_hd_received = 1;
    xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_SPIHD_RECEIVED, eSetBits, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// cb when hdcomplete isr
void nxspi_delay_setgpio_start(uint32_t delay_us);
void __spihdreceived_cb_isr(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint64_t diff;

    /* debug received hd */
    g_nxspi.tfsize_received_hd = ((*(volatile uint32_t *)0x2000C20C)&4095);
    g_nxspi.dst_received_hd    = (*(volatile uint32_t *)0x2000C204);

    g_nxspi.cfg_endtime = bflb_mtimer_get_time_us();
    diff = g_nxspi.cfg_endtime - g_nxspi.cfg_starttime;
    if (diff > NXSPI_GPIO_SAFEDELAY) {
        g_nxspi.cfg_usetime = 0;
    } else {
        g_nxspi.cfg_usetime = NXSPI_GPIO_SAFEDELAY - diff;
    }
    //g_hd_received = 1;
    //xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_SPIHD_RECEIVED, eSetBits, &xHigherPriorityTaskWoken);
    if ((g_nxspi.cfg_usetime > 5) && (g_nxspi.cfg_usetime < NXSPI_GPIO_SAFEDELAY)) {
        g_nxspi.time_start_cnt += 1;
        g_nxspi.time_lastcfg = g_nxspi.cfg_usetime;
        nxspi_delay_setgpio_start(g_nxspi.cfg_usetime);
    } else {
        g_hd_received = 1;
        xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_SPIHD_RECEIVED, eSetBits, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// cs isr
extern uint8_t nx_gpio_trigmode_get(struct bflb_device_s *dev, uint8_t pin);
void __cs_handler_isr(uint8_t pin)
{
    uint8_t mode;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    mode = nx_gpio_trigmode_get(sgpio, NXSPI_GPIO_CS);
    g_nxspi.cs_enter_cnt++;
    NX_LOGD("cs_enter_cnt:%ld, mode:%d\r\n", g_nxspi.cs_enter_cnt, mode);

    /* arg check */
    if ((pin != NXSPI_GPIO_CS) || (NULL == g_nxspi.task_hdl)) {
        NX_LOGW("pin:%d, hdr:%p\r\n", pin, g_nxspi.task_hdl);
        return;
    }

    if (mode == GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL) {
        bflb_irq_disable(sgpio->irq_num);
        NX_LOGD("gpio[complete]disable low_level\r\n");
        xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_CS_LOW, eSetBits, &xHigherPriorityTaskWoken);
    } else if (mode == GPIO_INT_TRIG_MODE_ASYNC_FALLING_EDGE) {
        bflb_irq_disable(sgpio->irq_num);
        g_cs_rising = 1;
        xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_TRANS_COMPLETE, eSetBits, &xHigherPriorityTaskWoken);
    } else {
        NX_LOGE("never here\r\n");
        xTaskNotifyFromISR(g_nxspi.task_hdl, NTF_TRANS_COMPLETE, eSetBits, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// task ts start //todo: set head 0
void __trans_start()
{
    uint8_t      *send = NULL;
    uint8_t      *recv = NULL;
    int res;
#if GPIO_TIME_ENABLE
    uint64_t  func_usetime_us;
#endif

    g_nxspi.ts_start_cnt++;

    up_header.rx_stall = 0;

    /* get dnfq -> g */
    if (xQueueReceive(g_nxspi.dnfq, &g_nxspi.dnmsg, 5) != pdPASS) {
        NX_LOGW("wait rx buf for tx timeout\r\n");
        g_nxspi.rx_stall_cnt++;
        //continue;
        up_header.rx_stall = 1;
        g_nxspi.dnmsg = NULL;
        recv = NULL;
    } else {
        recv = g_nxspi.dnmsg->payload;
    }

    /* get upvq -> g */
    if (xQueueReceive(g_nxspi.upvq, &g_nxspi.upmsg, 0) == pdPASS) {
        up_header.len = g_nxspi.upmsg->len;
        up_header.type = g_nxspi.upmsg->type;
        send = g_nxspi.upmsg->payload;
    } else {
        up_header.len = 0;
        g_nxspi.upmsg = NULL;
        send = NULL;
    }

    /* spi transfer */
#if GPIO_TIME_ENABLE
    nxspi_hwgpio_high(NXSPI_GPIO_TIME);
    func_usetime_us = bflb_mtimer_get_time_us();
#endif
    nxspi_hwspi_init(__spihdreceived_cb_isr, NULL);
    nxspi_hwgpio_init(NXSPI_GPIO_CS, __cs_handler_isr, GPIO_INT_TRIG_MODE_ASYNC_FALLING_EDGE);
    g_nxspi.cfg_starttime = bflb_mtimer_get_time_us();
#if GPIO_TIME_ENABLE
    func_usetime_us = (bflb_mtimer_get_time_us()) - func_usetime_us;
    nx_process_value(&g_nxspi.stats, func_usetime_us);
    nxspi_hwgpio_low(NXSPI_GPIO_TIME);
#endif
    NX_LOGD("gpio[start]enable rising\r\n");
    //NX_LOGD("up_header\r\n");
    g_hd_received = 0;
    g_cs_rising   = 0;

    dn_header.len = NXBD_MTU + 1;// set a err magic, it will changed by dma.
#if 0
    printf("dn_header:%p,magic:%04X,l:0x%04X(%d),v:%d,f-%d,t:%d,rsvd:0x%04x\r\n",
            &dn_header, dn_header.magic, dn_header.len, dn_header.len, dn_header.version,
            dn_header.flags, dn_header.type, dn_header.rsvd);
    printf("up_header:%p,magic:%04X,l:0x%04X(%d),v:%d,f-%d,t:%d,rsvd:0x%04x\r\n",
            &up_header, up_header.magic, up_header.len, up_header.len, up_header.version,
            up_header.flags, up_header.type, up_header.rsvd);
#endif

    nxspi_hwspi_ts(&up_header, &dn_header, sizeof(dn_header),
                send, recv, NXBD_MTU,
                __spihdreceived_cb_isr, NULL);
#if 0
    res = xTimerStart(g_nxspi.timer, NXSPI_START_TIMEOUT);
    if (res != pdPASS) {
        printf("Failed to start timer!\r\n");
    }
#endif

    /* debug after_start */
    g_nxspi.tfsize_after_start = ((*(volatile uint32_t *)0x2000C20C)&4095);
    g_nxspi.dst_after_start    = (*(volatile uint32_t *)0x2000C204);
}

// task ts complete
void __trans_bdcomplete()
{
    QueueHandle_t q;
    bflb_dma_channel_stop(dma0_ch0);
    bflb_dma_channel_stop(dma0_ch1);

    g_nxspi.ts_complete_cnt++;

    /* debug complete */
    g_nxspi.tfsize_complete = ((*(volatile uint32_t *)0x2000C20C)&4095);
    g_nxspi.dst_complete    = (*(volatile uint32_t *)0x2000C204);

    /* g -> upfq */
    if (g_nxspi.upmsg) {
        if (xQueueSend(g_nxspi.upfq, &g_nxspi.upmsg, 0) == pdPASS) {
            NX_LOGD("send %p, len %d ok\r\n", g_nxspi.upmsg->payload, g_nxspi.upmsg->len);
        } else {
            NX_LOGE("send %p, len %d err\r\n", g_nxspi.upmsg->payload, g_nxspi.upmsg->len);
        }
        g_nxspi.upmsg = NULL;
    }

    /* g -> dnvq */
    if (g_nxspi.dnmsg) {
        g_nxspi.dnmsg->len = dn_header.len;
        g_nxspi.dnmsg->type = dn_header.type;
        NX_LOGD(" type:%d, len:%d, rx_stall:%d, %p, %p\r\n",
                g_nxspi.dnmsg->type,
                g_nxspi.dnmsg->len,
                up_header.rx_stall,
                (*(volatile uint32_t *)0x2000C204),
                g_nxspi.dnmsg->payload);
        if ((g_nxspi.dnmsg->len > 0) && (0 == up_header.rx_stall) && ((*(volatile uint32_t *)0x2000C204) != g_nxspi.dnmsg->payload)) {
            if (g_nxspi.dnmsg->type==NXSPI_TYPE_AT) {
                q = g_nxspi.dnat;
            } else if (g_nxspi.dnmsg->type==NXSPI_TYPE_NET) {
                q = g_nxspi.dnnet;
            } else {
                q = g_nxspi.dndef;
            }
            if (xQueueSend(q, &g_nxspi.dnmsg, 0) == pdPASS) {
                NX_LOGI("Recv %p, len %d ok\r\n", g_nxspi.dnmsg->payload, g_nxspi.dnmsg->len);
            } else {
                NX_LOGE("Recv %p, len %d err\r\n", g_nxspi.dnmsg->payload, g_nxspi.dnmsg->len);
            }
        }  else {
            if (xQueueSend(g_nxspi.dnfq, &g_nxspi.dnmsg, 0) == pdPASS) {
                NX_LOGD("Q->dnfq ok\r\n", g_nxspi.dnmsg->payload, g_nxspi.dnmsg->len);
            } else {
                NX_LOGE("Q->dnfq err\r\n", g_nxspi.dnmsg->payload, g_nxspi.dnmsg->len);
            }
        }
        g_nxspi.dnmsg = NULL;
    } else {
        if (dn_header.len) {
            // because user did not process the data in a timely manner
            NX_LOGD("Discard len:%d.\r\n", dn_header.len);
            g_nxspi.discard_cnt += 1;
            g_nxspi.discard_bytes += dn_header.len;
        }
    }
}

int _bdreceived(void)
{
    uint16_t len;

    len = NXSPI_GETMAX_LEN(dn_header.len, up_header.len);

    // check
    if ((len < 0) || (len > NXBD_MTU)) {
        NX_LOGD("early dn_header.len:%d, up_header.len:%d, len:%ld\r\n", dn_header.len, up_header.len, len);
        return 0;
    }

    if ((*(volatile uint32_t *)0x2000C204) == (g_nxspi.dnmsg->payload+len)) {
        return 1;
    } else {
        NX_LOGD("dn_header.len:%d, up_header.len:%d, len:%ld\r\n", dn_header.len, up_header.len, len);
        NX_LOGD("dst 0x%08lX != 0x%08lX\r\n",
            (uint32_t)(*(volatile uint32_t *)0x2000C204),
            (uint32_t)(g_nxspi.dnmsg->payload+(len)));
    }

    return 0;
}

#define NXSPI_GET_QUEUE_WAITING ((0 != uxQueueMessagesWaiting(g_nxspi.upvq))    \
                                 || (0 != uxQueueMessagesWaiting(g_nxspi.dnat)) \
                                 || (0 != uxQueueMessagesWaiting(g_nxspi.dnnet))\
                                 || (0 != uxQueueMessagesWaiting(g_nxspi.dndef))\
                                )
#define NXSPI_GET_CS_ACTIVE     (nxspi_hwgpio_status(NXSPI_GPIO_CS) == 1)

#define NXSPI_SETSM(a)       {NX_LOGT("%s->%s\r\n",get_smstr(g_nxspi.sm),get_smstr(a));g_nxspi.sm = a;continue;}
#define NXSPI_GETQUEUE       (0 != uxQueueMessagesWaiting(g_nxspi.upvq))
#define NXSPI_SETIRQ1        {nxspi_hwgpio_high(NXSPI_GPIO_IRQ);}
#define NXSPI_SETIRQ0        {nxspi_hwgpio_low(NXSPI_GPIO_IRQ);}
#define NXSPI_GET_HDRECEIVED (g_hd_received == 1)
#define NXSPI_GET_BDRECEIVED (_bdreceived())

void state_machine()
{
    int cs;

    while (1) {
        cs = nxspi_hwgpio_status(NXSPI_GPIO_CS);
        if (NXSPI_SM_CONFIGURED ==  g_nxspi.sm) {
            nxspi_hwgpio_init(NXSPI_GPIO_CS, __cs_handler_isr, GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL);
            cs = nxspi_hwgpio_status(NXSPI_GPIO_CS);// get cs after cs init
            if (1 == cs) {
                __trans_start();
                NXSPI_SETIRQ1;
                NXSPI_SETSM(NXSPI_SM_STARTED);
            } else if (0 == cs) {
                if (NXSPI_GETQUEUE)  {
                    __trans_start();
                    NXSPI_SETIRQ1;
                    NXSPI_SETSM(NXSPI_SM_STARTED);
                } else {
                    NXSPI_SETIRQ0;
                }
            }
        } else if (NXSPI_SM_STARTED ==  g_nxspi.sm) {
            NX_LOGD("len:%d, dn:%d, up:%d\r\n",
                    NXSPI_GETMAX_LEN(dn_header.len,up_header.len),
                    dn_header.len, up_header.len);

            if (NXSPI_GET_HDRECEIVED) {
                NXSPI_SETIRQ0;
                NXSPI_SETSM(NXSPI_SM_HDRECEIVED);
            } else if (1 == cs) {
                NXSPI_SETIRQ1;
            } else  if ((0 == cs) && (NXSPI_GET_BDRECEIVED)) {
                g_nxspi.start2complete_cnt += 1;
                NX_LOGW("statemachine start2complete_cnt.\r\n");
#if 0//if 0: disable start to complete
                NXSPI_SETIRQ0;
                NXSPI_SETSM(NXSPI_SM_COMPLETE);
#endif
            }
        } else if (NXSPI_SM_HDRECEIVED ==  g_nxspi.sm) {
            if (0 == cs) {
                NXSPI_SETSM(NXSPI_SM_COMPLETE);
            } else {
                if (g_cs_rising) {
                    g_cs_rising = 0;// change from rising isr
                    NXSPI_SETSM(NXSPI_SM_COMPLETE);
                }
            }
        } else if (NXSPI_SM_COMPLETE ==  g_nxspi.sm) {
#if 0
            xTimerReset(g_nxspi.timer, 0);
            xTimerStop(g_nxspi.timer, 0);
#endif
            // check bd done ?
#if 0
            {
                uint16_t len = NXSPI_GETMAX_LEN(dn_header.len, up_header.len);
                if (((*(volatile uint32_t *)0x2000C204) != (g_nxspi.dnmsg->payload+len)) && ((*(volatile uint32_t *)0x2000C204) != (g_nxspi.dnmsg->payload+len - 4))) {
                    //NX_LOGA("dn_header.len:%d, up_header.len:%d, len:%ld\r\n", dn_header.len, up_header.len, len);
                    NX_LOGD("warn 0x%08lX!=0x%08lX, %d,%d, tsf:%d\r\n",
                        (uint32_t)(*(volatile uint32_t *)0x2000C204), (uint32_t)(g_nxspi.dnmsg->payload+(len)),
                        dn_header.len,
                        up_header.len,
                        ((*(volatile uint32_t *)0x2000C20C)&4095));
                }
            }
#endif
            // handle
            __trans_bdcomplete();
            NXSPI_SETIRQ0;
            NXSPI_SETSM(NXSPI_SM_CONFIGURED);
        } else {
            NX_LOGE("never here.\r\n");
        }
        break;
    }
}

/* header/body process */
void nxspi_task_entry(void *arg)
{
    uint32_t ulNotificationValue;

    g_nxspi.task_hdl = xTaskGetCurrentTaskHandle();
    NX_LOGI("nxspi_task_entry\r\n");

    /* gpio spi init */
    nxspi_hwgpio_init(NXSPI_GPIO_CS, __cs_handler_isr, GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL);
    nxspi_hwspi_init(__spihdreceived_cb_isr, NULL);

#if GPIO_TIME_ENABLE
    bflb_gpio_init(sgpio, NXSPI_GPIO_TIME, GPIO_OUTPUT | GPIO_INPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_3);
    nx_init_stats(&g_nxspi.stats);
#endif

    g_nxspi.sm = NXSPI_SM_CONFIGURED;
    while(1) {
        if (xTaskNotifyWait(0, 0xFFFFFFFF, &ulNotificationValue, portMAX_DELAY) == pdTRUE) {
            state_machine();
        }
    }
}

trans_desc_t *nxspi_writebuf_pop(uint8_t type, uint32_t timeout)
{
    BaseType_t result;
    trans_desc_t *msg;
    result = xQueueReceive(g_nxspi.upfq, &msg, timeout);
    if (result != pdPASS) {
        return NULL;
    }
    msg->type = type;
    return msg;
}

void nxspi_writebuf_push(trans_desc_t *msg)
{
    if (NULL == msg) {
        return;
    }
    while (xQueueSend(g_nxspi.upvq, &msg, portMAX_DELAY) != pdPASS);

    // set dr for send
    dataready_update_cb();

#if NXSPI_DEBUG
    g_nxspi.write_cnt++;
    g_nxspi.write_bytes += msg->len;
#endif
}

int nxspi_write(uint8_t type, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    trans_desc_t *msg;
    BaseType_t result;

    //printf("nxspi_write type:%d, len:%d\r\n", type, len);

    if (len > NXBD_MTU) {
        return -1;
    }

    // get msg from q
    result = xQueueReceive(g_nxspi.upfq, &msg, timeout);
    if (result != pdPASS) {
        return -2;
    }

    // buf to msg, msg to q
    msg->type = type;
    memcpy(msg->payload, buf, len);
    msg->len = len;
    if (len&NXSPI_ALIGN_MASK) {
        for (int i = 0; i < (NXSPI_ALIGN_BYTES - (len&NXSPI_ALIGN_MASK)); i++) {
            msg->payload[len+i] = 0;
        }
    }
    while (xQueueSend(g_nxspi.upvq, &msg, portMAX_DELAY) != pdPASS);

    // set dr for send
    dataready_update_cb();

#if NXSPI_DEBUG
    g_nxspi.write_cnt++;
    g_nxspi.write_bytes += len;
#endif
    return len;
}

trans_desc_t *nxspi_readbuf_pop(uint8_t type, uint32_t timeout)
{
    trans_desc_t *msg;
    BaseType_t result;
    QueueHandle_t q;

    if (type==NXSPI_TYPE_AT) {
        q = g_nxspi.dnat;
    } else if (type==NXSPI_TYPE_NET) {
        q = g_nxspi.dnnet;
    } else {
        q = g_nxspi.dndef;
    }
    result = xQueueReceive(q, &msg, timeout);
    if (result != pdPASS) {
        NX_LOGE("result:%d\r\n", result);
        return NULL;  // Timeout or error in receiving the message
    }

#if NXSPI_DEBUG
    g_nxspi.read_cnt++;
    if (msg->len > 0) {
        g_nxspi.read_bytes += msg->len;
    }
#endif

    return msg;
}

void nxspi_readbuf_push(trans_desc_t *msg)
{
    while (xQueueSend(g_nxspi.dnfq, &msg, portMAX_DELAY) != pdPASS);
}

int nxspi_read(uint8_t type, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    trans_desc_t *msg;
    int res = 0;  // Start with 0 to handle cases where no data is read.
    BaseType_t result;
    QueueHandle_t q;

    if ((NULL == buf) || (0 == len)) {
        NX_LOGD("arg error\r\n");
        return -1;  // Invalid input
    }

    if (type==NXSPI_TYPE_AT) {
        q = g_nxspi.dnat;
    } else if (type==NXSPI_TYPE_NET) {
        q = g_nxspi.dnnet;
    } else {
        q = g_nxspi.dndef;
    }
    // Try to receive a message from the queue
    result = xQueueReceive(q, &msg, timeout);
    if (result != pdPASS) {
        NX_LOGE("result:%d\r\n", result);
        return -2;  // Timeout or error in receiving the message
    }

    // Ensure the received message has valid data
    //NX_LOGD("msg->len:%d\r\n", msg->len);

    // Data can fit within the provided buffer
    res = (msg->len > len) ? len : msg->len;  // Set res to the actual data length
    memcpy(buf, msg->payload, res);

    // Return the message to the free queue
    while (xQueueSend(g_nxspi.dnfq, &msg, portMAX_DELAY) != pdPASS);

#if NXSPI_DEBUG
    g_nxspi.read_cnt++;
    if (res > 0) {
        g_nxspi.read_bytes += res;
    }
#endif

    return res;  // Return the actual number of bytes read
}

/* callback when enter sleep */
int nxspi_ps_enter(void)
{
    g_nxspi.ps_entercnt++;
    xTaskNotify(g_nxspi.task_hdl, NTF_LP_ENTER, eSetBits);
    return 0;
}

/* callback when exit sleep */
int nxspi_ps_exit(void)
{
    g_nxspi.ps_exitcnt++;
    g_nxspi.gpio_inited = 0;

    xTaskNotify(g_nxspi.task_hdl, NTF_LP_EXIT, eSetBits);
    return 0;
}

/* return: 1-busy(active), 0-idle */
int nxspi_ps_get(void)
{
    if ((NXSPI_GET_QUEUE_WAITING) || (NXSPI_SM_CONFIGURED != g_nxspi.sm) || NXSPI_GET_CS_ACTIVE) {
        return 1;
    }
    return 0;
}
