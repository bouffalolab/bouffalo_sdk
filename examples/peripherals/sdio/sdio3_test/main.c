#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_sdio3.h"

#if defined(BL628)
#include "bl628_glb.h"
#endif

#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

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

struct bflb_sdio3_config_s sdio3_cfg = {
    .func_num = SDIO_TEST_FUNC_NUM,                            /*!< function num: 1~2. */
    .ocr = 0xff0000,                                           /*!< OCR, [14:0]:res, [23:15]:2.7v~3.6v */
    .cap_flag = SDIO3_CAP_FLAG_SDR50 | SDIO3_CAP_FLAG_RD_WAIT, /*!< capability flag */
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

void sdio3_test(void)
{
    uint32_t f1_dnld_cnt_last = 0, f1_upld_cnt_last = 0;
    uint32_t f2_dnld_cnt_last = 0, f2_upld_cnt_last = 0;
    bool func1_ready = false;
    bool func2_ready = false;

#if defined(BL628)
    bflb_glb_per_clock_ungate(GLB_AHB_CLOCK_SDU);
    bflb_glb_per_clock_ungate(GLB_AHB_CLOCK_USB20_SDU);
    bflb_glb_per_clock_ungate(GLB_AHB_CLOCK_USB_SDIO);
#endif

    sdio3_hd = bflb_device_get_by_name("sdio3");
    if (sdio3_hd == NULL) {
        LOG_I("get device failed\r\n");
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
        bflb_sdio3_dnld_push(sdio3_hd, &dnld_desc);
    }
    /* card func ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_SET_FUNC_CARD_READY, 2);
#endif

    LOG_I("Waiting host ready...\r\n");
    while (1) {
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
            break;
        }

        bflb_mtimer_delay_ms(1);
    }

    /* print info */
    while (1) {
        if (f1_dnld_cnt_last != f1_dnld_cnt || f1_upld_cnt_last != f1_upld_cnt) {
            f1_dnld_cnt_last = f1_dnld_cnt;
            f1_upld_cnt_last = f1_upld_cnt;
            LOG_I("func1 test, dnld_cnt: %d, upld_cnt: %d\r\n", f1_dnld_cnt_last, f1_upld_cnt_last);
        }

        if (f2_dnld_cnt_last != f2_dnld_cnt || f2_upld_cnt_last != f2_upld_cnt) {
            f2_dnld_cnt_last = f2_dnld_cnt;
            f2_upld_cnt_last = f2_upld_cnt;
            LOG_I("func2 test, dnld_cnt: %d, upld_cnt: %d\r\n", f2_dnld_cnt_last, f2_upld_cnt_last);
        }

#if 0
        for (uint32_t i = 0; i < 200; i++) {
            intptr_t flag = bflb_irq_save();
            bflb_mtimer_delay_ms(1);
            bflb_irq_restore(flag);
            bflb_mtimer_delay_ms(1);
        }
#else
        bflb_mtimer_delay_ms(500);
#endif
    }
}

int main(void)
{
    board_init();

    board_sdio_gpio_init();

    LOG_I("sdio3 test case\r\n");

    /* sdio3 test */
    sdio3_test();

    LOG_I("sdio3 test case end\r\n");

    while (1) {
        bflb_mtimer_delay_ms(5000);
        LOG_I("sdio3 test...\r\n");
    }
}
