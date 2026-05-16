#include "bl_lp_internal.h"

static ATTR_NOCACHE_RAM_SECTION struct bl_lp_info_s lp_info_struct = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_rc32k_trim_t rc32k_trim_parameter = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_mtimer_t mtimer_info = { 0 };
#if (BL_LP_TIME_DEBUG)
static ATTR_NOCACHE_RAM_SECTION lp_fw_time_debug_t time_debug_buff[TIME_DEBUG_NUM_MAX] = { 0 };
#endif

bl_lp_fw_cfg_t lpfw_cfg = {
    .tim_wakeup_en = 1,
    .rtc_timeout_us = (60 * 1000 * 1000),
    .dtim_origin = BL_DTIM_NUM,
    .dtim_num = 0,
};

ATTR_TCM_CONST_SECTION iot2lp_para_t *const iot2lp_para = (iot2lp_para_t *)IOT2LP_PARA_ADDR;

static uint64_t g_rtc_timestamp_before_sleep_us = 0;    /* rtc 1 */
static uint64_t g_rtc_timestamp_after_sleep_us = 0;     /* rtc 2 */
static uint64_t g_mtimer_timestamp_before_sleep_us = 0; /* mtime 1 */
static uint64_t g_mtimer_timestamp_after_sleep_us = 0;  /* mtime 2 */
static uint64_t g_virtual_timestamp_base_us = 0;        /* virtual time base */

static void bl_lp_vtime_before_sleep(void);
static void bl_lp_vtime_after_sleep(void);

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
    CHECK_IOT2LP_POINTER(shared_func_p);
    CHECK_IOT2LP_POINTER(flash_parameter);
    CHECK_IOT2LP_POINTER(sec_cfg);
    CHECK_IOT2LP_POINTER(tzc_cfg);
    
#if (BL_LP_TIME_DEBUG)
    CHECK_IOT2LP_POINTER(time_debug);
#endif
    return 0;
}

/* jump to lpfw form bootrom */
ATTR_HBN_CODE_SECTION void lp_fw_pre(void)
{
    uint32_t tmpVal;

    /* xclk  select XTAL40M */
    (*(volatile uint32_t *)0x2000f030) |= (1 << 0);

    /* mtimer CLk set 1M = 40M/40 */
    (*(volatile uint32_t *)0x20009014) &= ~(0x3FF << 0);
    (*(volatile uint32_t *)0x20009014) |= (39 << 0);

    /* mtimer cnt reset */
    (*(volatile uint32_t *)0x20009014) |= (1 << 30);
    (*(volatile uint32_t *)0x20009014) &= ~(1 << 30);

    /* Set bus remap */
    tmpVal = BL_RD_REG(TZ1_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL);
    tmpVal = BL_SET_REG_BIT(tmpVal, TZC_SEC_TZC_BUS_RMP_EN);
    BL_WR_REG(TZ1_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL, tmpVal);

    /* Set Lock */
    tmpVal = BL_RD_REG(TZ1_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL);
    tmpVal = BL_SET_REG_BIT(tmpVal, TZC_SEC_TZC_BUS_RMP_EN_LOCK);
    BL_WR_REG(TZ1_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL, tmpVal);

    BL_WR_REG(HBN_BASE, HBN_RSV1, 0);
    BL_WR_REG(HBN_BASE, HBN_RSV0, 0);

    /* jump to lp_fw */
    void (*pFunc)(void);
    uint32_t lpfw_addr = ((uint32_t)__lpfw_share_start__ & 0x0FFFFFFF) | 0x60000000; /* cacheable */
    pFunc = (void (*)(void))lpfw_addr;
    pFunc();
}

extern uint32_t __hbn_ram_load__;
extern uint32_t __hbn_ram_start__;
extern uint32_t __hbn_ram_end__;

/* load hbn ram */
static void bl616_load_hbn_ram(void)
{
    uint32_t *pSrc, *pDest;

    /* BF Add HBNRAM data copy */
    pSrc = &__hbn_ram_load__;
    pDest = &__hbn_ram_start__;
    uint8_t i = 0;

    for (; pDest < &__hbn_ram_end__;) {
        BL_LP_LOG("0x%08x, ", *pSrc);
        *pDest++ = *pSrc++;
        i++;
        if (!(i % 8)) {
            BL_LP_LOG("\r\n");
        }
    }
}

#define GET_OFFSET(_type, _member) ((unsigned long)(&((_type *)0)->_member))

