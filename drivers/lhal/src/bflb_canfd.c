#include "bflb_canfd.h"
#include "bflb_clock.h"
#include "hardware/canfd_reg.h"

__UNUSED static const uint8_t bflb_canfd_dlc_table[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    12, 16, 20, 24, 32, 48, 64,
};

__UNUSED static uint8_t bflb_canfd_dlc_length_to_code(uint8_t length)
{
    if (length <= 8) {
        return length;
    } else if (length <= 24) {
        return 6 + (length + 3) / 4;
    } else if (length <= 32) {
        return 13;
    } else if (length <= 64) {
        return 11 + (length + 15) / 16;
    } else {
        return 15;
    }
}

__UNUSED static uint8_t bflb_canfd_dlc_code_to_length(uint8_t code)
{
    LHAL_PARAM_ASSERT(code < 16);
    return bflb_canfd_dlc_table[code];
}

__UNUSED static void bflb_canfd_set_acf(struct bflb_device_s *dev, struct bflb_canfd_acf_s *acf)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* set code value */
    regval = getreg32(reg_base + CANFD_ACFCTRL_OFFSET);
    regval &= ~CANFD_ACFCTRL_ACFADR_MASK;
    regval |= (acf->index << CANFD_ACFCTRL_ACFADR_SHIFT);
    regval &= ~CANFD_ACFCTRL_SELMASK;
    putreg32(regval, reg_base + CANFD_ACFCTRL_OFFSET);
    putreg32(acf->code, reg_base + CANFD_ACF_X_OFFSET);
    /* set mask value */
    regval = getreg32(reg_base + CANFD_ACFCTRL_OFFSET);
    regval &= ~CANFD_ACFCTRL_ACFADR_MASK;
    regval |= (acf->index << CANFD_ACFCTRL_ACFADR_SHIFT);
    regval |= CANFD_ACFCTRL_SELMASK;
    putreg32(regval, reg_base + CANFD_ACFCTRL_OFFSET);
    if (acf->id_type == CANFD_ACF_STANDARD_ONLY) {
        regval = CANFD_ACF_X_AIDEE;
    } else if (acf->id_type == CANFD_ACF_EXTENDED_ONLY) {
        regval = CANFD_ACF_X_AIDEE | CANFD_ACF_X_AIDE;
    } else {
        regval = 0;
    }
    putreg32(acf->mask | regval, reg_base + CANFD_ACF_X_OFFSET);
    /* set enable value */
    regval = getreg32(reg_base + CANFD_ACFCTRL_OFFSET);
    if (acf->enable) {
        regval |= (1 << (acf->index + CANFD_ACF_EN_SHIFT));
    } else {
        regval &= ~(1 << (acf->index + CANFD_ACF_EN_SHIFT));
    }
    putreg32(regval, reg_base + CANFD_ACFCTRL_OFFSET);
}

__UNUSED static void bflb_canfd_write_message(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data)
{
    uint32_t reg_base;
    uint32_t regval;
    uint8_t dlc, i;

    reg_base = dev->reg_base;

    /* write ID */
    if (head->time_stamping_enable) {
        putreg32(head->identifier | CANFD_TBUF_ID_TTSEN, reg_base + CANFD_TBUF_ID_OFFSET);
    } else {
        putreg32(head->identifier, reg_base + CANFD_TBUF_ID_OFFSET);
    }
    /* write control information */
    dlc = bflb_canfd_dlc_length_to_code(head->data_length);
    regval = dlc << CANFD_TBUF_CTRL_DLC_SHIFT;
    if (head->id_type == CANFD_ID_EXTENDED) {
        regval |= CANFD_TBUF_CTRL_IDE;
    }
    if (head->frame_type == CANFD_FRAME_REMOTE) {
        regval |= CANFD_TBUF_CTRL_RTR;
    }
    if (head->bit_rate_switch == CANFD_BRS_ON) {
        regval |= CANFD_TBUF_CTRL_BRS;
    }
    if (head->fd_format == CANFD_FORMAT_FD_CAN) {
        regval |= CANFD_TBUF_CTRL_FDF;
    }
    putreg32(regval, reg_base + CANFD_TBUF_CTRL_OFFSET);
    /* write data */
    regval = 0;
    for (i = 0; i < head->data_length; i++) {
        regval |= (data[i] << ((i % 4) * 8));
        if (i % 4 == 3) {
            putreg32(regval, reg_base + CANFD_TBUF_DATA_OFFSET + i - 3);
            regval = 0;
        }
    }
    if (i % 4 != 0) {
        putreg32(regval, reg_base + CANFD_TBUF_DATA_OFFSET + (i / 4) * 4);
    }
}

