#include "bflb_uart.h"
#include "bflb_clock.h"
#include "hardware/uart_reg.h"

#define UART_TX_TIMEOUT_COUNT (160 * 1000)

void bflb_uart_init(struct bflb_device_s *dev, const struct bflb_uart_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_UART_DATABITS(config->data_bits));
    LHAL_PARAM_ASSERT(IS_UART_STOPBITS(config->stop_bits));
    LHAL_PARAM_ASSERT(IS_UART_PARITY(config->parity));
    LHAL_PARAM_ASSERT(IS_UART_BITORDER(config->bit_order));
    LHAL_PARAM_ASSERT(IS_UART_FIFO_THRESHOLD(config->tx_fifo_threshold));
    LHAL_PARAM_ASSERT(IS_UART_FIFO_THRESHOLD(config->rx_fifo_threshold));

#ifdef romapi_bflb_uart_init
    romapi_bflb_uart_init(dev, config);
#else
    uint32_t div = 0;
    uint32_t tx_cfg;
    uint32_t rx_cfg;
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    /* Cal the baud rate divisor */
    div = (bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, dev->idx) * 10 / config->baudrate + 5) / 10;

    LHAL_PARAM_ASSERT(div < 0xFFFF);

    tx_cfg = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
    rx_cfg = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
    tx_cfg &= ~UART_CR_UTX_EN;
    rx_cfg &= ~UART_CR_URX_EN;
    putreg32(tx_cfg, reg_base + UART_UTX_CONFIG_OFFSET);
    putreg32(rx_cfg, reg_base + UART_URX_CONFIG_OFFSET);

    putreg32(((div - 1) << 0x10) | ((div - 1) & 0xFFFF), reg_base + UART_BIT_PRD_OFFSET);

    /* configure parity type */

    tx_cfg = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
    rx_cfg = getreg32(reg_base + UART_URX_CONFIG_OFFSET);

    switch (config->parity) {
        case UART_PARITY_NONE:
            tx_cfg &= ~UART_CR_UTX_PRT_EN;
            rx_cfg &= ~UART_CR_URX_PRT_EN;
            break;
        case UART_PARITY_ODD:
            tx_cfg |= UART_CR_UTX_PRT_EN;
            tx_cfg |= UART_CR_UTX_PRT_SEL;
            rx_cfg |= UART_CR_URX_PRT_EN;
            rx_cfg |= UART_CR_URX_PRT_SEL;
            break;
        case UART_PARITY_EVEN:
            tx_cfg |= UART_CR_UTX_PRT_EN;
            tx_cfg &= ~UART_CR_UTX_PRT_SEL;
            rx_cfg |= UART_CR_URX_PRT_EN;
            rx_cfg &= ~UART_CR_URX_PRT_SEL;
            break;
        default:
            break;
    }

    /* Configure data bits */
    tx_cfg &= ~UART_CR_UTX_BIT_CNT_D_MASK;
    tx_cfg |= (config->data_bits + 4) << UART_CR_UTX_BIT_CNT_D_SHIFT;
    rx_cfg &= ~UART_CR_URX_BIT_CNT_D_MASK;
    rx_cfg |= (config->data_bits + 4) << UART_CR_URX_BIT_CNT_D_SHIFT;

    /* Configure tx stop bits */
    tx_cfg &= ~UART_CR_UTX_BIT_CNT_P_MASK;
    tx_cfg |= config->stop_bits << UART_CR_UTX_BIT_CNT_P_SHIFT;

    /* Configure tx cts flow control function */
    if (config->flow_ctrl & UART_FLOWCTRL_CTS) {
        tx_cfg |= UART_CR_UTX_CTS_EN;
    } else {
        tx_cfg &= ~UART_CR_UTX_CTS_EN;
    }

    rx_cfg &= ~UART_CR_URX_DEG_EN;

    /* Write back */
    putreg32(tx_cfg, reg_base + UART_UTX_CONFIG_OFFSET);
    putreg32(rx_cfg, reg_base + UART_URX_CONFIG_OFFSET);
