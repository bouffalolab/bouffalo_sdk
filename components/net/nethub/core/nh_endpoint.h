#ifndef NH_ENDPOINT_H
#define NH_ENDPOINT_H

#include "nh_types.h"

struct nhif_ops;

typedef struct nhif_instance {
    nethub_channel_t type;
    nethub_endpoint_state_t state;
    const struct nhif_ops *ops;
    void *private_data;
} nhif_instance_t;

struct nhif_ops {
    nethub_channel_t type;
    int (*init)(void);
    int (*deinit)(void);
    nethub_route_cb_t input;
    nethub_route_cb_t output;
};

int nhif_register(const struct nhif_ops *ops, void *private_data);
int nhif_unregister(nethub_channel_t type);
int nhif_unregister_all(void);
nhif_instance_t *nhif_find_by_type(nethub_channel_t type);
int nhif_init_all(void);
int nhif_deinit_all(void);

const char *nethub_channel_to_string(nethub_channel_t type);

#endif
