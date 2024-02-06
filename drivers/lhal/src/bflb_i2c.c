#include "bflb_i2c.h"
#include "bflb_clock.h"
#include "hardware/i2c_reg.h"

#define PUT_UINT32_LE(field, value)            \
    do {                                       \
        (field)[0] = (uint8_t)((value) >> 0);  \
        (field)[1] = (uint8_t)((value) >> 8);  \
        (field)[2] = (uint8_t)((value) >> 16); \
        (field)[3] = (uint8_t)((value) >> 24); \
    } while (0)

static void bflb_i2c_addr_config(struct bflb_device_s *dev, uint16_t slaveaddr, uint32_t subaddr, uint8_t subaddr_size, bool is_addr_10bit)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_CONFIG_OFFSET);

    if (subaddr_size > 0) {
        regval |= I2C_CR_I2C_SUB_ADDR_EN;
        regval &= ~I2C_CR_I2C_SUB_ADDR_BC_MASK;
        regval |= ((subaddr_size - 1) << I2C_CR_I2C_SUB_ADDR_BC_SHIFT);
    } else {
        regval &= ~I2C_CR_I2C_SUB_ADDR_EN;
    }

    regval &= ~I2C_CR_I2C_SLV_ADDR_MASK;
    regval |= (slaveaddr << I2C_CR_I2C_SLV_ADDR_SHIFT);
#if !defined(BL602) && !defined(BL702)
    if (is_addr_10bit) {
        regval |= I2C_CR_I2C_10B_ADDR_EN;
    } else {
        regval &= ~I2C_CR_I2C_10B_ADDR_EN;
    }
#endif
    putreg32(subaddr, reg_base + I2C_SUB_ADDR_OFFSET);
    putreg32(regval, reg_base + I2C_CONFIG_OFFSET);
}

static inline void bflb_i2c_set_dir(struct bflb_device_s *dev, bool is_in)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_CONFIG_OFFSET);

    if (is_in) {
        regval |= I2C_CR_I2C_PKT_DIR;
    } else {
        regval &= ~I2C_CR_I2C_PKT_DIR;
    }
    putreg32(regval, reg_base + I2C_CONFIG_OFFSET);
}

static inline void bflb_i2c_set_datalen(struct bflb_device_s *dev, uint16_t data_len)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_CONFIG_OFFSET);
    regval &= ~I2C_CR_I2C_PKT_LEN_MASK;
    regval |= ((data_len - 1) << I2C_CR_I2C_PKT_LEN_SHIFT) & I2C_CR_I2C_PKT_LEN_MASK;
    putreg32(regval, reg_base + I2C_CONFIG_OFFSET);
}

