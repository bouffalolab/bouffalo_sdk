#include "bflb_mtimer.h"
#include "bflb_rtc.h"
#include "board.h"

#if defined (BL616CL) || defined (BL618DG)
#if defined (BL616CL)
#include "bl616cl_hbn.h"
#elif defined(BL618DG)
#include "bl618dg_hbn.h"
#endif
#endif

struct bflb_device_s *rtc;
struct bflb_tm g_time;

#if defined (BL616CL) || defined (BL618DG)

#define BL_PDS_CNT_TO_US(cnt) ((cnt) * 15625 / 512)  /* cnt / 32768 * 1000000 */

/* RC32K fine trim calibration using mtimer as reference */
static int rc32k_fine_trim(void)
{
    uint32_t retry_cnt = 0;
    uint64_t rtc_cnt;
    uint64_t rtc_record_us, rtc_now_us;
    uint64_t mtimer_record_us, mtimer_now_us;
    uint32_t rtc_us, mtimer_us;
    int error_ppm;
    int last_error_ppm = 0;
    uint32_t rc32k_code;
    uint32_t last_diff_code = 0;
    int first_measure = 1;

    printf("rc32k_fine_trim: r_code=%u\r\n",HBN_Get_RC32K_R_Code());

    while (retry_cnt < 100) {
        retry_cnt++;

        /* Disable IRQ */
        __disable_irq();
        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
        __enable_irq();

        rtc_record_us = BL_PDS_CNT_TO_US(rtc_cnt);

        bflb_mtimer_delay_ms(100);

        /* Disable IRQ */
        __disable_irq();
        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
        __enable_irq();

        rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* Calculate time difference and PPM error */
        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        error_ppm = ((int32_t)(rtc_us - mtimer_us)) * 1000000 / mtimer_us;

        printf("rc32k_fine_trim: mtimer_us=%u, rtc_us=%u, ppm=%d\r\n",
               mtimer_us, rtc_us, error_ppm);

        /* Fine trim target: ±500ppm */
        if (abs(error_ppm) > 500) {
            /* Calculate adjustment code (step ~200ppm) */
            int diff_code;
            int is_diverging = 0;

            /* Improved divergence detection: only when error absolute value increases */
            if (!first_measure && abs(error_ppm) > abs(last_error_ppm)) {
                if (abs(last_error_ppm) > 1) {
                    is_diverging = 1;
                }
            }

            if (is_diverging) {
                /* Diverging, rollback to last code and adjust by ±1 */
                printf("rc32k_fine_trim: detected divergence (ppm:%d, last:%d), rollback\r\n",
                       error_ppm, last_error_ppm);

                rc32k_code = (int)HBN_Get_RC32K_R_Code() - (int)last_diff_code;

                /* Rollback to last code and adjust by ±1 */
                if (last_error_ppm < 0) {
                    diff_code = -1;
                } else {
                    diff_code = 1;
                }
                rc32k_code += diff_code;
                HBN_Set_RC32K_R_Code(rc32k_code);

                printf("rc32k_fine_trim: retry_cnt=%u, adjust code=%u (diff=%d)\r\n",
                    retry_cnt, rc32k_code, diff_code);

                /* Record current error for next divergence detection */
                /* Diverging diff_code not Record */
                last_diff_code = diff_code;
                first_measure = 0;

                /* Wait for RC32K frequency to settle */
                bflb_mtimer_delay_ms(5);

            } else {
                /* Normal calculation of adjustment code */
                diff_code = error_ppm / 400 ;

                /* Limit max adjustment to avoid jumping at fluctuation points */
                if (diff_code > 5) diff_code = 5;
                if (diff_code < -5) diff_code = -5;

                rc32k_code = HBN_Get_RC32K_R_Code();
                rc32k_code += diff_code;

                HBN_Set_RC32K_R_Code(rc32k_code);

                printf("rc32k_fine_trim: retry_cnt=%u, adjust code=%u (diff=%d)\r\n",
                    retry_cnt, rc32k_code, diff_code);

                /* Record current error for next divergence detection */
                last_error_ppm = error_ppm;
                last_diff_code = diff_code;
                first_measure = 0;

                /* Wait for RC32K frequency to settle */
                bflb_mtimer_delay_ms(5);
            }

        } else {
            printf("rc32k_fine_trim: success! retry_cnt=%u, ppm=%d, code=%u\r\n",
                   retry_cnt, error_ppm, HBN_Get_RC32K_R_Code());
            return error_ppm;
        }
    }

    printf("rc32k_fine_trim: timeout!\r\n");
    return -1;
}
#endif

int main(void)
{
    board_init();

    rtc = bflb_device_get_by_name("rtc");

    bflb_rtc_set_time(rtc, 0);

#if defined (BL616CL) || defined (BL618DG)
    HBN_Trim_RC32K();
    rc32k_fine_trim();
#endif

    /* Set RTC time: 2023-11-27, 10:2:1, Monday */
    g_time.tm_sec = 1;
    g_time.tm_min = 2;
    g_time.tm_hour = 10;
    g_time.tm_wday = 1;
    g_time.tm_mday = 27;
    g_time.tm_mon = 10;
    g_time.tm_year = 2023 - 1900;
    bflb_rtc_set_utc_time(&g_time);
    while (1) {
        printf("time:%lld\r\n", BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)));
        bflb_rtc_get_utc_time(&g_time);
        printf("utc time:%u-%u-%u, %u:%u:%u, wday:%u\r\n",
               g_time.tm_year + 1900, g_time.tm_mon + 1, g_time.tm_mday,
               g_time.tm_hour, g_time.tm_min, g_time.tm_sec,
               g_time.tm_wday);
        bflb_mtimer_delay_ms(5000);
    }
}