#if defined(BL602)
    regval = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
    regval &= ~UART_CR_URX_RTS_SW_MODE;
    putreg32(regval, reg_base + UART_URX_CONFIG_OFFSET);

#else
    regval = getreg32(reg_base + UART_SW_MODE_OFFSET);
    regval &= ~UART_CR_URX_RTS_SW_MODE;
    putreg32(regval, reg_base + UART_SW_MODE_OFFSET);
#endif
    regval = getreg32(reg_base + UART_DATA_CONFIG_OFFSET);
    regval &= ~UART_CR_UART_BIT_INV;
    putreg32(regval, reg_base + UART_DATA_CONFIG_OFFSET);

    /* Enable tx free run mode */
    regval = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
    regval |= UART_CR_UTX_FRM_EN;
    putreg32(regval, reg_base + UART_UTX_CONFIG_OFFSET);

    /* Configure FIFO thresholds */
    regval = getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET);
    regval &= ~UART_TX_FIFO_TH_MASK;
    regval &= ~UART_RX_FIFO_TH_MASK;
    regval |= (config->tx_fifo_threshold << UART_TX_FIFO_TH_SHIFT) & UART_TX_FIFO_TH_MASK;
    regval |= (config->rx_fifo_threshold << UART_RX_FIFO_TH_SHIFT) & UART_RX_FIFO_TH_MASK;
    putreg32(regval, reg_base + UART_FIFO_CONFIG_1_OFFSET);

    /* Clear FIFO */
    regval = getreg32(reg_base + UART_FIFO_CONFIG_0_OFFSET);
    regval |= UART_TX_FIFO_CLR;
    regval |= UART_RX_FIFO_CLR;
    regval &= ~UART_DMA_TX_EN;
    regval &= ~UART_DMA_RX_EN;
    putreg32(regval, reg_base + UART_FIFO_CONFIG_0_OFFSET);

    putreg32(0xFFFFFFFF, reg_base + UART_INT_MASK_OFFSET);

    /* Enable UART tx rx unit */
    tx_cfg = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
    rx_cfg = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
    tx_cfg |= UART_CR_UTX_EN;
    rx_cfg |= UART_CR_URX_EN;
    putreg32(tx_cfg, reg_base + UART_UTX_CONFIG_OFFSET);
    putreg32(rx_cfg, reg_base + UART_URX_CONFIG_OFFSET);
#endif
}

void bflb_uart_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_deinit
    romapi_bflb_uart_deinit(dev);
#else
    uint32_t reg_base;
    uint32_t tx_cfg;
    uint32_t rx_cfg;

    reg_base = dev->reg_base;
    tx_cfg = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
    rx_cfg = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
    tx_cfg &= ~UART_CR_UTX_EN;
    rx_cfg &= ~UART_CR_URX_EN;
    putreg32(tx_cfg, reg_base + UART_UTX_CONFIG_OFFSET);
    putreg32(rx_cfg, reg_base + UART_URX_CONFIG_OFFSET);
#endif
}

void bflb_uart_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_enable
    romapi_bflb_uart_enable(dev);
#else
    uint32_t reg_base;
    uint32_t tx_cfg;
    uint32_t rx_cfg;

    reg_base = dev->reg_base;
    tx_cfg = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
    rx_cfg = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
    tx_cfg |= UART_CR_UTX_EN;
    rx_cfg |= UART_CR_URX_EN;
    putreg32(tx_cfg, reg_base + UART_UTX_CONFIG_OFFSET);
    putreg32(rx_cfg, reg_base + UART_URX_CONFIG_OFFSET);
#endif
}

void bflb_uart_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_disable
    romapi_bflb_uart_disable(dev);
