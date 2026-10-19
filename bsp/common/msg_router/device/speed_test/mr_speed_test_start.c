/**
 * @file mr_speed_test_start.c
 * @brief Speed test framework startup and shell command implementation
 */

#include <string.h>
#include <stdio.h>

#include "board.h"
#include "shell.h"

#include "FreeRTOS.h"

#include "mr_frame_buff_ctrl.h"
#include "mr_msg_ctrl.h"
#include "mr_speed_test.h"
#include "mr_speed_test_start.h"

#define DBG_TAG "SPEED_TEST"
#include "log.h"

/*****************************************************************************
 * Static Variables
 *****************************************************************************/

/** Message controller instance */
static mr_msg_ctrl_priv_t *g_msg_ctrl = NULL;

/** Array of speed test instance pointers */
static speed_test_instance_t *g_speed_test_instances[SPEED_TEST_TEST_MAX] = { NULL };

/** Message tags for each speed test instance */
static uint8_t g_speed_test_tag[SPEED_TEST_TEST_MAX] = { MR_MSG_TAG_TEST_1, MR_MSG_TAG_TEST_2, MR_MSG_TAG_TEST_3 };

#if !SPEED_TEST_ZEROCOPY_LOOPBACK
/** Speed test frame pool controller */
static mr_frame_queue_ctrl_t *g_test_frame_pool;
/** Frame buffer pool for speed test instances */
static FRAME_BUFFER_ATTR uint8_t g_test_frame_buff[TEST_FRAME_NUM][TEST_FRAME_SIZE];
#endif

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

/**
 * @brief Initialize speed test framework
 * @param msg_ctrl Message controller instance
 * @retval None
 * @note This function must be called before any speed test operations
 */
void speed_test_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
    if (msg_ctrl == NULL) {
        LOG_E("msg_ctrl is NULL\r\n");
        return;
    }
    g_msg_ctrl = msg_ctrl;

#if !SPEED_TEST_ZEROCOPY_LOOPBACK
    /* Initialize frame pool configuration */
    mr_frame_queue_ctrl_init_cfg_t pool_cfg = { .frame_buff = g_test_frame_buff,
                                                .frame_type = TEST_FRAME_TYPE,
                                                .frame_elem_cnt = TEST_FRAME_NUM,
                                                .frame_elem_size = TEST_FRAME_SIZE,
                                                .name = "speed_test_pool" };

    /* Create frame pool */
    g_test_frame_pool = mr_frame_queue_create(&pool_cfg);

    if (g_test_frame_pool == NULL) {
        LOG_E("Failed to create speed test frame pool\r\n");
        return;
    }
#else
    LOG_W("Speed test running in zero-copy loopback mode\r\n");
#endif

    LOG_I("Speed test initialized successfully\r\n");
}

/*****************************************************************************
 * Shell Command Functions
 *****************************************************************************/

/**
 * @brief Shell command to create a single speed test instance
 * @param argc Argument count
 * @param argv Argument vector, argv[1] should be instance number (0-2)
 * @retval None
 * @note Shell command: speed_test_create <instance_num>
 */
