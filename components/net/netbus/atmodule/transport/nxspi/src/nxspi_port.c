#include <stdio.h>
#include <nxspi.h>
#include <nxspi_log.h>

#include "bl616_glb.h"
#include "bflb_clock.h"
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <bflb_dma.h>
#include <bflb_spi.h>
#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include <hardware/spi_reg.h>
#include "bflb_gpio.h"
#include "hardware/gpio_reg.h"

#include "bflb_dma.h"
#include "bflb_l1c.h"
#include "hardware/dma_reg.h"

struct nx_dma_channel_lli_transfer_s {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nbytes;

    uint8_t si;
    uint8_t di;
    uint8_t i;
};

extern spi_header_t dn_header;
extern spi_header_t up_header;

extern  nxspi_desc_t g_nxspi;
extern struct bflb_dma_channel_lli_pool_s txlli_pool[4];
extern struct bflb_dma_channel_lli_pool_s rxlli_pool[4];
uint32_t ATTR_NOCACHE_RAM_SECTION g_black_hole = 0xaabbccdd;//_attribute__((section(".wifi_ram"))) = 0xFFFFFFFF;

struct bflb_device_s *sgpio = NULL;
struct bflb_device_s *spi_dev = NULL;
struct bflb_device_s *dma0_ch0 = NULL;
struct bflb_device_s *dma0_ch1 = NULL;

int nx_dma_channel_lli_reload(struct bflb_device_s *dev, struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t max_lli_count, struct nx_dma_channel_lli_transfer_s *transfer, uint32_t count);
static int _hw_init(void (*callback)(void *arg), void *arg)
{
    struct bflb_spi_config_s spi_cfg = {
        .freq = NXSPI_SPEED,
        .role = SPI_ROLE_SLAVE,
        .mode = NXSPI_MODE,
        .data_width = SPI_DATA_WIDTH_32BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 3,
        .rx_fifo_threshold = 3,
    };

    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_SPI0_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    struct bflb_dma_channel_config_s rx_config = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_SPI0_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    /* spi config */
    bflb_spi_init(spi_dev, &spi_cfg);
    if (NXSPI_3PINMODE) {
        uint32_t regval = getreg32(spi_dev->reg_base + SPI_CONFIG_OFFSET);
        putreg32(regval | SPI_CR_SPI_S_3PIN_MODE, spi_dev->reg_base + SPI_CONFIG_OFFSET);
    }
    //NX_LOGD("bflb_spi_link_txdma\r\n");
    bflb_spi_link_txdma(spi_dev, true);
    bflb_spi_link_rxdma(spi_dev, true);

    /* dma config */
    bflb_dma_channel_init(dma0_ch0, &tx_config);
    bflb_dma_channel_init(dma0_ch1, &rx_config);

    //bflb_dma_channel_irq_attach(dma0_ch0, NULL, arg);
    bflb_dma_channel_irq_attach(dma0_ch1, callback, NULL);
}

/* spi */
void nxspi_hwspi_init(void (*callback)(void *arg), void *arg)
{
    if (NULL == spi_dev) {
        spi_dev = bflb_device_get_by_name(NXSPI_NAME);
    }
    if (NULL == dma0_ch0) {
        dma0_ch0 = bflb_device_get_by_name(NXSPI_DMARXCH);
    }
    if (NULL == dma0_ch1) {
        dma0_ch1 = bflb_device_get_by_name(NXSPI_DMATXCH);
    }

    /* clock */
    PERIPHERAL_CLOCK_SPI0_ENABLE();
    PERIPHERAL_CLOCK_DMA0_ENABLE();
    GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);
    GLB_Swap_MCU_SPI_0_MOSI_With_MISO(0);

    /* reset */
    bflb_dma_channel_stop(spi_dev);
    bflb_dma_channel_stop(spi_dev);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_DMA);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SPI);

    /* spi dma config */
    NX_LOGD("_hw_init\r\n");
    _hw_init(callback, arg);
}

