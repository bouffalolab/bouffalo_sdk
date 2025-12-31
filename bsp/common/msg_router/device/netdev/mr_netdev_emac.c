/**
 * @file mr_netdev_emac.c
 * @brief EMAC Hardware Integration Layer for Network Device
 * @details This file implements the hardware integration between EMAC and netdev:
 *          - EMAC hardware initialization and configuration
 *          - PHY management and link state monitoring
 *          - TX/RX interrupt handlers
 *          - Frame buffer management for EMAC queues
 *          - Integration with netdev upload/download paths
 */

#include "mm.h"
#include "board.h"
#include "shell.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netdev.h"

#include "bflb_core.h"
#include "bflb_mtimer.h"
#include "bflb_emac.h"
#include "bflb_irq.h"

#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

#define DBG_TAG "NETDEV_EMAC"
#include "log.h"

/*****************************************************************************
 * EMAC Hardware Layer
 *****************************************************************************/

#if defined(BL616) || defined(BL616L) || defined(BL702) || defined(BL808)
#define EMAC_DEVICE_NAME BFLB_NAME_EMAC0
#elif defined(BL616D)
#define EMAC_DEVICE_NAME BFLB_NAME_EMAC_V2_1
#endif

/* Forward declarations for EMAC hardware interface */
static int emac_tx_frame_push(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt);
static void emac_tx_done_isr_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt, bool success);
static int emac_rx_frame_push(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt);
static void emac_rx_done_isr_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt, bool success);

static struct bflb_device_s *g_emac_dev = NULL;
static eth_phy_ctrl_t g_phy_ctrl;
static volatile bool g_link_up_flag = false;
static volatile int g_speed_mode = 0;

/** EMAC configuration */
static struct bflb_emac_config_s g_emac_cfg = {
    .mac_addr = { 0x18, 0xB9, 0x05, 0x12, 0x34, 0x56 },
    .clk_internal_mode = false,
#if defined(BL616L) || defined(BL616D)
    .md_clk_div = 79,
#else
    .md_clk_div = 39,
#endif
    .min_frame_len = 64,
    .max_frame_len = (14 + 4 + 1500 + 4),
};

/** eth phy configuration */
static eth_phy_init_cfg_t g_phy_cfg = {
    .speed_mode = EPHY_SPEED_MODE_AUTO_NEGOTIATION,
#if (defined(EMAC_SPEED_10M_SUPPORT) && EMAC_SPEED_10M_SUPPORT)
    .local_auto_negotiation_ability =
        EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX | EPHY_ABILITY_10M_T | EPHY_ABILITY_10M_FULL_DUPLEX,
#else
    .local_auto_negotiation_ability = EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX,
#endif
};

/**
 * @brief Update Ethernet link state and configure EMAC accordingly
 * @param[in] phy_ctrl Pointer to PHY controller structure
 * @param[in] emac_dev Pointer to EMAC device structure
 * @return true if link is up, false if link is down
 * @details Monitors PHY link state and speed mode, configures EMAC speed/duplex
 *          when link state or speed changes
 */
static bool eth_link_state_update(eth_phy_ctrl_t *phy_ctrl, struct bflb_device_s *emac_dev)
{
    int sta = eth_phy_ctrl(phy_ctrl, EPHY_CMD_GET_LINK_STA, 0);
    int speed = eth_phy_ctrl(phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);

    if (g_link_up_flag == false && sta == EPHY_LINK_STA_UP) {
        LOG_W("Ethernet link up\r\n");

        if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 10M_HALF_DUPLEX\r\n");
        } else if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 10M_FULL_DUPLEX\r\n");
        } else if (speed == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 100M_HALF_DUPLEX\r\n");
        } else if (speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 100M_FULL_DUPLEX\r\n");
        }

#if (defined(EMAC_SPEED_10M_SUPPORT) && EMAC_SPEED_10M_SUPPORT)
        /* 10M/100M speed mode */
        if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX || speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
            bflb_emac_feature_control(emac_dev, EMAC_CMD_SET_SPEED_10M, NULL);
        } else {
            bflb_emac_feature_control(emac_dev, EMAC_CMD_SET_SPEED_100M, NULL);
        }
