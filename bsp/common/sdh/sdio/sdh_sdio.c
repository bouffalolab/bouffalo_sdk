#include <string.h>

#include "bflb_l1c.h"
#include "sdh_spec.h"
#include "sdh_host.h"
#include "sdh_osal.h"
#include "sdh_sdio.h"

/* log */
#define DBG_TAG "SDH_SDIO"
#ifdef CONFIG_BSP_SDH_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_BSP_SDH_LOG_LEVEL
#endif
#include "log.h"

#define SDIO_MAX_FUNC_NUM          (7U)
#define SDIO_CIS_MAX_TUPLE_SIZE    (255U)
#define SDIO_CIS_SCAN_LIMIT        (2048U)
#define SDIO_CIS_FUNCE_COMMON_SIZE (4U)
#define SDIO_CIS_FUNCE_FUNC_1_00   (28U)
#define SDIO_CIS_FUNCE_FUNC_1_10   (42U)
#define SDIO_CCCR_REV_1_00         (0U)
#define SDIO_CCCR_REV_1_10         (1U)
#define SDIO_CCCR_REV_1_20         (2U)
#define SDIO_CCCR_REV_3_00         (3U)
#define SDIO_SDIO_REV_1_00         (0U)
#define SDIO_SDIO_REV_1_10         (1U)

static int sdh_sdio_check_func(const struct sdh_sdio_func_s *func);
static int sdh_sdio_cmd52(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t rw, uint8_t *data);
static int sdh_sdio_cmd52_resp(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg,
                               uint8_t rw, uint8_t *data, uint8_t resp_type);
static int sdh_sdio_cmd53_read(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len);
static int sdh_sdio_cmd53_write(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len);

static const uint32_t sdh_sdio_tran_exp[] = { 10000U, 100000U, 1000000U, 10000000U, 0U, 0U, 0U, 0U };
static const uint8_t sdh_sdio_tran_mant[] = { 0U, 10U, 12U, 13U, 15U, 20U, 25U, 30U, 35U, 40U, 45U, 50U, 55U, 60U, 70U, 80U };

#define SDH_SDIO_CMD53_BOUNCE_BUF_SIZE (SDIO_TRANSFER_MAX)

/* Use an uncached bounce buffer when user buffers are not cache-line aligned. */
static __ALIGNED(BFLB_CACHE_LINE_SIZE) ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t g_sdh_sdio_bounce_buf[SDH_SDIO_CMD53_BOUNCE_BUF_SIZE];

static inline bool sdh_sdio_addr_need_bounce(const void *addr, uint32_t len)
{
    if (!bflb_check_cache_addr(addr)) {
        return false;
    }
    return ((((uintptr_t)addr) & (BFLB_CACHE_LINE_SIZE - 1U)) != 0U) ||
           ((len & (BFLB_CACHE_LINE_SIZE - 1U)) != 0U);
}

static void sdh_sdio_cmd53_calc_chunk(struct sdh_sdio_s *sdio,
                                      uint8_t func,
                                      uint32_t remaining,
                                      uint32_t max_chunk_len,
                                      bool *block_mode,
                                      uint16_t *block_size,
                                      uint16_t *block_count,
                                      uint16_t *arg_length,
                                      uint32_t *chunk_len)
{
    uint16_t curr_block_size = sdio->functions[func].cur_blksize;
    uint32_t limit = remaining;

    if ((max_chunk_len != 0U) && (limit > max_chunk_len)) {
        limit = max_chunk_len;
    }

    if ((func != 0U) && (curr_block_size != 0U) && ((remaining % curr_block_size) == 0U) && (limit >= curr_block_size)) {
        uint32_t blocks = remaining / curr_block_size;
        uint32_t max_blocks = limit / curr_block_size;

        if (blocks > max_blocks) {
            blocks = max_blocks;
        }
        if (blocks > SDIO_ARG_CMD53_LENGTH_MAX) {
            blocks = SDIO_ARG_CMD53_LENGTH_MAX;
        }

        if (blocks > 0U) {
            *block_mode = true;
            *block_size = curr_block_size;
            *block_count = (uint16_t)blocks;
            *chunk_len = blocks * curr_block_size;
            *arg_length = *block_count;
            return;
        }
    }

    *block_mode = false;
    *chunk_len = limit > 512U ? 512U : limit;
    *block_count = 1U;
    *block_size = (uint16_t)(*chunk_len);
    *arg_length = (*chunk_len == 512U) ? 0U : (uint16_t)(*chunk_len);
}

/* Called from HW ISR context (sdh_irq). At this point ALL interrupt sources
 * have been masked by sdh_irq already — we must NOT touch interrupt enable
 * or clear registers here. Those are handled later:
 *   - CMD_COMP / TRAN_COMP: re-enabled by sdh_host_wait_done
 *   - CARD_INT:             re-enabled by sdh_sdio_irq_task after user handler
 *
 * This callback is invoked for every interrupt, not just CARD_INT, so we must
 * check the status register to filter for SDIO DAT1 in-band interrupts. */
static void sdh_sdio_host_irq_handler(struct sdh_host_s *host, void *arg)
{
    struct sdh_sdio_s *sdio = (struct sdh_sdio_s *)arg;
    uint32_t sta;

    if ((sdio == NULL) || (host->sdh_dev == NULL)) {
        return;
    }

    /* Read status — the bits are still latched because sdh_irq only disabled
     * the interrupt enable, it did not clear the status flags yet. */
    sta = bflb_sdh_sta_get(host->sdh_dev);

    if (!(sta & SDH_NORMAL_STA_CARD_INT)) {
        /* Not a CARD_INT event, nothing for us to do. */
        return;
    }
    /* Clear Card Interrupt Status Enable (034h bit8) to stop DAT[1] sampling
     * and clear the internal latch. CARD_INT is level-sensitive ROC — this is
     * the only way to ack it per SD Host Controller Spec v3.00 Section 2.2.9. */
    bflb_sdh_sta_en(host->sdh_dev, SDH_NORMAL_STA_CARD_INT, false);
    /* Wake the IRQ service task; it will claim the host, call the user
     * handler, then clear + re-enable CARD_INT in task context. */
    if (sdio->card_irq_sem != NULL) {
        sdh_osal_semb_give(sdio->card_irq_sem);
    } else if (sdio->card_irq_handler != NULL) {
        /* No-OS fallback: direct call. Handler must not touch the bus. */
        sdio->card_irq_handler(sdio, sdio->card_irq_arg);
    }
}

static void sdh_sdio_update_host_irq(struct sdh_sdio_s *sdio)
{
    if (sdio == NULL) {
        return;
    }

    sdh_host_irq_handler_set(sdio->host, sdio->card_irq_enabled ? sdh_sdio_host_irq_handler : NULL, sdio);
}

/* SDIO IRQ service task — Linux ksdioirqd analogue.
 *
 * Wait for HW ISR to signal via card_irq_sem, then in task context:
 *   1. claim host (acquire recursive mutex)
 *   2. invoke the registered card handler; handler may call
 *      sdh_sdio_cmd52/cmd53 — recursive mutex allows nested claim
 *   3. clear CARD_INT status and re-enable CARD_INT interrupt
 *   4. release host
 *
 * Task is created once in sdh_sdio_init and lives for the entire lifetime
 * of the sdio instance. It idles on card_irq_sem when no work is pending. */
static void sdh_sdio_irq_task(void *arg)
{
    struct sdh_sdio_s *sdio = (struct sdh_sdio_s *)arg;
    sdh_sdio_card_irq_handler_t handler;
    void *handler_arg;

    while (1) {
        /* block until HW ISR gives or deinit wakes us */
        if (sdh_osal_semb_take(sdio->card_irq_sem, 0) != 0) {
            continue;
        }

        if (sdio->card_irq_stop) {
            break;
        }

        if (!sdio->card_irq_enabled) {
            /* spurious wake: IRQ has been disabled, just go back to sleep */
            continue;
        }

        sdh_host_claim(sdio->host);

        handler = sdio->card_irq_handler;
        handler_arg = sdio->card_irq_arg;

        if (handler != NULL) {
            handler(sdio, handler_arg);
        }

        /* Clear the latched CARD_INT status, then re-enable it so the next
         * DAT1 event from the card can trigger a new interrupt.
         * This must happen AFTER the handler has consumed the pending source
         * on the card side, otherwise the same event would re-trigger. */
        /* Re-enable Card Interrupt Status Enable (034h bit8) to resume DAT[1]
         * sampling. The ISR cleared it to ack; now the handler has cleared the
         * card-side source. If DAT[1] is still low, a new IRQ fires — correct. */
        if (sdio->card_irq_enabled && (sdio->host->sdh_dev != NULL)) {
            bflb_sdh_sta_en(sdio->host->sdh_dev, SDH_NORMAL_STA_CARD_INT, true);
        }

        sdh_host_release(sdio->host);
    }

    /* graceful exit requested by sdh_sdio_deinit */
    sdio->card_irq_task = NULL;
    sdh_osal_task_exit_self();
}

