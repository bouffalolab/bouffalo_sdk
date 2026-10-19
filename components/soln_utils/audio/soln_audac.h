#ifndef _AUDAC_H
#define _AUDAC_H

#include "fbq_core.h"

#define AUDAC_DMA_NUME           "dma0_ch1"
#define AUDAC_DMA_AUTO_DELE_EN   (1)

#define AUDAC_TASK_PRIORITY_MAIN (25)

int soln_aud_play_rate_convert_task_init(void);
/** Transfer one producer reference to the rate-conversion queue on success. */
int soln_aud_play_rate_convert_push(fbq_elem_t *elem);

int soln_aud_play_task_init(void);

#endif
