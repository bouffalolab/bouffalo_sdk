#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "macsw/wl80211_mac.h"

//#define SUPPORT_5G

struct ieee80211_dot_d {
    char *code;
    uint8_t channel24G_num;
    uint8_t channel5G_num;
    uint8_t channel24G_chan[MAC_DOMAINCHANNEL_24G_MAX];
    uint8_t channel5G_chan[MAC_DOMAINCHANNEL_5G_MAX];
};

const struct ieee80211_dot_d country_list[] =
{
    /*First Country is the default country*/
    {
        .code   = "CN",
        .channel24G_num = 13,
        .channel24G_chan = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
#if defined(SUPPORT_5G)
        .channel5G_num = 13,
        .channel5G_chan = {36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165},
#else
        .channel5G_num = 0,
#endif
    },
    {
        .code   = "JP",
        .channel24G_num = 14,
        .channel24G_chan = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
#if defined(SUPPORT_5G)
        .channel5G_num = 20,
        .channel5G_chan = {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144},
#else
        .channel5G_num = 0,
#endif
    },
    {
        .code   = "US",
        .channel24G_num = 11,
        .channel24G_chan = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
#if defined(SUPPORT_5G)
        .channel5G_num = 24,
        .channel5G_chan = {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165},
#else
        .channel5G_num = 0,
#endif
    },
    {
        .code   = "EU",
        .channel24G_num = 13,
        .channel24G_chan = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
#if defined(SUPPORT_5G)
        .channel5G_num = 19,
        .channel5G_chan = {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140},
#else
        .channel5G_num = 0,
#endif
    },
};

/** List of supported Channel */
const struct me_chan_config_req _macsw_chan_def = {
/// define a channel
#define CHAN(_freq, _band, _flags, _pwr)                                       \
  {                                                                            \
      .freq = (_freq),                                                         \
      .band = (_band),                                                         \
      .flags = (_flags),                                                       \
      .tx_power = (_pwr),                                                      \
  }

/// define a channel in 2.4GHz band
#define CHAN_24(_freq, _flag, _pwr) CHAN(_freq, PHY_BAND_2G4, _flag, _pwr)
/// define a channel in 5GHz band
#define CHAN_5(_freq, _flag, _pwr) CHAN(_freq, PHY_BAND_5G, _flag, _pwr)

    .chan2G4_cnt = 14,
    .chan2G4[0] = CHAN_24(2412, 0, 20),
    .chan2G4[1] = CHAN_24(2417, 0, 20),
    .chan2G4[2] = CHAN_24(2422, 0, 20),
    .chan2G4[3] = CHAN_24(2427, 0, 20),
    .chan2G4[4] = CHAN_24(2432, 0, 20),
    .chan2G4[5] = CHAN_24(2437, 0, 20),
    .chan2G4[6] = CHAN_24(2442, 0, 20),
    .chan2G4[7] = CHAN_24(2447, 0, 20),
    .chan2G4[8] = CHAN_24(2452, 0, 20),
    .chan2G4[9] = CHAN_24(2457, 0, 20),
    .chan2G4[10] = CHAN_24(2462, 0, 20),
    .chan2G4[11] = CHAN_24(2467, 0, 20),
    .chan2G4[12] = CHAN_24(2472, 0, 20),
    .chan2G4[13] = CHAN_24(2484, 0, 20),

    .chan5G_cnt = 28,
    .chan5G[0] = CHAN_5(5180, 0, 20),                        // 36
    .chan5G[1] = CHAN_5(5200, 0, 20),                        // 40
    .chan5G[2] = CHAN_5(5220, 0, 20),                        // 44
    .chan5G[3] = CHAN_5(5240, 0, 20),                        // 48
    .chan5G[4] = CHAN_5(5260, CHAN_NO_IR | CHAN_RADAR, 20),  // 52
    .chan5G[5] = CHAN_5(5280, CHAN_NO_IR | CHAN_RADAR, 20),  // 56
    .chan5G[6] = CHAN_5(5300, CHAN_NO_IR | CHAN_RADAR, 20),  // 60
    .chan5G[7] = CHAN_5(5320, CHAN_NO_IR | CHAN_RADAR, 20),  // 64
    .chan5G[8] = CHAN_5(5500, CHAN_NO_IR | CHAN_RADAR, 20),  // 100
    .chan5G[9] = CHAN_5(5520, CHAN_NO_IR | CHAN_RADAR, 20),  // 104
    .chan5G[10] = CHAN_5(5540, CHAN_NO_IR | CHAN_RADAR, 20), // 108
    .chan5G[11] = CHAN_5(5560, CHAN_NO_IR | CHAN_RADAR, 20), // 112
    .chan5G[12] = CHAN_5(5580, CHAN_NO_IR | CHAN_RADAR, 20), // 116
    .chan5G[13] = CHAN_5(5600, CHAN_NO_IR | CHAN_RADAR, 20), // 120
    .chan5G[14] = CHAN_5(5620, CHAN_NO_IR | CHAN_RADAR, 20), // 124
    .chan5G[15] = CHAN_5(5640, CHAN_NO_IR | CHAN_RADAR, 20), // 128
    .chan5G[16] = CHAN_5(5660, CHAN_NO_IR | CHAN_RADAR, 20), // 132
    .chan5G[17] = CHAN_5(5680, CHAN_NO_IR | CHAN_RADAR, 20), // 136
    .chan5G[18] = CHAN_5(5700, CHAN_NO_IR | CHAN_RADAR, 20), // 140
    .chan5G[19] = CHAN_5(5720, CHAN_NO_IR | CHAN_RADAR, 20), // 144
    .chan5G[20] = CHAN_5(5745, 0, 20),                       // 149
    .chan5G[21] = CHAN_5(5765, 0, 20),                       // 153
    .chan5G[22] = CHAN_5(5785, 0, 20),                       // 157
    .chan5G[23] = CHAN_5(5805, 0, 20),                       // 161
    .chan5G[24] = CHAN_5(5825, 0, 20),                       // 165
    .chan5G[25] = CHAN_5(5845, 0, 20),
    .chan5G[26] = CHAN_5(5865, 0, 20),
    .chan5G[27] = CHAN_5(5885, 0, 20),
};

