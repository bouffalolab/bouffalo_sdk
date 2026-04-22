#include "bl_lp_internal.h"
#include "bl616cl_ext_dcdc.h"

static struct bflb_device_s *gpio;

ATTR_TCM_CONST_SECTION iot2lp_para_t *const iot2lp_para = (iot2lp_para_t *)IOT2LP_PARA_ADDR;

static ATTR_NOCACHE_RAM_SECTION struct bl_lp_info_s lp_info_struct = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_wifi_para_t wifi_param = { 0 };
ATTR_NOCACHE_RAM_SECTION lp_fw_wakeup_source_t wakeup_source = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_wakeup_reason_t wakeup_reason = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_mtimer_t mtimer_info = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_rc32k_trim_t rc32k_trim = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_bcn_delay_t beacon_delay_info = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_bcn_loss_t beacon_loss_info = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_jtag_t lp_fw_jtag_parameter = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_uart_t lp_fw_uart_cfg = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_clock_t lp_fw_clock_cfg = { 0 };
#if (BL_LP_TIME_DEBUG)
ATTR_NOCACHE_RAM_SECTION lp_fw_time_debug_t time_debug_buff[TIME_DEBUG_NUM_MAX] = { 0 };
#endif

bl_lp_fw_cfg_t lpfw_cfg = {
    .tim_wakeup_en = 1,
    .rtc_timeout_us = (60 * 1000 * 1000),
    .dtim_origin = 10,
    .dtim_num = 0,
};

static uint64_t g_rtc_timestamp_before_sleep_us = 0;
static uint64_t g_rtc_timestamp_after_sleep_us = 0;
static uint64_t g_mtimer_timestamp_before_sleep_us = 0;
static uint64_t g_mtimer_timestamp_after_sleep_us = 0;
static uint64_t g_virtual_timestamp_base_us = 0;

