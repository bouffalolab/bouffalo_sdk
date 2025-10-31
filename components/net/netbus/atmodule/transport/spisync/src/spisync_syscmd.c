
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

#include "bflb_gpio.h"

#include <shell.h>

#define SYSCMDPRO_TASK_PRI      (20)
#define SYSCMDPRO_STACK_SIZE    (1024)
#define SYSCMD_TASK_PRI         (20)
#define SYSCMD_STACK_SIZE       (1024)

#define SYSCMD_QUEUE_LENGTH     (10)
#define SYSCMD_ITEM_SIZE        sizeof(syscmd_msg_t)

#define SYSCMD_PACKAGE_TYPE_RNM (0x1)
#define SYSCMD_PACKAGE_TYPE_CTL (0x2)

#define SEND_CMD_RASINGEDGE     (0x80000001)
#define RECV_CMD_RASINGEDGE_ACK (0xC0000001)

#define RECV_CMD_M2SSLEEPA      (0x40000002)
#define SEND_CMD_M2SSLEEP_ACKB  (0x80000002)
#define RECV_CMD_M2SSLEEP_ACKC  (0xC0000002)

/// @brief
typedef struct _syscmd_msg {
    uint32_t cmd;
    uint16_t seq;
} syscmd_msg_t;

typedef struct _syscmd_ctx {
    QueueHandle_t q;
    TimerHandle_t timer;
    uint32_t      raisingedge_seq;
} syscmd_ctx_t;

/* =================================================================== */
/* cmd or rnm */

typedef struct _syscmd_ctlpayload {
    uint32_t cmd_id;
    uint16_t cmd_seq;
    uint16_t len;
    uint8_t  data[1];
} syscmd_ctlpayload_t;

typedef struct _syscmd_rnmpayload {
    uint8_t  data[1];
} syscmd_rnmpayload_t;

typedef struct _sycmd_seg {
    uint32_t package_type;
    uint32_t package_len;
    union {
        syscmd_ctlpayload_t ctrl;
        syscmd_rnmpayload_t rnm;
    };
} sycmd_seg_t;

/* =================================================================== */
extern spisync_t *g_spisync_current;
syscmd_ctx_t s_syscmd_ctx;

#ifdef LP_APP
int pm_status_update(int status);
#endif
BaseType_t _syscmd_send(syscmd_ctx_t *ctx, syscmd_msg_t *msg);

void send_rasingedage_ack(void)
{
    syscmd_msg_t msg;

    msg.cmd = SEND_CMD_RASINGEDGE;
    msg.seq = 0;// not used
    _syscmd_send(&s_syscmd_ctx, &msg);
}

void gpio0_isr(uint8_t pin)
{
    static uint32_t isrcnt = 0;

    if (!g_spisync_current) {
        spisync_err("g_spisync_current == NULL\r\n");
        return;
    }

    if (pin == WAKEUP_PIN) {
        spisync_log("gpio isrcnt:%d\r\n", isrcnt++);

        g_spisync_current->ps_keepactive = 1;
#ifdef LP_APP
        pm_status_update(g_spisync_current->ps_keepactive);
#endif

        send_rasingedage_ack();
    }
}

int _wakeup_pin_init(int pin, int trig_mode)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_irq_disable(gpio->irq_num);
    bflb_gpio_init(gpio, pin, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_int_init(gpio, pin, trig_mode);
    bflb_gpio_irq_attach(pin, gpio0_isr);
    bflb_irq_enable(gpio->irq_num);

    return 0;
}

int wakeup_pin_init(void)
{
    _wakeup_pin_init(WAKEUP_PIN, WAKEUP_TRIGMODE);

    return 0;
}