static int sdh_sdio_io_abort(struct sdh_sdio_s *sdio)
{
    int ret;
    uint8_t reg;

    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_ABORT, SDH_SDIO_READ, &reg);
    if (ret < 0) {
        reg = (1U << 3);
    } else {
        reg |= (1U << 3);
    }

    return sdh_sdio_cmd52_resp(sdio, 0, SDIO_CCCR_ABORT, SDH_SDIO_WRITE, &reg, SDH_RESP_TPYE_R5b);
}

static int sdh_sdio_go_idle(struct sdh_sdio_s *sdio)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_GO_IDLE_STATE;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = 0;
    cmd_cfg.resp_type = SDH_RESP_TPYE_NONE;

    ret = sdh_host_transfer(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    return sdh_host_wait_done(sdio->host, &transfer);
}

static int sdh_sdio_send_if_cond(struct sdh_sdio_s *sdio, uint32_t host_ocr)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SD_SEND_IF_COND;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = (((host_ocr & 0xFF8000U) != 0U) ? 0x0100U : 0U) | 0xAAU;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R7;

    ret = sdh_host_transfer(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    ret = sdh_host_wait_done(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    return ((cmd_cfg.resp[0] & 0xffU) == 0xAAU) ? 0 : -1;
}

static int sdh_sdio_send_op_cond(struct sdh_sdio_s *sdio, uint32_t ocr, uint32_t *resp)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SD_IO_SEND_OP_COND;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = ocr;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R4;

    ret = sdh_host_transfer(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    ret = sdh_host_wait_done(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (resp != NULL) {
        *resp = cmd_cfg.resp[0];
    }

    return 0;
}

static int sdh_sdio_send_rca(struct sdh_sdio_s *sdio)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SET_RELATIVE_ADDR;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = 0;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R6;

    ret = sdh_host_transfer(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    ret = sdh_host_wait_done(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (cmd_cfg.resp[0] & 0xE000U) {
        return -1;
    }

    sdio->rca = (cmd_cfg.resp[0] >> 16) & 0xFFFFU;
    return 0;
}

static int sdh_sdio_select_card(struct sdh_sdio_s *sdio)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SELECT_CARD;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = sdio->rca << 16;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;

    ret = sdh_host_transfer(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    ret = sdh_host_wait_done(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    return 0;
}

static uint32_t sdh_sdio_get_host_ocr(void)
{
    uint32_t ocr = 0;

#if SDH_HOST_SUP_OCR_3V3
    ocr |= SDIO_OCR_VDD_32_33;
#endif

    return ocr;
}

static uint32_t sdh_sdio_decode_max_dtr(uint8_t speed)
{
    uint32_t unit;
    uint32_t mant;

    unit = sdh_sdio_tran_exp[speed & 0x07U];
    mant = sdh_sdio_tran_mant[(speed >> 3) & 0x0fU];
    if ((unit == 0U) || (mant == 0U)) {
        return 0;
    }

    return unit * mant;
}

static int sdh_sdio_read_u8(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *value)
{
    return sdh_sdio_cmd52(sdio, func, reg, SDH_SDIO_READ, value);
}

static int sdh_sdio_read_le24(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint32_t *value)
{
    int ret;
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;

    ret = sdh_sdio_read_u8(sdio, func, reg, &b0);
    if (ret < 0) {
        return ret;
    }

    ret = sdh_sdio_read_u8(sdio, func, reg + 1U, &b1);
    if (ret < 0) {
        return ret;
    }

    ret = sdh_sdio_read_u8(sdio, func, reg + 2U, &b2);
    if (ret < 0) {
        return ret;
    }

    *value = (uint32_t)b0 | ((uint32_t)b1 << 8) | ((uint32_t)b2 << 16);
    return 0;
}

static int sdh_sdio_read_buf(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *buf, uint8_t len)
{
    int ret;

    for (uint8_t i = 0; i < len; i++) {
        ret = sdh_sdio_read_u8(sdio, func, reg + i, &buf[i]);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}

static int sdh_sdio_parse_manfid(struct sdh_sdio_s *sdio, uint8_t func, const uint8_t *buf, uint8_t len)
{
    uint16_t vendor;
    uint16_t device;

    if (len < 4U) {
        return -1;
    }

    vendor = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    device = (uint16_t)buf[2] | ((uint16_t)buf[3] << 8);

    if (func == 0U) {
        sdio->vendor = vendor;
        sdio->device = device;
    } else {
        sdio->functions[func].vendor = vendor;
        sdio->functions[func].device = device;
    }

    return 0;
}

static int sdh_sdio_parse_funce(struct sdh_sdio_s *sdio, uint8_t func, const uint8_t *buf, uint8_t len)
{
    uint8_t sdio_vsn;
    uint8_t min_size;
    uint16_t timeout;

    if (func == 0U) {
        if (len < SDIO_CIS_FUNCE_COMMON_SIZE) {
            return -1;
        }

        sdio->max_block_size = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
        sdio->max_dtr = sdh_sdio_decode_max_dtr(buf[3]);
        return 0;
    }

    sdio_vsn = sdio->sdio_version;
    min_size = (sdio_vsn == SDIO_SDIO_REV_1_00) ? SDIO_CIS_FUNCE_FUNC_1_00 : SDIO_CIS_FUNCE_FUNC_1_10;
    if ((len == SDIO_CIS_FUNCE_FUNC_1_00) && (sdio_vsn == SDIO_SDIO_REV_1_10)) {
        LOG_W("Function %u has broken SDIO 1.1 FUNCE, fallback to SDIO 1.0\r\n", func);
        sdio_vsn = SDIO_SDIO_REV_1_00;
    } else if (len < min_size) {
        return -1;
    }

    sdio->functions[func].max_block_size = (uint16_t)buf[12] | ((uint16_t)buf[13] << 8);

    if ((sdio_vsn > SDIO_SDIO_REV_1_00) && (len >= 30U)) {
        timeout = (uint16_t)buf[28] | ((uint16_t)buf[29] << 8);
        sdio->functions[func].enable_timeout_ms = (timeout > (uint16_t)(0xFFFFU / 10U)) ? 0xFFFFU : (uint16_t)(timeout * 10U);
    }

    return 0;
}

static int sdh_sdio_parse_cis(struct sdh_sdio_s *sdio, uint8_t func, uint32_t cis_ptr)
{
    int ret;
    uint32_t reg = cis_ptr;
    uint32_t scanned = 0;
    uint8_t tuple_code;
    uint8_t tuple_link;
    uint8_t tuple_buf[SDIO_CIS_MAX_TUPLE_SIZE];

    if ((cis_ptr == 0U) || (cis_ptr > SDIO_ARG_CMD52_REG_MASK)) {
        LOG_W("Function %u CIS pointer invalid: 0x%05lX\r\n", func, (unsigned long)cis_ptr);
        return 0;
    }

    while (scanned < SDIO_CIS_SCAN_LIMIT) {
        ret = sdh_sdio_read_u8(sdio, 0, reg, &tuple_code);
        if (ret < 0) {
            return ret;
        }
        reg++;
        scanned++;

        if (tuple_code == SDIO_CIS_TPL_END) {
            return 0;
        }

        if (tuple_code == SDIO_CIS_TPL_NULL) {
            continue;
        }

        ret = sdh_sdio_read_u8(sdio, 0, reg, &tuple_link);
        if (ret < 0) {
            return ret;
        }
        reg++;
        scanned++;

        if (tuple_link > (SDIO_CIS_SCAN_LIMIT - scanned)) {
            LOG_E("Function %u CIS tuple 0x%02X overruns scan limit\r\n", func, tuple_code);
            return -1;
        }

        if ((tuple_link > 0U) && ((reg > SDIO_ARG_CMD52_REG_MASK) || ((uint32_t)tuple_link > (SDIO_ARG_CMD52_REG_MASK - reg + 1U)))) {
            LOG_E("Function %u CIS tuple 0x%02X overruns address space\r\n", func, tuple_code);
            return -1;
        }

        if (tuple_link > 0U) {
            ret = sdh_sdio_read_buf(sdio, 0, reg, tuple_buf, tuple_link);
            if (ret < 0) {
                return ret;
            }
        }

        switch (tuple_code) {
            case SDIO_CIS_TPL_MANFID:
                ret = sdh_sdio_parse_manfid(sdio, func, tuple_buf, tuple_link);
                if (ret < 0) {
                    LOG_E("Function %u has malformed MANFID tuple len=%u\r\n", func, tuple_link);
                    return ret;
                }
                break;
            case SDIO_CIS_TPL_FUNCE:
                ret = sdh_sdio_parse_funce(sdio, func, tuple_buf, tuple_link);
                if (ret < 0) {
                    LOG_E("Function %u has malformed FUNCE tuple len=%u\r\n", func, tuple_link);
                    return ret;
                }
                break;
            default:
                break;
        }

        reg += tuple_link;
        scanned += tuple_link;
    }

    LOG_E("Function %u CIS scan limit reached without END tuple\r\n", func);
    return -1;
}

static int sdh_sdio_read_cccr(struct sdh_sdio_s *sdio)
{
    int ret;
    uint8_t reg;

    ret = sdh_sdio_read_u8(sdio, 0, SDIO_CCCR_CCCR_SDIO_REV, &reg);
    if (ret < 0) {
        return ret;
    }

    sdio->cccr_version = reg & 0x0fU;
    sdio->sdio_version = (reg >> 4) & 0x0fU;
    if (sdio->cccr_version > SDIO_CCCR_REV_3_00) {
        LOG_E("Unsupported CCCR version %u\r\n", sdio->cccr_version);
        return -1;
    }

    ret = sdh_sdio_read_u8(sdio, 0, SDIO_CCCR_CARD_CAP, &reg);
    if (ret < 0) {
        return ret;
    }

    sdio->card_cap = reg;
    if (reg & SDIO_CARD_CAP_SMB) {
        sdio->card_flag |= SDH_SDIO_FLAG_SUP_MULTIBLK;
    }
    if (reg & SDIO_CARD_CAP_LSC) {
        sdio->card_flag |= SDH_SDIO_FLAG_LOW_SPEED;
    }
    if (((reg & SDIO_CARD_CAP_LSC) == 0U) || ((reg & SDIO_CARD_CAP_4BLS) != 0U)) {
        sdio->card_flag |= SDH_SDIO_FLAG_SUP_4BIT;
    }

    if (sdio->cccr_version >= SDIO_CCCR_REV_1_10) {
        ret = sdh_sdio_read_u8(sdio, 0, SDIO_CCCR_POWER_CTRL, &reg);
        if (ret < 0) {
            return ret;
        }
    }

    if (sdio->cccr_version >= SDIO_CCCR_REV_1_20) {
        ret = sdh_sdio_read_u8(sdio, 0, SDIO_CCCR_SPEED, &reg);
        if (ret < 0) {
            return ret;
        }

        sdio->high_speed = reg;
        if (reg & SDIO_HIGHSPEED_SHS) {
            sdio->card_flag |= SDH_SDIO_FLAG_SUP_HIGHSPEED;
        }
    }

    ret = sdh_sdio_read_le24(sdio, 0, SDIO_CCCR_CIS_PTR_0, &sdio->common_cis_ptr);
    if (ret < 0) {
        return ret;
    }

    LOG_D("SDIO cccr:%u sdio:%u funcs:%u cap:0x%02X hs:0x%02X cis:0x%05lX\r\n",
          sdio->cccr_version,
          sdio->sdio_version,
          sdio->num_funcs,
          sdio->card_cap,
          sdio->high_speed,
          (unsigned long)sdio->common_cis_ptr);

    return 0;
}

static int sdh_sdio_read_fbr(struct sdh_sdio_s *sdio, uint8_t func)
{
    int ret;
    uint8_t reg;
    uint32_t base = SDIO_FBR_BASE(func);

    ret = sdh_sdio_read_u8(sdio, 0, base + SDIO_FBR_STD_FUNC_IF, &reg);
    if (ret < 0) {
        return ret;
    }

    sdio->functions[func].std_if = reg & 0x0fU;
    sdio->functions[func].class_code = sdio->functions[func].std_if;
    if (sdio->functions[func].class_code == 0x0fU) {
        ret = sdh_sdio_read_u8(sdio, 0, base + SDIO_FBR_EXT_FUNC_IF, &reg);
        if (ret < 0) {
            return ret;
        }
        sdio->functions[func].class_code = reg;
    }

    ret = sdh_sdio_read_le24(sdio, 0, base + SDIO_FBR_CIS_PTR_0, &sdio->functions[func].cis_ptr);
    if (ret < 0) {
        return ret;
    }

    return 0;
}

static int sdh_sdio_switch_hs(struct sdh_sdio_s *sdio)
{
    int ret;
    uint8_t reg;

    if ((sdio->card_flag & SDH_SDIO_FLAG_SUP_HIGHSPEED) == 0U) {
        return 0;
    }

    ret = sdh_sdio_read_u8(sdio, 0, SDIO_CCCR_SPEED, &reg);
    if (ret < 0) {
        return ret;
    }

    if ((reg & SDIO_HIGHSPEED_SHS) == 0U) {
        return 0;
    }

    reg |= SDIO_HIGHSPEED_EHS;
    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_SPEED, SDH_SDIO_WRITE, &reg);
    if (ret < 0) {
        return ret;
    }

    sdio->high_speed = reg;
    sdio->card_flag |= SDH_SDIO_FLAG_HIGHSPEED_EN;
    return 1;
}


static int sdh_sdio_disable_func(struct sdh_sdio_s *sdio, uint8_t func)
{
	int ret;
	unsigned char reg;

    if ((func == 0U) || (func > sdio->num_funcs) || (func >= 8U)) {
        return -1;
    }


    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_IO_ENABLE, SDH_SDIO_READ, &reg);
    if (ret < 0) {
        return ret;
    }


	reg &= ~(1 << func);

    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_IO_ENABLE, SDH_SDIO_WRITE, &reg);
	if (ret < 0) {
		return ret;
	}


	return 0;

}

static int sdh_sdio_enable_func(struct sdh_sdio_s *sdio, uint8_t func)
{
    int ret;
    uint8_t reg = 0;
    uint8_t ready = 0;
    uint32_t timeout_ms;

    if ((func == 0U) || (func > sdio->num_funcs) || (func >= 8U)) {
        return -1;
    }

    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_IO_ENABLE, SDH_SDIO_READ, &reg);
    if (ret < 0) {
        return ret;
    }

    reg |= (uint8_t)(1U << func);
    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_IO_ENABLE, SDH_SDIO_WRITE, &reg);
    if (ret < 0) {
        return ret;
    }

    timeout_ms = sdio->functions[func].enable_timeout_ms;
    if (timeout_ms == 0U) {
        timeout_ms = SDIO_DATA_TIMEOUT_MS;
    }

    for (uint32_t i = 0; i < timeout_ms; i++) {
        ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_IO_READY, SDH_SDIO_READ, &ready);
        if (ret < 0) {
            return ret;
        }

        if (ready & (uint8_t)(1U << func)) {
            sdio->functions[func].enabled = true;
            return 0;
        }
        arch_delay_ms(1);
    }

    return -1;
}

