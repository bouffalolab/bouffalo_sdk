#include <stdio.h>
#include <string.h>
#include "easyflash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bl616_glb.h"
#include "bl616_hbn.h"
#include "bflb_mtimer.h"
#include "app_clock_manager.h"
#include "bl616_lp.h"

#define CLOCK_SOURCE_KEY "clock_src"
#define RC_CAL_DATA_KEY "rc_cal"

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

void xtal32k_calibration(void)
{
    uint32_t retry_cnt = 5;
    uint64_t timeout_start;

    uint64_t rtc_cnt, rtc_record_us, rtc_now_us;
    uint64_t mtimer_record_us, mtimer_now_us;

    uint32_t rtc_us, mtimer_us;
    int32_t diff_us;

    bflb_lp_set_32k_clock_ready(0);

    while (retry_cnt) {
        retry_cnt--;

        __disable_irq();
        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
        __enable_irq();

        rtc_record_us = BFLB_PDS_CNT_TO_US(rtc_cnt);

        vTaskDelay(1000);

        __disable_irq();
        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
        __enable_irq();

        rtc_now_us = BFLB_PDS_CNT_TO_US(rtc_cnt);

        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        diff_us = rtc_us - mtimer_us;

        if(diff_us < 0){
            modify_register_bits_incremental(-8);
        }else if (diff_us > 25){
            modify_register_bits_incremental(8);
        } else {
            printf("xtal32k_check: mtimer_us:%ld, rtc_us:%ld minus:%ld\r\n", mtimer_us, rtc_us, diff_us);
            break;
        }

        //printf("xtal32k_check: mtimer_us:%ld, rtc_us:%ld minus:%ld\r\n", mtimer_us, rtc_us, diff_us);
    }

    bflb_lp_set_32k_clock_ready(1);
}

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

    bflb_mtimer_delay_ms(20);
    while(1){
        retry_cnt += 1;

        /* disable irq */
        __disable_irq();

        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_record_us = BFLB_PDS_CNT_TO_US(rtc_cnt);

        bflb_mtimer_delay_ms(50);
        /* disable irq */
        __disable_irq();

        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_now_us = BFLB_PDS_CNT_TO_US(rtc_cnt);

        /* calculate */
        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        /* call coarse_adj */
        error_ppm = bflb_lp_rtc_rc32k_coarse_adj(mtimer_us, rtc_us);

        printf("rc32k_coarse_trim: mtimer_us:%d, rtc_us:%d\r\n", mtimer_us, rtc_us);

        if(error_ppm > 2000 || error_ppm < -2000){
            /*  */
            printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, continue...\r\n", retry_cnt, error_ppm);
            bflb_mtimer_delay_ms(5);
        }else{
            /* finish */
            printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, finish!\r\n", retry_cnt, error_ppm);
            break;
        }
    }

    printf("rc32k coarse trim success!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

    /* set bflb_lp 32k clock ready */
    printf("rc32k_coarse_trim: set lp_32k ready!\r\n");
    bflb_lp_set_32k_clock_ready(1);

    cali_result = *((volatile uint32_t *)0x2000F200);

    return cali_result;
}

static void xtal32k_input(void)
{
    modify_bit(0x2000F204, 19, 0x1);
    modify_bit(0x2000F204, 21, 0x1);
    modify_bit(0x2000F204, 2, 0x1);
    modify_bit(0x2000F204, 10, 0);
    modify_bit(0x2000F204, 9, 0);
    modify_bit(0x2000F204, 17, 0x0);
}