__UNUSED static void bflb_canfd_read_message(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data)
{
    uint32_t reg_base;
    uint32_t regval;
    uint8_t dlc, i;
    uint64_t time_stamping;

    reg_base = dev->reg_base;

    /* read control information */
    regval = getreg32(reg_base + CANFD_RBUF_CTRL_OFFSET);
    dlc = (regval & CANFD_RBUF_CTRL_DLC_MASK) >> CANFD_RBUF_CTRL_DLC_SHIFT;
    head->data_length = bflb_canfd_dlc_code_to_length(dlc);
    if (regval & CANFD_RBUF_CTRL_IDE) {
        head->id_type = CANFD_ID_EXTENDED;
    } else {
        head->id_type = CANFD_ID_STANDARD;
    }
    if (regval & CANFD_RBUF_CTRL_RTR) {
        head->frame_type = CANFD_FRAME_REMOTE;
    } else {
        head->frame_type = CANFD_FRAME_DATA;
    }
    if (regval & CANFD_RBUF_CTRL_BRS) {
        head->bit_rate_switch = CANFD_BRS_ON;
    } else {
        head->bit_rate_switch = CANFD_BRS_OFF;
    }
    if (regval & CANFD_RBUF_CTRL_FDF) {
        head->fd_format = CANFD_FORMAT_FD_CAN;
    } else {
        head->fd_format = CANFD_FORMAT_CLASSIC_CAN;
    }
    /* read ID and ESI */
    regval = getreg32(reg_base + CANFD_RBUF_ID_OFFSET);
    if (regval & CANFD_RBUF_ID_ESI) {
        head->error_state_indicator = CANFD_ESI_PASSIVE;
    } else {
        head->error_state_indicator = CANFD_ESI_ACTIVE;
    }
    if (head->id_type == CANFD_ID_EXTENDED) {
        head->identifier = regval & CANFD_ID_EXTENDED_MASK;
    } else {
        head->identifier = regval & CANFD_ID_STANDARD_MASK;
    }
    /* read data */
    for (i = 0; i < head->data_length; i++) {
        if (i % 4 == 0) {
            regval = getreg32(reg_base + CANFD_RBUF_DATA_OFFSET + i);
        }
        data[i] = (regval >> ((i % 4) * 8)) & 0xFF;
    }
    /* read time stamping */
    if (head->time_stamping_enable) {
        regval = getreg32(reg_base + CANFD_RBUF_RTS_OFFSET + 4);
        time_stamping = ((uint64_t)regval) << 32;
        regval = getreg32(reg_base + CANFD_RBUF_RTS_OFFSET);
        time_stamping |= (uint64_t)regval;
        head->time_stamping = time_stamping;
    }
}

int bflb_canfd_init(struct bflb_device_s *dev, const struct bflb_canfd_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_CANFD_MODE_TYPE(config->mode));
    LHAL_PARAM_ASSERT(config->timing_slow.prescaler < 256);
    LHAL_PARAM_ASSERT(config->timing_slow.segment_1 < 256);
    LHAL_PARAM_ASSERT(config->timing_slow.segment_2 < 128);
    LHAL_PARAM_ASSERT(config->timing_slow.sync_jump_width < 128);
    LHAL_PARAM_ASSERT(config->timing_fast.prescaler < 256);
    LHAL_PARAM_ASSERT(config->timing_fast.segment_1 < 32);
    LHAL_PARAM_ASSERT(config->timing_fast.segment_2 < 16);
    LHAL_PARAM_ASSERT(config->timing_fast.sync_jump_width < 16);

#ifdef romapi_bflb_canfd_init
    return romapi_bflb_canfd_init(dev, config);
