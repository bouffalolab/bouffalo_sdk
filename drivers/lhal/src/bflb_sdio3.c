#include "bflb_sdio3.h"
#include "bflb_clock.h"
#include "hardware/sdio3_reg.h"

#ifdef SDIO3_DEBUG
#define SDIO3_DRV_DBG(a, ...) printf("[sdio3 drv]" a, ##__VA_ARGS__)
#else
#define SDIO3_DRV_DBG(a, ...)
#endif

#if (SDIO3_FUNC_QUEUE_NUM_MAX & (SDIO3_FUNC_QUEUE_NUM_MAX - 1) != 0)
#error "sdio3 queue num error, must be 2^n"
#elif (SDIO3_FUNC_QUEUE_NUM_MAX > 16)
#error "sdio3 queue num error, must be <=16"
#endif
#define SDIO3_FUNC_QUEUE_NUM_MAX_MASK  (SDIO3_FUNC_QUEUE_NUM_MAX - 1)

/* cmd 53 def */
#define SDIO3_CMD53_RW_COUNT_SHIFT     (0)
#define SDIO3_CMD53_RW_COUNT_MASK      (0x1ff << SDIO3_CMD53_RW_COUNT_SHIFT)
#define SDIO3_CMD53_RW_ADDR_SHIFT      (9)
#define SDIO3_CMD53_RW_ADDR_MASK       (0x1ffff << SDIO3_CMD53_RW_ADDR_SHIFT)
#define SDIO3_CMD53_RW_OP_CODE_MASK    (1 << 26)
#define SDIO3_CMD53_RW_BLOCK_MODE_MASK (1 << 27)
#define SDIO3_CMD53_RW_FUNC_SHIFT      (28)
#define SDIO3_CMD53_RW_FUNC_MASK       (0x03 << SDIO3_CMD53_RW_FUNC_SHIFT)
#define SDIO3_CMD53_RW_DIR_MASK        (1 << 31)

/* isr */
static void bflb_sdio3_isr(int irq, void *arg);

struct bflb_sdio3_queue_ctrl_s {
    uint8_t tran_sta;
    bflb_sdio3_trans_desc_t *trans_desc;

    uint8_t dnld_queue_out[SDIO3_FUNC_NUM_MAX];
    uint8_t dnld_queue_in[SDIO3_FUNC_NUM_MAX];
    bflb_sdio3_trans_desc_t dnld_queue_desc[SDIO3_FUNC_NUM_MAX][SDIO3_FUNC_QUEUE_NUM_MAX];

    uint8_t upld_queue_out[SDIO3_FUNC_NUM_MAX];
    uint8_t upld_queue_in[SDIO3_FUNC_NUM_MAX];
    bflb_sdio3_trans_desc_t upld_queue_desc[SDIO3_FUNC_NUM_MAX][SDIO3_FUNC_QUEUE_NUM_MAX];
};

/* dnld queue */
#define SDIO3_GET_DNLD_OUT(ctrl, func) (ctrl.dnld_queue_out[func - 1] & SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define SDIO3_GET_DNLD_IN(ctrl, func)  (ctrl.dnld_queue_in[func - 1] & SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define SDIO3_GET_DNLD_NUM(ctrl, func) ((uint8_t)(ctrl.dnld_queue_in[func - 1] - ctrl.dnld_queue_out[func - 1]))
/* dnld get out(pop) desc */
#define SDIO3_GET_DNLD_OUT_DESC(ctrl, func) \
    (&ctrl.dnld_queue_desc[func - 1][SDIO3_GET_DNLD_OUT(ctrl, func)])
/* dnld get in(push) desc */
#define SDIO3_GET_DNLD_IN_DESC(ctrl, func) \
    (&ctrl.dnld_queue_desc[func - 1][SDIO3_GET_DNLD_IN(ctrl, func)])

/* upld queue */
#define SDIO3_GET_UPLD_OUT(ctrl, func) (ctrl.upld_queue_out[func - 1] & SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define SDIO3_GET_UPLD_IN(ctrl, func)  (ctrl.upld_queue_in[func - 1] & SDIO3_FUNC_QUEUE_NUM_MAX_MASK)
#define SDIO3_GET_UPLD_NUM(ctrl, func) ((uint8_t)(ctrl.upld_queue_in[func - 1] - ctrl.upld_queue_out[func - 1]))
/* upld get out(pop) desc */
#define SDIO3_GET_UPLD_OUT_DESC(ctrl, func) \
    (&ctrl.upld_queue_desc[func - 1][SDIO3_GET_UPLD_OUT(ctrl, func)])
/* upld get in(push) desc */
#define SDIO3_GET_UPLD_IN_DESC(ctrl, func) \
    (&ctrl.upld_queue_desc[func - 1][SDIO3_GET_UPLD_IN(ctrl, func)])

/* isr event callback */
static void *sdio3_irq_arg = NULL;
static bflb_sdio3_irq_cb_t sdio3_irq_event_cb = NULL;
/* sdio3 ctrl */
static struct bflb_sdio3_queue_ctrl_s sdio3_ctrl = { 0 };
/* ADMA hw desc, must be nocache */
static ATTR_NOCACHE_RAM_SECTION struct bflb_sdio3_adma_hw_desc_s adma_hw_desc_buff[1];

