/**
 * @file nethub_types.h
 * @brief nethub framework core data type definitions
 */

#ifndef _NETHUB_TYPES_H
#define _NETHUB_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Configuration files are included by the application itself, not automatically included here */

/**
 * @brief Macro to avoid unused parameter warnings
 * @param param Unused parameter name
 */
#define NH_UNUSED(param)  ((void)param)

/**
 * @brief Macro to calculate array size
 * @param arr Array name
 * @return Number of array elements
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief Network interface type enumeration
 * @note All interface types are defined, configuration controls whether functionality is enabled
 */
typedef enum {
    /* channel */
    NHIF_TYPE_STA = 0,          /**< Station (Client) mode channel */
    NHIF_TYPE_AP,               /**< Access Point mode channel */
    NHIF_TYPE_TCPIPSTA,         /**< TCP/IP STA mode channel */
    NHIF_TYPE_TCPIPAP,          /**< TCP/IP AP mode channel */
    NHIF_TYPE_TCPIPNAT,         /**< TCP/IP NAT mode channel */
    NHIF_TYPE_BRIDGE,           /**< Bridge mode channel */
    /* peripherals device */
    NHIF_TYPE_SDIO,             /**< SDIO transport interface */
    NHIF_TYPE_USB,              /**< USB transport interface */
    NHIF_TYPE_SPI,              /**< SPI transport interface */
    NHIF_TYPE_MAX               /**< Maximum number of channel types */
} nhif_type_t;

/**
 * @brief Network packet buffer structure
 * @note Supports zero-copy data transmission, manages memory through callback functions
 */
typedef struct nh_skb {
    uint8_t *data;              /**< Data buffer pointer */
    uint32_t len;               /**< Data length (bytes) */
    void (*free_cb)(void *priv, void *frame_elem); /**< Callback function to free packet memory */
    void *cb_priv;              /**< Callback function private parameter */
    void *cb_arg;               /**< Callback function parameter */
    struct nh_skb *next;        /**< Next packet in queue */
} nh_skb_t;

/**
 * @brief Forwarding decision result enumeration
 */
typedef enum {
    NH_FORWARD_CONTINUE,        /**< Continue forwarding to next interface */
    NH_FORWARD_STOP,            /**< Stop forwarding, packet has been processed */
    NH_FORWARD_ERROR            /**< Error occurred during forwarding */
} nh_forward_result_t;

/**
 * @brief Forwarding callback function type
 * @param skb Packet buffer
 * @param arg User-defined parameter
 * @return Forwarding decision result
 */
typedef nh_forward_result_t (*nhforward_callback_t)(nh_skb_t *skb, void *arg);

/**
 * @brief Forwarding rule type
 */
typedef enum {
    NH_RULE_TYPE_FORWARD,       /**< Direct forwarding to specified interface */
    NH_RULE_TYPE_CONDITIONAL,   /**< Conditional forwarding, decide whether to forward based on previous interface callback return value */
    NH_RULE_TYPE_BYPASS         /**< Bypass this interface, do not forward to it */
} nh_rule_type_t;

/**
 * @brief Forwarding rule structure for single interface
 */
typedef struct nh_forward_rule {
    nhif_type_t dst_type;                       /**< Destination interface type */
    nh_rule_type_t rule_type;                   /**< Rule type */
    bool is_last_in_chain;                      /**< Whether it is the last in the forwarding chain */
} nh_forward_rule_t;

/**
 * @brief Forwarding rule table for single interface
 */
typedef struct nh_if_forward_table {
    const nh_forward_rule_t *rules;             /**< Forwarding rule array */
    uint32_t rule_count;                        /**< Rule count */
} nh_if_forward_table_t;

/**
 * @brief Global forwarding configuration
 */
typedef struct nh_forward_config {
    const nh_if_forward_table_t *if_tables;     /**< Forwarding rule table array for each interface */
    uint32_t if_count;                          /**< Interface count */
} nh_forward_config_t;

/**
 * @brief Interface state enumeration
 */
typedef enum {
    NHIF_STATE_UNINITIALIZED,   /**< Uninitialized */
    NHIF_STATE_INITIALIZED,     /**< Initialized */
    NHIF_STATE_ACTIVE,          /**< Active state */
    NHIF_STATE_ERROR            /**< Error state */
} nhif_state_t;

/**
 * @brief Error code definitions
 */
typedef enum {
    NH_OK = 0,                  /**< Operation successful */
    NH_ERR_INVALID_PARAM = -1,  /**< Invalid parameter */
    NH_ERR_NOT_FOUND = -2,      /**< Interface not found */
    NH_ERR_ALREADY_EXISTS = -3, /**< Interface already exists */
    NH_ERR_NO_MEMORY = -4,      /**< No memory */
    NH_ERR_NOT_INITIALIZED = -5, /**< Not initialized */
    NH_ERR_INVALID_STATE = -6,  /**< Invalid state */
    NH_ERR_INTERNAL = -7        /**< Internal error */
} nh_error_t;

/**
 * @brief NETHUB network statistics structure
 * @note Used to statistics complete data flow for dnld and upld functions
 */
typedef struct {
    /* === DNLD Download Statistics === */
    /* Producer statistics (transportsdio.c) */
    uint32_t dnld_producer_count;        /**< Total number of packets generated by DNLD producer */

    /* Consumer statistics (port_sta.c) */
    uint32_t dnld_consumer_netif_down;   /**< DNLD consumer 1: Number of packets dropped due to network interface not up */
    uint32_t dnld_consumer_tx_fail;      /**< DNLD consumer 2: Number of packets dropped due to tx failure */
    uint32_t dnld_consumer_free_normal;  /**< DNLD consumer 3: Number of packets released normally */
    uint32_t dnld_consumer_free_abnormal;/**< DNLD consumer 3: Number of packets released abnormally */

    /* DNLD Overall Statistics */
    uint32_t dnld_total_packets;         /**< Total number of packets processed by DNLD */
    uint32_t dnld_total_dropped;         /**< Total number of packets dropped by DNLD */
    uint32_t dnld_total_success;         /**< Total number of packets successfully processed by DNLD */

    /* === UPLD Upload Statistics === */
    /* Producer and flow through statistics */
    uint32_t upld_producer_count;        /**< UPLD producer: Number of packets when wifi data enters system */
    uint32_t upld_flow_through;          /**< Number of UPLD packets flowing through tcpipsta interface */

    /* Consumer statistics */
    uint32_t upld_start_consume;         /**< Number of UPLD packets starting transfer to sdio module */
    uint32_t upld_transfer_complete;     /**< Number of UPLD packets with sdio transfer complete */
    uint32_t upld_free_count;            /**< Number of UPLD packets finally released */

    /* UPLD Overall Statistics */
    uint32_t upld_total_packets;         /**< Total number of packets processed by UPLD */
} nethub_statistics_t;

#endif /* _NETHUB_TYPES_H */
