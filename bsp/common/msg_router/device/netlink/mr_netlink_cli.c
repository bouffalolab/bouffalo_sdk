/**
 * @file mr_netlink_cli.c
 * @brief Netlink CLI Test Implementation
 * @details Example implementation demonstrating Netlink device usage with shell commands
 *          - Creates Netlink instance
 *          - Provides shell commands for sending test data
 *          - Demonstrates proper Netlink device initialization and data handling
 */

#include "mm.h"
#include "board.h"
#include "shell.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netlink.h"

#define DBG_TAG "NETLINK"
#include "log.h"

/** Frame buffer memory attribute (aligned and non-cacheable) */
#define FRAME_BUFFER_ATTR  __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION

/* Netlink Configuration */
#define NETLINK_FRAME_CNT  (6)
#define NETLINK_FRAME_SIZE (1024 + FRAME_BUFF_HEADER_ROOM) /** Max ethernet packet size + overhead */
/** Netlink frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t netlink_frame_buff[NETLINK_FRAME_CNT][NETLINK_FRAME_SIZE];
/** Netlink instance */
static mr_netlink_priv_t *g_netlink_priv = NULL;

/*****************************************************************************
 * Netlink Data Processing Callbacks
 *****************************************************************************/
/**
 * @brief Process Netlink download data (host to device)
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_msg_pkt Pointer to received message packet
 * @retval 0 Success
 * @details Prints received data in hexadecimal format and frees buffer
 */
static int netlink_cli_dnld_data_output(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt)
{
    uint16_t data_len;

    data_len = MR_NETLINK_MSG_PACKET_GET_DATA_SIZE(netlink_msg_pkt);

    LOG_I("NETLINK %s dnld %dByte:\r\n", priv->netlink_cfg.name, data_len);
    DBG_HEXDUMP(netlink_msg_pkt->data, data_len);

    /* Free the buffer and update flow control */
    mr_netlink_dnld_elem_free(priv, netlink_msg_pkt);

    return 0;
}

/**
 * @brief Send string data via Netlink upload channel
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] argc Argument count
 * @param[in] argv Argument array
 * @details Concatenates all arguments with spaces and sends to host
 */
static void netlink_cli_upld_send_str(mr_netlink_priv_t *priv, int argc, char **argv)
{
    int ret;
    int buffer_pos = 0;
    mr_netlink_msg_t *netlink_msg_pkt;

    /* Verify Netlink channel is ready for transmission */
    if (priv->netlink_status != MR_NETLINK_DSTA_DEVICE_RUN) {
        LOG_E("NETLINK not ready or not in RUN state (status=%d)\r\n", priv->netlink_status);
        return;
    }

    /* Allocate upload message packet */
    ret = mr_netlink_upld_elem_alloc(priv, &netlink_msg_pkt, 20);
    if (ret < 0) {
        LOG_E("No available NETLINK upld buffer\r\n");
        return;
    }

    /* Concatenate all arguments with space separator */
    for (int i = 0; i < argc; i++) {
        int arg_len = strlen(argv[i]);

        /* Check if we have enough space for this argument + space + null terminator */
        if (buffer_pos + arg_len + 2 >= MR_NETLINK_MSG_PACKET_GET_BUFF_SIZE(netlink_msg_pkt)) {
            LOG_W("Command line too long, truncating at %d characters\r\n", buffer_pos);
            break;
        }

        /* Add space before argument (except for first one) */
        netlink_msg_pkt->data[buffer_pos++] = ' ';

        /* Copy argument */
        memcpy(&netlink_msg_pkt->data[buffer_pos], argv[i], arg_len);
        buffer_pos += arg_len;
    }

    /* Add newline at end */
    if (buffer_pos + 2 < MR_NETLINK_MSG_PACKET_GET_BUFF_SIZE(netlink_msg_pkt)) {
        netlink_msg_pkt->data[buffer_pos++] = '\r';
        netlink_msg_pkt->data[buffer_pos++] = '\n';
    }

    MR_NETLINK_MSG_PACKET_SET_DATA_SIZE(netlink_msg_pkt, buffer_pos);

    /* Send message packet */
    ret = mr_netlink_upld_elem_send(priv, netlink_msg_pkt);
    if (ret < 0) {
        LOG_E("Failed to send NETLINK upld data to msg_ctrl: %d\r\n", ret);
        mr_netlink_upld_elem_free(priv, netlink_msg_pkt);
    }
}

/**
 * @brief Shell command handler for Netlink send
 * @param[in] argc Argument count
 * @param[in] argv Argument array
 * @retval 0 Success
 */
static int cmd_netlink_send(int argc, char **argv)
{
    mr_netlink_priv_t *priv = g_netlink_priv;

    if (!priv) {
        LOG_E("NETLINK not initialized\r\n");
        return 0;
    }

    if (argc < 2) {
        LOG_W("Usage: netlink_send <str_1> <str_2> ...\r\n");
        return 0;
    }

    netlink_cli_upld_send_str(priv, argc - 1, &argv[1]);

    return 0;
}

/*****************************************************************************
 * Netlink Device Initialization
 * @note Initializes Netlink instance for testing
 *****************************************************************************/

/**
 * @brief Initialize Netlink CLI instance
 * @param[in] msg_ctrl Pointer to message controller
 * @details Creates Netlink instance for testing
 */
void netlink_cli_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
    /* Initialize Netlink */
    mr_netlink_cfg_t netlink_cfg = {
        .name = "netlink",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_NETLINK, /* Use netlink tag */
        .upld_frame_size = NETLINK_FRAME_SIZE,
        .upld_frame_count = NETLINK_FRAME_CNT,
        .upld_frame_buff = netlink_frame_buff,
        .dnld_credit_max = NETLINK_FRAME_CNT - 1,
        .upld_credit_update_threshold = NETLINK_FRAME_CNT / 2,
        .dnld_output_cb = netlink_cli_dnld_data_output,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 10,
        .task_period_max_ms = 100,
    };

    g_netlink_priv = mr_netlink_init(&netlink_cfg);
    if (!g_netlink_priv) {
        LOG_E("Failed to init netlink\r\n");
    }
    vTaskDelay(10);
}

/* Shell command exports for Netlink testing */
SHELL_CMD_EXPORT(cmd_netlink_send, netlink send string data to host);