#else
    uint32_t reg_base;
    uint32_t tx_cfg;
    uint32_t rx_cfg;

    reg_base = dev->reg_base;
    tx_cfg = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
    rx_cfg = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
    tx_cfg &= ~UART_CR_UTX_EN;
    rx_cfg &= ~UART_CR_URX_EN;
    putreg32(tx_cfg, reg_base + UART_UTX_CONFIG_OFFSET);
    putreg32(rx_cfg, reg_base + UART_URX_CONFIG_OFFSET);
#endif
}

void bflb_uart_link_txdma(struct bflb_device_s *dev, bool enable)
{
#ifdef romapi_bflb_uart_link_txdma
    romapi_bflb_uart_link_txdma(dev, enable);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + UART_FIFO_CONFIG_0_OFFSET);
    if (enable) {
        regval |= UART_DMA_TX_EN;
    } else {
        regval &= ~UART_DMA_TX_EN;
    }
    putreg32(regval, reg_base + UART_FIFO_CONFIG_0_OFFSET);
#endif
}

void bflb_uart_link_rxdma(struct bflb_device_s *dev, bool enable)
{
#ifdef romapi_bflb_uart_link_rxdma
    romapi_bflb_uart_link_rxdma(dev, enable);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + UART_FIFO_CONFIG_0_OFFSET);
    if (enable) {
        regval |= UART_DMA_RX_EN;
    } else {
        regval &= ~UART_DMA_RX_EN;
    }
    putreg32(regval, reg_base + UART_FIFO_CONFIG_0_OFFSET);
#endif
}

ATTR_TCM_SECTION int bflb_uart_putchar(struct bflb_device_s *dev, int ch)
{
#ifdef romapi_bflb_uart_putchar
    return romapi_bflb_uart_putchar(dev, ch);
#else
    uint64_t start_time;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    start_time = bflb_mtimer_get_time_ms();
    while ((getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET) & UART_TX_FIFO_CNT_MASK) == 0) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }
    putreg8(ch, reg_base + UART_FIFO_WDATA_OFFSET);
    return 0;
#endif
}

ATTR_TCM_SECTION int bflb_uart_getchar(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_getchar
    return romapi_bflb_uart_getchar(dev);
#else
    int ch = -1;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    if ((getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET) & UART_RX_FIFO_CNT_MASK) != 0) {
        ch = getreg8(reg_base + UART_FIFO_RDATA_OFFSET);
    }

    return ch;
#endif
}

ATTR_TCM_SECTION int bflb_uart_put(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
#ifdef romapi_bflb_uart_put
    return romapi_bflb_uart_put(dev, config);
#else
    int ret;
    for (uint32_t i = 0; i < len; i++) {
        ret = bflb_uart_putchar(dev, data[i]);
        if (ret < 0) {
            return ret;
        }
    }
    return 0;
#endif
}

ATTR_TCM_SECTION int bflb_uart_put_block(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
    int ret;
    uint32_t timeoutCnt = UART_TX_TIMEOUT_COUNT;
    for (uint32_t i = 0; i < len; i++) {
        ret = bflb_uart_putchar(dev, data[i]);
        if (ret < 0) {
            return ret;
        }
    }
    while (getreg32(dev->reg_base + UART_STATUS_OFFSET) & UART_STS_UTX_BUS_BUSY) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return -ETIMEDOUT;
        }
    }
    return 0;
}

ATTR_TCM_SECTION int bflb_uart_get(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
#ifdef romapi_bflb_uart_get
    return romapi_bflb_uart_get(dev, data, len);
#else
    int ch = -1;
    uint32_t count = 0;

    while (count < len) {
        if ((ch = bflb_uart_getchar(dev)) < 0) {
            break;
        }
        data[count] = ch;
        count++;
    }
    return count;
#endif
}

int bflb_uart_wait_tx_done(struct bflb_device_s *dev)
{
    uint64_t start_time;

    start_time = bflb_mtimer_get_time_ms();
    while (!bflb_uart_txempty(dev)) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 1000) {
            return -ETIMEDOUT;
        }
    }

    start_time = bflb_mtimer_get_time_ms();
    while (getreg32(dev->reg_base + UART_STATUS_OFFSET) & UART_STS_UTX_BUS_BUSY) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 1000) {
            return -ETIMEDOUT;
        }
    }
    return 0;
}

