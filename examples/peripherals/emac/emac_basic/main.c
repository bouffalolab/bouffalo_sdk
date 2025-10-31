#include "bflb_mtimer.h"
#include "bflb_emac.h"

#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

struct bflb_device_s *emac0;
eth_phy_ctrl_t phy_ctrl;

volatile uint32_t tx_success_cnt = 0;
volatile uint32_t tx_error_cnt = 0;
volatile uint64_t tx_total_size = 0;

volatile uint32_t rx_success_cnt = 0;
volatile uint32_t rx_error_cnt = 0;
volatile uint32_t rx_busy_cnt = 0;
volatile uint64_t rx_total_size = 0;

static const uint8_t arp_data[42] = {
    /* ARP reply to 192.168.123.178(e4:54:e8:ca:31:16): 192.168.123.100 is at 18:b9:05:12:34:56 */
    0xb0, 0x7b, 0x25, 0x00, 0x89, 0x53,                         // dst mac b0:7b:25:00:89:53
    0x18, 0xB9, 0x05, 0x12, 0x34, 0x56,                         // src mac
    0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02, // arp reply
    0x18, 0xB9, 0x05, 0x12, 0x34, 0x56,                         // src mac
    0xc0, 0xa8, 0x7b, 0x64,                                     // src ip 192.168.123.100
    0xb0, 0x7b, 0x25, 0x00, 0x89, 0x53,                         // dst mac b0:7b:25:00:89:53
    0xc0, 0xa8, 0x7b, 0xb2                                      // dst ip 192.168.123.178
};

ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) uint8_t eth_tx_buff[2 * 1024];
ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) uint8_t eth_rx_buff[2 * 1024];

void emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc)
{
    switch (irq_event) {
        case EMAC_IRQ_EVENT_RX_BUSY:
            rx_busy_cnt++;
            break;
        case EMAC_IRQ_EVENT_RX_FRAME:
            rx_success_cnt++;
            rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_RX_CTRL_FRAME:
            rx_success_cnt++;
            rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_RX_ERR_FRAME:
            LOG_W("rx err sta:%d\r\n", trans_desc->err_status);
            rx_error_cnt++;
            break;

        case EMAC_IRQ_EVENT_TX_FRAME:
            tx_success_cnt++;
            tx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_ERR_FRAME:
            if (trans_desc->err_status & (~EMAC_TX_STA_ERR_CS)) {
                LOG_W("tx err sta:%d\r\n", trans_desc->err_status);
                tx_error_cnt++;
            } else {
                tx_success_cnt++;
                tx_total_size += trans_desc->data_len;
            }

            break;

        default:
            break;
    }
}

int emac_test_init(void)
{
    int ret;

    /* phy cfg */
    eth_phy_init_cfg_t phy_cfg = {
        .speed_mode = EPHY_SPEED_MODE_AUTO_NEGOTIATION,
        .local_auto_negotiation_ability = EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX,
    };

    /* emac cfg */
    struct bflb_emac_config_s emac_cfg = {
        .mac_addr = { 0x18, 0xB9, 0x05, 0x12, 0x34, 0x56 },
        .clk_internal_mode = false,
#if defined(BL616L) || defined(BL616D)
        .md_clk_div = 79,
#else
        .md_clk_div = 39,
#endif
        .min_frame_len = 14 + 46 + 4,
        .max_frame_len = 14 + 1500 + 4,
    };

    /* emac init */
    emac0 = bflb_device_get_by_name("emac0");
    if (emac0 == NULL) {
        LOG_E("device_get error\r\n");
        return -1;
    }
    bflb_emac_init(emac0, &emac_cfg);
    bflb_emac_irq_attach(emac0, emac_irq_cb, NULL);

    /* scan eth_phy */
    ret = eth_phy_scan(&phy_ctrl, EPHY_ADDR_MIN, EPHY_ADDR_MAX);
    if (ret < 0) {
        return -1;
    }

    /* eth_phy init */
    ret = eth_phy_init(&phy_ctrl, &phy_cfg);
    if (ret < 0) {
        return -1;
    }

    /* LAN8720 Timing Adjustment: When in ref_clk input mode, invert the rx_clk. */
    if( (emac_cfg.clk_internal_mode == false) &&
        (phy_ctrl.phy_drv->phy_id == EPHY_LAN8720_ID)) {
        LOG_W("Invert rx_clk for LAN8720 Timing Adjustment.\r\n");
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_MAC_RX_CLK_INVERT, true);
    }

    /* wait link up */
    LOG_I("waiting link_up...\r\n");
    while (eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_LINK_STA, 0) != EPHY_LINK_STA_UP) {
        bflb_mtimer_delay_ms(10);
    }
    LOG_W("EPHY LINK UP\r\n");

    int speed_mode = eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);
    if (speed_mode == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
        LOG_I("eth_phy speed: 10M_HALF_DUPLEX\r\n");
    } else if (speed_mode == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
        LOG_I("eth_phy speed: 10M_FULL_DUPLEX\r\n");
    } else if (speed_mode == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
        LOG_I("eth_phy speed: 100M_HALF_DUPLEX\r\n");
    } else if (speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
        LOG_I("eth_phy speed: 100M_FULL_DUPLEX\r\n");
    }

    if (speed_mode == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, true);
    } else {
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, false);
    }

#if 0
    /* loop back mode */
    eth_phy_ctrl(&phy_ctrl, EPHY_CMD_SET_LOOPBACK_MODE, true);
    LOG_I("eth_phy loopback mode\r\n");

    bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, true);