#define CHECK_IOT2LP_POINTER(field)                       \
    if (para->field == NULL) {                            \
        BL_LP_LOG("iot2lp_para->"#field" is NULL\r\n");   \
        return -1;                                        \
    }

static int bl_lp_check_iot2lp_para_pointers(iot2lp_para_t *para)
{
    if (para == NULL) {
        BL_LP_LOG("iot2lp_para is NULL\r\n");
        return -1;
    }

    /* Check critical pointer variables in iot2lp_para */
    CHECK_IOT2LP_POINTER(wifi_parameter);
    CHECK_IOT2LP_POINTER(bcn_delay_info);
    CHECK_IOT2LP_POINTER(bcn_loss_info);
    CHECK_IOT2LP_POINTER(rc32k_trim_parameter);
    CHECK_IOT2LP_POINTER(mtimer_parameter);
    CHECK_IOT2LP_POINTER(lp_info);
    CHECK_IOT2LP_POINTER(flash_parameter);
    CHECK_IOT2LP_POINTER(sec_cfg);
    CHECK_IOT2LP_POINTER(tzc_cfg);
    CHECK_IOT2LP_POINTER(jtag_parameter);
    CHECK_IOT2LP_POINTER(uart_config);
    CHECK_IOT2LP_POINTER(clock_config);
    CHECK_IOT2LP_POINTER(wakeup_source_parameter);
    CHECK_IOT2LP_POINTER(wakeup_reason_info);

#if (BL_LP_TIME_DEBUG)
    CHECK_IOT2LP_POINTER(time_debug);
#endif
    return 0;
}

void bl_lp_wifi_param_update(bl_lp_fw_cfg_t *bl_lp_fw_cfg)
{
    if (bl_lp_fw_cfg->tim_wakeup_en) {
        iot2lp_para->wifi_parameter->tim_wakeup_en = 1;
        memcpy(iot2lp_para->wifi_parameter->bssid, bl_lp_fw_cfg->bssid, 6);
        memcpy(iot2lp_para->wifi_parameter->local_mac, bl_lp_fw_cfg->mac, 6);
        iot2lp_para->wifi_parameter->ap_channel = bl_lp_fw_cfg->channel;
        iot2lp_para->wifi_parameter->aid = bl_lp_fw_cfg->aid;
        iot2lp_para->wifi_parameter->bcn_target_level = bl_lp_fw_cfg->rssi;
        iot2lp_para->wifi_parameter->wifi_rx_buff = (uint8_t *)((uint32_t)export_get_rx_buffer1_addr() & 0x2FFFFFFF);

        /* lpfw rx timeout */
        iot2lp_para->mtimer_parameter->mtimer_timeout_en = 1;
        iot2lp_para->mtimer_parameter->mtimer_timeout_mini_us = bl_lp_fw_cfg->mtimer_timeout_mini_us;
        iot2lp_para->mtimer_parameter->mtimer_timeout_max_us = bl_lp_fw_cfg->mtimer_timeout_max_us;


        /* take rc32k error rate as default */
        if (!(*((volatile uint32_t *)0x2000f030) & (1 << 3))) {
            /* rc32k 1500-ppm */
            iot2lp_para->rtc32k_jitter_error_ppm = 600;
        } else {
            /* xtal 500-ppm */
            iot2lp_para->rtc32k_jitter_error_ppm = 400;
        }

        /* tim interval */
        if (bl_lp_fw_cfg->dtim_num != 0) {
            iot2lp_para->wifi_parameter->dtim_num = bl_lp_fw_cfg->dtim_num;
        } else {
            iot2lp_para->wifi_parameter->dtim_num = 10;
        }
        iot2lp_para->wifi_parameter->bcmc_dtim_mode = bl_lp_fw_cfg->bcmc_dtim_mode;

    } else {
        iot2lp_para->wifi_parameter->tim_wakeup_en = 0;
    }
}


void lp_fw_save_cpu_para(uint32_t save_addr)
{
    __asm__ __volatile__("sw      x0, 0(a0)\n\t"
                         "sw      x1, 4(a0)\n\t"
                         "sw      x2, 8(a0)\n\t"
                         "sw      x3, 12(a0)\n\t"
                         "sw      x4, 16(a0)\n\t"
                         "sw      x5, 20(a0)\n\t"
                         "sw      x6, 24(a0)\n\t"
                         "sw      x7, 28(a0)\n\t"
                         "sw      x8, 32(a0)\n\t"
                         "sw      x9, 36(a0)\n\t"
                         "sw      x10, 40(a0)\n\t"
                         "sw      x11, 44(a0)\n\t"
                         "sw      x12, 48(a0)\n\t"
                         "sw      x13, 52(a0)\n\t"
                         "sw      x14, 56(a0)\n\t"
                         "sw      x15, 60(a0)\n\t"
                         "sw      x16, 64(a0)\n\t"
                         "sw      x17, 68(a0)\n\t"
                         "sw      x18, 72(a0)\n\t"
                         "sw      x19, 76(a0)\n\t"
                         "sw      x20, 80(a0)\n\t"
                         "sw      x21, 84(a0)\n\t"
                         "sw      x22, 88(a0)\n\t"
                         "sw      x23, 92(a0)\n\t"
                         "sw      x24, 96(a0)\n\t"
                         "sw      x25, 100(a0)\n\t"
                         "sw      x26, 104(a0)\n\t"
                         "sw      x27, 108(a0)\n\t"
                         "sw      x28, 112(a0)\n\t"
                         "sw      x29, 116(a0)\n\t"
                         "sw      x30, 120(a0)\n\t"
                         "sw      x31, 124(a0)\n\t");
}

void ATTR_TCM_SECTION __attribute__((aligned(16))) lp_fw_restore_cpu_para(uint32_t save_addr)
{
    __asm__ __volatile__("lw      x0, 0(a0)\n\t"
                         "lw      x1, 4(a0)\n\t"
                         "lw      x2, 8(a0)\n\t"
                         "lw      x3, 12(a0)\n\t"
                         "lw      x4, 16(a0)\n\t"
                         "lw      x5, 20(a0)\n\t"
                         "lw      x6, 24(a0)\n\t"
                         "lw      x7, 28(a0)\n\t"
                         "lw      x8, 32(a0)\n\t"
                         "lw      x9, 36(a0)\n\t"
                         "lw      x10, 40(a0)\n\t"
                         "lw      x11, 44(a0)\n\t"
                         "lw      x12, 48(a0)\n\t"
                         "lw      x13, 52(a0)\n\t"
                         "lw      x14, 56(a0)\n\t"
                         "lw      x15, 60(a0)\n\t"
                         "lw      x16, 64(a0)\n\t"
                         "lw      x17, 68(a0)\n\t"
                         "lw      x18, 72(a0)\n\t"
                         "lw      x19, 76(a0)\n\t"
                         "lw      x20, 80(a0)\n\t"
                         "lw      x21, 84(a0)\n\t"
                         "lw      x22, 88(a0)\n\t"
                         "lw      x23, 92(a0)\n\t"
                         "lw      x24, 96(a0)\n\t"
                         "lw      x25, 100(a0)\n\t"
                         "lw      x26, 104(a0)\n\t"
                         "lw      x27, 108(a0)\n\t"
                         "lw      x28, 112(a0)\n\t"
                         "lw      x29, 116(a0)\n\t"
                         "lw      x30, 120(a0)\n\t"
                         "lw      x31, 124(a0)\n\t"
#if defined(CONFIG_FREERTOS)
                         "la      a0, freertos_risc_v_trap_handler\n\t"
#else
                         "la      a0, default_trap_handler\n\t"
#endif
                         "ori     a0, a0, 3\n\t"
                         "csrw    mtvec, a0\n\t");
}

void lp_fw_print_cpu_para(uint32_t save_addr)
{
    BL_LP_LOG("=== CPU Register Values Saved at 0x%08X ===\n", save_addr);

    uint32_t *regs = (uint32_t *)save_addr;

    // Print all 32 registers with their names
    BL_LP_LOG("x0 (zero):  0x%08X\n", regs[0]);  // hardwired zero
    BL_LP_LOG("x1 (ra):    0x%08X\n", regs[1]);  // return address
    BL_LP_LOG("x2 (sp):    0x%08X\n", regs[2]);  // stack pointer
    BL_LP_LOG("x3 (gp):    0x%08X\n", regs[3]);  // global pointer
    BL_LP_LOG("x4 (tp):    0x%08X\n", regs[4]);  // thread pointer
    BL_LP_LOG("x5 (t0):    0x%08X\n", regs[5]);  // temporary/alternate link register
    BL_LP_LOG("x6 (t1):    0x%08X\n", regs[6]);  // temporary
    BL_LP_LOG("x7 (t2):    0x%08X\n", regs[7]);  // temporary
    BL_LP_LOG("x8 (s0/fp): 0x%08X\n", regs[8]);  // saved register/frame pointer
    BL_LP_LOG("x9 (s1):    0x%08X\n", regs[9]);  // saved register
    BL_LP_LOG("x10 (a0):   0x%08X\n", regs[10]); // function argument/return value
    BL_LP_LOG("x11 (a1):   0x%08X\n", regs[11]); // function argument/return value
    BL_LP_LOG("x12 (a2):   0x%08X\n", regs[12]); // function argument
    BL_LP_LOG("x13 (a3):   0x%08X\n", regs[13]); // function argument
    BL_LP_LOG("x14 (a4):   0x%08X\n", regs[14]); // function argument
    BL_LP_LOG("x15 (a5):   0x%08X\n", regs[15]); // function argument
    BL_LP_LOG("x16 (a6):   0x%08X\n", regs[16]); // function argument
    BL_LP_LOG("x17 (a7):   0x%08X\n", regs[17]); // function argument
    BL_LP_LOG("x18 (s2):   0x%08X\n", regs[18]); // saved register
    BL_LP_LOG("x19 (s3):   0x%08X\n", regs[19]); // saved register
    BL_LP_LOG("x20 (s4):   0x%08X\n", regs[20]); // saved register
    BL_LP_LOG("x21 (s5):   0x%08X\n", regs[21]); // saved register
    BL_LP_LOG("x22 (s6):   0x%08X\n", regs[22]); // saved register
    BL_LP_LOG("x23 (s7):   0x%08X\n", regs[23]); // saved register
    BL_LP_LOG("x24 (s8):   0x%08X\n", regs[24]); // saved register
    BL_LP_LOG("x25 (s9):   0x%08X\n", regs[25]); // saved register
    BL_LP_LOG("x26 (s10):  0x%08X\n", regs[26]); // saved register
    BL_LP_LOG("x27 (s11):  0x%08X\n", regs[27]); // saved register
    BL_LP_LOG("x28 (t3):   0x%08X\n", regs[28]); // temporary
    BL_LP_LOG("x29 (t4):   0x%08X\n", regs[29]); // temporary
    BL_LP_LOG("x30 (t5):   0x%08X\n", regs[30]); // temporary
    BL_LP_LOG("x31 (t6):   0x%08X\n", regs[31]); // temporary
    BL_LP_LOG("========================================\n");

    uint32_t sp_addr = getreg32(save_addr + 2 * 4);
    BL_LP_LOG("SP address: 0x%08x\r\n", sp_addr);

    /* Print SP content, size 32 bytes */
    BL_LP_LOG("SP content (32 bytes):\r\n");
    for (int i = 0; i < 32; i += 4) {
        BL_LP_LOG("0x%08x: 0x%08x\r\n", sp_addr + i, getreg32(sp_addr + i));
    }
}


static void rtc_wakeup_init(uint64_t rtc_wakeup_cmp_cnt, uint64_t sleep_us)
{
    uint32_t tmpVal;
    uint64_t rtc_cnt;

    if (sleep_us == 0) {
        return;
    }

    if (rtc_wakeup_cmp_cnt == 0) {
        /* Tigger RTC val latch  */
        tmpVal = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H);
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
        BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
        BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);

        /* Read RTC cnt */
        rtc_cnt = (BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H) & 0xffff);
        rtc_cnt <<= 32;
        rtc_cnt |= BL_RD_REG(HBN_BASE, HBN_RTC_TIME_L);

        /* calculate RTC Comp cnt */
        rtc_cnt += BL_US_TO_PDS_CNT(sleep_us);
    } else {
        rtc_cnt = rtc_wakeup_cmp_cnt;
    }

    iot2lp_para->wakeup_source_parameter->rtc_wakeup_cnt = rtc_cnt;
    iot2lp_para->wakeup_source_parameter->rtc_wakeup_en = 1;

    HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, (uint32_t)rtc_cnt, (uint32_t)(rtc_cnt >> 32) & 0xffff,
                      HBN_RTC_COMP_BIT0_47);
}

