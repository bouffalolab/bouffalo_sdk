/**
 * @file nethub.c
 * @brief nethub framework core implementation
 */

#include "nethub.h"
#include "nh_log.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief nethub framework status
 */
static struct {
    bool initialized;                           /**< Whether initialized */
    bool started;                              /**< Whether started */
    const nh_forward_config_t *config;         /**< Current forwarding configuration */
} g_nethub_ctx = {0};


/**
 * @brief nethub framework initialization
 */
int nethub_init(const nh_forward_config_t *config)
{
    nh_info("Initializing nethub framework\n");

    if (g_nethub_ctx.initialized) {
        nh_warn("Warning: Framework already initialized\n");
        return NH_ERR_ALREADY_EXISTS;
    }

    /* Must provide forwarding configuration */
    if (config == NULL) {
        nh_err("Error: Forwarding configuration must be provided\n");
        return NH_ERR_INVALID_PARAM;
    }

    /* Validate configuration */
    if (nh_forward_validate_config(config) != NH_OK) {
        nh_err("Error: Invalid forwarding configuration\n");
        return NH_ERR_INVALID_PARAM;
    }

    g_nethub_ctx.config = config;
    /* Set forwarding engine configuration */
    nh_forward_set_config(config);
    nh_info("Using custom forwarding configuration\n");

    g_nethub_ctx.initialized = true;
    g_nethub_ctx.started = false;

    nh_info("nethub framework initialization completed\n");
    return NH_OK;
}

/**
 * @brief nethub framework deinitialization
 */
int nethub_deinit(void)
{
    nh_info("Deinitializing nethub framework\n");

    if (!g_nethub_ctx.initialized) {
        return NH_ERR_NOT_INITIALIZED;
    }

    if (g_nethub_ctx.started) {
        nethub_stop();
    }

    g_nethub_ctx.initialized = false;
    g_nethub_ctx.config = NULL;

    nh_info("nethub framework deinitialization completed\n");
    return NH_OK;
}

/**
 * @brief Start nethub framework
 */
int nethub_start(void)
{
    nh_info("Starting nethub framework\n");

    if (!g_nethub_ctx.initialized) {
        nh_err("Error: Framework not initialized\n");
        return NH_ERR_NOT_INITIALIZED;
    }

    if (g_nethub_ctx.started) {
        nh_warn("Warning: Framework already started\n");
        return NH_OK;
    }

    /* Initialize all registered interfaces */
    int ret = nhif_init_all();
    if (ret != NH_OK) {
        nh_err("Error: Interface initialization failed %d\n", ret);
        return ret;
    }

    g_nethub_ctx.started = true;
    nh_info("nethub framework started\n");
    return NH_OK;
}

/**
 * @brief Stop nethub framework
 */
int nethub_stop(void)
{
    nh_info("Stopping nethub framework\n");

    if (!g_nethub_ctx.started) {
        return NH_OK;
    }

    /* TODO: Stop all interfaces */

    g_nethub_ctx.started = false;
    nh_info("nethub framework stopped\n");
    return NH_OK;
}

/**
 * @brief Process interface input data
 */
nh_forward_result_t nethub_process_input(nh_skb_t *skb, nhif_type_t src_type)
{
    if (!g_nethub_ctx.initialized || !g_nethub_ctx.started) {
        nh_err("Error: Framework not initialized or not started\n");
        return NH_FORWARD_ERROR;
    }

    if (skb == NULL) {
        nh_err("Error: Invalid packet\n");
        return NH_FORWARD_ERROR;
    }

    nh_debug("Processing packet from interface %d, length %u\n", src_type, skb->len);

    /* Execute forwarding */
    nh_forward_result_t result = nh_forward_execute(skb, src_type);
    return result;
}

/**
 * @brief Send data to specified interface
 */
nh_forward_result_t nethub_send_output(nh_skb_t *skb, nhif_type_t dst_type)
{
    if (!g_nethub_ctx.initialized || !g_nethub_ctx.started) {
        nh_err("Error: Framework not initialized or not started\n");
        return NH_FORWARD_ERROR;
    }

    if (skb == NULL) {
        nh_err("Error: Invalid packet\n");
        return NH_FORWARD_ERROR;
    }

    /* Find target interface */
    nhif_instance_t *instance = nhif_find_by_type(dst_type);
    if (instance == NULL) {
        nh_err("Error: Target interface %d not found\n", dst_type);
        return NH_FORWARD_ERROR;
    }

    if (instance->ops == NULL || instance->ops->output == NULL) {
        nh_err("Error: Interface %d has no output callback function\n", dst_type);
        return NH_FORWARD_ERROR;
    }

    nh_debug("Sending packet to interface %d, length %u\n", dst_type, skb->len);

    /* Call interface output callback */
    return instance->ops->output(skb, instance->private_data);
}

/**
 * @brief Get nethub framework status
 */
bool nethub_is_initialized(void)
{
    return g_nethub_ctx.initialized;
}

void nethub_dumpbuf(uint8_t *str, uint8_t *buf, int len)
{
    if (str) {
        printf("%s :\n", str);
    }

    for (int i = 0; i < len; i += 32) {
        // Display current line start address
        printf("0x%08X: ", (uint32_t)(buf + i));

        // Process current line data (maximum 32 bytes)
        int line_len = (len - i) > 32 ? 32 : (len - i);

        for (int j = 0; j < line_len; j++) {
            // Group every 4 bytes with 2 spaces between groups
            if (j > 0 && j % 4 == 0) {
                printf("  ");
            }
            printf(" %02X", buf[i + j]);
        }
        printf("\n");
    }
}
