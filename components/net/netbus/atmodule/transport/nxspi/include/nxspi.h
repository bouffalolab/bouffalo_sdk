
#ifndef _NXSPI_H__
#define _NXSPI_H__

#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* pin config */
#define NXSPI_GPIO_IRQ  (20)
#define NXSPI_GPIO_CS   (28)//16 28
#define NXSPI_GPIO_CLK  (29)
#define NXSPI_GPIO_MISO (30)
#define NXSPI_GPIO_MOSI (27)

#define GPIO_TIME_ENABLE (0)// debug

#define NXSPI_GPIO_SAFEBLIND        (70+5) // from soc hw + 5(for time)
#define NXSPI_GPIO_SAFEEMPIRICAL    (5)  //According to the empirical values obtained from actual tests
#define NXSPI_GPIO_SAFEDELAY        (NXSPI_GPIO_SAFEBLIND - NXSPI_GPIO_SAFEEMPIRICAL)

/* start TimerOut */
#define NXSPI_START_TIMEOUT     (1000)

/* mem debug */
#define NXSPI_DEBUG             (1)

#ifdef NXSPI_NET
#define NXSPI_BUFMALLOC         (0)
#define NXBD_MTU                (2*1024)
#define NXBD_ITEMS              (4)
#define DMATX_LLIPOOL_CNT       (8) // dma config
#define DMARX_LLIPOOL_CNT       (8) // dma config
#else
#define NXSPI_BUFMALLOC         (0)
#define NXBD_MTU                (6*1024)
#define NXBD_ITEMS              (2)
#define DMATX_LLIPOOL_CNT       (4) // dma config
#define DMARX_LLIPOOL_CNT       (4) // dma config
#endif

/* event  */
#define NTF_TRANS_START         (1<<0) // transaction start
#define NTF_TRANS_COMPLETE      (1<<1) // transaction complete
#define NTF_DATA_READY          (1<<2) // data ready
#define NTF_SPIHD_RECEIVED      (1<<3) // spihd received
#define NTF_CS_LOW              (1<<4) // cs low
#define NTF_LP_EXIT             (1<<5) // lp exit
#define NTF_LP_ENTER            (1<<6) // lp enter
#define NTF_START_TIMEOUT       (1<<7) // lp enter

/* st  */
#define NXSPI_ST_INIT           (0)
#define NXSPI_ST_CONFIGURED     (1)  // Configured
#define NXSPI_ST_TSSTARTED      (2)  // Started
#define NXSPI_ST_HDRECEIVED     (3)  // Header Received
#define NXSPI_ST_DATAREADY      (4)  // Started
#define NXSPI_ST_LPSLEEP        (5)  // Sleeping

#define NXSPI_SPEED      (80000000)
#define NXSPI_MODE       (0)
#define NXSPI_NAME       ("spi0")
#define NXSPI_DMARXCH    ("dma0_ch0")
#define NXSPI_DMATXCH    ("dma0_ch1")
#define NXSPI_3PINMODE   (1)

/* internal */
#define NXSPI_SM_INIT           (0)
#define NXSPI_SM_CONFIGURED     (1)
#define NXSPI_SM_STARTED        (2)
#define NXSPI_SM_HDRECEIVED     (3)
#define NXSPI_SM_COMPLETE       (4)
#define NXSPI_SM_SLEEP          (5)

#define get_smstr(st)        \
    ((st) == NXSPI_SM_INIT ? "init" : \
    (st) == NXSPI_SM_CONFIGURED ? "Config" : \
    (st) == NXSPI_SM_STARTED ? "started" : \
    (st) == NXSPI_SM_HDRECEIVED ? "hdreceived" : \
    (st) == NXSPI_SM_COMPLETE ? "complete" : \
    (st) == NXSPI_SM_SLEEP ? "sleep" : "unknown")

#define get_gpiomodestr(mode)        \
     ((mode) == (GPIO_INT_TRIG_MODE_ASYNC_FALLING_EDGE) ? "falling":\
      (mode) == (GPIO_INT_TRIG_MODE_ASYNC_RISING_EDGE) ? "rising":\
      (mode) == (GPIO_INT_TRIG_MODE_ASYNC_LOW_LEVEL) ? "low":\
      (mode) == (GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL) ? "high" : "unknown")

#if GPIO_TIME_ENABLE
#define NXSPI_GPIO_TIME  (0)// gpio debug
#define NX_MAX_TOP 3
#define NX_MAX_BOTTOM 3

// Structure to hold statistical data
typedef struct {
    double average;
    uint64_t top[NX_MAX_TOP];
    uint64_t bottom[NX_MAX_BOTTOM];
    int count;
    uint64_t sum;
} nx_stats_t;
#endif

