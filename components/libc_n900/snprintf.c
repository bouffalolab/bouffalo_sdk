#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/**
 * This function will fill a formatted string to buffer.
 *
 * @param  buf is the buffer to save formatted string.
 *
 * @param  size is the size of buffer.
 *
 * @param  fmt is the format parameters.
 *
 * @return The number of characters actually written to buffer.
 */
int __wrap_snprintf(char *__restrict buf, size_t size, const char *__restrict fmt, ...)
{
    int n;
    va_list args;

    va_start(args, fmt);
    n = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return n;
}