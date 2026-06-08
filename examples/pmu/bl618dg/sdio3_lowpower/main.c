#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_sdio3.h"
#include "bflb_uart.h"
#include "shell.h"

#if defined(BL618DG)
#include "bl618dg_glb.h"
#include "bl618dg_pm.h"
#include "bl618dg_hbn.h"
#include "bl618dg_pds.h"
#include "bl_lp.h"
#endif

#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define PDS_TIMER_WAKEUP_TIME BL_MS_TO_PDS_CNT(3000)

#define SDIO3_DBG_INT_STATUS_OFFSET    (0x3C)
#define SDIO3_DBG_INT_STATUS_EN_OFFSET (0x40)
#define SDIO3_DBG_INT_SIGNAL_EN_OFFSET (0x44)
#define SDIO3_DBG_INT_STATUS2_OFFSET   (0x9C)
#define SDIO3_DBG_INT_STATUS_EN2_OFFSET (0xA0)
#define SDIO3_DBG_INT_SIGNAL_EN2_OFFSET (0xA4)

static struct bflb_device_s *uart;

// #define SDIO3_DBG(a, ...)   LOG_I(a, ##__VA_ARGS__)
#define SDIO3_DBG(a, ...)

#define SDIO_TEST_FUNC_NUM  (SDIO3_FUNC_NUM_MAX)

#define SDIO3_FN1_TEST_SIZE (2 * 1024)
#define SDIO3_FN2_TEST_SIZE (2 * 1024)

/* T-L-V struct */
typedef struct {
    uint16_t tag;
    uint16_t length;
    uint8_t value[0];
} sdio_tlv_t;

struct bflb_device_s *sdio3_hd;
volatile uint32_t f1_dnld_cnt, f1_upld_cnt;
volatile uint32_t f2_dnld_cnt, f2_upld_cnt;

typedef enum {
    SDIO_STATE_IDENTIFY = 0,
    SDIO_STATE_RUN,
} sdio_state_t;

static sdio_state_t sdio_state;
static uint32_t f1_dnld_cnt_last, f1_upld_cnt_last;
static uint32_t f2_dnld_cnt_last, f2_upld_cnt_last;
static bool func1_ready;
static bool func2_ready;

struct bflb_sdio3_config_s sdio3_cfg = {
    .func_num = SDIO_TEST_FUNC_NUM,                            /*!< function num: 1~2. */
    .ocr = 0xff0000,                                           /*!< OCR, [14:0]:res, [23:15]:2.7v~3.6v */
    .cap_flag = SDIO3_CAP_FLAG_RD_WAIT,         /*!< capability flag */
    .func1_dnld_size_max = SDIO3_FN1_TEST_SIZE,                /* dnld max size */
    .func2_dnld_size_max = SDIO3_FN2_TEST_SIZE,                /* dnld max size */
};

void sdio3_irq_cb_func1_dnld_cpl(void *arg, bflb_sdio3_trans_desc_t *trans_desc)
{
    f1_dnld_cnt++;

    /* invalid cache */
    bflb_l1c_dcache_invalidate_range(trans_desc->buff, trans_desc->data_len);

    sdio_tlv_t *tlv = (sdio_tlv_t *)trans_desc->buff;
    uint16_t tlv_len = tlv->length + sizeof(sdio_tlv_t);

    SDIO3_DBG("func1_dnld_cpl %d, tag 0x%04X, tran_len %d, tlv_len %d, upld it!\r\n\r\n", f1_dnld_cnt, tlv->tag, trans_desc->data_len, tlv_len);

    /* push to upld queue*/
    trans_desc->data_len = tlv_len;
    bflb_sdio3_upld_push(sdio3_hd, trans_desc);
}

void sdio3_irq_cb_func1_upld_cpl(void *arg, bflb_sdio3_trans_desc_t *trans_desc)
{
    f1_upld_cnt++;

    SDIO3_DBG("func1_upld_cpl %d, push buff to dnld queue\r\n\r\n", f1_upld_cnt);

    /* max upld buff len */
    trans_desc->buff_len = SDIO3_FN1_TEST_SIZE;
    /* push to dnld queue */
    bflb_sdio3_dnld_push(sdio3_hd, trans_desc);
}

