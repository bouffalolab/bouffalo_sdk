#ifndef NH_HUB_H
#define NH_HUB_H

#include "nh_common.h"
#include "nh_types.h"

int nethub_init(const nethub_config_t *config);
int nethub_deinit(void);
int nethub_start(void);
int nethub_stop(void);

nethub_route_result_t nethub_process_input(nethub_frame_t *frame, nethub_channel_t src_type);
nethub_route_result_t nethub_send_output(nethub_frame_t *frame, nethub_channel_t dst_type);

bool nethub_is_initialized(void);
bool nethub_is_started(void);

const char *nethub_profile_name(void);
nethub_channel_t nethub_host_channel(void);
nethub_channel_t nethub_get_active_wifi_channel(void);

#endif
