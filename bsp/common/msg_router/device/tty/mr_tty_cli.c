/**
 * @file mr_tty_cli.c
 * @brief TTY CLI Test Implementation
 * @details Example implementation demonstrating TTY device usage with shell commands
 *          - Creates multiple TTY instances (cmd, user_1, user_2, user_3)
 *          - Provides shell commands for sending test data
 *          - Demonstrates proper TTY device initialization and data handling
 */

#include "mm.h"
#include "board.h"
#include "shell.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_tty.h"

#define DBG_TAG "TTY"
#include "log.h"

/** Frame buffer memory attribute (aligned and non-cacheable) */
#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION

#ifdef CONFIG_MR_TTY_CLI_CMD
/* TTY Command Channel Configuration */
#define TTY_CMD_FRAME_CNT  (3)
#define TTY_CMD_FRAME_SIZE (512 + FRAME_BUFF_HEADER_ROOM)
/** Command channel frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t tty_cmd_frame_buff[TTY_CMD_FRAME_CNT][TTY_CMD_FRAME_SIZE];
/** Command channel TTY instance */
static mr_tty_priv_t *g_tty_cmd_priv = NULL;
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_1
/* TTY User Channel 1 Configuration */
#define TTY_USER_1_FRAME_CNT  (6)
#define TTY_USER_1_FRAME_SIZE (1024 + FRAME_BUFF_HEADER_ROOM)
/** User channel 1 frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t tty_user_1_frame_buff[TTY_USER_1_FRAME_CNT][TTY_USER_1_FRAME_SIZE];
/** User channel 1 TTY instance */
static mr_tty_priv_t *g_tty_user_1_priv = NULL;
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_2
/* TTY User Channel 2 Configuration */
#define TTY_USER_2_FRAME_CNT  (6)
#define TTY_USER_2_FRAME_SIZE (1024 + FRAME_BUFF_HEADER_ROOM)
/** User channel 2 frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t tty_user_2_frame_buff[TTY_USER_2_FRAME_CNT][TTY_USER_2_FRAME_SIZE];
/** User channel 2 TTY instance */
static mr_tty_priv_t *g_tty_user_2_priv = NULL;
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_3
/* TTY User Channel 3 Configuration */
#define TTY_USER_3_FRAME_CNT  (6)
#define TTY_USER_3_FRAME_SIZE (1024 + FRAME_BUFF_HEADER_ROOM)
/** User channel 3 frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t tty_user_3_frame_buff[TTY_USER_3_FRAME_CNT][TTY_USER_3_FRAME_SIZE];
/** User channel 3 TTY instance */
static mr_tty_priv_t *g_tty_user_3_priv = NULL;
#endif

/*****************************************************************************
 * TTY Data Processing Callbacks
 *****************************************************************************/
/**
 * @brief Process TTY download data (host to device)
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_msg_pkt Pointer to received message packet
 * @retval 0 Success
 * @details Prints received data in hexadecimal format and frees the buffer
 */
static int tty_cli_dnld_data_output(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_pkt)
{
    uint16_t data_len;

    data_len = MR_TTY_MSG_PACKET_GET_DATA_SIZE(tty_msg_pkt);

    LOG_I("TTY %s dnld %dByte:\r\n", priv->tty_cfg.name, data_len);
    DBG_HEXDUMP(tty_msg_pkt->data, data_len);

    /* Free the buffer and update flow control */
    mr_tty_dnld_elem_free(priv, tty_msg_pkt);

    return 0;
}

/**
 * @brief Send string data via TTY upload channel
 * @param[in] priv Pointer to TTY private structure
 * @param[in] argc Argument count
 * @param[in] argv Argument array
 * @details Concatenates all arguments with spaces and sends to host
 */