#else
    uint64_t start_time;
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* force canfd in reset state */
    putreg32(CANFD_CFG_STAT_RESET, reg_base + CANFD_CTRL_OFFSET);
    /* set default state */
    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    if (config->bosch_mode == 0) {
        regval |= CANFD_TCTRL_FD_ISO;
    }
    regval |= CANFD_TCTRL_TTTBM;
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
    regval = (0xB << CANFD_LIMIT_EWL_SHIFT);
    regval |= (0x1 << CANFD_LIMIT_AFWL_SHIFT);
    putreg32(regval, reg_base + CANFD_INT_OFFSET);
    putreg32(0, reg_base + CANFD_ERROR_OFFSET);

    /* configure slow speed timing */
    regval = (config->timing_slow.prescaler << CANFD_S_PRESC_SHIFT);
    regval |= (config->timing_slow.segment_1 << CANFD_S_SEG_1_SHIFT);
    regval |= (config->timing_slow.segment_2 << CANFD_S_SEG_2_SHIFT);
    regval |= (config->timing_slow.sync_jump_width << CANFD_S_SJW_SHIFT);
    putreg32(regval, reg_base + CANFD_S_TIMING_OFFSET);
    /* configure fast speed timing */
    regval = (config->timing_fast.prescaler << CANFD_F_PRESC_SHIFT);
    regval |= (config->timing_fast.segment_1 << CANFD_F_SEG_1_SHIFT);
    regval |= (config->timing_fast.segment_2 << CANFD_F_SEG_2_SHIFT);
    regval |= (config->timing_fast.sync_jump_width << CANFD_F_SJW_SHIFT);
    putreg32(regval, reg_base + CANFD_F_TIMING_OFFSET);

    /* set acceptance filter */
    if (config->acf && config->acf_count) {
        regval = getreg32(reg_base + CANFD_ACFCTRL_OFFSET);
        regval &= ~CANFD_ACF_EN_MASK;
        putreg32(regval, reg_base + CANFD_ACFCTRL_OFFSET);
        for (uint8_t i = 0; i < config->acf_count; i++) {
            bflb_canfd_set_acf(dev, &config->acf[i]);
        }
    }

    /* release reset state */
    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    regval &= ~CANFD_CFG_STAT_RESET;
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
    /* wait until reset status release */
    start_time = bflb_mtimer_get_time_ms();
    while ((getreg32(reg_base + CANFD_CTRL_OFFSET) & CANFD_CFG_STAT_RESET) != 0) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }

    /* config operation mode */
    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    if (config->mode == CANFD_MODE_LISTEN_ONLY) {
        regval &= ~CANFD_CFG_STAT_LBME;
        regval &= ~CANFD_CFG_STAT_LBMI;
        regval |= CANFD_TCMD_LOM;
    } else if (config->mode == CANFD_MODE_INTERNAL_LOOPBACK) {
        regval &= ~CANFD_CFG_STAT_LBME;
        regval |= CANFD_CFG_STAT_LBMI;
        regval &= ~CANFD_TCMD_LOM;
    } else if (config->mode == CANFD_MODE_EXTERNAL_LOOPBACK) {
        regval |= CANFD_CFG_STAT_LBME;
        regval &= ~CANFD_CFG_STAT_LBMI;
        regval &= ~CANFD_TCMD_LOM;
    }
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);

    /* set receive buffer almost full warning limit to 2 */
    regval = getreg32(reg_base + CANFD_INT_OFFSET);
    regval &= ~CANFD_LIMIT_AFWL_MASK;
    regval |= (2 << CANFD_LIMIT_AFWL_SHIFT);
    putreg32(regval, reg_base + CANFD_INT_OFFSET);

    return 0;
#endif
}

void bflb_canfd_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_canfd_deinit
    romapi_bflb_canfd_deinit(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(CANFD_CFG_STAT_RESET, reg_base + CANFD_CTRL_OFFSET);
#endif
}

void bflb_canfd_write_ptb(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data)
{
#ifdef romapi_bflb_canfd_write_ptb
    romapi_bflb_canfd_write_ptb(dev, head, data);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* select buffer */
    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    regval &= ~CANFD_TCMD_TBSEL;
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
    bflb_canfd_write_message(dev, head, data);
#endif
}