int bflb_sdio3_init(struct bflb_device_s *dev, struct bflb_sdio3_config_s *cfg)
{
    uint32_t reg_base;
    uint32_t regval;

    if (cfg->func_num == 0 || cfg->func_num > SDIO3_FUNC_NUM_MAX) {
        return -1;
    }

    reg_base = dev->reg_base;

    /* CCCR cfg */
    regval = getreg32(reg_base + SDIO3_SDIO_CCCR_CONTROL_REGISTER_OFFSET);
    /* asyn int */
    if (cfg->cap_flag & SDIO3_CAP_FLAG_SAI) {
        regval |= SDIO3_SAI;
    } else {
        regval &= ~SDIO3_SAI;
    }
    /* ddr50 */
    if (cfg->cap_flag & SDIO3_CAP_FLAG_DDR50) {
        regval |= SDIO3_DDR50;
    } else {
        regval &= ~SDIO3_DDR50;
    }
    /* sdr104 */
    if (cfg->cap_flag & SDIO3_CAP_FLAG_SDR104) {
        regval |= SDIO3_SDR104;
    } else {
        regval &= ~SDIO3_SDR104;
    }
    /* sdr50 */
    if (cfg->cap_flag & SDIO3_CAP_FLAG_SDR50) {
        regval |= SDIO3_SDR50;
    } else {
        regval &= ~SDIO3_SDR50;
    }
    /* block gap interrupt */
    if (cfg->cap_flag & SDIO3_CAP_FLAG_GAP_INT) {
        regval |= SDIO3_S4MI;
    } else {
        regval &= ~SDIO3_S4MI;
    }
    /* Read wait */
    if (cfg->cap_flag & SDIO3_CAP_FLAG_RD_WAIT) {
        regval |= SDIO3_SRW;
    } else {
        regval &= ~SDIO3_SRW;
    }
    /* disable bus suspend/resume */
    regval &= ~SDIO3_SBS;
    /* disable power ctrl */
    regval &= ~SDIO3_SMPC;
    /* disable 8bit */
    regval &= ~SDIO3_S8B;
    /* disable direct cmd52 */
    regval &= ~SDIO3_SDC;
    /* enable multiple block */
    regval |= SDIO3_SMB;
    /* enable sdr25/hs */
    regval |= SDIO3_SHS;
    /*  */
    putreg32(regval, reg_base + SDIO3_SDIO_CCCR_CONTROL_REGISTER_OFFSET);

    /* cmd and data output edge */
    regval = getreg32(reg_base + SDIO3_CONTROL_REGISTER_OFFSET);
    regval &= ~SDIO3_CMD_DATA_OUTPUT_EDGE;
    putreg32(regval, reg_base + SDIO3_CONTROL_REGISTER_OFFSET);

    /* AHB master burst size enable: INCR4, INCR8, INCR16, INCR */
    regval = getreg32(reg_base + SDIO3_AHB_MASTER_BURSTSIZE_REGISTER_OFFSET);
    regval &= ~SDIO3_AHBMAS_BURST_SIZE_ENABLE_MASK;
    regval |= (0x0f << SDIO3_AHBMAS_BURST_SIZE_ENABLE_SHIFT) & SDIO3_AHBMAS_BURST_SIZE_ENABLE_MASK;
    putreg32(regval, reg_base + SDIO3_AHB_MASTER_BURSTSIZE_REGISTER_OFFSET);

    /* ocr */
    regval = getreg32(reg_base + SDIO3_CARD_OCR_REGISTER_OFFSET);
    regval &= ~SDIO3_CARD_OCR_MASK;
    regval |= (cfg->ocr << SDIO3_CARD_OCR_SHIFT) & SDIO3_CARD_OCR_MASK;
    /* sig 1.8v */
    if (cfg->cap_flag & SDIO3_CAP_FLAG_SIG1V8) {
        regval |= SDIO3_SWITCHIN_TO_1P8V;
    } else {
        regval &= ~SDIO3_SWITCHIN_TO_1P8V;
    }
    /* func num */
    if (cfg->func_num == 1) {
        regval &= ~(0x0007 << 28);
        regval |= 0x0001 << 28;
    }
    putreg32(regval, reg_base + SDIO3_CARD_OCR_REGISTER_OFFSET);

    /* func ready */
    regval = getreg32(reg_base + SDIO3_IOREADY_REGISTER_OFFSET);
    for (uint8_t i = 1; i <= cfg->func_num; i++) {
        regval |= 0x01 << i;
    }
    putreg32(regval, reg_base + SDIO3_IOREADY_REGISTER_OFFSET);

#if SDIO3_DMA1_MODE_ENABLE
    /* dma1 mode */
    regval = getreg32(reg_base + SDIO3_CONTROL2_REGISTER_OFFSET);
    regval &= ~SDIO3_ADMA_ENABLE;
    putreg32(regval, reg_base + SDIO3_CONTROL2_REGISTER_OFFSET);

    /* dma1 page size cfg */
    regval = getreg32(reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval &= ~SDIO3_DMA1_BUFFER_SIZE_MASK;
    uint8_t size_bit = __builtin_ctz((SDIO3_DMA1_PAGA_SIZE >> 12));
    regval |= (size_bit << 1);
    putreg32(regval, reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

#else
    /* adma mode */
    regval = getreg32(reg_base + SDIO3_CONTROL2_REGISTER_OFFSET);
    regval |= SDIO3_ADMA_ENABLE;
    putreg32(regval, reg_base + SDIO3_CONTROL2_REGISTER_OFFSET);
#endif

    /* clean all interrupt_status */
    putreg32(0xffffffff, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    putreg32(0x000000ff, reg_base + SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET);

    /* enable interrupt_signal (sta1) */
    regval = SDIO3_TRANSFER_COMPLETE_INTERRUPT | SDIO3_WRITE_START_INTERRUPT | SDIO3_READ_START_INTERRUPT /* transfer int */
             | SDIO3_CMD11_INTERRUPT | SDIO3_CMD11_CLK_START | SDIO3_CMD11_CLK_STOP                       /* vol switch int */
             | SDIO3_CMD0_CMD52_SOFT_RESET | SDIO3_FUNCTION1_RESET | SDIO3_FUNCTION2_RESET                /* host reset int */
             | SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT | SDIO3_FUNCTIONX_ABORT_INTERRUPT;                 /* crc/abort int */
#if SDIO3_DMA1_MODE_ENABLE
    regval |= SDIO3_DMA1_INTERRUPT; /* DMA1 page int */
#endif
    putreg32(regval, reg_base + SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET);
    /* enable interrupt_signal (sta2) */
    regval = SDIO3_ADMA_ERROR_INTERRUPT; /* adma err int */
    putreg32(regval, reg_base + SDIO3_INTERRUPT_SIGNAL_ENABLE2_REGISTER_OFFSET);

    /* write custom reg */
    /* write dnld size max */
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(1) + SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE, &cfg->func1_dnld_size_max, 2);
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(2) + SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE, &cfg->func2_dnld_size_max, 2);
    /* write queue depth */
    uint8_t queue_num = SDIO3_FUNC_QUEUE_NUM_MAX;
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(1) + SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH, &queue_num, 1);
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(2) + SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH, &queue_num, 1);

    for (uint8_t i = 1; i <= SDIO3_FUNC_NUM_MAX; i++) {
        /* write status flag */
        regval = 0;
        bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + SDIO3_CUSTOM_REG_FUNC_STA_FLAG, &regval, 1);
        regval |= SDIO3_STA_FLAG_RD_LEN_COMPRESS_SUP;
