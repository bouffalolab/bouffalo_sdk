#include "bflb_sdh.h"
#include "bflb_clock.h"
#include "hardware/sdh_reg.h"

int bflb_sdh_init(struct bflb_device_s *dev, struct bflb_sdh_config_s *cfg)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(cfg);

#ifdef romapi_bflb_sdh_init
    return romapi_bflb_sdh_init(dev, cfg);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* Enable internal clock*/
    regval = getreg16(reg_base + SDH_CLOCK_CTRL_OFFSET);
    regval |= SDH_INT_CLK_EN_MSK;
    putreg16(regval, reg_base + SDH_CLOCK_CTRL_OFFSET);

    /* Wait for internal clock stable */
    do {
        regval = getreg16(reg_base + SDH_CLOCK_CTRL_OFFSET);
    } while ((regval & SDH_INT_CLK_STABLE_MSK) == 0);

    /* DMA Select ADMA2 */
    regval = getreg16(reg_base + SDH_HOST_CTRL_OFFSET);
    regval &= ~SDH_DMA_SEL_MSK;
    regval |= 2 << SDH_DMA_SEL_POS;

    /* SD bus power vol */
    regval &= ~SDH_SD_BUS_VLT_MSK;
    regval |= cfg->power_vol << SDH_SD_BUS_VLT_POS;
    putreg16(regval, reg_base + SDH_HOST_CTRL_OFFSET);

#ifdef SDH_STD_V3
    /* DMA burst */
    regval = getreg16(reg_base + SDH_CLOCK_AND_BURST_SIZE_SETUP_OFFSET);
    regval &= ~SDH_BRST_SIZE_MSK;
    regval |= cfg->dma_burst << SDH_BRST_SIZE_POS;

    /* FIFO Threshold */
    regval &= ~SDH_DMA_SIZE_MSK;
    regval |= cfg->dma_fifo_th << SDH_DMA_SIZE_POS;
    putreg16(regval, reg_base + SDH_CLOCK_AND_BURST_SIZE_SETUP_OFFSET);

    /* tx root clock cfg */
    regval = getreg16(reg_base + SDH_TX_CFG_REG_OFFSET);
    regval |= SDH_TX_INT_CLK_SEL_MSK;
    putreg16(regval, reg_base + SDH_TX_CFG_REG_OFFSET);
#endif

    /* enable all normal status */
    regval = SDH_NORMAL_STA_CMD_COMP | SDH_NORMAL_STA_TRAN_COMP | SDH_NORMAL_STA_BLK_GAP_EVENT |
             SDH_NORMAL_STA_DMA_INT | SDH_NORMAL_STA_BUFF_WR_RDY | SDH_NORMAL_STA_BUFF_RD_RDY |
             SDH_NORMAL_STA_CARD_INSERT | SDH_NORMAL_STA_CARD_REMOVE | SDH_NORMAL_STA_CARD_INT |
             SDH_NORMAL_STA_INT_A | SDH_NORMAL_STA_INT_B | SDH_NORMAL_STA_INT_C |
             SDH_NORMAL_STA_RETUNING_EVENT;
    /* enable all error status */
    regval |= SDH_ERROR_STA_CMD_TIMEOUT | SDH_ERROR_STA_CMD_CRC_ERR | SDH_ERROR_STA_CMD_END_BIT |
              SDH_ERROR_STA_CMD_IDX_ERR | SDH_ERROR_STA_DATA_TIMEOUT | SDH_ERROR_STA_DATA_CRC_ERR |
              SDH_ERROR_STA_DATA_END_BIT | SDH_ERROR_STA_AUTO_CMD_ERR | SDH_ERROR_STA_ADMA_ERR |
              SDH_ERROR_STA_TUNING_ERROR;
    putreg32(regval, reg_base + SDH_NORMAL_INT_STATUS_EN_OFFSET);

    return 0;
#endif
}