void bflb_canfd_write_stb(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data)
{
#ifdef romapi_bflb_canfd_write_stb
    romapi_bflb_canfd_write_stb(dev, head, data);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* select buffer */
    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    regval |= CANFD_TCMD_TBSEL;
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
    bflb_canfd_write_message(dev, head, data);
    /* mark the slot as filled */
    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    regval |= CANFD_TCTRL_TSNEXT;
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
#endif
}

void bflb_canfd_read_rx_buffer(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data)
{
#ifdef romapi_bflb_canfd_read_rx_buffer
    romapi_bflb_canfd_read_rx_buffer(dev, head, data);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    bflb_canfd_read_message(dev, head, data);
    /* mark the slot as empty */
    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    regval |= CANFD_RCTRL_RREL;
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
#endif
}

void bflb_canfd_exe_tx_cmd(struct bflb_device_s *dev, uint8_t cmd)
{
#ifdef romapi_bflb_canfd_exe_tx_cmd
    romapi_bflb_canfd_exe_tx_cmd(dev, cmd);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    regval |= (1 << (cmd + 8));
    putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
#endif
}

uint8_t bflb_canfd_ptb_is_busy(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_canfd_ptb_is_busy
    return romapi_bflb_canfd_ptb_is_busy(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    if (regval & CANFD_TCMD_TPE) {
        return 1;
    } else {
        return 0;
    }
#endif
}

uint8_t bflb_canfd_get_tx_buffer_state(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_canfd_get_tx_buffer_state
    return romapi_bflb_canfd_get_tx_buffer_state(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    return (regval & CANFD_TCTRL_TSSTAT_MASK) >> CANFD_TCTRL_TSSTAT_SHIFT;
#endif
}

uint8_t bflb_canfd_get_rx_buffer_state(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_canfd_get_rx_buffer_state
    return romapi_bflb_canfd_get_rx_buffer_state(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
    return (regval & CANFD_RCTRL_RSTAT_MASK) >> CANFD_RCTRL_RSTAT_SHIFT;
#endif
}

void bflb_canfd_set_tdc(struct bflb_device_s *dev, uint8_t enable, uint8_t offset)
{
#ifdef romapi_bflb_canfd_set_tdc
    romapi_bflb_canfd_set_tdc(dev, enable, offset);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_ERROR_OFFSET);
    regval &= ~CANFD_TDC_SSPOFF_MASK;
    regval |= (offset << CANFD_TDC_SSPOFF_SHIFT);
    if (enable) {
        regval |= CANFD_TDC_TDCEN;
    } else {
        regval &= ~CANFD_TDC_TDCEN;
    }
    putreg32(regval, reg_base + CANFD_ERROR_OFFSET);
#endif
}

void bflb_canfd_get_error_state(struct bflb_device_s *dev, struct bflb_canfd_error_s *error)
{
#ifdef romapi_bflb_canfd_get_error_state
    romapi_bflb_canfd_get_error_state(dev, error);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_ERROR_OFFSET);
    error->arbitration_lost_position = (regval & CANFD_EALCAP_ALC_MASK) >> CANFD_EALCAP_ALC_SHIFT;
    error->error_code = (regval & CANFD_EALCAP_KOER_MASK) >> CANFD_EALCAP_KOER_SHIFT;
    error->receive_error_count = (regval & CANFD_RECNT_RECNT_MASK) >> CANFD_RECNT_RECNT_SHIFT;
    error->transmit_error_count = (regval & CANFD_TECNT_TECNT_MASK) >> CANFD_TECNT_TECNT_SHIFT;
#endif
}

void bflb_canfd_int_enable(struct bflb_device_s *dev, uint32_t inten)
{
#ifdef romapi_bflb_canfd_int_enable
    romapi_bflb_canfd_int_enable(dev, inten);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t mask;

    reg_base = dev->reg_base;

    inten &= CANFD_INTEN_ALL;
    /* event CAN interrupt */
    mask = inten & CANFD_INT_EVENT_MASK;
    if (mask) {
        regval = getreg32(reg_base + CANFD_INT_OFFSET);
        regval |= mask;
        putreg32(regval, reg_base + CANFD_INT_OFFSET);
    }
    /* time trigger CAN interrupt */
    mask = inten & CANFD_INT_TT_MASK;
    if (mask) {
        regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
        regval |= mask;
        putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);
    }
#endif
}