#if defined(BOOTROM)
        regval |= SDIO3_STA_FLAG_SDIO_BOOT;
#else
        regval |= SDIO3_STA_FLAG_APP_RUN;
#endif
        bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + SDIO3_CUSTOM_REG_FUNC_STA_FLAG, &regval, 1);

        /* clean upld/dnld in/out ptr */
        regval = 0;
        bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN, &regval, 1);
        bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT, &regval, 1);
        bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN, &regval, 1);
        bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(i) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT, &regval, 1);
    }

    bflb_irq_attach(dev->irq_num, bflb_sdio3_isr, dev);
    bflb_irq_enable(dev->irq_num);

    return 0;
}

int bflb_sdio3_deinit(struct bflb_device_s *dev)
{
    bflb_irq_disable(dev->irq_num);

#if 0
#if defined(BL628)
    /* hw reset */
    extern void bflb_glb_ahb_mcu_software_reset(uint8_t swrst);
    bflb_glb_ahb_mcu_software_reset(45); /* GLB_AHB_MCU_SW_SDU */
#endif
#endif

    /* reset sdio3_ctrl */
    arch_memset((void *)&sdio3_ctrl, 0, sizeof(sdio3_ctrl));

    /* clear custom reg */
    uint8_t buf[8] = { 0 };
    for (uint32_t i = 0; i < SDIO3_CUSTOM_REG_SIZE; i += sizeof(buf)) {
        bflb_sdio3_custom_reg_write(dev, i, buf, sizeof(buf));
    }

    return 0;
}

