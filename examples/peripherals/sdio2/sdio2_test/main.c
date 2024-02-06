#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_sdio2.h"

#include "hardware/sdio2_reg.h"

#if defined(BL616)
#include "bl616_glb.h"
#endif

#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

// #define SDIO2_DBG(a, ...) printf(a, ##__VA_ARGS__)
#define SDIO2_DBG(a, ...)

#define SDIO2_TEST_DNLD_SIZE_MAX (2 * 1024)

/* T-L-V struct */
typedef struct {
    uint16_t tag;
    uint16_t length;
    uint8_t value[0];
} sdio_tlv_t;

volatile uint32_t dnld_cnt, upld_cnt, error_cnt;
struct bflb_device_s *sdio2_hd;
uint8_t __attribute__((aligned(32))) sdio2_test_buff[SDIO2_MAX_PORT_NUM][SDIO2_TEST_DNLD_SIZE_MAX];

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
void sdio2_dnld_irq(void *arg, bflb_sdio2_trans_desc_t *trans_desc)
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
void sdio2_upld_irq(void *arg, bflb_sdio2_trans_desc_t *trans_desc)
{
    /* upld done */
    upld_cnt++;

    /* max upld buff len */
    trans_desc->buff_len = SDIO2_TEST_DNLD_SIZE_MAX;

    SDIO2_DBG("upld done, push buff to dnld queue\r\n");

    /* push to dnld queue */
    bflb_sdio2_dnld_port_push(sdio2_hd, trans_desc);
}

/* error irq callback */
void sdio2_error_irq(void *arg, uint32_t err_code)
{
    error_cnt++;

    switch (err_code) {
        case SDIO2_ERROR_CRC:
            printf("sdio2 error: CRC ERROR!\r\n");
            break;
        case SDIO2_ERROR_ABORT:
            printf("sdio2 error: ABORT!\r\n");
            break;
        default:
            printf("sdio2 error: unkown code: %d!\r\n", err_code);
            break;
    }
}

#if defined(BL616)
/* reset irq callback */
void sdio2_reset_irq(void *arg,
                     bflb_sdio2_trans_desc_t *upld_rest_desc, uint32_t upld_rest_desc_num,
                     bflb_sdio2_trans_desc_t *dnld_rest_desc, uint32_t dnld_rest_desc_num)
{
    printf("\r\n***** sdio2 reset! *****\r\n");
    printf("upld rest num: %d, dnld rest num:%d\r\n", upld_rest_desc_num, dnld_rest_desc_num);

    /* clean cnt */
    dnld_cnt = 0;
    upld_cnt = 0;
    error_cnt = 0;

    /* link all dnld buff */
    printf("push all buff to dnld_queue\r\n");

    for (uint32_t i = 0; i < SDIO2_MAX_PORT_NUM; i++) {
        bflb_sdio2_trans_desc_t dnld_desc = {
            .buff = sdio2_test_buff[i],
            .buff_len = SDIO2_TEST_DNLD_SIZE_MAX,
            .user_arg = NULL,
        };
        bflb_sdio2_dnld_port_push(sdio2_hd, &dnld_desc);
    }
}
#endif

int main(void)
{
    uint32_t dnld_cnt_last = 0, upld_cnt_last = 0, error_cnt_last = 0;

    board_init();

#if defined(BL616)
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDU);
    /* enable sdio2 auto_reset and reset_irq */
    // GLB_Config_SDIO_Host_Reset_System(0);
    // GLB_Config_SDIO_Host_Reset_SDU(1);
    // GLB_Config_SDIO_Host_Interrupt_CPU(1);

#elif defined(BL602)

#endif

    printf("sdio2_test\r\n");

    sdio2_gpio_init();

    sdio2_hd = bflb_device_get_by_name("sdio2");
    if (sdio2_hd == NULL) {
        printf("get device failed\r\n");
        while (1) {
        }
    }

    /* sdio2 init */
    bflb_sdio2_init(sdio2_hd, SDIO2_TEST_DNLD_SIZE_MAX);

    /* wait host ready */
    printf("sdio2 wait host ready...\r\n");
    while (bflb_sdio2_check_host_ready(sdio2_hd) == 0) {
        bflb_mtimer_delay_ms(1);
    }
    printf("sdio2 host ready! \r\n");

    /* get block size */
    int block_size = bflb_sdio2_get_block_size(sdio2_hd);
    printf("sdio2 block size: %d\r\n", block_size);

    /* get max size */
    printf("sdio dnld max size %d\r\n", bflb_sdio2_get_dnld_max_size(sdio2_hd));
    printf("sdio upld max size %d\r\n", bflb_sdio2_get_upld_max_size(sdio2_hd));

    /* attach int callback */
    bflb_sdio2_dnld_irq_attach(sdio2_hd, sdio2_dnld_irq, NULL);
    bflb_sdio2_upld_irq_attach(sdio2_hd, sdio2_upld_irq, NULL);
    bflb_sdio2_error_irq_attach(sdio2_hd, sdio2_error_irq, NULL);

#if defined(BL616)
    /* attach reset callback */
    bflb_sdio2_reset_irq_attach(sdio2_hd, sdio2_reset_irq, NULL);
#endif

    bflb_l1c_dcache_clean_invalidate_range(sdio2_test_buff[0], SDIO2_MAX_PORT_NUM * SDIO2_TEST_DNLD_SIZE_MAX);

    /* link all dnld buff */
    printf("push all buff to dnld_queue\r\n");

    for (uint32_t i = 0; i < SDIO2_MAX_PORT_NUM; i++) {
        bflb_sdio2_trans_desc_t dnld_desc = {
            .buff = sdio2_test_buff[i],
            .buff_len = SDIO2_TEST_DNLD_SIZE_MAX,
            .user_arg = NULL,
        };
        bflb_sdio2_dnld_port_push(sdio2_hd, &dnld_desc);
    }

    printf("wait host....\r\n");

    while (1) {
        SDIO2_DBG("dnld bitmap 0x%04X....\r\n", getreg16(sdio2_hd->reg_base + SDIO2_WR_BIT_MAP_OFFSET));
        SDIO2_DBG("upld bitmap 0x%04X....\r\n", getreg16(sdio2_hd->reg_base + SDIO2_RD_BIT_MAP_OFFSET));

        for(uint32_t i=0; i<200; i++){
            intptr_t flag = bflb_irq_save();
            bflb_mtimer_delay_ms(1);
            bflb_irq_restore(flag);
            bflb_mtimer_delay_ms(1);
        }

        if (dnld_cnt_last != dnld_cnt || upld_cnt_last != upld_cnt) {
            dnld_cnt_last = dnld_cnt;
            upld_cnt_last = upld_cnt;
            printf("sdio2 test, dnld_cnt: %d, upld_cnt: %d\r\n", dnld_cnt_last, upld_cnt_last);
        }

        if (error_cnt_last != error_cnt) {
            error_cnt_last = error_cnt;
            printf("sdio error! error_cnt: %d\r\n", error_cnt_last);
        }

        // bflb_mtimer_delay_ms(500);
    }
}