#endif
        if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            bflb_emac_feature_control(emac_dev, EMAC_CMD_SET_FULL_DUPLEX, true);
        } else {
            bflb_emac_feature_control(emac_dev, EMAC_CMD_SET_FULL_DUPLEX, false);
        }

        g_link_up_flag = true;
        g_speed_mode = speed;

    } else if (g_link_up_flag == true && sta != EPHY_LINK_STA_UP) {
        g_link_up_flag = false;
        LOG_W("Ethernet link down\r\n");
    } else if (g_link_up_flag == true && g_speed_mode != speed) {
        /* Speed mode changed */
        LOG_W("Eth Emac Speed Mode Has Changed !!!\r\n");
        LOG_W("Ethernet link down\r\n");
        g_link_up_flag = false;
        g_speed_mode = 0;
    }

    return g_link_up_flag;
}

/**
 * @brief EMAC interrupt callback handler
 * @param[in] arg Pointer to network device private structure
 * @param[in] irq_event IRQ event type (RX/TX done, error, etc.)
 * @param[in] trans_desc Pointer to transfer descriptor
 * @details Dispatches to appropriate handlers based on event type:
 *          - RX events: Pass received frame to netdev upload path
 *          - TX events: Free transmitted frame to netdev download path
 *          - Error events: Drop frame and log error
 */
static void emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc)
{
    mr_netdev_priv_t *priv = (mr_netdev_priv_t *)arg;
    mr_netdev_msg_t *netdev_msg_packt = NULL;

    if (trans_desc) {
        netdev_msg_packt = (mr_netdev_msg_t *)((uintptr_t)trans_desc->buff_addr - sizeof(mr_netdev_msg_t));
    }

    switch (irq_event) {
        case EMAC_IRQ_EVENT_RX_BUSY:
            // LOG_W("emac rx busy\r\n");
            break;

        /* rx 错误或者控制帧 */
        case EMAC_IRQ_EVENT_RX_CTRL_FRAME:
        case EMAC_IRQ_EVENT_RX_ERR_FRAME:
            MR_NETDEV_MSG_PACKET_SET_DATA_SIZE(netdev_msg_packt, 0);
            emac_rx_done_isr_cb(priv, netdev_msg_packt, false);
            break;
        /* rx 成功 */
        case EMAC_IRQ_EVENT_RX_FRAME:
            MR_NETDEV_MSG_PACKET_SET_DATA_SIZE(netdev_msg_packt, trans_desc->data_len);
            emac_rx_done_isr_cb(priv, netdev_msg_packt, true);
            break;

        /* tx 错误 */
        case EMAC_IRQ_EVENT_TX_ERR_FRAME:
            emac_tx_done_isr_cb(priv, netdev_msg_packt, false);
            break;
        /* tx 成功 */
        case EMAC_IRQ_EVENT_TX_FRAME:
            emac_tx_done_isr_cb(priv, netdev_msg_packt, true);
            break;

        default:
            LOG_W("Unknown EMAC event: %d\r\n", irq_event);
            break;
    }
}

/**
 * @brief EMAC TX completion interrupt callback
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to transmitted packet
 * @param[in] success true if transmission succeeded, false on error
 * @details Releases transmitted packet back to netdev download buffer pool
 */
static void emac_tx_done_isr_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt, bool success)
{
    if (!priv || !netdev_msg_packt) {
        LOG_E("Invalid parameters\r\n");
        return;
    }

    if (success == false) {
        LOG_E("EMAC TX error: %p\r\n", netdev_msg_packt);
    }

    /* 释放来自 msg 的 frame, 注意这是 dnld 的包 */
    mr_netdev_dnld_elem_free(priv, netdev_msg_packt);
}

/**
 * @brief Push frame to EMAC hardware TX queue
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to packet to transmit
 * @retval 0 Success (packet queued for transmission)
 * @retval <0 Error (TX queue full or invalid parameters)
 */
