/* emac and phy */
#include "bflb_mtimer.h"
#include "bflb_emac.h"

#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

/* os */
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"

#include "eth_emac.h"

#define DBG_TAG "ETH_EMAC"
#include "log.h"

#define ETH_EMAC_ALIGN_UP(size, align) (((size) + (align) - 1) & ~((align) - 1))

static void eth_emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc);

/* tx buff def */
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) emac_tx_buff[EMAC_TX_BUFF_CNT][ETH_EMAC_ALIGN_UP((EAMC_BUF_HEAD_SIZE + EMAC_TX_BUFF_SIZE), 32)];
/* rx buff def */
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) emac_rx_buff[EMAC_RX_BUFF_CNT][ETH_EMAC_ALIGN_UP((EAMC_BUF_HEAD_SIZE + EMAC_RX_BUFF_SIZE), 32)];

/* tx pool queue */
static QueueHandle_t emac_tx_pool_queue = NULL;
/* rx process queue */
static QueueHandle_t emac_rx_process_queue = NULL;

/* eth event */
static eth_emac_event_cb_t eth_emac_event_cb = NULL;

/* emac */
static struct bflb_device_s *emac0;
/* eth phy */
static eth_phy_ctrl_t phy_ctrl;

static int g_link_sta = 0;

struct emac_debug_info_s emac_debug_info;

/* phy cfg */
static eth_phy_init_cfg_t phy_cfg = {
    .speed_mode = EPHY_SPEED_MODE_AUTO_NEGOTIATION,
#if (defined(EMAC_SPEED_10M_SUPPORT) && EMAC_SPEED_10M_SUPPORT)
    .local_auto_negotiation_ability = EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX | EPHY_ABILITY_10M_T | EPHY_ABILITY_10M_FULL_DUPLEX,
#else
    .local_auto_negotiation_ability = EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX,
#endif
};

/* emac cfg */
static struct bflb_emac_config_s emac_cfg = {
    .mac_addr = { 0x18, 0xB9, 0x05, 0x12, 0x34, 0x56 },
    .clk_internal_mode = false,
#if defined(BL616L) || defined(BL616D)
    .md_clk_div = 79,
#else
    .md_clk_div = 39,
#endif
    .min_frame_len = (14 + 46 + 4),
    .max_frame_len = (14 + 4 + 1500 + 4),
};

/* tx/rx done callback  */
static void eth_emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    switch (irq_event) {
        case EMAC_IRQ_EVENT_RX_BUSY:
            // LOG_W("rx busy\r\n");
            /* debug */
            emac_debug_info.rx_busy_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_CTRL_FRAME:
            LOG_W("rx ctrl frame, drop.\r\n");
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(emac0, trans_desc);
            /* debug */
            emac_debug_info.rx_push_cnt++;
            emac_debug_info.rx_success_cnt++;
            emac_debug_info.rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_RX_ERR_FRAME:
            LOG_W("rx err frame sta %d, drop.\r\n", trans_desc->err_status);
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(emac0, trans_desc);
            /* debug */
            emac_debug_info.rx_push_cnt++;
            emac_debug_info.rx_error_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_FRAME:
            trans_desc->data_len -= 4; /* remove FCS */
            xQueueSendFromISR(emac_rx_process_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            if (eth_emac_event_cb) {
                eth_emac_event_cb(irq_event);
            }
            /* debug */
            emac_debug_info.rx_success_cnt++;
            emac_debug_info.rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_FRAME:
            xQueueSendFromISR(emac_tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            if (eth_emac_event_cb) {
                eth_emac_event_cb(irq_event);
            }
            /* debug */
            emac_debug_info.tx_success_cnt++;
            emac_debug_info.tx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_ERR_FRAME:
            xQueueSendFromISR(emac_tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            if (eth_emac_event_cb) {
                eth_emac_event_cb(irq_event);
            }
            /* debug */
            if (trans_desc->err_status & (~EMAC_TX_STA_ERR_CS)) {
                LOG_W("tx err sta:%d\r\n", trans_desc->err_status);
                emac_debug_info.tx_error_cnt++;
            } else {
                emac_debug_info.tx_success_cnt++;
                emac_debug_info.tx_total_size += trans_desc->data_len;
            }
            break;

        default:
            break;
    }
}

void eth_emac_event_cb_register(eth_emac_event_cb_t cb)
{
    eth_emac_event_cb = cb;
}

int eth_emac_init(void)
{
    int ret;

    /* emac init */
    emac0 = bflb_device_get_by_name("emac0");
    if (emac0 == NULL) {
        LOG_E("device_get error\r\n");
        return -1;
    }
    bflb_emac_init(emac0, &emac_cfg);
    bflb_emac_irq_attach(emac0, eth_emac_irq_cb, NULL);
    // bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_PROMISCUOUS, false);

    /* scan eth_phy */
    ret = eth_phy_scan(&phy_ctrl, EPHY_ADDR_MIN, EPHY_ADDR_MAX);
    if (ret < 0) {
        return -2;
    }

    /* eth_phy init */
    ret = eth_phy_init(&phy_ctrl, &phy_cfg);
    if (ret < 0) {
        return -3;
    }

    /* LAN8720 Timing Adjustment: When in ref_clk input mode, invert the rx_clk. */
    if ((emac_cfg.clk_internal_mode == false) &&
        (phy_ctrl.phy_drv->phy_id == EPHY_LAN8720_ID)) {
        LOG_W("Invert rx_clk for LAN8720 Timing Adjustment.\r\n");
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_MAC_RX_CLK_INVERT, true);
    }

    LOG_I("TX_BUF_CNT:%d, RX_BUF_CNT:%d\r\n", EMAC_TX_BUFF_CNT, EMAC_RX_BUFF_CNT);

    /* tx pool queue init */
    if (emac_tx_pool_queue == NULL) {
        emac_tx_pool_queue = xQueueCreate(EMAC_TX_BUFF_CNT, sizeof(struct bflb_emac_trans_desc_s));
    }
    for (int i = 0; i < EMAC_TX_BUFF_CNT; i++) {
        struct bflb_emac_trans_desc_s tx_desc = {
            .buff_addr = &emac_tx_buff[i][EAMC_BUF_HEAD_SIZE],
        };
        xQueueSend(emac_tx_pool_queue, &tx_desc, portMAX_DELAY);
    }

    /* rx process queue init */
    if (emac_rx_process_queue == NULL) {
        emac_rx_process_queue = xQueueCreate(EMAC_RX_BUFF_CNT, sizeof(struct bflb_emac_trans_desc_s));
    }

    for (int i = 0; i < EMAC_RX_BUFF_CNT; i++) {
        struct bflb_emac_trans_desc_s rx_desc = {
            .buff_addr = &emac_rx_buff[i][EAMC_BUF_HEAD_SIZE],
        };
        bflb_emac_queue_rx_push(emac0, &rx_desc);
        emac_debug_info.rx_push_cnt += 1;
    }

    /* enable tx and rx */
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, true);
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, true);

    g_link_sta = 0;

    return 0;
}

