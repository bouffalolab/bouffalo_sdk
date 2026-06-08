#ifndef NH_PROFILE_H
#define NH_PROFILE_H

#include "nh_common.h"
#include "nh_filter.h"
#include "nh_endpoint.h"
#include "nethub_vchan.h"

typedef struct nh_vchan_ops {
    int (*send)(nethub_vchan_type_t type, const void *data, uint16_t len);
    int (*recv_register)(nethub_vchan_type_t type,
                         nethub_vchan_recv_cb_t recv_cb,
                         void *arg);
} nh_vchan_ops_t;

typedef struct {
    const char *name;
    nethub_channel_t host_link_type;
    nh_filter_policy_t wifi_rx_policy;
    const struct nhif_ops *host_endpoint_ops;
    const struct nh_vchan_ops *vchan_ops;
} nh_profile_t;

const nh_profile_t *nh_profile_get(void);
const nh_profile_t *nh_profile_sdio_get(void);
const nh_profile_t *nh_profile_usb_get(void);
const nh_profile_t *nh_profile_spi_get(void);
size_t nh_profile_get_host_count(void);
const nh_profile_t *nh_profile_get_host(size_t index);
const nh_profile_t *nh_profile_get_by_host(nethub_channel_t host_link);
const char *nh_profile_get_name(void);
nethub_channel_t nh_profile_get_host_link_type(void);
const nh_filter_policy_t *nh_profile_get_wifi_rx_policy(void);
const struct nhif_ops *nh_profile_get_endpoint_ops(size_t index);
size_t nh_profile_get_endpoint_count(void);
const nh_vchan_ops_t *nh_profile_get_vchan_ops(void);

#endif