struct mac_chan_def *wl80211_mac_chan_get(int freq) {
  int i, nb_chan;
  struct mac_chan_def *chans, *chan = NULL;

  if (freq < PHY_FREQ_5G) {
    chans = (struct mac_chan_def *)_macsw_chan_def.chan2G4;
    nb_chan = _macsw_chan_def.chan2G4_cnt;
  } else {
    chans = (struct mac_chan_def *)_macsw_chan_def.chan5G;
    nb_chan = _macsw_chan_def.chan5G_cnt;
  }

  for (i = 0; i < nb_chan; i++, chans++) {
    if (freq == chans->freq) {
      chan = chans;
      break;
    }
  }

  return chan;
}

int wl80211_mac_chan_config_update(uint8_t channel24G_num, uint8_t *channel24G_chan, uint8_t channel5G_num, uint8_t *channel5G_chan) {
  struct me_chan_config_req me_chan_req;
  memset(&me_chan_req, 0, sizeof(struct me_chan_config_req));

  for(uint8_t i = 0; i < channel24G_num; i++) {
    int freq = phy_channel_to_freq(PHY_BAND_2G4, channel24G_chan[i]);
    struct mac_chan_def *chan = wl80211_mac_chan_get(freq);
    if (chan) {
      memcpy(&me_chan_req.chan2G4[me_chan_req.chan2G4_cnt++], chan, sizeof(struct mac_chan_def));
    }
  }

  for(uint8_t i = 0; i < channel5G_num; i++) {
    int freq = phy_channel_to_freq(PHY_BAND_5G, channel5G_chan[i]);
    struct mac_chan_def *chan = wl80211_mac_chan_get(freq);
    if (chan) {
      memcpy(&me_chan_req.chan5G[me_chan_req.chan5G_cnt++], chan, sizeof(struct mac_chan_def));
    }
  }

  macif_kmsg_call(-1, NULL, 0, ME_CHAN_CONFIG_REQ, TASK_ME, &me_chan_req, sizeof(me_chan_req));
  return 0;
}

int wl80211_get_channel_nums(const char *country_code, uint8_t *c24G_cnt, uint8_t *c5G_cnt) {
  int i;
  if (c24G_cnt) {
    *c24G_cnt = country_list[0].channel24G_num; // default value
  }
  if (c5G_cnt) {
    *c5G_cnt = country_list[0].channel5G_num; // default value
  }

  for (i = 0; i < sizeof(country_list)/sizeof(country_list[0]); i++) {
    if (0 == strcmp(country_list[i].code, country_code)) {
      if (c24G_cnt) {
        *c24G_cnt = country_list[i].channel24G_num;
      }
      if (c5G_cnt) {
        *c5G_cnt = country_list[i].channel5G_num;
      }
    }
  }
  return 0;
}