bool bflb_uart_txready(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_txready
    return romapi_bflb_uart_txready(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;
    if ((getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET) & UART_TX_FIFO_CNT_MASK) != 0) {
        return true;
    } else {
        return false;
    }
#endif
}

bool bflb_uart_txempty(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_txempty
    return romapi_bflb_uart_txempty(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;
    if ((getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET) & UART_TX_FIFO_CNT_MASK) == (UART_TX_FIFO_CNT_MASK >> 1) + 1) {
        return true;
    } else {
        return false;
    }
#endif
}

bool bflb_uart_rxavailable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_rxavailable
    return romapi_bflb_uart_rxavailable(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;
    return ((getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET) & UART_RX_FIFO_CNT_MASK) != 0);
#endif
}

void bflb_uart_txint_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_uart_txint_mask
    romapi_bflb_uart_txint_mask(dev, mask);
#else
    uint32_t reg_base;
    uint32_t int_mask;

    reg_base = dev->reg_base;
    int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
    if (mask) {
        int_mask |= UART_CR_UTX_FIFO_MASK;
    } else {
        int_mask &= ~UART_CR_UTX_FIFO_MASK;
    }
    putreg32(int_mask, reg_base + UART_INT_MASK_OFFSET);
#endif
}

void bflb_uart_rxint_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_uart_rxint_mask
    romapi_bflb_uart_rxint_mask(dev, mask);
#else
    uint32_t reg_base;
    uint32_t int_mask;

    reg_base = dev->reg_base;
    int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
    if (mask) {
        int_mask |= UART_CR_URX_FIFO_MASK;
        int_mask |= UART_CR_URX_RTO_MASK;
    } else {
        int_mask &= ~UART_CR_URX_FIFO_MASK;
        int_mask &= ~UART_CR_URX_RTO_MASK;
    }
    putreg32(int_mask, reg_base + UART_INT_MASK_OFFSET);
#endif
}

void bflb_uart_errint_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_uart_errint_mask
    romapi_bflb_uart_errint_mask(dev, mask);
#else
    uint32_t reg_base;
    uint32_t int_mask;

    reg_base = dev->reg_base;
    int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
    if (mask) {
        int_mask |= UART_CR_URX_PCE_MASK;
        int_mask |= UART_CR_UTX_FER_MASK;
        int_mask |= UART_CR_URX_FER_MASK;
#if !defined(BL602)
        int_mask |= UART_CR_URX_LSE_MASK;
#endif
    } else {
        int_mask &= ~UART_CR_URX_PCE_MASK;
        int_mask &= ~UART_CR_UTX_FER_MASK;
        int_mask &= ~UART_CR_URX_FER_MASK;
#if !defined(BL602)
        int_mask &= ~UART_CR_URX_LSE_MASK;
#endif
    }
    putreg32(int_mask, reg_base + UART_INT_MASK_OFFSET);
#endif
}

uint32_t bflb_uart_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_uart_get_intstatus
    return romapi_bflb_uart_get_intstatus(dev);
#else
    uint32_t reg_base;
    uint32_t int_status;
    uint32_t int_mask;

    reg_base = dev->reg_base;
    int_status = getreg32(reg_base + UART_INT_STS_OFFSET);
    int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
    return (int_status & ~int_mask);
#endif
}

void bflb_uart_int_clear(struct bflb_device_s *dev, uint32_t int_clear)
{
#ifdef romapi_bflb_uart_int_clear
    romapi_bflb_uart_int_clear(dev, int_clear);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(int_clear, reg_base + UART_INT_CLEAR_OFFSET);
#endif
}