static int sdh_sdio_set_block_size(struct sdh_sdio_s *sdio, uint8_t func, uint16_t block_size)
{
    int ret;
    uint8_t data;
    uint32_t reg;

    if ((func == 0U) || (func > sdio->num_funcs) || (func >= 8U) || (block_size == 0U)) {
        return -1;
    }

    if ((sdio->functions[func].max_block_size != 0U) && (block_size > sdio->functions[func].max_block_size)) {
        return -1;
    }

    reg = SDIO_FBR_BASE(func) + SDIO_FBR_BLOCK_SIZE_0;
    data = (uint8_t)(block_size & 0xffU);
    ret = sdh_sdio_cmd52(sdio, 0, reg, SDH_SDIO_WRITE, &data);
    if (ret < 0) {
        return ret;
    }

    reg = SDIO_FBR_BASE(func) + SDIO_FBR_BLOCK_SIZE_1;
    data = (uint8_t)((block_size >> 8) & 0xffU);
    ret = sdh_sdio_cmd52(sdio, 0, reg, SDH_SDIO_WRITE, &data);
    if (ret < 0) {
        return ret;
    }

    sdio->functions[func].cur_blksize = block_size;
    return 0;
}

int sdh_sg_start_transfer(struct sdh_scatterlist *sgs, int sg_count,
	int total, struct sdh_sdio_func_s *func,
	unsigned int fix_inc, bool dir, unsigned int addr)
{
	static struct bflb_sdh_data_tranfer_s adma_tags[SDH_ADMA_SG_COUNT_MAX];
	struct sdh_host_s *host;
	struct sdh_host_transfer_s transfer;
	struct bflb_sdh_cmd_cfg_s cmd_cfg;
	struct bflb_sdh_data_cfg_s data_cfg;
	uint16_t block_count;
	int ret = 0;

	if (!func || !func->card || !func->card->host || !sgs || sg_count <= 0 || sg_count > SDH_ADMA_SG_COUNT_MAX)
		return -1;

	host = func->card->host;

	for (int i = 0; i < sg_count; i++) {
		void *buf = sgs->entries[i].buffer;
		uint32_t len = sgs->entries[i].length;

		if (dir) {
			bflb_l1c_dcache_clean_range(buf, len);
		} else {
			bflb_l1c_dcache_clean_range(buf, 1);
			bflb_l1c_dcache_clean_range((uint8_t *)buf + len, 1);
			bflb_l1c_dcache_invalidate_range(buf, len);
		}

		adma_tags[i].address = buf;
		adma_tags[i].length = len;
		adma_tags[i].int_en = (i == sg_count - 1);

	}

	block_count = total / func->cur_blksize;

	memset(&transfer, 0, sizeof(transfer));
	memset(&cmd_cfg, 0, sizeof(cmd_cfg));
	memset(&data_cfg, 0, sizeof(data_cfg));

	data_cfg.data_dir = dir ? SDH_TRANSFER_DIR_WR : SDH_TRANSFER_DIR_RD;
	data_cfg.block_size = func->cur_blksize;
	data_cfg.block_count = block_count;
	data_cfg.adma_tranfer = adma_tags;
	data_cfg.adma_tranfer_cnt = sg_count;

	cmd_cfg.index = CMD_SD_IO_RW_EXTENDED;
	cmd_cfg.resp_type = SDH_RESP_TPYE_R5;
	cmd_cfg.argument = (dir ? SDIO_ARG_CMD53_WRITE : SDIO_ARG_CMD53_READ) |
		      ((func->num & SDIO_ARG_CMD53_FUNC_MASK) << SDIO_ARG_CMD53_FUNC_SHIFT) |
		      SDIO_ARG_CMD53_BLOCK_MODE |
		      (fix_inc == SDH_SDIO_ADMA_FIX_INC ? 0 : SDIO_ARG_CMD53_INCREMENT) |
		      ((addr & SDIO_ARG_CMD53_REG_MASK) << SDIO_ARG_CMD53_REG_SHIFT) |
		      ((block_count == 512) ? 0 : (block_count & SDIO_ARG_CMD53_LENGTH_MASK));

	transfer.cmd = &cmd_cfg;
	transfer.data = &data_cfg;

	LOG_D("adma %s total:%d sg:%d blk:%d arg:0x%08x\r\n",
	      dir ? "wr" : "rd", total, sg_count, block_count, cmd_cfg.argument);

	ret = sdh_host_transfer(host, &transfer);
	if (ret < 0) {
		return ret;
	}

	ret = sdh_host_wait_done(host, &transfer);
	if (ret < 0) {
		return ret;
	}

	if (!dir) {
		for (int i = 0; i < sg_count; i++)
			bflb_l1c_dcache_invalidate_range(sgs->entries[i].buffer, sgs->entries[i].length);
	}

	if (R5_FLAGS_STATUS(cmd_cfg.resp[0]) & (R5_COM_CRC_ERROR | R5_ILLEGAL_COMMAND | R5_ERROR | R5_FUNCTION_NUMBER | R5_OUT_OF_RANGE)) {
		return -1;
	}

	return 0;
}

