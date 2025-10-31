#include "bflb_mtimer.h"
#include "bflb_irq.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

static int factorial(int n)
{
    if (DBG_BOOL(n <= 1)) {
        return DBG_VALUE(1);
    } else {
        return DBG_VALUE(n * factorial(n - 1));
    }
}

int main(void)
{
    board_init();

    char message[] = "hellokszhdfoiasjdkjnskjxnvuiolashdfoinaskjldfnvklasjhdfi213489o71234589073298dfl;asjdlfnkjzxncvhasdlkfljhasjkldjhnvjkaslndfvkljashdlifhui";

    /*!< All DBG macros will return the variables passed in */

    /*!< Print various types of variables using DBG_VALUE, type adaptive */
    DBG_VALUE(message);
    /*!< Use DBG_HEXDUMP to print data in canonical format, type adaptive */
    DBG_HEXDUMP(message, sizeof(message));

    const int a = 2;
    const int b = DBG_VALUE(3 * a) + 1;

    /*!< Use DBG_ARRAY to print arrays, type adaptive */
    int numbers[512] = { b, 13 };
    DBG_ARRAY(numbers, 512);

    DBG_VALUE(factorial(4));

    /*!< Printing Boolean expressions using DBG_BOOL */
    DBG_BOOL(1 == factorial(4));
    DBG_BOOL(24 == factorial(4));
    DBG_BOOL(1 == 0);
    DBG_BOOL(1 == 1);

    /*!< Assert whether the expression is true or not */
    _ASSERT_PARAM(1 == factorial(4));
    _ASSERT_FUNC(1 == factorial(4));
    /*!< Assert whether the expression is true or not */
    _ASSERT_TRUE_PARAM(1 == factorial(4));
    _ASSERT_TRUE_FUNC(1 == factorial(4));
    /*!< Assert whether the expression is false or not */
    _ASSERT_FALSE_PARAM(24 == factorial(4));
    _ASSERT_FALSE_FUNC(24 == factorial(4));
    /*!< Assert whether the expression is 0 or not */
    _ASSERT_ZERO_PARAM(factorial(4));
    _ASSERT_ZERO_FUNC(factorial(4));
    /*!< Assert whether two values are equal */
    _ASSERT_EQUAL_PARAM(1, factorial(4));
    _ASSERT_EQUAL_FUNC(1, factorial(4));

    while (1) {
        LOG_F("hello world fatal\r\n");
        LOG_E("hello world error\r\n");
        LOG_W("hello world warning\r\n");
        LOG_I("hello world information\r\n");
        LOG_D("hello world debug\r\n");
        LOG_T("hello world trace\r\n");
        LOG_RF("hello world fatal raw\r\n");
        LOG_RE("hello world error raw\r\n");
        LOG_RW("hello world warning raw\r\n");
        LOG_RI("hello world information raw\r\n");
        LOG_RD("hello world debug raw\r\n");
        LOG_RT("hello world trace raw\r\n");
        bflb_mtimer_delay_ms(1000);
    }
}

/*!< You can leave this function undefined */
/*!< and use the weak default handler */
void error_handler(void)
{
    /*!< assertion faild handler */
    printf("error handler\r\n");

    /*!< For testing purposes only, the following  */
    /*!< comments should be uncommented under normal  */
    /*!< circumstances, so that the MCU is stuck */

    // uintptr_t irq = bflb_irq_save();
    // volatile unsigned char dummy = 0;
    // while (dummy == 0) {
    // }
    // bflb_irq_restore(irq);
}