void lp_fw_save_cpu_para(uint32_t save_addr)
{
    __asm__ __volatile__(
        "sw      x0, 0(a0)\n\t"
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
void lp_fw_restore_cpu_para(uint32_t save_addr)
{
    __asm__ __volatile__(
        "lw      x0, 0(a0)\n\t"
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
        "la      a0, freertos_risc_v_trap_handler\n\t" // mcu default_trap_handler
        "ori     a0, a0, 3\n\t"
        "csrw    mtvec, a0\n\t");
}

static void rtc_wakeup_init(uint64_t rtc_wakeup_cmp_cnt, uint64_t sleep_us)
{
    uint32_t tmpVal;
    uint64_t rtc_cnt;

    if (sleep_us == 0) {
        return;
    }

    /* Set RTC compare mode, and enable rtc */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = tmpVal & 0xfffffff1;
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal | 0x00000003);

    if (rtc_wakeup_cmp_cnt == 0) {
        /* Tigger RTC val latch  */
        tmpVal = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H);
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
        BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
        BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);

        /* Read RTC cnt */
        rtc_cnt = (BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H) & 0xff);
        rtc_cnt <<= 32;
        rtc_cnt |= BL_RD_REG(HBN_BASE, HBN_RTC_TIME_L);

        /* calculate RTC Comp cnt */
        rtc_cnt += BL_US_TO_PDS_CNT(sleep_us);
    } else {
        rtc_cnt = rtc_wakeup_cmp_cnt;
    }

    iot2lp_para->rtc_wakeup_cnt = rtc_cnt;
    iot2lp_para->rtc_wakeup_en = 1;

    /* Set RTC Comp cnt */
    BL_WR_REG(HBN_BASE, HBN_TIME_H, (uint32_t)(rtc_cnt >> 32) & 0xff);
    BL_WR_REG(HBN_BASE, HBN_TIME_L, (uint32_t)rtc_cnt);

    /* Set interrupt delay option */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RTC_DLY_OPTION, HBN_RTC_INT_DELAY_0T);
    BL_WR_REG(HBN_BASE, HBN_CTL, (uint32_t)tmpVal);
}

static void bl_lp_fw_rc32k_trim_init(void)
{
    /* rc32k trim parameter initialization */
    memset(&rc32k_trim_parameter, 0, sizeof(rc32k_trim_parameter));
    iot2lp_para->rc32k_trim_parameter = &rc32k_trim_parameter;

    /* Save rc32k code in HBN_RAM */
    iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext = (*((volatile uint32_t *)0x2000F200)) >> 22;
    /* rc32k status: not ready */
    iot2lp_para->rc32k_trim_parameter->rc32k_clock_ready = 0;
    iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready = 0;
    iot2lp_para->rc32k_trim_parameter->rc32k_auto_cal_en = 1;
}
/********************  ********************/

void bl_lp_fw_init(void)
{
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();
    uint32_t chip_version = 0;

    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    printf("[LP] lpfw ram address: 0x%08X -> 0x%08X\r\n", lpfw_info->lpfw_memory_start, lpfw_info->lpfw_memory_end);
    printf("[LP] iot2lp_para size: %d\r\n", sizeof(iot2lp_para_t));

    /* clean iot2lp_para */
    memset(iot2lp_para, 0, (uint32_t)&iot2lp_para->reset_keep - (uint32_t)iot2lp_para);

    /*app/lpfw share func init*/
    shared_func_init();
    iot2lp_para->shared_func_p = (uint32_t *)shared_func_array;

    /* lpfw info */
    memset(&lp_info_struct, 0, sizeof(lp_info_struct));
    iot2lp_para->lp_info = &lp_info_struct;
    bl_lp_info_clear();

#if (BL_LP_TIME_DEBUG)
    memset(time_debug_buff, 0, sizeof(time_debug_buff));
    iot2lp_para->time_debug = time_debug_buff;
#endif
    bl_lp_fw_wifi_init();
    bl_lp_fw_bcn_delay_init();
    bl_lp_fw_bcn_loss_init();

    iot2lp_para->debug_io = 0xFF;

    bl_lp_fw_rc32k_trim_init();

    memset(&mtimer_info, 0, sizeof(mtimer_info));
    iot2lp_para->mtimer_parameter = &mtimer_info;

    /* get chip version*/
    chip_version = BL_RD_WORD(0x90015800);
    if (chip_version == 0x06160001) {
        /* only first version need pre jump */
        bl616_load_hbn_ram();
    } else {
        /* later version use OCRAM for recovery */
        BL_WR_WORD(0x22FC0000, 0x4e42484d);
        BL_WR_WORD(0x22FC0004, 0x4e42484d);
        BL_WR_WORD(0x22FC0008, LP_FW_PRE_JUMP_ADDR);
        /* em-buff need pre jump */
        bl616_load_hbn_ram();
        Tzc_Sec_OCRAM_Access_Set_Advance(0, 0x22FC0000, (0x400), 0x0);
    }

#ifndef CONFIG_LPFW_INIT_LOAD
    /* First load */
    bl_lpfw_ram_load();
    if (bl_lpfw_ram_verify() < 0) {
        assert(0);
    }
#endif

    bl_lp_xip_para_save();
}

/******************** lp info ********************/
/* active_app */
void bl_lp_time_info_update_app(void)
{
    uint64_t now_rtc_cnt, diff_rtc_cnt;
    HBN_Get_RTC_Timer_Val((uint32_t *)&now_rtc_cnt, (uint32_t *)&now_rtc_cnt + 1);
    diff_rtc_cnt = now_rtc_cnt - iot2lp_para->lp_info->time_record_start_rtc_cnt;

    iot2lp_para->lp_info->time_record_start_rtc_cnt = now_rtc_cnt;
    iot2lp_para->lp_info->time_total_rtc_cnt += diff_rtc_cnt;

    iot2lp_para->lp_info->time_active_app_rtc_cnt += diff_rtc_cnt;
}

