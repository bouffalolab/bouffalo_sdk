#include "sysinit.h"

extern const struct sysinit_item __sysinit_array_start[];
extern const struct sysinit_item __sysinit_array_stop[];

#ifdef CONFIG_SYSINIT_DEBUG
extern void early_printf(const char *fmt, ...);
#endif

void sysinit_run_all(void)
{
    const struct sysinit_item *p;

    for (p = __sysinit_array_start; p < __sysinit_array_stop; p++) {
        if (p->func) {
#ifdef CONFIG_SYSINIT_DEBUG
            early_printf("SYSINIT call %s: %p(%p)\n", p->name, p->func, p->arg);
#endif
            p->func(p->arg);
        }
    }
}