/* active_app */
void bl_lp_time_info_update_app(struct bl_lp_info_s *lp_info)
{
    uint64_t now_rtc_cnt, diff_rtc_cnt;
    HBN_Get_RTC_Timer_Val((uint32_t *)&now_rtc_cnt, (uint32_t *)&now_rtc_cnt + 1);
    diff_rtc_cnt = now_rtc_cnt - lp_info->time_record_start_rtc_cnt;

    lp_info->time_record_start_rtc_cnt = now_rtc_cnt;
    lp_info->time_total_rtc_cnt += diff_rtc_cnt;

    lp_info->time_active_app_rtc_cnt += diff_rtc_cnt;
}

/* sleep_pds */
void bl_lp_time_info_update_pds(struct bl_lp_info_s *lp_info)
{
    uint64_t now_rtc_cnt, diff_rtc_cnt;
    HBN_Get_RTC_Timer_Val((uint32_t *)&now_rtc_cnt, (uint32_t *)&now_rtc_cnt + 1);
    diff_rtc_cnt = now_rtc_cnt - lp_info->time_record_start_rtc_cnt;

    lp_info->time_record_start_rtc_cnt = now_rtc_cnt;
    lp_info->time_total_rtc_cnt += diff_rtc_cnt;

    lp_info->time_sleep_pds_rtc_cnt += diff_rtc_cnt;
}

/* active_lpfw */
void bl_lp_time_info_update_lpfw(struct bl_lp_info_s *lp_info)
{
    uint64_t now_rtc_cnt, diff_rtc_cnt;
    HBN_Get_RTC_Timer_Val((uint32_t *)&now_rtc_cnt, (uint32_t *)&now_rtc_cnt + 1);
    diff_rtc_cnt = now_rtc_cnt - lp_info->time_record_start_rtc_cnt;

    lp_info->time_record_start_rtc_cnt = now_rtc_cnt;
    lp_info->time_total_rtc_cnt += diff_rtc_cnt;

    lp_info->time_active_lpfw_rtc_cnt += diff_rtc_cnt;
}

void bl_lp_info_get(bl_lp_info_t *lp_info)
{
    struct bl_lp_info_s *lp_info_s = NULL;

    lp_info_s = iot2lp_para->lp_info;

    /* lpfw wakeup cnt */
    lp_info->lpfw_wakeup_cnt = iot2lp_para->lpfw_wakeup_cnt;

    /* bcn loss info */
    lp_info->lpfw_recv_cnt = lp_info_s->bcn_lpfw_recv_cnt;
    lp_info->lpfw_loss_cnt = lp_info_s->bcn_lpfw_loss_cnt;

    /* update active_app time */
    bl_lp_time_info_update_app(lp_info_s);

    /* time info */
    lp_info->time_total_us = BL_PDS_CNT_TO_US(lp_info_s->time_total_rtc_cnt);
    lp_info->sleep_pds_us = BL_PDS_CNT_TO_US(lp_info_s->time_sleep_pds_rtc_cnt);
    lp_info->active_lpfw_us = BL_PDS_CNT_TO_US(lp_info_s->time_active_lpfw_rtc_cnt);
    lp_info->active_app_us = BL_PDS_CNT_TO_US(lp_info_s->time_active_app_rtc_cnt);
}