static void bflb_i2c_set_frequence(struct bflb_device_s *dev, uint32_t freq)
{
    uint32_t regval;
    uint32_t reg_base;
    int32_t phase, phase0, phase1, phase2, phase3;
    int32_t bias;

    reg_base = dev->reg_base;

    phase = (bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_I2C, dev->idx) + freq / 2) / freq;
    if (freq <= 100 * 1000) {
        /* when SCL clock <= 100KHz, duty cycle is default 50%  */
        phase0 = (phase + 2) / 4;
        phase1 = phase0;
        phase2 = phase / 2 - phase0;
        phase3 = phase - phase0 - phase1 - phase2;
    } else {
        /* when SCL clock > 100KHz, duty cycle isdefault 33% */
        phase0 = (phase + 2) / 3;
        phase1 = (phase + 3) / 6;
        phase2 = (phase + 1) / 3 - phase1;
        phase3 = phase - phase0 - phase1 - phase2;
    }

    /* calculate rectify phase when de-glitch or clock-stretching is enabled */
    regval = getreg32(reg_base + I2C_CONFIG_OFFSET);
    if ((regval & I2C_CR_I2C_DEG_EN) && (regval & I2C_CR_I2C_SCL_SYNC_EN)) {
        bias = (regval & I2C_CR_I2C_DEG_CNT_MASK) >> I2C_CR_I2C_DEG_CNT_SHIFT;
        bias += 1;
    } else {
        bias = 0;
    }
    if (regval & I2C_CR_I2C_SCL_SYNC_EN) {
        bias += 3;
    }

    /* value should decrement one before write to register */
    phase0 = (phase0 <= 0) ? 1 : phase0;
    phase1 = (phase1 <= bias) ? (bias + 1) : phase1;
    phase2 = (phase2 <= 0) ? 1 : phase2;
    phase3 = (phase3 <= 0) ? 1 : phase3;
    /* only 8bit is available for phase0~3 in register */
    phase0 = (phase0 >= 256) ? 256 : phase0;
    phase1 = (phase1 >= 256) ? 256 : phase1;
    phase2 = (phase2 >= 256) ? 256 : phase2;
    phase3 = (phase0 >= 256) ? 256 : phase3;

    /* calculate data phase */
    regval = (phase0 - 1) << I2C_CR_I2C_PRD_D_PH_0_SHIFT;
    regval |= (((phase1 - bias - 1) <= 0) ? 1 : (phase1 - bias - 1)) << I2C_CR_I2C_PRD_D_PH_1_SHIFT; /* data phase1 must not be 0 */
    regval |= (phase2 - 1) << I2C_CR_I2C_PRD_D_PH_2_SHIFT;
    regval |= (phase3 - 1) << I2C_CR_I2C_PRD_D_PH_3_SHIFT;
    putreg32(regval, reg_base + I2C_PRD_DATA_OFFSET);
    /* calculate start phase */
    regval = (phase0 - 1) << I2C_CR_I2C_PRD_S_PH_0_SHIFT;
    regval |= (((phase0 + phase3 - 1) >= 256) ? 255 : (phase0 + phase3 - 1)) << I2C_CR_I2C_PRD_S_PH_1_SHIFT;
    regval |= (((phase1 + phase2 - 1) >= 256) ? 255 : (phase1 + phase2 - 1)) << I2C_CR_I2C_PRD_S_PH_2_SHIFT;
    regval |= (phase3 - 1) << I2C_CR_I2C_PRD_S_PH_3_SHIFT;
    putreg32(regval, reg_base + I2C_PRD_START_OFFSET);
    /* calculate stop phase */
    regval = (phase0 - 1) << I2C_CR_I2C_PRD_P_PH_0_SHIFT;
    regval |= (((phase1 + phase2 - 1) >= 256) ? 255 : (phase1 + phase2 - 1)) << I2C_CR_I2C_PRD_P_PH_1_SHIFT;
    regval |= (phase0 - 1) << I2C_CR_I2C_PRD_P_PH_2_SHIFT;
    regval |= (phase3 - 1) << I2C_CR_I2C_PRD_P_PH_3_SHIFT;
    putreg32(regval, reg_base + I2C_PRD_STOP_OFFSET);
}

static inline bool bflb_i2c_isbusy(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_BUS_BUSY_OFFSET);

    if (regval & I2C_STS_I2C_BUS_BUSY) {
        return true;
    }

    return false;
}

static inline bool bflb_i2c_isend(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_INT_STS_OFFSET);

    if (regval & I2C_END_INT) {
        return true;
    }

    return false;
}

static inline bool bflb_i2c_isnak(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_INT_STS_OFFSET);

    if (regval & I2C_NAK_INT) {
        return true;
    }

    return false;
}

static inline void bflb_i2c_enable(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_CONFIG_OFFSET);
    regval |= I2C_CR_I2C_M_EN;
    putreg32(regval, reg_base + I2C_CONFIG_OFFSET);
}

