#include <stdio.h>
#include <string.h>
#if defined(CONFIG_CLOCK_SOURCE_EF_PARAM) && CONFIG_CLOCK_SOURCE_EF_PARAM
#include "easyflash.h"
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#if defined(BL616)
#include "bl616_glb.h"
#include "bl616_aon.h"
#include "bl616_hbn.h"
#elif defined(BL616CL)
#include "bl616cl_glb.h"
#include "bl616cl_aon.h"
#include "bl616cl_hbn.h"
#elif defined(BL618DG)
#include "bl618dg_glb.h"
#include "bl618dg_aon.h"
#endif
#include "bflb_rtc.h"
#include "bflb_mtimer.h"
#include "clock_manager.h"
#include "bl_lp.h"

#if (defined(BL616CL) || defined(BL618DG)) && defined(CONFIG_CLOCK_SOURCE_EF_PARAM) && CONFIG_CLOCK_SOURCE_EF_PARAM
#error "BL616CL/BL618DG only support CONFIG_CLOCK_SOURCE_EF_PARAM=n for now."
#endif

#define CLOCK_SOURCE_KEY "clock_src"
#define RC_CAL_DATA_KEY  "rc_cal"

static bool clock_source_xtal_supported(void)
{
#if defined(BL616)
    return true;
#else
    return false;
#endif
}

#if defined(BL616CL) || defined(BL618DG)
#define RC32K_PPM_THRESHOLD              (200)
#define RC32K_ADJUST_TRIGGER_PPM         (500)
#define RC32K_SETTLE_DELAY_MS            (10)
#define RC32K_MEASURE_DELAY_MS           (100)
#define RC32K_START_DELAY_MS             (20)
#define RC32K_MAX_RETRY_CNT              (100)
#define RC32K_STEP_PPM                   (800)
#define RC32K_MAX_STEP                   (5)
#define RC32K_MIN_SIGNIFICANT_STEP_PPM   (400)

static struct bflb_device_s *rtc;

static int32_t rc32k_accuracy_ppm_calculate(uint32_t expect_time, uint32_t rc32k_actual_time)
{
    int diff_val;
    int diff_ppm;

    if (expect_time == 0 || rc32k_actual_time == 0) {
        return 1000 * 1000;
    }

    diff_val = rc32k_actual_time - expect_time;
    diff_ppm = (int64_t)diff_val * 1000 * 1000 / expect_time;

    return diff_ppm;
}

static bool rc32k_check_accuracy(int32_t actual_ppm, int32_t threshold_ppm)
{
    uint32_t abs_ppm;

    if (actual_ppm < 0) {
        abs_ppm = -actual_ppm;
    } else {
        abs_ppm = actual_ppm;
    }

    return abs_ppm <= threshold_ppm;
}

static void rc32k_get_trim_code(uint32_t *c_code, uint32_t *r_code)
{
#if defined(BL616CL)
    uint32_t tmp_val;

    tmp_val = BL_RD_REG(HBN_BASE, HBN_RC32K_0);
    *c_code = BL_GET_REG_BITS_VAL(tmp_val, HBN_RC32K_CAP_SEL_AON);
    *r_code = BL_GET_REG_BITS_VAL(tmp_val, HBN_RC32K_CODE_FR_CAL_AON);
#elif defined(BL618DG)
    uint32_t tmp_val;

    tmp_val = BL_RD_REG(AON_BASE, AON_RC32K_0);
    *c_code = BL_GET_REG_BITS_VAL(tmp_val, AON_RC32K_CAP_SEL_AON);

    tmp_val = BL_RD_REG(HBN_BASE, HBN_RC32K_CTRL0);
    *r_code = BL_GET_REG_BITS_VAL(tmp_val, HBN_RC32K_CODE_FR_EXT);
#endif
}