void bl_lp_info_clear(struct bl_lp_info_s *lp_info)
{
    /* bcn */
    lp_info->bcn_lpfw_recv_cnt = 0;
    lp_info->bcn_lpfw_loss_cnt = 0;

    /* time */
    uint64_t rtc_cnt_now;
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt_now, (uint32_t *)&rtc_cnt_now + 1);
    lp_info->time_total_rtc_cnt = 0;
    lp_info->time_sleep_pds_rtc_cnt = 0;
    lp_info->time_active_lpfw_rtc_cnt = 0;
    lp_info->time_active_app_rtc_cnt = 0;
    lp_info->time_record_start_rtc_cnt = rtc_cnt_now;
}

void bl_lp_fw_init(void)
{
    // bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();
    // uint32_t chip_version = 0;

    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    // BL_LP_LOG("[LP] lpfw ram address: 0x%08X -> 0x%08X\r\n", lpfw_info->lpfw_memory_start, lpfw_info->lpfw_memory_end);
    // BL_LP_LOG("[LP] iot2lp_para size: %d\r\n", sizeof(iot2lp_para_t));

    /* clean iot2lp_para */
    memset(iot2lp_para, 0, (uint32_t)&iot2lp_para->reset_keep - (uint32_t)iot2lp_para);

    /* debug time buff */
#if (BL_LP_TIME_DEBUG)
    memset(time_debug_buff, 0, sizeof(time_debug_buff));
    iot2lp_para->time_debug = time_debug_buff;
#endif

    /* beacon loss record */
    memset(&beacon_loss_info, 0, sizeof(beacon_loss_info));
    iot2lp_para->bcn_loss_info = &beacon_loss_info;

    memset(&wifi_param, 0, sizeof(wifi_param));
    iot2lp_para->wifi_parameter = &wifi_param;

    memset(&wakeup_source, 0, sizeof(wakeup_source));
    iot2lp_para->wakeup_source_parameter = &wakeup_source;

    memset(&wakeup_reason, 0, sizeof(wakeup_reason));
    iot2lp_para->wakeup_reason_info = &wakeup_reason;

    memset(&mtimer_info, 0, sizeof(mtimer_info));
    iot2lp_para->mtimer_parameter = &mtimer_info;

    memset(&rc32k_trim, 0, sizeof(rc32k_trim));
    iot2lp_para->rc32k_trim_parameter = &rc32k_trim;
    /* Save rc32k code in HBN_RAM */
    iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext = HBN_Get_RC32K_R_Code();
    /* rc32k status: not ready */
    iot2lp_para->rc32k_trim_parameter->rc32k_clock_ready = 0;
    iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready = 0;
    iot2lp_para->rc32k_trim_parameter->rc32k_auto_cal_en = 1;

    memset(&beacon_delay_info, 0, sizeof(beacon_delay_info));
    iot2lp_para->bcn_delay_info = &beacon_delay_info;
    static ATTR_NOCACHE_RAM_SECTION int32_t bcn_delay_buff[16] = { 0 };
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_buff = bcn_delay_buff;
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_size = sizeof(bcn_delay_buff) / sizeof(int32_t);
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_point = 0;
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_status = 0;

    memset(&beacon_loss_info, 0, sizeof(beacon_loss_info));
    // beacon_loss_info.bcn_loss_cfg_table=bcn_loss_cfg;
    iot2lp_para->bcn_loss_info = &beacon_loss_info;
    // bl_lp_fw_bcn_loss_init();
    iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
    iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
    bl_lp_fw_bcn_loss_cfg_dtim_default(10 /*iot2lp_para->wifi_parameter->dtim_num*/);

    memset(&lp_fw_jtag_parameter, 0, sizeof(lp_fw_jtag_parameter));
    lp_fw_jtag_parameter.jtag_en = 0;
    lp_fw_jtag_parameter.jtag_io[0] = 0;
    lp_fw_jtag_parameter.jtag_io[1] = 1;
    lp_fw_jtag_parameter.jtag_io[2] = 2;
    lp_fw_jtag_parameter.jtag_io[3] = 3;
    iot2lp_para->jtag_parameter = &lp_fw_jtag_parameter;

    memset(&lp_fw_uart_cfg, 0, sizeof(lp_fw_uart_cfg));
    lp_fw_uart_cfg.debug_log_en = 0;
    lp_fw_uart_cfg.uart_tx_io = 34;
    lp_fw_uart_cfg.uart_rx_io = 35;
    lp_fw_uart_cfg.baudrate = 2000000;
    iot2lp_para->uart_config = &lp_fw_uart_cfg;

    memset(&lp_fw_clock_cfg, 0, sizeof(lp_fw_clock_cfg));
    lp_fw_clock_cfg.mcu_clk_sel = GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M;
    lp_fw_clock_cfg.hclk_div = 1;
    lp_fw_clock_cfg.bclk_div = 0;
    lp_fw_clock_cfg.xclk_sel = HBN_MCU_XCLK_XTAL;
    iot2lp_para->clock_config = &lp_fw_clock_cfg;


    iot2lp_para->dcdc_sel_pin = BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED;

    /* lpfw info */
    memset(&lp_info_struct, 0, sizeof(lp_info_struct));
    iot2lp_para->lp_info = &lp_info_struct;
    bl_lp_info_clear(&lp_info_struct);

    bl_lp_xip_para_save();

#ifdef CONFIG_PSRAM
    bl_lp_psram_para_save();
#endif

#ifndef CONFIG_LPFW_INIT_LOAD
    /* First load */
    bl_lpfw_ram_load();
    if (bl_lpfw_ram_verify() < 0) {
        assert(0);
    }
#endif

    BL_LP_LOG("[LP] init finish\r\n");
}