int bflb_uart_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_uart_feature_control
    return romapi_bflb_uart_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;
    uint32_t tmp;
    uint32_t tx_tmp;
    uint32_t rx_tmp;
    uint32_t int_mask;

    reg_base = dev->reg_base;

    switch (cmd) {
        case UART_CMD_SET_BAUD_RATE:
            bflb_uart_disable(dev);

            /* Cal the baud rate divisor */
            tmp = (bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, dev->idx) * 10 / arg + 5) / 10;

            putreg32(((tmp - 1) << 0x10) | ((tmp - 1) & 0xFFFF), reg_base + UART_BIT_PRD_OFFSET);
            bflb_uart_enable(dev);
            break;

        case UART_CMD_SET_DATA_BITS:
            bflb_uart_disable(dev);

            /* Set data bits */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
            rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);

            tx_tmp &= ~UART_CR_UTX_BIT_CNT_D_MASK;
            tx_tmp |= (arg + 4) << UART_CR_UTX_BIT_CNT_D_SHIFT;

            rx_tmp &= ~UART_CR_URX_BIT_CNT_D_MASK;
            rx_tmp |= (arg + 4) << UART_CR_URX_BIT_CNT_D_SHIFT;

            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            bflb_uart_enable(dev);
            break;

        case UART_CMD_SET_STOP_BITS:
            bflb_uart_disable(dev);

            /* Set stop bits */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);

            tx_tmp &= ~UART_CR_UTX_BIT_CNT_P_MASK;
            tx_tmp |= arg << UART_CR_UTX_BIT_CNT_P_SHIFT;

            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            bflb_uart_enable(dev);
            break;

        case UART_CMD_SET_PARITY_BITS:
            bflb_uart_disable(dev);

            /* Set parity mode */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
            rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);

            if (arg == UART_PARITY_NONE) {
                tx_tmp &= ~UART_CR_UTX_PRT_EN;
                rx_tmp &= ~UART_CR_URX_PRT_EN;
            } else if (arg == UART_PARITY_ODD) {
                tx_tmp |= UART_CR_UTX_PRT_EN;
                tx_tmp |= UART_CR_UTX_PRT_SEL;
                rx_tmp |= UART_CR_URX_PRT_EN;
                rx_tmp |= UART_CR_URX_PRT_SEL;
            } else if (arg == UART_PARITY_EVEN) {
                tx_tmp |= UART_CR_UTX_PRT_EN;
                tx_tmp &= ~UART_CR_UTX_PRT_SEL;
                rx_tmp |= UART_CR_URX_PRT_EN;
                rx_tmp &= ~UART_CR_URX_PRT_SEL;
            }

            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            bflb_uart_enable(dev);
            break;

        case UART_CMD_CLR_TX_FIFO:
            /* Clear tx fifo */
            tmp = getreg32(reg_base + UART_FIFO_CONFIG_0_OFFSET);
            tmp |= UART_TX_FIFO_CLR;
            putreg32(tmp, reg_base + UART_FIFO_CONFIG_0_OFFSET);
            break;

        case UART_CMD_CLR_RX_FIFO:
            /* Clear rx fifo */
            tmp = getreg32(reg_base + UART_FIFO_CONFIG_0_OFFSET);
            tmp |= UART_RX_FIFO_CLR;
            putreg32(tmp, reg_base + UART_FIFO_CONFIG_0_OFFSET);
            break;

        case UART_CMD_SET_RTO_VALUE:
            /* Set rx time-out value */
            putreg32(arg, reg_base + UART_URX_RTO_TIMER_OFFSET);

            int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
            if (arg == 0) {
                int_mask |= UART_CR_URX_RTO_MASK;
            } else {
                int_mask &= ~UART_CR_URX_RTO_MASK;
            }
            putreg32(int_mask, reg_base + UART_INT_MASK_OFFSET);
            break;

        case UART_CMD_SET_RTS_VALUE:
#if defined(BL602)
#else
            /* Set rx rts output software control value */
            tmp = getreg32(reg_base + UART_SW_MODE_OFFSET);
            tmp &= ~UART_CR_URX_RTS_SW_VAL;
            if (arg) {
                tmp |= UART_CR_URX_RTS_SW_VAL;
            }
            putreg32(tmp, reg_base + UART_SW_MODE_OFFSET);
