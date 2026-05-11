#include "nethub_log.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE *g_log_fp;
static int g_debug_enabled;

int nethub_log_init(const char *path)
{
    const char *debug_env = getenv("NETHUB_WIFI_DEBUG");

    g_debug_enabled = (debug_env && strcmp(debug_env, "0") != 0);
    if (!path || path[0] == '\0') {
        return 0;
    }

    g_log_fp = fopen(path, "a");
    return g_log_fp ? 0 : -1;
}

void nethub_log_deinit(void)
{
    if (g_log_fp) {
        fclose(g_log_fp);
        g_log_fp = NULL;
    }
}

static void log_vwrite(const char *level, const char *file, int line, const char *fmt, va_list ap)
{
    FILE *out = g_log_fp ? g_log_fp : stderr;
    time_t now = time(NULL);
    struct tm tm_now;
    char ts[32];

    localtime_r(&now, &tm_now);
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm_now);
    fprintf(out, "%s [%s] %s:%d ", ts, level, file ? file : "?", line);
    vfprintf(out, fmt, ap);
    fputc('\n', out);
    fflush(out);
}

void nethub_log_write(const char *level, const char *file, int line, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log_vwrite(level, file, line, fmt, ap);
    va_end(ap);
}

void nethub_log_debug(const char *fmt, ...)
{
    va_list ap;

    if (!g_debug_enabled) {
        return;
    }

    va_start(ap, fmt);
    log_vwrite("D", __FILE__, __LINE__, fmt, ap);
    va_end(ap);
}
