#ifndef BOUFFALO_LIBC_POSIX_SIGNAL_H
#define BOUFFALO_LIBC_POSIX_SIGNAL_H

/* Keep the public signal types consistent with the FreeRTOS POSIX timer API.
 * The toolchain's newlib signal.h declares the same sigval/sigevent types and
 * conflicts with FreeRTOS+POSIX when both headers are pulled in. */
#include <FreeRTOS_POSIX.h>
#include <FreeRTOS_POSIX/signal.h>

#endif
