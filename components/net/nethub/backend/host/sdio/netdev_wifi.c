#include "mm.h"
#include "board.h"
#include "shell.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netdev.h"

#include "msg_ctrl_sdio.h"
#include "netdev_wifi.h"
#include "nh_hub.h"

#ifdef CONFIG_WL80211
#define FHOST_RX_BUF_CNT 20
#else
#include "fhost.h"
#endif

#include <wifi_mgmr.h>
#include <wifi_mgmr_ext.h>

#define DBG_TAG "NETDEV_STA"
#include "log.h"

#define DEBUG_STA_LOG_ENABLE (0)

#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_WIFI_RAM_SECTION

FRAME_BUFFER_ATTR uint8_t netdev_nethub_frame_buff[NETDEV_NETHUB_CNT][NETDEV_NETHUB_FRAME_SIZE];

mr_netdev_priv_t *g_netdev_wifi_priv = NULL;

static void wifi_tx_done_cb(void *arg)
{
    int ret;
    mr_netdev_priv_t *priv = g_netdev_wifi_priv;
    msg_sdio_dnld_fmt_t *dnld_fmt;

    if (!priv || !arg) {
        LOG_E("%s: Invalid parameters\r\n", __func__);
        return;
    }

    dnld_fmt = MSG_SDIO_DNLD_FMT_CONTAINER_OF(((uint8_t *)arg), trans_desc);

    memcpy(dnld_fmt->frame_elem, dnld_fmt->frame_elem_copy, sizeof(dnld_fmt->frame_elem));
    memcpy(dnld_fmt->netdev_msg, dnld_fmt->netdev_msg_copy, sizeof(dnld_fmt->netdev_msg));

#if DEBUG_STA_LOG_ENABLE
    printf("    --- dnld done, pkt:%p\r\n", dnld_fmt->netdev_msg);
#endif

    ret = mr_netdev_dnld_elem_free(priv, (mr_netdev_msg_t *)(dnld_fmt->netdev_msg));
    if (ret < 0) {
        LOG_E("%s: Failed to free dnld elem\r\n", __func__);
    }
}

int netdev_wifi_upld(nethub_frame_t *frame, void *arg)
{
    int ret;
    mr_netdev_priv_t *priv = g_netdev_wifi_priv;
    mr_netdev_msg_t *netdev_msg_pkt;
    mr_frame_elem_t *frame_elem;
    nethub_frame_t *frame_record;

    NETHUB_UNUSED(arg);

    if (!priv || !frame) {
        LOG_E("%s: Invalid parameters\r\n", __func__);
        return -1;
    }

    if (priv->netdev_status != MR_NETDEV_DSTA_DEVICE_RUN) {
        LOG_W("Device not in RUN state, dropping RX frame: frame=%p\r\n", frame);
        goto exit;
    }

    netdev_msg_pkt = (mr_netdev_msg_t *)((uintptr_t)frame->data - sizeof(mr_netdev_msg_t));
    frame_elem = MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(netdev_msg_pkt);

    frame_elem->frame_ctrl = NULL;
    frame_elem->frame_type = 0;
    frame_elem->frame_id = 0;
    frame_elem->buff_addr = (void *)netdev_msg_pkt;
    frame_elem->buff_size = 2048;

    netdev_msg_pkt->msg_pkt.tag = priv->netdev_cfg.msg_tag;
    netdev_msg_pkt->msg_pkt.sub_tag = 0;
    netdev_msg_pkt->flag = MR_NETDEV_FLAG_UPLD_DATA;
    netdev_msg_pkt->credit_update_flag = false;
    netdev_msg_pkt->credit_limit_cnt = 0;

    MR_NETDEV_MSG_PACKET_SET_DATA_SIZE(netdev_msg_pkt, frame->len);

    frame_record = (nethub_frame_t *)((uintptr_t)frame_elem - sizeof(nethub_frame_t));
    memcpy(frame_record, frame, sizeof(nethub_frame_t));

#if DEBUG_STA_LOG_ENABLE
    printf("    ==> upld start %s, pkt=%p, len=%u\r\n", __func__, netdev_msg_pkt, frame->len);
#endif

    ret = mr_netdev_upld_elem_send(priv, netdev_msg_pkt);
    if (ret < 0) {
        LOG_E("%s, Failed to send pkt\r\n", __func__);
        goto exit;
    }

    return 0;

exit:
    if (frame->free_cb) {
        frame->free_cb(frame->cb_arg);
    }
    return 0;
}

