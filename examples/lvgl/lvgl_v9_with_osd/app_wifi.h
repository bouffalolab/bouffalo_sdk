#ifndef LVGL_USB_VIDEO_APP_WIFI_H
#define LVGL_USB_VIDEO_APP_WIFI_H

int app_wifi_init(void);

/* Spawn the deferred WiFi bring-up task (delays, then runs app_wifi_init()). */
void app_wifi_start(void);

/* Register the NetHub WiFi RX filter that splits STA/AP traffic between the
 * local stack and the USB host (EAPOL local, ARP/DHCP/ICMP both, rest host). */
void app_wifi_rx_filter_init(void);

#endif /* LVGL_USB_VIDEO_APP_WIFI_H */