#if (SDIO_TEST_FUNC_NUM == 2)
void sdio3_irq_cb_func2_dnld_cpl(void *arg, bflb_sdio3_trans_desc_t *trans_desc)
{
    f2_dnld_cnt++;

    /* invalid cache */
    bflb_l1c_dcache_invalidate_range(trans_desc->buff, trans_desc->data_len);

    sdio_tlv_t *tlv = (sdio_tlv_t *)trans_desc->buff;
    uint16_t tlv_len = tlv->length + sizeof(sdio_tlv_t);

    SDIO3_DBG("func2_dnld_cpl %d, tag 0x%04X, tran_len %d, tlv_len %d, upld it!\r\n\r\n", f2_dnld_cnt, tlv->tag, trans_desc->data_len, tlv_len);

    /* push to upld queue*/
    trans_desc->data_len = tlv_len;
    bflb_sdio3_upld_push(sdio3_hd, trans_desc);
}

void sdio3_irq_cb_func2_upld_cpl(void *arg, bflb_sdio3_trans_desc_t *trans_desc)
{
    f2_upld_cnt++;

    SDIO3_DBG("func2_upld_cpl %d, push buff to dnld queue\r\n\r\n", f2_upld_cnt);

    /* max upld buff len */
    trans_desc->buff_len = SDIO3_FN1_TEST_SIZE;
    /* push to dnld queue */
    bflb_sdio3_dnld_push(sdio3_hd, trans_desc);
}
#endif

