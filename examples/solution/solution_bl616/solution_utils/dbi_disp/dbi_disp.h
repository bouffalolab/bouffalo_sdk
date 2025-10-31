#ifndef __DBI_DISP_H__
#define __DBI_DISP_H__

#define DISP_EVENT_FREE_QUEUE  (0x01 << 0)
#define DISP_EVENT_ENABLE_DISP (0x01 << 1)
#define DISP_EVENT_TE_TRIG     (0x01 << 2)

/* info display */
#define LCD_INFO_DISP_ENABLE   (1)

/* backlight pin */
#if !defined(LCD_BACKLIGHT_PWM_EN) || !defined(LCD_BACKLIGHT_PWM_PIN)
#define LCD_BACKLIGHT_PWM_EN 0
#endif

/* TE enable */
#if !defined(LCD_TE_ENABLE) || !defined(LCD_TE_PIN)
#define LCD_TE_ENABLE 0
#endif

extern uint32_t g_dbi_disp_total_frame_cnt;

int dbi_disp_task_init(void);

#endif /* __DVP_MJPEG_H__ */