void nxspi_hwspi_ts(uint8_t *send_hd, uint8_t *recv_hd, uint16_t hd_len,
                    uint8_t *send_bd, uint8_t *recv_bd, uint16_t bd_len,
                    void *cb, void *arg)
{
#if 1
    uint32_t regval;

    struct nx_dma_channel_lli_transfer_s tx_transfers[2];
    struct nx_dma_channel_lli_transfer_s rx_transfers[2];

    NX_LOGD("send:%p, recv:%p, len:%d, %02X:%02X:%02X:%02X\r\n",
            send_bd, recv_bd, bd_len,
            send_bd[0], send_bd[1], send_bd[2], send_bd[3]);

    //bflb_dma_channel_stop(dma0_ch0);
    //bflb_dma_channel_stop(dma0_ch1);
    /* header config */
    tx_transfers[0].src_addr = (uint32_t)send_hd;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    tx_transfers[0].nbytes = hd_len;
    tx_transfers[0].si = 1;
    tx_transfers[0].di = 0;
    tx_transfers[0].i = 0;

    rx_transfers[0].src_addr = (uint32_t)DMA_ADDR_SPI0_RDR;
    rx_transfers[0].dst_addr = (uint32_t)recv_hd;
    rx_transfers[0].nbytes = hd_len;
    rx_transfers[0].si = 0;
    rx_transfers[0].di = 1;
    rx_transfers[0].i = 1;

    /* data config */
    if (0 == send_bd) {
        tx_transfers[1].src_addr = (uint32_t)(&g_black_hole);
        tx_transfers[1].si = 0;
        // LOGA
        NX_LOGD("R send:%p, recv:%p, len:%d(%d)\r\n",
            send_bd, recv_bd, bd_len, up_header.len);
    } else {
        tx_transfers[1].src_addr = (uint32_t)send_bd;
        tx_transfers[1].si = 1;
        if ((0 == up_header.len) || (NXBD_MTU < up_header.len)) {
            // LOG Error
            NX_LOGD("never here send:%p, recv:%p, len:%d(%d)\r\n",
                send_bd, recv_bd, bd_len, up_header.len);
        } else {
            // LOGA
            NX_LOGD("W send:%p, recv:%p, len:%d, %02X:%02X:%02X:%02X...%02X\r\n",
                send_bd, recv_bd, bd_len,
                send_bd[0], send_bd[1], send_bd[2], send_bd[3],
                send_bd[up_header.len - 1]);
        }
    }
    tx_transfers[1].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    tx_transfers[1].nbytes = bd_len;
    tx_transfers[1].di = 0;
    tx_transfers[1].i = 0;

    if (0 == recv_bd) {
        rx_transfers[1].dst_addr = (uint32_t)(&g_black_hole);
        rx_transfers[1].di = 0;
    } else {
        rx_transfers[1].dst_addr = (uint32_t)recv_bd;
        rx_transfers[1].di = 1;
    }
    rx_transfers[1].src_addr = (uint32_t)DMA_ADDR_SPI0_RDR;
    rx_transfers[1].nbytes = bd_len;
    rx_transfers[1].si = 0;
    rx_transfers[1].i = 0;
#if 0
    uint32_t *reg = (uint32_t *)txlli_pool;
    for (int i = 0; i < 8; i++) {
        printf("0x%08lx ", reg[i]);
    }
    printf("\r\n");
    reg = (uint32_t *)rxlli_pool;
    for (int i = 0; i < 8; i++) {
        printf("0x%08lx ", reg[i]);
    }
    printf("\r\n");
#endif

    nx_dma_channel_lli_reload(dma0_ch0, txlli_pool, DMATX_LLIPOOL_CNT, tx_transfers, 2);
    nx_dma_channel_lli_reload(dma0_ch1, rxlli_pool, DMARX_LLIPOOL_CNT, rx_transfers, 2);
#if 0
    uint32_t *reg = (uint32_t *)txlli_pool;
    for (int i = 0; i < 8; i++) {
        printf("0x%08lx ", reg[i]);
    }
    printf("\r\n");
    reg = (uint32_t *)rxlli_pool;
    for (int i = 0; i < 8; i++) {
        printf("0x%08lx ", reg[i]);
    }
    printf("\r\n");
#endif

#else
    NX_LOGD("send:%p, recv:%p, len:%d, data:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
            send_bd, recv_bd, bd_len,
            send_bd[0], send_bd[1], send_bd[2], send_bd[3],
            send_bd[4], send_bd[5], send_bd[6], send_bd[7]);

    // tx dma->spi
    txlli_pool[0].control.bits.I = 0;
    txlli_pool[0].control.bits.TransferSize = hd_len>>2;
    txlli_pool[0].control.bits.DI = 0;
    txlli_pool[0].control.bits.SI = 1;
    txlli_pool[0].src_addr = send_hd;
    txlli_pool[0].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    txlli_pool[0].nextlli = (uint32_t)(uintptr_t)&txlli_pool[1];
    txlli_pool[1].control.bits.I = 0;
    txlli_pool[1].control.bits.TransferSize = bd_len>>2;
    txlli_pool[1].control.bits.DI = 0;
    if (NULL == send_bd) {
        txlli_pool[1].control.bits.SI = 0;
        txlli_pool[1].src_addr = &black_hole;
    } else {
        txlli_pool[1].control.bits.SI = 1;
        txlli_pool[1].src_addr = send_bd;
    }
    txlli_pool[1].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    txlli_pool[1].nextlli = 0;

    putreg32(txlli_pool[0].src_addr, dma0_ch0->reg_base + DMA_CxSRCADDR_OFFSET);
    putreg32(txlli_pool[0].dst_addr, dma0_ch0->reg_base + DMA_CxDSTADDR_OFFSET);
    putreg32(txlli_pool[0].nextlli, dma0_ch0->reg_base + DMA_CxLLI_OFFSET);
    putreg32(txlli_pool[0].control.WORD, dma0_ch0->reg_base + DMA_CxCONTROL_OFFSET);

    // rx spi->dma
    rxlli_pool[0].control.bits.I = 1;
    rxlli_pool[0].control.bits.TransferSize = hd_len>>2;
    rxlli_pool[0].control.bits.DI = 1;
    rxlli_pool[0].control.bits.SI = 0;
    rxlli_pool[0].src_addr = (uint32_t)DMA_ADDR_SPI0_RDR;
    rxlli_pool[0].dst_addr = recv_hd;
    rxlli_pool[0].nextlli = (uint32_t)(uintptr_t)&rxlli_pool[1];
    rxlli_pool[1].control.bits.I = 0;
    rxlli_pool[1].control.bits.TransferSize = bd_len>>2;
    rxlli_pool[1].control.bits.DI = 1;
    rxlli_pool[1].control.bits.SI = 0;
    rxlli_pool[1].src_addr = (uint32_t)DMA_ADDR_SPI0_RDR;
    rxlli_pool[1].dst_addr = recv_bd;
    rxlli_pool[1].nextlli = 0;

    putreg32(rxlli_pool[0].src_addr, dma0_ch1->reg_base + DMA_CxSRCADDR_OFFSET);
    putreg32(rxlli_pool[0].dst_addr, dma0_ch1->reg_base + DMA_CxDSTADDR_OFFSET);
    putreg32(rxlli_pool[0].nextlli, dma0_ch1->reg_base + DMA_CxLLI_OFFSET);
    putreg32(rxlli_pool[0].control.WORD, dma0_ch1->reg_base + DMA_CxCONTROL_OFFSET);
#if 0
    uint32_t *reg = (uint32_t *)txlli_pool;
    for (int i = 0; i < 8; i++) {
        printf("0x%08lx ", reg[i]);
    }
    printf("\r\n");
    reg = (uint32_t *)rxlli_pool;
    for (int i = 0; i < 8; i++) {
        printf("0x%08lx ", reg[i]);
    }
    printf("\r\n");
#endif
#endif
    bflb_dma_channel_start(dma0_ch0);
    bflb_dma_channel_start(dma0_ch1);
}