void _s2m_sendcmd(uint16_t seq, uint32_t cmd_id, uint32_t timeout)
{
    spisync_msg_t msg;
    int ret;

#if 0
    sycmd_seg_t buf;

    /* set buf arg */
    buf.package_type = SYSCMD_PACKAGE_TYPE_CTL;
    buf.package_len = 8;   // FIXME: magic number
    buf.ctrl.cmd_id = cmd_id;
    buf.ctrl.cmd_seq = seq;
    buf.ctrl.len = 0;      // FIXME: magic number
#else
    int i;
    uint8_t buf[16] = {0x02,0x00,0x00,0x00,// t
                       0x08,0x00,0x00,0x00,// l
                       0x02,0x00,0x00,0x80,// cmd_id
                       0x00,0x00,// seq
                       0x00,0x00};// len
    i = 8;
    memcpy(buf + i, &cmd_id, 4);
    i += 4;
    memcpy(buf + i, &seq, 2);
#endif

    /* build arg and write */
    SPISYNC_MSGINIT(&msg,
            SPISYNC_TYPEMSG_SYSCTRL,/* type */
            1,                      /* copy */
            timeout,                /* timeout FIXME: default 100ms or portMAX_DELAY */
            buf, 16,                /* buf buf_len */
            NULL,                   /* cb */
            NULL);                  /* cb_arg */

    spisync_buf("<===", &buf, 16);
    ret = spisync_write(g_spisync_current, &msg, 0);
    if (ret <= 0) {
        spisync_err("slave_send_rasingedge ret:%d, drop it.\r\n", ret);
    }
    return ret;
}

int m2s_sleepacka_process(syscmd_ctx_t *ctx, uint16_t seq)
{

    return 0;
}


int m2s_sleepackc_process(syscmd_ctx_t *ctx, uint16_t seq)
{
    // stop time
    spisync_log("--->reset stop timer.\r\n");
    xTimerReset(ctx->timer, 0);
    xTimerStop(ctx->timer, 0);

    return 0;
}

static int _ctl_process(uint8_t *buf, uint32_t len)
{
    syscmd_ctlpayload_t *ctl = buf;

    //spisync_buf("ctl --->", ctl, len);
    spisync_trace("ctl->cmd_id:0x%08lX, len:%d, seq:%d\r\n", ctl->cmd_id, ctl->len, ctl->cmd_seq);
    // FIXME: add check len
    switch (ctl->cmd_id) {
        case RECV_CMD_RASINGEDGE_ACK:
            spisync_log("recv RECV_CMD_RASINGEDGE_ACK ack\r\n");
            break;
        case RECV_CMD_M2SSLEEPA:
            spisync_log("recv RECV_CMD_M2SSLEEPA and start send B\r\n");

            // send cmdb
            _s2m_sendcmd(ctl->cmd_seq, SEND_CMD_M2SSLEEP_ACKB, 0);

            spisync_log("--->start timer.\r\n");
            xTimerStart(s_syscmd_ctx.timer, 100);
            break;
        case RECV_CMD_M2SSLEEP_ACKC:
            spisync_log("recv RECV_CMD_M2SSLEEP_ACKC\r\n");
            m2s_sleepackc_process(&s_syscmd_ctx, ctl->cmd_seq);

            // update keepactive
            g_spisync_current->ps_keepactive = 0;
#ifdef LP_APP
            pm_status_update(g_spisync_current->ps_keepactive);
#endif
            break;
        default :
            break;
    }
    return 0;
}
static int _rnm_process(uint8_t *buf, uint32_t len)
{
    return 0;
}

int syscmd_process(uint8_t *buf, uint32_t len)
{
    sycmd_seg_t *seg = buf;

    // FIXME: add check len
    switch (seg->package_type) {
        case SYSCMD_PACKAGE_TYPE_CTL:
            spisync_trace("seg t:%d, l:%d\r\n", seg->package_type, seg->package_len);
            _ctl_process((uint8_t *)&seg->ctrl, seg->package_len);
            break;
        case SYSCMD_PACKAGE_TYPE_RNM:
            spisync_log("Recv package_type rnm\r\n");
            _rnm_process((uint8_t *)&seg->rnm, seg->package_len);
            break;
        default :
            break;
    }
    return 0;
}

void slave_canentersleep_timer_callback(TimerHandle_t xTimer)
{
    spisync_log("------------------------ Slave module can enter sleep mode\r\n");

    g_spisync_current->ps_keepactive = 0;
#ifdef LP_APP
    pm_status_update(g_spisync_current->ps_keepactive);
#endif
}

