#include "tickless_hook.h"

static struct tickless_hooks tickless_hooks;

int tickless_hooks_register(bool (*allow_enter)(void *arg),
                            void *allow_enter_arg,
                            bool (*prepare_enter)(void *arg),
                            void *prepare_enter_arg,
                            bool (*after_resume)(void *arg),
                            void *after_resume_arg)
{
    tickless_hooks.allow_enter = allow_enter;
    tickless_hooks.allow_enter_arg = allow_enter_arg;
    tickless_hooks.prepare_enter = prepare_enter;
    tickless_hooks.prepare_enter_arg = prepare_enter_arg;
    tickless_hooks.after_resume = after_resume;
    tickless_hooks.after_resume_arg = after_resume_arg;

    return 0;
}

const struct tickless_hooks *tickless_hooks_get(void)
{
    return &tickless_hooks;
}