/* sleep_pds */
void bl_lp_time_info_update_pds(void)
{
    uint64_t now_rtc_cnt, diff_rtc_cnt;
    HBN_Get_RTC_Timer_Val((uint32_t *)&now_rtc_cnt, (uint32_t *)&now_rtc_cnt + 1);
    diff_rtc_cnt = now_rtc_cnt - iot2lp_para->lp_info->time_record_start_rtc_cnt;

    iot2lp_para->lp_info->time_record_start_rtc_cnt = now_rtc_cnt;
    iot2lp_para->lp_info->time_total_rtc_cnt += diff_rtc_cnt;

    iot2lp_para->lp_info->time_sleep_pds_rtc_cnt += diff_rtc_cnt;
}

/* active_lpfw */
void bl_lp_time_info_update_lpfw(void)
{
    uint64_t now_rtc_cnt, diff_rtc_cnt;
    HBN_Get_RTC_Timer_Val((uint32_t *)&now_rtc_cnt, (uint32_t *)&now_rtc_cnt + 1);
    diff_rtc_cnt = now_rtc_cnt - iot2lp_para->lp_info->time_record_start_rtc_cnt;

    iot2lp_para->lp_info->time_record_start_rtc_cnt = now_rtc_cnt;
    iot2lp_para->lp_info->time_total_rtc_cnt += diff_rtc_cnt;

    iot2lp_para->lp_info->time_active_lpfw_rtc_cnt += diff_rtc_cnt;
}

void bl_lp_info_get(bl_lp_info_t *lp_info)
{
    /* lpfw wakeup cnt */
    lp_info->lpfw_wakeup_cnt = iot2lp_para->lpfw_wakeup_cnt;

    /* bcn loss info */
    lp_info->lpfw_recv_cnt = iot2lp_para->lp_info->bcn_lpfw_recv_cnt;
    lp_info->lpfw_loss_cnt = iot2lp_para->lp_info->bcn_lpfw_loss_cnt;

    /* update active_app time */
    bl_lp_time_info_update_app();

    /* time info */
    lp_info->time_total_us = BL_PDS_CNT_TO_US(iot2lp_para->lp_info->time_total_rtc_cnt);
    lp_info->sleep_pds_us = BL_PDS_CNT_TO_US(iot2lp_para->lp_info->time_sleep_pds_rtc_cnt);
    lp_info->active_lpfw_us = BL_PDS_CNT_TO_US(iot2lp_para->lp_info->time_active_lpfw_rtc_cnt);
    lp_info->active_app_us = BL_PDS_CNT_TO_US(iot2lp_para->lp_info->time_active_app_rtc_cnt);
}

void bl_lp_info_clear(void)
{
    /* bcn */
    iot2lp_para->lp_info->bcn_lpfw_recv_cnt = 0;
    iot2lp_para->lp_info->bcn_lpfw_loss_cnt = 0;

    /* time */
    uint64_t rtc_cnt_now;
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt_now, (uint32_t *)&rtc_cnt_now + 1);
    iot2lp_para->lp_info->time_total_rtc_cnt = 0;
    iot2lp_para->lp_info->time_sleep_pds_rtc_cnt = 0;
    iot2lp_para->lp_info->time_active_lpfw_rtc_cnt = 0;
    iot2lp_para->lp_info->time_active_app_rtc_cnt = 0;
    iot2lp_para->lp_info->time_record_start_rtc_cnt = rtc_cnt_now;
}

/******************** lp info end ********************/

void bl_lp_fw_bcn_tpre_cfg(int32_t tpre_us)
{
    iot2lp_para->tpre = tpre_us;
}

void bl_lp_debug_record_time(iot2lp_para_t *iot_lp_para, char *info_str)
{
#if BL_LP_TIME_DEBUG
    uint64_t rtc_cnt, rtc_now_us;
    lp_fw_time_debug_t *time_debug = iot_lp_para->time_debug;

    if (!time_debug) {
        return;
    }

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
    if (!iot_lp_para->time_debug) {
        return;
    }
    memset(iot_lp_para->time_debug, 0, sizeof(lp_fw_time_debug_t) * TIME_DEBUG_NUM_MAX);
#endif
}

