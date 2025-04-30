#ifndef WIFI_H_VV2FWLIH
#define WIFI_H_VV2FWLIH

typedef enum {
    BL_MODE_NONE,
    BL_MODE_STA,      // card is STA
    BL_MODE_AP,       // card is AP
    BL_MODE_STA_AP,   // card is STA&AP
    BL_MODE_SNIFFER,  // card is sniffer
    BL_MODE_MAX,
} bl_wifi_mode_t;

const char *bl_mode_to_str(const bl_wifi_mode_t mode);
int bl_mode_xfer(bl_wifi_mode_t *mode, const bl_wifi_mode_t new_mode, bl_wifi_mode_t *old_mode);
void bl_change_eth_mac(void *dev);

#endif /* end of include guard: WIFI_H_VV2FWLIH */
