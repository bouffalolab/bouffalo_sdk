
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "hostrouter_log.h"

#include <sdiowifi_platform_adapt.h>

extern void vprint(const char *fmt, va_list argp);
/**
 * This function is print sdio debug.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void hr_debug(const char *tag, const char *filename, const char *funcname, const long line, const char *format, ...)
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
    }

    printf("[%ld][%s] %s %ld %s :", sdiowifi_tick_now(0), color_tag, filename, line, funcname);
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

static void __print_binary(uint32_t value, char *buffer, size_t size)
{
    int offset = 0;

    for (int i = 31; i >= 0; --i) {
        int written = snprintf(buffer + offset, size - offset, "%d", (value >> i) & 1);
        if (written < 0 || written >= size - offset) {
            // Handle error or buffer overflow
            break;
        }
        offset += written;
        if (i % 4 == 0) {
            written = snprintf(buffer + offset, size - offset, " ");
            if (written < 0 || written >= size - offset) {
                // Handle error or buffer overflow
                break;
            }
            offset += written;
        }
    }
    snprintf(buffer + offset, size - offset, "\n");
}

int hr_binary_regshow(uint32_t *reg)
{
    char buffer[48];
    __print_binary((*reg), buffer, sizeof(buffer));
    HR_LOGI("%s\r\n", buffer);

    return 0;
}

int hr_binary_valshow(uint32_t value)
{
    char buffer[48];
    __print_binary(value, buffer, sizeof(buffer));
    HR_LOGI("%s\r\n", buffer);

    return 0;
}
