#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"

#include "bflb_emac.h"
#include "ethernet_phy.h"

#define EMAC_TEST_INTERVAL       (9)
#define EMAC_TEST_TX_INTERVAL_US (3000)

static const uint8_t test_frame[42] = {
    /* ARP reply to 192.168.123.178(e4:54:e8:ca:31:16): 192.168.123.100 is at 18:b9:05:12:34:56 */
    0xb0, 0x7b, 0x25, 0x00, 0x89, 0x53,                         // dst mac b0:7b:25:00:89:53
    0x18, 0xB9, 0x05, 0x12, 0x34, 0x56,                         // src mac
    0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02, // arp reply
    0x18, 0xB9, 0x05, 0x12, 0x34, 0x56,                         // src mac
    0xc0, 0xa8, 0x7b, 0x64,                                     // src ip 192.168.123.100
    0xb0, 0x7b, 0x25, 0x00, 0x89, 0x53,                         // dst mac b0:7b:25:00:89:53
    0xc0, 0xa8, 0x7b, 0xb2                                      // dst ip 192.168.123.178
};
#define TEST_PATTERN_LEN (ETH_MAX_PACKET_SIZE - 32)
ATTR_NOCACHE_NOINIT_RAM_SECTION static uint8_t test_pattern[TEST_PATTERN_LEN] = { 0 };

static volatile uint32_t tx_pkg_cnt = 0;
static volatile uint32_t tx_err_cnt = 0;
static volatile uint32_t tx_pkg_cnt_last = 0;

static volatile uint32_t rx_pkg_cnt = 0;
static volatile uint32_t rx_err_cnt = 0;

static volatile uint32_t rx_bytes = 0;

static uint32_t time = 0;
static uint32_t last_time = 0;
#define ETH_RXBUFNB 5
#define ETH_TXBUFNB 5

ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethRxBuff[ETH_RXBUFNB][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffers */
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethTxBuff[ETH_TXBUFNB][ETH_TX_BUFFER_SIZE]; /* Ethernet Transmit Buffers */

struct bflb_device_s *emac0;

void emac_isr(int irq, void *arg)
{
    uint32_t int_sts_val;
    uint32_t index = 0;
    int_sts_val = bflb_emac_get_int_status(emac0);
    // printf("emac int:%08lx\r\n", int_sts_val);
    if (int_sts_val & EMAC_INT_STS_TX_DONE) {
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_TX);
        bflb_emac_bd_tx_dequeue(index);
        bflb_emac_int_clear(emac0, EMAC_INT_STS_TX_DONE);
        tx_pkg_cnt++;
    }

    if (int_sts_val & EMAC_INT_STS_TX_ERROR) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_TX_ERROR);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_TX);
        bflb_emac_bd_tx_on_err(index);

        printf("EMAC tx error !!!\r\n");
        tx_err_cnt++;
    }

    if (int_sts_val & EMAC_INT_STS_RX_DONE) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_DONE);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_RX);
        bflb_emac_bd_rx_enqueue(index);

        uint32_t rx_len;
        bflb_emac_bd_rx_dequeue(-1, &rx_len, NULL);

        if (rx_len) {
            rx_pkg_cnt++;
            rx_bytes += rx_len;
        }
    }

    if (int_sts_val & EMAC_INT_STS_RX_ERROR) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_ERROR);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_RX);
        bflb_emac_bd_rx_on_err(index);

        printf("EMAC rx error!!!\r\n");
        rx_err_cnt++;
    }

    if (int_sts_val & EMAC_INT_STS_RX_BUSY) {
        printf("emac rx busy at %s:%d\r\n", __func__, __LINE__);
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_BUSY);
    }
}

