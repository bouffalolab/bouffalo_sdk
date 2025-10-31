#ifndef __SPISYNC_INTERNAL_H__
#define __SPISYNC_INTERNAL_H__

/* a<b ? */
#define SEQ_LT(a,b)     ((int16_t)((uint16_t)(a) - (uint16_t)(b)) < 0)
/* a<=b ? */
#define SEQ_LEQ(a,b)    ((int16_t)((uint16_t)(a) - (uint16_t)(b)) <= 0)
/* a>b ? */
#define SEQ_GT(a,b)     ((int16_t)((uint16_t)(a) - (uint16_t)(b)) > 0)
/* a>=b ? */
#define SEQ_GEQ(a,b)    ((int16_t)((uint16_t)(a) - (uint16_t)(b)) >= 0)

/* a<b ? */
#define CLAMP_LT(a,b)     ((int32_t)((uint32_t)(a) - (uint32_t)(b)) < 0)
/* a<=b ? */
#define CLAMP_LEQ(a,b)    ((int32_t)((uint32_t)(a) - (uint32_t)(b)) <= 0)
/* a>b ? */
#define CLAMP_GT(a,b)     ((int32_t)((uint32_t)(a) - (uint32_t)(b)) > 0)
/* a>=b ? */
#define CLAMP_GEQ(a,b)    ((int32_t)((uint32_t)(a) - (uint32_t)(b)) >= 0)

#define UINT32_MIN(a, b)  ((a>b)?b:a)

#define CALC_CLAMP_THRESHOLD  (((((1U << 32) - 1) / (6)) - 6)) // 32bit, clamp_tot_cnt=6
#define CALC_ACKSEQ_THRESHOLD (((((1U << 16) - 1) / (SPISYNC_RXSLOT_COUNT)) - SPISYNC_RXSLOT_COUNT)) // uint16, rx_slat_cnt=SPISYNC_RXSLOT_COUNT

/* enum EVT */
#define EVT_SPISYNC_RESET_BIT       (1<<1)
#define EVT_SPISYNC_CTRL_BIT        (1<<2)
#define EVT_SPISYNC_WRITE_BIT       (1<<3)
#define EVT_SPISYNC_READ_BIT        (1<<4)
#define EVT_SPISYNC_RSYNC_BIT       (1<<5)
#define EVT_SPISYNC_DUMP_BIT        (1<<6)
#define EVT_SPISYNC_ALL_BIT         (EVT_SPISYNC_RESET_BIT | \
                                     EVT_SPISYNC_CTRL_BIT  | \
                                     EVT_SPISYNC_WRITE_BIT | \
                                     EVT_SPISYNC_READ_BIT  | \
                                     EVT_SPISYNC_RSYNC_BIT | \
                                     EVT_SPISYNC_DUMP_BIT)

#define WAKEUP_PIN              (GPIO_PIN_28)
#define WAKEUP_TRIGMODE         (1)//0-GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE 1-GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE

void spisync_setevent_internal(spisync_t *spisync, uint32_t evt);

uint16_t spisync_checksum(void *buf, uint32_t len);
void send_rasingedage_ack(void);

#endif

