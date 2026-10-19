#ifndef __LPFW_CUSTOM_RX_H__
#define __LPFW_CUSTOM_RX_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LP_FW_CUSTOM_RX_MAGIC                           0x43525831u /* "CRX1" */
#define LP_FW_CUSTOM_RX_VERSION                         1u
#define LP_FW_CUSTOM_RX_MAX_FRAME_LEN                   512u

#define LP_FW_CUSTOM_RX_ACTION_CONTINUE                 0u
#define LP_FW_CUSTOM_RX_ACTION_SLEEP_NOW                1u
#define LP_FW_CUSTOM_RX_ACTION_WAKE_APP                 2u
#define LP_FW_CUSTOM_RX_ACTION_WAKE_ERROR               3u

#define LP_FW_CUSTOM_RX_ERROR_NONE                      0u
#define LP_FW_CUSTOM_RX_ERROR_BAD_CONFIG                1u
#define LP_FW_CUSTOM_RX_ERROR_BAD_CALLBACK              2u
#define LP_FW_CUSTOM_RX_ERROR_BAD_DESCRIPTOR            3u
#define LP_FW_CUSTOM_RX_ERROR_BAD_ACTION                4u

/* RX filter bits shared by the APP and the BL618DG LPFW MAC register ABI. */
#define LP_FW_CUSTOM_RX_FILTER_ACCEPT_OTHER_MGMT_FRAMES (1u << 15)
#define LP_FW_CUSTOM_RX_FILTER_ACCEPT_BEACON            (1u << 10)
#define LP_FW_CUSTOM_RX_FILTER_ACCEPT_ERROR_FRAMES      (1u << 5)
#define LP_FW_CUSTOM_RX_FILTER_ACCEPT_OTHER_BSSID       (1u << 4)
#define LP_FW_CUSTOM_RX_FILTER_ACCEPT_BROADCAST         (1u << 3)

#define LP_FW_CUSTOM_RX_FILTER_DEFAULT                 \
    (LP_FW_CUSTOM_RX_FILTER_ACCEPT_OTHER_MGMT_FRAMES | \
     LP_FW_CUSTOM_RX_FILTER_ACCEPT_OTHER_BSSID |       \
     LP_FW_CUSTOM_RX_FILTER_ACCEPT_BROADCAST)

typedef struct {
    uint16_t struct_size;
    uint16_t frame_len;
    uint32_t frame_addr;
    uint32_t statinfo;
    uint32_t tsf_low;
    uint32_t tsf_high;
    uint8_t channel;
    uint8_t reserved8[3];
    uint32_t rx_vec_1[4];
    uint32_t rx_vec_2[2];
} lp_fw_custom_rx_packet_t;

typedef uint32_t (*lp_fw_custom_rx_callback_t)(const lp_fw_custom_rx_packet_t *packet, void *arg);

typedef struct {
    uint32_t rx_filter;
    uint8_t local_mac[6];
    uint8_t local_mac_mask[6];
    uint8_t bssid[6];
    uint8_t bssid_mask[6];
} lp_fw_custom_rx_mac_cfg_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t struct_size;

    uint8_t enable;
    uint8_t channel;
    uint16_t flags;

    lp_fw_custom_rx_mac_cfg_t mac;

    uint32_t interval_us;
    uint32_t window_us;
    uint32_t max_frames_per_window;

    uint32_t callback_addr;
    uint32_t callback_arg_addr;

    uint64_t next_wakeup_rtc_cnt;

    uint32_t last_action;
    uint32_t last_error;
    uint32_t rx_count;
    uint32_t callback_count;
    uint32_t wake_count;
    uint32_t invalid_desc_drop_count;
    uint32_t too_large_drop_count;
    uint32_t non_contiguous_drop_count;
} lp_fw_custom_rx_cfg_t;

static inline int lp_fw_custom_rx_cfg_is_valid(const lp_fw_custom_rx_cfg_t *cfg)
{
    return cfg != NULL &&
           cfg->magic == LP_FW_CUSTOM_RX_MAGIC &&
           cfg->version == LP_FW_CUSTOM_RX_VERSION &&
           cfg->struct_size == sizeof(lp_fw_custom_rx_cfg_t) &&
           cfg->enable != 0u &&
           cfg->channel != 0u &&
           cfg->interval_us > cfg->window_us &&
           cfg->window_us != 0u &&
           cfg->max_frames_per_window != 0u &&
           cfg->callback_addr != 0u &&
           cfg->callback_arg_addr != 0u;
}

_Static_assert(sizeof(uintptr_t) == sizeof(uint32_t), "Custom RX requires a 32-bit address space");
_Static_assert(sizeof(lp_fw_custom_rx_packet_t) == 48u, "Custom RX packet ABI changed");
_Static_assert(sizeof(lp_fw_custom_rx_mac_cfg_t) == 28u, "Custom RX MAC ABI changed");
_Static_assert(sizeof(lp_fw_custom_rx_cfg_t) == 104u, "Custom RX config ABI changed");
_Static_assert(offsetof(lp_fw_custom_rx_cfg_t, callback_addr) == 52u, "Custom RX callback offset changed");
_Static_assert(offsetof(lp_fw_custom_rx_cfg_t, next_wakeup_rtc_cnt) == 64u, "Custom RX deadline offset changed");

#ifdef __cplusplus
}
#endif

#endif