/* Convert the description of the data transfer into ADMA2 hardware descriptors. */
int bflb_sdh_adma2_desc_init(struct bflb_device_s *dev, struct bflb_sdh_data_cfg_s *data_cfg)
{
    LHAL_PARAM_ASSERT(dev);

#ifdef romapi_bflb_sdh_adma2_desc_init
    return romapi_bflb_sdh_adma2_desc_init(dev, data_cfg);
#else
    struct bflb_sdh_data_tranfer_s *curr_tranfer = NULL;
    struct bflb_sdh_adma2_hw_desc_s *curr_hw_desc = NULL;
    uint32_t hw_desc_cnt = 0;

    if (data_cfg == NULL) {
        return -1;
    }

    if (data_cfg->adma2_hw_desc_raw_mode) {
        return 0;
    }

    for (uint32_t data_desc_cnt = 0; data_desc_cnt < data_cfg->adma_tranfer_cnt; data_desc_cnt++) {
        /*  */
        curr_tranfer = &(data_cfg->adma_tranfer[data_desc_cnt]);
        uint32_t tranfer_addr = (uint32_t)(uintptr_t)curr_tranfer->address;
        uint32_t tranfer_len = curr_tranfer->length;
        bool tranfer_int_en = curr_tranfer->int_en;

        /* Check for unaligned address. */
        if (tranfer_addr % USDHC_ADMA2_ADDRESS_ALIGN) {
            return -2;
        }

        for (uint32_t curr_len = 0; curr_len < tranfer_len;) {
            /*  */
            curr_hw_desc = &(data_cfg->adma2_hw_desc[hw_desc_cnt]);
            /* data address */
            curr_hw_desc->address = tranfer_addr + curr_len;
            /* desc attribute */
            curr_hw_desc->attribute = SDH_ADMA2_ATTR_VALID | SDH_ADMA2_ATTR_ACT_TRAN;

            /* data length */
            if (tranfer_len - curr_len > SDH_ADMA2_DESC_LEN_MAX) {
                curr_hw_desc->length = SDH_ADMA2_DESC_LEN_MAX & SDH_ADMA2_DESC_LEN_MASK;
                curr_len += SDH_ADMA2_DESC_LEN_MAX;
            } else {
                curr_hw_desc->length = tranfer_len - curr_len;
                curr_len = tranfer_len;

                /* Check for unaligned size. */
                if (curr_hw_desc->length % USDHC_ADMA2_ADDRESS_ALIGN) {
                    return -3;
                }
            }

            hw_desc_cnt += 1;

            /* The number of hardware descriptors is insufficient. */
            if (hw_desc_cnt > data_cfg->adma2_hw_desc_cnt) {
                return -4;
            }
        }

        /* Optional data segment interrupt. */
        if (tranfer_int_en) {
            curr_hw_desc->attribute |= SDH_ADMA2_ATTR_INT;
        }
    }

    /* End flag of the hardware descriptor. */
    if (curr_hw_desc) {
        curr_hw_desc->attribute |= SDH_ADMA2_ATTR_END;
    }

    return 0;
#endif
}

/*  */
int bflb_sdh_cmd_cfg(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(cmd_cfg);

#ifdef romapi_bflb_sdh_cmd_cfg
    return romapi_bflb_sdh_cmd_cfg(dev, cmd_cfg);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* check cmd inhibit */
    regval = getreg16(reg_base + SDH_PRESENT_STATE_1_OFFSET);
    if (regval & SDH_CMD_INHIBIT_CMD_MSK) {
        return -1;
    }

    /* check data inhibit */
    if (cmd_cfg->resp_type == SDH_RESP_TPYE_R1b || cmd_cfg->resp_type == SDH_RESP_TPYE_R5b) {
        if (cmd_cfg->cmd_type != SDH_CMD_TPYE_ABORT) {
            if (regval & SDH_CMD_INHIBIT_DAT_MSK) {
                return -2;
            }
        }
    }

    /* cmd arg */
    putreg32(cmd_cfg->argument, reg_base + SDH_ARG_LOW_OFFSET);

    /* cmd cfg */
    regval = getreg8(reg_base + SDH_CMD_OFFSET);

    /* cmd type */
    regval &= ~SDH_CMD_TYPE_MSK;
    regval |= (cmd_cfg->cmd_type << SDH_CMD_TYPE_POS) & SDH_CMD_TYPE_MSK;

    regval &= ~SDH_RESP_TYPE_MSK;

    switch (cmd_cfg->resp_type) {
        case SDH_RESP_TPYE_R2:
            /* resp_type:0x01, 136-bit */
            regval |= 1 << SDH_RESP_TYPE_POS;
            /* index_check: disable */
            regval &= ~SDH_CMD_INDEX_CHK_EN_MSK;
            /* crc_check: enable */
            regval |= SDH_CMD_CRC_CHK_EN_MSK;
            break;

        case SDH_RESP_TPYE_R3:
        case SDH_RESP_TPYE_R4:
            /* resp_type:0x02, 48-bit */
            regval |= 2 << SDH_RESP_TYPE_POS;
            /* index_check: disable */
            regval &= ~SDH_CMD_INDEX_CHK_EN_MSK;
            /* crc_check: disable */
            regval &= ~SDH_CMD_CRC_CHK_EN_MSK;
            break;

        case SDH_RESP_TPYE_R1:
        case SDH_RESP_TPYE_R5:
        case SDH_RESP_TPYE_R6:
        case SDH_RESP_TPYE_R7:
            /* resp_type:0x02, 48-bit */
            regval |= 2 << SDH_RESP_TYPE_POS;
            /* index_check: enable */
            regval |= SDH_CMD_INDEX_CHK_EN_MSK;
            /* crc_check: enable */
            regval |= SDH_CMD_CRC_CHK_EN_MSK;
            break;

        case SDH_RESP_TPYE_R1b:
        case SDH_RESP_TPYE_R5b:
            /* resp_type:0x03, 48-bit, check busy after response */
            regval |= 3 << SDH_RESP_TYPE_POS;
            /* index_check: enable */
            regval |= SDH_CMD_INDEX_CHK_EN_MSK;
            /* crc_check: enable */
            regval |= SDH_CMD_CRC_CHK_EN_MSK;
            break;

        default:
            /* No response */
            break;
    }

    /* put reg but not trig cmd send */
    putreg8(regval, reg_base + SDH_CMD_OFFSET);

    return 0;
#endif
}