int bl_lp_init(void)
{
    bl_lp_env_init();

    bl_lp_fw_init();

    return 0;
}

/* Virtual time functions */
static void bl_lp_vtime_before_sleep(void)
{
    uint64_t rtc_cnt, mtimer_cnt;

    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    mtimer_cnt = CPU_Get_MTimer_Counter();

    /* update timestamp */
    g_rtc_timestamp_before_sleep_us = BL_PDS_CNT_TO_US(rtc_cnt);
    g_mtimer_timestamp_before_sleep_us = mtimer_cnt;

    /* update virtual time base */
    g_virtual_timestamp_base_us += g_mtimer_timestamp_before_sleep_us - g_mtimer_timestamp_after_sleep_us;
}

static void bl_lp_vtime_after_sleep(void)
{
    uint64_t rtc_cnt, mtimer_cnt;

    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    mtimer_cnt = CPU_Get_MTimer_Counter();

    /* update timestamp */
    g_rtc_timestamp_after_sleep_us = BL_PDS_CNT_TO_US(rtc_cnt);
    g_mtimer_timestamp_after_sleep_us = mtimer_cnt;

    /* update virtual time base */
    g_virtual_timestamp_base_us += g_rtc_timestamp_after_sleep_us - g_rtc_timestamp_before_sleep_us;
}

uint64_t bl_lp_get_virtual_us(void)
{
    uint64_t mtimer_cnt;

    mtimer_cnt = CPU_Get_MTimer_Counter();

    return (g_virtual_timestamp_base_us + mtimer_cnt - g_mtimer_timestamp_after_sleep_us);
}

static uint32_t get_dtim_num(uint32_t period_dtim, uint32_t bcn_past_num, lp_fw_wifi_para_t *wifi_param)
{
    uint32_t dtim_num;
    uint8_t next_dtim_count;

    if (wifi_param->bcmc_dtim_mode) {
        dtim_num = period_dtim * wifi_param->beacon_dtim_period;

        dtim_num = dtim_num - (bcn_past_num % dtim_num);

        next_dtim_count =
            (wifi_param->last_beacon_dtim_count + bcn_past_num + dtim_num) % wifi_param->beacon_dtim_period;
        if (next_dtim_count) {
            if (dtim_num + next_dtim_count > wifi_param->beacon_dtim_period) {
                dtim_num = dtim_num + next_dtim_count - wifi_param->beacon_dtim_period;
            } else {
                dtim_num = dtim_num + next_dtim_count;
            }
        }
    } else {
        dtim_num = period_dtim - (bcn_past_num % period_dtim);
    }

    return dtim_num;
}


/* Get flash IO select from efuse */
static uint32_t ATTR_TCM_SECTION get_sf_pin_select(void)
{
    uint32_t tmpVal;
    /* Read from efuse sw usage 0 */
    tmpVal = BL_RD_WORD(0x20056000 + 0x74);
    return (tmpVal >> 5) & 0x3f;
}

/* Check if the pin is flash IO */
static int ATTR_TCM_SECTION is_flash_io(uint8_t pin, uint32_t sf_pin_select)
{
    if (sf_pin_select & (1 << 2)) {
        /* Flash IO is GPIO 6-11 */
        return (pin >= GPIO_PIN_6 && pin <= GPIO_PIN_11);
    } else if (sf_pin_select & (1 << 3)) {
        /* Flash IO is GPIO 24-29 */
        return (pin >= GPIO_PIN_24 && pin <= GPIO_PIN_29);
    }
    return 0;
}

static int ATTR_TCM_SECTION is_psram_io(uint8_t pin)
{
    return (pin >= 46 && pin <= 57);
}

static int ATTR_TCM_SECTION is_uart_io(uint8_t pin)
{
    if (lp_fw_uart_cfg.debug_log_en == 1) {
        return (pin == lp_fw_uart_cfg.uart_tx_io) || (pin == lp_fw_uart_cfg.uart_rx_io);
    } else {
        return 0;
    }
}

/* Configure GPIO for PDS low power and enable keep */
static void ATTR_TCM_SECTION bl_pds_gpio_keep_enable(void)
{
    uint32_t sf_pin_select;

    /* Get flash IO configuration and configure GPIO for low power */
    sf_pin_select = get_sf_pin_select();

    gpio = bflb_device_get_by_name("gpio");

    /* Enable all PDS GPIO keep for low power consumption */
    for (uint8_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
        if (!is_flash_io(i, sf_pin_select) && !is_psram_io(i) && !is_uart_io(i)) {
            if (bl_ext_dcdc_pds_is_keep_pin(i)) {
                PDS_Enable_GPIO_Keep(i);
                continue;
            }
            bflb_gpio_init(gpio, i, GPIO_ANALOG | GPIO_FLOAT | GPIO_DRV_0);
            PDS_Enable_GPIO_Keep(i);
        }
    }
}

static int board_dcdc_enter_pds(void *arg)
{
    uint8_t pin = (uintptr_t)arg;

    HBN_SW_Set_Ldo_Soc_Vout(HBN_LDO_SOC_LEVEL_0P900V);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);
    GLB_Set_MCU_System_CLK_Div(1, 0);

    if (gpio == NULL) {
        gpio = bflb_device_get_by_name("gpio");
    }
    
    pm_disable_gpio_keep(pin);

    bflb_gpio_init(gpio, pin, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_reset(gpio, pin);

    PDS_Enable_GPIO_Keep(pin);

    arch_delay_us(100);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);

    return 0;
}

static int board_dcdc_exit_pds(void *arg)
{
    uint8_t pin = (uintptr_t)arg;

    if (gpio == NULL) {
        gpio = bflb_device_get_by_name("gpio");
    }
    
    pm_disable_gpio_keep(pin);

    bflb_gpio_init(gpio, pin, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);

    bflb_gpio_set(gpio, pin);

    PDS_Enable_GPIO_Keep(pin);

    return 0;
}