static uint32_t sdh_sdio_get_max_clock(const struct sdh_sdio_s *sdio)
{
    if (sdio->card_flag & SDH_SDIO_FLAG_HIGHSPEED_EN) {
        return SDIO_HS_CLK_HZ;
    }

    if ((sdio->max_dtr != 0U) && (sdio->max_dtr < SDIO_DEFAULT_CLK_HZ)) {
        return sdio->max_dtr;
    }

    return SDIO_DEFAULT_CLK_HZ;
}

static int sdh_sdio_init_card(struct sdh_sdio_s *sdio)
{
    int ret;
    uint32_t resp4 = 0;
    uint32_t card_ocr;
    uint32_t host_ocr;
    uint32_t selected_ocr;
    uint32_t target_clock;
    uint32_t actual_clock;
    uint8_t data;

    ret = sdh_sdio_go_idle(sdio);
    if (ret < 0) {
        LOG_W("Standard SDIO goto idle failed, continue\r\n");
        return ret;
    }

    sdh_sdio_io_abort(sdio);

    host_ocr = sdh_sdio_get_host_ocr();

    if (sdh_sdio_send_if_cond(sdio, host_ocr) < 0) {
        LOG_D("CMD8 not accepted, continue with SDIO attach\r\n");
    }

    ret = sdh_sdio_send_op_cond(sdio, host_ocr, &resp4);
    if (ret < 0) {
        LOG_E("CMD5 OCR query failed\r\n");
        return ret;
    }

    card_ocr = resp4 & SDIO_OCR_VDD_MASK;
    selected_ocr = host_ocr & card_ocr;
    if (selected_ocr == 0U) {
        LOG_E("No compatible SDIO OCR host=0x%08lX card=0x%08lX\r\n",
              (unsigned long)host_ocr,
              (unsigned long)card_ocr);
        return -1;
    }

    for (uint32_t i = 0; i < SDIO_DATA_TIMEOUT_MS; i++) {
        ret = sdh_sdio_send_op_cond(sdio, selected_ocr, &resp4);
        if (ret < 0) {
            return ret;
        }

        if (resp4 & SDIO_OCR_FLAG_READY) {
            sdio->ocr = resp4;
            sdio->num_funcs = (uint8_t)((resp4 & SDIO_OCR_FLAG_NUM_FUNC_MASK) >> 28);
            if (resp4 & SDIO_OCR_FLAG_MEM_PRESENT) {
                sdio->card_flag |= SDH_SDIO_FLAG_MEM_PRESENT;
            }
            break;
        }

        arch_delay_ms(1);
        ret = -1;
    }
    if (ret < 0) {
        LOG_E("CMD5 wait ready timeout\r\n");
        return ret;
    }

    ret = sdh_sdio_send_rca(sdio);
    if (ret < 0) {
        LOG_E("CMD3 failed\r\n");
        return ret;
    }

    ret = sdh_sdio_select_card(sdio);
    if (ret < 0) {
        LOG_E("CMD7 failed\r\n");
        return ret;
    }

    ret = sdh_sdio_read_cccr(sdio);
    if (ret < 0) {
        LOG_E("Read CCCR failed\r\n");
        return ret;
    }

    ret = sdh_sdio_parse_cis(sdio, 0, sdio->common_cis_ptr);
    if (ret < 0) {
        LOG_E("Read common CIS failed\r\n");
        return ret;
    }

    for (uint8_t func = 1; func <= sdio->num_funcs && func <= SDIO_MAX_FUNC_NUM; func++) {
        ret = sdh_sdio_read_fbr(sdio, func);
        if (ret < 0) {
            LOG_E("Read FBR for function %u failed\r\n", func);
            return ret;
        }

        ret = sdh_sdio_parse_cis(sdio, func, sdio->functions[func].cis_ptr);
        if (ret < 0) {
            LOG_E("Read CIS for function %u failed\r\n", func);
            return ret;
        }

        if (sdio->functions[func].vendor == 0U) {
            sdio->functions[func].vendor = sdio->vendor;
            sdio->functions[func].device = sdio->device;
        }

        if (sdio->functions[func].max_block_size == 0U) {
            sdio->functions[func].max_block_size = SDIO_DEFAULT_BLOCK_SIZE;
        }
        if (sdio->functions[func].enable_timeout_ms == 0U) {
            sdio->functions[func].enable_timeout_ms = SDIO_DATA_TIMEOUT_MS;
        }
    }

    ret = sdh_sdio_switch_hs(sdio);
    if (ret < 0) {
        LOG_E("Switch high-speed failed\r\n");
        return ret;
    }

    target_clock = sdh_sdio_get_max_clock(sdio);
    ret = sdh_sdio_set_clock(sdio, target_clock, NULL);
    if (ret < 0) {
        LOG_E("Set transfer clock failed\r\n");
        return ret;
    }

	ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_IO_ENABLE, SDH_SDIO_READ, &data);
    if (ret < 0) {
        LOG_E("Read common CCCR ENABLE failed\r\n");
    }
	ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_IO_READY, SDH_SDIO_READ, &data);
    if (ret < 0) {
        LOG_E("Read common CCCR READY failed\r\n");
    }

	if (sdio->card_flag & SDH_SDIO_FLAG_SUP_4BIT) {
	    ret = sdh_sdio_set_bus_width(sdio, SDH_SDIO_BUS_WIDTH_4);
	} else {
		LOG_W("Card does not report 4-bit support\r\n");
	}

    uint8_t block_size_reg[2] = { 0 };
    uint32_t fbr_block_size_reg = SDIO_FBR_BASE(1) + SDIO_FBR_BLOCK_SIZE_0;
	if (sdio->num_funcs >= 1U) {
		ret = sdh_sdio_enable_func(sdio, 1);
	    if (ret < 0) {
	        LOG_E("Read common func1 failed\r\n");
	    }
		ret = sdh_sdio_set_block_size(sdio, 1, SDIO_DEFAULT_BLOCK_SIZE);
	    if (ret < 0) {
	        LOG_E("Read common block size failed\r\n");
	    }
		ret = sdh_sdio_cmd53_read(sdio, 0, fbr_block_size_reg, block_size_reg, sizeof(block_size_reg));
        if (ret < 0) {
	        LOG_E("Read common read block size failed\r\n");
	    }
		ret = sdh_sdio_cmd53_write(sdio, 0, fbr_block_size_reg, block_size_reg, sizeof(block_size_reg));
        if (ret < 0) {
	        LOG_E("Read common write block size failed\r\n");
	    }
	}
    sdh_set_data_timeout_ms(sdio->host, SDIO_DATA_TIMEOUT_MS, NULL);

    return 0;
}

