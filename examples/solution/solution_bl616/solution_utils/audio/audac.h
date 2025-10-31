#ifndef _AUDAC_H
#define _AUDAC_H

#define AUDAC_DMA_NUME           "dma0_ch1"
#define AUDAC_DMA_AUTO_DELE_EN   (1)

#define AUDAC_TASK_PRIORITY_MAIN (25)

int audio_play_rate_convert_task_init(void);
int audio_play_rate_convert_push(audio_out_frame_t *audac_frame_info);

int audio_play_task_init(void);

#endif
