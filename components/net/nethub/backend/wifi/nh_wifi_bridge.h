#ifndef NH_WIFI_BRIDGE_H
#define NH_WIFI_BRIDGE_H

#include <stddef.h>

#include "nh_endpoint.h"

const struct nhif_ops *nh_wifi_endpoint_get_ops(size_t index);
size_t nh_wifi_endpoint_get_count(void);

#endif