void board_ext_dcdc_init(void)
{
    iot2lp_para->dcdc_sel_pin = BL_EXT_DCDC_OUTPUT_CTRL_PIN;

    if (BL_EXT_DCDC_OUTPUT_CTRL_PIN == BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED) {
        return;
    }

    bl_ext_dcdc_pds_cfg_t cfg = {
        .keep_pin_num = 1,
        .keep_pins = { BL_EXT_DCDC_OUTPUT_CTRL_PIN },
        .settle_time_us = 200,
        .enter_pds_cb = board_dcdc_enter_pds,
        .enter_pds_arg = (void *)(uintptr_t)BL_EXT_DCDC_OUTPUT_CTRL_PIN,
        .exit_pds_cb = board_dcdc_exit_pds,
        .exit_pds_arg = (void *)(uintptr_t)BL_EXT_DCDC_OUTPUT_CTRL_PIN,
    };

    /* Enable GPIO select EXT DCDC output */
    bl_ext_dcdc_pds_register(&cfg);
}

int ATTR_TCM_SECTION bl_lp_fw_enter(bl_lp_fw_cfg_t *bl_lp_fw_cfg)
{
    uint64_t rtc_sleep_us, rtc_wakeup_cmp_cnt;
    uint32_t dtim_num, bcn_past_num, beacon_interval_us;
    uint64_t last_beacon_rtc_us = 0;
    uint64_t rtc_cnt, rtc_now_us;
    uint32_t pds_sleep_us;
    uint32_t period_dtim;
    uint32_t bcn_loss_level;
    int32_t rtc32k_error_us;
    uint32_t total_error;

    lp_fw_bcn_loss_level_t *bcn_loss_cfg = NULL;

    if (bl_lp_fw_cfg == NULL) {
        return -1;
    }

    board_ext_dcdc_init();

    bl_lp_debug_record_time(iot2lp_para, "bl_lp_fw_enter");

    // rtc_wakeup_cmp_cnt = bl_lp_fw_cfg->rtc_wakeup_cmp_cnt;
    // rtc_sleep_us = bl_lp_fw_cfg->rtc_timeout_us;

     /* clean wakeup reason */
    memset(&wakeup_reason, 0, sizeof(wakeup_reason));
    iot2lp_para->wakeup_reason_info = &wakeup_reason;

    /* clean wakeup source */
    iot2lp_para->wakeup_source_parameter->rtc_wakeup_en = 0;
    iot2lp_para->wakeup_source_parameter->rtc_wakeup_cnt = 0;

    LP_HOOK(pre_user, bl_lp_fw_cfg);

    if (bl_ext_dcdc_pds_prepare_enter() != 0) {
        BL_LP_LOG("---- dcdc enter ----\r\n");

        return -1;
    }

    __disable_irq();

    LP_HOOK(pre_sys, bl_lp_fw_cfg);

    if (bl_lp_fw_cfg->lpfw_copy) {
        bl_lpfw_ram_load();
    }

    if (bl_lp_fw_cfg->lpfw_verify) {
        /* ensure integrity of lpfw  */
        if (bl_lpfw_ram_verify() < 0) {
            assert(0);
        }
    }

    bl_lp_wifi_param_update(bl_lp_fw_cfg);


    // uint32_t sf_pin_select;

    /* Get flash IO configuration and configure GPIO for low power */
    // sf_pin_select = get_sf_pin_select();
    // pds_gpio_keep_enable(sf_pin_select);

   /* ready sleep*/
    iot2lp_para->wakeup_flag = 0;
    iot2lp_para->pattern = 0xAA5555AA;
    iot2lp_para->app_entry = (uintptr_t)lp_fw_restore_cpu_para;
    iot2lp_para->args[0] = GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR;
    iot2lp_para->wakeup_reason_info->wakeup_reason = LPFW_WAKEUP_UNKOWN;

   /* cacheable */
    pm_set_wakeup_callback(
        (void (*)(void))((uint32_t)__lpfw_share_start__ | 0x60000000) /*(void (*)(void))LP_FW_PRE_JUMP_ADDR*/);

    LP_HOOK(pre_sleep, iot2lp_para);

    HBN_Pin_WakeUp_Mask(0xFF);

    bl616cl_lp_io_wakeup_prepare();

    bl_pds_gpio_keep_enable();

    // PDS_Set_All_GPIO_Pad_IntClr();

    /* app to sleep_pds, update time_info */
    bl_lp_time_info_update_app(iot2lp_para->lp_info);

    bl_lp_vtime_before_sleep();

    csi_dcache_clean();

    /* Gets the current rtc time */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    rtc_wakeup_cmp_cnt = bl_lp_fw_cfg->rtc_wakeup_cmp_cnt;
    rtc_sleep_us = bl_lp_fw_cfg->rtc_timeout_us;

    if (bl_lp_fw_cfg->tim_wakeup_en) {
        /* last beacon timestamp */
        last_beacon_rtc_us = iot2lp_para->last_beacon_stamp_rtc_us;
        /* beacon delay */
        last_beacon_rtc_us -= iot2lp_para->bcn_delay_info->last_beacon_delay_us;

        /* get period of dtim */
        if (iot2lp_para->bcn_loss_info->continuous_loss_cnt == 0) {
            period_dtim = iot2lp_para->wifi_parameter->dtim_num;
        } else {
            bcn_loss_level = iot2lp_para->bcn_loss_info->bcn_loss_level;
            bcn_loss_cfg = &(iot2lp_para->bcn_loss_info->bcn_loss_cfg_table[bcn_loss_level]);
            period_dtim = bcn_loss_cfg->dtim_num;
        }

        /* beacon interval us, TU * 1024 */
        beacon_interval_us = iot2lp_para->wifi_parameter->beacon_interval_tu * 1024;
        bcn_past_num = (rtc_now_us - last_beacon_rtc_us + PROTECT_BF_MS * 1000) / beacon_interval_us;
        dtim_num = get_dtim_num(period_dtim, bcn_past_num, iot2lp_para->wifi_parameter);


        // BL_LP_LOG("period_dtim:%d\r\n", period_dtim);
        // BL_LP_LOG("dtim_num:%d\r\n", dtim_num);
        // BL_LP_LOG("bcn_past_num:%d\r\n", bcn_past_num);
        // BL_LP_LOG("beacon_interval_us:%d\r\n", beacon_interval_us);
        // BL_LP_LOG("rc32k code %d\r\n", iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext);
        /* last bcn -> next bcn */
        pds_sleep_us = (dtim_num + bcn_past_num) * beacon_interval_us;

        /* rc32k error compensation (last bcn -> next bcn) */
        rtc32k_error_us =
            (int32_t)((int64_t)pds_sleep_us * iot2lp_para->rc32k_trim_parameter->rtc32k_error_ppm / (1000 * 1000));

        /* now -> next bcn */
        pds_sleep_us = pds_sleep_us - (uint32_t)(rtc_now_us - last_beacon_rtc_us);

        /* rc32k error */
        if (rtc32k_error_us > 0 || pds_sleep_us > (-rtc32k_error_us)) {
            pds_sleep_us += rtc32k_error_us;
        } else {
            pds_sleep_us = 0;
        }
    } else {
        pds_sleep_us = 0;
    }

    if (bcn_loss_cfg) {
        if (pds_sleep_us > bcn_loss_cfg->win_extend_start_us) {
            pds_sleep_us -= bcn_loss_cfg->win_extend_start_us;
        } else {
            pds_sleep_us = 0;
        }
    }


    if (iot2lp_para->last_beacon_stamp_rtc_valid) {
        /* total error form (lasst bcn -> next bcn), jitter error compensation (ppm) */
        total_error = (int64_t)((rtc_now_us - last_beacon_rtc_us) + pds_sleep_us) * iot2lp_para->rtc32k_jitter_error_ppm / (1000 * 1000);
    } else {
        /* total error form (now -> next bcn), jitter error compensation (ppm) */
        total_error = (int64_t)pds_sleep_us * iot2lp_para->rtc32k_jitter_error_ppm / (1000 * 1000);
    }
    if (total_error > 2 * 1000) {
        total_error = 2 * 1000;
    }
    if (pds_sleep_us > total_error) {
        pds_sleep_us -= total_error;
    } else {
        pds_sleep_us = 0;
    }

    /* Recorded the error value, for mtimer timeout */
    iot2lp_para->last_sleep_error_us = total_error;



    if ((rtc_wakeup_cmp_cnt == 0) && rtc_sleep_us > ((uint64_t)24 * 60 * 60 * 1000 * 1000)) {
        rtc_sleep_us = ((uint64_t)24 * 60 * 60 * 1000 * 1000);
    }

    /* Prevent rtc from colliding with pds */
    if (rtc_wakeup_cmp_cnt == 0 && rtc_sleep_us) {
        /*  */
        if (((pds_sleep_us >= rtc_sleep_us) && (pds_sleep_us - rtc_sleep_us) < PROTECT_BF_MS * 1000) ||
            ((pds_sleep_us <= rtc_sleep_us) && (rtc_sleep_us - pds_sleep_us) < PROTECT_AF_MS * 1000)) {
            /* Advance the RTC time */
            if (pds_sleep_us > PROTECT_AF_MS * 1000) {
                rtc_sleep_us = pds_sleep_us - PROTECT_AF_MS * 1000;
            } else {
                rtc_sleep_us = 0;
            }
            BL_LP_LOG("---- Advance of rtc ----\r\n");
        }
    }

    csi_dcache_clean();

    BL_LP_LOG("pds_sleep_us:%ld\r\n", pds_sleep_us);
    // BL_LP_LOG("last_beacon_stamp_rtc_us: %lld\r\n", iot2lp_para->last_beacon_stamp_rtc_us);
    // BL_LP_LOG("stamp_rtc_valid: %ld\r\n", iot2lp_para->last_beacon_stamp_rtc_valid);
    // BL_LP_LOG("rtc_wakeup_cmp_cnt:%lld\r\n", rtc_wakeup_cmp_cnt);
    BL_LP_LOG("rtc_sleep_us:%lld\r\n", rtc_sleep_us);
    // BL_LP_LOG("rc32k code %ld\r\n", iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext);
    // BL_LP_LOG("rtc ppm %ld\r\n", iot2lp_para->rc32k_trim_parameter->rtc32k_error_ppm);
    arch_delay_us(500);
    pds_sleep_us -= 500;

    /* Check iot2lp_para pointer variables for valid values */
    if (bl_lp_check_iot2lp_para_pointers(iot2lp_para) != 0) {
        assert(0);
    }

    /* Check buf_addr when bcmc_dtim_mode is enabled */
    if (iot2lp_para->wifi_parameter->bcmc_dtim_mode && iot2lp_para->wifi_parameter->buf_addr == 0) {
        assert(0);
    }


    bl_lp_debug_record_time(iot2lp_para, "lp_fw_save_cpu_para");

    lp_fw_save_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);

    if (iot2lp_para->wakeup_flag == 0) {

        if (rtc_wakeup_cmp_cnt || rtc_sleep_us) {
            /* rtc cfg */
            rtc_wakeup_init(rtc_wakeup_cmp_cnt, rtc_sleep_us);
            /* clear rtc IRQ */
            HBN_Clear_IRQ(HBN_INT_RTC);
            /* enable rtc wakeup source */
            PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
        } else {
            /* disable rtc comp */
            uint32_t tmpVal;
            tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
            tmpVal = tmpVal & 0xfffffff1;
            BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);
        }

        if (bl_lp_fw_cfg->tim_wakeup_en) {
            /* pds15 enter */
            pm_pds_mode_enter(PM_PDS_LEVEL_15, BL_US_TO_PDS_CNT(pds_sleep_us));
        } else {
            /* pds15 enter */
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
        }


        iot2lp_para->wakeup_flag = 1;
        lp_fw_restore_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);
    }

