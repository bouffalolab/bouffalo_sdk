#ifndef NH_PROFILE_H
#define NH_PROFILE_H

#include "nh_common.h"
#include "nh_filter.h"
#include "nh_endpoint.h"

typedef struct {
    const char *name;
    nethub_channel_t host_link_type;
    nh_filter_policy_t wifi_rx_policy;
    const struct nhif_ops *host_endpoint_ops;
    const struct nh_ctrlpath_ops *ctrlpath_ops;
} nh_profile_t;

typedef struct nh_ctrlpath_ops {
    int (*upld_send)(uint8_t *data_buff, uint32_t data_size);
    int (*dnld_register)(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg);
} nh_ctrlpath_ops_t;

const nh_profile_t *nh_profile_get(void);
const nh_profile_t *nh_profile_sdio_get(void);
const nh_profile_t *nh_profile_usb_get(void);
const nh_profile_t *nh_profile_spi_get(void);
const char *nh_profile_get_name(void);
nethub_channel_t nh_profile_get_host_link_type(void);
const nh_filter_policy_t *nh_profile_get_wifi_rx_policy(void);
const struct nhif_ops *nh_profile_get_endpoint_ops(size_t index);
size_t nh_profile_get_endpoint_count(void);
const nh_ctrlpath_ops_t *nh_profile_get_ctrlpath_ops(void);

#endif
