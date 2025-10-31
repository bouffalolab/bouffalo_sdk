#ifndef _WIFI_MGMR_H_
#define _WIFI_MGMR_H_

#include <stdint.h>

#define WIFI_EVENT_BEACON_IND_AUTH_OPEN 0
#define WIFI_EVENT_BEACON_IND_AUTH_WEP 1
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK 2
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK 3
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK 4
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT 5
#define WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE 6
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE 7
#define WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN 0xff

#define WIFI_EVENT_BEACON_IND_CIPHER_NONE 0
#define WIFI_EVENT_BEACON_IND_CIPHER_WEP 1
#define WIFI_EVENT_BEACON_IND_CIPHER_AES 2
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP 3
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES 4

typedef struct mode {
  char *mode_str;
  int mode;
} wifimode_t;

struct reason_code {
  uint16_t reason_code;
  const char *action;
};

void wifi_mgmr_sta_ps_enter(void);
void wifi_mgmr_sta_ps_exit(void);

int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]);
int wifi_mgmr_sta_mac_get(uint8_t mac[6]);
int wifi_mgmr_sta_channel_get(uint8_t *channel);
int wifi_mgmr_sta_aid_get(void);
int wifi_mgmr_sta_state_get(void);

char *wifi_mgmr_mode_to_str(uint32_t mode);
char *wifi_mgmr_auth_to_str(uint8_t auth);
char *wifi_mgmr_cipher_to_str(uint8_t cipher);
const char *wifi_mgmr_get_sm_status_code_str(uint16_t status_code);
#endif
