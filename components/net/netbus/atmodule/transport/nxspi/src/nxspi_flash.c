

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
#include <bflb_core.h>
#include "bflb_dma.h"
#include "bl616_glb.h"
#include "bflb_clock.h"
#include <bflb_gpio.h>

extern nxspi_desc_t g_nxspi;

extern spi_header_t dn_header;
extern spi_header_t up_header;

extern char *dn_buf;
extern char *up_buf;
extern void spi_start_timeout_handler(void *arg);
extern void nxspi_task_entry(void *arg);

extern struct bflb_dma_channel_lli_pool_s txlli_pool[4];
extern struct bflb_dma_channel_lli_pool_s rxlli_pool[4];

extern trans_desc_t dnmsg_desc[NXBD_ITEMS];
extern trans_desc_t upmsg_desc[NXBD_ITEMS];

int nxspi_fakewrite_forread(uint8_t *buf, uint16_t len, uint32_t timeout)
{
    trans_desc_t *msg;
    int total_sent = 0;  // Track the total number of bytes successfully written
    int chunk_size = 0;
    BaseType_t result;

    // Check for invalid arguments
    if (buf == NULL || len == 0) {
        NX_LOGW("arg err.\r\n");
        return -1;  // Invalid buffer or length
    }

    while (len > 0) {
        // Wait for an available descriptor from the free queue
        result = xQueueReceive(g_nxspi.dnfq, &msg, timeout);
        if (result != pdPASS) {
            break;
        }

        // Determine the size of the current chunk to write
        if (len > NXBD_MTU) {
            chunk_size = NXBD_MTU;
            len -= NXBD_MTU;
        } else {
            chunk_size = len;
            len = 0;  // All data written
        }

        NX_LOGD("%p, %p, %d.\r\n", msg->payload, buf, chunk_size);
        // Copy the chunk of data to the descriptor's payload
        memcpy(msg->payload, buf, chunk_size);
        msg->len = chunk_size;
        buf += chunk_size;  // Move the buffer pointer forward by the written chunk size

        // Send the message to the queue for transmission
        while (xQueueSend(g_nxspi.dnat, &msg, portMAX_DELAY) != pdPASS);

        //
        total_sent += chunk_size;
    }
    return total_sent;
}

static int _init_queue(void)
{
    g_nxspi.dnfq = xQueueCreate(NXBD_ITEMS + 1, sizeof(trans_desc_t *));
    g_nxspi.upvq = xQueueCreate(NXBD_ITEMS + 1, sizeof(trans_desc_t *));
    g_nxspi.upfq = xQueueCreate(NXBD_ITEMS + 1, sizeof(trans_desc_t *));

    g_nxspi.dnat = xQueueCreate(NXBD_ITEMS + 1, sizeof(trans_desc_t *));
    g_nxspi.dnnet = xQueueCreate(NXBD_ITEMS + 1, sizeof(trans_desc_t *));
    g_nxspi.dndef = xQueueCreate(NXBD_ITEMS + 1, sizeof(trans_desc_t *));

    if (!g_nxspi.dnfq || !g_nxspi.upvq || !g_nxspi.upfq || !g_nxspi.dnat || !g_nxspi.dnnet || !g_nxspi.dndef) {
        NX_LOGE("failed to create queue\r\n");
        return -1;
    }
#if (NXSPI_BUFMALLOC)
    dn_buf = malloc_aligned_with_padding_nocache(NXBD_MTU * NXBD_ITEMS, 32);
    up_buf = malloc_aligned_with_padding_nocache(NXBD_MTU * NXBD_ITEMS, 32);
    if (NULL == dn_buf || NULL == up_buf) {
        NX_LOGE("failed to mem\r\n");
        return -1;
    }
#else
    dn_buf = (char *)(((uint32_t)dn_buf & (~0x60000000)) | (0x20000000));
    up_buf = (char *)(((uint32_t)up_buf & (~0x60000000)) | (0x20000000));
#endif
    for (int i = 0; i < NXBD_ITEMS; i++) {
        trans_desc_t *msg;
        dnmsg_desc[i].len = 0;
        dnmsg_desc[i].payload = dn_buf + NXBD_MTU*i;
        msg = &(dnmsg_desc[i]);
        memset(dnmsg_desc[i].payload, 0xFF, NXBD_MTU);

        xQueueSend(g_nxspi.dnfq, &msg, portMAX_DELAY);
        NX_LOGI("g_nxspi.dnfq:%p, dnfq %d-payload+%p\r\n", g_nxspi.dnfq, i, dnmsg_desc[i].payload);
    }

    for (int i = 0; i < NXBD_ITEMS; i++) {
        trans_desc_t *msg;
        upmsg_desc[i].len = 0;
        upmsg_desc[i].payload = up_buf + NXBD_MTU*i;
        msg = &(upmsg_desc[i]);
        memset(upmsg_desc[i].payload, 0xFF, NXBD_MTU);

        xQueueSend(g_nxspi.upfq, &msg, portMAX_DELAY);
        NX_LOGI("g_nxspi.upfq:%p, upfq %d-payload+%p\r\n", g_nxspi.upfq, i, upmsg_desc[i].payload);
    }

    return 0;
}

extern void __spihddelay_cb_isr(int irq, void *arg);
int nxspi_init(void)
{
    int ret;

    /* val */
    memset(&g_nxspi, 0, sizeof(g_nxspi));
    memset(&txlli_pool, 0 , sizeof(txlli_pool));
    memset(&rxlli_pool, 0 , sizeof(rxlli_pool));
    memset(&dn_header, 0 , sizeof(spi_header_t));
    memset(&up_header, 0 , sizeof(spi_header_t));
    // up
    up_header.magic  = 0x55AA;
    up_header.len    = 0;

    /* init queue */
    if (_init_queue()) {
        NX_LOGD("failed to create queue, %ld\r\n", ret);
        return -1;
    }
    NX_LOGI("_init_queue\r\n");

    g_nxspi.timer = xTimerCreate("s2c",
            pdMS_TO_TICKS(NXSPI_START_TIMEOUT),     // per 1 S
            pdFALSE,                                // auto reload
            NULL,
            spi_start_timeout_handler);

    g_nxspi.cfg_starttime = 0;
    g_nxspi.cfg_endtime = 0;
    g_nxspi.cfg_usetime = 0;
    PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE();
    g_nxspi.timer0 = bflb_device_get_by_name("timer0");
    bflb_irq_attach(g_nxspi.timer0->irq_num, __spihddelay_cb_isr, NULL);

    /* init task */
    ret = xTaskCreate(nxspi_task_entry, "nxspi", 1024/4, NULL, 29, &g_nxspi.task_hdl);//pri: 0(lowpri)->30(highpri)
    if (ret != pdPASS) {
        NX_LOGE("failed to create spisync task, %ld\r\n", ret);
        return -1;
    }

    return 0;
}