void nxspi_hwspi_transfer(uint8_t *send, uint8_t *recv, uint16_t len, void *cb, void *arg)
{
    uint32_t regval;
    static uint32_t black_hole __attribute__((section(".wifi_ram"))) = 0xFFFFFFFF;

    struct nx_dma_channel_lli_transfer_s tx_transfers[1];
    struct nx_dma_channel_lli_transfer_s rx_transfers[1];

    NX_LOGD("send:%p, recv:%p, len:%d, data:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
            send, recv, len,
            send[0], send[1], send[2], send[3],
            send[4], send[5], send[6], send[7]);

    //bflb_dma_channel_stop(dma0_ch0);
    //bflb_dma_channel_stop(dma0_ch1);

    tx_transfers[0].src_addr = (uint32_t)send;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    tx_transfers[0].nbytes = len;

    rx_transfers[0].src_addr = (uint32_t)DMA_ADDR_SPI0_RDR;
    rx_transfers[0].dst_addr = (uint32_t)recv;
    rx_transfers[0].nbytes = len;

    if (NULL == send) {
        tx_transfers[0].src_addr = &black_hole;
        //bflb_dma_feature_control(dma0_ch0,  DMA_CMD_SET_SRCADDR_INCREMENT, 0);
        regval = *((volatile uint32_t *)0x2000C10C);//dma0
        regval &= ~(1<<26U);// src 0
        *((volatile uint32_t *)0x2000C10C) = regval;// dma0
    } else {
        //bflb_dma_feature_control(dma0_ch0,  DMA_CMD_SET_SRCADDR_INCREMENT, 1);
        regval = *((volatile uint32_t *)0x2000C10C);//dma0
        regval |= (1<<26U);// src 1
        *((volatile uint32_t *)0x2000C10C) = regval;// dma0
    }
    if (NULL == recv) {
        rx_transfers[0].dst_addr = &black_hole;
        //bflb_dma_feature_control(dma0_ch1,  DMA_CMD_SET_DSTADDR_INCREMENT, 0);
        regval = *((volatile uint32_t *)0x2000C20C);//dma1
        regval &= ~(1<<27U);// dest 0
        *((volatile uint32_t *)0x2000C20C) = regval;// dma1
    } else{
        //bflb_dma_feature_control(dma0_ch1,  DMA_CMD_SET_DSTADDR_INCREMENT, 1);
        regval = *((volatile uint32_t *)0x2000C20C);//dma1
        regval |= (1<<27U);// dest 1
        *((volatile uint32_t *)0x2000C20C) = regval;// dma1
    }

    bflb_dma_channel_lli_reload(dma0_ch0, txlli_pool, DMATX_LLIPOOL_CNT, tx_transfers, 1);
    bflb_dma_channel_lli_reload(dma0_ch1, rxlli_pool, DMARX_LLIPOOL_CNT, rx_transfers, 1);

#if 0
    bflb_dma_channel_start(dma0_ch0);
    bflb_dma_channel_start(dma0_ch1);
#else
    regval = *((volatile uint32_t *)0x2000C110);//dma0
    regval |= (1U);// DMA_E
    *((volatile uint32_t *)0x2000C110) = regval;// dma0
    regval = *((volatile uint32_t *)0x2000C210);//dma1
    regval |= (1U);// dest 1
    *((volatile uint32_t *)0x2000C210) = regval;// dma1
#endif
}