int wl80211_get_channel_list(const char *country_code, uint8_t **c24G_list, uint8_t **c5G_list) {
  int i;
  if (c24G_list) {
    *c24G_list = (uint8_t *)&country_list[0].channel24G_chan[0]; // default value
  }
  if (c5G_list) {
    *c5G_list = (uint8_t *)&country_list[0].channel5G_chan[0]; // default value
  }

  for (i = 0; i < sizeof(country_list)/sizeof(country_list[0]); i++) {
    if (0 == strcmp(country_list[i].code, country_code)) {
      if (c24G_list) {
        *c24G_list = (uint8_t *)&country_list[i].channel24G_chan[0];
      }
      if (c5G_list) {
        *c5G_list = (uint8_t *)&country_list[i].channel5G_chan[0];
      }
    }
  }
  return 0;
}

static int wl80211_check_country_code(const char* country_code) {
  int i;
  if (!country_code)
    return -1;

  for (i = 0; i < sizeof(country_list)/sizeof(country_list[0]); i++) {
    if (0 == strcmp(country_list[i].code, country_code)) {
      break;
    }
  }

  if (i == sizeof(country_list)/sizeof(country_list[0])) {
    return -1;
  }

  return 0;
}

int wl80211_set_country_code(char *country_code) {
  uint8_t channel24G_num;
  uint8_t channel5G_num;
  uint8_t *channel24G_list;
  uint8_t *channel5G_list;
  int ret;

  if(strlcpy(wl80211_glb.country_code, country_code, sizeof(wl80211_glb.country_code)) >= sizeof(wl80211_glb.country_code)) {
    printf("[OS]: strlcpy truncated \r\n");
  }
  wl80211_glb.country_code[2] = '\0';

  if (wl80211_check_country_code(wl80211_glb.country_code)) {
    strlcpy(wl80211_glb.country_code, country_list[0].code, sizeof(wl80211_glb.country_code));
    wl80211_glb.country_code[2] = '\0';
    printf("Warning: country code %s is not supported, default to using CN.\r\n", country_code);
  }

  wl80211_get_channel_nums(wl80211_glb.country_code, (uint8_t *)&channel24G_num, (uint8_t *)&channel5G_num);
  wl80211_get_channel_list(wl80211_glb.country_code, &channel24G_list, &channel5G_list);
  ret = wl80211_mac_chan_config_update(channel24G_num, channel24G_list, channel5G_num, channel5G_list);
  if (ret != 0) {
    printf("%s channel config failed\r\n", __func__);
    return -1;
  }

  return 0;
}

int wl80211_get_country_code(char *country_code)
{
  assert(country_code != NULL);

  memcpy(country_code, wl80211_glb.country_code, sizeof(wl80211_glb.country_code));
  return 0;
}

int wl80211_channel_valid_check(uint16_t channel)
{
  uint8_t channel24G_num;
  uint8_t channel5G_num;
  uint8_t *channel24G_list;
  uint8_t *channel5G_list;
  int i;

  wl80211_get_channel_nums(wl80211_glb.country_code, (uint8_t *)&channel24G_num, (uint8_t *)&channel5G_num);
  wl80211_get_channel_list(wl80211_glb.country_code, &channel24G_list, &channel5G_list);

  for (i = 0; i < channel24G_num; i++) {
    if (channel == channel24G_list[i]) {
      return 0;
    }
  }
  for (i = 0; i < channel5G_num; i++) {
    if (channel == channel5G_list[i]) {
      return 0;
    }
  }

  return -1;
}

int wl80211_freq_valid_check(uint16_t freq)
{
  uint8_t channel24G_num;
  uint8_t channel5G_num;
  uint8_t *channel24G_list;
  uint8_t *channel5G_list;
  int i;

  wl80211_get_channel_nums(wl80211_glb.country_code, (uint8_t *)&channel24G_num, (uint8_t *)&channel5G_num);
  wl80211_get_channel_list(wl80211_glb.country_code, &channel24G_list, &channel5G_list);

  for (i = 0; i < channel24G_num; i++) {
    if (freq == phy_channel_to_freq(E_PHY_BAND_2G4, channel24G_list[i])) {
      return 0;
    }
  }
  for (i = 0; i < channel5G_num; i++) {
    if (freq == phy_channel_to_freq(E_PHY_BAND_5G, channel5G_list[i])) {
      return 0;
    }
  }

  return -1;
}