void bl_lp_debug_dump_time(iot2lp_para_t *iot_lp_para)
{
#if BL_LP_TIME_DEBUG
    lp_fw_time_debug_t *time_debug = iot_lp_para->time_debug;

    BL_LP_LOG("time debug dump, buff_addr 0x%08X\r\n", (uint32_t)time_debug);

    if (!time_debug) {
        return;
    }

    for (int i = 0; i < 8; i++) {
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

int ATTR_TCM_SECTION bl_lp_fw_enter(bl_lp_fw_cfg_t *bl_lp_fw_cfg)
{
    // uintptr_t dst_addr = LP_FW_START_ADDR;
    // uint32_t lpfw_size = *((uint32_t *)__lpfw_load_addr - 7);

    uint32_t dtim_num, bcn_past_num, bcn_loss_level;
    lp_fw_bcn_loss_level_t *bcn_loss_cfg = NULL;

    uint64_t rtc_cnt, rtc_now_us, last_beacon_rtc_us;
    uint32_t pds_sleep_us, beacon_interval_us, total_error;
    int32_t rtc32k_error_us;
    uint64_t rtc_sleep_us, rtc_wakeup_cmp_cnt;

    if (bl_lp_fw_cfg == NULL) {
        return -1;
    }

    rtc_wakeup_cmp_cnt = bl_lp_fw_cfg->rtc_wakeup_cmp_cnt;
    rtc_sleep_us = bl_lp_fw_cfg->rtc_timeout_us;

    if ((bl_lp_fw_cfg->tim_wakeup_en == 0) && (rtc_wakeup_cmp_cnt == 0) && (rtc_sleep_us == 0)) {
        /* no wake-up source */
        return -2;
    }

    bl_lp_debug_record_time(iot2lp_para, "bl_lp_fw_enter");

    iot2lp_para->wifi_parameter->buf_addr = bl_lp_fw_cfg->buf_addr;
    iot2lp_para->wifi_parameter->pack_env = bl_lp_fw_cfg->pack_env;
    iot2lp_para->wifi_parameter->bcmc_dtim_mode = bl_lp_fw_cfg->bcmc_dtim_mode;

    /* clean wake bits */
    iot2lp_para->wake_io_bits = 0;
    iot2lp_para->wake_acomp_bits = 0;
    iot2lp_para->wake_io_edge_bits = 0;
    iot2lp_para->wake_acomp_edge_bits = 0;

    LP_HOOK(pre_user, bl_lp_fw_cfg);

    __disable_irq();

    LP_HOOK(pre_sys, bl_lp_fw_cfg);

    rtc_sleep_us = bl_lp_fw_cfg->rtc_timeout_us;

    if (bl_lp_fw_cfg->lpfw_copy) {
        bl_lpfw_ram_load();
    }

    if (bl_lp_fw_cfg->lpfw_verify) {
        /* ensure integrity of lpfw  */
        if (bl_lpfw_ram_verify() < 0) {
            assert(0);
        }
    }

    iot2lp_para->wakeup_reason = LPFW_WAKEUP_UNKOWN;
    iot2lp_para->rtc_wakeup_en = 0;
    iot2lp_para->rtc_wakeup_cnt = 0;

    if ((rtc_wakeup_cmp_cnt == 0) && rtc_sleep_us > ((uint64_t)24 * 60 * 60 * 1000 * 1000)) {
        rtc_sleep_us = ((uint64_t)24 * 60 * 60 * 1000 * 1000);
    }

    /* tim interval */
    if (bl_lp_fw_cfg->dtim_num != 0) {
        iot2lp_para->wifi_parameter->dtim_num = bl_lp_fw_cfg->dtim_num;
    } else {
        iot2lp_para->wifi_parameter->dtim_num = 10;
    }

    if (iot2lp_para->wifi_parameter->beacon_leg_rate == 0) {
        iot2lp_para->wifi_parameter->beacon_leg_rate = 2; //1M
    }

    /* beacon interval us, TU * 1024 */
    beacon_interval_us = iot2lp_para->wifi_parameter->beacon_interval_tu * 1024;

    /* get period of dtim */
    if (iot2lp_para->bcn_loss_info->continuous_loss_cnt == 0) {
        dtim_num = iot2lp_para->wifi_parameter->dtim_num;
    } else {
        bcn_loss_level = iot2lp_para->bcn_loss_info->bcn_loss_level;
        bcn_loss_cfg = &(iot2lp_para->bcn_loss_info->bcn_loss_cfg_table[bcn_loss_level]);
        dtim_num = bcn_loss_cfg->dtim_num;
    }

    // dtim_period_us = dtim_num * beacon_interval_us;

    /* last beacon timestamp */
    last_beacon_rtc_us = iot2lp_para->last_beacon_stamp_rtc_us;
    /* beacon delay */
    last_beacon_rtc_us -= iot2lp_para->bcn_delay_info->last_beacon_delay_us;

    bflb_l1c_dcache_clean_all();

    /* Gets the current rtc time */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    /* calculate pds and rtc sleep time */
    if (iot2lp_para->last_beacon_stamp_rtc_valid) {
        /* beacon num form (last bcn -> now) */
        bcn_past_num = (rtc_now_us - last_beacon_rtc_us + PROTECT_BF_MS * 1000) / beacon_interval_us;

        if (iot2lp_para->wifi_parameter->bcmc_dtim_mode) {
            dtim_num = dtim_num * iot2lp_para->wifi_parameter->beacon_dtim_period;

            dtim_num = dtim_num - (bcn_past_num % dtim_num);

            uint8_t next_dtim_count = (iot2lp_para->wifi_parameter->last_beacon_dtim_count + bcn_past_num + dtim_num) % iot2lp_para->wifi_parameter->beacon_dtim_period;
            if (next_dtim_count) {
                if (dtim_num + next_dtim_count > iot2lp_para->wifi_parameter->beacon_dtim_period) {
                    dtim_num = dtim_num + next_dtim_count - iot2lp_para->wifi_parameter->beacon_dtim_period;
                } else {
                    dtim_num = dtim_num + next_dtim_count;
                }
            }
        } else {
            dtim_num = dtim_num - (bcn_past_num % dtim_num);
        }

        /* last bcn -> next bcn */
        pds_sleep_us = (dtim_num + bcn_past_num) * beacon_interval_us;

        /* rc32k error compensation (last bcn -> next bcn) */
        rtc32k_error_us = (int32_t)((int64_t)pds_sleep_us * iot2lp_para->rc32k_trim_parameter->rtc32k_error_ppm / (1000 * 1000));

        /* now -> next bcn */
        pds_sleep_us = pds_sleep_us - (uint32_t)(rtc_now_us - last_beacon_rtc_us);

        /* rc32k error */
        if (rtc32k_error_us > 0 || pds_sleep_us > (-rtc32k_error_us)) {
            pds_sleep_us += rtc32k_error_us;
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

    } else {
        /* It shouldn't be here */
        pds_sleep_us = dtim_num * beacon_interval_us / 2;
    }
#if 0
    BL_LP_LOG("last_beacon_stamp_rtc_us: %lld\r\n", iot2lp_para->last_beacon_stamp_rtc_us);
    BL_LP_LOG("stamp_rtc_valid: %ld\r\n", iot2lp_para->last_beacon_stamp_rtc_valid);
    BL_LP_LOG("pds_sleep_ms: %ld\r\n", pds_sleep_us / 1000);
    BL_LP_LOG("rtc_timeout_ms: %lld\r\n", rtc_sleep_us / 1000);
    BL_LP_LOG("rc32k code %ld\r\n", iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext);
    BL_LP_LOG("rtc ppm %ld\r\n", iot2lp_para->rc32k_trim_parameter->rtc32k_error_ppm);
    pds_sleep_us -= 500;
#endif

    /* get glb gpio inturrpt status */
    uint64_t glb_io_int_stat = bl_lp_check_gpio_int();
    iot2lp_para->wake_io_bits = glb_io_int_stat;
    uint8_t acomp_int_stat = bl_lp_check_acomp_int();
    iot2lp_para->wake_acomp_bits = acomp_int_stat;

    /* Set lp_fw as the wake-up entry  */
    pm_set_wakeup_callback((void (*)(void))LP_FW_PRE_JUMP_ADDR);
    *((volatile uint32_t *)0x2000e504) &= ~(1 << 6);
    pm_pds_mask_all_wakeup_src();
    AON_Output_Float_LDO15_RF();
    AON_Output_Float_DCDC18();
    HBN_Pin_WakeUp_Mask(0xF);
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_1_GPIO0_GPIO7);
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_2_GPIO8_GPIO15);
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_3_GPIO20_GPIO27);
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_4_GPIO28_GPIO34);

    bl616_lp_wakeup_prepare();

    iot2lp_para->wifi_parameter->wifi_rx_buff = (uint8_t *)((uint32_t)export_get_rx_buffer1_addr() & 0x2FFFFFFF);
    /* lpfw cfg: system para */
    iot2lp_para->mcu_sts = bl_lp_fw_cfg->mcu_sts;
    iot2lp_para->lpfw_wakeup_cnt = 0;
    iot2lp_para->pattern = 0xAA5555AA;
    iot2lp_para->wakeup_flag = 0;
    iot2lp_para->app_entry = (uintptr_t)lp_fw_restore_cpu_para;
    iot2lp_para->args[0] = GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR;

    /* rc32k auto calibration */
    iot2lp_para->rc32k_trim_parameter->rc32k_auto_cal_en = 1;

    /* allow dtim wakeup ,if tim_wakeup_en == 1 */
    iot2lp_para->wifi_parameter->tim_wakeup_en = bl_lp_fw_cfg->tim_wakeup_en;

