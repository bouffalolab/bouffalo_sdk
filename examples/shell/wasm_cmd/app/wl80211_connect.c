#include <stdio.h>
#include <string.h>

#include "wifi_mgmr.h"
#include "wl80211.h"

int aot_wl80211_connect(const char *ssid, const char *password,
                        const char *bssid, uint8_t channel)
{
    wifi_mgmr_sta_connect_params_t param = { 0 };

    if (!ssid || !ssid[0])
        return -1;
    strlcpy(param.ssid, ssid, sizeof(param.ssid));
    if (password)
        strlcpy(param.key, password, sizeof(param.key));
    if (bssid) {
        unsigned int b[6];
        int i;

        if (sscanf(bssid, "%x:%x:%x:%x:%x:%x",
                   &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) != 6)
            return -1;
        for (i = 0; i < 6; i++) {
            if (b[i] > 0xff)
                return -1;
        }
        snprintf(param.bssid_str, sizeof(param.bssid_str),
                 "%02X:%02X:%02X:%02X:%02X:%02X",
                 (unsigned)b[0], (unsigned)b[1], (unsigned)b[2],
                 (unsigned)b[3], (unsigned)b[4], (unsigned)b[5]);
    }
    if (channel) {
        int freq = wl80211_channel_to_freq(channel);

        if (freq <= 0)
            return -1;
        param.freq1 = (uint16_t)freq;
    }
    /* Use the wifi_mgmr connect path so the netif is brought up and DHCP
     * runs after the STA connects. */
    param.use_dhcp = 1;
    return wifi_mgmr_sta_connect(&param);
}