static inline void bflb_i2c_disable(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_CONFIG_OFFSET);
    regval &= ~I2C_CR_I2C_M_EN;
    putreg32(regval, reg_base + I2C_CONFIG_OFFSET);
    /* Clear I2C fifo */
    regval = getreg32(reg_base + I2C_FIFO_CONFIG_0_OFFSET);
    regval |= I2C_TX_FIFO_CLR;
    regval |= I2C_RX_FIFO_CLR;
    putreg32(regval, reg_base + I2C_FIFO_CONFIG_0_OFFSET);
    /* Clear I2C interrupt status */
    regval = getreg32(reg_base + I2C_INT_STS_OFFSET);
    regval |= I2C_CR_I2C_END_CLR;
    regval |= I2C_CR_I2C_NAK_CLR;
    regval |= I2C_CR_I2C_ARB_CLR;
    putreg32(regval, reg_base + I2C_INT_STS_OFFSET);
}

static inline bool bflb_i2c_isenable(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + I2C_CONFIG_OFFSET);
    if (regval & I2C_CR_I2C_M_EN) {
        return true;
    }

    return false;
}

static int bflb_i2c_write_bytes(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
    uint32_t reg_base;
    uint32_t temp = 0;
    uint8_t *tmp_buf;
    uint64_t start_time;

    reg_base = dev->reg_base;
    tmp_buf = data;
    while (len >= 4) {
        for (uint8_t i = 0; i < 4; i++) {
            temp += (tmp_buf[i] << ((i % 4) * 8));
        }
        tmp_buf += 4;
        len -= 4;
        start_time = bflb_mtimer_get_time_ms();
        while ((getreg32(reg_base + I2C_FIFO_CONFIG_1_OFFSET) & I2C_TX_FIFO_CNT_MASK) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        putreg32(temp, reg_base + I2C_FIFO_WDATA_OFFSET);
        if (!bflb_i2c_isenable(dev)) {
            bflb_i2c_enable(dev);
        }
        temp = 0;
    }

    if (len > 0) {
        for (uint8_t i = 0; i < len; i++) {
            temp += (tmp_buf[i] << ((i % 4) * 8));
        }
        start_time = bflb_mtimer_get_time_ms();
        while ((getreg32(reg_base + I2C_FIFO_CONFIG_1_OFFSET) & I2C_TX_FIFO_CNT_MASK) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        putreg32(temp, reg_base + I2C_FIFO_WDATA_OFFSET);
        if (!bflb_i2c_isenable(dev)) {
            bflb_i2c_enable(dev);
        }
    }

    start_time = bflb_mtimer_get_time_ms();
    while (bflb_i2c_isbusy(dev) || !bflb_i2c_isend(dev) || bflb_i2c_isnak(dev)) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }
    bflb_i2c_disable(dev);

    return 0;
}

static int bflb_i2c_read_bytes(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
    uint32_t reg_base;
    uint32_t temp = 0;
    uint8_t *tmp_buf;
    uint64_t start_time;

    reg_base = dev->reg_base;
    tmp_buf = data;

    bflb_i2c_enable(dev);

    while (len >= 4) {
        start_time = bflb_mtimer_get_time_ms();
        while ((getreg32(reg_base + I2C_FIFO_CONFIG_1_OFFSET) & I2C_RX_FIFO_CNT_MASK) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        temp = getreg32(reg_base + I2C_FIFO_RDATA_OFFSET);
        PUT_UINT32_LE(tmp_buf, temp);
        tmp_buf += 4;
        len -= 4;
    }

    if (len > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while ((getreg32(reg_base + I2C_FIFO_CONFIG_1_OFFSET) & I2C_RX_FIFO_CNT_MASK) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        temp = getreg32(reg_base + I2C_FIFO_RDATA_OFFSET);

        for (uint8_t i = 0; i < len; i++) {
            tmp_buf[i] = (temp >> (i * 8)) & 0xff;
        }
    }

    start_time = bflb_mtimer_get_time_ms();
    while (bflb_i2c_isbusy(dev) || !bflb_i2c_isend(dev)) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }
    bflb_i2c_disable(dev);

    return 0;
}

void bflb_i2c_init(struct bflb_device_s *dev, uint32_t frequency)
{
#ifdef romapi_bflb_i2c_init
    romapi_bflb_i2c_init(dev, frequency);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    bflb_i2c_disable(dev);

    regval = getreg32(reg_base + I2C_INT_STS_OFFSET);

    regval |= (I2C_CR_I2C_END_MASK |
               I2C_CR_I2C_TXF_MASK |
               I2C_CR_I2C_RXF_MASK |
               I2C_CR_I2C_NAK_MASK |
               I2C_CR_I2C_ARB_MASK |
               I2C_CR_I2C_FER_MASK);

    putreg32(regval, reg_base + I2C_INT_STS_OFFSET);

    bflb_i2c_set_frequence(dev, frequency);
#endif
}

void bflb_i2c_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_i2c_deinit
    romapi_bflb_i2c_deinit(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    bflb_i2c_disable(dev);

    regval = getreg32(reg_base + I2C_INT_STS_OFFSET);

    regval |= (I2C_CR_I2C_END_MASK |
               I2C_CR_I2C_TXF_MASK |
               I2C_CR_I2C_RXF_MASK |
               I2C_CR_I2C_NAK_MASK |
               I2C_CR_I2C_ARB_MASK |
               I2C_CR_I2C_FER_MASK);

    putreg32(regval, reg_base + I2C_INT_STS_OFFSET);
#endif
}

void bflb_i2c_link_txdma(struct bflb_device_s *dev, bool enable)
{
#ifdef romapi_bflb_i2c_link_txdma
    romapi_bflb_i2c_link_txdma(dev, enable);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + I2C_FIFO_CONFIG_0_OFFSET);
    if (enable) {
        regval |= I2C_DMA_TX_EN;
    } else {
        regval &= ~I2C_DMA_TX_EN;
    }
    putreg32(regval, reg_base + I2C_FIFO_CONFIG_0_OFFSET);
#endif
}

void bflb_i2c_link_rxdma(struct bflb_device_s *dev, bool enable)
{
#ifdef romapi_bflb_i2c_link_rxdma
    romapi_bflb_i2c_link_rxdma(dev, enable);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + I2C_FIFO_CONFIG_0_OFFSET);
    if (enable) {
        regval |= I2C_DMA_RX_EN;
    } else {
        regval &= ~I2C_DMA_RX_EN;
    }
    putreg32(regval, reg_base + I2C_FIFO_CONFIG_0_OFFSET);
#endif
}

