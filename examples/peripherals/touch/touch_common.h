#ifndef TOUCH_COMMON_H
#define TOUCH_COMMON_H

#include "bflb_touch_v2.h"

#define TOUCH_SELF_CAP_CHANNEL_MASK  (0x00EFU) /* CH0~CH3, CH5~CH7; CH4 is the active shield. */
#define TOUCH_MUTUAL_TX_CHANNEL_MASK (0x000FU) /* CH0~CH3 */
#define TOUCH_MUTUAL_RX_CHANNEL_MASK (0x00F0U) /* CH4~CH7 */
#define TOUCH_MUTUAL_CHANNEL_MASK    (0xFFFFU)
#define TOUCH_INT_CLEAR_ALL \
    (TOUCH_V2_INT_DET_MASK | TOUCH_V2_INT_SCAN_LAST_MASK | TOUCH_V2_INT_HOP_FREQ_0_MASK | TOUCH_V2_INT_HOP_FREQ_1_MASK)

#define TOUCH_DETECT_THRESHOLD  (200U)
#define TOUCH_DETECT_HYSTERESIS (3U)
#define TOUCH_IDLE_CAL_DELAY_MS (2000U)
#define TOUCH_SCAN_TIMEOUT_MS   (200U)

struct touch_irq_context_s {
    struct bflb_device_s *dev;
    uint16_t channel_mask;
    volatile uint32_t pending_status;
    struct bflb_touch_v2_channel_data_s channel_data[TOUCH_V2_CHANNEL_MAX];
};

int touch_gpio_init(void);
void touch_print_gpio_map(bool mutual_mode);
void touch_config_init(struct bflb_touch_v2_config_s *config, bool mutual_mode, bool calibration);
void touch_set_thresholds(struct bflb_device_s *touch, uint16_t channel_mask);
void touch_dump_channels(struct bflb_device_s *touch, uint16_t channel_mask, const char *tag);
void touch_irq_handler(int irq, void *arg);
void touch_process_events(struct touch_irq_context_s *context);
int touch_calibrate(struct bflb_device_s *touch, bool mutual_mode);

#endif