#ifndef CONFIG_UNKNOWN_IO_WAKEUP_FORBIDDEN
    /* allow unkown_reason wakeup */
    iot2lp_para->unkown_io_wakeup_en = 1;
#else
    /* forbid unkown_reason wakeup */
    iot2lp_para->unkown_io_wakeup_en = 0;
#endif

    if (bl_lp_fw_cfg->tim_wakeup_en) {
        iot2lp_para->tim_wake_enable = 1;
        /* lpfw cfg: wifi para */
        memcpy(iot2lp_para->wifi_parameter->bssid, bl_lp_fw_cfg->bssid, 6);
        memcpy(iot2lp_para->wifi_parameter->local_mac, bl_lp_fw_cfg->mac, 6);
        iot2lp_para->wifi_parameter->ap_channel = bl_lp_fw_cfg->channel;
        iot2lp_para->wifi_parameter->aid = bl_lp_fw_cfg->aid;
        iot2lp_para->wifi_parameter->bcn_target_level = bl_lp_fw_cfg->rssi;

        /* lpfw rx timeout */
        iot2lp_para->mtimer_parameter->mtimer_timeout_en = 1;
        iot2lp_para->mtimer_parameter->mtimer_timeout_mini_us = bl_lp_fw_cfg->mtimer_timeout_mini_us;
        iot2lp_para->mtimer_parameter->mtimer_timeout_max_us = bl_lp_fw_cfg->mtimer_timeout_max_us;

        /* take rc32k error rate as default */
        if (!(*((volatile uint32_t *)0x2000f030) & (1 << 3))) {
            /* rc32k 1500-ppm */
            iot2lp_para->rtc32k_jitter_error_ppm = 1200;
        } else {
            /* xtal 500-ppm */
            iot2lp_para->rtc32k_jitter_error_ppm = 400;
        }

        /* Compensates for pds goto sleep error */
        if (pds_sleep_us > 1000) {
            /* last beacon form */
            pds_sleep_us -= 1000;
        }

    } else {
        iot2lp_para->tim_wake_enable = 0;
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

    /* Minimum limit */
    if (pds_sleep_us < (PDS_WAKEUP_MINI_LIMIT_US + PDS_WAKEUP_DELAY_US)) {
        pds_sleep_us = PDS_WAKEUP_MINI_LIMIT_US;
    } else {
        pds_sleep_us -= PDS_WAKEUP_DELAY_US;
    }

    if (rtc_wakeup_cmp_cnt == 0) {
        if (rtc_sleep_us < (PDS_WAKEUP_MINI_LIMIT_US + PDS_WAKEUP_DELAY_US)) {
            rtc_sleep_us = PDS_WAKEUP_MINI_LIMIT_US;
        } else {
            rtc_sleep_us -= PDS_WAKEUP_DELAY_US;
        }
    } else {
        if (rtc_wakeup_cmp_cnt > PDS_WAKEUP_DELAY_CNT) {
            rtc_wakeup_cmp_cnt -= PDS_WAKEUP_DELAY_CNT;
        }
    }

    LP_HOOK(pre_sleep, iot2lp_para);

    /* app to sleep_pds, update time_info */
    bl_lp_time_info_update_app();

    bl_lp_vtime_before_sleep();

    L1C_DCache_Clean_All();


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
        /* Check io_stat , judge whether to enter PDS mode */
        /* if io_stat isn't 0x0, will sleep 2ms */
        if (1 == bl_lp_wakeup_check_allow()) {
            rtc_sleep_us = 2000;
            pds_sleep_us = 2000;
        }

        if ((bl_lp_fw_cfg->tim_wakeup_en) && (rtc_wakeup_cmp_cnt || rtc_sleep_us)) {
            /* rtc cfg */
            rtc_wakeup_init(rtc_wakeup_cmp_cnt, rtc_sleep_us);
            /* clear rtc IRQ */
            BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0x00000001 << 16);
            /* enable rtc wakeup source */
            BL_WR_REG(PDS_BASE, PDS_INT, BL_RD_REG(PDS_BASE, PDS_INT) | (0x00000001 << 11));
            /* pds15 enter */
            pm_pds_mode_enter(PM_PDS_LEVEL_15, BL_US_TO_PDS_CNT(pds_sleep_us));

        } else if (bl_lp_fw_cfg->tim_wakeup_en) {
            /* disable rtc comp */
            uint32_t tmpVal;
            tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
            tmpVal = tmpVal & 0xfffffff1;
            BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);
            /* pds15 enter */
            pm_pds_mode_enter(PM_PDS_LEVEL_15, BL_US_TO_PDS_CNT(pds_sleep_us));

        } else if (rtc_wakeup_cmp_cnt || rtc_sleep_us) {
            /* rtc cfg */
            rtc_wakeup_init(rtc_wakeup_cmp_cnt, rtc_sleep_us);
            /* clear rtc IRQ */
            BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0x00000001 << 16);
            /* enable rtc wakeup source */
            BL_WR_REG(PDS_BASE, PDS_INT, BL_RD_REG(PDS_BASE, PDS_INT) | (0x00000001 << 11));
            /* pds15 enter */
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
        }

        iot2lp_para->wakeup_flag = 1;
        lp_fw_restore_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);
    }