void sdio3_irq_cb_soft_reset(void *arg, bflb_sdio3_trans_desc_t *trans_desc)
{
    LOG_I("sdio3 soft reset!\r\n");

    /* free func1 buff */
    {
        bflb_sdio3_trans_desc_t func1_desc;
        int func1_free_n = 0;
        LOG_I("free func1 dnld queue buff\r\n");
        /* free func1 dnld queue buff */
        while (bflb_sdio3_dnld_pop(sdio3_hd, &func1_desc, 1) == 0) {
            free(func1_desc.buff);
            func1_free_n++;
        }
        LOG_I("free func1 upld queue buff\r\n");
        /* free func1 upld queue buff */
        while (bflb_sdio3_upld_pop(sdio3_hd, &func1_desc, 1) == 0) {
            free(func1_desc.buff);
            func1_free_n++;
        }
        LOG_I("func1 buff free num:%d\r\n", func1_free_n);
    }
#if (SDIO_TEST_FUNC_NUM == 2)
    /* free func2 buff */
    {
        bflb_sdio3_trans_desc_t func2_desc;
        int func2_free_n = 0;
        LOG_I("free func2 dnld queue buff\r\n");
        /* free func1 dnld queue buff */
        while (bflb_sdio3_dnld_pop(sdio3_hd, &func2_desc, 2) == 0) {
            free(func2_desc.buff);
            func2_free_n++;
        }
        LOG_I("free func2 upld queue buff\r\n");
        /* free func1 upld queue buff */
        while (bflb_sdio3_upld_pop(sdio3_hd, &func2_desc, 2) == 0) {
            free(func2_desc.buff);
            func2_free_n++;
        }
        LOG_I("func2 buff free num:%d\r\n", func2_free_n);
    }
#endif
    /* sdio3 queue ctrl reset */
    bflb_sdio3_deinit(sdio3_hd);

    /* sdio3 init */
    bflb_sdio3_init(sdio3_hd, &sdio3_cfg);
    /* card init ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_INIT_READY, 0);

    LOG_I("malloc buff and push to func1 dnld_queue\r\n");
    for (uint32_t i = 0; i < SDIO3_FUNC_QUEUE_NUM_MAX; i++) {
        void *buff = malloc(SDIO3_FN1_TEST_SIZE);
        if (buff == NULL) {
            LOG_E("malloc NULL\r\n");
            while (1) {
            }
        }
        bflb_sdio3_trans_desc_t dnld_desc = {
            .func = 1,
            .buff = buff,
            .buff_len = SDIO3_FN1_TEST_SIZE,
            .user_arg = NULL,
        };
        bflb_l1c_dcache_invalidate_range(buff, SDIO3_FN1_TEST_SIZE);
        bflb_sdio3_dnld_push(sdio3_hd, &dnld_desc);
    }
    /* card func ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_SET_FUNC_CARD_READY, 1);

#if (SDIO_TEST_FUNC_NUM == 2)
    LOG_I("malloc buff and push to func2 dnld_queue\r\n");
    for (uint32_t i = 0; i < SDIO3_FUNC_QUEUE_NUM_MAX; i++) {
        void *buff = malloc(SDIO3_FN2_TEST_SIZE);
        if (buff == NULL) {
            LOG_E("malloc NULL\r\n");
            while (1) {
            }
        }
        bflb_sdio3_trans_desc_t dnld_desc = {
            .func = 2,
            .buff = buff,
            .buff_len = SDIO3_FN2_TEST_SIZE,
            .user_arg = NULL,
        };
        bflb_l1c_dcache_invalidate_range(buff, SDIO3_FN2_TEST_SIZE);
        bflb_sdio3_dnld_push(sdio3_hd, &dnld_desc);
    }
    /* card func ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_SET_FUNC_CARD_READY, 2);
#endif
}

void sdio3_irq_cb(void *arg, uint32_t irq_event, bflb_sdio3_trans_desc_t *trans_desc)
{
    switch (irq_event) {
        case SDIO3_IRQ_EVENT_CMD11_SWITCH:
            LOG_W("The host try to switch the voltage to 1.8v\r\n");
            break;

        case SDIO3_IRQ_EVENT_CMD11_CLK_START:
            LOG_W("CLK START\r\n");
            break;

        case SDIO3_IRQ_EVENT_CMD11_CLK_STOP:
            LOG_W("CLK STOP\r\n");
            break;

        case SDIO3_IRQ_EVENT_DNLD_CPL:
            if (trans_desc->func == 1) {
                /* func1 dnld */
                sdio3_irq_cb_func1_dnld_cpl(arg, trans_desc);
            }
#if (SDIO_TEST_FUNC_NUM == 2)
            else {
                sdio3_irq_cb_func2_dnld_cpl(arg, trans_desc);
            }
#endif
            break;

        case SDIO3_IRQ_EVENT_UPLD_CPL:
            if (trans_desc->func == 1) {
                /* func1 upld */
                sdio3_irq_cb_func1_upld_cpl(arg, trans_desc);
            }
#if (SDIO_TEST_FUNC_NUM == 2)
            else {
                sdio3_irq_cb_func2_upld_cpl(arg, trans_desc);
            }
#endif
            break;

        case SDIO3_IRQ_EVENT_SOFT_RESET:
            sdio3_irq_cb_soft_reset(arg, trans_desc);
            break;

        case SDIO3_IRQ_EVENT_FUNC1_RESET:
            SDIO3_DBG("func1_reset\r\n");
            break;

        case SDIO3_IRQ_EVENT_FUNC2_RESET:
            SDIO3_DBG("func2_reset\r\n");
            break;

        default:
            if (irq_event >= SDIO3_IRQ_EVENT_ERR_CRC) {
                LOG_E("sdio3 error event %d\r\n", irq_event);
            } else {
                SDIO3_DBG("irq_event %d\r\n", irq_event);
            }
            break;
    }
}

