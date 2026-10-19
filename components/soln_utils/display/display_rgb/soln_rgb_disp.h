#ifndef __RGB_DISP_H__
#define __RGB_DISP_H__

#include <stdint.h>

/* info display */
#define LCD_INFO_DISP_ENABLE (1)

uint32_t soln_rgb_disp_get_total_frame_count(void);
void soln_rgb_disp_clear_total_frame_count(void);

int soln_rgb_disp_task_init(void);

#endif /* __RGB_DISP_H__ */