#if defined(__riscv_flen)
    __asm__ __volatile__(
        "lw      x0, 0(a0)\n\t"
        /* FP: initial state */
        "csrr    t0, mstatus\n\t"
        "li      t1, ~0x6000\n\t"
        "and     t0, t0, t1\n\t"
        "li      t1, 0x2000\n\t"
        "or      t0, t0, t1\n\t"
        "csrw    mstatus, t0\n\t");
    /* csrwi   fcsr, 0 */
#endif
    /* enable mxstatus THEADISAEE */
    uint32_t mxstatus = __get_MXSTATUS();
    mxstatus |= (1 << 22);
    /* enable mxstatus MM */
    mxstatus |= (1 << 15);
    __set_MXSTATUS(mxstatus);

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

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

    /* Restore CPU:320M and Mtimer:1M */
    // GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL | GLB_PLL_AUPLL);
    GLB_Set_MCU_System_CLK_Div(0, 3);
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_CLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK) / 1000000 - 1);

    /* disable wdt */
    // bl_wdt_disable();
    uint32_t regval = 0;
    putreg16(0xBABA, 0x2000A500 + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, 0x2000A500 + TIMER_WSAR_OFFSET);

    regval = getreg32(0x2000A500 + TIMER_WMER_OFFSET);
    regval &= ~TIMER_WE;
    putreg32(regval, 0x2000A500 + TIMER_WMER_OFFSET);

    /* The rc32k code is restored  */
    uint32_t reg = *((volatile uint32_t *)0x2000F200) & ~(0x3FF << 22);
    reg = reg | iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext << 22;
    *((volatile uint32_t *)0x2000F200) = reg;

    bl_lp_vtime_after_sleep();

    bl_lp_debug_record_time(iot2lp_para, "return APP");

    LP_HOOK(post_sys, iot2lp_para);

    /* try to use app mtimer to cal rc32,so make beacon_stamp_mtimer_valid=0 and init rc32k auto cal
       if xtal32 used, following code is also ok */
    pm_rc32k_auto_cal_init();

    PDS_IntClear();

    bl_lp_fw_cfg->wakeup_reason = iot2lp_para->wakeup_reason;
    bl_lp_fw_cfg->lpfw_recv_cnt = iot2lp_para->lp_info->bcn_lpfw_recv_cnt;
    bl_lp_fw_cfg->lpfw_loss_cnt = iot2lp_para->lp_info->bcn_lpfw_loss_cnt;

    if ((iot2lp_para->wakeup_reason & LPFW_WAKEUP_IO) || (iot2lp_para->wakeup_reason & LPFW_WAKEUP_ACOMP)) {
        bl616_lp_soft_irq_trigger();
    }

    BL_LP_LOG("--app wakeup reason %ld\r\n", bl_lp_fw_cfg->wakeup_reason);
