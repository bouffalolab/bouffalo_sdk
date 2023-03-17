#include <stdio.h>
#include <stdlib.h>
#include "./pikapython/pikascript-lib/PikaStdDevice/pika_hal.h"
#include "bflb_flash.h"
#include "bflb_gpio.h"
#include "bflb_uart.h"
#include "board.h"
#include "log.h"
#include "usbd_cdc_user.h"
// #include "lwip/init.h"
#include "pikaScript.h"
#if PIKA_FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#endif

#if defined(BL616)
#include "bl616_glb.h"
#elif defined(BL602)
#include "bl602_glb.h"
#elif defined(BL702)
#include "bl702_glb.h"
#elif defined(BL808)
#include "bl808_glb.h"
#endif

#define REPL_UART0 0
#define REPL_USB 1

#define REPL_PORT REPL_USB

struct bflb_device_s* uartx = NULL;
// static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

volatile FILE g_pika_app_flash_file = {0};
volatile int g_pika_app_flash_pos = 0;
#define _PIKA_APP_FLASH_ADDR 0x100000   // 1M
#define _PIKA_APP_FLASH_SIZE 32 * 1024  // 32K

#define _PIKA_APP_FLASH_INITED 0xFE
#define _PIKA_APP_FLASH_VOID 0xFF
#define _PIKA_APP_FLASH_SAVED 0x0F

// static HeapRegion_t xHeapRegions[] = {
//     {(uint8_t*)freertos_heap, 0},
//     {NULL, 0}, /* Terminates the array. */
//     {NULL, 0}  /* Terminates the array. */
// };

static int _pika_app_check(void) {
    uint8_t buf = {0};
    FILE* f = pika_platform_fopen("app.pika", "rb");
    pika_platform_fread(&buf, 1, 1, f);
    if (buf == _PIKA_APP_FLASH_SAVED) {
        return 1;
    }
    return 0;
}

#define ERAISE_BATCH_SIZE (_PIKA_APP_FLASH_SIZE / 1)

static int _do_eraise_app(void){
    for (uint32_t i = 0; i < (_PIKA_APP_FLASH_SIZE / ERAISE_BATCH_SIZE); i++) {
        int ret = 0;
        ret = bflb_flash_erase(
            _PIKA_APP_FLASH_ADDR + i * ERAISE_BATCH_SIZE,
            ERAISE_BATCH_SIZE);
        if (ret != 0) {
            pika_platform_printf("Erase app.pika failed\r\n");
            return -1;
        }
    }
    return 0;
}

static void _eraise_app(void){
    pika_platform_printf("Erasing app.pika...\r\n");
    pika_platform_printf("Please release the button\r\n");
    _do_eraise_app();
    pika_platform_printf("Erase app.pika done\r\n");
    pika_platform_reboot();
}

uint8_t _pika_app_buf[_PIKA_APP_FLASH_SIZE] = {0};
static void consumer_task(void* pvParameters) {
    cdc_acm_init();
    vTaskDelay(1000);
    struct bflb_device_s* gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_33,
                   GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    vTaskDelay(100);
    if (bflb_gpio_read(gpio, GPIO_PIN_33) == 0){
        _eraise_app();
    }

    PikaObj* root = newRootObj("root", New_PikaMain);
    if (_pika_app_check()) {
        pika_platform_printf("Load app.pika from flash\r\n");
        FILE* f = pika_platform_fopen("app.pika", "rb");
        pika_platform_fread(_pika_app_buf, 1, _PIKA_APP_FLASH_SIZE, f);
        obj_linkLibrary(root, (uint8_t*)_pika_app_buf);
        obj_runModule(root, "main");
    } else {
        pika_platform_printf("Load app.pika from flash failed\r\n");
        extern unsigned char pikaModules_py_a[];
        obj_linkLibrary(root, pikaModules_py_a);
        obj_runModule(root, "main");
    }
    pikaScriptShell(root);
    while (1) {
    }
}