static void cmd_speed_test_create(int argc, char **argv)
{
    uint8_t msg_tag = 0;
    char instance_name[32];

    /* Check argument count */
    if (argc < 2) {
        LOG_W("Usage: %s <instance_num>\r\n", argv[0]);
        return;
    }

    /* Check if framework is initialized */
    if (g_msg_ctrl == NULL) {
        LOG_E("Speed test not initialized. Call speed_test_init() first.\r\n");
        return;
    }

#if !SPEED_TEST_ZEROCOPY_LOOPBACK
    if (g_test_frame_pool == NULL) {
        LOG_E("Frame pool not created. Cannot create speed test instance.\r\n");
        return;
    }
#endif

    /* Parse instance number */
    msg_tag = atoi(argv[1]);
    if (msg_tag >= SPEED_TEST_TEST_MAX) {
        LOG_W("Invalid instance number, must be less than %d\r\n", SPEED_TEST_TEST_MAX);
        return;
    }

    /* Check if instance already exists */
    if (g_speed_test_instances[msg_tag] != NULL) {
        LOG_W("Instance %d already exists\r\n", msg_tag);
        return;
    }

    /* Create instance name with index */
    snprintf(instance_name, sizeof(instance_name), "speed_test_%d", msg_tag);

    /* Create speed test instance */
#if SPEED_TEST_ZEROCOPY_LOOPBACK
    g_speed_test_instances[msg_tag] = speed_test_create(instance_name, g_msg_ctrl, NULL, g_speed_test_tag[msg_tag]);
#else
    g_speed_test_instances[msg_tag] =
        speed_test_create(instance_name, g_msg_ctrl, g_test_frame_pool, g_speed_test_tag[msg_tag]);
#endif

    /* Check creation result */
    if (g_speed_test_instances[msg_tag] == NULL) {
        LOG_E("Failed to create speed test instance %d\r\n", msg_tag);
    } else {
        LOG_I("Speed test instance %d created successfully\r\n", msg_tag);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_speed_test_create, speed_test_create, cmd speed_test_create);

/**
 * @brief Shell command to destroy a single speed test instance
 * @param argc Argument count
 * @param argv Argument vector, argv[1] should be instance number (0-2)
 * @retval None
 * @note Shell command: speed_test_destroy <instance_num>
 */
static void cmd_speed_test_destroy(int argc, char **argv)
{
    uint8_t msg_tag = 0;

    /* Check argument count */
    if (argc < 2) {
        LOG_W("Usage: %s <instance_num>\r\n", argv[0]);
        return;
    }

    /* Parse instance number */
    msg_tag = atoi(argv[1]);
    if (msg_tag >= SPEED_TEST_TEST_MAX) {
        LOG_W("Invalid instance number, must be less than %d\r\n", SPEED_TEST_TEST_MAX);
        return;
    }

    /* Check if instance exists */
    if (g_speed_test_instances[msg_tag] == NULL) {
        LOG_W("Instance %d does not exist\r\n", msg_tag);
        return;
    }

    /* Destroy instance */
    speed_test_destroy(g_speed_test_instances[msg_tag]);
    g_speed_test_instances[msg_tag] = NULL;

    LOG_I("Speed test instance %d destroyed successfully\r\n", msg_tag);
}

SHELL_CMD_EXPORT_ALIAS(cmd_speed_test_destroy, speed_test_destroy, cmd speed_test_destroy);

/**
 * @brief Shell command to create all speed test instances
 * @param argc Argument count (unused)
 * @param argv Argument vector (unused)
 * @retval None
 * @note Shell command: speed_test_create_all
 */
static void cmd_speed_test_create_all(int argc, char **argv)
{
    LOG_I("Creating all speed test instances...\r\n");

    /* Create all instances by calling cmd_speed_test_create */
    for (int i = 0; i < SPEED_TEST_TEST_MAX; i++) {
        char instance_str[8];
        char *create_argv[2] = { "speed_test_create", instance_str };

        /* Convert instance number to string */
        snprintf(instance_str, sizeof(instance_str), "%d", i);

        /* Call cmd_speed_test_create which handles all checks */
        cmd_speed_test_create(2, create_argv);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_speed_test_create_all, speed_test_create_all, cmd speed_test_create_all);

/**
 * @brief Shell command to destroy all speed test instances
 * @param argc Argument count (unused)
 * @param argv Argument vector (unused)
 * @retval None
 * @note Shell command: speed_test_destroy_all
 */
static void cmd_speed_test_destroy_all(int argc, char **argv)
{
    LOG_I("Destroying all speed test instances...\r\n");

    /* Destroy all existing instances */
    for (int i = 0; i < SPEED_TEST_TEST_MAX; i++) {
        if (g_speed_test_instances[i] != NULL) {
            speed_test_destroy(g_speed_test_instances[i]);
            g_speed_test_instances[i] = NULL;
            LOG_I("Speed test instance %d destroyed successfully\r\n", i);
        }
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_speed_test_destroy_all, speed_test_destroy_all, cmd speed_test_destroy_all);

/**
 * @brief Shell command to show speed test framework status
 * @param argc Argument count (unused)
 * @param argv Argument vector (unused)
 * @retval None
 * @note Shell command: speed_test_status
 */
static void cmd_speed_test_status(int argc, char **argv)
{
    LOG_I("Speed test status:\r\n");
#if !SPEED_TEST_ZEROCOPY_LOOPBACK
    LOG_I("  Initialized: %s\r\n", (g_msg_ctrl != NULL && g_test_frame_pool != NULL) ? "Yes" : "No");
    LOG_I("  Frame pool: %s\r\n", g_test_frame_pool ? "Created" : "Not created");
#endif
    LOG_I("  Instances:\r\n");

    /* Show status of each instance */
    for (int i = 0; i < SPEED_TEST_TEST_MAX; i++) {
        LOG_I("    Instance %d: %s\r\n", i, g_speed_test_instances[i] ? "Running" : "Stopped");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_speed_test_status, speed_test_status, cmd speed_test_status);
