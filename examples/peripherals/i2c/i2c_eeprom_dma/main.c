#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_dma.h"
#include "board.h"

#define EEPROM_TRANSFER_LENGTH 32
#define EEPROM_SELECT_PAGE0    (0 << 5)

struct bflb_device_s *i2c0;
struct bflb_device_s *dma0_ch0;
struct bflb_device_s *dma0_ch1;

static ATTR_NOCACHE_NOINIT_RAM_SECTION uint32_t send_buffer[8];
static ATTR_NOCACHE_NOINIT_RAM_SECTION uint32_t receive_buffer[8];
static volatile uint32_t dma_tc_flag0 = 0;
static volatile uint32_t dma_tc_flag1 = 0;

struct bflb_dma_channel_lli_pool_s tx_llipool[20]; /* max trasnfer size 4064 * 20 */

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("Send done\r\n");
}

void dma0_ch1_isr(void *arg)
{
    dma_tc_flag1++;
    printf("Receive done\r\n");
}

int main(void)
{
    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr[2] = { 0x00, EEPROM_SELECT_PAGE0};

    board_init();
    board_i2c0_gpio_init();

    /* Send and receive buffer init */
    for (size_t i = 0; i < 32; i++) {
        ((uint8_t *)send_buffer)[i] = i;
        ((uint8_t *)receive_buffer)[i] = 0;
    }

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);
    bflb_i2c_link_txdma(i2c0, true);
    bflb_i2c_link_rxdma(i2c0, true);

    /* Write page 0 */
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s tx_config;

    tx_config.direction = DMA_MEMORY_TO_PERIPH;
    tx_config.src_req = DMA_REQUEST_NONE;
    tx_config.dst_req = DMA_REQUEST_I2C0_TX;
    tx_config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    tx_config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    tx_config.src_burst_count = DMA_BURST_INCR1;
    tx_config.dst_burst_count = DMA_BURST_INCR1;
    tx_config.src_width = DMA_DATA_WIDTH_32BIT;
    tx_config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch0, &tx_config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    struct bflb_dma_channel_lli_transfer_s tx_transfers[1];
    tx_transfers[0].src_addr = (uint32_t)send_buffer;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_I2C0_TDR;
    tx_transfers[0].nbytes = 32;
    bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 20, tx_transfers, 1);

    msgs[0].addr = 0x50;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;

    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_DMA;
    msgs[1].buffer = NULL;
    msgs[1].length = 32;
    bflb_i2c_transfer(i2c0, msgs, 2);

    bflb_dma_channel_start(dma0_ch0);

    while (dma_tc_flag0 == 0) {
    }
    while ((bflb_i2c_get_intstatus(i2c0) & I2C_INTSTS_END) == 0) {
    }
    bflb_i2c_deinit(i2c0);

    printf("write over\r\n");
    bflb_mtimer_delay_ms(100);

    /* Read page 0 */
    dma0_ch1 = bflb_device_get_by_name("dma0_ch1");

    struct bflb_dma_channel_config_s rx_config;

    rx_config.direction = DMA_PERIPH_TO_MEMORY;
    rx_config.src_req = DMA_REQUEST_I2C0_RX;
    rx_config.dst_req = DMA_REQUEST_NONE;
    rx_config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    rx_config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    rx_config.src_burst_count = DMA_BURST_INCR1;
    rx_config.dst_burst_count = DMA_BURST_INCR1;
    rx_config.src_width = DMA_DATA_WIDTH_32BIT;
    rx_config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch1, &rx_config);

    bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL);

    struct bflb_dma_channel_lli_pool_s rx_llipool[20];
    struct bflb_dma_channel_lli_transfer_s rx_transfers[1];
    rx_transfers[0].src_addr = (uint32_t)DMA_ADDR_I2C0_RDR;
    rx_transfers[0].dst_addr = (uint32_t)receive_buffer;
    rx_transfers[0].nbytes = 32;

    bflb_dma_channel_lli_reload(dma0_ch1, rx_llipool, 20, rx_transfers, 1);

    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_DMA | I2C_M_READ;
    msgs[1].buffer = NULL;
    msgs[1].length = 32;
    bflb_i2c_transfer(i2c0, msgs, 2);

    bflb_dma_channel_start(dma0_ch1);

    while (dma_tc_flag1 == 0) {
    }
    while ((bflb_i2c_get_intstatus(i2c0) & I2C_INTSTS_END) == 0) {
    }
    bflb_i2c_deinit(i2c0);

    printf("read over\r\n");

    /* Check read data */
    for (uint8_t i = 0; i < 32; i++) {
        if (((uint8_t *)send_buffer)[i] != ((uint8_t *)receive_buffer)[i]) {
            printf("check fail, %d write: %02x, read: %02x\r\n", i, ((uint8_t *)send_buffer)[i], ((uint8_t *)receive_buffer)[i]);
        }
    }
    printf("check over\r\n");
    printf("end\r\n");

    while(1){
    }
}
