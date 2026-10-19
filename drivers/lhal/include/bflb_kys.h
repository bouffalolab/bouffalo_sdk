#ifndef _BFLB_KYS_H
#define _BFLB_KYS_H

#include "bflb_core.h"

struct bflb_kys_config_s {
    const uint8_t *row_pins;   /* Pins of rows */
    const uint8_t *col_pins;   /* Pins of columns */
    uint8_t row_num;           /* Number of rows, max:8 */
    uint8_t col_num;           /* Number of columns, max:8 */
    uint8_t dummy_pin;         /* Select an unused pin from 0 to 31 as the dummy pin */
    uint8_t deglitch_en;       /* Enable deglitch function */
    uint8_t deglitch_cnt;      /* Deglitch count */
    uint8_t idle_duration;     /* Idle duration between column scans */
    uint8_t ghost_en;          /* Enable ghost key event detection */
    uint8_t kyd_en;            /* Enable pds wakeup detection */
};

struct bflb_kys_result_s {
    uint8_t key_num;       /* Number of pressed keys */
    uint8_t key_code[8];   /* Key code array of pressed keys */
    uint8_t row_idx[8];    /* Row index array of pressed keys */
    uint8_t col_idx[8];    /* Column index array of pressed keys */
};

struct bflb_kys_wakeup_info_s {
    uint8_t key_event;
    uint8_t key_code;
    uint8_t row_idx;
    uint8_t col_idx;
};

/* keyscan interrupt enable define */
#define KEYSCAN_INT_EN_DONE          (0x1 << 7)
#define KEYSCAN_INT_EN_FIFOFULL      (0x1 << 8)
#define KEYSCAN_INT_EN_FIFOHALF      (0x1 << 9)
#define KEYSCAN_INT_EN_FIFOQUARTER   (0x1 << 10)
#define KEYSCAN_INT_EN_FIFO_NONEMPTY (0x1 << 11)
#define KEYSCAN_INT_EN_GHOST         (0x1 << 12)

/* keyscan interrupt status define */
#define KEYSCAN_INT_STS_DONE          (0x1 << 7)
#define KEYSCAN_INT_STS_FIFOFULL      (0x1 << 8)
#define KEYSCAN_INT_STS_FIFOHALF      (0x1 << 9)
#define KEYSCAN_INT_STS_FIFOQUARTER   (0x1 << 10)
#define KEYSCAN_INT_STS_FIFO_NONEMPTY (0x1 << 11)
#define KEYSCAN_INT_STS_GHOST         (0x1 << 12)

/* keyscan interrupt clear */
#define KEYSCAN_INT_CLR_DONE  (0x1 << 7)
#define KEYSCAN_INT_CLR_FIFO  (0x1 << 8)
#define KEYSCAN_INT_CLR_GHOST (0x1 << 12)

/* keyscan wakeup key event */
#define KEYSCAN_WAKEUP_KEY_EVENT_NONE    (0)
#define KEYSCAN_WAKEUP_KEY_EVENT_PRESS   (1)
#define KEYSCAN_WAKEUP_KEY_EVENT_RELEASE (2)

#ifdef __cplusplus
extern "C" {
#endif

void bflb_kys_init(struct bflb_device_s *dev, const struct bflb_kys_config_s *config);
void bflb_kys_enable(struct bflb_device_s *dev);
void bflb_kys_disable(struct bflb_device_s *dev);
void bflb_kys_int_enable(struct bflb_device_s *dev, uint32_t flag, bool enable);
void bflb_kys_int_clear(struct bflb_device_s *dev, uint32_t flag);
uint32_t bflb_kys_get_int_status(struct bflb_device_s *dev);
void bflb_kys_get_fifo_info(struct bflb_device_s *dev, uint8_t *fifo_head, uint8_t *fifo_tail, uint8_t *fifo_valid_cnt);
uint8_t bflb_kys_read_keyvalue(struct bflb_device_s *dev, uint8_t index);

int bflb_kys_trigger_poll(struct bflb_device_s *dev, struct bflb_kys_result_s *result, uint32_t timeout_ms);
int bflb_kys_trigger_interrupt(struct bflb_device_s *dev);
void bflb_kys_get_result(struct bflb_device_s *dev, struct bflb_kys_result_s *result);
void bflb_kys_abort(struct bflb_device_s *dev);

void bflb_kys_set_white_keys(uint8_t white_key_num, uint8_t row_idx[], uint8_t col_idx[]);
void bflb_kys_get_wakeup_info(struct bflb_kys_wakeup_info_s *wakeup_info);
void bflb_kys_clear_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif
