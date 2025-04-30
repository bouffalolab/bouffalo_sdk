#ifndef __RNM_COMMON_H__
#define __RNM_COMMON_H__

#include <stdint.h>
#include <rnm_msg.h>

#include <sdiowifi_platform_adapt.h>

#ifdef __cplusplus
extern "C" {
#endif

#define rnm_malloc platform_malloc 
#define rnm_free   platform_free 

#define RNM_STATISTIC_SCRATCH_COUNTER_NUM 8

typedef struct {
    rnm_base_msg_t cmd;
    void (*free_fn)(void *data);
    void *data;
    uint16_t data_len;
} rnm_msg_t;

typedef enum {
    RNM_OUT_COMMAND,
    RNM_OUT_ETH_FRAME,
    RNM_OUT_SNIFFER_FRAME,
} rnm_output_data_type_t;

enum {
    RNM_OUTPUT_FN_SUCCESS,
    RNM_OUTPUT_FN_TIMEOUT,
    RNM_OUTPUT_FN_PEER_NOT_READY,
    RNM_OUTPUT_FN_ERROR_OTHER,
};

/**
 * @brief  Callback used to send RNM messages.
 *
 * @return  See rnm_output_return_value.
 */
typedef int (*rnm_output_t)(void *env, rnm_output_data_type_t type, const void *data, uint16_t data_length);
typedef int (*rnm_heartbeat_failure_cb_t)(void *env);

typedef struct {
    struct {
        uint32_t tx_cnt;
        uint32_t rx_cnt;
    } cmds[BF1B_CMD_MAX];
    uint32_t scratch_counter[RNM_STATISTIC_SCRATCH_COUNTER_NUM];
} rnm_statistic_t;

typedef struct {
    int idx;
    const char *desc;
} rnm_stat_scr_ctr_desc_t;

#define RNM_MAKE_STAT_SCR_CTR_DESC(index, description) [index] = {.idx = index, .desc = description}

typedef struct {
    uint16_t cmd;
    const char *cmd_name;
} rnm_cmd_desc_t;

extern rnm_cmd_desc_t rnm_cmd_desc[BF1B_CMD_MAX];

/**
 * @brief  Convert RNM Wi-Fi authentication type code to C string.
 *
 * @param auth  RNM Wi-Fi authentication type code.
 *
 * @return  C string description of Wi-Fi authentication type.
 */
const char *rnm_auth_to_str(uint8_t auth);

/**
 * @brief  Convert RNM Wi-Fi cipher type code to C string.
 *
 * @param cipher  RNM Wi-Fi cipher type code.
 *
 * @return  C string description of Wi-Fi cipher type.
 */
const char *rnm_cipher_to_str(uint8_t cipher);

/**
 * @brief  Dump statistic data of cmds.
 *
 * @param stat     Statistic data.
 * @param sc_desc  Description of scratch counters.
 * @param sc_cnt   Number of scratch counters.
 *
 * @return  0 on success or -1 on error.
 */
int rnm_statistic_dump(const rnm_statistic_t *stat, rnm_stat_scr_ctr_desc_t *sc_desc, int sc_cnt);

#ifdef __cplusplus
}
#endif

#endif

