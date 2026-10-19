#ifndef _USB_READER_H
#define _USB_READER_H

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#endif

/* Max JPEG size per frame. 480x960 q=4 often averages ~45KB but peaks higher. */
#define JPG_BUFFER_SIZE (128 * 1024)
#define BUFFER_COUNT    3

/* Create the JPEG buffer queues and register the app video callbacks. */
int usb_reader_init(void);

#if defined(CONFIG_FREERTOS)
/* Producer task: read length-prefixed JPEG frames and push them to the output queue. */
void usb_reader_task(void *param);
#endif

#endif /* _USB_READER_H */