static void _erase_app_task(void* pvParameters) {
    struct bflb_device_s* gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_2,
                   GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
    int time = 0;
    while (1) {
        if (bflb_gpio_read(gpio, GPIO_PIN_2) == 1) {
            time++;
            if (time > 100) {
                _eraise_app();
            }
        } else {
            time = 0;
        }
        vTaskDelay(10);
    }
}

static void usb_cdc_fflush_task(void* pvParameters) {
    while (1) {
        vTaskDelay(30);
        usb_cdc_fflush();
    }
}

int main(void) {
    board_init();
    #if REPL_PORT == REPL_UART0
    uartx = bflb_device_get_by_name("uart0");
    bflb_uart_feature_control(uartx, UART_CMD_SET_BAUD_RATE, 115200);
    #endif
    // xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    // vPortDefineHeapRegions(xHeapRegions);
    // printf("Heap size: %d\r\n", configTOTAL_HEAP_SIZE);

#if PIKA_FREERTOS_ENABLE
    xTaskCreate(_erase_app_task, (char*)"erase_app_task", 8192, NULL,
                configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(consumer_task, (char*)"consumer_task", 8 * 1024, NULL, 3, NULL);
    xTaskCreate(usb_cdc_fflush_task, (char*)"usb_cdc_fflush_task", 1024, NULL,
                1, NULL);
    vTaskStartScheduler();
#else
    consumer_task(NULL);
#endif

    while (1) {
    }
}

/* Platform Porting */

char pika_platform_getchar(void) {
    #if REPL_PORT == REPL_UART0
    while (1) {
        int c = bflb_uart_getchar(uartx);
        if (c != -1) {
            return c;
        }
    }
    #elif REPL_PORT == REPL_USB
    return usb_cdc_user_getchar();
    #endif
}

int pika_platform_putchar(char ch) {
    #if REPL_PORT == REPL_UART0
    bflb_uart_putchar(uartx, ch);
    return 0;
    #elif REPL_PORT == REPL_USB
    return usb_cdc_user_putchar(ch);
    #endif
}

void pika_platform_reboot(void) {
    GLB_SW_System_Reset();
}

void* pika_platform_malloc(size_t size) {
#if PIKA_FREERTOS_ENABLE
    return pvPortMalloc(size);
#else
    return malloc(size);
#endif
}

void pika_platform_free(void* ptr) {
#if PIKA_FREERTOS_ENABLE
    return vPortFree(ptr);
#else
    free(ptr);
#endif
}

/* fopen */
FILE* pika_platform_fopen(const char* filename, const char* modes) {
    if (strcmp("app.pika", filename) == 0) {
        g_pika_app_flash_pos = 0;
        FILE* fp = (FILE*)&g_pika_app_flash_file;
        if (strchr(modes, 'w') == NULL) {
            return fp;
        }
        _do_eraise_app();
        return fp;
    }
    return NULL;
}

/* fwrite */
size_t pika_platform_fwrite(const void* ptr,
                            size_t size,
                            size_t n,
                            FILE* stream) {
    if (stream == (FILE*)&g_pika_app_flash_file) {
        bflb_flash_write(_PIKA_APP_FLASH_ADDR + g_pika_app_flash_pos,
                         (uint8_t*)ptr, size * n);
        g_pika_app_flash_pos += size * n;
        return size * n;
    }
    return 0;
}

/* fread */
size_t pika_platform_fread(void* ptr, size_t size, size_t n, FILE* stream) {
    if (stream == (FILE*)&g_pika_app_flash_file) {
        bflb_flash_read(_PIKA_APP_FLASH_ADDR + g_pika_app_flash_pos,
                        (uint8_t*)ptr, size * n);
        g_pika_app_flash_pos += size * n;
        return size * n;
    }
    return 0;
}

/* fclose */
int pika_platform_fclose(FILE* stream) {
    if (stream == (FILE*)&g_pika_app_flash_file) {
        for (uint32_t i = 1; i < 32; i++) {
            /* add EOF */
            pika_platform_fwrite("\0", 1, 1, stream);
        }
        return 0;
    }
    return -1;
}