void bflb_canfd_int_disable(struct bflb_device_s *dev, uint32_t inten)
{
#ifdef romapi_bflb_canfd_int_disable
    romapi_bflb_canfd_int_disable(dev, inten);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t mask;

    reg_base = dev->reg_base;

    inten &= CANFD_INTEN_ALL;
    /* event CAN interrupt */
    mask = inten & CANFD_INT_EVENT_MASK;
    if (mask) {
        regval = getreg32(reg_base + CANFD_INT_OFFSET);
        regval &= ~mask;
        putreg32(regval, reg_base + CANFD_INT_OFFSET);
    }
    /* time trigger CAN interrupt */
    mask = inten & CANFD_INT_TT_MASK;
    if (mask) {
        regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
        regval &= ~mask;
        putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);
    }
#endif
}

uint32_t bflb_canfd_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_canfd_get_intstatus
    return romapi_bflb_canfd_get_intstatus(dev);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t mask_event, mask_tt;

    reg_base = dev->reg_base;
    
    /* event CAN interrupt */
    regval = getreg32(reg_base + CANFD_INT_OFFSET);
    mask_event = regval & (CANFD_INT_EVENT_MASK & CANFD_INTSTS_ALL);
    /* time trigger CAN interrupt */
    regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
    mask_tt = regval & (CANFD_INT_TT_MASK & CANFD_INTSTS_ALL);

    return (mask_event | mask_tt);
#endif
}

void bflb_canfd_int_clear(struct bflb_device_s *dev, uint32_t intclr)
{
#ifdef romapi_bflb_canfd_int_clear
    romapi_bflb_canfd_int_clear(dev, intclr);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t mask;

    reg_base = dev->reg_base;

    intclr &= CANFD_INTCLR_ALL;
    /* event CAN interrupt */
    mask = intclr & CANFD_INT_EVENT_MASK;
    regval = getreg32(reg_base + CANFD_INT_OFFSET);
    regval |= mask;
    putreg32(regval, reg_base + CANFD_INT_OFFSET);
    /* time trigger CAN interrupt */
    mask = intclr & CANFD_INT_TT_MASK;
    if (mask) {
        regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
        regval |= mask;
        putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);
    }
#endif
}

uint64_t bflb_canfd_get_tts(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_canfd_get_tts
    return romapi_bflb_canfd_get_tts(dev);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint64_t time_stamping;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_TTS_OFFSET + 4);
    time_stamping = ((uint64_t)regval) << 32;
    regval = getreg32(reg_base + CANFD_TTS_OFFSET);
    time_stamping |= (uint64_t)regval;
    return time_stamping;
#endif
}

void bflb_canfd_tt_init(struct bflb_device_s *dev, const struct bflb_canfd_tt_config_s *config)
{
#ifdef romapi_bflb_canfd_tt_init
    romapi_bflb_canfd_tt_init(dev, config);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* first disable TTCAN */
    regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
    regval &= ~CANFD_TTCFG_TTEN;
    putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);
    /* config TTCAN prescaler and disable interrupt */
    regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
    regval &= ~CANFD_TTCFG_T_PRESC_MASK;
    regval |= (config->prescaler << CANFD_TTCFG_T_PRESC_SHIFT);
    regval &= ~CANFD_TTCFG_WTIE;
    regval &= ~CANFD_TTCFG_TTIE;
    putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);
    /* disable time stamping */
    regval = getreg32(reg_base + CANFD_TIMECFG_OFFSET);
    regval &= ~CANFD_TIMECFG_TIMEEN;
    putreg32(regval, reg_base + CANFD_TIMECFG_OFFSET);
    /* config time stamping */
    regval = getreg32(reg_base + CANFD_TIMECFG_OFFSET);
    if (config->time_stamping == CANFD_TIME_STAMPING_POS_SOF) {
        regval &= ~CANFD_TIMECFG_TIMEPOS;
        regval |= CANFD_TIMECFG_TIMEEN;
    } else if (config->time_stamping == CANFD_TIME_STAMPING_POS_EOF) {
        regval |= CANFD_TIMECFG_TIMEPOS;
        regval |= CANFD_TIMECFG_TIMEEN;
    }
    putreg32(regval, reg_base + CANFD_TIMECFG_OFFSET);
    /* config watch trigger time */
    regval = getreg32(reg_base + CANFD_TT_WTRIG_OFFSET);
    regval &= ~CANFD_TT_WTRIG_MASK;
    regval |= (config->watch_trigger_time << CANFD_TT_WTRIG_SHIFT);
    putreg32(regval, reg_base + CANFD_TT_WTRIG_OFFSET);
    /* enable TTCAN */
    regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
    regval |= CANFD_TTCFG_TTEN;
    putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);
