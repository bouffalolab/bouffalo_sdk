#ifndef _AUADC_H
#define _AUADC_H

#include <stdint.h>

#include "fbq_core.h"

#define AUADC_DMA_NUME           "dma0_ch0"
#define AUADC_DMA_AUTO_DELE_EN   (1)

#define AUADC_TASK_PRIORITY_MAIN (22)

#define AUADC_FRAME_SIZE         (640)

int soln_aud_record_rate_convert_task_init(void);
/** Transfer one producer reference to the rate-conversion queue on success. */
int soln_aud_record_rate_convert_push(fbq_elem_t *elem, uint32_t timeout);

int soln_aud_record_task_init(void);

#endif