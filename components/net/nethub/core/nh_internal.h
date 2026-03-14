#ifndef NH_INTERNAL_H
#define NH_INTERNAL_H

#include "nh_profile.h"

typedef struct {
    bool initialized;
    bool started;
    const nh_profile_t *profile;
    nethub_channel_t host_link_type;
} nethub_context_t;

nethub_context_t *nh_ctx_get(void);

#endif