static int sdh_sdio_transfer_cmd53(struct sdh_sdio_s *sdio,
                                   uint8_t func,
                                   uint32_t reg,
                                   uint8_t rw,
                                   bool incr_addr,
                                   uint8_t *data,
                                   uint32_t len)
{
    int ret = 0;
    uint32_t remaining = len;
    uint8_t *data_ptr = data;

    while (remaining > 0U) {
        bool block_mode = false;
        bool use_bounce = false;
        uint16_t arg_length;
        uint16_t block_size = sdio->functions[func].cur_blksize;
        uint16_t block_count;
        uint32_t chunk_len;
        struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
        struct bflb_sdh_data_cfg_s data_cfg = { 0 };
        struct sdh_host_transfer_s transfer = { &cmd_cfg, &data_cfg };
        struct bflb_sdh_data_tranfer_s data_transfer = { 0 };
        uint8_t *dma_data_ptr = data_ptr;

        sdh_sdio_cmd53_calc_chunk(sdio, func, remaining, 0U, &block_mode, &block_size, &block_count, &arg_length, &chunk_len);

        if (sdh_sdio_addr_need_bounce(data_ptr, chunk_len)) {
            use_bounce = true;

            if (chunk_len > sizeof(g_sdh_sdio_bounce_buf)) {
                sdh_sdio_cmd53_calc_chunk(sdio,
                                          func,
                                          remaining,
                                          sizeof(g_sdh_sdio_bounce_buf),
                                          &block_mode,
                                          &block_size,
                                          &block_count,
                                          &arg_length,
                                          &chunk_len);
            }

            printf("bufptr not align bounce:%d %s remain:%d block_mode:%d block_size:%d block_count:%d arg_length:%d chunk_len:%d incr_addr:%d reg:%x arg:%x %d tot_len:%d bufptr:%p\r\n",
               use_bounce,
               (rw == SDH_SDIO_WRITE) ? "write" : "read",
               remaining, block_mode, block_size, block_count, arg_length, chunk_len,
               incr_addr, reg, cmd_cfg.argument, data_transfer.length, len, dma_data_ptr);

            dma_data_ptr = g_sdh_sdio_bounce_buf;
            if (rw == SDH_SDIO_WRITE) {
                memcpy(dma_data_ptr, data_ptr, chunk_len);
            }
        }

        cmd_cfg.index = CMD_SD_IO_RW_EXTENDED;
        cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
        cmd_cfg.resp_type = SDH_RESP_TPYE_R5;
        cmd_cfg.argument = ((rw == SDH_SDIO_WRITE) ? SDIO_ARG_CMD53_WRITE : SDIO_ARG_CMD53_READ) |
                           ((func & SDIO_ARG_CMD53_FUNC_MASK) << SDIO_ARG_CMD53_FUNC_SHIFT) |
                           (incr_addr ? SDIO_ARG_CMD53_INCREMENT : 0U) |
                           (block_mode ? SDIO_ARG_CMD53_BLOCK_MODE : 0U) |
                           ((reg & SDIO_ARG_CMD53_REG_MASK) << SDIO_ARG_CMD53_REG_SHIFT) |
                           ((uint32_t)arg_length << SDIO_ARG_CMD53_LENGTH_SHIFT);

        data_transfer.address = dma_data_ptr;
        data_transfer.length = chunk_len;
        data_transfer.int_en = 0;

        data_cfg.data_dir = (rw == SDH_SDIO_WRITE) ? SDH_TRANSFER_DIR_WR : SDH_TRANSFER_DIR_RD;
        data_cfg.data_type = SDH_DATA_TPYE_NORMAL;
        data_cfg.auto_cmd_mode = SDH_AUTO_CMD_DISABLE;
        data_cfg.block_size = block_size;
        data_cfg.block_count = block_count;
        data_cfg.adma2_hw_desc_raw_mode = false;
        data_cfg.adma_tranfer = &data_transfer;
        data_cfg.adma_tranfer_cnt = 1;
#if 0
        printf("sdh_sdio_transfer_cmd53 func:%d bounce:%d %s remain:%d block_mode:%d block_size:%d block_count:%d arg_length:%d chunk_len:%d incr_addr:%d reg:%x arg:%x %d tot_len:%d bufptr:%p\r\n",
               func,
               use_bounce,
               (rw == SDH_SDIO_WRITE) ? "write" : "read",
               remaining, block_mode, block_size, block_count, arg_length, chunk_len,
               incr_addr, reg, cmd_cfg.argument, data_transfer.length, len, dma_data_ptr);
#endif
        if ((rw == SDH_SDIO_WRITE) && !use_bounce) {
            bflb_l1c_dcache_clean_range(data_ptr, chunk_len);
        } else if ((rw == SDH_SDIO_READ) && !use_bounce) {
            bflb_l1c_dcache_clean_range(data_ptr, 1);
            bflb_l1c_dcache_clean_range(data_ptr + chunk_len, 1);
            bflb_l1c_dcache_invalidate_range(data_ptr, chunk_len);
        }

        ret = sdh_host_transfer(sdio->host, &transfer);
        if (ret < 0) {
            return ret;
        }

        ret = sdh_host_wait_done(sdio->host, &transfer);
        if (ret < 0) {
            printf("response:%08x\r\n", cmd_cfg.resp[0]);
            return ret;
        }

        if ((rw == SDH_SDIO_READ) && !use_bounce) {
            bflb_l1c_dcache_invalidate_range(data_ptr, chunk_len);
        }

        if (R5_FLAGS_STATUS(cmd_cfg.resp[0]) & (R5_COM_CRC_ERROR | R5_ILLEGAL_COMMAND | R5_ERROR | R5_FUNCTION_NUMBER | R5_OUT_OF_RANGE)) {
            return -1;
        }

        if ((rw == SDH_SDIO_READ) && use_bounce) {
            memcpy(data_ptr, dma_data_ptr, chunk_len);
        }

        data_ptr += chunk_len;
        if (incr_addr) {
            reg += chunk_len;
        }
        remaining -= chunk_len;
    }

    return 0;
}