void nxspi_hwspi_start(void)
{
}

#define GPIO_WR_ENABLE  (0)
/* gpio */
void nxspi_hwgpio_init(int pin, void (*callback)(uint8_t pin), uint8_t trig_mode)
{
    static uint8_t attached = 0;

// todo: set reg
    if (NULL == sgpio) {
        sgpio = bflb_device_get_by_name("gpio");
    }
    if (0 == g_nxspi.gpio_inited) {
        /* spi clk */
        bflb_gpio_init(sgpio, NXSPI_GPIO_CLK, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);
        /* spi miso */
        bflb_gpio_init(sgpio, NXSPI_GPIO_MISO, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);
        /* spi mosi */
        bflb_gpio_init(sgpio, NXSPI_GPIO_MOSI, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);

    #if GPIO_WR_ENABLE
        /* nx */
        (*(volatile uint32_t *)(0x200008C4  + NXSPI_GPIO_IRQ*4)) = 0x2400B63;// 2000 08c4  + 28*4
    #else
        /* irq */
        bflb_gpio_init(sgpio, NXSPI_GPIO_IRQ, GPIO_OUTPUT | GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_3);
    #endif

        bflb_gpio_init(sgpio, pin, GPIO_INPUT | GPIO_PULLDOWN | GPIO_SMT_EN);

        PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE();

        g_nxspi.gpio_inited = 1;
    }

    /* cs */
    bflb_gpio_int_mask(sgpio, pin, true);
    bflb_irq_disable(sgpio->irq_num);

    bflb_gpio_int_init(sgpio, pin, trig_mode);//GPIO_INT_TRIG_MODE_SYNC_FALLING_RISING_EDGE);
    if (!attached) {
        bflb_gpio_irq_attach(pin, callback);
        attached = 1;
    }
    bflb_gpio_int_mask(sgpio, pin, false);
    bflb_irq_clear_pending(sgpio->irq_num);
    bflb_irq_enable(sgpio->irq_num);
}