static int emac_tx_frame_push(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt)
{
    int ret;
    struct bflb_emac_trans_desc_s trans_desc;

    if (!priv || !netdev_msg_packt) {
        return -1;
    }

    trans_desc.buff_addr = (void *)netdev_msg_packt->data;
    trans_desc.data_len = MR_NETDEV_MSG_PACKET_GET_DATA_SIZE(netdev_msg_packt);
    trans_desc.attr_flag = 0;
    trans_desc.err_status = 0;

    /* 加入到 emac 硬件发送队列 */
    ret = bflb_emac_queue_tx_push(g_emac_dev, &trans_desc);
    if (ret < 0) {
        LOG_E("Failed to send TX frame: %d\r\n", ret);
        return ret;
    }

    return 0;
}

/**
 * @brief EMAC RX completion interrupt callback
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to received packet
 * @param[in] success true if reception succeeded, false on error
 * @details Sends received packet to host via netdev upload path if link is up
 *          and device is in RUN state, otherwise drops the packet
 */
static void emac_rx_done_isr_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt, bool success)
{
    int ret;

    if (!priv || !netdev_msg_packt) {
        return;
    }

    if (success == false) {
        LOG_W("EMAC RX error, dropping frame: %p\r\n", netdev_msg_packt);
        goto free_exit;
    }
    if (g_link_up_flag == false) {
        LOG_W("EMAC RX received but link is down, dropping frame: %p\r\n", netdev_msg_packt);
        goto free_exit;
    }
    if (priv->netdev_status != MR_NETDEV_DSTA_DEVICE_RUN) {
        LOG_W("EMAC RX received but device not in RUN state, dropping frame: %p\r\n", netdev_msg_packt);
        goto free_exit;
    }

    /* send netdev_msg_packt */
    ret = mr_netdev_upld_elem_send(priv, netdev_msg_packt);
    if (ret < 0) {
        LOG_E("Failed to send packt\r\n");
        goto free_exit;
    }

    return;

free_exit:
    /* 释放packt, 注意这是通过 upld_elem_alloc 获取的 upld packet */
    ret = mr_netdev_upld_elem_free(priv, netdev_msg_packt);
    if (ret < 0) {
        LOG_E("Failed to free upld frame packt\r\n");
    }
    return;
}

/**
 * @brief Push idle buffer to EMAC hardware RX queue
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to empty buffer to receive into
 * @retval 0 Success (buffer queued for reception)
 * @retval <0 Error (RX queue full or invalid parameters)
 */
static int emac_rx_frame_push(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt)
{
    int ret;
    struct bflb_emac_trans_desc_s trans_desc;

    if (!priv || !netdev_msg_packt) {
        return -1;
    }

    trans_desc.buff_addr = netdev_msg_packt->data;
    trans_desc.data_len = 0;
    trans_desc.attr_flag = 0;
    trans_desc.err_status = 0;

    /* 加入到 emac 接收队列 */
    ret = bflb_emac_queue_rx_push(g_emac_dev, &trans_desc);
    if (ret < 0) {
        LOG_E("Failed to push RX frame: %d\r\n", ret);
        return ret;
    }

    return 0;
}

/**
 * @brief Initialize EMAC hardware and PHY
 * @retval 0 Success
 * @retval <0 Error (device not found, PHY scan failed, or initialization error)
 * @details Initializes EMAC hardware, scans for PHY, and applies chip-specific timing
 */
