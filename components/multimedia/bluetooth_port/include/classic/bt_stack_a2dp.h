/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#ifndef __BT_STACK_A2DP_H__
#define __BT_STACK_A2DP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Sampling Frequency */
#define BT_PRF_A2DP_SBC_SAMP_FREQ_16000 BIT(7)
#define BT_PRF_A2DP_SBC_SAMP_FREQ_32000 BIT(6)
#define BT_PRF_A2DP_SBC_SAMP_FREQ_44100 BIT(5)
#define BT_PRF_A2DP_SBC_SAMP_FREQ_48000 BIT(4)

/* Channel Mode */
#define BT_PRF_A2DP_SBC_CH_MODE_MONO  BIT(3)
#define BT_PRF_A2DP_SBC_CH_MODE_DUAL  BIT(2)
#define BT_PRF_A2DP_SBC_CH_MODE_STREO BIT(1)
#define BT_PRF_A2DP_SBC_CH_MODE_JOINT BIT(0)

/* Block Length */
#define BT_PRF_A2DP_SBC_BLK_LEN_4  BIT(7)
#define BT_PRF_A2DP_SBC_BLK_LEN_8  BIT(6)
#define BT_PRF_A2DP_SBC_BLK_LEN_12 BIT(5)
#define BT_PRF_A2DP_SBC_BLK_LEN_16 BIT(4)

/* Subbands */
#define BT_PRF_A2DP_SBC_SUBBAND_4 BIT(3)
#define BT_PRF_A2DP_SBC_SUBBAND_8 BIT(2)

/* Allocation Method */
#define BT_PRF_A2DP_SBC_ALLOC_MTHD_SNR      BIT(1)
#define BT_PRF_A2DP_SBC_ALLOC_MTHD_LOUDNESS BIT(0)

#define BT_PRF_A2DP_SBC_SAMP_FREQ(preset)  ((preset->config[0] >> 4) & 0x0f)
#define BT_PRF_A2DP_SBC_CHAN_MODE(preset)  ((preset->config[0]) & 0x0f)
#define BT_PRF_A2DP_SBC_BLK_LEN(preset)    ((preset->config[1] >> 4) & 0x0f)
#define BT_PRF_A2DP_SBC_SUB_BAND(preset)   ((preset->config[1] >> 2) & 0x03)
#define BT_PRF_A2DP_SBC_ALLOC_MTHD(preset) ((preset->config[1]) & 0x03)

/** @brief SBC Codec */
typedef struct bt_prf_a2dp_codec_sbc_params {
    /** First two octets of configuration */
    uint8_t config[2];
    /** Minimum Bitpool Value */
    uint8_t min_bitpool;
    /** Maximum Bitpool Value */
    uint8_t max_bitpool;
} bt_prf_a2dp_codec_sbc_params_t;

/** @brief A2DP state callback enum */
typedef enum {
    BT_PRF_A2DP_CONNECTION_STATE_EVT = 0, /*!< connection state changed event */
    BT_PRF_A2DP_AUDIO_STATE_EVT,          /*!< audio stream transmission state changed event */
    BT_PRF_A2DP_AUDIO_SBC_CFG_EVT,        /*!< audio sbc codec is configured, only used for A2DP SINK */
} bt_prf_a2dp_cb_event_t;

/** @brief A2DP connection state enum */
typedef enum {
    BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL = 0, /*!< disconnected normal */
    BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_ABNORMAL,   /*!< disconnected by loss signal */
    BT_PRF_A2DP_CONNECTION_STATE_CONNECTING,              /*!< connecting remote device */
    BT_PRF_A2DP_CONNECTION_STATE_CONNECTED,               /*!< connection established */
    BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTING            /*!< disconnecting remote device */
} bt_prf_a2dp_connection_state_t;

/** @brief A2DP audio state enum */
typedef enum {
    BT_PRF_A2DP_AUDIO_STATE_REMOTE_SUSPEND = 0, /*!< audio stream suspended by remote device */
    BT_PRF_A2DP_AUDIO_STATE_STOPPED,            /*!< audio stream stopped */
    BT_PRF_A2DP_AUDIO_STATE_STARTED,            /*!< audio stream started */
} bt_prf_a2dp_audio_state_t;

/** @brief A2DP state callback parameters */
typedef union
{
    /**
     * @brief  BT_PRF_A2DP_CONNECTION_STATE_EVT
     */
    struct a2d_conn_stat_param {
        bt_prf_a2dp_connection_state_t state;     /*!< one of values from bt_prf_a2dp_connection_state_t */
        bt_dev_addr_t                  peer_addr; /*!< remote bluetooth device address */
    } conn_stat;                                  /*!< A2DP connection status */

    /**
     * @brief BT_PRF_A2DP_AUDIO_STATE_EVT
     */
    struct a2d_audio_stat_param {
        bt_prf_a2dp_audio_state_t state;     /*!< one of the values from bt_prf_a2dp_audio_state_t */
        bt_dev_addr_t             peer_addr; /*!< remote bluetooth device address */
    } audio_stat;                            /*!< audio stream playing state */

    /**
     * @brief BT_PRF_A2DP_AUDIO_CFG_EVT
     */
    struct a2d_audio_sbc_cfg_param {
        bt_dev_addr_t                  peer_addr; /*!< remote bluetooth device address */
        bt_prf_a2dp_codec_sbc_params_t sbc;       /*!< A2DP media codec capability information */
    } audio_cfg;                                  /*!< media codec configuration information */
} bt_prf_a2dp_cb_param_t;

/**
 * @brief      A2DP event callback function type
 * @param[in]  event         Event type
 * @param[in]  param         Pointer to callback parameter
 */
typedef void (*bt_prf_a2dp_cb_t)(bt_prf_a2dp_cb_event_t event, bt_prf_a2dp_cb_param_t *param);

/**
 * @brief      A2DP data callback function type
 * @param[in]  buf         buffer for a2dp stream pcm data
 * @param[in]  len         len for a2dp stream pcm data
 */
typedef void (*bt_prf_a2dp_sink_data_cb_t)(const uint8_t *buf, uint32_t len);

/**
 * @brief      register event callback function.
 * @param[in]  callback         callback function
 */
void bt_prf_a2dp_register_callback(bt_prf_a2dp_cb_t callback);

/**
 * @brief      register data callback function.
 * @param[in]  callback         callback function
 */
void bt_prf_a2dp_sink_register_data_callback(bt_prf_a2dp_sink_data_cb_t callback);

/**
 * @brief      Initialize the bluetooth A2DP sink module.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_a2dp_sink_init(void);

/**
 * @brief      Connect to remote bluetooth A2DP source.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_a2dp_sink_connect(bt_dev_addr_t *peer_addr);

/**
 * @brief      Disconnect to remote bluetooth A2DP source.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_a2dp_sink_disconnect(bt_dev_addr_t *peer_addr);

#ifdef __cplusplus
}
#endif

#endif /* __BT_STACK_A2DP_H__ */