static uint32_t rc32k_coarse_calibration(uint32_t cali_val)
{
    uint32_t retry_cnt = 0;
    uint64_t timeout_start;

    uint64_t rtc_cnt;
    uint64_t rtc_record_us;
    uint64_t rtc_now_us;
    uint64_t mtimer_record_us;
    uint64_t mtimer_now_us;

    uint32_t rtc_us;
    uint32_t mtimer_us;
    int error_ppm;
    bool ret = false;

    uint32_t c_code = 0;
    uint32_t r_code = 0;
    uint32_t rc32k_code = 0;
    int last_error_ppm = 0;
    int last_diff_code = 0;
    int first_measure = 1;

    HBN_Trim_RC32K();
    rc32k_get_trim_code(&c_code, &r_code);
    rc32k_code = r_code;
    printf("c_code=%d,r_code=%d\r\n", c_code, r_code);

    HBN_32K_Sel(HBN_32K_RC);
#if defined(BL616CL)
    HBN_Set_RTC_CLK_Sel(HBN_RTC_CLK_F32K);
#endif

    rtc = bflb_device_get_by_name("rtc");
    bflb_rtc_set_time(rtc, 0);

    printf("rc32k_coarse_trim task enable, freq_mtimer must be 1MHz!\r\n");
    timeout_start = bflb_mtimer_get_time_us();

    vTaskDelay(RC32K_START_DELAY_MS);

    while (retry_cnt < RC32K_MAX_RETRY_CNT) {
        int diff_code;
        int is_diverging = 0;

        retry_cnt++;

        __disable_irq();
        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
        __enable_irq();

        rtc_record_us = BL_PDS_CNT_TO_US(rtc_cnt);

        vTaskDelay(RC32K_MEASURE_DELAY_MS);

        __disable_irq();
        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
        __enable_irq();

        rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        error_ppm = rc32k_accuracy_ppm_calculate(mtimer_us, rtc_us);

        printf("rc32k_coarse_trim: mtimer_us:%d, rtc_us:%d\r\n", mtimer_us, rtc_us);

        ret = rc32k_check_accuracy(error_ppm, RC32K_PPM_THRESHOLD);
        rc32k_get_trim_code(&c_code, &r_code);
        rc32k_code = r_code;
        printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, r_code=%d", retry_cnt, error_ppm, r_code);

        if (ret) {
            printf(", finish!\r\n");
            break;
        }

        if (abs(error_ppm) <= RC32K_ADJUST_TRIGGER_PPM) {
            printf("\r\n");
            break;
        }

        if (!first_measure && abs(error_ppm) > abs(last_error_ppm) && abs(last_error_ppm) > 1) {
            is_diverging = 1;
        }

        if (is_diverging) {
            printf(" (diverging, rollback)\r\n");

            rc32k_code = HBN_Get_RC32K_R_Code();
            rc32k_code = (uint32_t)((int)rc32k_code - last_diff_code);

            diff_code = (last_error_ppm < 0) ? -1 : 1;
            rc32k_code = (uint32_t)((int)rc32k_code + diff_code);
            HBN_Set_RC32K_R_Code(rc32k_code);

            printf("rc32k_coarse_trim: adjust code=%u (diff=%d)\r\n", rc32k_code, diff_code);

            last_diff_code = diff_code;
            first_measure = 0;
            vTaskDelay(RC32K_SETTLE_DELAY_MS);
            continue;
        }

        diff_code = error_ppm / RC32K_STEP_PPM;

        if (diff_code > RC32K_MAX_STEP) {
            diff_code = RC32K_MAX_STEP;
        }
        if (diff_code < -RC32K_MAX_STEP) {
            diff_code = -RC32K_MAX_STEP;
        }

        if (diff_code == 0 && abs(error_ppm) > RC32K_MIN_SIGNIFICANT_STEP_PPM) {
            diff_code = (error_ppm > 0) ? 1 : -1;
        }

        rc32k_code = HBN_Get_RC32K_R_Code();
        rc32k_code = (uint32_t)((int)rc32k_code + diff_code);
        HBN_Set_RC32K_R_Code(rc32k_code);

        printf(" (adjust code=%u, diff=%d)\r\n", rc32k_code, diff_code);

        last_error_ppm = error_ppm;
        last_diff_code = diff_code;
        first_measure = 0;

        vTaskDelay(RC32K_SETTLE_DELAY_MS);
    }

    if (retry_cnt >= RC32K_MAX_RETRY_CNT) {
        printf("rc32k_coarse_trim: timeout!\r\n");
    }

    printf("rc32k coarse trim success!, total time:%dms\r\n",
           (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

    bl_lp_rc32k_save_code(rc32k_code);

    return rc32k_code;
}

#else
static uint32_t rc32k_coarse_calibration(uint32_t cali_val)
{
    uint32_t retry_cnt = 0;
    uint64_t timeout_start;

    uint64_t rtc_cnt, rtc_record_us, rtc_now_us;
    uint64_t mtimer_record_us, mtimer_now_us;

    uint32_t rtc_us, mtimer_us;
    int error_ppm;
    uint32_t cali_result;

    if (cali_val) {
        *((volatile uint32_t *)0x2000F200) = cali_val;
    }

    printf("rc32k_coarse_trim task enable, freq_mtimer must be 1MHz!\r\n");
    timeout_start = bflb_mtimer_get_time_us();

    vTaskDelay(20);
    while (1) {
        retry_cnt += 1;

        /* disable irq */
        __disable_irq();

        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_record_us = BL_PDS_CNT_TO_US(rtc_cnt);

        vTaskDelay(50);
        /* disable irq */
        __disable_irq();

        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* calculate */
        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        /* call coarse_adj */
        error_ppm = bl_lp_rtc_rc32k_coarse_adj(mtimer_us, rtc_us);

        printf("rc32k_coarse_trim: mtimer_us:%d, rtc_us:%d\r\n", mtimer_us, rtc_us);

        if (error_ppm > 2000 || error_ppm < -2000) {
            /*  */
            printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, continue...\r\n", retry_cnt, error_ppm);
            vTaskDelay(5);
        } else {
            /* finish */
            printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, finish!\r\n", retry_cnt, error_ppm);
            break;
        }
    }

    printf("rc32k coarse trim success!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

    cali_result = *((volatile uint32_t *)0x2000F200);

    return cali_result;
}
#endif

static int xtal32k_check(int crystal_flag)
{
    uint32_t xtal32_regulator_flag = 0;

    uint64_t timeout_start;

    uint32_t retry_cnt = 0;

    uint64_t rtc_cnt, rtc_record_us, rtc_now_us;
    uint64_t mtimer_record_us, mtimer_now_us;

    uint32_t rtc_us, mtimer_us;
    int32_t diff_us;

    uint32_t success_flag = 0;

    int ret = 0;

    bl_lp_set_32k_clock_ready(0);

    vTaskDelay(10);
    printf("xtal32k_check_entry task enable, freq_mtimer must be 1MHz!\r\n");

#if defined(BL618DG)
    AON_Power_On_Xtal_32K();
    (void)crystal_flag;
#elif defined(BL616CL)
    HBN_Power_On_Xtal_32K();
    (void)crystal_flag;
#else
    bl_lp_power_on_xtal32k(crystal_flag);
#endif

    timeout_start = bflb_mtimer_get_time_us();

    printf("xtal32k_check: delay 100 ms\r\n");
    //vTaskDelay(500);
    vTaskDelay(100);

    printf("xtal32k_check: start check\r\n");

    HBN_32K_Sel(1);
    vTaskDelay(2);

    while (1) {
        retry_cnt += 1;

        /* disable irq */
        __disable_irq();

        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_record_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* delay */
        vTaskDelay(10);

        /* disable irq */
        __disable_irq();

        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* calculate */
        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        diff_us = rtc_us - mtimer_us;

        if (crystal_flag) {
            if (diff_us < -100 || diff_us > 100) {
                /* continue */
                //printf("xtal32k_check: retry_cnt:%d, diff_us:%d, continue...\r\n", retry_cnt, diff_us);
                vTaskDelay(10);
            } else {
                /* finish */
                //printf("xtal32k_check: retry_cnt:%d, diff_us:%d, finish!\r\n", retry_cnt, diff_us);
                success_flag = 1;
                break;
            }

            /* 1sec, set xtal regulator */
            if ((xtal32_regulator_flag == 0) && (bflb_mtimer_get_time_us() - timeout_start > 1000 * 1000)) {
                printf("xtal32K_check: reset xtal32k regulator\r\n");
                xtal32_regulator_flag = 1;

                HBN_32K_Sel(0);
#if defined(BL618DG)
                AON_Power_Off_Xtal_32K();
#else
                HBN_Power_Off_Xtal_32K();
#endif

                vTaskDelay(10);

#if defined(BL618DG)
                AON_Set_Xtal_32K_Regulator(3);
                AON_Power_On_Xtal_32K();
#else
                HBN_Set_Xtal_32K_Regulator(3);
                HBN_Power_On_Xtal_32K();
#endif
                HBN_32K_Sel(1);
            }

            if (bflb_mtimer_get_time_us() - timeout_start > 3 * 1000 * 1000) {
                success_flag = 0;
                break;
            }
        } else {
            if (diff_us < -200 || diff_us > 200) {
                /* continue */
                vTaskDelay(10);
            } else {
                /* finish */
                success_flag = 1;
                break;
            }

            if (bflb_mtimer_get_time_us() - timeout_start > 1 * 1000 * 1000) {
                success_flag = 0;
                printf("The oscillator deviation is too large, causing oscillator initialization to fail.\r\n");
                break;
            }
        }
    }

    if (success_flag) {
        printf("xtal32k_check: success!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

        /* GPIO17 no pull */
        *((volatile uint32_t *)0x2000F014) &= ~(1 << 16);

        ret = 1;
        printf("Select xtak32k as clock source!\r\n");

    } else {
        printf("xtal32k_check: failure!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);
        printf("xtal32k_check: select rc32k, and xtal32k poweroff \r\n");
        HBN_32K_Sel(0);
#if defined(BL618DG)
        AON_Power_Off_Xtal_32K();
#else
        HBN_Power_Off_Xtal_32K();
#endif
    }

    /* */
    bl_lp_set_32k_clock_ready(1);

    return ret;
}

#if defined(CONFIG_CLOCK_SOURCE_EF_PARAM) && CONFIG_CLOCK_SOURCE_EF_PARAM
// Read clock source from flash using EasyFlash
int clock_source_read(uint8_t *clock_source)
{
    size_t value_len = 0;

    *clock_source = 0; // Default if not found

    // Use EasyFlash to read clock source setting
    if (ef_get_env_blob(CLOCK_SOURCE_KEY, clock_source, sizeof(uint8_t), &value_len) == 0 ||
        value_len != sizeof(uint8_t)) {
        printf("Clock source file not found\r\n");
        return -1;
    }

    printf("Read clock_source=%d from flash\r\n", *clock_source);
    return 0;
}

// Write clock source to flash using EasyFlash
int clock_source_write(uint8_t clock_source)
{
    if (clock_source < 1 || clock_source > 3) {
        printf("Invalid clock source value: %d\r\n", clock_source);
        return -1;
    }

    // Use EasyFlash to write clock source setting
    if (ef_set_env_blob(CLOCK_SOURCE_KEY, &clock_source, sizeof(uint8_t)) != EF_NO_ERR) {
        printf("Failed to write clock source\r\n");
        return -2;
    }

    printf("Wrote clock_source=%d to flash\r\n", clock_source);
    return 0;
}

// Read RC calibration data from flash using EasyFlash
int rc_cal_data_read(uint32_t *rc_cal_value)
{
    size_t value_len = 0;

    *rc_cal_value = 0; // Default if not found

    // Use EasyFlash to read RC calibration data
    if (ef_get_env_blob(RC_CAL_DATA_KEY, rc_cal_value, sizeof(uint32_t), &value_len) == 0 ||
        value_len != sizeof(uint32_t)) {
        printf("RC calibration data file not found\r\n");
        return -1;
    }

    printf("Read RC calibration=0x%08X from flash\r\n", *rc_cal_value);
    return 0;
}

// Write RC calibration data to flash using EasyFlash
int rc_cal_data_write(uint32_t rc_cal_value)
{
    // Use EasyFlash to write RC calibration data
    if (ef_set_env_blob(RC_CAL_DATA_KEY, &rc_cal_value, sizeof(uint32_t)) != EF_NO_ERR) {
        printf("Failed to write RC calibration data\r\n");
        return -1;
    }

    printf("Wrote RC calibration=0x%08X to flash\r\n", rc_cal_value);
    return 0;
}


// Store RC calibration data
static int store_rc_cal_data(uint32_t rc_cal_data)
{
    if (rc_cal_data == 0) {
        printf("Warning: RC calibration data appears to be 0\r\n");
    }

    return rc_cal_data_write(rc_cal_data);
}

// Perform full RC calibration and save to flash
static int perform_full_rc_calibration(void)
{
    uint32_t cali_val;

    printf("Performing full RC calibration...\r\n");

    // Perform the existing RC32K calibration
    cali_val = rc32k_coarse_calibration(0);

    /* set bl_lp 32k clock ready */
    printf("rc32k_coarse_trim: set lp_32k ready!\r\n");
    bl_lp_set_32k_clock_ready(1);

    // Store the calibration data
    if (store_rc_cal_data(cali_val) != 0) {
        printf("Warning: Failed to save RC calibration data\r\n");
        return -1;
    }

    printf("Full RC calibration completed and stored\r\n");
    return 0;
}

// Apply saved RC calibration data
static int apply_rc_cal_data(uint32_t rc_cal_data)
{
    printf("Applying saved RC calibration data: 0x%08x\r\n", rc_cal_data);

    // Apply RC calibration data
    rc32k_coarse_calibration(rc_cal_data);

    /* set bl_lp 32k clock ready */
    printf("rc32k_coarse_trim: set lp_32k ready!\r\n");
    bl_lp_set_32k_clock_ready(1);

    return 0;
}



// Set clock source (API function)
int app_set_clock_source(uint8_t source)
{
    uint8_t current_source;

    if (source < CLOCK_SOURCE_RC || source > CLOCK_SOURCE_ACTIVE) {
        printf("Invalid clock source value: %d\r\n", source);
        return -1;
    }

    // Read current setting
    if (clock_source_read(&current_source) == 0 && current_source == source) {
        printf("Clock source already set to %d, no change needed\r\n", source);
        return 0;
    }

    // Write new value
    if (clock_source_write(source) != 0) {
        printf("Failed to write clock source to Flash\r\n");
        return -2;
    }

    printf("Clock source updated to %d, restarting device...\r\n", source);

    // Restart device
    __disable_irq();
    bflb_mtimer_delay_ms(10);
    bl_sys_reset_por();

    return 0; // Should not reach here
}

// Get current clock source (API function)
int app_get_clock_source(uint8_t *source)
{
    if (source == NULL) {
        return -1;
    }

    // Read from storage
    if (clock_source_read(source) != 0) {
        // Default to RC if read fails
        *source = CLOCK_SOURCE_RC;
        return -2;
    }

    return 0;
}


// Initialize clock system based on stored settings
int app_clock_init(void)
{
    uint8_t clock_source = 0;
    uint32_t rc_cal_data = 0;
    int rc_success = 0, xtal_success = 0;

    // Read clock source setting
    if (clock_source_read(&clock_source) != 0 || clock_source == 0) {
        // First boot or no settings - perform full calibration and use RC
        printf("No clock source setting found, performing first-time setup\r\n");
        perform_full_rc_calibration();
        clock_source_write(CLOCK_SOURCE_RC);
        return 0;
    }

    printf("Initializing clock with source: %d\r\n", clock_source);

    // Always attempt to load RC calibration data first
    if (rc_cal_data_read(&rc_cal_data) == 0 && rc_cal_data != 0) {
        apply_rc_cal_data(rc_cal_data);
        rc_success = 1;
    } else {
        // If no saved data, do a full calibration
        perform_full_rc_calibration();
        rc_success = 1;
    }

    // Now handle XTAL if needed
    if (clock_source == CLOCK_SOURCE_PASSIVE) {
        printf("Initializing passive XTAL oscillator\r\n");
        if (xtal32k_check(1)) {
            xtal_success = 1;
        } else {
            printf("ERROR: Failed to initialize passive crystal oscillator\r\n");
            // Fall back to RC
            clock_source_write(CLOCK_SOURCE_RC);
        }
    } else if (clock_source == CLOCK_SOURCE_ACTIVE) {
        printf("Initializing active XTAL oscillator\r\n");
        if (xtal32k_check(0)) {
            xtal_success = 1;
        } else {
            printf("ERROR: Failed to initialize active crystal oscillator\r\n");
            // Fall back to RC
            clock_source_write(CLOCK_SOURCE_RC);
        }
    }

    return (rc_success || xtal_success) ? 0 : -1;
}
#else

#define CLOCK_INIT_TIMEOUT_MS   (10000)

static TaskHandle_t rc32k_coarse_trim_task_hd = NULL;
static TaskHandle_t xtal32k_check_entry_task_hd = NULL;
static SemaphoreHandle_t clock_init_done_sem = NULL;

/**********************************************************
    rc32k coarse trim task func
 **********************************************************/
static void rc32k_coarse_trim_task(void *pvParameters)
{
    rc32k_coarse_calibration(0);

    /* coarse_adj success */
    if(xtal32k_check_entry_task_hd){
        /* resume xtal32k_check task */
        printf("rc32k_coarse_trim: Resume xtal32k_check task!\r\n");
        xTaskNotifyGive(xtal32k_check_entry_task_hd);
    }else{
        /* set bl_lp 32k clock ready */
        printf("rc32k_coarse_trim: set lp_32k ready!\r\n");
        bl_lp_set_32k_clock_ready(1);
        if (clock_init_done_sem) {
            xSemaphoreGive(clock_init_done_sem);
        }
    }

    printf("rc32k_coarse_trim: rc32k code:%d\r\n", iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext);

    printf("rc32k_coarse task: vTaskDelete\r\n");
    vTaskDelete(NULL);
}

/**********************************************************
    xtal32k check task func
 **********************************************************/
static void xtal32k_check_entry_task(void *pvParameters)
{
    if (rc32k_coarse_trim_task_hd) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    xtal32k_check(1);

    if (clock_init_done_sem) {
        xSemaphoreGive(clock_init_done_sem);
    }

    printf("xtal32k_check task: vTaskDelete\r\n");
    vTaskDelete(NULL);
}


int app_clock_init(void)
{
    clock_init_done_sem = xSemaphoreCreateBinary();
    if (clock_init_done_sem == NULL) {
        printf("[CLOCK] Error: Failed to create semaphore\r\n");
        return -1;
    }

    /* coarse trim rc32k */
    printf("[OS] Create rc32k_coarse_trim task...\r\n");
    xTaskCreate(rc32k_coarse_trim_task, (char*)"rc32k_coarse_trim", 512, NULL, 11, &rc32k_coarse_trim_task_hd);

    if (clock_source_xtal_supported()) {
        /* auto check xtal32k, only test */
        printf("[OS] Create xtal32k_check_entry task...\r\n");
        xTaskCreate(xtal32k_check_entry_task, (char*)"xtal32k_check_entry", 512, NULL, 10, &xtal32k_check_entry_task_hd);
    } else {
        xtal32k_check_entry_task_hd = NULL;
        printf("[OS] XTAL32K check is disabled on this chip, RC32K only.\r\n");
    }

    printf("[OS] Waiting for clock init to complete...\r\n");
    if (xSemaphoreTake(clock_init_done_sem, pdMS_TO_TICKS(CLOCK_INIT_TIMEOUT_MS)) != pdTRUE) {
        printf("[CLOCK] Error: Clock init timeout!\r\n");
        vSemaphoreDelete(clock_init_done_sem);
        clock_init_done_sem = NULL;
        return -1;
    }

    printf("[OS] Clock init complete!\r\n");
    vSemaphoreDelete(clock_init_done_sem);
    clock_init_done_sem = NULL;

    return 0;
}
#endif
