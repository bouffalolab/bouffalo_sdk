/**
 * @file nh_forward.c
 * @brief nethub forwarding engine implementation
 */

#include "nh_forward.h"
#include "nhif_ops.h"
#include "nethub.h"
#include "nh_log.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Current forwarding configuration
 */
static const nh_forward_config_t *g_current_config = NULL;

/**
 * @brief Set forwarding rule configuration
 */
int nh_forward_set_config(const nh_forward_config_t *config)
{
    if (config == NULL) {
        nh_err("Error: Configuration is NULL\n");
        return NH_ERR_INVALID_PARAM;
    }

    if (nh_forward_validate_config(config) != NH_OK) {
        nh_err("Error: Configuration validation failed\n");
        return NH_ERR_INVALID_PARAM;
    }

    g_current_config = config;
    nh_info("Forwarding configuration set successfully\n");
    return NH_OK;
}

/**
 * @brief Get current forwarding rule configuration
 */
const nh_forward_config_t* nh_forward_get_config(void)
{
    return g_current_config;
}

/**
 * @brief Get forwarding rule table for specified interface
 */
const nh_if_forward_table_t* nh_forward_get_if_table(nhif_type_t src_type)
{
    const nh_forward_config_t *config = nh_forward_get_config();

    if (src_type >= NHIF_TYPE_MAX || src_type >= config->if_count) {
        return NULL;
    }

    return &config->if_tables[src_type];
}

/**
 * @brief Execute packet forwarding
 */
nh_forward_result_t nh_forward_execute(nh_skb_t *skb, nhif_type_t src_type)
{
    const nh_if_forward_table_t *table = nh_forward_get_if_table(src_type);

    if (table == NULL || table->rules == NULL) {
        nh_debug("Warning: Interface %d has no forwarding rules\n", src_type);
        return NH_FORWARD_CONTINUE;
    }

    nh_debug("Starting to execute forwarding rules for interface %d, total %u rules\n",
           src_type, table->rule_count);

    nh_forward_result_t last_result = NH_FORWARD_CONTINUE;

    /* Iterate through forwarding rules */
    for (uint32_t i = 0; i < table->rule_count; i++) {
        const nh_forward_rule_t *rule = &table->rules[i];
        nhif_type_t dst_type = rule->dst_type;

        nh_debug("Processing rule %u: forward to interface %d, type %d\n",
               i, dst_type, rule->rule_type);

        switch (rule->rule_type) {
            case NH_RULE_TYPE_FORWARD:
                /* Direct forwarding */
                {
                    nh_forward_result_t result = nethub_send_output(skb, dst_type);
                    nh_debug("Direct forward to interface %d result: %d\n", dst_type, result);
                    last_result = result;
                }
                break;

            case NH_RULE_TYPE_CONDITIONAL:
                /* Conditional forwarding based on previous processing result */
                if (last_result == NH_FORWARD_CONTINUE) {
                    nh_forward_result_t result = nethub_send_output(skb, dst_type);
                    nh_debug("Conditional forward to interface %d result: %d\n", dst_type, result);
                    last_result = result;
                } else {
                    nh_debug("Skip conditional forward to interface %d, previous result: %d\n",
                           dst_type, last_result);
                }
                break;

            case NH_RULE_TYPE_BYPASS:
                /* Bypass this interface */
                nh_debug("Bypass interface %d\n", dst_type);
                break;

            default:
                nh_err("Error: Unknown rule type %d\n", rule->rule_type);
                break;
        }

        /* If this is the last rule in the chain, end forwarding */
        if (rule->is_last_in_chain) {
            nh_debug("Reached end of forwarding chain, ending forwarding\n");
            break;
        }
    }

    return last_result;
}

/**
 * @brief Print current forwarding rule table
 */
void nh_forward_print_rules(void)
{
    const nh_forward_config_t *config = nh_forward_get_config();

    nh_info("===== Forwarding Rule Table =====\n");
    nh_info("Interface count: %u\n", config->if_count);

    for (uint32_t i = 0; i < config->if_count && i < NHIF_TYPE_MAX; i++) {
        const nh_if_forward_table_t *table = &config->if_tables[i];

        if (table->rules == NULL || table->rule_count == 0) {
            nh_info("Interface %d: No forwarding rules\n", i);
            continue;
        }

        nh_info("Interface %d forwarding rules (%u entries):\n", i, table->rule_count);

        for (uint32_t j = 0; j < table->rule_count; j++) {
            const nh_forward_rule_t *rule = &table->rules[j];
            const char *type_str;

            switch (rule->rule_type) {
                case NH_RULE_TYPE_FORWARD:   type_str = "FORWARD";   break;
                case NH_RULE_TYPE_CONDITIONAL: type_str = "CONDITIONAL"; break;
                case NH_RULE_TYPE_BYPASS:    type_str = "BYPASS";    break;
                default:                     type_str = "UNKNOWN";   break;
            }

            nh_info("  Rule %u: -> Interface %d [%s]%s\n",
                   j, rule->dst_type, type_str,
                   rule->is_last_in_chain ? " [LAST]" : "");

            /* Avoid compiler warnings */
            NH_UNUSED(type_str);
        }
    }
    nh_info("=================== End Forwarding Rule Table ==================\n");
}

/**
 * @brief Validate the validity of forwarding rule configuration
 */
int nh_forward_validate_config(const nh_forward_config_t *config)
{
    if (config == NULL) {
        nh_err("Error: Configuration is NULL\n");
        return NH_ERR_INVALID_PARAM;
    }

    if (config->if_tables == NULL || config->if_count == 0) {
        nh_err("Error: Interface table is NULL or count is 0\n");
        return NH_ERR_INVALID_PARAM;
    }

    if (config->if_count > NHIF_TYPE_MAX) {
        nh_err("Error: Interface count %u exceeds maximum %d\n",
               config->if_count, NHIF_TYPE_MAX);
        return NH_ERR_INVALID_PARAM;
    }

    /* Validate forwarding rules for each interface */
    for (uint32_t i = 0; i < config->if_count && i < NHIF_TYPE_MAX; i++) {
        const nh_if_forward_table_t *table = &config->if_tables[i];

        if (table->rules == NULL) {
            nh_debug(" Warning: Interface %d rule table is empty\n", i);
            continue;
        }

        if (table->rule_count == 0) {
            nh_debug(" Warning: Interface %d rule count is 0\n", i);
            continue;
        }

        /* Validate rule content */
        for (uint32_t j = 0; j < table->rule_count; j++) {
            const nh_forward_rule_t *rule = &table->rules[j];

            if (rule->dst_type >= NHIF_TYPE_MAX) {
                nh_err(" Error: Interface %d rule %u target interface %d is invalid\n",
                       i, j, rule->dst_type);
                return NH_ERR_INVALID_PARAM;
            }

            if (rule->dst_type == i) {
                nh_debug(" Warning: Interface %d rule %u forwards to itself\n", i, j);
            }
        }
    }

    nh_info("Forwarding configuration validation passed\n");
    return NH_OK;
}
