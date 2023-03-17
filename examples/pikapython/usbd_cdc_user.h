#ifndef _USBD_CDC_H_
#define _USBD_CDC_H_

#include "usbd_core.h"
#include "usbd_cdc.h"

void cdc_acm_init(void);
int usbd_ep_read_sync(void *buf, uint32_t nbytes);
int usbd_ep_write_sync(const void *buf, uint32_t nbytes);
void cdc_acm_data_send_with_dtr_test(void);
char usb_cdc_user_getchar(void);
int usb_cdc_user_putchar(char ch);
int usb_cdc_fflush(void);

#endif