static int sdh_sdio_cmd53_read(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len)
{
    if ((sdio == NULL) || (data == NULL) || (len == 0U) || (func > 7U)) {
        return -1;
    }

    return sdh_sdio_transfer_cmd53(sdio, func, reg, SDH_SDIO_READ, true, data, len);
}

static int sdh_sdio_cmd53_write(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len)
{
    if ((sdio == NULL) || (data == NULL) || (len == 0U) || (func > 7U)) {
        return -1;
    }

    return sdh_sdio_transfer_cmd53(sdio, func, reg, SDH_SDIO_WRITE, true, data, len);
}

static int sdh_sdio_cmd52(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t rw, uint8_t *data)
{
    return sdh_sdio_cmd52_resp(sdio, func, reg, rw, data, SDH_RESP_TPYE_R5);
}

static int sdh_sdio_cmd52_resp(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg,
                               uint8_t rw, uint8_t *data, uint8_t resp_type)
{
    int ret;
    uint8_t cmd_data = 0;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    if ((sdio == NULL) || (data == NULL) || (func > 7U) || (reg > SDIO_ARG_CMD52_REG_MASK)) {
        return -1;
    }

    if (rw == SDH_SDIO_WRITE) {
        cmd_data = *data;
    }

    cmd_cfg.index = CMD_SD_IO_RW_DIRECT;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.resp_type = resp_type;
    cmd_cfg.argument = ((rw == SDH_SDIO_WRITE) ? SDIO_ARG_CMD52_WRITE : SDIO_ARG_CMD52_READ) |
                       ((func & SDIO_ARG_CMD52_FUNC_MASK) << SDIO_ARG_CMD52_FUNC_SHIFT) |
                       ((reg & SDIO_ARG_CMD52_REG_MASK) << SDIO_ARG_CMD52_REG_SHIFT) |
                       ((uint32_t)(cmd_data & SDIO_ARG_CMD52_DATA_MASK) << SDIO_ARG_CMD52_DATA_SHIFT);

    ret = sdh_host_transfer(sdio->host, &transfer);
    if (ret < 0) {
    	printf("error func:%d reg:%x arg:%08x\r\n", func, reg, cmd_cfg.argument);
        //return ret;
    }

    ret = sdh_host_wait_done(sdio->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R5_FLAGS_STATUS(cmd_cfg.resp[0]) & (R5_COM_CRC_ERROR | R5_ILLEGAL_COMMAND | R5_ERROR | R5_FUNCTION_NUMBER | R5_OUT_OF_RANGE)) {
        return -1;
    }

    if (rw == SDH_SDIO_READ) {
        *data = (uint8_t)SDIO_R5_DATA(cmd_cfg.resp);
    }

    return 0;
}

static int sdh_sdio_check_func(const struct sdh_sdio_func_s *func)
{
    if ((func == NULL) || (func->card == NULL) || (func->num > 7U)) {
        return -1;
    }

    return 0;
}

/* Re-enumerate the SDIO card without touching host hardware or the IRQ task.
 * Equivalent to a "plug-out → plug-in" cycle at the card protocol level:
 *   1. Disable card IRQ
 *   2. Re-run the card init sequence (CMD0→CMD5→CMD3→CMD7→CCCR→CIS→HS)
 *   3. Restore bus width / clock / block size as needed by upper layer */
int sdh_sdio_rescan(struct sdh_sdio_s *sdio)
{
    int ret;
    struct sdh_host_s *host;

    if ((sdio == NULL) || (sdio->host == NULL)) {
        return -1;
    }

    host = sdio->host;

    /* 1. disable card irq */
    sdio->card_irq_enabled = false;
    sdh_sdio_update_host_irq(sdio);
    bflb_sdh_sta_en(host->sdh_dev, SDH_NORMAL_STA_CARD_INT, false);
    bflb_sdh_sta_int_en(host->sdh_dev, SDH_NORMAL_STA_CARD_INT, false);
    sdio->card_irq_handler = NULL;
    sdio->card_irq_arg = NULL;

    /* 2. reset card-side state (keep host HW and IRQ task intact) */
    sdio->ocr = 0;
    sdio->rca = 0;
    sdio->num_funcs = 0;
    sdio->card_flag = 0;

    /* 3. slow clock + 1-bit bus for fresh enumeration */
    sdh_set_bus_width(host, 1);
    sdh_sdio_set_clock(sdio, SDIO_LP_CLK_HZ, NULL);
    sdh_set_data_timeout_ms(host, SDIO_DATA_TIMEOUT_MS, NULL);
    sdh_send_active_clk(host);

    /* 4. re-enumerate */
    ret = sdh_sdio_init_card(sdio);
    if (ret < 0) {
        LOG_E("rescan: card init failed %d\r\n", ret);
        return ret;
    }

    host->card = sdio;

    LOG_I("rescan OK: RCA=0x%04lX funcs=%u\r\n",
          (unsigned long)sdio->rca, sdio->num_funcs);

    return 0;
}

/* Hard reset: re-initialize SDH host controller hardware, then rescan the card.
 * Use when the bus is in an unrecoverable error state. The IRQ task is kept
 * alive across the reset. */
int sdh_sdio_reset(struct sdh_sdio_s *sdio)
{
    int ret;
    struct sdh_host_s *host;

    if ((sdio == NULL) || (sdio->host == NULL)) {
        return -1;
    }

    host = sdio->host;

    /* 1. disable card irq */
    sdio->card_irq_enabled = false;
    sdh_sdio_update_host_irq(sdio);
    sdio->card_irq_handler = NULL;
    sdio->card_irq_arg = NULL;

    /* 2. tear down host HW (but NOT the IRQ task / semaphore) */
    sdh_host_deinit(host);

    /* 3. re-init host HW from scratch */
    ret = sdh_host_init(host);
    if (ret < 0) {
        LOG_E("reset: host init failed %d\r\n", ret);
        return ret;
    }
    sdh_force_clock(host, true);

    /* 4. reset card-side state */
    sdio->ocr = 0;
    sdio->rca = 0;
    sdio->num_funcs = 0;
    sdio->card_flag = 0;

    /* 5. slow clock + 1-bit for fresh enumeration */
    sdh_set_bus_width(host, 1);
    sdh_sdio_set_clock(sdio, SDIO_LP_CLK_HZ, NULL);
    sdh_set_data_timeout_ms(host, SDIO_DATA_TIMEOUT_MS, NULL);
    sdh_send_active_clk(host);

    /* 6. re-enumerate */
    ret = sdh_sdio_init_card(sdio);
    if (ret < 0) {
        LOG_E("reset: card init failed %d\r\n", ret);
        return ret;
    }

    host->card = sdio;

    LOG_I("reset OK: RCA=0x%04lX funcs=%u\r\n",
          (unsigned long)sdio->rca, sdio->num_funcs);

    return 0;
}

int sdh_sdio_scan_done(struct sdh_sdio_s *sdio)
{
	static struct sdh_host_s host;

    board_sdh_gpio_init();

    return sdh_sdio_init(sdio, &host);
}

int sdh_sdio_set_clock(struct sdh_sdio_s *sdio, uint32_t freq, uint32_t *actual_freq)
{
    int ret;
    uint32_t freq_hz = 0;

    if (sdio == NULL) {
        return -1;
    }

    ret = sdh_set_bus_clock(sdio->host, freq, &freq_hz);
    if (ret < 0) {
        return ret;
    }

    sdio->bus_clock_hz = freq_hz;
    if (actual_freq != NULL) {
        *actual_freq = freq_hz;
    }

    return 0;
}