static int emac_phy_init(void)
{
    int ret;

    /* get device */
    g_emac_dev = bflb_device_get_by_name(EMAC_DEVICE_NAME);
    if (!g_emac_dev) {
        LOG_E("Failed to get emac device\r\n");
        return -1;
    }

    /* gpio init */
#if defined(BL616D)
    if (g_emac_dev->idx == 1) {
        board_emac1_gpio_init();
    } else
#endif
    {
        board_emac_gpio_init();
    }

    /* 初始化EMAC硬件 */
    bflb_emac_init(g_emac_dev, &g_emac_cfg);
    bflb_emac_feature_control(g_emac_dev, EMAC_CMD_SET_RX_PROMISCUOUS, false);
    /* enable tx and rx */
    bflb_emac_feature_control(g_emac_dev, EMAC_CMD_SET_TX_EN, true);
    bflb_emac_feature_control(g_emac_dev, EMAC_CMD_SET_RX_EN, true);

    /* 初始化PHY */
    ret = eth_phy_scan(&g_phy_ctrl, EPHY_ADDR_MIN, EPHY_ADDR_MAX);
    if (ret < 0) {
        LOG_E("Failed to scan PHY: %d\r\n", ret);
        return -1;
    }
    ret = eth_phy_init(&g_phy_ctrl, &g_phy_cfg);
    if (ret < 0) {
        LOG_E("Failed to initialize PHY: %d\r\n", ret);
        return -1;
    }
    /* LAN8720时序调整 */
    if ((g_emac_cfg.clk_internal_mode == false) && (g_phy_ctrl.phy_drv->phy_id == EPHY_LAN8720_ID)) {
        LOG_W("Applying LAN8720 timing adjustment\r\n");
        bflb_emac_feature_control(g_emac_dev, EMAC_CMD_SET_MAC_RX_CLK_INVERT, true);
    }

    return 0;
}

/**
 * @brief Enable EMAC interrupts and attach callback
 * @param[in] arg Pointer to network device private structure
 */
static void emac_int_start(void *arg)
{
    /* enable emac interrupt */
    bflb_emac_irq_attach(g_emac_dev, emac_irq_cb, arg);
}

/*****************************************************************************
 * NETDEV Integration Layer
 *****************************************************************************/

/** Frame buffer memory attribute (aligned and non-cacheable) */
#define FRAME_BUFFER_ATTR      __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION

/* NETDEV Channel Configuration */
#define NETDEV_EMAC_FRAME_CNT  (8)                                /**< Total frame buffer count */
#define NETDEV_EMAC_FRAME_SIZE (512 * 3 + FRAME_BUFF_HEADER_ROOM) /**< Frame buffer size */
/** NETDEV frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t netdev_emac_frame_buff[NETDEV_EMAC_FRAME_CNT][NETDEV_EMAC_FRAME_SIZE];
/** NETDEV channel instance */
static mr_netdev_priv_t *g_netdev_emac_priv = NULL;

/* Reserve buffers for control messages, push idle buffers to EMAC RX queue */
#define NETDEV_EMAC_CMD_FRAME_CNT (2) /**< Reserved buffers for control messages */

/**
 * @brief NETDEV download output callback
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to download packet from host
 * @retval 0 Success (packet pushed to EMAC TX queue)
 * @retval <0 Error (link down or TX queue full)
 * @details Called when packet received from host, pushes to EMAC for transmission
 */
static int netdev_dnld_output_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt)
{
    if (g_link_up_flag == false) {
        LOG_W("Ethernet link is down, dropping frame\r\n");
        return -1;
    }

    return emac_tx_frame_push(priv, netdev_msg_packt);
}

/**
 * @brief NETDEV upload done callback
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to uploaded packet
 * @retval 0 Success (packet pushed to EMAC RX queue or freed)
 * @retval <0 Error
 * @details Called after packet sent to host, recycles buffer to EMAC RX queue
 *          if link is up and sufficient buffers available for control messages
 */
static int netdev_upld_done_cb(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt)
{
    int ret;

    if (g_link_up_flag == false) {
        goto free_exit;
    }

    if (MR_NETDEV_UPLD_BUFF_GET_CNT(priv) <= NETDEV_EMAC_CMD_FRAME_CNT) {
        goto free_exit;
    }

    return emac_rx_frame_push(priv, netdev_msg_packt);

free_exit:
    /* 释放packt, 注意这是通过 upld_elem_alloc 获取的 upld packet */
    ret = mr_netdev_upld_elem_free(priv, netdev_msg_packt);
    if (ret < 0) {
        LOG_E("Failed to free upld frame packt\r\n");
    }
    return ret;
}