int bflb_i2c_transfer(struct bflb_device_s *dev, struct bflb_i2c_msg_s *msgs, int count)
{
#ifdef romapi_bflb_i2c_transfer
    return romapi_bflb_i2c_transfer(dev, msgs, count);
#else
    uint16_t subaddr = 0;
    uint16_t subaddr_size = 0;
    bool is_addr_10bit = false;
    int ret = 0;

    if (count > 2) {
        return -EINVAL;
    }

    bflb_i2c_disable(dev);

    for (uint16_t i = 0; i < count; i++) {
        if (msgs[i].flags & I2C_M_TEN) {
            is_addr_10bit = true;
        } else {
            is_addr_10bit = false;
        }
        if (msgs[i].flags & I2C_M_NOSTOP) {
            subaddr = 0;
            for (uint8_t j = 0; j < msgs[i].length; j++) {
                subaddr += msgs[i].buffer[j] << (j * 8);
            }
            subaddr_size = msgs[i].length;
            bflb_i2c_addr_config(dev, msgs[i].addr, subaddr, subaddr_size, is_addr_10bit);
            i++;
        } else {
            subaddr = 0;
            subaddr_size = 0;
            bflb_i2c_addr_config(dev, msgs[i].addr, subaddr, subaddr_size, is_addr_10bit);
        }

        if (msgs[i].length > 256) {
            return -EINVAL;
        }
        bflb_i2c_set_datalen(dev, msgs[i].length);
        if (msgs[i].flags & I2C_M_READ) {
            bflb_i2c_set_dir(dev, 1);
            if ((msgs[i].flags & I2C_M_DMA) == 0) {
                ret = bflb_i2c_read_bytes(dev, msgs[i].buffer, msgs[i].length);
                if (ret < 0) {
                    return ret;
                }
            } else {
                bflb_i2c_enable(dev);
            }
        } else {
            bflb_i2c_set_dir(dev, 0);
            if ((msgs[i].flags & I2C_M_DMA) == 0) {
                ret = bflb_i2c_write_bytes(dev, msgs[i].buffer, msgs[i].length);
                if (ret < 0) {
                    return ret;
                }
            } else {
                bflb_i2c_enable(dev);
            }
        }
    }

    return 0;
#endif
}

