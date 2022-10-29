#ifdef CONFIG_BFLOG
#include "bflog.h"

bflog_t __bflog_recorder;
static uint8_t bflog_pool[1024];
bflog_direct_stream_t bflog_direct_stream;

extern uint16_t __console_output(void *ptr, uint16_t size);

#endif

void log_init(void)
{
#ifdef CONFIG_BFLOG
    void *record = (void *)&__bflog_recorder;
    void *direct = (void *)&bflog_direct_stream;

    /*!< create recorder */
    bflog_create_s(record, bflog_pool, 1024, BFLOG_MODE_SYNC);

    /*!< create stream direct */
    bflog_direct_create(direct, BFLOG_DIRECT_TYPE_STREAM, BFLOG_DIRECT_COLOR_ENABLE, NULL, NULL);
    bflog_direct_init_stream_s((void *)direct, __console_output);

    /*!< connect direct and recorder */
    bflog_append_s(record, direct);

    /*!< resume direct */
    bflog_direct_resume_s(direct);

    /*!< resume record */
    bflog_resume_s(record);
#endif
}

__attribute__((weak)) void error_handler(void)
{
    volatile unsigned char dummy = 0;
    while (dummy == 0) {
    }
}
