#define DBG_TAG "MAIN"

#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_rtc.h"
#include "board.h"
#include "bflb_log.h"

/*!< Adding the BFLB_LOG tag allows the use of tag filtering functionality */
/*!< If not added, the tag can still be displayed, but tag filtering will not work */

/*!< Define a BFLB_LOG tag */
BFLB_LOG_DEFINE_TAG(MAIN, DBG_TAG, true);

/*!< Cancel the previous BFLB_LOG tag */
#undef BFLB_LOG_TAG

/*!< Define the BFLB_LOG tag as a new tag */
#define BFLB_LOG_TAG BFLB_LOG_GET_TAG(MAIN)

/*!< Bflog bare-metal synchronization routine */
/*!< bflb_log barebone sync example */

struct bflb_device_s *uart0 = NULL;
static struct bflb_device_s *rtc = NULL;

/*!< Current UTC Timestamp 2022-12-16 17:52 */
uint32_t timestamp_base = 1671184300;

/** @defgroup   example_bflb_log_port port
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
uint64_t bflb_log_clock(void)
{
    return bflb_mtimer_get_time_us();
}

uint32_t bflb_log_time(void)
{
    return BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)) + timestamp_base;
}

char *bflb_log_thread(void)
{
    return "";
}
/*---------------------------------------------------------------------------
* @}            example_bflb_log_port port
----------------------------------------------------------------------------*/

#define EXAMPLE_LOG_POOL_SIZE 4096

bflb_log_t example_recorder;
static uint32_t example_pool[EXAMPLE_LOG_POOL_SIZE / 4];
bflb_log_direct_stream_t example_uart_stream;

uint16_t example_uart_stream_output(void *ptr, uint16_t size)
{
    for (size_t i = 0; i < size; i++) {
        bflb_uart_putchar(uart0, ((char *)ptr)[i]);
    }
    return size;
}

void example_log_init(void)
{
    void *record = (void *)&example_recorder;
    void *direct = (void *)&example_uart_stream;

    /*!< Create a logger, configure the memory pool, set the memory pool size, and set the mode to synchronous.*/
    /*!< create recorder */
    if (0 != bflb_log_create(record, example_pool, EXAMPLE_LOG_POOL_SIZE, BFLB_LOG_MODE_SYNC)) {
        printf("bflb_log_create faild\r\n");
    }

    /*!< Create an outputter, of the stream outputter type, enable color output, and set the mutex to NULL. */
    /*!< create stream direct */
    bflb_log_direct_create(direct, BFLB_LOG_DIRECT_TYPE_STREAM, BFLB_LOG_DIRECT_COLOR_ENABLE, NULL, NULL);
    /*!< Configure the output function of the stream output */
    bflb_log_direct_init_stream((void *)direct, example_uart_stream_output);

    /*!< Add the outputter to the logger */
    /*!< connect direct and recorder */
    bflb_log_append(record, direct);

    /*!< Restore the outputter to working mode*/
    /*!< resume direct */
    bflb_log_direct_resume(direct);

    /*!< Restore the logger to working mode */
    /*!< resume record */
    bflb_log_resume(record);
}

extern void test_log(void);

int main(void)
{
    board_init();

    /*!< uart0 already initialized in bsp/board */
    uart0 = bflb_device_get_by_name("uart0");
    rtc = bflb_device_get_by_name("rtc");
    bflb_rtc_set_time(rtc, 0);

    example_log_init();

    uint8_t test = 0;

    while (1) {
        if (test == 0) {
            bflb_log_global_filter("MAIN", true);
            bflb_log_global_filter("TEST", true);
            printf("\r\n============================== enable all output\r\n\r\n");
        } else if (test == 1) {
            bflb_log_global_filter("MAIN", false);
            bflb_log_global_filter("TEST", true);
            printf("\r\n============================== enable only MAIN output\r\n\r\n");
        } else if (test == 2) {
            bflb_log_global_filter("MAIN", false);
            bflb_log_global_filter("TEST", false);
            printf("\r\n============================== disable all output\r\n\r\n");
        }

        if (++test >= 3) {
            test = 0;
        }

        BFLB_LOG_F(&example_recorder, "hello world this is fatal error\r\n");
        BFLB_LOG_E(&example_recorder, "hello world this is error\r\n");
        BFLB_LOG_W(&example_recorder, "hello world this is warning\r\n");
        BFLB_LOG_I(&example_recorder, "hello world this is information\r\n");
        BFLB_LOG_D(&example_recorder, "hello world this is degug information\r\n");
        BFLB_LOG_T(&example_recorder, "hello world this is trace information\r\n");
        test_log();
        bflb_mtimer_delay_ms(1000);
    }
}
