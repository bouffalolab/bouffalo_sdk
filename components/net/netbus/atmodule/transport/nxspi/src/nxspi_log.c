
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <stdbool.h>
#include "nxspi_log.h"

extern void vprint(const char *fmt, va_list argp);

uint32_t nx_tick(void)
{
    if (xPortIsInsideInterrupt()) {
        return xTaskGetTickCountFromISR();
    } else {
        return xTaskGetTickCount();
    }
}

/**
 * This function is print sdio debug.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void nxs_debug(const char *tag, const char *filename, const char *funcname, const long line, const char *format, ...)
{
    const char *color_d = "\x1b[36msdiod\x1b[0m";
    const char *color_i = "\x1b[32msdioi\x1b[0m";
    const char *color_w = "\x1b[33msdiow\x1b[0m";
    const char *color_e = "\x1b[31msdioe\x1b[0m";
    const char *color_a = "\x1b[35msdioa\x1b[0m";
    char *color_tag = (char *)tag;

    if (tag[0] == 'a') {
        color_tag = (char *)color_a;
    } else if (tag[0] == 'e') {
        color_tag = (char *)color_e;
    } else if (tag[0] == 'w') {
        color_tag = (char *)color_w;
    } else if (tag[0] == 'i') {
        color_tag = (char *)color_i;
    } else if (tag[0] == 'd') {
        color_tag = (char *)color_d;
    } else if (tag[0] == 'p') {
        color_tag = (char *)color_i;
    }

    printf("[%ld][%s] %s %ld %s :", nx_tick(), color_tag, filename, line, funcname);
    {
        va_list args;

        /* args point to the first variable parameter */
        va_start(args, format);

        /* You can add your code under here. */
#ifdef BL_IOT_SDK
        vprint(format, args);
#else
        extern int console_vsnprintf(const char *fmt, va_list args);
        console_vsnprintf(format, args);
#endif
        va_end(args);
    }
}