int eth_emac_deinit(void)
{
    /* disable emac tx/rx */
    if (emac0) {
        bflb_irq_disable(emac0->irq_num);
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, false);
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, false);
        bflb_emac_bd_ctrl_clean(emac0);
    }

    /* disbale ephy */
    eth_phy_ctrl(&phy_ctrl, EPHY_CMD_SET_POWER_DOWN, 0);

    /* delete emac tx/rx queue */
    if (emac_tx_pool_queue) {
        vQueueDelete(emac_tx_pool_queue);
        emac_tx_pool_queue = NULL;
    }
    if (emac_rx_process_queue) {
        vQueueDelete(emac_rx_process_queue);
        emac_rx_process_queue = NULL;
    }

    memset((void *)&emac_debug_info, 0, sizeof(struct emac_debug_info_s));
    g_link_sta = 0;
    return 0;
}

void eth_emac_restart(void)
{
    LOG_W("Eth Emac ReStart (LinkDown) !!!\r\n");

    /* disable tx and rx, and clean tx/rx bd */
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, false);
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, false);
    bflb_emac_bd_ctrl_clean(emac0);
    bflb_mtimer_delay_us(200);

    memset((void *)&emac_debug_info, 0, sizeof(struct emac_debug_info_s));
    g_link_sta = 0;

    /* tx queue reinit */
    xQueueReset(emac_tx_pool_queue);
    for (int i = 0; i < EMAC_TX_BUFF_CNT; i++) {
        struct bflb_emac_trans_desc_s tx_desc = {
            .buff_addr = &emac_tx_buff[i][EAMC_BUF_HEAD_SIZE],
        };
        xQueueSend(emac_tx_pool_queue, &tx_desc, portMAX_DELAY);
    }

    /* rx queue reinit */
    xQueueReset(emac_rx_process_queue);
    for (int i = 0; i < EMAC_RX_BUFF_CNT; i++) {
        struct bflb_emac_trans_desc_s rx_desc = {
            .buff_addr = &emac_rx_buff[i][EAMC_BUF_HEAD_SIZE],
        };
        bflb_emac_queue_rx_push(emac0, &rx_desc);
        emac_debug_info.rx_push_cnt += 1;
    }

    /* enable tx and rx */
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, true);
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, true);
}

