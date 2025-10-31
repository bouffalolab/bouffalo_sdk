#ifndef __BL616L_SYS_H__
#define __BL616L_SYS_H__

#include <stdint.h>
#include <stdbool.h>

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
int bl_sys_rstinfo_set(BL_RST_REASON_E val);
int bl_sys_rstinfo_getsting(char *info);
char *bl_sys_rstinfo_getstring(void);
void bl_sys_rstinfo_init(void);
int bl_sys_reset_por(void);
void bl_sys_reset_system(void);
int bl_sys_init(void);

void bl_cpu_sysmap_init(bool dcache_preload_en, bool dcache_amr_en);

#endif
