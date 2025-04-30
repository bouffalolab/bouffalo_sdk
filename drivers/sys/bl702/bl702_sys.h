#ifndef __BL702_SYS_H__
#define __BL702_SYS_H__

#include <stdint.h>

typedef enum {
    BL_RST_POWER_OFF = 0,
    BL_RST_HARDWARE_WATCHDOG,
    BL_RST_FATAL_EXCEPTION,
    BL_RST_SOFTWARE_WATCHDOG,
    BL_RST_SOFTWARE,
    BL_RST_HBN,
    BL_RST_BOD,
} BL_RST_REASON_E;

BL_RST_REASON_E bl_sys_rstinfo_get(void);
char * bl_sys_rstinfo_getstring(void);
void bl_sys_rstinfo_init(void);
int bl_sys_reset_por(void);
void bl_sys_reset_system(void);
int bl_sys_isxipaddr(uint32_t addr);

#endif