int sdh_sdio_set_bus_width(struct sdh_sdio_s *sdio, uint8_t bus_width)
{
    int ret;
    uint8_t reg = 0;

    if ((bus_width != SDH_SDIO_BUS_WIDTH_1) && (bus_width != SDH_SDIO_BUS_WIDTH_4)) {
        return -1;
    }

    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_BUS_IF_CTRL, SDH_SDIO_READ, &reg);
    if (ret < 0) {
        return ret;
    }

    reg &= (uint8_t)~SDIO_BUS_IF_CTRL_WIDTH_MASK;
    reg |= (bus_width == SDH_SDIO_BUS_WIDTH_4) ? SDIO_BUS_IF_CTRL_WIDTH_4BIT : SDIO_BUS_IF_CTRL_WIDTH_1BIT;

    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_BUS_IF_CTRL, SDH_SDIO_WRITE, &reg);
    if (ret < 0) {
        return ret;
    }

    return sdh_set_bus_width(sdio->host, bus_width);
}


/* Aligns with Linux sdio_claim_irq(func, handler):
 *   1. Program the card's CCCR INT_ENABLE — set IEN bit for this func + IENM
 *   2. Register software handler
 *   3. Enable controller's CARD_INT source
 *
 * The IRQ service task and its semaphore are created once in sdh_sdio_init.
 *
 * Caller MUST already hold the host (via sdh_sdio_claim_host) because this
 * function issues CMD52 to the card. */
int sdh_sdio_claim_irq(struct sdh_sdio_func_s *func, sdh_sdio_card_irq_handler_t handler, void *arg)
{
    struct sdh_sdio_s *sdio;
    uint8_t reg;
    int ret;

    if (sdh_sdio_check_func(func) < 0 || handler == NULL) {
        return -1;
    }

    sdio = func->card;
    if ((sdio->host == NULL) || (sdio->host->sdh_dev == NULL)) {
        return -1;
    }

    if (sdio->card_irq_handler != NULL) {
        /* already claimed, mirror Linux -EBUSY */
        return -1;
    }

    /* 1. read CCCR INT_ENABLE (0x04) — CCCR belongs to function 0 */
    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_INT_ENABLE, SDH_SDIO_READ, &reg);
    if (ret < 0) {
        return ret;
    }

    /* 2. set IEN bit for this function + IENM (master enable, bit0) */
    reg |= (uint8_t)(1U << func->num);
    reg |= (uint8_t)(1U << 0);

    /* 3. write back */
    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_INT_ENABLE, SDH_SDIO_WRITE, &reg);
    if (ret < 0) {
        return ret;
    }

    /* 4. register handler */
    sdio->card_irq_handler = handler;
    sdio->card_irq_arg = arg;
    sdio->card_irq_enabled = true;

    /* 5. hook HW ISR path */
    sdh_sdio_update_host_irq(sdio);

    /* 6. enable controller CARD_INT source:
     *    Status Enable (034h bit8): start DAT[1] sampling
     *    Signal Enable (038h bit8): route CARD_INT to CPU interrupt line */
    bflb_sdh_sta_en(sdio->host->sdh_dev, SDH_NORMAL_STA_CARD_INT, true);
    bflb_sdh_sta_int_en(sdio->host->sdh_dev, SDH_NORMAL_STA_CARD_INT, true);

    return 0;
}

/* Aligns with Linux sdio_release_irq(func):
 *   1. Disable controller CARD_INT source
 *   2. Clear this func's IEN bit in CCCR; clear IENM if no func IEN remains
 *   3. Clear handler
 *
 * The IRQ service task is NOT destroyed here — it keeps idling on the
 * semaphore and will be torn down in sdh_sdio_deinit.
 *
 * Caller MUST already hold the host. */
int sdh_sdio_release_irq(struct sdh_sdio_func_s *func)
{
    struct sdh_sdio_s *sdio;
    uint8_t reg;
    int ret;

    if (sdh_sdio_check_func(func) < 0) {
        return -1;
    }

    sdio = func->card;
    if ((sdio->host == NULL) || (sdio->host->sdh_dev == NULL)) {
        return -1;
    }

    /* 1. mask controller CARD_INT:
     *    Status Enable (034h bit8): stop DAT[1] sampling + clear internal latch
     *    Signal Enable (038h bit8): stop routing to CPU interrupt line */
    bflb_sdh_sta_en(sdio->host->sdh_dev, SDH_NORMAL_STA_CARD_INT, false);
    bflb_sdh_sta_int_en(sdio->host->sdh_dev, SDH_NORMAL_STA_CARD_INT, false);

    sdio->card_irq_enabled = false;
    sdh_sdio_update_host_irq(sdio);

    /* 2. disable IEN for this function on the card (best-effort).
     * CCCR belongs to function 0. */
    ret = sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_INT_ENABLE, SDH_SDIO_READ, &reg);
    if (ret == 0) {
        reg &= (uint8_t)~(1U << func->num);
        /* If no function IEN bits remain, clear IENM as well */
        if ((reg & 0xFEU) == 0U) {
            reg &= (uint8_t)~(1U << 0);
        }
        (void)sdh_sdio_cmd52(sdio, 0, SDIO_CCCR_INT_ENABLE, SDH_SDIO_WRITE, &reg);
    }

    /* 3. clear handler */
    sdio->card_irq_handler = NULL;
    sdio->card_irq_arg = NULL;

    return 0;
}

void sdh_sdio_claim_host(struct sdh_sdio_func_s *func)
{
    if (sdh_sdio_check_func(func) < 0) {
        return;
    }

    (void)sdh_host_claim(func->card->host);
}

void sdh_sdio_release_host(struct sdh_sdio_func_s *func)
{
    if (sdh_sdio_check_func(func) < 0) {
        return;
    }

    (void)sdh_host_release(func->card->host);
}

int sdh_sdio_func_enable(struct sdh_sdio_func_s *func)
{
    if (sdh_sdio_check_func(func) < 0) {
        return -1;
    }

    return sdh_sdio_enable_func(func->card, func->num);
}

int sdh_sdio_func_disable(struct sdh_sdio_func_s *func)
{
    if (sdh_sdio_check_func(func) < 0) {
        return -1;
    }

    return sdh_sdio_disable_func(func->card, func->num);
}

int sdh_sdio_func_set_block_size(struct sdh_sdio_func_s *func, unsigned int block_size)
{
    if ((sdh_sdio_check_func(func) < 0) || (block_size > 0xFFFFU)) {
        return -1;
    }

    return sdh_sdio_set_block_size(func->card, func->num, (uint16_t)block_size);
}

unsigned char sdh_sdio_readb(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data = 0;

    if (sdh_sdio_check_func(func) < 0) {
        if (err_ret != NULL) {
            *err_ret = -1;
        }
        return 0;
    }

    ret = sdh_sdio_cmd52(func->card, func->num, addr, SDH_SDIO_READ, &data);
    if (err_ret != NULL) {
        *err_ret = ret;
    }

    return data;
}

unsigned short sdh_sdio_readw(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data[2] = { 0 };

    ret = sdh_sdio_memcpy_fromio(func, data, addr, sizeof(data));
    if (err_ret != NULL) {
        *err_ret = ret;
    }

    if (ret < 0) {
        return 0;
    }

    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

unsigned int sdh_sdio_readl(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data[4] = { 0 };

    ret = sdh_sdio_memcpy_fromio(func, data, addr, sizeof(data));
    if (err_ret != NULL) {
        *err_ret = ret;
    }

    if (ret < 0) {
        return 0;
    }

    return (uint32_t)data[0] |
           ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) |
           ((uint32_t)data[3] << 24);
}

void sdh_sdio_writeb(struct sdh_sdio_func_s *func, unsigned char b, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data = b;

    if (sdh_sdio_check_func(func) < 0) {
        if (err_ret != NULL) {
            *err_ret = -1;
        }
        return;
    }

    ret = sdh_sdio_cmd52(func->card, func->num, addr, SDH_SDIO_WRITE, &data);
    if (err_ret != NULL) {
        *err_ret = ret;
    }
}

void sdh_sdio_writew(struct sdh_sdio_func_s *func, unsigned short b, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data[2];

    data[0] = (uint8_t)(b & 0xffU);
    data[1] = (uint8_t)((b >> 8) & 0xffU);

    ret = sdh_sdio_memcpy_toio(func, addr, data, sizeof(data));
    if (err_ret != NULL) {
        *err_ret = ret;
    }
}