int bflb_sdh_data_cfg(struct bflb_device_s *dev, struct bflb_sdh_data_cfg_s *data_cfg)
{
    LHAL_PARAM_ASSERT(dev);

#ifdef romapi_bflb_sdh_data_cfg
    return romapi_bflb_sdh_data_cfg(dev, data_cfg);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* without data */
    if (data_cfg == NULL) {
        /* no data present */
        regval = getreg8(reg_base + SDH_CMD_OFFSET);
        regval &= ~SDH_DATA_PRESENT_MSK;
        putreg8(regval, reg_base + SDH_CMD_OFFSET);

        /* disable auto cmd */
        regval = getreg16(reg_base + SDH_TRANSFER_MODE_OFFSET);
        regval &= ~SDH_AUTO_CMD_EN_MSK;

        /* disable multi block mode */
        regval &= ~SDH_MULTI_BLK_SEL_MSK;

        /* disable dma */
        regval &= ~SDH_DMA_EN_MSK;
        putreg16(regval, reg_base + SDH_TRANSFER_MODE_OFFSET);
        return 0;
    }

    /* check data inhibit */
    regval = getreg16(reg_base + SDH_PRESENT_STATE_1_OFFSET);
    if (regval & SDH_CMD_INHIBIT_DAT_MSK) {
        return -2;
    }

    /* data present */
    regval = getreg8(reg_base + SDH_CMD_OFFSET);
    regval |= SDH_DATA_PRESENT_MSK;
    putreg8(regval, reg_base + SDH_CMD_OFFSET);

    /* data transfer direction */
    regval = getreg16(reg_base + SDH_TRANSFER_MODE_OFFSET);
    if (data_cfg->data_dir == SDH_TRANSFER_DIR_RD) {
        regval |= SDH_TO_HOST_DIR_MSK;
    } else {
        regval &= ~SDH_TO_HOST_DIR_MSK;
    }

    /* auto cmd */
    regval &= ~SDH_AUTO_CMD_EN_MSK;
    regval |= data_cfg->auto_cmd_mode << SDH_AUTO_CMD_EN_POS;

    /* auto cmd23 arg, block cnt */
    if (data_cfg->auto_cmd_mode == SDH_AUTO_CMD_CMD23) {
        putreg32(data_cfg->block_count, reg_base + SDH_SYS_ADDR_LOW_OFFSET);
    }

    /* multi block mode */
    if (data_cfg->block_count != 1) {
        regval |= SDH_MULTI_BLK_SEL_MSK;
    } else {
        regval &= ~SDH_MULTI_BLK_SEL_MSK;
    }

    /* block cnt enable */
    if (data_cfg->block_count != 0) {
        regval |= SDH_BLK_CNT_EN_MSK;
    } else {
        regval &= ~SDH_BLK_CNT_EN_MSK;
    }

    /* adma2 addr */
    putreg32((uint32_t)(uintptr_t)data_cfg->adma2_hw_desc, reg_base + SDH_ADMA_SYS_ADDR_1_OFFSET);

    /* dma enable */
    regval |= SDH_DMA_EN_MSK;
    putreg16(regval, reg_base + SDH_TRANSFER_MODE_OFFSET);

    /* set block cnt */
    if (data_cfg->block_count != 0) {
        putreg16(data_cfg->block_count, reg_base + SDH_BLOCK_COUNT_OFFSET);
    }

    /* block size */
    regval = getreg16(reg_base + SDH_BLOCK_SIZE_OFFSET);
    regval &= ~SDH_BLOCK_SIZE_MSK;
    regval |= data_cfg->block_size << SDH_BLOCK_SIZE_POS;
    putreg16(regval, reg_base + SDH_BLOCK_SIZE_OFFSET);

    return 0;
#endif
}

