#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_sdio2.h"

#if defined(BL616)
#include "bl616_glb.h"
#endif

#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

// #define SDIO2_DBG(a, ...) LOG_I(a, ##__VA_ARGS__)
#define SDIO2_DBG(a, ...)

#define SDIO2_TEST_SIZE (2 * 1024)

/* T-L-V struct */
typedef struct {
    uint16_t tag;
    uint16_t length;
    uint8_t value[0];
} sdio_tlv_t;

volatile uint32_t dnld_cnt, upld_cnt;
struct bflb_device_s *sdio2_hd;

/* gpio init */
void sdio2_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

#if defined(BL616)
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
#elif defined(BL602)
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_5, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
#endif
}

/* dnld irq callback */
void sdio2_dnld_irq_callback(void *arg, bflb_sdio2_trans_desc_t *trans_desc)
{
    /* dnld done */
    dnld_cnt++;
    /* invalid cache */
    bflb_l1c_dcache_invalidate_range(trans_desc->buff, trans_desc->data_len);

    sdio_tlv_t *tlv = (sdio_tlv_t *)trans_desc->buff;

    /* update dnld data_len */
    trans_desc->data_len = tlv->length + sizeof(sdio_tlv_t);

    SDIO2_DBG("dnld done, tag 0x%04X, len %d, upld it!\r\n", tlv->tag, (tlv->length + sizeof(sdio_tlv_t)));

    /* push to upld queue*/
    bflb_sdio2_upld_port_push(sdio2_hd, trans_desc);
}

/* upld irq callback */
void sdio2_upld_irq_callback(void *arg, bflb_sdio2_trans_desc_t *trans_desc)
{
    /* upld done */
    upld_cnt++;

    /* max upld buff len */
    trans_desc->buff_len = SDIO2_TEST_SIZE;

    SDIO2_DBG("upld done, push buff to dnld queue\r\n");

    /* push to dnld queue */
    bflb_sdio2_dnld_port_push(sdio2_hd, trans_desc);
}

#if defined(BL616)
/* reset irq callback */
void sdio2_reset_irq_callback(void *arg)
{
    LOG_I("\r\n***** sdio2 reset! *****\r\n");

    /* clean cnt */
    dnld_cnt = 0;
    upld_cnt = 0;

    /* free test buff */
    {
        bflb_sdio2_trans_desc_t trans_desc;
        int free_n = 0;
        LOG_I("free dnld queue buff\r\n");
        while (bflb_sdio2_dnld_port_pop(sdio2_hd, &trans_desc) == 0) {
            free(trans_desc.buff);
            free_n++;
        }

        LOG_I("free upld queue buff\r\n");
        while (bflb_sdio2_upld_port_pop(sdio2_hd, &trans_desc) == 0) {
            free(trans_desc.buff);
            free_n++;
        }

        LOG_I("buff free num:%d\r\n", free_n);
    }

    /* sdio2 queue ctrl reset */
    bflb_sdio2_deinit(sdio2_hd);

    /* sdio2 init */
    bflb_sdio2_init(sdio2_hd, SDIO2_TEST_SIZE);

    /* link all dnld buff */
    LOG_I("malloc buff and push to dnld_queue\r\n");
    for (uint32_t i = 0; i < SDIO2_MAX_PORT_NUM; i++) {
        void *buff = malloc(SDIO2_TEST_SIZE);
        if (buff == NULL) {
            LOG_E("malloc NULL\r\n");
            while (1) {
            }
        }

        bflb_sdio2_trans_desc_t dnld_desc = {
            .buff = buff,
            .buff_len = SDIO2_TEST_SIZE,
            .user_arg = NULL,
        };
        bflb_sdio2_dnld_port_push(sdio2_hd, &dnld_desc);
    }
}
#endif

void sdio2_irq_cb(void *arg, uint32_t irq_event, bflb_sdio2_trans_desc_t *trans_desc)
{
    switch (irq_event) {
        case SDIO2_IRQ_EVENT_DNLD_CPL:
            sdio2_dnld_irq_callback(arg, trans_desc);
            break;

        case SDIO2_IRQ_EVENT_UPLD_CPL:
            sdio2_upld_irq_callback(arg, trans_desc);
            break;

#if defined(BL616)
        case SDIO2_IRQ_EVENT_SOFT_RESET:
            sdio2_reset_irq_callback(arg);
            break;
#endif

        case SDIO2_IRQ_EVENT_ERR_CRC:
            LOG_E("sdio2 error: CRC ERROR!\r\n");
            break;
        case SDIO2_IRQ_EVENT_ERR_ABORT:
            LOG_E("sdio2 error: ABORT!\r\n");
            break;
        case SDIO2_IRQ_EVENT_ERR_UNKNOW:
            LOG_E("sdio2 error: UNKNOW!\r\n");
            break;
        default:
            LOG_E("irq_event %d\r\n", irq_event);
            break;
    }
}

void sdio2_test(void)
{
    uint32_t dnld_cnt_last = 0, upld_cnt_last = 0;

#if defined(BL616)
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDU);
#elif defined(BL602)
#endif

    sdio2_hd = bflb_device_get_by_name("sdio2");
    if (sdio2_hd == NULL) {
        LOG_I("get device failed\r\n");
        while (1) {
        }
    }

    /* sdio2 init */
    bflb_sdio2_init(sdio2_hd, SDIO2_TEST_SIZE);

    /* attach int callback */
    bflb_sdio2_irq_attach(sdio2_hd, sdio2_irq_cb, NULL);

    /* malloc buff and link to dnld queue */
    LOG_I("malloc buff and push to dnld_queue\r\n");
    for (uint32_t i = 0; i < SDIO2_MAX_PORT_NUM; i++) {
        void *buff = malloc(SDIO2_TEST_SIZE);
        if (buff == NULL) {
            LOG_E("malloc NULL\r\n");
            while (1) {
            }
        }

        bflb_sdio2_trans_desc_t dnld_desc = {
            .buff = buff,
            .buff_len = SDIO2_TEST_SIZE,
            .user_arg = NULL,
        };
        bflb_sdio2_dnld_port_push(sdio2_hd, &dnld_desc);
    }

    /* wait host ready */
    LOG_I("sdio2 wait host ready...\r\n");
    while (bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_FUNC_HOST_READY, 0) == 0) {
        bflb_mtimer_delay_ms(1);
    }
    LOG_I("sdio2 host ready! \r\n");
    /* bus width */
    LOG_I("sdio2 bus width: %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_BUS_WIDTH, 0));
    /* get block size */
    LOG_I("sdio2 block size: %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_FUNC_BLK_SIZE, 0));
    /* get max size */
    LOG_I("sdio dnld max size %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_DNLD_MAX_SIZE, 0));
    LOG_I("sdio upld max size %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_UPLD_MAX_SIZE, 0));

    /* print info */
    while (1) {
        if (dnld_cnt_last != dnld_cnt || upld_cnt_last != upld_cnt) {
            dnld_cnt_last = dnld_cnt;
            upld_cnt_last = upld_cnt;
            LOG_I("sdio2 test, dnld_cnt: %d, upld_cnt: %d\r\n", dnld_cnt_last, upld_cnt_last);
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

    LOG_I("sdio2_test\r\n");

    sdio2_gpio_init();

    /* sdio2 test */
    sdio2_test();

    while (1) {
        bflb_mtimer_delay_ms(5000);
        LOG_I("sdio2 test...\r\n");
    }
}