static int xtal32k_check_entry_task(int crystal_flag)
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

    bflb_lp_set_32k_clock_ready(0);

    vTaskDelay(10);
    printf("xtal32k_check_entry task enable, freq_mtimer must be 1MHz!\r\n");

    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = GPIO_FUN_ANALOG,
        .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1
    };

    if (crystal_flag) {
        gpioCfg.gpioPin = 16;
        GLB_GPIO_Init(&gpioCfg);
        printf("use passive crystal.\r\n");
    } else {
        printf("use active crtstal.\r\n");
    }

    gpioCfg.gpioPin = 17;
    GLB_GPIO_Init(&gpioCfg);
    *(uint32_t*)(0x2000F204) = 0x210288;

    if (crystal_flag) {
        /* power on */
        HBN_Set_Xtal_32K_Inverter_Amplify_Strength(3);
        HBN_Power_On_Xtal_32K();
    } else {
        HBN_Power_On_Xtal_32K();
        xtal32k_input();
    }

    timeout_start = bflb_mtimer_get_time_us();

    printf("xtal32k_check: delay 100 ms\r\n");
    //vTaskDelay(500);
    vTaskDelay(100);


    printf("xtal32k_check: start check\r\n");

    HBN_32K_Sel(1);
    vTaskDelay(2);

    while(1){
        retry_cnt += 1;

        /* disable irq */
        __disable_irq();

        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_record_us = BFLB_PDS_CNT_TO_US(rtc_cnt);

        /* delay */
        vTaskDelay(10);

         /* disable irq */
        __disable_irq();

        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_now_us = BFLB_PDS_CNT_TO_US(rtc_cnt);

        /* calculate */
        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        diff_us = rtc_us - mtimer_us;

        //printf("xtal32k_check: mtimer_us:%d, rtc_us:%d\r\n", mtimer_us, rtc_us);

        if(diff_us < -100 || diff_us > 100){
            /* continue */
            //printf("xtal32k_check: retry_cnt:%d, diff_us:%d, continue...\r\n", retry_cnt, diff_us);
            vTaskDelay(10);
        }else{
            /* finish */
            //printf("xtal32k_check: retry_cnt:%d, diff_us:%d, finish!\r\n", retry_cnt, diff_us);
            success_flag = 1;
            break;
        }

        /* 1sec, set xtal regulator */
        if((xtal32_regulator_flag == 0) && (bflb_mtimer_get_time_us() - timeout_start > 1000*1000)){
            printf("xtal32K_check: reset xtal32k regulator\r\n");
            xtal32_regulator_flag = 1;

            HBN_32K_Sel(0);
            HBN_Power_Off_Xtal_32K();

            vTaskDelay(10);

            HBN_Set_Xtal_32K_Regulator(3);
            HBN_Power_On_Xtal_32K();
            HBN_32K_Sel(1);
        }

        if(bflb_mtimer_get_time_us() - timeout_start > 3 * 1000 * 1000){
            success_flag = 0;
            break;
        }
    }

    if(success_flag){
        printf("xtal32k_check: success!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

        /* GPIO17 no pull */
        *((volatile uint32_t *)0x2000F014) &= ~(1 << 16);

        ret = 1;
        printf("select xtal32k\r\n");

    }else{
        printf("xtal32k_check: failure!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);
        printf("xtal32k_check: select rc32k, and xtal32k poweroff \r\n");
        HBN_32K_Sel(0);
        HBN_Power_Off_Xtal_32K();
    }

    /* */
    printf("xtal32k_check: set lp_32k ready!\r\n");
    bflb_lp_set_32k_clock_ready(1);

    return ret;
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
        if (xtal32k_check_entry_task(1)) {
            xtal_success = 1;
        } else {
            printf("ERROR: Failed to initialize passive crystal oscillator\r\n");
            // Fall back to RC
            clock_source_write(CLOCK_SOURCE_RC);
        }
    } else if (clock_source == CLOCK_SOURCE_ACTIVE) {
        printf("Initializing active XTAL oscillator\r\n");
        if (xtal32k_check_entry_task(0)) {
            xtal_success = 1;
        } else {
            printf("ERROR: Failed to initialize active crystal oscillator\r\n");
            // Fall back to RC
            clock_source_write(CLOCK_SOURCE_RC);
        }
    }
    
    return (rc_success || xtal_success) ? 0 : -1;
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
