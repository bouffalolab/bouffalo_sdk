#ifndef LVGL_USB_VIDEO_APP_WIFI_H
#define LVGL_USB_VIDEO_APP_WIFI_H

/* Start WiFi using the SDK event sequence, then delete the startup task. */
void wifi_start_firmware_task(void *param);

/* Register the USB NetHub filter, retaining traffic for the local IP stack. */
void app_wifi_rx_filter_init(void);

#endif /* LVGL_USB_VIDEO_APP_WIFI_H */