static void tty_cli_upld_send_str(mr_tty_priv_t *priv, int argc, char **argv)
{
    int ret;
    int buffer_pos = 0;
    mr_tty_msg_t *tty_msg_pkt;

    /* Verify TTY channel is ready for transmission */
    if (priv->tty_status != MR_TTY_DSTA_DEVICE_RUN || !priv->tty_ready) {
        LOG_E("TTY not ready or not in RUN state (status=%d, ready=%d)\r\n", priv->tty_status, priv->tty_ready);
        return;
    }

    /* Allocate upload message packet */
    ret = mr_tty_upld_elem_alloc(priv, &tty_msg_pkt, 20);
    if (ret < 0) {
        LOG_E("No available TTY upld buffer\r\n");
        return;
    }

    /* Concatenate all arguments with space separator */
    for (int i = 0; i < argc; i++) {
        int arg_len = strlen(argv[i]);

        /* Check if we have enough space for this argument + space + null terminator */
        if (buffer_pos + arg_len + 2 >= MR_TTY_MSG_PACKET_GET_BUFF_SIZE(tty_msg_pkt)) {
            LOG_W("Command line too long, truncating at %d characters\r\n", buffer_pos);
            break;
        }

        /* Add space before argument (except for first one) */
        tty_msg_pkt->data[buffer_pos++] = ' ';

        /* Copy argument */
        memcpy(&tty_msg_pkt->data[buffer_pos], argv[i], arg_len);
        buffer_pos += arg_len;
    }

    /* Add newline at the end */
    if (buffer_pos + 2 < MR_TTY_MSG_PACKET_GET_BUFF_SIZE(tty_msg_pkt)) {
        tty_msg_pkt->data[buffer_pos++] = '\r';
        tty_msg_pkt->data[buffer_pos++] = '\n';
    }

    MR_TTY_MSG_PACKET_SET_DATA_SIZE(tty_msg_pkt, buffer_pos);

    /* Send message packet */
    ret = mr_tty_upld_elem_send(priv, tty_msg_pkt);
    if (ret < 0) {
        LOG_E("Failed to send TTY upld data to msg_ctrl: %d\r\n", ret);
        mr_tty_upld_elem_free(priv, tty_msg_pkt);
    }
}

/**
 * @brief Shell command handler for TTY send
 * @param[in] argc Argument count
 * @param[in] argv Argument array
 * @retval 0 Success
 */
static int cmd_tty_cmd_send(int argc, char **argv)
{
    mr_tty_priv_t *priv = NULL;

#ifdef CONFIG_MR_TTY_CLI_CMD
    if (strcmp(argv[0], "tty_cmd_send") == 0) {
        priv = g_tty_cmd_priv;
    } else
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_1
        if (strcmp(argv[0], "tty_user_1_send") == 0) {
        priv = g_tty_user_1_priv;
    } else
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_2
        if (strcmp(argv[0], "tty_user_2_send") == 0) {
        priv = g_tty_user_2_priv;
    } else
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_3
        if (strcmp(argv[0], "tty_user_3_send") == 0) {
        priv = g_tty_user_3_priv;
    } else
#endif
    {
        LOG_E("Unknown TTY command: %s\r\n", argv[0]);
        return 0;
    }

    if (!priv) {
        LOG_E("TTY not initialized\r\n");
        return 0;
    }

    if (argc < 2) {
        LOG_W("Usage: %s <str_1> <str_2> ...\r\n", argv[0]);
        return 0;
    }

    tty_cli_upld_send_str(priv, argc - 1, &argv[1]);

    return 0;
}

/*****************************************************************************
 * TTY Device Initialization
 * @note Initializes multiple TTY instances for testing
 *****************************************************************************/

/**
 * @brief Initialize all TTY CLI instances
 * @param[in] msg_ctrl Pointer to message controller
 * @details Creates 4 TTY instances: cmd, user_1, user_2, user_3
 */