#if 0
    BL_LP_LOG("wake_io_bits 0x%016llx\r\n", iot2lp_para->wake_io_bits);
    BL_LP_LOG("wake_acomp_bits 0x%x\r\n", iot2lp_para->wake_acomp_bits);
    BL_LP_LOG("wake_io_edge_bits 0x%016llx\r\n", iot2lp_para->wake_io_edge_bits);
    BL_LP_LOG("wake_acomp_edge_bits 0x%x\r\n", iot2lp_para->wake_acomp_edge_bits);
#endif
#if 1
    BL_LP_LOG("want receive %ld, loss %ld\r\n", bl_lp_fw_cfg->lpfw_recv_cnt, bl_lp_fw_cfg->lpfw_loss_cnt);
    if (bl_lp_fw_cfg->lpfw_recv_cnt) {
        BL_LP_LOG("loss rate %ld.%ld%%\r\n", bl_lp_fw_cfg->lpfw_loss_cnt * 100 / bl_lp_fw_cfg->lpfw_recv_cnt, (bl_lp_fw_cfg->lpfw_loss_cnt * 10000 / bl_lp_fw_cfg->lpfw_recv_cnt) % 100);
    }
    BL_LP_LOG("next tim : %d\r\n", bl_lp_get_next_beacon_time(0));
    BL_LP_LOG("rc32k code %ld, rc32k ppm %ld \r\n", iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext, iot2lp_para->rc32k_trim_parameter->rtc32k_error_ppm);
#endif
#if 0
    BL_LP_LOG("rtc_sleep: %lld\r\n", rtc_sleep_us);
    BL_LP_LOG("rtc_wakeup_cnt: %ld\r\n", (uint32_t)(BL_PDS_CNT_TO_US(iot2lp_para->rtc_wakeup_cnt)));
#endif

    bl_lp_debug_record_time(iot2lp_para, "bl_lp_fw_enter end");

#if (BL_LP_TIME_DEBUG)
    bl_lp_debug_dump_time(iot2lp_para);
#endif

    bl_lp_debug_clean_time(iot2lp_para);

    return iot2lp_para->wakeup_reason;
}

uint8_t bl_lp_get_hal_boot2_lp_ret(void)
{
    if (iot2boot2_para->hbn_pattern == 0x55AAAA55) {
        return iot2boot2_para->hal_boot2_lp_ret;
    } else {
        return -1;
    }
}

void bl_lp_hbn_init(uint8_t wdt_en, uint8_t feed_wdt_pin, uint8_t feed_wdt_type, uint32_t feed_wdt_max_continue_times)
{
    memset((void *)IOT2BOOT2_PARA_ADDR, 0, sizeof(iot2boot2_para_t));

    if (wdt_en != 0) {
        iot2boot2_para->wdt_pattern = 0xAAAA5555;
        iot2boot2_para->feed_wdt_io = feed_wdt_pin;
        iot2boot2_para->feed_wdt_type = feed_wdt_type;
        iot2boot2_para->feed_wdt_max_continue_times = feed_wdt_max_continue_times;
        iot2boot2_para->feed_wdt_continue_times = 0;
    } else {
        iot2boot2_para->wdt_pattern = 0x0;
        iot2boot2_para->feed_wdt_io = 0xFF;
        iot2boot2_para->feed_wdt_type = 0;
        iot2boot2_para->feed_wdt_max_continue_times = 0;
        iot2boot2_para->feed_wdt_continue_times = 0;
    }
}

int bl_lp_hbn_enter(bl_lp_hbn_fw_cfg_t *bl_lp_hbn_fw_cfg)
{
    // uint32_t time_s=10;

    /* clean wake bits */
    iot2boot2_para->wkup_io_bits = 0;
    iot2boot2_para->wkup_acomp_bits = 0;
    iot2boot2_para->wkup_io_edge_bits = 0;
    iot2boot2_para->wkup_acomp_edge_bits = 0;
    iot2boot2_para->wakeup_reason = 0;

    bl616_lp_wakeup_prepare();

    iot2boot2_para->feed_wdt_continue_times = 0;
    iot2boot2_para->hbn_level = bl_lp_hbn_fw_cfg->hbn_level;
    iot2boot2_para->hbn_sleep_period = bl_lp_hbn_fw_cfg->hbn_sleep_cnt;
    iot2boot2_para->hbn_pattern = 0x55AAAA55;
    iot2boot2_para->wkup_rtc_cnt = (bflb_rtc_get_time(NULL) + bl_lp_hbn_fw_cfg->hbn_sleep_cnt) & 0xFFFFFFFFFF;
    HBN_Set_Ldo11_Rt_Vout(0xA);
    HBN_Set_Ldo11_Soc_Vout(0xA);
    AON_Output_Pulldown_DCDC18();
    bl_sys_rstinfo_set(BL_RST_HBN);
    pm_hbn_mode_enter(iot2boot2_para->hbn_level, iot2boot2_para->hbn_sleep_period);
    return 0;
}