#endif
}

void bflb_canfd_tt_set_reference_id(struct bflb_device_s *dev, uint32_t id, uint8_t ide_enable)
{
#ifdef romapi_bflb_canfd_tt_set_reference_id
    romapi_bflb_canfd_tt_set_reference_id(dev, id, ide_enable);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
    if (ide_enable) {
        id &= CANFD_ID_EXTENDED_MASK;
        regval |= CANFD_REF_MSG_REF_IDE;
    } else {
        id &= CANFD_ID_STANDARD_MASK;
        regval &= ~CANFD_REF_MSG_REF_IDE;
    }
    regval &= ~CANFD_REF_MSG_REF_ID_MASK;
    regval |= (id << CANFD_REF_MSG_REF_ID_SHIFT);
    putreg32(regval, reg_base + CANFD_REF_MSG_OFFSET);
#endif
}

void bflb_canfd_tt_write_tb(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data, struct bflb_canfd_tt_message_config_s *tt)
{
#ifdef romapi_bflb_canfd_tt_write_tb
    romapi_bflb_canfd_tt_write_tb(dev, head, data);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* select the slot */
    regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
    regval &= ~CANFD_TBSLOT_TBPTR_MASK;
    regval |= (tt->slot << CANFD_TBSLOT_TBPTR_SHIFT);
    putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);
    bflb_canfd_write_message(dev, head, data);
    /* mark the slot as filled */
    regval = getreg32(reg_base + CANFD_TTCFG_OFFSET);
    regval |= CANFD_TBSLOT_TBF;
    putreg32(regval, reg_base + CANFD_TTCFG_OFFSET);

    /* select transmit trigger tb slot pointer */
    regval = getreg32(reg_base + CANFD_TRIG_CFG_OFFSET);
    regval &= ~CANFD_TRIG_CFG_0_TTPTR_MASK;
    regval |= (tt->slot << CANFD_TRIG_CFG_0_TTPTR_SHIFT);
    regval &= ~CANFD_TRIG_CFG_1_TTYPE_MASK;
    regval |= (tt->type << CANFD_TRIG_CFG_1_TTYPE_SHIFT);
    regval &= ~CANFD_TRIG_CFG_1_TEW_MASK;
    regval |= (tt->tew << CANFD_TRIG_CFG_1_TEW_SHIFT);
    regval &= ~CANFD_TT_TRIG_MASK;
    regval |= (tt->trig_time << CANFD_TT_TRIG_SHIFT);
    putreg32(regval, reg_base + CANFD_TRIG_CFG_OFFSET);
#endif
}

