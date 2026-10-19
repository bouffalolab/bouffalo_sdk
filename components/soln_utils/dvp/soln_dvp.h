#ifndef __DVP_H__
#define __DVP_H__

#include <stdint.h>

uint32_t soln_dvp_get_total_frame_count(void);
void soln_dvp_clear_total_frame_count(void);

int soln_dvp_cam_task_init(void);

#endif /* __DVP_MJPEG_H__ */