void nxspi_hwgpio_disableirq(void)
{
    bflb_irq_disable(sgpio->irq_num);
    bflb_dma_channel_irq_detach(dma0_ch1);
}
void nxspi_hwgpio_high(int pin)
{
#if GPIO_WR_ENABLE
    //printf("--------------- set start %d high, 0x%08lX\r\n", pin, (*(volatile uint32_t *)0x20000934));
    // todo: set reg
    // todo: set reg
    if (pin == NXSPI_GPIO_TXN) {
        (*(volatile uint32_t *)(0x200008C4  + NXSPI_GPIO_TXN*4)) = 0x3400B63;
    } else if (pin == NXSPI_GPIO_DR) {
        (*(volatile uint32_t *)(0x200008C4  + NXSPI_GPIO_DR*4)) = 0x3400B63;
    } else {
        bflb_gpio_set(sgpio, pin);
    }
#else
    bflb_gpio_set(sgpio, pin);
#endif
    //printf("--------------- set stop  %d high, 0x%08lX\r\n", pin, (*(volatile uint32_t *)0x20000934));
}
void nxspi_hwgpio_low(int pin)
{
#if GPIO_WR_ENABLE
    //printf("--------------- set start %d low , 0x%08lX\r\n", pin, (*(volatile uint32_t *)0x20000934));
    // todo: set reg
    if (pin == NXSPI_GPIO_TXN) {
        (*(volatile uint32_t *)(0x200008C4  + NXSPI_GPIO_TXN*4)) = 0x2400B63;
    } else if (pin == NXSPI_GPIO_DR) {
        (*(volatile uint32_t *)(0x200008C4  + NXSPI_GPIO_DR*4)) = 0x2400B63;
    } else {
        bflb_gpio_reset(sgpio, pin);
    }
#else
    bflb_gpio_reset(sgpio, pin);
#endif
    //printf("--------------- set stop  %d low , 0x%08X\r\n", pin, (*(volatile uint32_t *)0x20000934));
}
int nxspi_hwgpio_status(int pin)
{
    // todo: set reg
    return bflb_gpio_read(sgpio, pin);
}

