#ifndef NH_RUNTIME_H
#define NH_RUNTIME_H

#include "nethub_defs.h"

nethub_channel_t nh_runtime_get_active_wifi_channel(void);
int nh_runtime_set_active_wifi_channel(nethub_channel_t channel);

nethub_statistics_t *nh_runtime_stats(void);
const nethub_statistics_t *nh_runtime_stats_readonly(void);

#endif
