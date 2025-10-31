#include "blctl_log.h"

static int bl_log_level = BL_LOG_LEVEL_INFO;

int bl_log_level_get(void)
{
    return bl_log_level;
}

int bl_log_level_set(int level)
{
    bl_log_level = level;
    return 0;
}

void bl_log_print(int level, const char *fmt, ...)
{
    va_list va;

    if (bl_log_level_get() < level)
        return;

    va_start(va, fmt);
    vfprintf(stdout, fmt, va);
    va_end(va);
}