#if defined(__riscv_flen)
    __asm__ __volatile__("lw      x0, 0(a0)\n\t"
                         /* FP: initial state */
                         "csrr    t0, mstatus\n\t"
                         "li      t1, ~0x6000\n\t"
                         "and     t0, t0, t1\n\t"
                         "li      t1, 0x2000\n\t"
                         "or      t0, t0, t1\n\t"
                         "csrw    mstatus, t0\n\t");
    /* csrwi   fcsr, 0 */
#endif
    /* enable mstatus FS */
    uint32_t mstatus = __get_MSTATUS();
    mstatus |= (1 << 13);
    __set_MSTATUS(mstatus);

    /* enable mxstatus THEADISAEE */
    uint32_t mxstatus = __get_MXSTATUS();
    mxstatus |= (1 << 22);
    /* enable mxstatus MM */
    mxstatus |= (1 << 15);
    __set_MXSTATUS(mxstatus);

    /* get interrupt level from info */
    CLIC->CLICCFG =
        (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    /* tspend interrupt will be clear auto*/
    /* tspend use positive interrupt */
#ifdef CONFIG_IRQ_USE_VECTOR
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x3;
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x3;
#else
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x2;
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x2;
#endif

    /* disable mexstatus SPUSHEN and SPSWAPEN for ipush/ipop*/
    uint32_t mexstatus = __get_MEXSTATUS();
    mexstatus &= ~(0x3 << 16);
    __set_MEXSTATUS(mexstatus);


    bl_lp_vtime_after_sleep();


    bl_lp_debug_record_time(iot2lp_para, "return APP");

    if ((iot2lp_para->wakeup_reason_info->wakeup_reason & LPFW_WAKEUP_IO)) {
        for (uint8_t i = HBN_INT_GPIO0; i <= HBN_INT_GPIO5; i++) {
            if (SET == HBN_Get_INT_State(i)) {
                BL_LP_LOG("[E] gpio %d\r\n",i);
                // iot2lp_para->wakeup_reason_info->wakeup_io_bits |= ((uint64_t)1<<i);
                HBN_Clear_IRQ(i);
            }
        }
        for (uint32_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
            if (PDS_Get_GPIO_Pad_IntStatus(i)) {
                BL_LP_LOG("[E] gpio %d\r\n",i);
                // iot2lp_para->wakeup_reason_info->wakeup_io_bits |= ((uint64_t)1<<i);
                PDS_Set_GPIO_Pad_IntClr(i);
            }
        }

        bl616cl_lp_soft_irq_trigger();
    }

    /* Disable GPIO keep leave to the business layer */
    // PDS_Disable_ALL_GPIO_Keep();
    /* clock re-init */

    /* watch-dog disable */

    /* rc32k code restored */
    HBN_Set_RC32K_R_Code(iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext);

    /* lpfw firmware has already set the DCDC voltage */
    // bl_ext_dcdc_pds_restore_exit();

    LP_HOOK(post_sys, iot2lp_para);

    bl_lp_debug_record_time(iot2lp_para, "bl_lp_fw_enter end");

#if (BL_LP_TIME_DEBUG)
    bl_lp_debug_dump_time(iot2lp_para);
#endif

    return iot2lp_para->wakeup_reason_info->wakeup_reason;
}

