#ifndef TICKLESS_HOOK_H
#define TICKLESS_HOOK_H

#include <stdbool.h>

#include "FreeRTOS.h"
#include "bl_lp.h"

struct tickless_hooks {
    bool (*allow_enter)(void *arg);
    void *allow_enter_arg;
    bool (*prepare_enter)(void *arg);
    void *prepare_enter_arg;
    bool (*after_resume)(void *arg);
    void *after_resume_arg;
};

int tickless_hooks_register(bool (*allow_enter)(void *arg),
                            void *allow_enter_arg,
                            bool (*prepare_enter)(void *arg),
                            void *prepare_enter_arg,
                            bool (*after_resume)(void *arg),
                            void *after_resume_arg);
const struct tickless_hooks *tickless_hooks_get(void);

#endif