int bflb_sdh_tranfer_start(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg, struct bflb_sdh_data_cfg_s *data_cfg)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(cmd_cfg);

#ifdef romapi_bflb_sdh_tranfer_start
    return romapi_bflb_sdh_tranfer_start(dev, cmd_cfg, data_cfg);
#else
    int ret;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    /* cmd cfg */
    ret = bflb_sdh_cmd_cfg(dev, cmd_cfg);
    if (ret < 0) {
        return ret;
    }

    /* Convert the ADMA2 description */
    if (data_cfg) {
        ret = bflb_sdh_adma2_desc_init(dev, data_cfg);
        if (ret < 0) {
            return ret;
        }
    }

    /* data cfg */
    ret = bflb_sdh_data_cfg(dev, data_cfg);
    if (ret < 0) {
        return ret;
    }

    /* Trigger a transfer transaction. */
    putreg8(cmd_cfg->index, reg_base + SDH_CMD_OFFSET + 1);

    return 0;
#endif
}

int bflb_sdh_get_resp(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(cmd_cfg);

#ifdef romapi_bflb_sdh_get_resp
    return romapi_bflb_sdh_get_resp(dev, cmd_cfg);
#else
    uint32_t reg_base;
    reg_base = dev->reg_base;

    if (cmd_cfg->resp_type == SDH_RESP_TPYE_NONE) {
        /* no resp */
        cmd_cfg->resp[0] = 0;
        cmd_cfg->resp[1] = 0;
        cmd_cfg->resp[2] = 0;
        cmd_cfg->resp[3] = 0;
    } else {
        /* 136-bit / 48-bit resp */
        cmd_cfg->resp[0] = getreg32(reg_base + SDH_RESP_0_OFFSET);
        cmd_cfg->resp[1] = getreg32(reg_base + SDH_RESP_2_OFFSET);
        cmd_cfg->resp[2] = getreg32(reg_base + SDH_RESP_4_OFFSET);
        cmd_cfg->resp[3] = getreg32(reg_base + SDH_RESP_6_OFFSET);
    }
    return 0;
#endif
}

/* enable/disable status sig  */
void bflb_sdh_sta_en(struct bflb_device_s *dev, uint32_t sta_bit, bool en)
{
#ifdef romapi_bflb_sdh_sta_en
    romapi_bflb_sdh_sta_en(dev, sta_bit, en);
#else
    uint32_t regval;
    uint32_t reg_base = dev->reg_base;

    regval = getreg32(reg_base + SDH_NORMAL_INT_STATUS_EN_OFFSET);
    if (en) {
        regval |= sta_bit;
    } else {
        regval &= ~sta_bit;
    }
    /* control error interrupt using the SDH_ERROR_xxx */
    regval &= ~SDH_NORMAL_STA_ERROR;
    putreg32(regval, reg_base + SDH_NORMAL_INT_STATUS_EN_OFFSET);
#endif
}

uint32_t bflb_sdh_sta_en_get(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_sdh_sta_en_get
    return romapi_bflb_sdh_sta_en_get(dev);
#else
    uint32_t regval;
    uint32_t reg_base = dev->reg_base;
    regval = getreg32(reg_base + SDH_NORMAL_INT_STATUS_EN_OFFSET);

    return regval;
#endif
}