void bflb_i2c_int_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_i2c_int_mask
    romapi_bflb_i2c_int_mask(dev, int_type, mask);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + I2C_INT_STS_OFFSET);
    regval &= ~((int_type & 0xff) << 8);
    if (mask) {
        regval |= (int_type & 0xff) << 8;
    }
    putreg32(regval, reg_base + I2C_INT_STS_OFFSET);
#endif
}

void bflb_i2c_int_clear(struct bflb_device_s *dev, uint32_t int_clear)
{
#ifdef romapi_bflb_i2c_int_clear
    romapi_bflb_i2c_int_clear(dev, int_clear);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + I2C_INT_STS_OFFSET);
    regval |= (int_clear & 0xff) << 16;
    putreg32(regval, reg_base + I2C_INT_STS_OFFSET);
#endif
}

uint32_t bflb_i2c_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_i2c_get_intstatus
    return romapi_bflb_i2c_get_intstatus(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;
    return (getreg32(reg_base + I2C_INT_STS_OFFSET) & 0xff);
#endif
}

int bflb_i2c_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_i2c_feature_control
    return romapi_bflb_i2c_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;
    uint32_t regval;
    struct bflb_i2c_timing_s *timing;

    reg_base = dev->reg_base;
    switch (cmd) {
        case I2C_CMD_SET_SCL_SYNC:
            regval = getreg32(reg_base + I2C_CONFIG_OFFSET);
            if (arg == 0) {
                regval &= ~I2C_CR_I2C_SCL_SYNC_EN;
            } else {
                regval |= I2C_CR_I2C_SCL_SYNC_EN;
            }
            putreg32(regval, reg_base + I2C_CONFIG_OFFSET);
            break;
        case I2C_CMD_SET_DEGLITCH:
            regval = getreg32(reg_base + I2C_CONFIG_OFFSET);
            regval &= ~I2C_CR_I2C_DEG_CNT_MASK;
            if (arg == 0) {
                regval &= ~I2C_CR_I2C_DEG_EN;
            } else {
                regval |= I2C_CR_I2C_DEG_EN;
                regval |= ((arg - 1) << I2C_CR_I2C_DEG_CNT_SHIFT);
            }
            putreg32(regval, reg_base + I2C_CONFIG_OFFSET);
            break;
        case I2C_CMD_SET_TIMING:
            timing = (struct bflb_i2c_timing_s *)arg;
            regval = getreg32(reg_base + I2C_PRD_DATA_OFFSET);
            regval &= ~I2C_CR_I2C_PRD_D_PH_0_MASK;
            regval |= (timing->data_phase0 << I2C_CR_I2C_PRD_D_PH_0_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_D_PH_1_MASK;
            regval |= (timing->data_phase1 << I2C_CR_I2C_PRD_D_PH_1_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_D_PH_2_MASK;
            regval |= (timing->data_phase2 << I2C_CR_I2C_PRD_D_PH_2_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_D_PH_3_MASK;
            regval |= (timing->data_phase3 << I2C_CR_I2C_PRD_D_PH_3_SHIFT);
            putreg32(regval, reg_base + I2C_PRD_DATA_OFFSET);
            regval = getreg32(reg_base + I2C_PRD_START_OFFSET);
            regval &= ~I2C_CR_I2C_PRD_S_PH_0_MASK;
            regval |= (timing->start_phase0 << I2C_CR_I2C_PRD_S_PH_0_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_S_PH_1_MASK;
            regval |= (timing->start_phase1 << I2C_CR_I2C_PRD_S_PH_1_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_S_PH_2_MASK;
            regval |= (timing->start_phase2 << I2C_CR_I2C_PRD_S_PH_2_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_S_PH_3_MASK;
            regval |= (timing->start_phase3 << I2C_CR_I2C_PRD_S_PH_3_SHIFT);
            putreg32(regval, reg_base + I2C_PRD_START_OFFSET);
            regval = getreg32(reg_base + I2C_PRD_STOP_OFFSET);
            regval &= ~I2C_CR_I2C_PRD_P_PH_0_MASK;
            regval |= (timing->stop_phase0 << I2C_CR_I2C_PRD_P_PH_0_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_P_PH_1_MASK;
            regval |= (timing->stop_phase1 << I2C_CR_I2C_PRD_P_PH_1_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_P_PH_2_MASK;
            regval |= (timing->stop_phase2 << I2C_CR_I2C_PRD_P_PH_2_SHIFT);
            regval &= ~I2C_CR_I2C_PRD_P_PH_3_MASK;
            regval |= (timing->stop_phase3 << I2C_CR_I2C_PRD_P_PH_3_SHIFT);
            putreg32(regval, reg_base + I2C_PRD_STOP_OFFSET);
            break;
        case I2C_CMD_GET_TIMING:
            timing = (struct bflb_i2c_timing_s *)arg;
            regval = getreg32(reg_base + I2C_PRD_DATA_OFFSET);
            timing->data_phase0 = (regval & I2C_CR_I2C_PRD_D_PH_0_MASK) >> I2C_CR_I2C_PRD_D_PH_0_SHIFT;
            timing->data_phase1 = (regval & I2C_CR_I2C_PRD_D_PH_1_MASK) >> I2C_CR_I2C_PRD_D_PH_1_SHIFT;
            timing->data_phase2 = (regval & I2C_CR_I2C_PRD_D_PH_2_MASK) >> I2C_CR_I2C_PRD_D_PH_2_SHIFT;
            timing->data_phase3 = (regval & I2C_CR_I2C_PRD_D_PH_3_MASK) >> I2C_CR_I2C_PRD_D_PH_3_SHIFT;
            regval = getreg32(reg_base + I2C_PRD_START_OFFSET);
            timing->start_phase0 = (regval & I2C_CR_I2C_PRD_S_PH_0_MASK) >> I2C_CR_I2C_PRD_S_PH_0_SHIFT;
            timing->start_phase1 = (regval & I2C_CR_I2C_PRD_S_PH_1_MASK) >> I2C_CR_I2C_PRD_S_PH_1_SHIFT;
            timing->start_phase2 = (regval & I2C_CR_I2C_PRD_S_PH_2_MASK) >> I2C_CR_I2C_PRD_S_PH_2_SHIFT;
            timing->start_phase3 = (regval & I2C_CR_I2C_PRD_S_PH_3_MASK) >> I2C_CR_I2C_PRD_S_PH_3_SHIFT;
            regval = getreg32(reg_base + I2C_PRD_STOP_OFFSET);
            timing->stop_phase0 = (regval & I2C_CR_I2C_PRD_P_PH_0_MASK) >> I2C_CR_I2C_PRD_P_PH_0_SHIFT;
            timing->stop_phase1 = (regval & I2C_CR_I2C_PRD_P_PH_1_MASK) >> I2C_CR_I2C_PRD_P_PH_1_SHIFT;
            timing->stop_phase2 = (regval & I2C_CR_I2C_PRD_P_PH_2_MASK) >> I2C_CR_I2C_PRD_P_PH_2_SHIFT;
            timing->stop_phase3 = (regval & I2C_CR_I2C_PRD_P_PH_3_MASK) >> I2C_CR_I2C_PRD_P_PH_3_SHIFT;
            break;
        default:
            ret = -EPERM;
            break;
    }
    return ret;
#endif
}