static int netdev_dnld_output_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_pkt)
{
    nethub_route_result_t result;
    msg_sdio_dnld_fmt_t *dnld_fmt;
    nethub_frame_t frame = { 0 };

    if (!priv || !netdev_msg_pkt || netdev_msg_pkt->flag != MR_NETDEV_FLAG_DNLD_DATA) {
        LOG_E("%s: Invalid parameters\r\n", __func__);
        return -1;
    }

    dnld_fmt = MSG_SDIO_DNLD_FMT_CONTAINER_OF(netdev_msg_pkt, netdev_msg);

    memcpy(dnld_fmt->frame_elem_copy, dnld_fmt->frame_elem, sizeof(dnld_fmt->frame_elem));
    memcpy(dnld_fmt->netdev_msg_copy, dnld_fmt->netdev_msg, sizeof(dnld_fmt->netdev_msg));

    frame.data = dnld_fmt->eth_payload;
    frame.len = MR_NETDEV_MSG_PACKET_GET_DATA_SIZE(netdev_msg_pkt);
    frame.free_cb = wifi_tx_done_cb;
    frame.cb_arg = dnld_fmt->trans_desc;
    frame.next = NULL;

#if DEBUG_STA_LOG_ENABLE
    printf("<-- dnld start, pkt:%p, len:%u,\r\n", netdev_msg_pkt, frame.len);
#endif

    result = nethub_process_input(&frame, NETHUB_CHANNEL_SDIO);
    if (result == NETHUB_ROUTE_STOP) {
        LOG_W("Data consumed, forwarding stopped\n");
    } else if (result != NETHUB_ROUTE_CONTINUE) {
        LOG_W("Forwarding result abnormal: %d\n", result);
    }

    return 0;
}

static int netdev_upld_done_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_pkt)
{
    mr_frame_elem_t *frame_elem;
    nethub_frame_t *frame_record;

    if (!priv || !netdev_msg_pkt) {
        LOG_E("%s: Invalid parameters\r\n", __func__);
        return -1;
    }

    frame_elem = MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(netdev_msg_pkt);
    frame_record = (nethub_frame_t *)((uintptr_t)frame_elem - sizeof(nethub_frame_t));

    if (frame_record->free_cb) {
        frame_record->free_cb(frame_record->cb_arg);
    } else {
        LOG_E("No frame free callback, memory leak may occur\r\n");
    }

#if DEBUG_STA_LOG_ENABLE
    printf("    === upld done %s, pkt=%p\r\n", __func__, netdev_msg_pkt);
#endif

    return 0;
}

static int netdev_task_cb(mr_netdev_priv_t *priv, uint32_t *notified_value)
{
    NETHUB_UNUSED(priv);
    NETHUB_UNUSED(notified_value);

    return 0;
}

int netdev_wifi_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
    mr_netdev_cfg_t netdev_cfg = {
        .name = "netdev_wifi",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_NETDEV_WIFI_STA,
        .upld_frame_size = NETDEV_NETHUB_FRAME_SIZE,
        .upld_frame_count = NETDEV_NETHUB_CNT,
        .upld_frame_buff = netdev_nethub_frame_buff,
#ifdef BL618DG
        .dnld_credit_max = 18,
        .upld_credit_update_threshold = 14,
#else
        .dnld_credit_max = 8,
        .upld_credit_update_threshold = 6,
#endif
        .dnld_output_cb = netdev_dnld_output_cb,
        .upld_done_cb = netdev_upld_done_cb,
        .task_priority = 27,
        .task_stack_size = 1024,
        .task_period_max_ms = 100,
        .netdev_task_cb = netdev_task_cb,
        .link_up_flag = true,
        .mac = { 0x22, 0x22, 0x22, 0x88, 0x88, 0x88 },
        .ip = { 0, 0, 0, 0 },
    };

    wifi_mgmr_sta_mac_get(netdev_cfg.mac);

    g_netdev_wifi_priv = mr_netdev_init(&netdev_cfg);
    if (!g_netdev_wifi_priv) {
        LOG_E("Failed to init netdev emac\r\n");
        return -1;
    }

    return 0;
}

