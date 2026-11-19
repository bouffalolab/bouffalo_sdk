#include <stddef.h>

#include "sysinit.h"

#ifdef CONFIG_SYSINIT
extern const struct sysinit_item __sysinit_array_start[];
extern const struct sysinit_item __sysinit_array_stop[];

#ifdef CONFIG_SYSINIT_DEBUG
extern void early_printf(const char *fmt, ...);
#endif

/* Where the pre-scheduler iteration stopped. The scheduler barrier sets this
 * to the first post-scheduler entry so the continuation task can resume. */
static const struct sysinit_item *sysinit_resume;

static void sysinit_run_items(const struct sysinit_item *p)
{
    for (; p < __sysinit_array_stop; p++) {
        if (p->func) {
#ifdef CONFIG_SYSINIT_DEBUG
            early_printf("SYSINIT call %s: %p(%p)\n", p->name, p->func, p->arg);
#endif
            sysinit_resume = p + 1;
            p->func(p->arg);
        }
    }
}
#endif /* CONFIG_SYSINIT */

void sysinit_run_pre_schedule(void)
{
#ifdef CONFIG_SYSINIT
    sysinit_run_items(__sysinit_array_start);
#endif
}

void sysinit_run_post_schedule(void)
{
#ifdef CONFIG_SYSINIT
    const struct sysinit_item *p = sysinit_resume;

    if (p == NULL || p >= __sysinit_array_stop) {
        return;
    }

    sysinit_resume = NULL;
    sysinit_run_items(p);
#endif
}