#endif

    LOG_I("eth_phy init done\r\n\r\n");
    return 0;
}

void emac_test(void)
{
    uint32_t time_node;

    uint32_t tx_cnt_old = 0;
    uint32_t rx_cnt_old = 0;

    uint64_t tx_total_size_old = 0;
    uint64_t rx_total_size_old = 0;

    uint32_t tx_push_cnt = 0;
    uint32_t rx_push_cnt = 0;

    if (emac_test_init() < 0) {
        LOG_E("emac test init falied\r\n");
        return;
    }

    /* tx arp data */
    memcpy(eth_tx_buff, arp_data, sizeof(arp_data));

    struct bflb_emac_trans_desc_s tx_test_desc = {
        .buff_addr = eth_tx_buff,
        .data_len = sizeof(arp_data),
    };

    struct bflb_emac_trans_desc_s rx_test_desc = {
        .buff_addr = eth_rx_buff,
    };

    /* enable tx and rx */
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, true);
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, true);

    time_node = bflb_mtimer_get_time_ms();

    while (1) {
        /* try to push tx */
        if (bflb_emac_feature_control(emac0, EMAC_CMD_GET_TX_DB_AVAILABLE, 0) > 0) {
            if (bflb_emac_queue_tx_push(emac0, &tx_test_desc) == 0) {
                tx_push_cnt += 1;
            }
        }

        /* try to push rx */
        if (bflb_emac_feature_control(emac0, EMAC_CMD_GET_RX_DB_AVAILABLE, 0) > 0) {
            if (bflb_emac_queue_rx_push(emac0, &rx_test_desc) == 0) {
                rx_push_cnt += 1;
            }
        }

        /* get info */
        if (bflb_mtimer_get_time_ms() - time_node > 2 * 1000) {
            time_node = bflb_mtimer_get_time_ms();

            uint32_t tx_db_avail = bflb_emac_feature_control(emac0, EMAC_CMD_GET_TX_DB_AVAILABLE, 0);
            uint32_t rx_db_avail = bflb_emac_feature_control(emac0, EMAC_CMD_GET_RX_DB_AVAILABLE, 0);

            uint64_t tx_size = tx_total_size - tx_total_size_old;
            tx_total_size_old = tx_total_size;
            uint32_t tx_cnt = tx_success_cnt - tx_cnt_old;
            tx_cnt_old = tx_success_cnt;
            LOG_I("TX: Speed: %dMbps, valid_data_speed: %dMbps\r\n", (uint32_t)(tx_cnt * 64 * 8 / 2 / 1000 / 1000), (uint32_t)(tx_size * 8 / 2 / 1000 / 1000));
            LOG_I("    success cnt:%d, error cnt:%d, total size:%lldByte\r\n", tx_success_cnt, tx_error_cnt, tx_total_size);
            LOG_I("    push_cnt:%d, tx_db available:%d\r\n", tx_push_cnt, tx_db_avail);

            uint64_t rx_size = rx_total_size - rx_total_size_old;
            rx_total_size_old = rx_total_size;
            uint32_t rx_cnt = rx_success_cnt - rx_cnt_old;
            rx_cnt_old = rx_success_cnt;
            LOG_I("RX: Speed: %dMbps, valid_data_speed: %dMbps\r\n", (uint32_t)(rx_cnt * 64 * 8 / 2 / 1000 / 1000), (uint32_t)(rx_size * 8 / 2 / 1000 / 1000));
            LOG_I("    success cnt:%d, error cnt:%d, total size:%lldByte\r\n", rx_success_cnt, rx_error_cnt, rx_total_size);
            LOG_I("    push_cnt:%d, rx_db available:%d, busy cnt:%d\r\n", rx_push_cnt, rx_db_avail, rx_busy_cnt);
            LOG_RI("\r\n");

            /* check link sta */
            if (eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_LINK_STA, 0) != EPHY_LINK_STA_UP) {
                LOG_W("EPHY LINK DOWN\r\n");
                /* disable tx and rx, and clean tx/rx bd */
                bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, false);
                bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, false);
                bflb_emac_bd_ctrl_clean(emac0);

                LOG_I("waiting link_up...\r\n");
                while (eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_LINK_STA, 0) != EPHY_LINK_STA_UP) {
                    bflb_mtimer_delay_ms(10);
                }
                LOG_W("EPHY LINK UP\r\n");

                int speed_mode = eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);
                if (speed_mode == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
                    LOG_I("eth_phy speed: 10M_HALF_DUPLEX\r\n");
                } else if (speed_mode == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
                    LOG_I("eth_phy speed: 10M_FULL_DUPLEX\r\n");
                } else if (speed_mode == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
                    LOG_I("eth_phy speed: 100M_HALF_DUPLEX\r\n");
                } else if (speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
                    LOG_I("eth_phy speed: 100M_FULL_DUPLEX\r\n");
                }

                if (speed_mode == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
                    bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, true);
                } else {
                    bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, false);
                }
                /* enable tx and rx */
                bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, true);
                bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, true);

                time_node = bflb_mtimer_get_time_ms();
            }
        }
    }
}

int main(void)
{
    board_init();
    /* emac gpio init */
    board_emac_gpio_init();

    bflb_mtimer_delay_ms(100);

    LOG_I("EMAC ARP Packet test!\r\n");

    emac_test();

    LOG_I("EMAC ARP Packet test end!\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