#endif
            break;

        case UART_CMD_GET_TX_FIFO_CNT:
            /* Get tx fifo count */
            ret = (getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET) & UART_TX_FIFO_CNT_MASK) >> UART_TX_FIFO_CNT_SHIFT;
            break;
        case UART_CMD_GET_RX_FIFO_CNT:
            /* Get rx fifo count */
            ret = (getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET) & UART_RX_FIFO_CNT_MASK) >> UART_RX_FIFO_CNT_SHIFT;
            break;
        case UART_CMD_SET_AUTO_BAUD:
            /* Set auto baudrate detection  */
            tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
            rx_tmp = getreg32(reg_base + UART_INT_MASK_OFFSET);
            tmp &= ~UART_CR_URX_ABR_EN;
            if (arg == UART_AUTO_BAUD_0X55) {
                tmp |= UART_CR_URX_ABR_EN;
#if !defined(BL602) && !defined(BL702)
                rx_tmp &= ~UART_CR_URX_AD5_MASK;
#endif
            } else {
                tmp |= UART_CR_URX_ABR_EN;
#if !defined(BL602) && !defined(BL702)
                rx_tmp &= ~UART_CR_URX_ADS_MASK;
#endif
            }

            putreg32(tmp, reg_base + UART_URX_CONFIG_OFFSET);
            putreg32(rx_tmp, reg_base + UART_INT_MASK_OFFSET);
            break;

        case UART_CMD_GET_AUTO_BAUD:
            /* Get auto baudrate detection count value */
            tmp = getreg32(reg_base + UART_STS_URX_ABR_PRD_OFFSET);
            if (arg == UART_AUTO_BAUD_START) {
                ret = (tmp & UART_STS_URX_ABR_PRD_START_MASK);
            } else {
                ret = ((tmp & UART_STS_URX_ABR_PRD_0X55_MASK) >> UART_STS_URX_ABR_PRD_0X55_SHIFT);
            }
            break;
#if !defined(BL602)
        case UART_CMD_SET_BREAK_VALUE:
            /* Set lin mode break value */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);

            tx_tmp &= ~UART_CR_UTX_BIT_CNT_B_MASK;
            tx_tmp |= arg << UART_CR_UTX_BIT_CNT_B_SHIFT;

            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            break;

        case UART_CMD_SET_TX_LIN_VALUE:
            /* Set tx lin mode */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
            tx_tmp &= ~UART_CR_UTX_LIN_EN;
            if (arg) {
                tx_tmp |= UART_CR_UTX_LIN_EN;
            }

            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            break;

        case UART_CMD_SET_RX_LIN_VALUE:
            /* Set rx lin mode */
            rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
            rx_tmp &= ~UART_CR_URX_LIN_EN;
            if (arg) {
                rx_tmp |= UART_CR_URX_LIN_EN;
            }

            putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            break;
#endif
        case UART_CMD_SET_GLITCH_VALUE:
            rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
            rx_tmp &= ~UART_CR_URX_DEG_CNT_MASK;
            rx_tmp &= ~UART_CR_URX_DEG_EN;
            if (arg) {
                rx_tmp |= (arg << UART_CR_URX_DEG_CNT_SHIFT) & UART_CR_URX_DEG_CNT_MASK;
                rx_tmp |= UART_CR_URX_DEG_EN;
            }
            putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            break;