#if SDIO3_DMA1_MODE_ENABLE
/* dma1 init */
static int bflb_sdio3_dma1_start(struct bflb_device_s *dev, uint32_t addr, uint16_t len)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* address */
    putreg32(addr, reg_base + SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);

    /* start */
    regval = getreg32(reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval |= SDIO3_DMA1_ADDRESS_VALID;
    putreg32(regval, reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

    return 0;
}
#else
/* adma desc init */
static int bflb_sdio3_adma_start(struct bflb_device_s *dev, struct bflb_sdio3_adma_hw_desc_s *hw_desc, uint32_t addr, uint16_t len)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* adma hw desc init */
    hw_desc->address = addr;
    hw_desc->length = len;
    hw_desc->attribute = SDIO2_ADMA_ATTR_VALID | SDIO2_ADMA_ATTR_END | SDIO2_ADMA_ATTR_ACT_TRAN;

    /* adma desc addr reg */
    regval = (uint32_t)(uintptr_t)hw_desc;
    putreg32(regval, reg_base + SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);

    /* adma start */
    regval = getreg32(reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval |= SDIO3_DMA1_ADDRESS_VALID;
    putreg32(regval, reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

    return 0;
}
#endif

static int bflb_sdio3_adma_stop(struct bflb_device_s *dev)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* adma stop */
    regval = getreg32(reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    regval &= ~SDIO3_DMA1_ADDRESS_VALID;
    putreg32(regval, reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);

    return 0;
}

int bflb_sdio3_custom_reg_write(struct bflb_device_s *dev, uint16_t reg_offset, void *buff, uint16_t len)
{
    uint32_t reg_base = dev->reg_base;

    if (reg_offset + len > SDIO3_CUSTOM_REG_SIZE) {
        return -1;
    }

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* Write card address register */
    putreg32(reg_offset, reg_base + SDIO3_CUSTOM_ADDRESS_REGISTER_OFFSET);
    /* Write card data register */
    for (uint16_t i = 0; i < len; i++) {
        putreg8(((uint8_t *)buff)[i], reg_base + SDIO3_CUSTOM_DATA_REGISTER_OFFSET);
    }

    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_sdio3_custom_reg_read(struct bflb_device_s *dev, uint16_t reg_offset, void *buff, uint16_t len)
{
    uint32_t reg_base = dev->reg_base;

    if (reg_offset + len > SDIO3_CUSTOM_REG_SIZE) {
        return -1;
    }

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* Write card address register */
    putreg32(reg_offset, reg_base + SDIO3_CUSTOM_ADDRESS_REGISTER_OFFSET);
    /* Read card data register */
    for (uint16_t i = 0; i < len; i++) {
        ((uint8_t *)buff)[i] = getreg8(reg_base + SDIO3_CUSTOM_DATA_REGISTER_OFFSET);
    }
    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_sdio3_dnld_push(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc)
{
    if (trans_desc == NULL || trans_desc->func == 0 || trans_desc->func > SDIO3_FUNC_NUM_MAX) {
        return -1;
    }

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* check queue */
    if (SDIO3_GET_DNLD_NUM(sdio3_ctrl, trans_desc->func) >= SDIO3_FUNC_QUEUE_NUM_MAX) {
        bflb_irq_restore(flag);
        return -2;
    }

    /* update dnld queue */
    uint8_t dnld_in = sdio3_ctrl.dnld_queue_in[trans_desc->func - 1];
    bflb_sdio3_trans_desc_t *dnld_desc = SDIO3_GET_DNLD_IN_DESC(sdio3_ctrl, trans_desc->func);
    *dnld_desc = *trans_desc;
    sdio3_ctrl.dnld_queue_in[trans_desc->func - 1] += 1;

    /* clean dnld length */
    uint16_t dnld_elem = 0;
    uint8_t dnld_in_mask = dnld_in & SDIO3_FUNC_QUEUE_NUM_MAX_MASK; /* SDIO3_GET_DNLD_IN(sdio3_ctrl, trans_desc->func) */
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(dnld_desc->func) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE + dnld_in_mask * 2, &dnld_elem, 2);

    /* update queue in point */
    dnld_in = sdio3_ctrl.dnld_queue_in[dnld_desc->func - 1];
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(dnld_desc->func) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN, &dnld_in, 1);

    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_sdio3_upld_push(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc)
{
    if (trans_desc == NULL || trans_desc->func == 0 || trans_desc->func > SDIO3_FUNC_NUM_MAX) {
        return -1;
    }

    /* update queue elem (length)  */
    uint16_t blk_size, upld_elem;
    bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(trans_desc->func) + SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE, &blk_size, 2);
    if (!blk_size || trans_desc->data_len % blk_size) {
        /* byte mode */
        if (trans_desc->data_len < 64) {
            upld_elem = (trans_desc->data_len << 2) | 0x00; /* 1-byte, byte mode */
        } else {
            upld_elem = (trans_desc->data_len << 2) | 0x01; /* 2-byte, byte mode */
        }
    } else {
        /* block mode */
        if (trans_desc->data_len / blk_size < 64) {
            upld_elem = ((trans_desc->data_len / blk_size) << 2) | 0x02; /* 1-byte, block mode */
        } else {
            upld_elem = ((trans_desc->data_len / blk_size) << 2) | 0x03; /* 2-byte, block mode */
        }
    }

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* check queue */
    if (SDIO3_GET_UPLD_NUM(sdio3_ctrl, trans_desc->func) >= SDIO3_FUNC_QUEUE_NUM_MAX) {
        bflb_irq_restore(flag);
        return -2;
    }

    /* update upld queue */
    uint8_t upld_in = sdio3_ctrl.upld_queue_in[trans_desc->func - 1];
    bflb_sdio3_trans_desc_t *upld_desc = SDIO3_GET_UPLD_IN_DESC(sdio3_ctrl, trans_desc->func);
    *upld_desc = *trans_desc;
    sdio3_ctrl.upld_queue_in[upld_desc->func - 1] += 1;

    uint8_t upld_in_mask = upld_in & SDIO3_FUNC_QUEUE_NUM_MAX_MASK; /* SDIO3_GET_UPLD_IN(sdio3_ctrl, upld_desc->func) */
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(upld_desc->func) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE + upld_in_mask * 2, &upld_elem, 2);

    /* update queue in point */
    upld_in = sdio3_ctrl.upld_queue_in[upld_desc->func - 1];
    bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(upld_desc->func) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN, &upld_in, 1);

    /* unlock */
    bflb_irq_restore(flag);

    /* TODO: write read_len reg (card2host int) */
    if (upld_desc->func == 1) {
        putreg16(1, dev->reg_base + SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
    } else if (upld_desc->func == 2) {
        putreg16(1, dev->reg_base + SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
    }

    return 0;
}

/* pop dnld queue, can only be used after reset. */
int bflb_sdio3_dnld_pop(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc, uint8_t func)
{
    arch_memset(trans_desc, 0, sizeof(bflb_sdio3_trans_desc_t));

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* not IDLE */
    if (sdio3_ctrl.tran_sta != SDIO3_TRAN_STA_IDLE) {
        bflb_irq_restore(flag);
        return -1;
    }

    /* check queue */
    if (SDIO3_GET_DNLD_NUM(sdio3_ctrl, func) == 0) {
        bflb_irq_restore(flag);
        return 1;
    }

    *trans_desc = *(SDIO3_GET_DNLD_OUT_DESC(sdio3_ctrl, func));
    sdio3_ctrl.dnld_queue_out[func - 1] += 1;

    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

/* pop upld queue, can only be used after reset. */
int bflb_sdio3_upld_pop(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc, uint8_t func)
{
    arch_memset(trans_desc, 0, sizeof(bflb_sdio3_trans_desc_t));

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* not IDLE */
    if (sdio3_ctrl.tran_sta != SDIO3_TRAN_STA_IDLE) {
        bflb_irq_restore(flag);
        return -1;
    }

    /* check queue */
    if (SDIO3_GET_UPLD_NUM(sdio3_ctrl, func) == 0) {
        bflb_irq_restore(flag);
        return 1;
    }

    *trans_desc = *(SDIO3_GET_UPLD_OUT_DESC(sdio3_ctrl, func));
    sdio3_ctrl.upld_queue_out[func - 1] += 1;

    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_sdio3_feature_control(struct bflb_device_s *dev, int cmd, uintptr_t arg)
{
    int ret = 0;
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;
    uintptr_t flag;

    switch (cmd) {
        case SDIO3_CMD_INIT_READY:
            /* queue ctrl ready */
            if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_IDLE) {
                sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;
            }
            /* card ready */
            regval = getreg32(reg_base + SDIO3_CONTROL_REGISTER_OFFSET);
            regval |= SDIO3_CARD_INIT_DONE;
            putreg32(regval, reg_base + SDIO3_CONTROL_REGISTER_OFFSET);
            break;

        case SDIO3_CMD_GET_CARD_STA:
            /* get card status (Resp_1) */
            regval = getreg32(reg_base + SDIO3_COMMAND_REGISTER_OFFSET);
            regval = (regval & SDIO3_BLOCK_SIZE_MASK) >> SDIO3_BLOCK_SIZE_SHIFT;
            ret = regval;
            break;

        case SDIO3_CMD_GET_BUS_WIDTH:
            /* get bus width */
            regval = getreg32(reg_base + SDIO3_COMMAND_REGISTER_OFFSET);
            regval = (regval & SDIO3_CURRENT_BUS_WIDTH_MASK) >> SDIO3_CURRENT_BUS_WIDTH_SHIFT;
            if (regval == 0) {
                ret = 1;
            } else if (regval == 1) {
                ret = 4;
            } else if (regval == 2) {
                ret = 8;
            } else {
                ret = -1;
            }
            break;

        case SDIO3_CMD_SET_FUNC_CARD_READY:
            /* get func1/func2 ready sta, arg: func num */
            regval = 1;
            bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + SDIO3_CUSTOM_REG_FUNC_CARD_READY, &regval, 1);
            ret = (uint8_t)regval;
            break;

        case SDIO3_CMD_GET_FUNC_HOST_READY:
            /* get func1/func2 ready sta, arg: func num */
            regval = 0;
            bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + SDIO3_CUSTOM_REG_FUNC_HOST_READY, &regval, 1);
            ret = (uint8_t)regval;
            break;

        case SDIO3_CMD_GET_FUNC_BLK_SIZE:
            /* get func1/func2 blk size, arg: func num */
            regval = 0;
            bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE, &regval, 2);
            ret = (uint16_t)regval;
            break;

        case SDIO3_CMD_GET_DNLD_MAX_SIZE:
            /* get func1/func2 dnld max size, arg: func num */
            regval = 0;
            bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE, &regval, 2);
            ret = (uint16_t)regval;
            break;

        case SDIO3_CMD_GET_UPLD_MAX_SIZE:
            /* get func1/func2 upld max size, arg: func num */
            regval = 0;
            bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(arg) + SDIO3_CUSTOM_REG_FUNC_UPLD_MAX_SIZE, &regval, 2);
            ret = (uint16_t)regval;
            break;

        case SDIO3_CMD_GET_DNLD_QUEUE_WAIT_NUM:
            flag = bflb_irq_save();
            ret = SDIO3_GET_DNLD_NUM(sdio3_ctrl, arg);
            bflb_irq_restore(flag);
            break;

        case SDIO3_CMD_GET_DNLD_QUEUE_AVAILABLE:
            flag = bflb_irq_save();
            ret = SDIO3_FUNC_QUEUE_NUM_MAX - SDIO3_GET_DNLD_NUM(sdio3_ctrl, arg);
            bflb_irq_restore(flag);
            break;

        case SDIO3_CMD_GET_UPLD_QUEUE_WAIT_NUM:
            flag = bflb_irq_save();
            ret = SDIO3_GET_UPLD_NUM(sdio3_ctrl, arg);
            bflb_irq_restore(flag);
            break;

        case SDIO3_CMD_GET_UPLD_QUEUE_AVAILABLE:
            flag = bflb_irq_save();
            ret = SDIO3_FUNC_QUEUE_NUM_MAX - SDIO3_GET_UPLD_NUM(sdio3_ctrl, arg);
            bflb_irq_restore(flag);
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

