#include <stdio.h>

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include <task.h>
#endif

#include "adckey.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "board.h"

#if defined(BL618DG)
#include "bflb_adc_v3.h"
#define DEMO_KEY_GPIO_PIN     GPIO_PIN_13
#define DEMO_KEY_ADC_CHANNEL  ADC_EXTERNAL_CHANNEL_1
#define DEMO_AUX_GPIO_PIN     GPIO_PIN_28
#define DEMO_AUX_ADC_CHANNEL  ADC_EXTERNAL_CHANNEL_4
#define DEMO_DRIVER_NAME      "ADC_V3"
#elif defined(BL616CL)
#include "bflb_adc_v2.h"
#define DEMO_KEY_GPIO_PIN     GPIO_PIN_5
#define DEMO_KEY_ADC_CHANNEL  ADC_EXTERNAL_CHANNEL_5
#define DEMO_AUX_GPIO_PIN     GPIO_PIN_4
#define DEMO_AUX_ADC_CHANNEL  ADC_EXTERNAL_CHANNEL_4
#define DEMO_DRIVER_NAME      "ADC_V2"
#else
#include "bflb_adc.h"
#define DEMO_KEY_GPIO_PIN     GPIO_PIN_20
#define DEMO_KEY_ADC_CHANNEL  ADC_CHANNEL_0
#define DEMO_AUX_GPIO_PIN     GPIO_PIN_21
#define DEMO_AUX_ADC_CHANNEL  ADC_CHANNEL_1
#define DEMO_DRIVER_NAME      "ADC"
#endif

#define DEMO_KEY_PERIOD_MS (50u)
#define DEMO_AUX_PERIOD_MS (1000u)
#define DEMO_LONG_PRESS_MS (1000u)

#if defined(CONFIG_ADCKEY_DEFAULT_LOW)
static const uint16_t g_key_target_mv[] = { 600, 1200, 1800, 2400 };
#else
static const uint16_t g_key_target_mv[] = { 2370, 1650, 1100, 0 };
#endif

static adckey_handle_t g_adckey;

static void adckey_demo_key_callback(const adckey_event_data_t *event_data)
{
    const char *event_name;

    if (event_data == NULL) {
        return;
    }

    switch (event_data->event) {
#if defined(CONFIG_ADCKEY_SUPPORT_PRESS)
        case ADCKEY_EVENT_PRESS:
            event_name = "PRESS";
            break;
#endif
        case ADCKEY_EVENT_LONG_PRESS:
            event_name = "LONG_PRESS";
            break;
        case ADCKEY_EVENT_RELEASE:
            event_name = "RELEASE";
            break;
        default:
            event_name = "UNKNOWN";
            break;
    }

    printf("adckey: item=%u key=%u event=%s sampled=%umV\r\n",
           (unsigned)event_data->item_id,
           (unsigned)event_data->key_id,
           event_name,
           (unsigned)event_data->sampled_mv);
}

static void adckey_demo_adc_callback(const adckey_adc_data_t *adc_data)
{
    if (adc_data == NULL) {
        return;
    }

    printf("adckey: adc item=%u sampled=%umV\r\n",
           (unsigned)adc_data->item_id,
           (unsigned)adc_data->sampled_mv);
}

static const adckey_item_config_t g_adckey_items[] = {
    {
        .type = ADCKEY_ITEM_TYPE_KEY,
        .gpio_pin = DEMO_KEY_GPIO_PIN,
        .adc_channel = DEMO_KEY_ADC_CHANNEL,
        .sample_period_ms = DEMO_KEY_PERIOD_MS,
        .config.key =
            {
                .target_mv = g_key_target_mv,
                .key_num = sizeof(g_key_target_mv) / sizeof(g_key_target_mv[0]),
                .long_press_ms = DEMO_LONG_PRESS_MS,
                .callback = adckey_demo_key_callback,
            },
    },
    {
        .type = ADCKEY_ITEM_TYPE_ADC,
        .gpio_pin = DEMO_AUX_GPIO_PIN,
        .adc_channel = DEMO_AUX_ADC_CHANNEL,
        .sample_period_ms = DEMO_AUX_PERIOD_MS,
        .config.adc =
            {
                .callback = adckey_demo_adc_callback,
            },
    },
};

static const adckey_config_t g_adckey_cfg = {
#if defined(CONFIG_FREERTOS)
    .task_stack_size = 1024,
    .task_priority = 5,
#endif
    .items = g_adckey_items,
    .item_num = sizeof(g_adckey_items) / sizeof(g_adckey_items[0]),
};

#if defined(CONFIG_FREERTOS)
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    while (1) {}
}

void vAssertCalled(void)
{
    taskDISABLE_INTERRUPTS();
    while (1) {}
}
#endif

int main(void)
{
    board_init();

    printf("adc_key_basic demo using %s\r\n", DEMO_DRIVER_NAME);
    printf("item0: key gpio=%u channel=%u period=%u ms\r\n",
           (unsigned)DEMO_KEY_GPIO_PIN,
           (unsigned)DEMO_KEY_ADC_CHANNEL,
           (unsigned)DEMO_KEY_PERIOD_MS);
    printf("item1: adc gpio=%u channel=%u period=%u ms\r\n",
           (unsigned)DEMO_AUX_GPIO_PIN,
           (unsigned)DEMO_AUX_ADC_CHANNEL,
           (unsigned)DEMO_AUX_PERIOD_MS);
#if defined(CONFIG_ADCKEY_DEFAULT_LOW)
    printf("Configure key target_mv in ascending order because idle voltage is 0mV.\r\n");
#else
    printf("Configure key target_mv in descending order because idle voltage is 3200mV.\r\n");
#endif

    if (adckey_init(&g_adckey_cfg, &g_adckey) != 0) {
        printf("adckey_init failed\r\n");
        while (1) {
            bflb_mtimer_delay_ms(1000);
        }
    }

    if (adckey_start(g_adckey) != 0) {
        printf("adckey_start failed\r\n");
        while (1) {
            bflb_mtimer_delay_ms(1000);
        }
    }

#if defined(CONFIG_FREERTOS)
    printf("adckey FreeRTOS mode started\r\n");
    vTaskStartScheduler();
    while (1) {}
#else
    printf("adckey baremetal poll mode started\r\n");
    while (1) {
        (void)adckey_poll(g_adckey);
        bflb_mtimer_delay_ms(1);
    }
#endif
}