/* enable/disable status interrupt */
void bflb_sdh_sta_int_en(struct bflb_device_s *dev, uint32_t sta_bit, bool en)
{
#ifdef romapi_bflb_sdh_sta_int_en
    romapi_bflb_sdh_sta_int_en(dev, sta_bit, en);
#else
    uint32_t regval;
    uint32_t reg_base = dev->reg_base;

    regval = getreg32(reg_base + SDH_NORMAL_INT_STATUS_INT_EN_OFFSET);
    if (en) {
        regval |= sta_bit;
    } else {
        regval &= ~sta_bit;
    }
    /* control error interrupt using the SDH_ERROR_xxx */
    regval &= ~SDH_NORMAL_STA_ERROR;
    putreg32(regval, reg_base + SDH_NORMAL_INT_STATUS_INT_EN_OFFSET);
#endif
}

uint32_t bflb_sdh_sta_int_en_get(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_sdh_sta_int_en_get
    return romapi_bflb_sdh_sta_int_en_get(dev);
#else
    uint32_t regval;
    uint32_t reg_base = dev->reg_base;
    regval = getreg32(reg_base + SDH_NORMAL_INT_STATUS_INT_EN_OFFSET);

    return regval;
#endif
}

/* get status */
uint32_t bflb_sdh_sta_get(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_sdh_sta_get
    return romapi_bflb_sdh_sta_get(dev);
#else
    uint32_t regval;
    uint32_t reg_base = dev->reg_base;
    regval = getreg32(reg_base + SDH_NORMAL_INT_STATUS_OFFSET);

    return regval;
#endif
}

/* clr status */
void bflb_sdh_sta_clr(struct bflb_device_s *dev, uint32_t sta_bit)
{
#ifdef romapi_bflb_sdh_sta_clr
    romapi_bflb_sdh_sta_clr(dev, sta_bit);
#else
    uint32_t reg_base = dev->reg_base;
    putreg32(sta_bit, reg_base + SDH_NORMAL_INT_STATUS_OFFSET);
#endif
}