int bflb_canfd_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_canfd_feature_control
    return romapi_bflb_canfd_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    switch (cmd) {
        case CANFD_CMD_SET_RESET:
            /* enter or exit reset mode */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_CFG_STAT_RESET;
            } else {
                regval &= ~CANFD_CFG_STAT_RESET;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_PTB_SHOT:
            /* enable or disable transmission primary single shot mode for PTB */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_CFG_STAT_TPSS;
            } else {
                regval &= ~CANFD_CFG_STAT_TPSS;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_STB_SHOT:
            /* enable or disable transmission secondary single shot mode for STB */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_CFG_STAT_TSSS;
            } else {
                regval &= ~CANFD_CFG_STAT_TSSS;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_BUS_OFF:
            /* enable or disable buss off */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_CFG_STAT_BUSOFF;
            } else {
                regval &= ~CANFD_CFG_STAT_BUSOFF;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_TRANSCEIVER_STANDBY_MODE:
            /* enable or disable transceiver standby mode */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_TCMD_STBY;
            } else {
                regval &= ~CANFD_TCMD_STBY;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_STB_PRIORITY_MODE:
            /* enable or disable transmit buffer secondary operation with priority decision mode */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_TCTRL_TSMODE;
            } else {
                regval &= ~CANFD_TCTRL_TSMODE;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_SELF_ACK_EXTERNAL_LOOPBACK:
            /* enable or disable self-acknowledge in loopback mode */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_RCTRL_SACK;
            } else {
                regval &= ~CANFD_RCTRL_SACK;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_RX_OVERFLOW_DROP_NEW:
            /* receive buffer overflow mode */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_RCTRL_ROM;
            } else {
                regval &= ~CANFD_RCTRL_ROM;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_RX_STORE_CORRECT_AND_ERROR_FRAME:
            /* RB stores correct data frames as well as data frames with error */
            regval = getreg32(reg_base + CANFD_CTRL_OFFSET);
            if (arg) {
                regval |= CANFD_RCTRL_RBALL;
            } else {
                regval &= ~CANFD_RCTRL_RBALL;
            }
            putreg32(regval, reg_base + CANFD_CTRL_OFFSET);
            break;

        case CANFD_CMD_SET_RX_THRESHOLD:
            /* receive buffer almost full warning limit */
            regval = getreg32(reg_base + CANFD_INT_OFFSET);
            regval &= ~CANFD_LIMIT_AFWL_MASK;
            regval |= (arg << CANFD_LIMIT_AFWL_SHIFT);
            putreg32(regval, reg_base + CANFD_INT_OFFSET);
            break;

        case CANFD_CMD_GET_RX_THRESHOLD:
            /* get receive buffer almost full warning limit */
            regval = getreg32(reg_base + CANFD_INT_OFFSET);
            regval = (regval & CANFD_LIMIT_AFWL_MASK) >> CANFD_LIMIT_AFWL_SHIFT;
            return regval;

        case CANFD_CMD_SET_ERROR_WARNING_LIMIT:
            /* programmable error warning limit = (EWL+1)*8. possible limit values: 8, 16, … 128. */
            regval = getreg32(reg_base + CANFD_INT_OFFSET);
            regval &= ~CANFD_LIMIT_EWL_MASK;
            regval |= (arg << CANFD_LIMIT_EWL_SHIFT);
            putreg32(regval, reg_base + CANFD_INT_OFFSET);
            break;

        case CANFD_CMD_GET_ERROR_WARNING_LIMIT:
            /* get programmable error warning limit */
            regval = getreg32(reg_base + CANFD_INT_OFFSET);
            regval = (regval & CANFD_LIMIT_EWL_MASK) >> CANFD_LIMIT_EWL_SHIFT;
            return regval;

        case CANFD_CMD_GET_VERSION:
            /* get version */
            regval = getreg32(reg_base + CANFD_VERSION_OFFSET);
            regval = (regval & CANFD_VERSION_MASK) >> CANFD_VERSION_SHIFT;
            return regval;

        case CANFD_CMD_SET_TIME_STAMPING:
            /* set cia603 time stamping, use @ref CANFD_TIME_STAMPING */
            regval = getreg32(reg_base + CANFD_TIMECFG_OFFSET);
            regval &= ~CANFD_TIMECFG_TIMEEN;
            putreg32(regval, reg_base + CANFD_TIMECFG_OFFSET);
            regval = getreg32(reg_base + CANFD_TIMECFG_OFFSET);
            if (arg == CANFD_TIME_STAMPING_POS_SOF) {
                regval &= ~CANFD_TIMECFG_TIMEPOS;
                regval |= CANFD_TIMECFG_TIMEEN;
            } else if (arg == CANFD_TIME_STAMPING_POS_EOF) {
                regval |= CANFD_TIMECFG_TIMEPOS;
                regval |= CANFD_TIMECFG_TIMEEN;
            } else {
                regval &= ~CANFD_TIMECFG_TIMEPOS;
                regval &= ~CANFD_TIMECFG_TIMEEN;
            }
            putreg32(regval, reg_base + CANFD_TIMECFG_OFFSET);
            break;

        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