int bflb_sdio3_irq_attach(struct bflb_device_s *dev, bflb_sdio3_irq_cb_t irq_event_cb, void *arg)
{
    sdio3_irq_event_cb = irq_event_cb;
    sdio3_irq_arg = arg;

    return 0;
}

/* isr callback: cmd0/cmd52 soft reset and func_x reset  */
static void bflb_sdio3_isr_cb_soft_reset(struct bflb_device_s *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* FIXME: after a soft reset, CMD5 triggers SDIO3_PROGRAM_START, disable it */
    if (int_sta1 & SDIO3_PROGRAM_START) {
        /* clr int */
        putreg32(SDIO3_PROGRAM_START, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        /* disable int signal */
        int_sta1 = getreg32(reg_base + SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET);
        int_sta1 &= ~SDIO3_PROGRAM_START;
        putreg32(int_sta1, reg_base + SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET);
        return;
    }

    /* 1. cmd0/cmd52 soft reset */
    if (int_sta1 & SDIO3_CMD0_CMD52_SOFT_RESET) {
        /* clr int */
        putreg32(SDIO3_CMD0_CMD52_SOFT_RESET, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        SDIO3_DRV_DBG("sdio3 soft reset\r\n");

        /*TODO: reset restore */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_IDLE;

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_SOFT_RESET, NULL);
        }
        return;
    }

    /* 2. func1 reset */
    if (int_sta1 & SDIO3_FUNCTION1_RESET) {
        /* clr int */
        putreg32(SDIO3_FUNCTION1_RESET, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        int_sta1 = getreg32(reg_base + SDIO3_IOREADY_REGISTER_OFFSET);
        SDIO3_DRV_DBG("sdio3 func1 reset\r\n");

        /*TODO: func1 reset restore */

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_FUNC1_RESET, NULL);
        }
    }

    /* 3. func2 reset */
    if (int_sta1 & SDIO3_FUNCTION2_RESET) {
        /* clr int */
        putreg32(SDIO3_FUNCTION2_RESET, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        int_sta1 = getreg32(reg_base + SDIO3_IOREADY_REGISTER_OFFSET);
        SDIO3_DRV_DBG("sdio3 func2 reset\r\n");

        /*TODO: func2 reset restore */

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_FUNC2_RESET, NULL);
        }
    }
}

