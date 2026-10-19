#ifndef BFLB_CONSOLE_OUTPUT_H
#define BFLB_CONSOLE_OUTPUT_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Common output entry for the selected console transport. */
ssize_t bflb_console_write(const void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* BFLB_CONSOLE_OUTPUT_H */