void tty_cli_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
#ifdef CONFIG_MR_TTY_CLI_CMD
    /* Initialize TTY command channel */
    mr_tty_cfg_t tty_cmd_cfg = {
        .name = "tty_cmd",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_TTY_CMD,
        .upld_frame_size = TTY_CMD_FRAME_SIZE,
        .upld_frame_count = TTY_CMD_FRAME_CNT,
        .upld_frame_buff = tty_cmd_frame_buff,
        .dnld_credit_max = TTY_CMD_FRAME_CNT - 1,
        .upld_credit_update_threshold = TTY_CMD_FRAME_CNT / 2,
        .dnld_output_cb = tty_cli_dnld_data_output,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 15,
        .task_period_max_ms = 100,
    };
    g_tty_cmd_priv = mr_tty_init(&tty_cmd_cfg);
    if (!g_tty_cmd_priv) {
        LOG_E("Failed to init tty cmd\r\n");
    }
    vTaskDelay(10);
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_1
    /* Initialize TTY user channel 1 */
    mr_tty_cfg_t tty_user_1_cfg = {
        .name = "tty_user_1",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_TTY_USER_1,
        .upld_frame_size = TTY_USER_1_FRAME_SIZE,
        .upld_frame_count = TTY_USER_1_FRAME_CNT,
        .upld_frame_buff = tty_user_1_frame_buff,
        .dnld_credit_max = TTY_USER_1_FRAME_CNT - 1,
        .upld_credit_update_threshold = TTY_USER_1_FRAME_CNT / 2,
        .dnld_output_cb = tty_cli_dnld_data_output,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 10,
        .task_period_max_ms = 100,
    };
    g_tty_user_1_priv = mr_tty_init(&tty_user_1_cfg);
    if (!g_tty_user_1_priv) {
        LOG_E("Failed to init tty user_1\r\n");
    }
    vTaskDelay(10);
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_2
    /* Initialize TTY user channel 2 */
    mr_tty_cfg_t tty_user_2_cfg = {
        .name = "tty_user_2",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_TTY_USER_2,
        .upld_frame_size = TTY_USER_2_FRAME_SIZE,
        .upld_frame_count = TTY_USER_2_FRAME_CNT,
        .upld_frame_buff = tty_user_2_frame_buff,
        .dnld_credit_max = TTY_USER_2_FRAME_CNT - 1,
        .upld_credit_update_threshold = TTY_USER_2_FRAME_CNT / 2,
        .dnld_output_cb = tty_cli_dnld_data_output,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 10,
        .task_period_max_ms = 100,
    };
    g_tty_user_2_priv = mr_tty_init(&tty_user_2_cfg);
    if (!g_tty_user_2_priv) {
        LOG_E("Failed to init tty user_2\r\n");
    }
    vTaskDelay(10);
#endif

#ifdef CONFIG_MR_TTY_CLI_USER_3
    /* Initialize TTY user channel 3 */
    mr_tty_cfg_t tty_user_3_cfg = {
        .name = "tty_user_3",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_TTY_USER_3,
        .upld_frame_size = TTY_USER_3_FRAME_SIZE,
        .upld_frame_count = TTY_USER_3_FRAME_CNT,
        .upld_frame_buff = tty_user_3_frame_buff,
        .dnld_credit_max = TTY_USER_3_FRAME_CNT - 1,
        .upld_credit_update_threshold = TTY_USER_3_FRAME_CNT / 2,
        .dnld_output_cb = tty_cli_dnld_data_output,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 10,
        .task_period_max_ms = 100,
    };
    g_tty_user_3_priv = mr_tty_init(&tty_user_3_cfg);
    if (!g_tty_user_3_priv) {
        LOG_E("Failed to init tty user_3\r\n");
    }
    vTaskDelay(10);
#endif
}

/* Shell command exports for TTY testing */
#ifdef CONFIG_MR_TTY_CLI_CMD
SHELL_CMD_EXPORT_ALIAS(cmd_tty_cmd_send, tty_cmd_send, tty send cmd);
#endif
#ifdef CONFIG_MR_TTY_CLI_USER_1
SHELL_CMD_EXPORT_ALIAS(cmd_tty_cmd_send, tty_user_1_send, tty send user_1);
#endif
#ifdef CONFIG_MR_TTY_CLI_USER_2
SHELL_CMD_EXPORT_ALIAS(cmd_tty_cmd_send, tty_user_2_send, tty send user_2);
#endif
#ifdef CONFIG_MR_TTY_CLI_USER_3
SHELL_CMD_EXPORT_ALIAS(cmd_tty_cmd_send, tty_user_3_send, tty send user_3);
#endif
