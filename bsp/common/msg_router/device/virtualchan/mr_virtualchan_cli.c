/**
 * @file mr_virtualchan_cli.c
 * @brief Virtual channel CLI test implementation
 * @details Demonstrates virtual channel device usage with shell commands:
 *          - Create virtual channel instance
 *          - Provide shell command to send test data
 *          - Show proper virtual channel device initialization and data processing
 */

#include "mm.h"
#include "board.h"
#include "shell.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_virtualchan.h"

#define DBG_TAG "VIRTUALCHAN"
#include "log.h"

/** Frame buffer memory attributes (aligned and non-cacheable) */
#define FRAME_BUFFER_ATTR  __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION

/* Virtual channel configuration */
#define VIRTUALCHAN_FRAME_CNT  (6)
#define VIRTUALCHAN_FRAME_SIZE (1024 + FRAME_BUFF_HEADER_ROOM) /** Maximum Ethernet packet size + overhead */
/** Virtual channel frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t virtualchan_frame_buff[VIRTUALCHAN_FRAME_CNT][VIRTUALCHAN_FRAME_SIZE];
/** Virtual channel instance */
static mr_virtualchan_priv_t *g_virtualchan_priv = NULL;

/*****************************************************************************
 * Virtual Channel Data Processing Callbacks
 *****************************************************************************/
/**
 * @brief Process virtual channel download data (host to device)
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_msg_pkt Pointer to received message packet
 * @retval 0 Success
 * @details Print received data in hexadecimal format and free buffer
 */
static int virtualchan_cli_dnld_data_output(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt)
{
    uint16_t data_len;

    data_len = MR_VIRTUALCHAN_MSG_PACKET_GET_DATA_SIZE(virtualchan_msg_pkt);

    LOG_I("VIRTUALCHAN %s dnld %dByte:\r\n", priv->virtualchan_cfg.name, data_len);
    DBG_HEXDUMP(virtualchan_msg_pkt->data, data_len);

    /* Free buffer and update flow control */
    mr_virtualchan_dnld_elem_free(priv, virtualchan_msg_pkt);

    return 0;
}

/**
 * @brief Send string data through virtual channel upload path
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] argc Argument count
 * @param[in] argv Argument array
 * @details Join all arguments with spaces and send to host
 */
static void virtualchan_cli_upld_send_str(mr_virtualchan_priv_t *priv, int argc, char **argv)
{
    int ret;
    int buffer_pos = 0;
    mr_virtualchan_msg_t *virtualchan_msg_pkt;

    /* Verify virtual channel is ready for transmission */
    if (priv->virtualchan_status != MR_VIRTUALCHAN_DSTA_DEVICE_RUN) {
        LOG_E("VIRTUALCHAN not ready or not in RUN state (status=%d)\r\n", priv->virtualchan_status);
        return;
    }

    /* Allocate upload message packet */
    ret = mr_virtualchan_upld_elem_alloc(priv, &virtualchan_msg_pkt, 20);
    if (ret < 0) {
        LOG_E("No available VIRTUALCHAN upld buffer\r\n");
        return;
    }

    /* Join all arguments with space separator */
    for (int i = 0; i < argc; i++) {
        int arg_len = strlen(argv[i]);

        /* Check if there's enough space for this argument + space + null terminator */
        if (buffer_pos + arg_len + 2 >= MR_VIRTUALCHAN_MSG_PACKET_GET_BUFF_SIZE(virtualchan_msg_pkt)) {
            LOG_W("Command line too long, truncating at %d characters\r\n", buffer_pos);
            break;
        }

        /* Add space before argument (except for first one) */
        virtualchan_msg_pkt->data[buffer_pos++] = ' ';

        /* Copy argument */
        memcpy(&virtualchan_msg_pkt->data[buffer_pos], argv[i], arg_len);
        buffer_pos += arg_len;
    }

    /* Add newline at end */
    if (buffer_pos + 2 < MR_VIRTUALCHAN_MSG_PACKET_GET_BUFF_SIZE(virtualchan_msg_pkt)) {
        virtualchan_msg_pkt->data[buffer_pos++] = '\r';
        virtualchan_msg_pkt->data[buffer_pos++] = '\n';
    }

    MR_VIRTUALCHAN_MSG_PACKET_SET_DATA_SIZE(virtualchan_msg_pkt, buffer_pos);

    /* Send message packet */
    ret = mr_virtualchan_upld_elem_send(priv, virtualchan_msg_pkt);
    if (ret < 0) {
        LOG_E("Failed to send VIRTUALCHAN upld data to msg_ctrl: %d\r\n", ret);
        mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
    }
}

/**
 * @brief Virtual channel send shell command handler function
 * @param[in] argc Argument count
 * @param[in] argv Argument array
 * @retval 0 Success
 */
static int cmd_virtualchan_send(int argc, char **argv)
{
    mr_virtualchan_priv_t *priv = g_virtualchan_priv;

    if (!priv) {
        LOG_E("VIRTUALCHAN not initialized\r\n");
        return 0;
    }

    if (argc < 2) {
        LOG_W("Usage: virtualchan_send <str_1> <str_2> ...\r\n");
        return 0;
    }

    virtualchan_cli_upld_send_str(priv, argc - 1, &argv[1]);

    return 0;
}

/*****************************************************************************
 * Virtual Channel Device Initialization
 * @note Initialize virtual channel instance for testing
 *****************************************************************************/

/**
 * @brief Initialize virtual channel CLI instance
 * @param[in] msg_ctrl Pointer to message controller
 * @details Create virtual channel instance for testing
 */
void virtualchan_cli_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
    /* Initialize virtual channel */
    mr_virtualchan_cfg_t virtualchan_cfg = {
        .name = "virtualchan",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_VIRTUALCHAN, /* Use virtualchan tag */
        .upld_frame_size = VIRTUALCHAN_FRAME_SIZE,
        .upld_frame_count = VIRTUALCHAN_FRAME_CNT,
        .upld_frame_buff = virtualchan_frame_buff,
        .dnld_credit_max = VIRTUALCHAN_FRAME_CNT - 1,
        .upld_credit_update_threshold = VIRTUALCHAN_FRAME_CNT / 2,
        .dnld_output_cb = virtualchan_cli_dnld_data_output,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 10,
        .task_period_max_ms = 100,
    };

    g_virtualchan_priv = mr_virtualchan_init(&virtualchan_cfg);
    if (!g_virtualchan_priv) {
        LOG_E("Failed to init virtualchan\r\n");
    }
    vTaskDelay(10);
}

/* Shell command export for virtual channel testing */
SHELL_CMD_EXPORT(cmd_virtualchan_send, virtualchan send string data to host);
