/**
 * @file sdiowifi_rules.c
 * @brief Forward rules configuration for SDIO WiFi application
 */

#include <stddef.h>
#include "../include/nethub_types.h"

/* STA interface forward rules: STA -> TCPIPSTA -> SDIO */
static nh_forward_rule_t sta_rules[] = {
    {NHIF_TYPE_TCPIPSTA, NH_RULE_TYPE_CONDITIONAL, false},
    {NHIF_TYPE_SDIO, NH_RULE_TYPE_FORWARD, true}
};

/* AP interface forward rules: AP -> TCPIPAP */
static nh_forward_rule_t ap_rules[] = {
    {NHIF_TYPE_TCPIPAP, NH_RULE_TYPE_FORWARD, true}
};

/* TCPIPSTA interface forward rules: TCPIPSTA -> STA */
static nh_forward_rule_t tcpipsta_rules[] = {
    {NHIF_TYPE_STA, NH_RULE_TYPE_FORWARD, true}
};

/* TCPIPAP interface forward rules: TCPIPAP -> AP */
static nh_forward_rule_t tcpipap_rules[] = {
    {NHIF_TYPE_AP, NH_RULE_TYPE_FORWARD, true}
};

/* SDIO interface forward rules: SDIO -> STA */
static nh_forward_rule_t sdio_rules[] = {
    {NHIF_TYPE_STA, NH_RULE_TYPE_FORWARD, true}
};

/* Forward rule table */
static const nh_if_forward_table_t sdiowifi_if_tables[NHIF_TYPE_MAX] = {
    [NHIF_TYPE_STA] = {sta_rules, ARRAY_SIZE(sta_rules)},
    [NHIF_TYPE_AP] = {ap_rules, ARRAY_SIZE(ap_rules)},
    [NHIF_TYPE_TCPIPSTA] = {tcpipsta_rules, ARRAY_SIZE(tcpipsta_rules)},
    [NHIF_TYPE_TCPIPAP] = {tcpipap_rules, ARRAY_SIZE(tcpipap_rules)},
    [NHIF_TYPE_SDIO] = {sdio_rules, ARRAY_SIZE(sdio_rules)},
    /* Other interfaces have no forward rules */
    [NHIF_TYPE_TCPIPNAT] = {NULL, 0},
    [NHIF_TYPE_BRIDGE] = {NULL, 0},
    [NHIF_TYPE_USB] = {NULL, 0},
    [NHIF_TYPE_SPI] = {NULL, 0},
};

/**
 * @brief Get SDIO WiFi forward configuration
 */
const nh_forward_config_t* sdiowifi_get_forward_config(void)
{
    static const nh_forward_config_t config = {
        .if_tables = sdiowifi_if_tables,
        .if_count = NHIF_TYPE_MAX
    };

    return &config;
}