int bflb_sdh_feature_control(struct bflb_device_s *dev, int cmd, uintptr_t arg)
{
#ifdef romapi_bflb_sdh_feature_control
    return romapi_bflb_sdh_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    switch (cmd) {
        case SDH_CMD_SOFT_RESET_ALL:
            break;

        case SDH_CMD_SET_BUS_WIDTH:
            regval = getreg16(reg_base + SDH_HOST_CTRL_OFFSET);
            if (arg == 1) {
                regval &= ~SDH_EX_DATA_WIDTH_MSK;
                regval &= ~SDH_DATA_WIDTH_MSK;
            } else if (arg == 4) {
                regval &= ~SDH_EX_DATA_WIDTH_MSK;
                regval |= SDH_DATA_WIDTH_MSK;
            } else if (arg == 8) {
                regval |= SDH_EX_DATA_WIDTH_MSK;
            } else {
                ret = -1;
            }
            putreg16(regval, reg_base + SDH_HOST_CTRL_OFFSET);
            break;

        case SDH_CMD_SET_SD_BUS_POWER:
            regval = getreg16(reg_base + SDH_HOST_CTRL_OFFSET);
            if (arg) {
                regval |= SDH_SD_BUS_POWER_MSK;
            } else {
                regval &= ~SDH_SD_BUS_POWER_MSK;
            }
            putreg16(regval, reg_base + SDH_HOST_CTRL_OFFSET);
            break;

        case SDH_CMD_SET_HS_MODE_EN:
            regval = getreg16(reg_base + SDH_HOST_CTRL_OFFSET);
            if (arg) {
                regval |= SDH_HI_SPEED_EN_MSK;
            } else {
                regval &= ~SDH_HI_SPEED_EN_MSK;
            }
            putreg16(regval, reg_base + SDH_HOST_CTRL_OFFSET);
            break;

        case SDH_CMD_SET_INTERNAL_CLK_EN:
            regval = getreg16(reg_base + SDH_CLOCK_CTRL_OFFSET);
            if (arg) {
                regval |= SDH_INT_CLK_EN_MSK;
            } else {
                regval &= ~SDH_INT_CLK_EN_MSK;
            }
            putreg16(regval, reg_base + SDH_CLOCK_CTRL_OFFSET);
            break;

        case SDH_CMD_GET_INTERNAL_CLK_STABLE:
            regval = getreg16(reg_base + SDH_CLOCK_CTRL_OFFSET);
            if (regval & SDH_INT_CLK_STABLE_MSK) {
                return ret = 1;
            }
            break;

        case SDH_CMD_SET_BUS_CLK_EN:
            regval = getreg16(reg_base + SDH_CLOCK_CTRL_OFFSET);
            if (arg) {
                regval |= SDH_SD_CLK_EN_MSK;
            } else {
                regval &= ~SDH_SD_CLK_EN_MSK;
            }
            putreg16(regval, reg_base + SDH_CLOCK_CTRL_OFFSET);
            break;

        case SDH_CMD_SET_BUS_CLK_DIV:
            arg = (arg + 1) / 2;
            arg = (arg > 0x3ff) ? 0x3ff : arg;
            regval = getreg16(reg_base + SDH_CLOCK_CTRL_OFFSET);
            /* bit[7:0] */
            regval &= ~SDH_SD_FREQ_SEL_LO_MSK;
            regval |= (arg << SDH_SD_FREQ_SEL_LO_POS) & SDH_SD_FREQ_SEL_LO_MSK;
            /* bit[9:8] */
            regval &= ~SDH_SD_FREQ_SEL_HI_MSK;
            regval |= ((arg >> 8) << SDH_SD_FREQ_SEL_HI_POS) & SDH_SD_FREQ_SEL_HI_MSK;
            putreg16(regval, reg_base + SDH_CLOCK_CTRL_OFFSET);
            break;

        case SDH_CMD_SET_DATA_TIMEOUT_CNT_VAL:
            /*!< Data timeout_s = (2^(val+13)) / root_clk_freq */
            regval = getreg8(reg_base + SDH_TIMEOUT_CTRL_SW_RESET_OFFSET);
            regval &= ~SDH_TIMEOUT_VALUE_MSK;
            regval |= (arg << SDH_TIMEOUT_VALUE_POS) & SDH_TIMEOUT_VALUE_MSK;
            putreg8(regval, reg_base + SDH_TIMEOUT_CTRL_SW_RESET_OFFSET);
            break;

        case SDH_CMD_SET_ASYNC_INT_EN:
            regval = getreg16(reg_base + SDH_HOST_CTRL_2_OFFSET);
            if (arg) {
                regval |= SDH_ASYNC_INT_EN_MSK;
            } else {
                regval &= ~SDH_ASYNC_INT_EN_MSK;
            }
            putreg16(regval, reg_base + SDH_HOST_CTRL_2_OFFSET);
            break;

#ifdef SDH_STD_V3
        case SDH_CMD_ACTIVE_CLK_OUT:
            /* enable and clr misc completed sta */
            regval = getreg16(reg_base + SDH_CE_ATA_2_OFFSET);
            regval |= SDH_MISC_INT_MSK;
            regval |= SDH_MISC_INT_EN_MSK;
            putreg16(regval, reg_base + SDH_CE_ATA_2_OFFSET);
            /* set misc clock cnt and start out */
            regval = getreg32(reg_base + SDH_CFG_FIFO_PARAM_OFFSET);
            regval &= ~SDH_GEN_PAD_CLK_CNT_MSK;
            regval |= (arg << SDH_GEN_PAD_CLK_CNT_POS) & SDH_GEN_PAD_CLK_CNT_MSK;
            putreg32(regval, reg_base + SDH_CFG_FIFO_PARAM_OFFSET);
            regval |= SDH_GEN_PAD_CLK_ON_MSK;
            putreg32(regval, reg_base + SDH_CFG_FIFO_PARAM_OFFSET);
            /* wait misc completed */
            do {
                regval = getreg16(reg_base + SDH_CE_ATA_2_OFFSET);
            } while ((regval & SDH_MISC_INT_MSK) == 0);
            /* clr misc completed sta */
            regval |= SDH_MISC_INT_MSK;
            putreg16(regval, reg_base + SDH_CE_ATA_2_OFFSET);
            break;

        case SDH_CMD_FORCE_CLK_OUTPUT:
            regval = getreg32(reg_base + SDH_FIFO_PARAM_OFFSET);
            regval |= SDH_OVRRD_CLK_OEN_MSK;
            if (arg) {
                regval |= SDH_FORCE_CLK_ON_MSK;
            } else {
                regval &= ~SDH_FORCE_CLK_ON_MSK;
            }
            putreg32(regval, reg_base + SDH_FIFO_PARAM_OFFSET);
            break;
#endif
        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