void nx_dma_lli_config(struct bflb_device_s *dev,
                         struct bflb_dma_channel_lli_pool_s *lli_pool,
                         uint32_t lli_count,
                         struct nx_dma_channel_lli_transfer_s *transfer,
                         uint32_t transfer_offset,
                         uint32_t last_transfer_len)
{
    uint32_t channel_base;
    union bflb_dma_lli_control_s dma_ctrl_cfg;
    uint32_t src_addr = transfer->src_addr;
    uint32_t dst_addr = transfer->dst_addr;

    channel_base = dev->reg_base;

    dma_ctrl_cfg = (union bflb_dma_lli_control_s)getreg32(channel_base + DMA_CxCONTROL_OFFSET);

    dma_ctrl_cfg.bits.TransferSize = 4064;
    dma_ctrl_cfg.bits.I = 0;

    /* nbytes will be integer multiple of 4064*n or 4064*2*n or 4064*4*n,(n>0) */
    for (uint32_t i = 0; i < lli_count; i++) {
        lli_pool[i].src_addr = src_addr;
        lli_pool[i].dst_addr = dst_addr;
        lli_pool[i].nextlli = 0;

        if (transfer->si) {
            src_addr += transfer_offset;
        }

        if (transfer->di) {
            dst_addr += transfer_offset;
        }

        if (i == lli_count - 1) {
            dma_ctrl_cfg.bits.TransferSize = last_transfer_len;
            dma_ctrl_cfg.bits.I = transfer->i;
        }

        if (i) {
            lli_pool[i - 1].nextlli = (uint32_t)(uintptr_t)&lli_pool[i];
        }

        lli_pool[i].control = dma_ctrl_cfg;
        lli_pool[i].control.bits.DI = transfer->di;
        lli_pool[i].control.bits.SI = transfer->si;
    }
}
int nx_dma_channel_lli_reload(struct bflb_device_s *dev, struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t max_lli_count, struct nx_dma_channel_lli_transfer_s *transfer, uint32_t count)
{
    uint32_t channel_base;
    uint32_t actual_transfer_offset = 0;
    uint32_t actual_transfer_len = 0;
    uint32_t last_transfer_len = 0;
    uint32_t current_lli_count = 0;
    uint32_t lli_count_used_offset = 0;
    union bflb_dma_lli_control_s dma_ctrl_cfg;

    channel_base = dev->reg_base;

    dma_ctrl_cfg = (union bflb_dma_lli_control_s)getreg32(channel_base + DMA_CxCONTROL_OFFSET);

    switch (dma_ctrl_cfg.bits.SWidth) {
        case DMA_DATA_WIDTH_8BIT:
            actual_transfer_offset = 4064;
            break;
        case DMA_DATA_WIDTH_16BIT:
            actual_transfer_offset = 4064 << 1;
            break;
        case DMA_DATA_WIDTH_32BIT:
            actual_transfer_offset = 4064 << 2;
            break;
        default:
            break;
    }

    for (size_t i = 0; i < count; i++) {
        switch (dma_ctrl_cfg.bits.SWidth) {
            case DMA_DATA_WIDTH_8BIT:
                actual_transfer_len = transfer[i].nbytes;
                break;
            case DMA_DATA_WIDTH_16BIT:
                if (transfer[i].nbytes % 2) {
                    return -1;
                }
                actual_transfer_len = transfer[i].nbytes >> 1;
                break;
            case DMA_DATA_WIDTH_32BIT:
                if (transfer[i].nbytes % 4) {
                    return -1;
                }
                actual_transfer_len = transfer[i].nbytes >> 2;
                break;

            default:
                break;
        }

        current_lli_count = actual_transfer_len / 4064 + 1;
        last_transfer_len = actual_transfer_len % 4064;

        /* The maximum transfer capacity of the last node is 4095 */
        if (current_lli_count > 1 && last_transfer_len < (4095 - 4064)) {
            current_lli_count--;
            last_transfer_len += 4064;
        }

        nx_dma_lli_config(dev, &lli_pool[lli_count_used_offset], current_lli_count, &transfer[i], actual_transfer_offset, last_transfer_len);
        if (i) {
            lli_pool[lli_count_used_offset - 1].nextlli = (uint32_t)(uintptr_t)&lli_pool[lli_count_used_offset];
        }
        lli_count_used_offset += current_lli_count;

        if (lli_count_used_offset > max_lli_count) {
            return -ENOMEM;
        }
    }

    putreg32(lli_pool[0].src_addr, channel_base + DMA_CxSRCADDR_OFFSET);
    putreg32(lli_pool[0].dst_addr, channel_base + DMA_CxDSTADDR_OFFSET);
    putreg32(lli_pool[0].nextlli, channel_base + DMA_CxLLI_OFFSET);
    putreg32(lli_pool[0].control.WORD, channel_base + DMA_CxCONTROL_OFFSET);

    return lli_count_used_offset;
}

