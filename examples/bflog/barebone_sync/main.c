#define DBG_TAG "MAIN"

#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_rtc.h"
#include "board.h"
#include "bflog.h"

/*!< 添加BFLOG 标签可以使用标签过滤功能 */
/*!< 不添加则标签仍然能显示, 但标签过滤无效 */

/*!< 定义一个 BFLOG 标签 */
BFLOG_DEFINE_TAG(MAIN, DBG_TAG, true);

/*!< 取消之前的BFLOG 标签 */
#undef BFLOG_TAG

/*!< 将BFLOG 标签定义为新的标签 */
#define BFLOG_TAG BFLOG_GET_TAG(MAIN)

/*!< bflog 裸机同步例程 */
/*!< bflog barebone sync example */

struct bflb_device_s *uart0 = NULL;
static struct bflb_device_s *rtc = NULL;

/*!< 当前的UTC时间戳 2022-12-16 17:52 */
uint32_t timestamp_base = 1671184300;

/** @defgroup   example_bflog_port port
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
uint64_t bflog_clock(void)
{
    return bflb_mtimer_get_time_us();
}

uint32_t bflog_time(void)
{
    return BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)) + timestamp_base;
}

char *bflog_thread(void)
{
    return "";
}
/*---------------------------------------------------------------------------
* @}            example_bflog_port port
----------------------------------------------------------------------------*/

#define EXAMPLE_LOG_POOL_SIZE 4096

bflog_t example_recorder;
static uint32_t example_pool[EXAMPLE_LOG_POOL_SIZE / 4];
bflog_direct_stream_t example_uart_stream;

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

    /*!< 创建一个记录器, 配置内存池, 内存池大小, 模式为同步 */
    /*!< create recorder */
    if (0 != bflog_create(record, example_pool, EXAMPLE_LOG_POOL_SIZE, BFLOG_MODE_SYNC)) {
        printf("bflog_create faild\r\n");
    }

    /*!< 创建输出器, 类型为流输出器, 使能颜色输出, 互斥锁设为NULL */
    /*!< create stream direct */
    bflog_direct_create(direct, BFLOG_DIRECT_TYPE_STREAM, BFLOG_DIRECT_COLOR_ENABLE, NULL, NULL);
    /*!< 配置流输出的输出函数 */
    bflog_direct_init_stream((void *)direct, example_uart_stream_output);

    /*!< 添加输出器到记录器 */
    /*!< connect direct and recorder */
    bflog_append(record, direct);

    /*!< 恢复输出器到工作模式 */
    /*!< resume direct */
    bflog_direct_resume(direct);

    /*!< 恢复记录器到工作模式 */
    /*!< resume record */
    bflog_resume(record);
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
            bflog_global_filter("MAIN", true);
            bflog_global_filter("TEST", true);
            printf("\r\n============================== enable all output\r\n\r\n");
        } else if (test == 1) {
            bflog_global_filter("MAIN", false);
            bflog_global_filter("TEST", true);
            printf("\r\n============================== enable only MAIN output\r\n\r\n");
        } else if (test == 2) {
            bflog_global_filter("MAIN", false);
            bflog_global_filter("TEST", false);
            printf("\r\n============================== disable all output\r\n\r\n");
        }

        if (++test >= 3) {
            test = 0;
        }

        BFLOG_F(&example_recorder, "hello world this is fatal error\r\n");
        BFLOG_E(&example_recorder, "hello world this is error\r\n");
        BFLOG_W(&example_recorder, "hello world this is warning\r\n");
        BFLOG_I(&example_recorder, "hello world this is information\r\n");
        BFLOG_D(&example_recorder, "hello world this is degug information\r\n");
        BFLOG_T(&example_recorder, "hello world this is trace information\r\n");
        test_log();
        bflb_mtimer_delay_ms(1000);
    }
}
