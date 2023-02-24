#include <stdio.h>
#include <stdlib.h>
#include "./pikapython/pikascript-lib/PikaStdDevice/pika_hal.h"
#include "FreeRTOS.h"
#include "bflb_flash.h"
#include "bflb_gpio.h"
#include "bflb_uart.h"
#include "board.h"
#include "log.h"
// #include "lwip/init.h"
#include "pikaScript.h"
#include "task.h"

#if defined(BL616)
#include "bl616_glb.h"
#elif defined(BL602)
#include "bl602_glb.h"
#elif defined(BL702)
#include "bl702_glb.h"
#elif defined(BL808)
#include "bl808_glb.h"
#endif

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

uint8_t _pika_app_buf[_PIKA_APP_FLASH_SIZE] = {0};
static void consumer_task(void* pvParameters) {
    PikaObj* root = newRootObj("root", New_PikaMain);
    if (_pika_app_check()) {
        printf("Load app.pika from flash\r\n");
        FILE* f = pika_platform_fopen("app.pika", "rb");
        pika_platform_fread(_pika_app_buf, 1, _PIKA_APP_FLASH_SIZE, f);
        obj_linkLibrary(root, (uint8_t*)_pika_app_buf);
        obj_runModule(root, "main");
    } else {
        printf("Load app.pika from flash failed\r\n");
        extern unsigned char pikaModules_py_a[];
        obj_linkLibrary(root, pikaModules_py_a);
        obj_runModule(root, "main");
    }
    pikaScriptShell(root);
    while (1) {
    }
}

static void _erise_app_task(void* pvParameters) {
    struct bflb_device_s* gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_2,
                   GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
    int time = 0;
    while (1) {
        if (bflb_gpio_read(gpio, GPIO_PIN_2) == 1) {
            time++;
            if (time > 100) {
                printf("Erasing app.pika...\r\n");
                printf("Please release the button\r\n");
                for (uint32_t i = 1; i < (_PIKA_APP_FLASH_SIZE / 1024); i++) {
                    bflb_flash_erase(_PIKA_APP_FLASH_ADDR + (i - 1) * 1024,
                                     i * 1024);
                }
                printf("Erase app.pika done\r\n");
                pika_platform_reboot();
            }
        } else {
            time = 0;
        }
        vTaskDelay(10);
    }
}

int main(void) {
    board_init();
    uartx = bflb_device_get_by_name("uart0");
    bflb_uart_feature_control(uartx, UART_CMD_SET_BAUD_RATE, 115200);
    // xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    // vPortDefineHeapRegions(xHeapRegions);
    // printf("Heap size: %d\r\n", configTOTAL_HEAP_SIZE);

    xTaskCreate(_erise_app_task, (char*)"erise_app_task", 8192, NULL,
                configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(consumer_task, (char*)"consumer_task", 8192, NULL, 3, NULL);
    vTaskStartScheduler();

    while (1) {
    }
}

/* Platform Porting */

char pika_platform_getchar(void) {
    while (1) {
        int c = bflb_uart_getchar(uartx);
        if (c != -1) {
            return c;
        }
    }
}

int pika_platform_putchar(char ch) {
    bflb_uart_putchar(uartx, ch);
    return 0;
}

void pika_platform_reboot(void) {
    GLB_SW_System_Reset();
}

void* pika_platform_malloc(size_t size) {
    return pvPortMalloc(size);
}

void pika_platform_free(void* ptr) {
    return vPortFree(ptr);
}

/* fopen */
FILE* pika_platform_fopen(const char* filename, const char* modes) {
    if (strcmp("app.pika", filename) == 0) {
        g_pika_app_flash_pos = 0;
        FILE* fp = (FILE*)&g_pika_app_flash_file;
        if (strchr(modes, 'w') == NULL) {
            return fp;
        }
        for (uint32_t i = 1; i < (_PIKA_APP_FLASH_SIZE / 1024); i++) {
            bflb_flash_erase(_PIKA_APP_FLASH_ADDR + (i - 1) * 1024, i * 1024);
        }
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