static int cmd_netdev_info(int argc, const char *argv[])
{
    mr_netdev_priv_t *priv = g_netdev_wifi_priv;

    NETHUB_UNUSED(argc);
    NETHUB_UNUSED(argv);

    if (!priv) {
        LOG_E("netdev not initialized\r\n");
        return -1;
    }

    printf("\r\n========== NETDEV Private Data ==========\r\n");

    printf("\r\n【配置信息】\r\n");
    printf("  name: %s\r\n", priv->netdev_cfg.name ? priv->netdev_cfg.name : "NULL");
    printf("  msg_ctrl: %p\r\n", priv->netdev_cfg.msg_ctrl);
    printf("  msg_tag: 0x%02X\r\n", priv->netdev_cfg.msg_tag);

    printf("\r\n【上传缓冲区配置】\r\n");
    printf("  upld_frame_size: %u bytes\r\n", priv->netdev_cfg.upld_frame_size);
    printf("  upld_frame_count: %u\r\n", priv->netdev_cfg.upld_frame_count);
    printf("  upld_frame_buff: %p\r\n", priv->netdev_cfg.upld_frame_buff);
    printf("  upld_frame_type: %u\r\n", priv->netdev_cfg.upld_frame_type);

    printf("\r\n【流控配置】\r\n");
    printf("  dnld_credit_max: %u\r\n", priv->netdev_cfg.dnld_credit_max);
    printf("  upld_credit_update_threshold: %u\r\n", priv->netdev_cfg.upld_credit_update_threshold);

    printf("\r\n【回调函数】\r\n");
    printf("  dnld_output_cb: %p\r\n", priv->netdev_cfg.dnld_output_cb);
    printf("  upld_done_cb: %p\r\n", priv->netdev_cfg.upld_done_cb);
    printf("  netdev_event: %p\r\n", priv->netdev_cfg.netdev_event);
    printf("  netdev_reset_cb: %p\r\n", priv->netdev_cfg.netdev_reset_cb);

    printf("\r\n【任务配置】\r\n");
    printf("  task_stack_size: %d\r\n", priv->netdev_cfg.task_stack_size);
    printf("  task_priority: %d\r\n", priv->netdev_cfg.task_priority);
    printf("  task_period_max_ms: %u\r\n", priv->netdev_cfg.task_period_max_ms);
    printf("  netdev_task_cb: %p\r\n", priv->netdev_cfg.netdev_task_cb);

    printf("\r\n【网络信息】\r\n");
    printf("  link_up_flag: %s\r\n", priv->netdev_cfg.link_up_flag ? "UP" : "DOWN");
    printf("  mac: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
           priv->netdev_cfg.mac[0], priv->netdev_cfg.mac[1], priv->netdev_cfg.mac[2],
           priv->netdev_cfg.mac[3], priv->netdev_cfg.mac[4], priv->netdev_cfg.mac[5]);
    printf("  ip: %u.%u.%u.%u\r\n",
           priv->netdev_cfg.ip[0], priv->netdev_cfg.ip[1],
           priv->netdev_cfg.ip[2], priv->netdev_cfg.ip[3]);

    printf("\r\n【运行时状态】\r\n");
    printf("  upld_frame_ctrl: %p\r\n", priv->upld_frame_ctrl);
    printf("  netdev_status: %d", priv->netdev_status);
    switch (priv->netdev_status) {
        case MR_NETDEV_DSTA_ERROR:
            printf(" (ERROR)\r\n");
            break;
        case MR_NETDEV_DSTA_IDLE:
            printf(" (IDLE)\r\n");
            break;
        case MR_NETDEV_DSTA_HOST_READY:
            printf(" (HOST_READY)\r\n");
            break;
        case MR_NETDEV_DSTA_DEVICE_RUN:
            printf(" (DEVICE_RUN)\r\n");
            break;
        default:
            printf(" (UNKNOWN)\r\n");
            break;
    }

    printf("\r\n【流控状态】\r\n");
    printf("  device_dnld_credit_limit: %u\r\n", priv->device_dnld_credit_limit);
    printf("  credit_limit_update_last: %u\r\n", priv->credit_limit_update_last);

    printf("\r\n【任务信息】\r\n");
    printf("  netdev_proc_task: %p\r\n", priv->netdev_proc_task);
    printf("  msg_dnld_queue: %p\r\n", priv->msg_dnld_queue);
    printf("  stop_requested: %s\r\n", priv->stop_requested ? "true" : "false");

    printf("\r\n【就绪状态】\r\n");
    printf("  netdev_ready: %s\r\n", priv->netdev_ready ? "true" : "false");

    printf("\r\n==========================================\r\n");

    msg_ctrl_sdio_print_dnld_status();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_netdev_info, netdev_info, show netdev private data);
