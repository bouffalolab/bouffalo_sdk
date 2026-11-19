/*
 * Compatibility entry and SYSINIT registration for the low-power init.
 *
 * The real initialization is _bl_lp_init() in the per-chip <chip>_lp.c file.
 * SYSINIT calls it once from SI_SUB_PRE_SCHED for LPAPP builds; applications
 * must not call bl_lp_init() themselves.
 */

#include <stddef.h>
#include <stdio.h>

#include "bl_lp_internal.h"
#include "sysinit.h"

#if defined(BL616) || defined(BL616CL) || defined(BL618DG) || (defined(BL702) && defined(CONFIG_PM))

#if defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2
int bl_lp_init(void)
{
    printf("[W] deprecated: %s() is handled by SYSINIT, will be removed in Bouffalo SDK v3\r\n", __func__);

#if IS_ENABLED(CONFIG_SYSINIT) && defined(CONFIG_LPAPP) && CONFIG_LPAPP
    return 0;
#else
    return _bl_lp_init();
#endif
}
#endif /* defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2 */

#if IS_ENABLED(CONFIG_SYSINIT) && defined(CONFIG_LPAPP) && CONFIG_LPAPP
static void bl_lp_sysinit(const void *arg)
{
    (void)arg;
    (void)_bl_lp_init();
}
SYSINIT(bl_lp, SI_SUB_PRE_SCHED, SI_ORDER_FOURTH, bl_lp_sysinit, NULL);
#endif

#endif /* supported LP chips */
