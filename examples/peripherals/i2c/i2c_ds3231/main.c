#include "bflb_mtimer.h"
#include "ds3231.h"
#include "board.h"
#include "bflb_uart.h"
#include "shell.h"
int main(void)
{
    uint8_t year, month, date, week, hour, min, sec;

    board_init();
    board_i2c0_gpio_init();

    static struct bflb_device_s *uart0;
    uart0 = bflb_device_get_by_name("uart0");
    shell_init();

    ds3231_init();

    printf("DS3231 time reading started\r\n");
    int ch;
    uint8_t t = 0;
    while(1){
        if((ch = bflb_uart_getchar(uart0)) != -1)
        {
            shell_handler(ch);
        }
        if(t++ == 50){
        t = 0;
            ds3231_get_time(&year, &month, &date, &week, &hour, &min, &sec);

            printf("Time: 20%02d-%02d-%02d %s %02d:%02d:%02d\r\n",
                year, month, date,
                (week == 0) ? "Sun" : (week == 1) ? "Mon" : (week == 2) ? "Tue" :
                (week == 3) ? "Wed" : (week == 4) ? "Thu" : (week == 5) ? "Fri" : "Sat",
                hour, min, sec);
        }
        bflb_mtimer_delay_ms(100);
    }
}

int shell_set_time(int argc, char **argv)
{
    if (argc != 8) {
        printf("Usage: set_time <year> <month> <day> <weekday> <hour> <min> <sec>\n");
        printf("Example: set_time 26 3 10 1 14 30 45\n");
        return -1;
    }

    uint8_t year = atoi(argv[1]);
    uint8_t month = atoi(argv[2]);
    uint8_t day = atoi(argv[3]);
    uint8_t weekday = atoi(argv[4]);
    uint8_t hour = atoi(argv[5]);
    uint8_t min = atoi(argv[6]);
    uint8_t sec = atoi(argv[7]);

    ds3231_set_time(year, month, day, weekday, hour, min, sec);
    printf("Time set: 20%02d-%02d-%02d %s %02d:%02d:%02d\r\n",year, month, day,
            (weekday == 0) ? "Sun" :(weekday == 1) ? "Mon" :(weekday == 2) ? "Tue" :
            (weekday == 3) ? "Wed" :(weekday == 4) ? "Thu" :(weekday == 5) ? "Fri" :"Sat",
            hour, min, sec);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_set_time, set_time, Set DS3231 time);