/* isr callback: func1/func2 dnld/upld transfer start int */
static void bflb_sdio3_isr_cb_tran_start(struct bflb_device_s *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    uint8_t func_n;
    uint16_t blk_cnt, blk_size;
    uint32_t cmd53_arg;

    cmd53_arg = getreg32(reg_base + SDIO3_ARGUMENT_REGISTER_OFFSET);
    func_n = (cmd53_arg & SDIO3_CMD53_RW_FUNC_MASK) >> SDIO3_CMD53_RW_FUNC_SHIFT;
    blk_cnt = (cmd53_arg & SDIO3_CMD53_RW_COUNT_MASK) >> SDIO3_CMD53_RW_COUNT_SHIFT;

    if (cmd53_arg & SDIO3_CMD53_RW_BLOCK_MODE_MASK) {
        /* TODO: funcx block size? */
        regval = getreg32(reg_base + SDIO3_COMMAND_REGISTER_OFFSET);
        blk_size = (regval & SDIO3_BLOCK_SIZE_MASK) >> SDIO3_BLOCK_SIZE_SHIFT;
    } else {
        blk_size = 1;
        if (blk_cnt == 0) {
            blk_cnt = 512;
        }
    }
    SDIO3_DRV_DBG("cmd53 trans, arg:0x%08X, size:%d*%d\r\n", cmd53_arg, blk_size, blk_cnt);

    if (int_sta1 & SDIO3_WRITE_START_INTERRUPT) {
        /* dnld opt */
        putreg32(SDIO3_WRITE_START_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        /* ckeck queue */
        if (SDIO3_GET_DNLD_NUM(sdio3_ctrl, func_n) == 0) {
            if (sdio3_irq_event_cb != NULL) {
                sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_DNLD_QUEUE, NULL);
            }
            return;
        }
        /* update transfer status */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_BUSY_DNLD;
        sdio3_ctrl.trans_desc = SDIO3_GET_DNLD_OUT_DESC(sdio3_ctrl, func_n);

        /* read host dnld len */
        uint16_t host_dnld_len;
        uint8_t dnld_queue_out = SDIO3_GET_DNLD_OUT(sdio3_ctrl, func_n);
        bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(func_n) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE + dnld_queue_out * 2, &host_dnld_len, 2);
        SDIO3_DRV_DBG("dnld len raw: 0x%04X\r\n", host_dnld_len);
        if (host_dnld_len == 0) {
            sdio3_ctrl.trans_desc->data_len = blk_size * blk_cnt;
        } else {
            if ((host_dnld_len & 0x01) == 0) {
                /* 1-byte  */
                host_dnld_len &= 0x00ff;
            }
            if (host_dnld_len & 0x02) {
                /* block mode */
                uint16_t host_blk_size;
                bflb_sdio3_custom_reg_read(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(func_n) + SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE, &host_blk_size, 2);
                sdio3_ctrl.trans_desc->data_len = (host_dnld_len >> 2) * host_blk_size;
            } else {
                /* byte mode */
                sdio3_ctrl.trans_desc->data_len = (host_dnld_len >> 2);
            }
        }

    } else if (int_sta1 & SDIO3_READ_START_INTERRUPT) {
        /* upld opt */
        putreg32(SDIO3_READ_START_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        /* ckeck queue */
        if (SDIO3_GET_UPLD_NUM(sdio3_ctrl, func_n) == 0) {
            if (sdio3_irq_event_cb != NULL) {
                sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_UPLD_QUEUE, NULL);
            }
            return;
        }
        /* update transfer status */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_BUSY_UPLD;
        sdio3_ctrl.trans_desc = SDIO3_GET_UPLD_OUT_DESC(sdio3_ctrl, func_n);
        /* clear upld length */
        if (func_n == 1) {
            putreg32(0, reg_base + SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
        } else if (func_n == 2) {
            putreg32(0, reg_base + SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
        }
    }

#if SDIO3_DMA1_MODE_ENABLE
    /* dma1 init */
    SDIO3_DRV_DBG("dma1 start %p\r\n", sdio3_ctrl.trans_desc->buff);
    bflb_sdio3_dma1_start(dev, (uint32_t)(uintptr_t)sdio3_ctrl.trans_desc->buff, blk_size * blk_cnt);
#else
    /* adma init */
    SDIO3_DRV_DBG("adma start %p\r\n", sdio3_ctrl.trans_desc->buff);
    bflb_sdio3_adma_start(dev, adma_hw_desc_buff, (uint32_t)(uintptr_t)sdio3_ctrl.trans_desc->buff, blk_size * blk_cnt);
#endif
}

#if SDIO3_DMA1_MODE_ENABLE
/* isr callback: dma1 page int */
static void bflb_sdio3_isr_cb_dma1_page(struct bflb_device_s *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* clr int */
    putreg32(SDIO3_DMA1_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);

    if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_DNLD || sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_UPLD) {
        bflb_sdio3_trans_desc_t *trans_desc = sdio3_ctrl.trans_desc;
        regval = getreg32(reg_base + SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);
        if (regval % SDIO3_DMA1_PAGA_SIZE) {
            regval += SDIO3_DMA1_PAGA_SIZE - ((uint32_t)(uintptr_t)trans_desc->buff % SDIO3_DMA1_PAGA_SIZE);
        } else {
            regval += SDIO3_DMA1_PAGA_SIZE;
        }

        /* address */
        putreg32(regval, reg_base + SDIO3_DMA1_ADDRESS_REGISTER_OFFSET);
        SDIO3_DRV_DBG("dma1 page addr: 0x%08X\r\n", regval);

        /* start */
        regval = getreg32(reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
        regval |= SDIO3_DMA1_ADDRESS_VALID;
        putreg32(regval, reg_base + SDIO3_DMA1_CONTROL_REGISTER_OFFSET);
    } else {
        SDIO3_DRV_DBG("dma1 err\r\n");
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_UNKNOWN, NULL);
        }
    }
}
#endif