int bl_lp_fw_enter_check_allow(void)
{
    uint64_t last_beacon_rtc_us, rtc_now_us, rtc_cnt;
    uint32_t beacon_interval_us;
    uint32_t dtim_num, bcn_past_num, now_dtim_count = 0;

    if (iot2lp_para->last_beacon_stamp_rtc_valid == 0) {
        /* It shouldn't be here */
        return 1;
    }

    /* beacon interval us, TU * 1024 */
    beacon_interval_us = iot2lp_para->wifi_parameter->beacon_interval_tu * 1024;

    /* get period of dtim */
    if (iot2lp_para->bcn_loss_info->continuous_loss_cnt) {
        uint32_t bcn_loss_level = iot2lp_para->bcn_loss_info->bcn_loss_level;
        lp_fw_bcn_loss_level_t *bcn_loss_cfg = &(iot2lp_para->bcn_loss_info->bcn_loss_cfg_table[bcn_loss_level]);
        dtim_num = bcn_loss_cfg->dtim_num;
        // dtim_period_us = bcn_loss_cfg->dtim_num * beacon_interval_us;
    } else {
        dtim_num = iot2lp_para->wifi_parameter->dtim_num;
        // dtim_period_us = dtim_num * beacon_interval_us;
    }

    /* Time stamp of the last beacon */
    last_beacon_rtc_us = iot2lp_para->last_beacon_stamp_rtc_us;
    last_beacon_rtc_us -= iot2lp_para->bcn_delay_info->last_beacon_delay_us;

    /* Gets the current rtc time */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    /* beacon num form (last bcn -> now) */
    /* The time difference since the last beacon was received, divided by beacon interval */
    bcn_past_num = (rtc_now_us - last_beacon_rtc_us + beacon_interval_us / 2) / beacon_interval_us;

    if (bcn_past_num == 0) {
        return 1;
    } else {
        if (iot2lp_para->wifi_parameter->bcmc_dtim_mode) {
            dtim_num = dtim_num * iot2lp_para->wifi_parameter->beacon_dtim_period;
            now_dtim_count = (iot2lp_para->wifi_parameter->last_beacon_dtim_count + bcn_past_num) % iot2lp_para->wifi_parameter->beacon_dtim_period;

            if (now_dtim_count != 0) {
                return 1;
            }

            if ((bcn_past_num + (iot2lp_para->wifi_parameter->beacon_dtim_period - iot2lp_para->wifi_parameter->last_beacon_dtim_count) % iot2lp_para->wifi_parameter->beacon_dtim_period) % dtim_num != 0) {
                return 1;
            }

        } else {
            if (bcn_past_num % dtim_num != 0) {
                return 1;
            }
        }

        /* now bcn */
        uint64_t now_beacon_rtc_us = last_beacon_rtc_us + bcn_past_num * beacon_interval_us;

        if (rtc_now_us < now_beacon_rtc_us && now_beacon_rtc_us - rtc_now_us < (PROTECT_BF_MS + 3) * 1000) {
            BL_LP_LOG("-Not allowed sleep BF: now_dtim_cnt:%d, past_num:%d \r\n", now_dtim_count, bcn_past_num);
            return 0;
        } else if (rtc_now_us - now_beacon_rtc_us < (PROTECT_AF_MS) * 1000) {
            BL_LP_LOG("-Not allowed sleep AF: now_dtim_cnt:%d, past_num:%d \r\n", now_dtim_count, bcn_past_num);
            return 0;
        }
    }

    return 1;
}

/* get the remaining time of the next beacon or dtim, mode 0:dtim, 1:beacon */
int bl_lp_init(void)
{
    bl_lp_env_init();

#if defined(CFG_BL_WIFI_PS_ENABLE)
    bl_lp_fw_init();
#endif

    return 0;
}

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

int bl_lp_get_wake_reason(void)
{
    return (int)iot2lp_para->wakeup_reason;
}

void bl_lp_bod_init(uint8_t en, uint8_t rst, uint8_t irq, uint32_t threshold)
{
    HBN_BOD_CFG_Type bod_cfg;
    if (en) {
        bod_cfg.enableBod = ENABLE;
    } else {
        bod_cfg.enableBod = DISABLE;
    }
    if (irq) {
        bod_cfg.enableBodInt = 1;
    } else {
        bod_cfg.enableBodInt = 0;
    }
    bod_cfg.bodThreshold = threshold;
    if (rst) {
        bod_cfg.enablePorInBod = HBN_BOD_MODE_POR_RELEVANT;
    } else {
        bod_cfg.enablePorInBod = HBN_BOD_MODE_POR_INDEPENDENT;
    }
    HBN_Set_BOD_Cfg(&bod_cfg);
}

int bl_lp_get_wake_io_state(void)
{
    return (int)bl_lp_env_get()->gpio_stat;
}