/**
 * @brief NETDEV periodic task callback
 * @param[in] priv Pointer to network device private structure
 * @param[in,out] notified_value Pointer to task notification value
 * @retval 0 Success
 * @details Called periodically by netdev daemon thread:
 *          - Updates PHY link state every 100ms
 *          - Notifies netdev layer of link state changes
 *          - Pushes idle buffers to EMAC RX queue when link is up
 */
static int netdev_task_cb(mr_netdev_priv_t *priv, uint32_t *notified_value)
{
    static uint32_t ephy_update_ms = 0;
    static bool link_up_last = false;

    /* update ephy status */
    if (bflb_mtimer_get_time_ms() - ephy_update_ms > 100) {
        ephy_update_ms = bflb_mtimer_get_time_ms();
        eth_link_state_update(&g_phy_ctrl, g_emac_dev);

        if (link_up_last != g_link_up_flag) {
            mr_netdev_set_link_up(priv, g_link_up_flag);
            link_up_last = g_link_up_flag;
        }
    }

    if (g_link_up_flag == false) {
        return 0;
    }

    /* 将空闲的 buff 放入 emac 硬件接收队列 */
    int upld_buff_cnt = MR_NETDEV_UPLD_BUFF_GET_CNT(priv);
    if (upld_buff_cnt <= NETDEV_EMAC_CMD_FRAME_CNT) {
        return 0;
    }

    upld_buff_cnt -= NETDEV_EMAC_CMD_FRAME_CNT;
    for (int i = 0; i < upld_buff_cnt; i++) {
        mr_netdev_msg_t *netdev_msg_packt = NULL;
        int ret = mr_netdev_upld_elem_alloc(priv, &netdev_msg_packt, 0);
        if (ret < 0) {
            break;
        }

        ret = emac_rx_frame_push(priv, netdev_msg_packt);
        if (ret < 0) {
            mr_netdev_upld_elem_free(priv, netdev_msg_packt);
            break;
        }
    }

    return 0;
}

/**
 * @brief Initialize EMAC-based network device
 * @param[in] msg_ctrl Pointer to message controller instance
 * @details Initializes EMAC hardware, PHY, and netdev layer with:
 *          - Frame buffer pool (8 x 1536 bytes)
 *          - Download credit limit (7 frames)
 *          - Upload credit update threshold (4 frames)
 *          - Daemon thread for link monitoring and buffer management
 */
void netdev_emac_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
    if (emac_phy_init() < 0) {
        return;
    }

    /* netdev init */
    mr_netdev_cfg_t netdev_cfg = {
        .name = "netdev_emac",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_NETDEV_ETH,
        .upld_frame_size = NETDEV_EMAC_FRAME_SIZE,
        .upld_frame_count = NETDEV_EMAC_FRAME_CNT,
        .upld_frame_buff = netdev_emac_frame_buff,
        .dnld_credit_max = NETDEV_EMAC_FRAME_CNT - 1,
        .upld_credit_update_threshold = NETDEV_EMAC_FRAME_CNT / 2,

        .dnld_output_cb = netdev_dnld_output_cb,
        .upld_done_cb = netdev_upld_done_cb,

        .task_priority = 16,
        .task_stack_size = 1024,
        .task_period_max_ms = 100,
        .netdev_task_cb = netdev_task_cb,

        .link_up_flag = false,
        .mac = { 0x18, 0xB9, 0x05, 0x12, 0x34, 0x56 },
        .ip = { 0, 0, 0, 0 },
    };
    memcpy(netdev_cfg.mac, g_emac_cfg.mac_addr, 6);

    g_netdev_emac_priv = mr_netdev_init(&netdev_cfg);
    if (!g_netdev_emac_priv) {
        LOG_E("Failed to init netdev emac\r\n");
        return;
    }

    /* enable emac interrupt */
    emac_int_start(g_netdev_emac_priv);

    return;
}

#ifdef CONFIG_SHELL
#include "shell.h"

static int netdev_update_mac(int argc, char **argv)
{
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(netdev_update_mac, netdev_update_mac, update netdev mac address);

static int netdev_update_ip(int argc, char **argv)
{
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(netdev_update_ip, netdev_update_ip, update netdev ip address);

#endif