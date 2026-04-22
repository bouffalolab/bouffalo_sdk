#ifndef __RGB_DISP_H__
#define __RGB_DISP_H__

/* info display */
#define LCD_INFO_DISP_ENABLE   (1)

extern uint32_t g_rgb_disp_total_frame_cnt;

int rgb_disp_task_init(void);

#endif /* __RGB_DISP_H__ */
