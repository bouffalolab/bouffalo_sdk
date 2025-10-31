#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/**
 * This function will fill a formatted string to buffer
 *
 * @param  buf the buffer to save formatted string.
 *
 * @param  format is the format parameters.
 *
 * @return The number of characters actually written to buffer.
 */
int __wrap_sprintf(char *buf, const char *format, ...)
{
    int32_t n;
    va_list arg_ptr;

    va_start(arg_ptr, format);
    n = vsprintf(buf, format, arg_ptr);
    va_end(arg_ptr);

    return n;
}