static bool sdio3_init_once(void)
{
#if defined(BL618DG)
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDU);
#endif

    sdio3_hd = bflb_device_get_by_name("sdio3");
    if (sdio3_hd == NULL) {
        LOG_I("get device failed\r\n");
        return false;
    }

    /* sdio3_hd init */
    bflb_sdio3_init(sdio3_hd, &sdio3_cfg);
    /* irq event cb attach */
    bflb_sdio3_irq_attach(sdio3_hd, sdio3_irq_cb, NULL);
    /* card init ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_INIT_READY, 0);

    /* malloc buff and link to dnld queue */
    LOG_I("push buff to func1 dnld_queue\r\n");
    for (uint32_t i = 0; i < SDIO3_FUNC_QUEUE_NUM_MAX; i++) {
        void *buff = malloc(SDIO3_FN1_TEST_SIZE);
        if (buff == NULL) {
            LOG_E("malloc NULL\r\n");
            while (1) {
            }
        }
        bflb_sdio3_trans_desc_t dnld_desc = {
            .func = 1,
            .buff = buff,
            .buff_len = SDIO3_FN1_TEST_SIZE,
            .user_arg = NULL,
        };
        bflb_l1c_dcache_invalidate_range(buff, SDIO3_FN1_TEST_SIZE);
        bflb_sdio3_dnld_push(sdio3_hd, &dnld_desc);
    }
    /* card func ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_SET_FUNC_CARD_READY, 1);

#if (SDIO_TEST_FUNC_NUM == 2)
    LOG_I("push buff to func2 dnld_queue\r\n");
    for (uint32_t i = 0; i < SDIO3_FUNC_QUEUE_NUM_MAX; i++) {
        void *buff = malloc(SDIO3_FN2_TEST_SIZE);
        if (buff == NULL) {
            LOG_E("malloc NULL\r\n");
            while (1) {
            }
        }
        bflb_sdio3_trans_desc_t dnld_desc = {
            .func = 2,
            .buff = buff,
            .buff_len = SDIO3_FN2_TEST_SIZE,
            .user_arg = NULL,
        };
        bflb_l1c_dcache_invalidate_range(buff, SDIO3_FN2_TEST_SIZE);
        bflb_sdio3_dnld_push(sdio3_hd, &dnld_desc);
    }
    /* card func ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_SET_FUNC_CARD_READY, 2);
#endif

    func1_ready = false;
    func2_ready = false;
    f1_dnld_cnt_last = f1_dnld_cnt;
    f1_upld_cnt_last = f1_upld_cnt;
    f2_dnld_cnt_last = f2_dnld_cnt;
    f2_upld_cnt_last = f2_upld_cnt;
    sdio_state = SDIO_STATE_IDENTIFY;

    LOG_I("Waiting host ready...\r\n");

    return true;
}

static void sdio3_identify_step(void)
{
    if (!func1_ready && bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_FUNC_HOST_READY, 1)) {
        LOG_I("sdio3 func1 host ready! \r\n");
        func1_ready = true;
        /* block size */
        uint16_t f1_blk_size = bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_FUNC_BLK_SIZE, 1);
        LOG_I("func1 blk size: %d\r\n", f1_blk_size);
        /* get dnld/upld max size */
        uint16_t f1_dnld_max_size = bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_DNLD_MAX_SIZE, 1);
        LOG_I("func1 dnld max size: %d\r\n", f1_dnld_max_size);
        uint16_t f1_upld_max_size = bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_UPLD_MAX_SIZE, 1);
        LOG_I("func1 upld max size: %d\r\n", f1_upld_max_size);
    }

    if (!func2_ready && bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_FUNC_HOST_READY, 2)) {
        LOG_I("sdio3 func2 host ready! \r\n");
        func2_ready = true;
        /* block size */
        uint16_t f2_blk_size = bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_FUNC_BLK_SIZE, 2);
        LOG_I("func2 blk size: %d\r\n", f2_blk_size);
        /* get dnld/upld max size */
        uint16_t f2_dnld_max_size = bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_DNLD_MAX_SIZE, 2);
        LOG_I("func2 dnld max size: %d\r\n", f2_dnld_max_size);
        uint16_t f2_upld_max_size = bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_UPLD_MAX_SIZE, 2);
        LOG_I("func2 upld max size: %d\r\n", f2_upld_max_size);
    }

    if (func1_ready && (func2_ready || SDIO_TEST_FUNC_NUM < 2)) {
        sdio_state = SDIO_STATE_RUN;
    }
}

