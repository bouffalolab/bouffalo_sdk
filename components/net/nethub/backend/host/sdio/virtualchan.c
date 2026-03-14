/**
 * @file mr_virtualchan.c
 * @brief Virtual channel CLI test implementation
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

#include "nh_vchan_backend.h"
#include "nethub_vchan.h"

#define DBG_TAG "VIRTUALCHAN"
#include "log.h"

#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION

#define NETHUB_VCHAN_SLOT_CNT (4)

#pragma pack(push, 1)
struct nethub_vchan_data_hdr {
    uint8_t data_type;
    uint8_t reserved;
    uint16_t len;
    uint8_t data[];
} __attribute__((packed));
#pragma pack(pop)

#define VIRTUALCHAN_FRAME_SIZE (NETHUB_VCHAN_MAX_DATA_LEN \
                                + sizeof(struct nethub_vchan_data_hdr) \
                                + FRAME_BUFF_HEADER_ROOM \
                                + sizeof(mr_virtualchan_msg_t))

static FRAME_BUFFER_ATTR uint8_t virtualchan_frame_buff[NETHUB_VCHAN_SLOT_CNT][VIRTUALCHAN_FRAME_SIZE];

static mr_virtualchan_priv_t *g_virtualchan_priv = NULL;

static nethub_vchan_recv_cb_t g_nethub_vchan_recv_cb[NETHUB_VCHAN_TYPE_MAX] = {NULL};
static void *g_nethub_vchan_recv_arg[NETHUB_VCHAN_TYPE_MAX] = {NULL};

static int nethub_vchan_type_is_valid(nethub_vchan_type_t type)
{
    return (type > 0) && (type < NETHUB_VCHAN_TYPE_MAX);
}

static int virtualchan_dnld_data_output(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt)
{
    struct nethub_vchan_data_hdr *hdr;
    uint16_t data_len;

    data_len = MR_VIRTUALCHAN_MSG_PACKET_GET_DATA_SIZE(virtualchan_msg_pkt);

    hdr = (struct nethub_vchan_data_hdr *)virtualchan_msg_pkt->data;
    if (nethub_vchan_type_is_valid((nethub_vchan_type_t)hdr->data_type)) {
        nethub_vchan_type_t type = (nethub_vchan_type_t)hdr->data_type;
        nethub_vchan_recv_cb_t cb = g_nethub_vchan_recv_cb[type];
        void *arg = g_nethub_vchan_recv_arg[type];

        if (cb) {
            cb(arg, hdr->data, hdr->len);
        }
    } else {
        LOG_E("arg err\r\n");
    }

    LOG_I("VIRTUALCHAN %s dnld %dByte (hdr->len:%d):\r\n",
          priv->virtualchan_cfg.name, data_len, hdr->len);
    DBG_HEXDUMP(virtualchan_msg_pkt->data, data_len);

    mr_virtualchan_dnld_elem_free(priv, virtualchan_msg_pkt);

    return 0;
}

static void virtualchan_show_help(void)
{
    LOG_I("=== Virtual Channel Command Help ===\r\n");
    LOG_I("Usage:\r\n");
    LOG_I("  virtualchan help                    - Show this help information\r\n");
    LOG_I("  virtualchan status                  - Show virtual channel status information\r\n");
    LOG_I("  virtualchan send <type> <len>       - Send sequential data to host\r\n");
    LOG_I("\r\n");
    LOG_I("Data Types:\r\n");
    LOG_I("  user    - User data type (type=%d)\r\n", NETHUB_VCHAN_TYPE_USER);
    LOG_I("  at      - AT command data type (type=%d)\r\n", NETHUB_VCHAN_TYPE_AT);
    LOG_I("  system  - System data type (type=%d)\r\n", NETHUB_VCHAN_TYPE_SYSTEM);
    LOG_I("\r\n");
    LOG_I("Examples:\r\n");
    LOG_I("  virtualchan help                    - Display help information\r\n");
    LOG_I("  virtualchan status                  - Display current status\r\n");
    LOG_I("  virtualchan send user 100           - Send 100 bytes of user data\r\n");
    LOG_I("  virtualchan send at 50              - Send 50 bytes of AT command data\r\n");
    LOG_I("  virtualchan send system 200         - Send 200 bytes of system data\r\n");
    LOG_I("====================================\r\n");
}

static int cmd_virtualchan(int argc, char **argv)
{
    mr_virtualchan_priv_t *priv = g_virtualchan_priv;

    if (!priv) {
        LOG_E("VIRTUALCHAN not initialized\r\n");
        return -1;
    }

    if (argc == 1 || (argc == 2 && strcmp(argv[1], "help") == 0)) {
        virtualchan_show_help();
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "status") == 0) {
        LOG_I("=== VIRTUALCHAN Status ===\r\n");
        LOG_I("name: %s\r\n", priv->virtualchan_cfg.name);
        LOG_I("msg_tag: %d\r\n", priv->virtualchan_cfg.msg_tag);
        LOG_I("status: %d (%s)\r\n", priv->virtualchan_status,
              priv->virtualchan_status == MR_VIRTUALCHAN_DSTA_IDLE ? "IDLE" :
              priv->virtualchan_status == MR_VIRTUALCHAN_DSTA_HOST_READY ? "HOST_READY" :
              priv->virtualchan_status == MR_VIRTUALCHAN_DSTA_DEVICE_RUN ? "DEVICE_RUN" : "UNKNOWN");
        LOG_I("ready: %s\r\n", priv->virtualchan_ready ? "YES" : "NO");
        LOG_I("upld_frame_size: %d\r\n", priv->virtualchan_cfg.upld_frame_size);
        LOG_I("upld_frame_count: %d\r\n", priv->virtualchan_cfg.upld_frame_count);
        LOG_I("upld_frame_type: %d\r\n", priv->virtualchan_cfg.upld_frame_type);
        LOG_I("dnld_credit_max: %d\r\n", priv->virtualchan_cfg.dnld_credit_max);
        LOG_I("upld_credit_update_threshold: %d\r\n", priv->virtualchan_cfg.upld_credit_update_threshold);
        LOG_I("device_dnld_credit_limit: %d\r\n", priv->device_dnld_credit_limit);
        LOG_I("credit_limit_update_last: %d\r\n", priv->credit_limit_update_last);
        LOG_I("task_stack_size: %d\r\n", priv->virtualchan_cfg.task_stack_size);
        LOG_I("task_priority: %d\r\n", priv->virtualchan_cfg.task_priority);
        LOG_I("task_period_max_ms: %d\r\n", priv->virtualchan_cfg.task_period_max_ms);
        LOG_I("stop_requested: %s\r\n", priv->stop_requested ? "YES" : "NO");
        LOG_I("virtualchan_proc_task: %p\r\n", (void *)priv->virtualchan_proc_task);
        LOG_I("upld_frame_ctrl: %p\r\n", (void *)priv->upld_frame_ctrl);
        if (priv->msg_dnld_queue) {
            LOG_I("msg_dnld_queue: %p (waiting=%d)\r\n",
                  (void *)priv->msg_dnld_queue,
                  mr_frame_queue_messages_waiting(priv->msg_dnld_queue));
        }
        LOG_I("==========================\r\n");
        return 0;
    }

    if (argc >= 2 && strcmp(argv[1], "send") == 0) {
        nethub_vchan_type_t data_type;
        int data_len;
        uint8_t *temp_buff;
        int i;
        int ret;

        if (priv->virtualchan_status != MR_VIRTUALCHAN_DSTA_DEVICE_RUN) {
            LOG_E("VIRTUALCHAN not ready or not in RUN state (status=%d)\r\n", priv->virtualchan_status);
            return -1;
        }

        if (argc != 4) {
            LOG_E("Invalid send command format. Usage: virtualchan send <user|at|system> <len>\r\n");
            return -1;
        }

        if (strcmp(argv[2], "user") == 0) {
            data_type = NETHUB_VCHAN_TYPE_USER;
        } else if (strcmp(argv[2], "at") == 0) {
            data_type = NETHUB_VCHAN_TYPE_AT;
        } else if (strcmp(argv[2], "system") == 0) {
            data_type = NETHUB_VCHAN_TYPE_SYSTEM;
        } else {
            LOG_E("Invalid data type: %s (valid types: user, at, system)\r\n", argv[2]);
            return -1;
        }

        data_len = atoi(argv[3]);
        if (data_len <= 0 || data_len > NETHUB_VCHAN_MAX_DATA_LEN) {
            LOG_E("Invalid data length: %d (valid range: 1-%d)\r\n", data_len, NETHUB_VCHAN_MAX_DATA_LEN);
            return -1;
        }

        temp_buff = malloc(data_len);
        if (!temp_buff) {
            LOG_E("Failed to allocate temporary buffer\r\n");
            return -1;
        }

        for (i = 0; i < data_len; i++) {
            temp_buff[i] = (uint8_t)i;
        }

        LOG_I("Preparing to send %d bytes (type=%d): ", data_len, data_type);
        for (i = 0; i < data_len && i < 16; i++) {
            printf("%02X ", temp_buff[i]);
        }
        if (data_len > 16) {
            printf("... (total %d bytes)", data_len);
        }
        printf("\r\n");

        ret = nethub_vchan_send(data_type, temp_buff, data_len);
        free(temp_buff);

        if (ret < 0) {
            LOG_E("Failed to send virtualchan data\r\n");
            return -1;
        }

        LOG_I("VIRTUALCHAN sent %d bytes (type=%d)\r\n", data_len, data_type);
        return 0;
    }

    LOG_W("Invalid command. Use 'virtualchan help' for usage information\r\n");
    return -1;
}

int nethub_vchan_recv_register(nethub_vchan_type_t type,
                               nethub_vchan_recv_cb_t recv_cb,
                               void *arg)
{
    if (nethub_vchan_type_is_valid(type)) {
        g_nethub_vchan_recv_cb[type] = recv_cb;
        g_nethub_vchan_recv_arg[type] = arg;
        return 0;
    }

    return -1;
}

int nethub_vchan_send(nethub_vchan_type_t type, const void *data, uint16_t len)
{
    int ret;
    uint16_t total_len;
    mr_virtualchan_priv_t *priv = g_virtualchan_priv;
    mr_virtualchan_msg_t *virtualchan_msg_pkt;
    struct nethub_vchan_data_hdr *hdr;

    if (!priv || !data || len == 0 || len > NETHUB_VCHAN_MAX_DATA_LEN ||
        !nethub_vchan_type_is_valid(type)) {
        LOG_E("%s: Invalid parameters\r\n", __func__);
        return -1;
    }

    ret = mr_virtualchan_upld_elem_alloc(priv, &virtualchan_msg_pkt, 100);
    if (ret < 0) {
        return -1;
    }

    total_len = sizeof(struct nethub_vchan_data_hdr) + len;
    if (total_len > MR_VIRTUALCHAN_MSG_PACKET_GET_BUFF_SIZE(virtualchan_msg_pkt)) {
        LOG_E("%s upld data size too large: %d > %d\r\n", priv->virtualchan_cfg.name, total_len,
              MR_VIRTUALCHAN_MSG_PACKET_GET_BUFF_SIZE(virtualchan_msg_pkt));
        mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
        return -1;
    }

    hdr = (struct nethub_vchan_data_hdr *)virtualchan_msg_pkt->data;
    memset(hdr, 0, sizeof(struct nethub_vchan_data_hdr));
    hdr->data_type = type;
    hdr->len = len;
    memcpy(hdr->data, data, len);

    MR_VIRTUALCHAN_MSG_PACKET_SET_DATA_SIZE(virtualchan_msg_pkt, total_len);

    ret = mr_virtualchan_upld_elem_send(priv, virtualchan_msg_pkt);
    if (ret < 0) {
        LOG_E("%s Failed to send VIRTUALCHAN upld data to msg_ctrl: %d\r\n", priv->virtualchan_cfg.name, ret);
        mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
        return -1;
    }

    return 0;
}

int nethub_vchan_user_send(const void *data, uint16_t len)
{
    return nethub_vchan_send(NETHUB_VCHAN_TYPE_USER, data, len);
}

int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg)
{
    return nethub_vchan_recv_register(NETHUB_VCHAN_TYPE_USER, recv_cb, cb_arg);
}

int nethub_vchan_at_send(const void *data, uint16_t len)
{
    return nethub_vchan_send(NETHUB_VCHAN_TYPE_AT, data, len);
}

int nethub_vchan_at_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg)
{
    return nethub_vchan_recv_register(NETHUB_VCHAN_TYPE_AT, recv_cb, cb_arg);
}

int nethub_vchan_backend_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
    mr_virtualchan_cfg_t virtualchan_cfg = {
        .name = "virtualchan",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_VIRTUALCHAN,
        .upld_frame_size = VIRTUALCHAN_FRAME_SIZE,
        .upld_frame_count = NETHUB_VCHAN_SLOT_CNT,
        .upld_frame_buff = virtualchan_frame_buff,
        .dnld_credit_max = NETHUB_VCHAN_SLOT_CNT - 1,
        .upld_credit_update_threshold = NETHUB_VCHAN_SLOT_CNT / 2,
        .dnld_output_cb = virtualchan_dnld_data_output,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 10,
        .task_period_max_ms = 100,
    };

    g_virtualchan_priv = mr_virtualchan_init(&virtualchan_cfg);
    if (!g_virtualchan_priv) {
        LOG_E("Failed to init virtualchan\r\n");
        return -1;
    }
    vTaskDelay(10);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_virtualchan, virtualchan, help|status|send <user|at|system> <len> - show help/status or send data);
