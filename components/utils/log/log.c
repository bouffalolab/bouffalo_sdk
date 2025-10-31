#include "bflb_uart.h"

#ifdef CONFIG_BFLB_LOG
#include "bflb_log.h"

#ifndef CONFIG_LOG_POOL_SIZE
#define CONFIG_LOG_POOL_SIZE 1024
#endif

bflb_log_t __bflb_log_recorder;
void *__bflb_log_recorder_pointer = &__bflb_log_recorder;
static uint8_t bflb_log_pool[CONFIG_LOG_POOL_SIZE];
bflb_log_direct_stream_t bflb_log_direct_stream;

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
#if defined(CONFIG_NEWLIB)
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#endif

#ifdef CONFIG_BFLB_LOG
    void *record = (void *)&__bflb_log_recorder;
    void *direct = (void *)&bflb_log_direct_stream;

    /*!< create recorder */
    bflb_log_create(record, bflb_log_pool, CONFIG_LOG_POOL_SIZE, BFLB_LOG_MODE_SYNC);

    /*!< create stream direct */
    bflb_log_direct_create(direct, BFLB_LOG_DIRECT_TYPE_STREAM, BFLB_LOG_DIRECT_COLOR_ENABLE, NULL, NULL);
    bflb_log_direct_init_stream((void *)direct, console_output);

    /*!< connect direct and recorder */
    bflb_log_append(record, direct);

    /*!< resume direct */
    bflb_log_direct_resume(direct);

    /*!< resume record */
    bflb_log_resume(record);
#endif
}

__attribute__((weak)) void error_handler(void)
{
    volatile unsigned char dummy = 0;
    while (dummy == 0) {
    }
}