static void sdio3_run_step(void)
{
    if (f1_dnld_cnt_last != f1_dnld_cnt || f1_upld_cnt_last != f1_upld_cnt) {
        f1_dnld_cnt_last = f1_dnld_cnt;
        f1_upld_cnt_last = f1_upld_cnt;
        // LOG_I("func1 test, dnld_cnt: %d, upld_cnt: %d\r\n", f1_dnld_cnt_last, f1_upld_cnt_last);
    }
    if (f2_dnld_cnt_last != f2_dnld_cnt || f2_upld_cnt_last != f2_upld_cnt) {
        f2_dnld_cnt_last = f2_dnld_cnt;
        f2_upld_cnt_last = f2_upld_cnt;
        // LOG_I("func2 test, dnld_cnt: %d, upld_cnt: %d\r\n", f2_dnld_cnt_last, f2_upld_cnt_last);
    }
}

static void shell_poll(void)
{
    int ch;

    if ((ch = bflb_uart_getchar(uart)) != -1) {
        shell_handler(ch);
    }
}

void sdio3_lowpower_test(void)
{
    if (!sdio3_init_once()) {
        while (1) {
            bflb_mtimer_delay_ms(1000);
        }
    }

    while (1) {
        shell_poll();

        switch (sdio_state) {
            case SDIO_STATE_IDENTIFY:
                sdio3_identify_step();
                bflb_mtimer_delay_ms(1);
                break;

            case SDIO_STATE_RUN:
                sdio3_run_step();
                bflb_mtimer_delay_ms(1);
                break;

            default:
                bflb_mtimer_delay_ms(1);
                break;
        }
    }
}

#if defined(BL618DG)
static void app_print_wakeup_source(void)
{
    for (uint32_t i = 8; i <= 49; i++) {
        if (PDS_Get_GPIO_Pad_IntStatus(i)) {
            printf("gpio_%d wakeup pds\r\n", i);
            PDS_Set_GPIO_Pad_IntClr(i);
        }
    }
    for (uint8_t i = HBN_INT_GPIO0; i <= HBN_INT_GPIO7; i++) {
        if (SET == HBN_Get_INT_State(i)) {
            printf("gpio%d wakeup pds\r\n", i);
            HBN_Clear_IRQ(i);
        }
    }
    PDS_IntClear();
}