int bl_lp_get_wake_reason(void)
{
    return (int)iot2lp_para->wakeup_reason_info->wakeup_reason;
}

void ATTR_TCM_SECTION bl_lp_debug_record_time(iot2lp_para_t *iot_lp_para, char *info_str)
{
#if BL_LP_TIME_DEBUG
    uint64_t rtc_cnt, rtc_now_us;
    lp_fw_time_debug_t *time_debug = iot_lp_para->time_debug;

    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    for (uint8_t i = 0; i < TIME_DEBUG_NUM_MAX; i++) {
        if (time_debug[i].trig_cnt == 0 || time_debug[i].info_str == info_str) {
            time_debug[i].time_stamp_us = rtc_now_us;
            time_debug[i].info_str = info_str;
            time_debug[i].trig_cnt++;
            break;
        }
    }
#endif
}

void bl_lp_debug_clean_time(iot2lp_para_t *iot_lp_para)
{
#if BL_LP_TIME_DEBUG
    memset(iot_lp_para->time_debug, 0, sizeof(lp_fw_time_debug_t) * TIME_DEBUG_NUM_MAX);
#endif
}

void bl_lp_debug_dump_time(iot2lp_para_t *iot_lp_para)
{
#if BL_LP_TIME_DEBUG
    lp_fw_time_debug_t *time_debug = iot_lp_para->time_debug;

    BL_LP_LOG("time debug dump, buff_addr 0x%08X\r\n", (uint32_t)time_debug);

    for (int i = 0; i < TIME_DEBUG_NUM_MAX; i++) {
        if (time_debug[i].trig_cnt == 0) {
            break;
        }

        BL_LP_LOG("time debug[%d] = %lld, cnt: %ld", i, time_debug[i].time_stamp_us, time_debug[i].trig_cnt);

        if (i > 0) {
            BL_LP_LOG(", diff %lld", time_debug[i].time_stamp_us - time_debug[i - 1].time_stamp_us);
        }

        if (time_debug[i].info_str) {
            BL_LP_LOG(", \tinfo: %s", time_debug[i].info_str);
        }

        BL_LP_LOG(";\r\n");
    }

    BL_LP_LOG("\r\ndump end\r\n");
#endif
}
