/****************************************************************************
 * port_libcpp.c - C++ Standard Library Support Functions for Picolibc
 *
 * This file provides functions required by libstdc++ (C++ standard library)
 * that may not be available in picolibc or are GNU-specific extensions.
 *
 ****************************************************************************/

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/****************************************************************************
 * External environment variables (defined in syscalls_nosys.c)
 ****************************************************************************/
extern char *__env[1];
extern char **environ;

/****************************************************************************
 * Name: __xpg_strerror_r
 *
 * Description:
 *   XPG (X/Open Portability Guide) version of strerror_r.
 *   This is a GNU-specific function required by libstdc++.
 *
 *   The GNU version returns a char* pointer, while the XSI version returns
 *   an int. This implementation follows the GNU specification.
 *
 * Parameters:
 *   errnum - Error number
 *   buf    - Buffer to store error string (may be ignored by GNU version)
 *   buflen - Buffer size (may be ignored by GNU version)
 *
 * Return Value:
 *   Pointer to error message string
 *
 ****************************************************************************/
char *__xpg_strerror_r(int errnum, char *buf, size_t buflen)
{
    (void)buf;      /* Unused in GNU version */
    (void)buflen;   /* Unused in GNU version */

    /* Use standard strerror to get the error message */
    return (char *)strerror(errnum);
}

/****************************************************************************
 * Name: getenv
 *
 * Description:
 *   Get value of environment variable.
 *   In embedded systems, environment variables are typically not supported,
 *   so this returns NULL to indicate the variable doesn't exist.
 *
 * Parameters:
 *   name - Name of environment variable
 *
 * Return Value:
 *   Pointer to value string, or NULL if not found (always NULL in this case)
 *
 ****************************************************************************/
char *getenv(const char *name)
{
    (void)name;

    /* Embedded systems typically don't have environment variables */
    return NULL;
}

/****************************************************************************
 * Weak symbols for strtoul/strtol
 *
 * Note: Picolibc should provide these functions. If they're missing,
 * we provide weak implementations here. The picolibc versions will
 * take precedence if available.
 ****************************************************************************/
#pragma weak strtoul
unsigned long strtoul(const char *nptr, char **endptr, int base)
{
    unsigned long result = 0;
    int sign = 1;
    int digit;

    /* Skip leading whitespace */
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' ||
           *nptr == '\r' || *nptr == '\v' || *nptr == '\f') {
        nptr++;
    }

    /* Handle optional sign */
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    /* Determine base if not specified */
    if (base == 0 || base == 16) {
        if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
            nptr += 2;
            base = 16;
        } else if (base == 0) {
            base = (nptr[0] == '0') ? 8 : 10;
        }
    }

    /* Validate base */
    if (base < 2 || base > 36) {
        if (endptr) {
            *endptr = (char *)nptr;
        }
        errno = EINVAL;
        return 0;
    }

    /* Convert digits */
    while (*nptr) {
        if (*nptr >= '0' && *nptr <= '9') {
            digit = *nptr - '0';
        } else if (*nptr >= 'a' && *nptr <= 'z') {
            digit = *nptr - 'a' + 10;
        } else if (*nptr >= 'A' && *nptr <= 'Z') {
            digit = *nptr - 'A' + 10;
        } else {
            break;  /* Invalid digit */
        }

        if (digit >= base) {
            break;  /* Digit not valid for this base */
        }

        /* Check for overflow */
        if (result > (ULONG_MAX - digit) / base) {
            errno = ERANGE;
            result = ULONG_MAX;
            break;
        }

        result = result * base + digit;
        nptr++;
    }

    if (endptr) {
        *endptr = (char *)nptr;
    }

    return result * sign;
}

#pragma weak strtol
long strtol(const char *nptr, char **endptr, int base)
{
    unsigned long result;
    int sign = 1;
    const char *orig_nptr = nptr;

    /* Skip leading whitespace */
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' ||
           *nptr == '\r' || *nptr == '\v' || *nptr == '\f') {
        nptr++;
    }

    /* Handle optional sign */
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    /* Use strtoul for actual conversion */
    result = strtoul(nptr, endptr, base);

    /* Update endptr if conversion happened after whitespace/sign */
    if (endptr && *endptr == nptr) {
        *endptr = (char *)orig_nptr;
    }

    /* Apply sign with overflow check */
    if (sign == -1) {
        if (result > (unsigned long)LONG_MAX + 1) {
            errno = ERANGE;
            return (sign == 1) ? LONG_MAX : LONG_MIN;
        }
        return -(long)result;
    }

    if (result > LONG_MAX) {
        errno = ERANGE;
        return LONG_MAX;
    }

    return (long)result;
}

/****************************************************************************
 * Name: setenv
 *
 * Description:
 *   Set or change environment variable.
 *   Stub implementation that always fails.
 *
 ****************************************************************************/
int setenv(const char *name, const char *value, int overwrite)
{
    (void)name;
    (void)value;
    (void)overwrite;

    errno = ENOSYS;
    return -1;
}

/****************************************************************************
 * Name: unsetenv
 *
 * Description:
 *   Remove environment variable.
 *   Stub implementation that always fails.
 *
 ****************************************************************************/
int unsetenv(const char *name)
{
    (void)name;

    errno = ENOSYS;
    return -1;
}

/****************************************************************************
 * Name: clearenv
 *
 * Description:
 *   Clear all environment variables.
 *   Stub implementation that always fails.
 *
 ****************************************************************************/
int clearenv(void)
{
    errno = ENOSYS;
    return -1;
}
