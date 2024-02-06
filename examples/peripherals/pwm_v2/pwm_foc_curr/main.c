#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_adc.h"
#include "bflb_dma.h"
#include "bflb_clock.h"
#include "board.h"
#include "math.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_gpio.h"
#include "../foc.h"

#define PWM_CH_U   (PWM_CH2)
#define PWM_CH_V   (PWM_CH3)
#define PWM_CH_W   (PWM_CH0)
#define PWM_PIN_UH (GPIO_PIN_28)
#define PWM_PIN_UL (GPIO_PIN_29)
#define PWM_PIN_VH (GPIO_PIN_30)
#define PWM_PIN_VL (GPIO_PIN_31)
#define PWM_PIN_WH (GPIO_PIN_32)
#define PWM_PIN_WL (GPIO_PIN_33)
#define HALL_PIN_U (GPIO_PIN_11)
#define HALL_PIN_V (GPIO_PIN_15)
#define HALL_PIN_W (GPIO_PIN_18)
#define CURR_CH_U  (ADC_CHANNEL_1)
#define CURR_CH_V  (ADC_CHANNEL_2)
#define CURR_CH_W  (ADC_CHANNEL_3)
#define CURR_PIN_U (GPIO_PIN_19)
#define CURR_PIN_V (GPIO_PIN_2)
#define CURR_PIN_W (GPIO_PIN_3)

struct bflb_adc_channel_s adc_chans[] = {
    {
        .pos_chan = CURR_CH_U,
        .neg_chan = ADC_CHANNEL_GND,
    },
    {
        .pos_chan = CURR_CH_V,
        .neg_chan = ADC_CHANNEL_GND,
    },
    {
        .pos_chan = CURR_CH_W,
        .neg_chan = ADC_CHANNEL_GND,
    }
};

struct bflb_device_s *pwm;
struct bflb_device_s *gpio;
struct bflb_device_s *adc;
struct bflb_device_s *dma0_ch0;
struct foc_s foc;
ATTR_NOCACHE_NOINIT_RAM_SECTION uint32_t adc_val[3];
struct bflb_dma_channel_lli_pool_s lli[1]; /* max trasnfer size 4064 * 1 */
struct bflb_dma_channel_lli_transfer_s transfer[1] = {
    {
        .src_addr = (uint32_t)DMA_ADDR_ADC_RDR,
        .dst_addr = (uint32_t)adc_val,
        .nbytes = sizeof(adc_val) / sizeof(adc_val[0]) * 4,
    },
};

void delay(void)
{
    uint32_t i = 500;
    while (i--) {};
}

void dma0_ch0_isr(void *arg)
{
    csi_dcache_clean_invalid_range(adc_val, sizeof(adc_val));
    // printf("0x%08X, 0x%08X, 0x%08X\n", adc_val[0], adc_val[1], adc_val[2]);
    foc_hall_calc_angle(&foc.hall);
    foc.svpwm.angle = foc.hall.angle + 16384; /* add 90 degree */
    foc_svpwm_output(&foc.svpwm);
    foc.curr.adc_u = (adc_val[0] & 0xFFFF) >> 4;
    foc.curr.adc_v = (adc_val[1] & 0xFFFF) >> 4;
    foc.curr.adc_w = (adc_val[2] & 0xFFFF) >> 4;
    // foc_curr_calc_abc(&foc.curr);
    // foc_curr_calc_clark(&foc.curr);
    // foc_curr_calc_park(&foc.curr);
    // foc_curr_filter(&foc);
    bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfer, 1);
    bflb_dma_channel_start(dma0_ch0);
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(foc.curr.adc_u / 1000 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)((foc.curr.adc_u % 1000) / 100 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)((foc.curr.adc_u % 100) / 10 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(foc.curr.adc_u % 10 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(',');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(foc.curr.adc_v / 1000 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)((foc.curr.adc_v % 1000) / 100 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)((foc.curr.adc_v % 100) / 10 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(foc.curr.adc_v % 10 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(',');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(foc.curr.adc_w / 1000 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)((foc.curr.adc_w % 1000) / 100 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)((foc.curr.adc_w % 100) / 10 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)(foc.curr.adc_w % 10 + '0');
    while (((*(volatile uint32_t *)0x2000A084) & 0x3F) == 0) {}
    *(volatile uint32_t *)0x2000A088 = (uint32_t)('\n');
}

void svpwm_bindto_bl616(void)
{
    uint16_t center = FOC_PWM_PERIOD_VALUE >> 1;
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH_U, center - foc.svpwm.u, center + foc.svpwm.u);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH_V, center - foc.svpwm.v, center + foc.svpwm.v);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH_W, center - foc.svpwm.w, center + foc.svpwm.w);
}

