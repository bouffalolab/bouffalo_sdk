#ifndef USB_CONSOLE_H
#define USB_CONSOLE_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void usb_console_init(void);
ssize_t usb_console_write(const void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* USB_CONSOLE_H */