int syscmd_processor_task(void *arg)
{
    spisync_msg_t msg;
    uint32_t buffer_size    = 1536;
    uint8_t *buf            = malloc(buffer_size);
    uint32_t timeout        = portMAX_DELAY;
    int res;

    SPISYNC_MSGINIT(&msg,
                SPISYNC_TYPEMSG_SYSCTRL,  /* type */
                0,                        /* copy */
                timeout,                  /* timeout */
                buf, buffer_size,         /* buf buf_len */
                NULL,                     /* cb */
                NULL);                    /* cb_arg */

    while (1) {
        res = spisync_read(g_spisync_current, &msg, 0);
        if (res <=0) {
            continue;
        }
        spisync_trace("spisync read type:%d, len:%d\r\n", msg.type, msg.buf_len);
        spisync_buf("===>", msg.buf, msg.buf_len);
        syscmd_process(buf, msg.buf_len);
    }

    free(buf);
    vTaskDelete(NULL);
}

BaseType_t _syscmd_send(syscmd_ctx_t *ctx, syscmd_msg_t *msg)
{
    if (xPortIsInsideInterrupt()) {
        return xQueueSendFromISR(ctx->q, msg, NULL);
    } else {
        return xQueueSend(ctx->q, msg, portMAX_DELAY);
    }
}

void syscmd_task(void *arg)
{
    uint16_t rasing_edge_seq =  1;
    syscmd_ctx_t *ctx;
    syscmd_msg_t msg;

    ctx = (syscmd_ctx_t *)arg;

    while (1) {
        if (xQueueReceive(ctx->q, &msg, portMAX_DELAY) != pdPASS) {
            continue;
        }
        spisync_trace("Received: cmd = 0x%08lX, seq = %u\n", msg.cmd, msg.seq);
        switch (msg.cmd) {
            case SEND_CMD_RASINGEDGE:
                _s2m_sendcmd(rasing_edge_seq, SEND_CMD_RASINGEDGE, 0);
                rasing_edge_seq++;
                break;
            case SEND_CMD_M2SSLEEP_ACKB:
                _s2m_sendcmd(msg.seq, SEND_CMD_M2SSLEEP_ACKB, 0);
                break;
            default:
                spisync_err("Received: cmd = 0x%08lX, seq = %u\n", msg.cmd, msg.seq);
                break;
        }
    }
}

int syscmd_init(void)
{
    BaseType_t ret;

    memset(&s_syscmd_ctx, 0, sizeof(s_syscmd_ctx));

    s_syscmd_ctx.q = xQueueCreate(SYSCMD_QUEUE_LENGTH, SYSCMD_ITEM_SIZE);
    if (s_syscmd_ctx.q == NULL) {
        spisync_err("Failed to create queue.\n");
        return -1;
    }

    s_syscmd_ctx.timer  = xTimerCreate("delay_rst",
                                pdMS_TO_TICKS(100),         // FIXME: hardcode
                                pdFALSE,                    // auto reload
                                (void *)&s_syscmd_ctx,
                                slave_canentersleep_timer_callback);

    ret = xTaskCreate(syscmd_task,
                    "syscmd",
                    SYSCMD_STACK_SIZE/sizeof(StackType_t),
                    &s_syscmd_ctx,
                    SYSCMD_TASK_PRI,
                    NULL
                    );
    if (ret != pdPASS) {
        spisync_err("failed to create syscmd task, %ld\r\n", ret);
    }

    ret = xTaskCreate(syscmd_processor_task,
                    "syscmdpro",
                    SYSCMDPRO_STACK_SIZE/sizeof(StackType_t),
                    &s_syscmd_ctx,
                    SYSCMDPRO_TASK_PRI,
                    NULL
                    );
    if (ret != pdPASS) {
        spisync_err("failed to create syscmd task, %ld\r\n", ret);
    }

    wakeup_pin_init();

    return 0;
}

int ss_cmd_test(int argc, char **argv)
{

    _s2m_sendcmd(0x11, 0x12345678, 0);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(ss_cmd_test, sst, sscmd test);

