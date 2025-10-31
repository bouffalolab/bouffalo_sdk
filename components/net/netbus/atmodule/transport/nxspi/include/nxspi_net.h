#ifndef __NXSPI_NET_H__
#define __NXSPI_NET_H__

#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "lwip/netif.h"

#include <nxspi.h>

#define NXBD_UPLD_ITEMS     (20)
#define NXBD_DNLD_ITEMS     (11)
#define TX_PBUF_FRAME_LEN   (1514)
#define TX_PBUF_PAYLOAD_LEN (PBUF_LINK_ENCAPSULATION_HLEN + TX_PBUF_FRAME_LEN)

typedef struct _nettrans_desc {
    struct pbuf_custom pbuf;
    int                payload_len;
    char               *payload_buf;
} nettrans_desc_t;

typedef struct _spinet_desc {
    QueueHandle_t dnfq;  // download free queue
    QueueHandle_t upvq;  // up valid queue

    TaskHandle_t uptask_hdl;
    TaskHandle_t dntask_hdl;

    /* global val */
    nettrans_desc_t *dnmsg;
    struct pbuf *upmsg;
    uint8_t    dbg_dntask_mode;

#define  SPINET_NETSTREAM_TO_HOST   0
#define  SPINET_NETSTREAM_TO_LOCAL  1
    uint8_t netstream;
    uint32_t local_pktcnt;
    uint32_t host_pktcnt;
    uint32_t oth_pktcnt;

#if NXSPI_DEBUG
    uint32_t write_cnt;
    uint32_t write_bytes;
    uint32_t read_cnt;
    uint32_t read_bytes;
#endif
} spinet_t;

void spinet_init(void);

#endif