static void app_print_wakeup_reason_regs(void)
{
    uint32_t pds_int = getreg32(PDS_BASE + PDS_INT_OFFSET);
    uint32_t pds_wakeup_src = PDS_Get_All_Wakeup_Src();
    uint32_t hbn_irq_stat = getreg32(HBN_BASE + HBN_IRQ_STAT_OFFSET);
    uint32_t hbn_int_state = HBN_Get_All_INT_State();

    printf("wakeup reason regs:\r\n");
    printf("  PDS_INT(0x%08X)=0x%08X\r\n", PDS_BASE + PDS_INT_OFFSET, pds_int);
    printf("  PDS wakeup src=0x%08X\r\n", pds_wakeup_src);
    printf("  HBN_IRQ_STAT(0x%08X)=0x%08X\r\n", HBN_BASE + HBN_IRQ_STAT_OFFSET, hbn_irq_stat);
    printf("  HBN int state=0x%08X\r\n", hbn_int_state);

    if (SET == PDS_Get_IntStatus(PDS_INT_WAKEUP)) {
        printf("  PDS_INT_WAKEUP set\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_PDS_TIMER)) {
        printf("  --PDS timer wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_HBN_IRQ_OUT)) {
        printf("  --HBN IRQ out wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_GPIO_IRQ)) {
        printf("  --GPIO IRQ wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_PDS_GPIO_INT)) {
        printf("  --PDS GPIO INT wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_IRRX_INT)) {
        printf("  --IRRX INT wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_WIFI_WAKEUP)) {
        printf("  --WIFI wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_DM_SLP_IRQ)) {
        printf("  --DM SLP IRQ wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_USB_SDU)) {
        printf("  --USB SDU wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_KEYSCAN)) {
        printf("  --Keyscan wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ)) {
        printf("  --WIFI TBTT wakeup\r\n");
    }
    if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_SW_CONFIG)) {
        printf("  --SW config wakeup\r\n");
    }
}

static void app_print_sdio_irq_status(const char *tag)
{
    struct bflb_device_s *dev = bflb_device_get_by_name("sdio3");
    uint32_t reg_base = dev ? dev->reg_base : SDU_BASE;
    int irq_num = dev ? dev->irq_num : SDIO_IRQn;
    uint32_t int_status = getreg32(reg_base + SDIO3_DBG_INT_STATUS_OFFSET);
    uint32_t int_status_en = getreg32(reg_base + SDIO3_DBG_INT_STATUS_EN_OFFSET);
    uint32_t int_signal_en = getreg32(reg_base + SDIO3_DBG_INT_SIGNAL_EN_OFFSET);
    uint32_t int_status2 = getreg32(reg_base + SDIO3_DBG_INT_STATUS2_OFFSET);
    uint32_t int_status_en2 = getreg32(reg_base + SDIO3_DBG_INT_STATUS_EN2_OFFSET);
    uint32_t int_signal_en2 = getreg32(reg_base + SDIO3_DBG_INT_SIGNAL_EN2_OFFSET);

    printf("%s SDIO IRQ status:\r\n", tag);
    printf("  irq=%d pending=%d enable=%d\r\n", irq_num, ECLIC_GetPendingIRQ(irq_num), ECLIC_GetEnableIRQ(irq_num));
    printf("  SDIO3_INT_STATUS      (0x%08X)=0x%08X\r\n", reg_base + SDIO3_DBG_INT_STATUS_OFFSET, int_status);
    printf("  SDIO3_INT_STATUS_EN   (0x%08X)=0x%08X\r\n", reg_base + SDIO3_DBG_INT_STATUS_EN_OFFSET, int_status_en);
    printf("  SDIO3_INT_SIGNAL_EN   (0x%08X)=0x%08X\r\n", reg_base + SDIO3_DBG_INT_SIGNAL_EN_OFFSET, int_signal_en);
    printf("  SDIO3_INT_STATUS2     (0x%08X)=0x%08X\r\n", reg_base + SDIO3_DBG_INT_STATUS2_OFFSET, int_status2);
    printf("  SDIO3_INT_STATUS_EN2  (0x%08X)=0x%08X\r\n", reg_base + SDIO3_DBG_INT_STATUS_EN2_OFFSET, int_status_en2);
    printf("  SDIO3_INT_SIGNAL_EN2  (0x%08X)=0x%08X\r\n", reg_base + SDIO3_DBG_INT_SIGNAL_EN2_OFFSET, int_signal_en2);

    uint32_t active = int_status & int_signal_en;
    uint32_t active2 = int_status2 & int_signal_en2;
    if (active) {
        printf("  active status & signal=0x%08X\r\n", active);
    }
    if (active2) {
        printf("  active status2 & signal2=0x%08X\r\n", active2);
    }
}

static void app_set_mie(uint32_t mie)
{
    __ASM volatile("csrw mie, %0" ::"r"(mie)
                   : "memory");
}

int cmd_pds_enter(int argc, char **argv)
{
    uint32_t pds_mode = PM_PDS_LEVEL_3;
    uint32_t mie_before_pds;
    uint32_t mie_after_clear;

    if (argc >= 2) {
        pds_mode = atoi(argv[1]);
    }

    printf("pds_mode:%d, wake by PDS timer after 3s\r\n", pds_mode);

#if 0
    uint32_t wake_io = GPIO_PIN_43;
    static volatile lp_gpio_cfg_type lp_wake_io_cfg;
    memset((void *)&lp_wake_io_cfg, 0x00, sizeof(lp_wake_io_cfg));

    lp_wake_io_cfg.io_pu = (uint64_t)1 << wake_io;
    lp_wake_io_cfg.io_ie = (uint64_t)1 << wake_io;
    lp_wake_io_cfg.io_wakeup_unmask = (uint64_t)1 << wake_io;

    /* falling edge = trig_mode 0 for sync, 8 for async */
    uint8_t trig_mode = 8; /* async falling edge */
    if (wake_io <= 7) {
        lp_wake_io_cfg.io_0_7_trig_mode = trig_mode;
    } else if (wake_io <= 15) {
        lp_wake_io_cfg.io_8_15_trig_mode = trig_mode;
    } else if (wake_io <= 23) {
        lp_wake_io_cfg.io_16_23_trig_mode = trig_mode;
    } else if (wake_io <= 31) {
        lp_wake_io_cfg.io_24_31_trig_mode = trig_mode;
    } else if (wake_io <= 39) {
        lp_wake_io_cfg.io_32_39_trig_mode = trig_mode;
    } else if (wake_io <= 47) {
        lp_wake_io_cfg.io_40_47_trig_mode = trig_mode;
    } else {
        lp_wake_io_cfg.io_48_52_trig_mode = trig_mode;
    }

    pm_lowpower_gpio_cfg((lp_gpio_cfg_type *)&lp_wake_io_cfg);
#endif

    PDS_Mask_All_Wakeup_Src();
    HBN_Pin_WakeUp_Mask(0xF);
    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_PDS_TIMER, UNMASK);
    app_print_sdio_irq_status("before PDS");

    printf("enter PDS%d, wake by PDS timer after 3s\r\n", pds_mode);
    arch_delay_us(500);

    pm_pds_irq_register();

    mie_before_pds = __RV_CSR_READ(CSR_MIE);
    app_set_mie(0);
    mie_after_clear = __RV_CSR_READ(CSR_MIE);
    printf("MIE before PDS=0x%08X, after clear=0x%08X\r\n", mie_before_pds, mie_after_clear);

    bl_lp_pds_enter_with_restore(pds_mode, PDS_TIMER_WAKEUP_TIME);

    app_set_mie(mie_before_pds);
    printf("Wakeup PDS%d\r\n", pds_mode);

    app_print_sdio_irq_status("after PDS");
    app_print_wakeup_reason_regs();
    app_print_wakeup_source();
    PDS_IntClear();

    shell_init();

    sdio3_hd = bflb_device_get_by_name("sdio3");
    if (sdio3_hd == NULL) {
        printf("get device failed\r\n");
    } else {
        bflb_irq_enable(sdio3_hd->irq_num);
    }



    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pds_enter, pds_enter, enter pds mode with 3s timer wakeup);


int dump_regs(int argc, char **argv)
{
    uint32_t addr = 0x20085000;
    uint32_t count = 32;
    volatile uint32_t *ptr;

    if (argc >= 2) {
        addr = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = (uint32_t)strtoul(argv[2], NULL, 0);
    }

    ptr = (volatile uint32_t *)addr;
    printf("Dump registers from 0x%08X, count=%u\r\n", addr, count);
    for (uint32_t i = 0; i < count; i++) {
        if (i % 4 == 0) {
            printf("0x%08X: ", (uint32_t)(addr + i * 4));
        }
        printf("0x%08X ", ptr[i]);
        if (i % 4 == 3) {
            printf("\r\n");
        }
    }
    if (count % 4 != 0) {
        printf("\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(dump_regs, dump_regs, dump regs [addr] [count]);


#endif

int main(void)
{

    board_init();

    board_sdio_gpio_init();

    LOG_I("sdio3 lowpower test case\r\n");

#if defined(BL618DG)
    app_print_wakeup_source();
#endif

    uart = bflb_device_get_by_name("uart0");
    shell_init();

    /* sdio3 low-power test (includes shell loop for BL618DG) */
    sdio3_lowpower_test();

    while (1) {
        bflb_mtimer_delay_ms(5000);
    }

}
