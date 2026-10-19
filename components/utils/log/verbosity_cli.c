#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "console_output.h"
#include "shell.h"

#ifdef CONFIG_WIFI6
#include "macsw_plat.h"
#endif

static int verbosity_print(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

static int verbosity_print(const char *fmt, ...)
{
    char buffer[160];
    va_list args;
    int length;

    va_start(args, fmt);
    length = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    if (length < 0 || (size_t)length >= sizeof(buffer)) {
        return -1;
    }

    /* Command replies must remain visible when automatic logs are clipped. */
    return bflb_console_write(buffer, (size_t)length) == length ? 0 : -1;
}

#ifdef CONFIG_WIFI6
static int verbosity_wifi_show(void)
{
    static const char *const names[] = {
        "fatal", "error", "warn", "info", "debug", "trace", "off"
    };
    int level = wifi_verbosity_get();
    const char *name = level >= WIFI_DIAG_FATAL && level <= WIFI_DIAG_OFF ?
                       names[level] : "unknown";

    return verbosity_print("module=wifi scope=macsw-auto level=%s debug_enabled=%s\r\n",
                           name, wifi_verbosity_enabled(WIFI_DIAG_DEBUG) ? "yes" : "no");
}
#endif

static int verbosity_cmd(int argc, char **argv)
{
    if (argc < 1 || argc > 3) {
        goto usage;
    }

    if (argc == 1) {
#ifdef CONFIG_WIFI6
        return verbosity_wifi_show();
#else
        return verbosity_print("No modules available\r\n");
#endif
    }

    if (strcmp(argv[1], "wifi") != 0) {
        goto unavailable;
    }

#ifdef CONFIG_WIFI6
    if (argc == 3) {
        int level;

        if (strcmp(argv[2], "off") == 0) {
            level = WIFI_DIAG_OFF;
        } else if (strcmp(argv[2], "error") == 0) {
            level = WIFI_DIAG_ERROR;
        } else if (strcmp(argv[2], "info") == 0) {
            level = WIFI_DIAG_INFO;
        } else if (strcmp(argv[2], "debug") == 0) {
            level = WIFI_DIAG_DEBUG;
        } else {
            goto usage;
        }
        if (wifi_verbosity_set(level) != 0) {
            verbosity_print("Wi-Fi verbosity update failed\r\n");
            return -1;
        }
    }

    return verbosity_wifi_show();
#endif

unavailable:
    verbosity_print("Module not available\r\n");
    return -1;

usage:
    verbosity_print("usage: verbosity [module [off|error|info|debug]]\r\n");
    return -1;
}

SHELL_CMD_EXPORT_ALIAS(verbosity_cmd, verbosity, Query or set module diagnostic verbosity);
