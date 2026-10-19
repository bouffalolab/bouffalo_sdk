#ifndef __DBI_DISP_H__
#define __DBI_DISP_H__

#include <stdint.h>

/* info display */
#define LCD_INFO_DISP_ENABLE (1)

uint32_t soln_dbi_disp_get_total_frame_count(void);
void soln_dbi_disp_clear_total_frame_count(void);

int soln_dbi_disp_task_init(void);

#endif /* __DBI_DISP_H__ */