/* isr callback: func1/func2  dnld/upld transfer complete int */
static void bflb_sdio3_isr_cb_tran_complete(struct bflb_device_s *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* clr int */
    putreg32(SDIO3_TRANSFER_COMPLETE_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    SDIO3_DRV_DBG("trans complete\r\n");

#if SDIO3_DMA1_MODE_ENABLE
    putreg32(SDIO3_DMA1_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
#endif

    if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_DNLD) {
        /* dnld complete, update dnld queue */
        sdio3_ctrl.dnld_queue_out[sdio3_ctrl.trans_desc->func - 1] += 1;
        bflb_sdio3_trans_desc_t comp_desc = *sdio3_ctrl.trans_desc;
        /* update custom reg */
        uint8_t func = comp_desc.func;
        uint8_t dnld_out = sdio3_ctrl.dnld_queue_out[func - 1];
        bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(func) + SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT, &dnld_out, 1);
        /* update transfer status */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* program done (release DAT0) */
        regval = getreg32(reg_base + SDIO3_CONTROL_REGISTER_OFFSET);
        regval |= SDIO3_PROGRAM_DONE;
        putreg32(regval, reg_base + SDIO3_CONTROL_REGISTER_OFFSET);

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_DNLD_CPL, &comp_desc);
        }

    } else if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_UPLD) {
        /* upld complete, update upld queue */
        sdio3_ctrl.upld_queue_out[sdio3_ctrl.trans_desc->func - 1] += 1;
        bflb_sdio3_trans_desc_t comp_desc = *sdio3_ctrl.trans_desc;
        /* update custom reg */
        uint8_t func = comp_desc.func;
        uint8_t upld_out = sdio3_ctrl.upld_queue_out[func - 1];
        bflb_sdio3_custom_reg_write(dev, SDIO3_CUSTOM_REG_FUNC_OFFSET(func) + SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT, &upld_out, 1);
        /* update transfer status */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* TODO: write read_len reg (card2host int) */
        if (comp_desc.func == 1 && SDIO3_GET_UPLD_NUM(sdio3_ctrl, 1)) {
            putreg16(1, dev->reg_base + SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
        }
#if (SDIO3_FUNC_NUM_MAX >= 2)
        else if (comp_desc.func == 2 && SDIO3_GET_UPLD_NUM(sdio3_ctrl, 2)) {
            putreg16(1, dev->reg_base + SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
        }
#endif
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_UPLD_CPL, &comp_desc);
        }
    } else {
        SDIO3_DRV_DBG("complete err\r\n");
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_UNKNOWN, NULL);
        }
    }
}

/* isr callback: func1/func2  dnld/upld transfer abort int */
static void bflb_sdio3_isr_cb_tran_abort(struct bflb_device_s *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* clr int */
    putreg32(SDIO3_FUNCTIONX_ABORT_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    SDIO3_DRV_DBG("trans abort\r\n");

    if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_DNLD) {
        /* dnld abort */
        bflb_sdio3_adma_stop(dev);
        bflb_sdio3_trans_desc_t abort_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status, and clean data_len */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc->data_len = 0;
        sdio3_ctrl.trans_desc = NULL;

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_DNLD_ABORT, &abort_desc);
        }

    } else if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_UPLD) {
        /* upld abort */
        bflb_sdio3_adma_stop(dev);
        bflb_sdio3_trans_desc_t abort_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* TODO: write read_len reg (card2host int) */
        if (abort_desc.func == 1 && SDIO3_GET_UPLD_NUM(sdio3_ctrl, 1)) {
            putreg16(1, dev->reg_base + SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
        }
#if (SDIO3_FUNC_NUM_MAX >= 2)
        else if (abort_desc.func == 2 && SDIO3_GET_UPLD_NUM(sdio3_ctrl, 2)) {
            putreg16(1, dev->reg_base + SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
        }
#endif
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_UPLD_ABORT, &abort_desc);
        }
    }
#if 0
        /* TODO: this interrupt occurs during initialization */
        else {
            SDIO3_DRV_DBG("abort err\r\n");
            if (sdio3_irq_event_cb != NULL) {
                sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_UNKNOWN, NULL);
            }
        }
#endif
}

/* isr callback: func1/func2  dnld/upld transfer abort int */
static void bflb_sdio3_isr_cb_crc_err(struct bflb_device_s *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* clr int */
    putreg32(SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    SDIO3_DRV_DBG("crc error\r\n");

    if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_DNLD) {
        /* dnld crc error */
        bflb_sdio3_adma_stop(dev);
        bflb_sdio3_trans_desc_t crc_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status, and clean data_len */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc->data_len = 0;

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_CRC, &crc_desc);
        }
        return;
    }