/* header */
typedef struct spi_header {
    uint16_t magic;
    uint16_t len;
    uint8_t  version   : 2;
    uint8_t  rx_stall  : 1;
    uint8_t  flags     : 5;
#define NXSPI_TYPE_AT   (0)
#define NXSPI_TYPE_NET  (1)
#define NXSPI_TYPE_DEF  (2)
#define NXSPI_TYPE_HCI  (3)
    uint8_t  type;
    uint16_t rsvd;
} spi_header_t;

/* body */
typedef struct _trans_desc {
    uint32_t     len;
    uint8_t      type;
    uint8_t      resvd[3];
    char         *payload;
} trans_desc_t;

typedef struct _nxspi_desc {
//    QueueHandle_t dnvq;  // download valid queue
    QueueHandle_t dnfq;  // download free queue
    QueueHandle_t upvq;  // up valid queue
    QueueHandle_t upfq;  // up free queue

    QueueHandle_t dnat;
    QueueHandle_t dnnet;
    QueueHandle_t dndef;

    uint64_t cfg_starttime;
    uint64_t cfg_endtime;
    uint32_t cfg_usetime;
    struct bflb_device_s *timer0;
    TimerHandle_t timer; // for start->complete timeout
    TaskHandle_t task_hdl;

    trans_desc_t *upmsg;
    trans_desc_t *dnmsg;

    uint8_t gpio_inited;

    /* debug */
#if NXSPI_DEBUG
    uint32_t write_cnt;
    uint32_t write_bytes;
    uint32_t read_cnt;
    uint32_t read_bytes;

    uint32_t ps_entercnt;
    uint32_t ps_exitcnt;

    uint32_t rx_stall_cnt;
    uint32_t discard_cnt;
    uint32_t discard_bytes;
    uint32_t start2complete_cnt;
#endif

    /* val */
    uint32_t sm;            // state machine

    /* val */
    uint32_t cs_enter_cnt;
    uint32_t ts_start_cnt;
    uint32_t ts_complete_cnt;

    /* debug */
    uint32_t tfsize_after_start;
    uint32_t tfsize_received_hd;
    uint32_t tfsize_complete;

    uint32_t dst_after_start;
    uint32_t dst_received_hd;
    uint32_t dst_complete;

    uint32_t time_start_cnt;
    uint32_t time_isr_cnt;
    uint32_t time_lastcfg;
#if GPIO_TIME_ENABLE
    nx_stats_t stats;
#endif
} nxspi_desc_t;

/* port */
void nxspi_hwspi_init(void (*callback)(void *arg), void *arg);
void nxspi_hwspi_transfer(uint8_t *send, uint8_t *recv, uint16_t len, void *cb, void *arg);

int  nxspi_hwmem_init(void);
void nxspi_hwgpio_init(int pin, void (*callback)(uint8_t pin), uint8_t trig_mode);
void nxspi_hwgpio_high(int pin);
void nxspi_hwgpio_low(int pin);
int  nxspi_hwgpio_status(int pin);

/* api */
int nxspi_init(void);

/* api write */
int nxspi_write(uint8_t type, uint8_t *buf, uint16_t len, uint32_t timeout);
trans_desc_t *nxspi_writebuf_pop(uint8_t type, uint32_t timeout);
void nxspi_writebuf_push(trans_desc_t *msg);

/* api read */
int nxspi_read(uint8_t type, uint8_t *buf, uint16_t len, uint32_t timeout);
trans_desc_t *nxspi_readbuf_pop(uint8_t type, uint32_t timeout);
void nxspi_readbuf_push(trans_desc_t *msg);

/* debug */
int nxspi_fakewrite_forread(uint8_t *buf, uint16_t len, uint32_t timeout);

/* mem manage */
void *malloc_aligned_with_padding(int size, int align_bytes);
void free_aligned_with_padding(void *ptr);
void *malloc_aligned_with_padding_nocache(int size, int align_bytes);
void free_aligned_with_padding_nocache(void *ptr);

/* port */
uint64_t bflb_mtimer_get_time_us(void);
void nxspi_hwgpio_disableirq(void);
uint32_t nx_tick(void);

/* ps */
/*
 * callback when enter sleep
 */
int nxspi_ps_enter(void);
/*
 * callback when exit sleep
 */
int nxspi_ps_exit(void);
/*
 * return: 0-idle, 1-busy(active)
 */
int nxspi_ps_get(void);

#if GPIO_TIME_ENABLE
// Function to initialize the statistics structure
void nx_init_stats(nx_stats_t *stats);
// Function to process a new value
void nx_process_value(nx_stats_t *stats, uint64_t value);
#endif

#endif