#if !defined(BL602) && !defined(BL702)
        case UART_CMD_SET_TX_RS485_EN:
            /* Set tx rs485 transceiver enable */
            tx_tmp = getreg32(reg_base + UART_UTX_RS485_CFG_OFFSET);
            tx_tmp &= ~UART_CR_UTX_RS485_EN;

            if (arg) {
                tx_tmp |= UART_CR_UTX_RS485_EN;
            }

            putreg32(tx_tmp, reg_base + UART_UTX_RS485_CFG_OFFSET);
            break;

        case UART_CMD_SET_TX_RS485_POLARITY:
            /* Set tx rs485 de pin polarity */
            tx_tmp = getreg32(reg_base + UART_UTX_RS485_CFG_OFFSET);
            tx_tmp &= ~UART_CR_UTX_RS485_POL;

            if (arg) {
                tx_tmp |= UART_CR_UTX_RS485_POL;
            }

            putreg32(tx_tmp, reg_base + UART_UTX_RS485_CFG_OFFSET);
            break;

        case UART_CMD_SET_ABR_ALLOWABLE_ERROR:
            /* Set auto baudrate detection mode pulse-width tolerance value for codeword 0x55 */
            rx_tmp = getreg32(reg_base + UART_URX_ABR_PW_TOL_OFFSET);
            rx_tmp &= ~UART_CR_URX_ABR_PW_TOL_MASK;
            rx_tmp |= arg << UART_CR_URX_ABR_PW_TOL_SHIFT;

            putreg32(rx_tmp, reg_base + UART_URX_ABR_PW_TOL_OFFSET);
            break;
#endif
        case UART_CMD_SET_SW_RTS_CONTROL:
#if defined(BL602)
            if (arg) {
                rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
                rx_tmp |= UART_CR_URX_RTS_SW_MODE;
                putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            } else {
                rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
                rx_tmp &= ~UART_CR_URX_RTS_SW_MODE;
                putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            }
#else
            if (arg) {
                rx_tmp = getreg32(reg_base + UART_SW_MODE_OFFSET);
                rx_tmp |= UART_CR_URX_RTS_SW_MODE;
                putreg32(rx_tmp, reg_base + UART_SW_MODE_OFFSET);

            } else {
                rx_tmp = getreg32(reg_base + UART_SW_MODE_OFFSET);
                rx_tmp &= ~UART_CR_URX_RTS_SW_MODE;
                putreg32(rx_tmp, reg_base + UART_SW_MODE_OFFSET);
            }
#endif
            break;
#if !defined(BL702L)
        case UART_CMD_IR_CONFIG: {
            struct bflb_uart_ir_config_s *ir_config = (struct bflb_uart_ir_config_s *)arg;
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
            if (ir_config->tx_en) {
                tx_tmp |= UART_CR_UTX_IR_EN;
            } else {
                tx_tmp &= ~UART_CR_UTX_IR_EN;
            }
            if (ir_config->tx_inverse) {
                tx_tmp |= UART_CR_UTX_IR_INV;
            } else {
                tx_tmp &= ~UART_CR_UTX_IR_INV;
            }
            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);

            rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
            if (ir_config->rx_en) {
                rx_tmp |= UART_CR_URX_IR_EN;
            } else {
                rx_tmp &= ~UART_CR_URX_IR_EN;
            }
            if (ir_config->rx_inverse) {
                rx_tmp |= UART_CR_URX_IR_INV;
            } else {
                rx_tmp &= ~UART_CR_URX_IR_INV;
            }
            putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            /* Configure tx ir pulse start and stop position */
            putreg32((ir_config->tx_pluse_stop << 16) | ir_config->tx_pluse_start, reg_base + UART_UTX_IR_POSITION_OFFSET);
            /* Configure rx ir pulse start position */
            putreg32(ir_config->rx_pluse_start, reg_base + UART_URX_IR_POSITION_OFFSET);
        } break;
