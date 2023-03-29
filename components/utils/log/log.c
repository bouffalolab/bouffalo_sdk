#include "bflb_uart.h"

#ifdef CONFIG_BFLOG
#include "bflog.h"

#ifndef CONFIG_LOG_POOL_SIZE
#define CONFIG_LOG_POOL_SIZE 1024
#endif

bflog_t __bflog_recorder;
void *__bflog_recorder_pointer = &__bflog_recorder;
static uint8_t bflog_pool[CONFIG_LOG_POOL_SIZE];
bflog_direct_stream_t bflog_direct_stream;

extern struct bflb_device_s *console;
static uint16_t console_output(void *ptr, uint16_t size)
{
    for (size_t i = 0; i < size; i++) {
        bflb_uart_putchar(console, ((char *)ptr)[i]);
    }
    return size;
}

#endif

void log_start(void)
{
#if defined(CONFIG_NEWLIB) && CONFIG_NEWLIB
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#endif

#ifdef CONFIG_BFLOG
    void *record = (void *)&__bflog_recorder;
    void *direct = (void *)&bflog_direct_stream;

    /*!< create recorder */
    bflog_create(record, bflog_pool, CONFIG_LOG_POOL_SIZE, BFLOG_MODE_SYNC);

    /*!< create stream direct */
    bflog_direct_create(direct, BFLOG_DIRECT_TYPE_STREAM, BFLOG_DIRECT_COLOR_ENABLE, NULL, NULL);
    bflog_direct_init_stream((void *)direct, console_output);

    /*!< connect direct and recorder */
    bflog_append(record, direct);

    /*!< resume direct */
    bflog_direct_resume(direct);

    /*!< resume record */
    bflog_resume(record);
#endif
}

__attribute__((weak)) void error_handler(void)
{
    volatile unsigned char dummy = 0;
    while (dummy == 0) {
    }
}