void sdh_sdio_writel(struct sdh_sdio_func_s *func, unsigned int b, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data[4];

    data[0] = (uint8_t)(b & 0xffU);
    data[1] = (uint8_t)((b >> 8) & 0xffU);
    data[2] = (uint8_t)((b >> 16) & 0xffU);
    data[3] = (uint8_t)((b >> 24) & 0xffU);

    ret = sdh_sdio_memcpy_toio(func, addr, data, sizeof(data));
    if (err_ret != NULL) {
        *err_ret = ret;
    }
}

int sdh_sdio_memcpy_fromio(struct sdh_sdio_func_s *func, void *dst, unsigned int addr, int count)
{
    if ((sdh_sdio_check_func(func) < 0) || ((dst == NULL) && (count != 0)) || (count < 0)) {
        return -1;
    }

    if (count == 0) {
        return 0;
    }

    return sdh_sdio_cmd53_read(func->card, func->num, addr, (uint8_t *)dst, (uint32_t)count);
}

int sdh_sdio_memcpy_toio(struct sdh_sdio_func_s *func, unsigned int addr, const void *src, int count)
{
    if ((sdh_sdio_check_func(func) < 0) || ((src == NULL) && (count != 0)) || (count < 0)) {
        return -1;
    }

    if (count == 0) {
        return 0;
    }

    return sdh_sdio_cmd53_write(func->card, func->num, addr, (uint8_t *)(uintptr_t)src, (uint32_t)count);
}

int sdh_sdio_func_readsb(struct sdh_sdio_func_s *func, void *dst, unsigned int addr, int count)
{
    if ((sdh_sdio_check_func(func) < 0) || ((dst == NULL) && (count != 0)) || (count < 0)) {
        return -1;
    }

    if (count == 0) {
        return 0;
    }

    return sdh_sdio_readsb(func->card, func->num, addr, (uint8_t *)dst, (uint32_t)count);
}

int sdh_sdio_func_writesb(struct sdh_sdio_func_s *func, unsigned int addr, const void *src, int count)
{
    if ((sdh_sdio_check_func(func) < 0) || ((src == NULL) && (count != 0)) || (count < 0)) {
        return -1;
    }

    if (count == 0) {
        return 0;
    }

    return sdh_sdio_writesb(func->card, func->num, addr, (uint8_t *)(uintptr_t)src, (uint32_t)count);
}

unsigned char sdh_sdio_f0_readb(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data = 0;

    if (sdh_sdio_check_func(func) < 0) {
        if (err_ret != NULL) {
            *err_ret = -1;
        }
        return 0;
    }

    ret = sdh_sdio_cmd52(func->card, 0, addr, SDH_SDIO_READ, &data);
    if (err_ret != NULL) {
        *err_ret = ret;
    }

    return data;
}

void sdh_sdio_f0_writeb(struct sdh_sdio_func_s *func, unsigned char b, unsigned int addr, int *err_ret)
{
    int ret;
    uint8_t data = b;

    if (sdh_sdio_check_func(func) < 0) {
        if (err_ret != NULL) {
            *err_ret = -1;
        }
        return;
    }

    ret = sdh_sdio_cmd52(func->card, 0, addr, SDH_SDIO_WRITE, &data);
    if (err_ret != NULL) {
        *err_ret = ret;
    }
}

int sdh_sdio_readsb(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len)
{
    if ((sdio == NULL) || (data == NULL) || (len == 0U) || (func > 7U)) {
        return -1;
    }

    return sdh_sdio_transfer_cmd53(sdio, func, reg, SDH_SDIO_READ, false, data, len);
}

int sdh_sdio_writesb(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len)
{
    if ((sdio == NULL) || (data == NULL) || (len == 0U) || (func > 7U)) {
        return -1;
    }

    return sdh_sdio_transfer_cmd53(sdio, func, reg, SDH_SDIO_WRITE, false, data, len);
}

int sdh_sdio_deinit(struct sdh_sdio_s *sdio)
{
    if (sdio == NULL) {
        return -1;
    }

    /* 1. Prevent new IRQs from firing */
    sdio->card_irq_enabled = false;
    sdh_host_irq_handler_set(sdio->host, NULL, NULL);
    bflb_sdh_sta_en(sdio->host->sdh_dev, SDH_NORMAL_STA_CARD_INT, false);
    bflb_sdh_sta_int_en(sdio->host->sdh_dev, SDH_NORMAL_STA_CARD_INT, false);

    /* 2. Signal the IRQ task to exit and wait for it to finish.
     * Set stop flag first, then give semaphore to wake the task.
     * Whether the task is blocked on semb_take or running the handler,
     * it will see stop=true on the next loop iteration and exit. */
    if (sdio->card_irq_task != NULL) {
        sdio->card_irq_stop = true;
        if (sdio->card_irq_sem != NULL) {
            sdh_osal_semb_give(sdio->card_irq_sem);
        }
        /* Wait until the task has self-deleted */
        while (sdio->card_irq_task != NULL) {
            sdh_osal_delay_ms(1);
        }
    }

    /* 3. Now the task has exited, safe to delete semaphore */
    if (sdio->card_irq_sem != NULL) {
        sdh_osal_semb_delete(sdio->card_irq_sem);
        sdio->card_irq_sem = NULL;
    }

    /* Unlink host → card before tearing down */
    if (sdio->host != NULL) {
        sdio->host->card = NULL;
    }

    sdh_host_deinit(sdio->host);
    memset(sdio, 0, sizeof(struct sdh_sdio_s));

    return 0;
}

int sdh_sdio_init(struct sdh_sdio_s *sdio, struct sdh_host_s *sdh_host)
{
    int ret;

    if (sdio == NULL) {
        return -1;
    }

    memset(sdio, 0, sizeof(struct sdh_sdio_s));
    memset(sdh_host, 0, sizeof(struct sdh_host_s));

    sdio->host = sdh_host;

    for (uint8_t i = 0; i < 8U; i++) {
        sdio->functions[i].card = sdio;
        sdio->functions[i].num = i;
        sdio->functions[i].enable_timeout_ms = SDIO_DATA_TIMEOUT_MS;
        sdio->functions[i].max_block_size = SDIO_DEFAULT_BLOCK_SIZE;
    }
    sdio->max_block_size = SDIO_DEFAULT_BLOCK_SIZE;

    ret = sdh_host_init(sdio->host);
    if (ret < 0) {
        return ret;
    }
    sdh_force_clock(sdio->host, true);
    /* Create ISR->task binary semaphore and the SDIO IRQ service task once.
     * They live for the entire lifetime of the sdio instance and are reused
     * across repeated sdh_sdio_irq_enable/disable calls. On no-OS builds
     * these adapter calls return NULL and the ISR falls back to direct
     * handler invocation. */
    sdio->card_irq_stop = false;
    sdio->card_irq_sem = sdh_osal_semb_create();
    if (sdio->card_irq_sem != NULL) {
        sdio->card_irq_task = sdh_osal_task_create("sdioirq",
                                                   sdh_sdio_irq_task,
                                                   sdio,
                                                   2048,
                                                   30);
        if (sdio->card_irq_task == NULL) {
            sdh_osal_semb_delete(sdio->card_irq_sem);
            sdio->card_irq_sem = NULL;
            /* Keep going in a degraded mode: HW ISR will call handler
             * directly. Upper layer should avoid bus access in that case. */
        }
    }

    sdh_sdio_update_host_irq(sdio);

    sdh_set_bus_width(sdio->host, 1);
    ret = sdh_sdio_set_clock(sdio, SDIO_LP_CLK_HZ, NULL);
    if (ret < 0) {
        return ret;
    }
    sdh_set_data_timeout_ms(sdio->host, SDIO_DATA_TIMEOUT_MS, NULL);
    sdh_send_active_clk(sdio->host);

    ret = sdh_sdio_init_card(sdio);
    if (ret < 0) {
        return ret;
    }

    if (sdio->card_flag & SDH_SDIO_FLAG_MEM_PRESENT) {
        LOG_W("SDIO combo card detected; SD memory side is not attached by sdh_sdio.c\r\n");
    }

    /* Link host ↔ card */
    sdio->host->card = sdio;

    LOG_I("sdio init success: RCA=0x%04lX, funcs=%u, ocr=0x%08lX, clk=%luHz\r\n",
          (unsigned long)sdio->rca,
          sdio->num_funcs,
          (unsigned long)sdio->ocr,
          (unsigned long)sdio->bus_clock_hz);

    return 0;
}