#if 1
    /* callback */
    if (sdio3_irq_event_cb != NULL) {
        sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_UNKNOWN, NULL);
    }
#else
    else if (sdio3_ctrl.tran_sta == SDIO3_TRAN_STA_BUSY_UPLD) {

        /* upld abort */
        bflb_sdio3_trans_desc_t abort_desc = *sdio3_ctrl.trans_desc;
        /* update transfer status */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;
        sdio3_ctrl.trans_desc = NULL;

        /* TODO: write read_len reg (card2host int) */
        if (abort_desc.func == 1 && SDIO3_GET_UPLD_NUM(sdio3_ctrl, 1)) {
            putreg16(1, dev->reg_base + SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET);
        } else if (abort_desc.func == 2 && SDIO3_GET_UPLD_NUM(sdio3_ctrl, 2)) {
            putreg16(1, dev->reg_base + SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET);
        }

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_CRC, &abort_desc);
        }
    }
#endif
}

/* isr callback: voltage switch to 1.8v int (cmd11) */
static void bflb_sdio3_isr_cb_vol_switch(struct bflb_device_s *dev, uint32_t int_sta1)
{
    uint32_t reg_base = dev->reg_base;

    /* voltage switch to 1.8v int (cmd11) */
    if (int_sta1 & SDIO3_CMD11_INTERRUPT) {
        /* clr int */
        putreg32(SDIO3_CMD11_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        SDIO3_DRV_DBG("cmd11 come\r\n");
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_CMD11_SWITCH, NULL);
        }
        return;
    }
    /* clock stop after cmd11, disable output */
    if (int_sta1 & SDIO3_CMD11_CLK_STOP) {
        /* clr int */
        putreg32(SDIO3_CMD11_CLK_STOP, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        SDIO3_DRV_DBG("cmd11 clk stop\r\n");
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_CMD11_CLK_STOP, NULL);
        }
        return;
    }
    /* clock start after cmd11, enable output */
    if (int_sta1 & SDIO3_CMD11_CLK_START) {
        /* clr int */
        putreg32(SDIO3_CMD11_CLK_START, reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
        SDIO3_DRV_DBG("cmd11 clk start\r\n");
        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_CMD11_CLK_START, NULL);
        }
        return;
    }
}

static void bflb_sdio3_isr(int irq, void *arg)
{
    struct bflb_device_s *dev = (struct bflb_device_s *)arg;
    uint32_t reg_base = dev->reg_base;
    uint32_t int_sta, int_umask;

    int_umask = getreg32(reg_base + SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET);
    int_sta = getreg32(reg_base + SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET);
    SDIO3_DRV_DBG("isr sta 0x%08X\r\n", int_sta);

    int_sta &= int_umask; /* mask interrupt */

    /* 1. cmd0/cmd52 soft reset or func_x reset  */
    /* TODO: SDIO3_PROGRAM_START,it will trigger when the card is recognized again. */
    if (int_sta & (SDIO3_CMD0_CMD52_SOFT_RESET | SDIO3_FUNCTION1_RESET | SDIO3_FUNCTION2_RESET | SDIO3_PROGRAM_START)) {
        bflb_sdio3_isr_cb_soft_reset(dev, int_sta);
        return;
    }

    /* 2. func1/func2  dnld/upld start int */
    if ((int_sta & SDIO3_WRITE_START_INTERRUPT) || (int_sta & SDIO3_READ_START_INTERRUPT)) {
        bflb_sdio3_isr_cb_tran_start(dev, int_sta);
        return;
    }

    /* 3. func1/func2  dnld/upld complete int */
    if (int_sta & SDIO3_TRANSFER_COMPLETE_INTERRUPT) {
        bflb_sdio3_isr_cb_tran_complete(dev, int_sta);
        return;
    }

#if SDIO3_DMA1_MODE_ENABLE
    /* dma1 page int */
    if (int_sta & SDIO3_DMA1_INTERRUPT) {
        bflb_sdio3_isr_cb_dma1_page(dev, int_sta);
        return;
    }
#endif

    /* 4. func1/func2  dnld/upld abort int */
    if (int_sta & SDIO3_FUNCTIONX_ABORT_INTERRUPT) {
        bflb_sdio3_isr_cb_tran_abort(dev, int_sta);
        return;
    }

    /* 5. voltage switch to 1.8v int (cmd11) */
    if (int_sta & (SDIO3_CMD11_INTERRUPT | SDIO3_CMD11_CLK_STOP | SDIO3_CMD11_CLK_START)) {
        bflb_sdio3_isr_cb_vol_switch(dev, int_sta);
        return;
    }

    /* 6. CRC error int */
    if (int_sta & SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT) {
        bflb_sdio3_isr_cb_crc_err(dev, int_sta);
        return;
    }

    /* 7. int sta2, adma error int */
    int_sta = getreg32(reg_base + SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET);
    if (int_sta & SDIO3_ADMA_ERROR_INTERRUPT) {
        /* clr int */
        putreg32(SDIO3_ADMA_ERROR_INTERRUPT, reg_base + SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET);
        SDIO3_DRV_DBG("adma err\r\n");

        /* TODO: error restore */
        sdio3_ctrl.tran_sta = SDIO3_TRAN_STA_READY;

        /* callback */
        if (sdio3_irq_event_cb != NULL) {
            sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_ADMA, NULL);
        }

        return;
    }

    /* 4. unknown int, callback */
    if (sdio3_irq_event_cb != NULL) {
        SDIO3_DRV_DBG("sdio3 unknown int\r\n");
        sdio3_irq_event_cb(sdio3_irq_arg, SDIO3_IRQ_EVENT_ERR_UNKNOWN, NULL);
    }
}