uint8_t nx_gpio_trigmode_get(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t cfg_address;
    uint32_t regval;

    reg_base = dev->reg_base;
    cfg_address = reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2);
    regval = getreg32(cfg_address);
    return (uint8_t)((regval & GLB_REG_GPIO_0_INT_MODE_SET_MASK)>>GLB_REG_GPIO_0_INT_MODE_SET_SHIFT);
}

#if GPIO_TIME_ENABLE
/* -------------------------------- statistical -------------------------------- */
// Function to initialize the statistics structure
void nx_init_stats(nx_stats_t *stats)
{
    stats->average = 0.0;
    stats->count = 0;
    stats->sum = 0;
    for (int i = 0; i < NX_MAX_TOP; i++) {
        stats->top[i] = 0;
        stats->bottom[i] = UINT64_MAX;
    }
}

// Function to update the top N values
void nx_update_top(uint64_t *top, uint64_t value) {
    for (int i = 0; i < NX_MAX_TOP; i++) {
        if (value > top[i]) {
            for (int j = NX_MAX_TOP - 1; j > i; j--) {
                top[j] = top[j - 1];
            }
            top[i] = value;
            break;
        }
    }
}

// Function to update the bottom N values
void nx_update_bottom(uint64_t *bottom, uint64_t value) {
    for (int i = 0; i < NX_MAX_BOTTOM; i++) {
        if (value < bottom[i]) {
            for (int j = NX_MAX_BOTTOM - 1; j > i; j--) {
                bottom[j] = bottom[j - 1];
            }
            bottom[i] = value;
            break;
        }
    }
}

// Function to process a new value
void nx_process_value(nx_stats_t *stats, uint64_t value)
{
    stats->count++;
    stats->sum += value;
    stats->average = (double)stats->sum / stats->count;

    nx_update_top(stats->top, value);
    nx_update_bottom(stats->bottom, value);
}
#endif

void nxspi_delay_setgpio_start(uint32_t delay_us)
{
    struct bflb_timer_config_s cfg0;

    cfg0.counter_mode = TIMER_COUNTER_MODE_UP; /* preload when match occur */
    cfg0.clock_source = TIMER_CLKSRC_XTAL;
    cfg0.clock_div = 39; /* for bl616/bflb_undef/bflb_undefp is 39, for bflb_undef is 31 */
    cfg0.trigger_comp_id = TIMER_COMP_ID_0;
    cfg0.comp0_val = delay_us; /* match value 0 */
    cfg0.comp1_val = 90000000; /* match value 1 */ // 90 S
    cfg0.comp2_val = 100000000; /* match value 2 */ // 99 S
    cfg0.preload_val = 0;    /* preload value */

    /* Timer init with default configuration */
    bflb_timer_init(g_nxspi.timer0, &cfg0);
    bflb_irq_enable(g_nxspi.timer0->irq_num);
    bflb_timer_start(g_nxspi.timer0);
}