void svpwm_close_pwm(void)
{
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_CH_U);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_CH_V);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_CH_W);
    bflb_pwm_v2_channel_negative_stop(pwm, PWM_CH_U);
    bflb_pwm_v2_channel_negative_stop(pwm, PWM_CH_V);
    bflb_pwm_v2_channel_negative_stop(pwm, PWM_CH_W);
}

void peri_init(void)
{
    struct bflb_pwm_v2_config_s pwm_cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
        .clk_div = 1,
        .period = FOC_PWM_PERIOD_VALUE,
    };
    struct bflb_pwm_v2_channel_config_s pwm_ch_cfg = {
        .positive_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .negative_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .positive_stop_state = PWM_STATE_INACTIVE,
        .negative_stop_state = PWM_STATE_ACTIVE,
        .positive_brake_state = PWM_STATE_INACTIVE,
        .negative_brake_state = PWM_STATE_ACTIVE,
        .dead_time = FOC_PWM_DEAD_TIME,
    };
    struct bflb_adc_config_s adc_cfg = {
        .clk_div = ADC_CLK_DIV_16,
        .scan_conv_mode = true,
        .continuous_conv_mode = false,
        .differential_mode = false,
        .resolution = ADC_RESOLUTION_12B,
        .vref = ADC_VREF_3P2V,
    };
    struct bflb_dma_channel_config_s dma_cfg = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_ADC,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    /* config hall */
    bflb_gpio_init(gpio, HALL_PIN_U, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, HALL_PIN_V, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, HALL_PIN_W, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    /* config adc */
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 4);
    bflb_gpio_init(gpio, CURR_PIN_U, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, CURR_PIN_V, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, CURR_PIN_W, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_adc_init(adc, &adc_cfg);
    bflb_adc_channel_config(adc, adc_chans, sizeof(adc_chans) / sizeof(adc_chans[0]));
    bflb_adc_link_rxdma(adc, true);
    *(volatile uint32_t *)0x2000F910 |= (1 << 11);
    bflb_dma_channel_init(dma0_ch0, &dma_cfg);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
    bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfer, 1);
    bflb_dma_channel_start(dma0_ch0);
    /* config pwm */
    bflb_gpio_init(gpio, PWM_PIN_UH, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_UL, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_VH, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_VL, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_WH, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_WL, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    GLB_Set_PWM1_IO_Sel(GLB_PWM1_IO_DIFF_END);
    bflb_pwm_v2_init(pwm, &pwm_cfg);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_TRIG_ADC_SRC, PWM_TRIG_ADC_SRC_PERIOD);
    bflb_pwm_v2_channel_init(pwm, PWM_CH_U, &pwm_ch_cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_CH_V, &pwm_ch_cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_CH_W, &pwm_ch_cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH_U, 0, 0);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH_V, 0, 0);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH_W, 0, 0);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH_U);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH_U);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH_V);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH_V);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH_W);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH_W);
    foc_svpwm_install_callback_set_pwm(&foc.svpwm, svpwm_bindto_bl616);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_SW_BRAKE_ENABLE, true);
    bflb_pwm_v2_start(pwm);
    arch_delay_ms(2000);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_SW_BRAKE_ENABLE, false);
    bflb_irq_enable(pwm->irq_num);
}

uint8_t hall_get(void)
{
    uint8_t hall = 0;
    if (bflb_gpio_read(gpio, HALL_PIN_U)) {
        hall |= (1 << 2);
    }
    if (bflb_gpio_read(gpio, HALL_PIN_V)) {
        hall |= (1 << 1);
    }
    if (bflb_gpio_read(gpio, HALL_PIN_W)) {
        hall |= (1 << 0);
    }
    return hall;
}

int main(void)
{
    board_init();
    pwm = bflb_device_get_by_name("pwm_v2_0");
    gpio = bflb_device_get_by_name("gpio");
    adc = bflb_device_get_by_name("adc");
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    foc_hall_install_callback_delay_1us(&foc.hall, delay);
    foc_hall_install_callback_read_hall_pin(&foc.hall, hall_get);
    foc_hall_init(&foc.hall);
    foc.svpwm.angle = foc.hall.angle + 16384; /* add 90 degree */
    foc_svpwm_init(&foc.svpwm);
    peri_init();
    while (1) {
        // bflb_adc_start_conversion(adc);
        // arch_delay_ms(500);
    }
}
