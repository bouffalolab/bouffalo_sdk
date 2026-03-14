#ifndef NETHUB_DEFS_H
#define NETHUB_DEFS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    NETHUB_CHANNEL_WIFI_STA = 0,
    NETHUB_CHANNEL_WIFI_AP,
    NETHUB_CHANNEL_STACK_STA,
    NETHUB_CHANNEL_STACK_AP,
    NETHUB_CHANNEL_STACK_NAT,
    NETHUB_CHANNEL_BRIDGE,
    NETHUB_CHANNEL_SDIO,
    NETHUB_CHANNEL_USB,
    NETHUB_CHANNEL_SPI,
    NETHUB_CHANNEL_MAX
} nethub_channel_t;

typedef enum {
    NETHUB_OK = 0,
    NETHUB_ERR_INVALID_PARAM = -1,
    NETHUB_ERR_NOT_FOUND = -2,
    NETHUB_ERR_ALREADY_EXISTS = -3,
    NETHUB_ERR_NO_MEMORY = -4,
    NETHUB_ERR_NOT_INITIALIZED = -5,
    NETHUB_ERR_INVALID_STATE = -6,
    NETHUB_ERR_INTERNAL = -7,
    NETHUB_ERR_NOT_SUPPORTED = -8,
} nethub_status_t;

typedef int (*nethub_ctrl_rx_cb_t)(void *ctx, uint8_t *data, uint32_t data_size);

typedef struct {
    uint32_t dnld_producer_count;
    uint32_t dnld_consumer_netif_down;
    uint32_t dnld_consumer_tx_fail;
    uint32_t dnld_consumer_free_normal;
    uint32_t dnld_consumer_free_abnormal;
    uint32_t dnld_total_packets;
    uint32_t dnld_total_dropped;
    uint32_t dnld_total_success;
    uint32_t upld_producer_count;
    uint32_t upld_flow_through;
    uint32_t upld_start_consume;
    uint32_t upld_transfer_complete;
    uint32_t upld_free_count;
    uint32_t upld_total_packets;
} nethub_statistics_t;

typedef struct {
    bool initialized;
    bool started;
    bool custom_wifi_rx_filter_active;
    const char *profile_name;
    nethub_channel_t host_channel;
    nethub_channel_t active_wifi_channel;
    nethub_statistics_t statistics;
} nethub_runtime_status_t;

#endif
