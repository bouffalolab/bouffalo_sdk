/*
 * C++ runtime support for the freestanding Bouffalo SDK image.
 *
 * The SDK deliberately links without the toolchain startup objects.  In a
 * hosted executable __dso_handle is normally supplied by crtbegin.o, but
 * that object is not present in an SDK image.  libstdc++ uses this handle
 * when registering C++ object destructors with __cxa_atexit.
 */

#include <stddef.h>
#include <stdio.h>

#include "sysinit.h"

/*
 * Keep this definition weak so targets which deliberately remove
 * -nostartfiles can use the stronger definition from crtbegin.o.
 */
void *__dso_handle __attribute__((weak, visibility("hidden"))) = 0;

extern void __libc_init_array(void);

#if IS_ENABLED(CONFIG_COMPAT_V2)
/*
 * The constructor array is owned by SYSINIT now, so the old public entry keeps
 * only the deprecated compatibility behavior: print the unified warning and do
 * not initialize anything.  There is no public header for this entry, so the
 * deprecation is reported at run time only.
 */
void bflb_cxx_runtime_init(void)
{
    printf("[W] deprecated: %s() is handled by SYSINIT, will be removed in Bouffalo SDK v3\r\n", __func__);
}
#endif /* IS_ENABLED(CONFIG_COMPAT_V2) */

/*
 * C++ global constructors must run before any application code, and their
 * initialization depends on the system heap: global new()/malloc() is only
 * usable after the board heap registration.
 *
 * The default BSP registers the heap in board_heap at
 * SI_SUB_DRIVERS/SI_ORDER_SECOND (40.01).  This record runs at
 * SI_SUB_DRIVERS/SI_ORDER_MIDDLE (40.80), so console (40.00), heap (40.01)
 * and board/log service (40.02) are all complete without relying on
 * section-name sorting.  Do not order it before the heap record.  A custom
 * board that registers its heap later than 40.02 must move this record after
 * its heap registration as well.
 *
 * The scheduler barrier (SI_SUB_SCHED) has not been reached, so constructors
 * keep the non-blocking startup-context contract: they must not use tasks,
 * blocking calls, interrupt dependencies or services that are initialized
 * after SI_SUB_DRIVERS.  Board-specific DRIVERS records ordered after MIDDLE
 * (board finish/late) are not part of the constructor dependency set.
 * SYSINIT runs this record exactly once, so no separate idempotency flag is
 * needed.
 */
static void cxx_runtime_sysinit(const void *arg)
{
    (void)arg;

    __libc_init_array();
}
SYSINIT(cxx_runtime, SI_SUB_DRIVERS, SI_ORDER_MIDDLE, cxx_runtime_sysinit, NULL);