#endif
        case UART_CMD_SET_TX_FREERUN:
            /* Set tx freerun */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);

            if (arg) {
                tx_tmp |= UART_CR_UTX_FRM_EN;
            } else {
                tx_tmp &= ~UART_CR_UTX_FRM_EN;
            }

            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            break;
        case UART_CMD_SET_TX_END_INTERRUPT:
            /* Set tx end interrupt */
            int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
            if (arg) {
                int_mask &= ~UART_CR_UTX_END_MASK;
            } else {
                int_mask |= UART_CR_UTX_END_MASK;
            }
            putreg32(int_mask, reg_base + UART_INT_MASK_OFFSET);
            break;
        case UART_CMD_SET_RX_END_INTERRUPT:
            /* Set rx end interrupt */
            int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
            if (arg) {
                int_mask &= ~UART_CR_URX_END_MASK;
            } else {
                int_mask |= UART_CR_URX_END_MASK;
            }
            putreg32(int_mask, reg_base + UART_INT_MASK_OFFSET);
            break;
        case UART_CMD_SET_TX_TRANSFER_LEN:
            /* Set tx transfer length */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
            tx_tmp &= ~UART_CR_UTX_LEN_MASK;
            tx_tmp |= ((arg - 1) << UART_CR_UTX_LEN_SHIFT);
            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            break;
        case UART_CMD_SET_RX_TRANSFER_LEN:
            /* Set rx transfer length */
            rx_tmp = getreg32(reg_base + UART_URX_CONFIG_OFFSET);
            rx_tmp &= ~UART_CR_URX_LEN_MASK;
            rx_tmp |= ((arg - 1) << UART_CR_URX_LEN_SHIFT);
            putreg32(rx_tmp, reg_base + UART_URX_CONFIG_OFFSET);
            break;
        case UART_CMD_SET_TX_EN:
            /* Set tx enable */
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);

            if (arg) {
                tx_tmp |= UART_CR_UTX_EN;
            } else {
                tx_tmp &= ~UART_CR_UTX_EN;
            }

            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            break;
#if !defined(BL602) && !defined(BL702)
        case UART_CMD_SET_BCR_END_INTERRUPT:
            /* Set bcr value */
            int_mask = getreg32(reg_base + UART_INT_MASK_OFFSET);
            int_mask &= ~UART_CR_URX_BCR_MASK;
            putreg32(int_mask, reg_base + UART_INT_MASK_OFFSET);

            rx_tmp = getreg32(reg_base + UART_URX_BCR_INT_CFG_OFFSET);
            rx_tmp &= ~UART_CR_URX_BCR_VALUE_MASK;
            rx_tmp |= (arg << UART_CR_URX_BCR_VALUE_SHIFT);
            putreg32(rx_tmp, reg_base + UART_URX_BCR_INT_CFG_OFFSET);
            break;
        case UART_CMD_GET_BCR_COUNT:
            /* Get bcr value */
            rx_tmp = getreg32(reg_base + UART_URX_BCR_INT_CFG_OFFSET);
            ret = ((rx_tmp & UART_STS_URX_BCR_COUNT_MASK) >> UART_STS_URX_BCR_COUNT_SHIFT);
            break;
#endif
        case UART_CMD_SET_CTS_EN:
            tx_tmp = getreg32(reg_base + UART_UTX_CONFIG_OFFSET);
            if (arg) {
                tx_tmp |= UART_CR_UTX_CTS_EN;
            } else {
                tx_tmp &= ~UART_CR_UTX_CTS_EN;
            }
            putreg32(tx_tmp, reg_base + UART_UTX_CONFIG_OFFSET);
            break;
        case UART_CMD_SET_TX_FIFO_THREHOLD:
            tx_tmp = getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET);
            tx_tmp &= ~UART_TX_FIFO_TH_MASK;
            tx_tmp |= (arg << UART_TX_FIFO_TH_SHIFT) & UART_TX_FIFO_TH_MASK;
            putreg32(tx_tmp, reg_base + UART_FIFO_CONFIG_1_OFFSET);
            break;
        case UART_CMD_SET_RX_FIFO_THREHOLD:
            rx_tmp = getreg32(reg_base + UART_FIFO_CONFIG_1_OFFSET);
            rx_tmp &= ~UART_RX_FIFO_TH_MASK;
            rx_tmp |= (arg << UART_RX_FIFO_TH_SHIFT) & UART_RX_FIFO_TH_MASK;
            putreg32(rx_tmp, reg_base + UART_FIFO_CONFIG_1_OFFSET);
            break;
        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
