#ifndef _AUADC_H
#define _AUADC_H

#include <stdint.h>

#define AUADC_DMA_NUME           "dma0_ch0"
#define AUADC_DMA_AUTO_DELE_EN   (1)

#define AUADC_TASK_PRIORITY_MAIN (22)

#define AUADC_FRAME_SIZE         (640)

int audio_record_rate_convert_task_init(void);
int audio_record_rate_convert_push(audio_in_frame_t *auadc_frame_info, uint32_t timeout);

int audio_record_task_init(void);

#endif