#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/**
 * This function will fill a formatted string to buffer.
 *
 * @param  buf is the buffer to save formatted string.
 *
 * @param  format is the format parameters.
 *
 * @param  arg_ptr is a list of variable parameters.
 *
 * @return The number of characters actually written to buffer.
 */
int vsprintf(char *buf, const char *format, va_list arg_ptr)
{
    return vsnprintf(buf, (size_t) - 1, format, arg_ptr);
}