int main(void)
{
    struct bflb_emac_config_s emac_cfg = {
        .inside_clk = EMAC_CLK_USE_EXTERNAL,
        .mii_clk_div = 49,
        .min_frame_len = 64,
        .max_frame_len = ETH_MAX_PACKET_SIZE,
        .mac_addr[0] = 0x18,
        .mac_addr[1] = 0xB9,
        .mac_addr[2] = 0x05,
        .mac_addr[3] = 0x12,
        .mac_addr[4] = 0x34,
        .mac_addr[5] = 0x56,
    };

    struct bflb_emac_phy_cfg_s phy_cfg = {
        .auto_negotiation = 1, /*!< Speed and mode auto negotiation */
        .full_duplex = 0,      /*!< Duplex mode */
        .speed = 0,            /*!< Speed mode */
#ifdef PHY_8720
        .phy_address = 1,  /*!< PHY address */
        .phy_id = 0x7c0f0, /*!< PHY OUI, masked */
#else
#ifdef PHY_8201F
        .phy_address = 0, /*!< PHY address */
        .phy_id = 0x120,  /*!< PHY OUI, masked */
#endif
#endif
        .phy_state = PHY_STATE_DOWN,
    };

    uint32_t rx_len = 0;
    uint32_t loop = 0;

    board_init();
    /* emac gpio init */
    board_emac_gpio_init();

    /* emac & BD init and interrupt attach */
    emac0 = bflb_device_get_by_name("emac0");
    bflb_emac_init(emac0, &emac_cfg);
    bflb_emac_bd_init(emac0, (uint8_t *)ethTxBuff, ETH_TXBUFNB, (uint8_t *)ethRxBuff, ETH_RXBUFNB);
    bflb_irq_attach(emac0->irq_num, emac_isr, emac0);
    bflb_emac_int_clear(emac0, EMAC_INT_EN_ALL);
    bflb_emac_int_enable(emac0, EMAC_INT_EN_ALL, 1);

    printf("EMAC ARP Packet test!\r\n");

    /* phy module init */
    ethernet_phy_init(emac0, &phy_cfg);
    printf("ETH PHY init ok!\r\n");
    ethernet_phy_status_get();
    if (PHY_STATE_UP == phy_cfg.phy_state) {
        printf("PHY[%lx] @%d ready on %dMbps, %s duplex\n\r", phy_cfg.phy_id, phy_cfg.phy_address, phy_cfg.speed, phy_cfg.full_duplex ? "full" : "half");
    } else {
        printf("PHY Init fail\n\r");
        while (1) {
            bflb_mtimer_delay_ms(10);
        }
    }

    /* test data */
    memset(test_pattern, 0x5a, TEST_PATTERN_LEN);
    memcpy(test_pattern, test_frame, sizeof(test_frame));

    /* EMAC transmit start */
    printf("EMAC start\r\n");
    bflb_emac_start(emac0);
    bflb_irq_enable(emac0->irq_num);

    while (1) {
        /* start tx queue and rx queue */
        bflb_emac_bd_tx_enqueue(EMAC_NORMAL_PACKET, sizeof(test_frame), test_pattern);
        test_pattern[27]++;
        bflb_emac_bd_rx_dequeue(EMAC_NORMAL_PACKET, &rx_len, NULL);
        bflb_emac_bd_tx_enqueue(EMAC_NORMAL_PACKET, sizeof(test_frame), test_pattern);
        test_pattern[27]++;
        bflb_emac_bd_tx_enqueue(EMAC_NORMAL_PACKET, sizeof(test_frame), test_pattern);
        test_pattern[27]++;
        bflb_emac_bd_rx_dequeue(EMAC_NORMAL_PACKET, &rx_len, NULL);
        loop++;
        if ((loop & 0xfffff) == 0) {
            time = bflb_mtimer_get_time_ms();
            printf("\r\nCurrent Bandwidth: %ldMbps\r\n", (((tx_pkg_cnt - tx_pkg_cnt_last) * 64 * 8) / ((time - last_time) * 1000)));
            printf("tx cnt: %ld, lose cnt: %ld\n\r", tx_pkg_cnt, tx_err_cnt);
            printf("rx cnt: %ld, lose cnt: %ld, bytes: %ld\n\r", rx_pkg_cnt, rx_err_cnt, rx_bytes);
            last_time = time;
            tx_pkg_cnt_last = tx_pkg_cnt;
        }
    }

    while (1) {
        LOG_F("emac basic\r\n");
        LOG_E("emac basic\r\n");
        LOG_W("emac basic\r\n");
        LOG_I("emac basic\r\n");
        LOG_D("emac basic\r\n");
        LOG_T("emac basic\r\n");
        bflb_mtimer_delay_ms(1000);
    }
}