bool eth_link_state_update(void)
{
    static int speed_mode = 0;

    int sta = eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_LINK_STA, 0);
    int speed = eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);

    /* link up check */
    if (sta == EPHY_LINK_STA_UP && g_link_sta != EPHY_LINK_STA_UP) {
        LOG_W("Eth Emac LinkUp !!!\r\n");

#if (defined(EMAC_SPEED_10M_SUPPORT) && EMAC_SPEED_10M_SUPPORT)
        /* 10M/100M speed mode */
        if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX || speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
            bflb_emac_feature_control(emac0, EMAC_CMD_SET_SPEED_10M, NULL);
        } else {
            bflb_emac_feature_control(emac0, EMAC_CMD_SET_SPEED_100M, NULL);
        }
#endif
        /* enable full duplex mode */
        if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, true);
        } else {
            bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, false);
        }

        if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 10M_HALF_DUPLEX\r\n");
        } else if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 10M_FULL_DUPLEX\r\n");
        } else if (speed == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 100M_HALF_DUPLEX\r\n");
        } else if (speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 100M_FULL_DUPLEX\r\n");
        }
        speed_mode = speed;
        g_link_sta = sta;

        return true;
    }

    /* link down check */
    if ((sta != EPHY_LINK_STA_UP && g_link_sta == EPHY_LINK_STA_UP) || (sta == EPHY_LINK_STA_UP && speed_mode != speed)) {
        /* */
        if (sta == EPHY_LINK_STA_UP && speed_mode != speed) {
            LOG_W("Eth Emac Speed Mode Has Changed !!!\r\n");
        }
        LOG_W("Eth Emac LinkDown !!!\r\n");

        speed_mode = speed;
        g_link_sta = 0;

        return false;
    }

    if (g_link_sta == EPHY_LINK_STA_UP) {
        return true;
    } else {
        return false;
    }
}

int eth_emac_tx_buff_get(struct bflb_emac_trans_desc_s *trans_desc, uint32_t timeout)
{
    if (xQueueReceive(emac_tx_pool_queue, trans_desc, timeout) == pdFALSE) {
        // LOG_W("no emac tx buff\r\n");
        return -1;
    }

    return 0;
}

int eth_emac_tx_buff_push(struct bflb_emac_trans_desc_s *trans_desc)
{
    trans_desc->attr_flag = 0;
    trans_desc->err_status = 0;

    emac_debug_info.tx_push_cnt += 1;

    int ret = bflb_emac_queue_tx_push(emac0, trans_desc);
    if (ret < 0) {
        LOG_E("emac tx push error\r\n");
        return -1;
    }

    return 0;
}

int eth_emac_rx_data_get(struct bflb_emac_trans_desc_s *trans_desc, uint32_t timeout)
{
    if (xQueueReceive(emac_rx_process_queue, trans_desc, timeout) == pdFALSE) {
        // LOG_W("no emac rx data\r\n");
        return -1;
    }

    return 0;
}

int eth_emac_rx_data_free(struct bflb_emac_trans_desc_s *trans_desc)
{
    trans_desc->attr_flag = 0;
    trans_desc->err_status = 0;

    emac_debug_info.rx_push_cnt += 1;
    int ret = bflb_emac_queue_rx_push(emac0, trans_desc);
    if (ret < 0) {
        LOG_E("emac rx push error\r\n");
        return -1;
    }

    return 0;
}

void eth_eamc_info_dump(void)
{
    uint32_t tx_db_avail = bflb_emac_feature_control(emac0, EMAC_CMD_GET_TX_DB_AVAILABLE, 0);
    uint32_t rx_db_avail = bflb_emac_feature_control(emac0, EMAC_CMD_GET_RX_DB_AVAILABLE, 0);

    if (emac0 == NULL) {
        return;
    }

    LOG_RI("\r\n");
    LOG_I("TX: success cnt:%d, error cnt:%d, total size:%lldByte\r\n", emac_debug_info.tx_success_cnt, emac_debug_info.tx_error_cnt, emac_debug_info.tx_total_size);
    LOG_I("    push_cnt:%d, tx_db waiting:%d, tx_bd_ptr:%d\r\n", emac_debug_info.tx_push_cnt, (EMAC_TX_BD_NUM_MAX - tx_db_avail), bflb_emac_feature_control(emac0, EMAC_CMD_GET_TX_BD_PTR, 0));

    LOG_I("RX: success cnt:%d, error cnt:%d, total size:%lldByte\r\n", emac_debug_info.rx_success_cnt, emac_debug_info.rx_error_cnt, emac_debug_info.rx_total_size);
    LOG_I("    push_cnt:%d, rx_db waiting:%d, rx_bd_ptr:%d, busy cnt:%d\r\n", emac_debug_info.rx_push_cnt, (EMAC_TX_BD_NUM_MAX - rx_db_avail), bflb_emac_feature_control(emac0, EMAC_CMD_GET_RX_BD_PTR, 0), emac_debug_info.rx_busy_cnt);
    LOG_RI("\r\n");
}
