#include <assert.h>
#include "bflb_core.h"
#include "bl616_pm.h"
#include "bflb_rtc.h"
#include "bl_lp.h"
#include "bl_sys.h"
#include "bl616_pds.h"
#include "bl616_hbn.h"
#include "hardware/timer_reg.h"
#include "ef_data_reg.h"
#include "bl616_glb_gpio.h"
#include "bflb_dma.h"
#include "bl616_aon.h"
#include "bflb_acomp.h"
#include "bflb_sec_sha.h"
#include "bl616_tzc_sec.h"
#include "tzc_sec_reg.h"
#include "bl616_l1c.h"
#include "bl616_clock.h"
#include "bl616_xip_recovery.h"

#if (0 && !defined(BL_WIFI_LP_FW))
#define BL_LP_LOG        printf
#define BL_LP_TIME_DEBUG 0
#else
#define BL_LP_LOG(...)
#define BL_LP_TIME_DEBUG 0
#endif


// extern void CPU_Interrupt_Enable(uint32_t irq_num);
// extern void CPU_Interrupt_Disable(uint32_t irq_num);
extern int lpfw_recal_rc32k(uint64_t beacon_timestamp_now_us, uint64_t rtc_timestamp_now_us, uint32_t mode, int clock_ready_check);
extern int32_t lpfw_calculate_beacon_delay(uint64_t beacon_timestamp_us, uint64_t rtc_timestamp_us, uint32_t mode);
extern int32_t lpfw_beacon_delay_sliding_win_get_average();
extern int32_t AON_Set_LDO11_SOC_Sstart_Delay(uint8_t delay);
extern uint32_t *export_get_rx_buffer1_addr(void);

#ifdef BL_WIFI_LP_FW
uint64_t (*shared_cpu_get_mtimer_counter)(void) = NULL;
void (*shared_arch_delay_ms)(uint32_t) = NULL;
void (*shared_arch_delay_us)(uint32_t) = NULL;
int32_t (*shared_cpu_reset_mtimer)(void) = NULL;
int32_t (*shared_lpfw_calculate_beacon_delay)(uint64_t, uint64_t, uint32_t) = NULL;
int32_t (*shared_lpfw_beacon_delay_sliding_win_get_average)(void) = NULL;
int32_t (*shared_aon_set_ldo11_soc_sstart_delay)(uint32_t) = NULL;
int32_t (*shared_pds_default_level_config)(uint32_t *, uint32_t) = NULL;
#endif
uint32_t *shared_func_array[32];

bl_lp_fw_cfg_t lpfw_cfg = {
    .tim_wakeup_en = 1,
    .rtc_timeout_us = (60 * 1000 * 1000),
    .dtim_origin = BL_DTIM_NUM,
    .dtim_num = 0,
};

struct lp_env {
    void *sys_enter_arg;
    void *user_enter_arg;
    void *sys_exit_arg;
    void *user_exit_arg;

    bl_lp_cb_t sys_pre_enter_callback;
    bl_lp_cb_t user_pre_enter_callback;
    bl_lp_cb_t sys_after_exit_callback;
    bl_lp_cb_t user_after_exit_callback;

    uint8_t wifi_hw_resume;
    uint8_t wifi_fw_ready;
    uint32_t gpio_stat;
};

static lp_fw_tzc_t tzc_info;
static struct lp_env *gp_lp_env = NULL;

static uint64_t g_rtc_timestamp_before_sleep_us = 0;    /* rtc 1 */
static uint64_t g_rtc_timestamp_after_sleep_us = 0;     /* rtc 2 */
static uint64_t g_mtimer_timestamp_before_sleep_us = 0; /* mtime 1 */
static uint64_t g_mtimer_timestamp_after_sleep_us = 0;  /* mtime 2 */
static uint64_t g_virtual_timestamp_base_us = 0;        /* virtual time base */

static void bl_lp_vtime_before_sleep(void);
static void bl_lp_vtime_after_sleep(void);

static void bl_lp_io_wakeup_init(bl_lp_io_cfg_t *io_wakeup_cfg);
static void bl_lp_acomp_wakeup_init(bl_lp_acomp_cfg_t *acomp_wakeup_cfg);

static bl_lp_io_cfg_t *gp_lp_io_cfg = NULL;
static bl_lp_io_cfg_t g_lp_io_cfg_bak;
static bl_lp_acomp_cfg_t *gp_lp_acomp_cfg = NULL;
static bl_lp_acomp_cfg_t g_lp_acomp_cfg_bak;
static void bl_lp_soft_irq(void);
static bl_lp_soft_irq_callback_t lp_soft_callback = { NULL };

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
    uint32_t lpfw_addr = ((uint32_t)__lpfw_share_start & 0x0FFFFFFF) | 0x60000000; /* cacheable */
    pFunc = (void (*)(void))lpfw_addr;
    pFunc();
}

extern uint32_t __hbn_load_addr;
extern uint32_t __hbn_ram_start__;
extern uint32_t __hbn_ram_end__;

/* load hbn ram */
static void bl616_load_hbn_ram(void)
{
    uint32_t *pSrc, *pDest;

    /* BF Add HBNRAM data copy */
    pSrc = &__hbn_load_addr;
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

#if 1
static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_ctx_s ctx_sha256;

static void lpfw_sec_sha256(uint32_t addr, uint32_t len, uint8_t *result)
{
    struct bflb_device_s *sha256;
    sha256 = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_group0_request_sha_access(sha256);

    bflb_sha_init(sha256, SHA_MODE_SHA256);
    bflb_sha256_start(sha256, &ctx_sha256);
    bflb_sha256_update(sha256, &ctx_sha256, (const uint8_t *)addr, len);
    bflb_sha256_finish(sha256, &ctx_sha256, result);
}
#endif

static int8_t set_shared_func(uint8_t index, uint32_t *func)
{
    if ((((uint32_t)func >> 28) & 0xF) == 0xA) {
        return -1;
    }
    shared_func_array[index] = func;
    return 0;
}

static void shared_func_init(void)
{
    int8_t flag = 0;
    flag |= set_shared_func(0, (uint32_t *)CPU_Get_MTimer_Counter);
    flag |= set_shared_func(1, (uint32_t *)arch_delay_ms);
    flag |= set_shared_func(2, (uint32_t *)arch_delay_us);
    flag |= set_shared_func(3, (uint32_t *)CPU_Reset_MTimer);
    flag |= set_shared_func(4, (uint32_t *)lpfw_calculate_beacon_delay);
    flag |= set_shared_func(6, (uint32_t *)lpfw_beacon_delay_sliding_win_get_average);
    flag |= set_shared_func(7, (uint32_t *)AON_Set_LDO11_SOC_Sstart_Delay);
    flag |= set_shared_func(8, (uint32_t *)PDS_Default_Level_Config);

    if (flag != 0) {
        BL_LP_LOG("shared_func_init err!\r\n");
    }
}

/******************** lp fw info ********************/

bl_lp_fw_info_t *bl_lpfw_bin_get_info(void)
{
    return (bl_lp_fw_info_t *)__lpfw_load_addr;
}

int bl_lpfw_bin_check(void)
{
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();

    if (lpfw_info->magic_code != 0x7766706C) {
        printf("lpfw magic code error\r\n");
        return -1;
    }

    if ((lpfw_info->lpfw_memory_start & 0x0FFFFFFF) != ((uint32_t)__lpfw_share_start & 0x0FFFFFFF)) {
        printf("lpfw memory start address error: lpfw:0x%08X, app:0x%08X\r\n", lpfw_info->lpfw_memory_start, (uint32_t)__lpfw_share_start);
        return -2;
    }

    if ((lpfw_info->lpfw_memory_end - lpfw_info->lpfw_memory_start) > ((uint32_t)__lpfw_share_end - (uint32_t)__lpfw_share_start)) {
        printf("lpfw memory size_over\r\n");
        return -3;
    }

    return 0;
}

char *bl_lpfw_bin_get_version_str(void)
{
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();

    if (lpfw_info->magic_code != 0x7766706C) {
        return NULL;
    }

    return lpfw_info->lpfw_version_str;
}

int bl_lpfw_ram_load(void)
{
    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    uint32_t lpfw_addr = ((uint32_t)__lpfw_share_start & 0x0FFFFFFF) | 0x60000000; /* cacheable */
    uint32_t lpfw_size = *((uint32_t *)__lpfw_load_addr - 7);

    /* load */
    memcpy((void *)lpfw_addr, __lpfw_load_addr, lpfw_size);
    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)lpfw_addr, lpfw_size);

    return 0;
}

int bl_lpfw_ram_verify(void)
{
    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    uint32_t lpfw_addr = ((uint32_t)__lpfw_share_start & 0x0FFFFFFF) | 0x60000000; /* cacheable */
    uint32_t lpfw_size = *((uint32_t *)__lpfw_load_addr - 7);
    uint8_t *lpfw_sha256 = (uint8_t *)(__lpfw_load_addr - 16);
    uint8_t result[32];

    /* hardware sha256 */
    lpfw_sec_sha256(lpfw_addr, lpfw_size, result);

    if (memcmp(result, lpfw_sha256, 32) != 0) {
        printf("lpfw sha256 check failed\r\n");
        return -1;
    }
    return 0;
}

static void bl_lp_tzc_para_save(void)
{
    iot2lp_para->tzc_cfg = &tzc_info;

    iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_ctrl = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_CTRL);
    iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_r0 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R0);
    iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_r1 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R1);
    iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_r2 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R2);

    iot2lp_para->tzc_cfg->tzc_sf_tzsrg_ctrl = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_CTRL);
    iot2lp_para->tzc_cfg->tzc_sf_tzsrg_r0 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R0);
    iot2lp_para->tzc_cfg->tzc_sf_tzsrg_r1 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R1);
    iot2lp_para->tzc_cfg->tzc_sf_tzsrg_r2 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R2);
    iot2lp_para->tzc_cfg->tzc_sf_tzsrg_msb = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_MSB);
}


/********************  ********************/

void bl_lp_fw_init()
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
    static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bl_lp_info_s lp_info_struct = { 0 };
    memset(&lp_info_struct, 0, sizeof(lp_info_struct));
    iot2lp_para->lp_info = &lp_info_struct;
    bl_lp_info_clear();

#if (BL_LP_TIME_DEBUG)
    /* time debug */
    static ATTR_NOCACHE_NOINIT_RAM_SECTION lp_fw_time_debug_t time_debug_buff[TIME_DEBUG_NUM_MAX] = { 0 };
    memset(time_debug_buff, 0, sizeof(time_debug_buff));
    iot2lp_para->time_debug = time_debug_buff;
#endif

    /* Setting the Default Value */
    iot2lp_para->dtim_num = 10;
    iot2lp_para->beacon_interval_tu = 100;
    iot2lp_para->rc32k_auto_cal_en = 1;
    iot2lp_para->debug_io = 0xFF;

    iot2lp_para->bcn_loss_level = 0;
    bl_lp_fw_bcn_loss_cfg_dtim_default(iot2lp_para->dtim_num);

    /* Save rc32k code in HBN_RAM */
    iot2lp_para->rc32k_fr_ext = (*((volatile uint32_t *)0x2000F200)) >> 22;
    /* rc32k status: not ready */
    iot2lp_para->rc32k_clock_ready = 0;

    /* sliding window init */
    static ATTR_NOCACHE_NOINIT_RAM_SECTION int32_t bcn_delay_buff[16] = { 0 };
    iot2lp_para->bcn_delay_sliding_win_buff = bcn_delay_buff;
    iot2lp_para->bcn_delay_sliding_win_size = sizeof(bcn_delay_buff) / sizeof(int32_t);
    iot2lp_para->bcn_delay_sliding_win_point = 0;
    iot2lp_para->bcn_delay_sliding_win_status = 0;

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
    bl_lp_tzc_para_save();
    printf("[LP] flash jdec_id: 0x%08lX\r\n", (unsigned long)iot2lp_para->flash_jdec_id);
}

int bl_lp_beacon_interval_update(uint16_t beacon_interval_tu)
{
    if (beacon_interval_tu < 20 || beacon_interval_tu > 1000) {
        return -1;
    }

    if (beacon_interval_tu == iot2lp_para->beacon_interval_tu) {
        return 0;
    }

    BL_LP_LOG("beacon interval update %d -> %d", (int)iot2lp_para->beacon_interval_tu, (int)beacon_interval_tu);

    iot2lp_para->beacon_interval_tu = beacon_interval_tu;

    /* TODO: Other actions may be required, such as resetting the state */

    return 0;
}

void bl_lp_fw_bcn_loss_cfg(lp_fw_bcn_loss_level_t *cfg_table, uint16_t table_num, uint16_t loop_start, uint16_t loss_max)
{
    if (cfg_table == NULL || table_num == 0) {
        return;
    }

    if (loop_start >= table_num) {
        loop_start = table_num;
    }

    /* cache clean */
    csi_dcache_clean_range(cfg_table, sizeof(lp_fw_bcn_loss_level_t) * table_num);

    /* nocache ram */
    cfg_table = (void *)(((uint32_t)cfg_table) & 0x2fffffff);

    iot2lp_para->continuous_loss_cnt_max = loss_max;
    iot2lp_para->bcn_loss_cfg_table = cfg_table;
    iot2lp_para->bcn_loss_loop_start = loop_start;
    iot2lp_para->bcn_loss_level_max = table_num;

    /* clear */
    iot2lp_para->bcn_loss_level = 0;
    iot2lp_para->continuous_loss_cnt = 0;
}
int bl_lp_beacon_tim_update(uint8_t *tim, uint8_t mode)
{
    uint8_t tim_id = tim[0];
    uint8_t tim_len = tim[1];
    uint8_t dtim_count = tim[2];
    uint8_t dtim_period = tim[3];

    if (tim_id != 5 || tim_len < 4) {
        BL_LP_LOG("tim elem error, id:%d,len:%d\r\n", tim_id, tim_len);
        return -1;
    }

    if (dtim_count >= dtim_period) {
        BL_LP_LOG("dtim count %d >= period %d\r\n", dtim_count, dtim_period);
        return -1;
    }

    if (mode == BEACON_STAMP_LPFW) {
        if ((iot2lp_para->beacon_dtim_period != dtim_period) && iot2lp_para->beacon_dtim_period) {
            return -1; /* not allow change dtim period */
        }
    } else {
        iot2lp_para->beacon_dtim_period = dtim_period;
    }

    iot2lp_para->last_beacon_dtim_count = dtim_count;

    BL_LP_LOG("[LP] beacon tim update: id:%d, len:%d, dtim_count:%d, dtim_period:%d\r\n",
                  tim_id, tim_len, dtim_count, dtim_period);

    return 0;
}
void bl_lp_fw_bcn_loss_cfg_dtim_default(uint8_t dtim_num)
{
    int32_t cfg_table_num;
    int32_t cfg_loop_start;
    uint32_t cfg_loss_max;
    static ATTR_NOCACHE_NOINIT_RAM_SECTION lp_fw_bcn_loss_level_t bcn_loss_cfg_buff[20];

    /* dtim10 */
    int32_t dtim10_table_num = 18;
    int32_t dtim10_loop_start = 6;
    uint32_t dtim10_loss_max = 6 + 12 * 20;
    static const lp_fw_bcn_loss_level_t dtim10_cfg_table[18] = {
        { 10, 0, 2000, 4000 },  /* loss 0, unused */
        { 8, 0, 2000, 6000 },   /* loss 1 */
        { 6, 0, 3000, 10000 },  /* loss 2 */
        { 6, 0, 4000, 10000 },  /* loss 3 */
        { 3, 0, 6000, 14000 },  /* loss 4 */
        { 3, 1, 15000, 40000 }, /* loss 5, wakeup */

        { 9, 0, 2000, 5000 },   /* loss 1 */
        { 8, 0, 3000, 8000 },   /* loss 2 */
        { 8, 0, 4000, 12000 },  /* loss 3 */
        { 6, 0, 5000, 12000 },  /* loss 4 */
        { 6, 0, 6000, 12000 },  /* loss 5 */
        { 6, 1, 20000, 50000 }, /* loss 6, wakeup */

        { 9, 0, 2000, 4000 },    /* loss 1 */
        { 8, 0, 4000, 8000 },    /* loss 2 */
        { 8, 0, 6000, 12000 },   /* loss 3 */
        { 6, 0, 6000, 12000 },   /* loss 4 */
        { 6, 0, 6000, 12000 },   /* loss 5 */
        { 6, 1, 50000, 110000 }, /* loss 6, wakeup */
                                 /* cfg table over */
    };

    /* dtim6 */
    int32_t dtim6_table_num = 20;
    int32_t dtim6_loop_start = 6;
    uint32_t dtim6_loss_max = 6 + 14 * 20;
    static const lp_fw_bcn_loss_level_t dtim6_cfg_table[20] = {
        { 6, 0, 2000, 4000 },   /* loss 0, unused */
        { 6, 0, 2000, 6000 },   /* loss 1 */
        { 5, 0, 3000, 10000 },  /* loss 2 */
        { 4, 0, 4000, 8000 },   /* loss 3 */
        { 3, 0, 6000, 14000 },  /* loss 4 */
        { 3, 1, 15000, 40000 }, /* loss 5, wakeup */

        { 6, 0, 2000, 4000 },   /* loss 1 */
        { 6, 0, 3000, 8000 },   /* loss 2 */
        { 5, 0, 4000, 8000 },   /* loss 3 */
        { 5, 0, 4000, 10000 },  /* loss 4 */
        { 4, 0, 3000, 10000 },  /* loss 5 */
        { 4, 0, 5000, 10000 },  /* loss 6 */
        { 3, 1, 20000, 50000 }, /* loss 7, wakeup */

        { 6, 0, 2000, 4000 },    /* loss 1 */
        { 6, 0, 3000, 8000 },    /* loss 2 */
        { 5, 0, 4000, 8000 },    /* loss 3 */
        { 5, 0, 4000, 10000 },   /* loss 4 */
        { 4, 0, 3000, 10000 },   /* loss 5 */
        { 4, 0, 5000, 10000 },   /* loss 6 */
        { 3, 1, 50000, 110000 }, /* loss 7, wakeup */
                                 /* cfg table over */
    };

    /* dtim3 */
    int32_t dtim3_table_num = 20;
    int32_t dtim3_loop_start = 8;
    uint32_t dtim3_loss_max = 8 + 12 * 20;
    static const lp_fw_bcn_loss_level_t dtim3_cfg_table[20] = {
        { 3, 0, 2000, 4000 },   /* loss 0, unused */
        { 3, 0, 2000, 6000 },   /* loss 1 */
        { 3, 0, 3000, 10000 },  /* loss 2 */
        { 3, 0, 4000, 8000 },   /* loss 3 */
        { 2, 0, 3000, 8000 },   /* loss 4 */
        { 3, 0, 3000, 10000 },  /* loss 5 */
        { 2, 0, 6000, 14000 },  /* loss 6 */
        { 3, 1, 15000, 40000 }, /* loss 7, wakeup */

        { 3, 0, 2000, 5000 },    /* loss 1 */
        { 3, 0, 3000, 6000 },    /* loss 2 */
        { 3, 0, 4000, 8000 },    /* loss 3 */
        { 3, 0, 10000, 20000 },  /* loss 4 */
        { 3, 0, 3000, 6000 },    /* loss 5 */
        { 2, 0, 3000, 6000 },    /* loss 6 */
        { 3, 0, 4000, 8000 },    /* loss 7 */
        { 3, 0, 4000, 8000 },    /* loss 8 */
        { 3, 0, 4000, 10000 },   /* loss 9 */
        { 2, 0, 4000, 10000 },   /* loss 10 */
        { 3, 0, 10000, 20000 },  /* loss 11 */
        { 3, 1, 50000, 110000 }, /* loss 12, wakeup */
                                 /* cfg table over */
    };

    /* dtim1 */
    int32_t dtim1_table_num = 20;
    int32_t dtim1_loop_start = 8;
    uint32_t dtim1_loss_max = 8 + 12 * 20;
    static const lp_fw_bcn_loss_level_t dtim1_cfg_table[20] = {
        { 1, 0, 2000, 4000 },   /* loss 0, unused */
        { 1, 0, 2000, 6000 },   /* loss 1 */
        { 1, 0, 3000, 10000 },  /* loss 2 */
        { 1, 0, 4000, 8000 },   /* loss 3 */
        { 2, 0, 3000, 8000 },   /* loss 4 */
        { 3, 0, 3000, 8000 },   /* loss 5 */
        { 2, 0, 6000, 14000 },  /* loss 6 */
        { 1, 1, 15000, 40000 }, /* loss 7, wakeup */

        { 1, 0, 2000, 4000 },    /* loss 1 */
        { 2, 0, 3000, 6000 },    /* loss 2 */
        { 3, 0, 4000, 8000 },    /* loss 3 */
        { 1, 0, 10000, 20000 },  /* loss 4 */
        { 2, 0, 3000, 6000 },    /* loss 5 */
        { 3, 0, 3000, 6000 },    /* loss 6 */
        { 1, 0, 4000, 8000 },    /* loss 7 */
        { 2, 0, 4000, 8000 },    /* loss 8 */
        { 3, 0, 4000, 8000 },    /* loss 9 */
        { 1, 0, 4000, 8000 },    /* loss 10 */
        { 2, 0, 10000, 20000 },  /* loss 11 */
        { 3, 1, 50000, 110000 }, /* loss 12, wakeup */
                                 /* cfg table over */
    };

    if (dtim_num == 0) {
        dtim_num = 10;
    }

    if (dtim_num < 3) {
        /* dtim 1 */
        memcpy(bcn_loss_cfg_buff, dtim1_cfg_table, sizeof(dtim1_cfg_table));
        cfg_table_num = dtim1_table_num;
        cfg_loop_start = dtim1_loop_start;
        cfg_loss_max = dtim1_loss_max;
        BL_LP_LOG("bcn_loss_cfg: dtim-1\r\n");
    } else if (dtim_num < 6) {
        /* dtim 3 */
        memcpy(bcn_loss_cfg_buff, dtim3_cfg_table, sizeof(dtim3_cfg_table));
        cfg_table_num = dtim3_table_num;
        cfg_loop_start = dtim3_loop_start;
        cfg_loss_max = dtim3_loss_max;
        BL_LP_LOG("bcn_loss_cfg: dtim-3\r\n");
    } else if (dtim_num < 9) {
        /* dtim6 */
        memcpy(bcn_loss_cfg_buff, dtim6_cfg_table, sizeof(dtim6_cfg_table));
        cfg_table_num = dtim6_table_num;
        cfg_loop_start = dtim6_loop_start;
        cfg_loss_max = dtim6_loss_max;
        BL_LP_LOG("bcn_loss_cfg: dtim-6\r\n");
    } else {
        /* dtim10 */
        memcpy(bcn_loss_cfg_buff, dtim10_cfg_table, sizeof(dtim10_cfg_table));
        cfg_table_num = dtim10_table_num;
        cfg_loop_start = dtim10_loop_start;
        cfg_loss_max = dtim10_loss_max;
        BL_LP_LOG("bcn_loss_cfg: dtim-10\r\n");
    }

    bl_lp_fw_bcn_loss_cfg(bcn_loss_cfg_buff, cfg_table_num, cfg_loop_start, cfg_loss_max);
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

void bl_lp_fw_disconnection()
{
    /* Reset parameters */
    iot2lp_para->dtim_num = 10;
    iot2lp_para->beacon_interval_tu = 100;
    /* Save rc32k code in HBN_RAM */
    iot2lp_para->rc32k_fr_ext = (*((volatile uint32_t *)0x2000F200)) >> 22;
    /* Clear continuous_loss_cnt */
    iot2lp_para->bcn_loss_level = 0;
    iot2lp_para->continuous_loss_cnt = 0;

    /* Clear the data of the sliding window */
    iot2lp_para->bcn_delay_sliding_win_point = 0;
    iot2lp_para->bcn_delay_sliding_win_status = 0;
    iot2lp_para->last_beacon_delay_us = 0;
    iot2lp_para->bcn_delay_offset = 0;

    /* Invalid timestamp */
    iot2lp_para->last_beacon_stamp_rtc_valid = 0;
    iot2lp_para->last_rc32trim_stamp_valid = 0;
    iot2lp_para->rc32k_trim_ready = 0;
    iot2lp_para->last_rc32trim_stamp_rtc_us = 0;
}

void modify_bit(uint32_t *reg_addr, uint8_t bit_position, uint8_t bit_value)
{
    if (bit_value) {
        *reg_addr |= (1U << bit_position);
    } else {
        *reg_addr &= ~(1U << bit_position);
    }
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

void bl_lp_power_on_xtal32k(uint8_t type)
{
    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = GPIO_FUN_ANALOG,
        .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1
    };
    if (type) {
        gpioCfg.gpioPin = 16;
        GLB_GPIO_Init(&gpioCfg);
    }
    gpioCfg.gpioPin = 17;
    GLB_GPIO_Init(&gpioCfg);
    BL_WR_WORD(0x2000F204, 0x210288);

    if (type) {
        /* power on */
        HBN_Set_Xtal_32K_Inverter_Amplify_Strength(3);
        HBN_Power_On_Xtal_32K();
    } else {
        HBN_Power_On_Xtal_32K();
        xtal32k_input();
    }
}


void bl_lp_rtc_use_xtal32K()
{
    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = GPIO_FUN_ANALOG,
        .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1
    };

    gpioCfg.gpioPin = 16;
    GLB_GPIO_Init(&gpioCfg);
    gpioCfg.gpioPin = 17;
    GLB_GPIO_Init(&gpioCfg);

    HBN_Power_On_Xtal_32K();
    HBN_32K_Sel(1);
    /* GPIO17 no pull */
    *((volatile uint32_t *)0x2000F014) &= ~(1 << 16);
}

void bl_lp_rtc_use_rc32k()
{
    HBN_32K_Sel(0);
    HBN_Power_Off_Xtal_32K();
}

int bl_lp_rtc_rc32k_coarse_adj(uint32_t expect_time, uint32_t rc32k_actual_time)
{
    int diff_val, diff_ppm, diff_code;

    if (iot2lp_para->rc32k_clock_ready) {
        return 0;
    }

    if (expect_time == 0 || rc32k_actual_time == 0) {
        return 1000 * 1000;
    }

    diff_val = rc32k_actual_time - expect_time;
    diff_ppm = (int64_t)diff_val * 1000 * 1000 / expect_time;
    if (diff_ppm < 0) {
        diff_code = (diff_ppm / 2000 - 1) * 2 / 3;
    } else {
        diff_code = (diff_ppm / 2000 + 1) * 2 / 3;
    }

    if (diff_ppm > 800 * 1000 || diff_ppm < -800 * 1000) {
        /* The error is too large. Abort calibration */
        return 1000 * 1000;
    }

    /* get rc32k recal code */
    uint32_t rc32k_reg = *((volatile uint32_t *)0x2000F200);
    uint32_t rc32k_code = rc32k_reg >> 22;

    BL_LP_LOG("rc32k coarse_adj, code:%d, ppm:%d, diff_code:%d\r\n", rc32k_code, diff_ppm, diff_code);

    if (diff_code != 0) {
        rc32k_code += diff_code;

        /* set rc32k code */
        rc32k_reg &= ~0xffc00000;
        rc32k_reg |= rc32k_code << 22;
        *((volatile uint32_t *)0x2000F200) = rc32k_reg;
    }

    /* save the code */
    iot2lp_para->rc32k_fr_ext = rc32k_code;

    return diff_ppm;
}


int bl_lp_set_32k_clock_ready(uint8_t ready_val)
{
    iot2lp_para->rc32k_clock_ready = ready_val;
    iot2lp_para->rc32k_trim_ready = 0;

    BL_LP_LOG("bl_lp set 32k ready %d\r\n", ready_val);

    return ready_val;
}

int bl_lp_get_32k_clock_ready()
{
    return iot2lp_para->rc32k_clock_ready;
}

int bl_lp_set_32k_trim_ready(uint8_t ready_val)
{
    iot2lp_para->rc32k_trim_ready = ready_val;

    BL_LP_LOG("bl_lp set 32k trim ready %d\r\n", ready_val);

    return ready_val;
}

int bl_lp_get_bcn_delay_ready()
{
    if (iot2lp_para->bcn_delay_sliding_win_status < iot2lp_para->bcn_delay_sliding_win_size) {
        return 0;
    } else {
        return 1;
    }
}

int bl_lp_get_32k_trim_ready()
{
    return iot2lp_para->rc32k_trim_ready;
}


static uint8_t bl_lp_wakeup_check(void)
{
    if ((iot2lp_para->wake_io_bits) || (iot2lp_para->wake_acomp_bits)) {
        return 1;
    } else {
        return 0;
    }
}

__WEAK uint64_t bl_lp_check_gpio_int(void)
{
    uint64_t result = 0;
    return result;
}
__WEAK uint8_t bl_lp_check_acomp_int(void)
{
    uint8_t result = 0;
    return result;
}

void bl_lp_debug_record_time(iot2lp_para_t *iot_lp_para, char *info_str)
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

    iot2lp_para->buf_addr = bl_lp_fw_cfg->buf_addr;
    iot2lp_para->pack_env = bl_lp_fw_cfg->pack_env;

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

    iot2lp_para->bcmc_dtim_mode = bl_lp_fw_cfg->bcmc_dtim_mode;
    /* tim interval */
    if (bl_lp_fw_cfg->dtim_num != 0) {
        iot2lp_para->dtim_num = bl_lp_fw_cfg->dtim_num;
    } else {
        iot2lp_para->dtim_num = 10;
    }

    if (iot2lp_para->beacon_leg_rate == 0) {
        iot2lp_para->beacon_leg_rate = 2; //1M
    }

    /* beacon interval us, TU * 1024 */
    beacon_interval_us = iot2lp_para->beacon_interval_tu * 1024;

    /* get period of dtim */
    if (iot2lp_para->continuous_loss_cnt == 0) {
        dtim_num = iot2lp_para->dtim_num;
    } else {
        bcn_loss_level = iot2lp_para->bcn_loss_level;
        bcn_loss_cfg = &(iot2lp_para->bcn_loss_cfg_table[bcn_loss_level]);
        dtim_num = bcn_loss_cfg->dtim_num;
    }

    // dtim_period_us = dtim_num * beacon_interval_us;

    /* last beacon timestamp */
    last_beacon_rtc_us = iot2lp_para->last_beacon_stamp_rtc_us;
    /* beacon delay */
    last_beacon_rtc_us -= iot2lp_para->last_beacon_delay_us;

    bflb_l1c_dcache_clean_all();

    /* Gets the current rtc time */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    /* calculate pds and rtc sleep time */
    if (iot2lp_para->last_beacon_stamp_rtc_valid) {
        /* beacon num form (last bcn -> now) */
        bcn_past_num = (rtc_now_us - last_beacon_rtc_us + PROTECT_BF_MS * 1000) / beacon_interval_us;

        if (iot2lp_para->bcmc_dtim_mode) {
            dtim_num = dtim_num * iot2lp_para->beacon_dtim_period;

            dtim_num = dtim_num - (bcn_past_num % dtim_num);

            uint8_t next_dtim_count = (iot2lp_para->last_beacon_dtim_count + bcn_past_num + dtim_num) % iot2lp_para->beacon_dtim_period;
            if (next_dtim_count) {
                if (dtim_num + next_dtim_count > iot2lp_para->beacon_dtim_period) {
                    dtim_num = dtim_num + next_dtim_count - iot2lp_para->beacon_dtim_period;
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
        rtc32k_error_us = (int32_t)((int64_t)pds_sleep_us * iot2lp_para->rtc32k_error_ppm / (1000 * 1000));

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
    BL_LP_LOG("rc32k code %ld\r\n", iot2lp_para->rc32k_fr_ext);
    BL_LP_LOG("rtc ppm %ld\r\n", iot2lp_para->rtc32k_error_ppm);
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

    if (gp_lp_io_cfg) {
        g_lp_io_cfg_bak = *gp_lp_io_cfg;
        BL_LP_LOG("io_wakeup_unmask: 0x%llX\r\n", (unsigned long long)g_lp_io_cfg_bak.io_wakeup_unmask);
        bl_lp_io_wakeup_init(&g_lp_io_cfg_bak);
    }

    if (gp_lp_acomp_cfg) {
        g_lp_acomp_cfg_bak = *gp_lp_acomp_cfg;
        BL_LP_LOG("acomp0: %d, acomp1: %d\r\n", g_lp_acomp_cfg_bak.acomp0_trig_mode, g_lp_acomp_cfg_bak.acomp1_trig_mode);
        bl_lp_acomp_wakeup_init(&g_lp_acomp_cfg_bak);
    }

    iot2lp_para->wifi_rx_buff = (uint8_t *)((uint32_t)export_get_rx_buffer1_addr() & 0x2FFFFFFF);
    /* lpfw cfg: system para */
    iot2lp_para->mcu_sts = bl_lp_fw_cfg->mcu_sts;
    // iot2lp_para->lpfw_loss_cnt = 0;
    // iot2lp_para->lpfw_recv_cnt = 0;
    iot2lp_para->lpfw_wakeup_cnt = 0;
    iot2lp_para->pattern = 0xAA5555AA;
    iot2lp_para->wakeup_flag = 0;
    iot2lp_para->flash_offset = bflb_sf_ctrl_get_flash_image_offset(0, SF_CTRL_FLASH_BANK0);
    iot2lp_para->app_entry = (uintptr_t)lp_fw_restore_cpu_para;
    iot2lp_para->args[0] = GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR;

    /* rc32k auto calibration */
    iot2lp_para->rc32k_auto_cal_en = 1;

    /* allow dtim wakeup ,if tim_wakeup_en == 1 */
    iot2lp_para->tim_wakeup_en = bl_lp_fw_cfg->tim_wakeup_en;

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
        memcpy(iot2lp_para->bssid, bl_lp_fw_cfg->bssid, 6);
        memcpy(iot2lp_para->local_mac, bl_lp_fw_cfg->mac, 6);
        iot2lp_para->ap_channel = bl_lp_fw_cfg->channel;
        iot2lp_para->aid = bl_lp_fw_cfg->aid;
        iot2lp_para->bcn_target_level = bl_lp_fw_cfg->rssi;

        /* lpfw rx timeout */
        iot2lp_para->mtimer_timeout_en = 1;
        iot2lp_para->mtimer_timeout_mini_us = bl_lp_fw_cfg->mtimer_timeout_mini_us;
        iot2lp_para->mtimer_timeout_max_us = bl_lp_fw_cfg->mtimer_timeout_max_us;

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

    bl_lp_debug_record_time(iot2lp_para, "lp_fw_save_cpu_para");

    lp_fw_save_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);

    if (iot2lp_para->wakeup_flag == 0) {
        /* Check io_stat , judge whether to enter PDS mode */
        /* if io_stat isn't 0x0, will sleep 2ms */
        if (1 == bl_lp_wakeup_check()) {
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
    reg = reg | iot2lp_para->rc32k_fr_ext << 22;
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
        /* register */
        bflb_irq_attach(MSOFT_IRQn, (irq_callback)bl_lp_soft_irq, NULL);
        /* trig soft int */
        BL_LP_SOFT_INT_TRIG;
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
    BL_LP_LOG("rc32k code %ld, rc32k ppm %ld \r\n", iot2lp_para->rc32k_fr_ext, iot2lp_para->rtc32k_error_ppm);
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

uint32_t bl_lp_get_hbn_wakeup_reason(void)
{
    return iot2boot2_para->wakeup_reason;
}
void bl_lp_get_hbn_wakeup_io_bits(uint8_t *io_bits, uint8_t *io_edge_bits)
{
    *io_bits = iot2boot2_para->wkup_io_bits;
    *io_edge_bits = iot2boot2_para->wkup_io_edge_bits;
}

void bl_lp_get_hbn_wakeup_acomp_bits(uint8_t *acomp_bits, uint8_t *acomp_edge_bits)
{
    *acomp_bits = iot2boot2_para->wkup_acomp_bits;
    *acomp_edge_bits = iot2boot2_para->wkup_acomp_edge_bits;
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

    if (gp_lp_io_cfg) {
        g_lp_io_cfg_bak = *gp_lp_io_cfg;
        BL_LP_LOG("io_wakeup_unmask: 0x%llX\r\n", g_lp_io_cfg_bak.io_wakeup_unmask);
        bl_lp_io_wakeup_init(&g_lp_io_cfg_bak);
    }

    if (gp_lp_acomp_cfg) {
        g_lp_acomp_cfg_bak = *gp_lp_acomp_cfg;
        BL_LP_LOG("acomp0: %d, acomp1: %d\r\n", g_lp_acomp_cfg_bak.acomp0_trig_mode, g_lp_acomp_cfg_bak.acomp1_trig_mode);
        bl_lp_acomp_wakeup_init(&g_lp_acomp_cfg_bak);
    }

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
    beacon_interval_us = iot2lp_para->beacon_interval_tu * 1024;

    /* get period of dtim */
    if (iot2lp_para->continuous_loss_cnt) {
        uint32_t bcn_loss_level = iot2lp_para->bcn_loss_level;
        lp_fw_bcn_loss_level_t *bcn_loss_cfg = &(iot2lp_para->bcn_loss_cfg_table[bcn_loss_level]);
        dtim_num = bcn_loss_cfg->dtim_num;
        // dtim_period_us = bcn_loss_cfg->dtim_num * beacon_interval_us;
    } else {
        dtim_num = iot2lp_para->dtim_num;
        // dtim_period_us = dtim_num * beacon_interval_us;
    }

    /* Time stamp of the last beacon */
    last_beacon_rtc_us = iot2lp_para->last_beacon_stamp_rtc_us;
    last_beacon_rtc_us -= iot2lp_para->last_beacon_delay_us;

    /* Gets the current rtc time */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    /* beacon num form (last bcn -> now) */
    /* The time difference since the last beacon was received, divided by beacon interval */
    bcn_past_num = (rtc_now_us - last_beacon_rtc_us + beacon_interval_us / 2) / beacon_interval_us;

    if (bcn_past_num == 0) {
        return 1;
    } else {
        if (iot2lp_para->bcmc_dtim_mode) {
            dtim_num = dtim_num * iot2lp_para->beacon_dtim_period;
            now_dtim_count = (iot2lp_para->last_beacon_dtim_count + bcn_past_num) % iot2lp_para->beacon_dtim_period;
    
            if (now_dtim_count != 0) {
                return 1;
            }
    
            if ((bcn_past_num + (iot2lp_para->beacon_dtim_period - iot2lp_para->last_beacon_dtim_count) % iot2lp_para->beacon_dtim_period) % dtim_num != 0) {
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
int bl_lp_get_next_beacon_time(uint8_t mode)
{
    uint64_t last_beacon_rtc_us, rtc_now_us, rtc_cnt;
    uint32_t dtim_period_us, beacon_interval_us;
    int32_t next_time;

    if (iot2lp_para->last_beacon_stamp_rtc_valid == 0) {
        /* It shouldn't be here */
        return -1;
    }

    /* beacon interval us, TU * 1024 */
    beacon_interval_us = iot2lp_para->beacon_interval_tu * 1024;

    /* Time stamp of the last beacon */
    last_beacon_rtc_us = iot2lp_para->last_beacon_stamp_rtc_us;
    last_beacon_rtc_us -= iot2lp_para->last_beacon_delay_us;

    /* get the current rtc time */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    if (mode) {
        /* get period of beacon */
        dtim_period_us = beacon_interval_us;
    } else {
        /* get period of dtim */
        if (iot2lp_para->continuous_loss_cnt) {
            uint32_t bcn_loss_level = iot2lp_para->bcn_loss_level;
            lp_fw_bcn_loss_level_t *bcn_loss_cfg = &(iot2lp_para->bcn_loss_cfg_table[bcn_loss_level]);
            dtim_period_us = bcn_loss_cfg->dtim_num * beacon_interval_us;
        } else {
            dtim_period_us = iot2lp_para->dtim_num * beacon_interval_us;
        }
    }

    if (last_beacon_rtc_us + dtim_period_us >= rtc_now_us) {
        /* Before the expected time */
        next_time = (last_beacon_rtc_us + dtim_period_us - rtc_now_us);
    } else {
        /* After the expected time */
        next_time = beacon_interval_us - ((rtc_now_us - last_beacon_rtc_us) % beacon_interval_us);
    }

    /* us to ms */
    next_time = next_time / 1000;

    return next_time;
}

int bl_lp_init(void)
{
    assert(!gp_lp_env);

    gp_lp_env = malloc(sizeof(struct lp_env));
    assert(gp_lp_env);

    memset(gp_lp_env, 0, sizeof(struct lp_env));

#if defined(CFG_BL_WIFI_PS_ENABLE)
    bl_lp_fw_init();
#endif

    return 0;
}

int bl_lp_sys_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg)
{
    assert(gp_lp_env);

    if (enter_callback == NULL && exit_callback == NULL) {
        return -1;
    }

    gp_lp_env->sys_pre_enter_callback = enter_callback;
    gp_lp_env->sys_after_exit_callback = exit_callback;
    gp_lp_env->sys_enter_arg = enter_arg;
    gp_lp_env->sys_exit_arg = exit_arg;

    return 0;
}

int bl_lp_user_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg)
{
    assert(gp_lp_env);

    if (enter_callback == NULL && exit_callback == NULL) {
        return -1;
    }

    gp_lp_env->user_pre_enter_callback = enter_callback;
    gp_lp_env->user_after_exit_callback = exit_callback;
    gp_lp_env->user_enter_arg = enter_arg;
    gp_lp_env->user_exit_arg = exit_arg;

    return 0;
}

void bl_lp_call_user_pre_enter(void)
{
    assert(gp_lp_env);

    if (gp_lp_env->user_pre_enter_callback) {
        gp_lp_env->user_pre_enter_callback(gp_lp_env->user_enter_arg);
    }
}

void bl_lp_call_user_after_exit(void)
{
    assert(gp_lp_env);

    if (gp_lp_env->user_after_exit_callback) {
        gp_lp_env->user_after_exit_callback(gp_lp_env->user_exit_arg);
    }
}

void bl_lp_call_sys_pre_enter(void)
{
    assert(gp_lp_env);

    if (gp_lp_env->sys_pre_enter_callback) {
        gp_lp_env->sys_pre_enter_callback(gp_lp_env->sys_enter_arg);
    }
}

void bl_lp_call_sys_after_exit(void)
{
    assert(gp_lp_env);

    if (gp_lp_env->sys_after_exit_callback) {
        gp_lp_env->sys_after_exit_callback(gp_lp_env->sys_exit_arg);
    }
}

static void bl_lp_soft_irq(void)
{
    uint64_t wakeup_io_bits = iot2lp_para->wake_io_bits;
    uint32_t wakeup_acmp_bits = iot2lp_para->wake_acomp_bits;

    /* disable soft int */
    bflb_irq_disable(MSOFT_IRQn);
    /* clear soft int */
    BL_LP_SOFT_INT_CLEAR;

    if ((iot2lp_para->wakeup_reason & LPFW_WAKEUP_IO) && lp_soft_callback.wakeup_io_callback) {
        lp_soft_callback.wakeup_io_callback(wakeup_io_bits);
    }

    if ((iot2lp_para->wakeup_reason & LPFW_WAKEUP_ACOMP) && lp_soft_callback.wakeup_acomp_callback) {
        lp_soft_callback.wakeup_acomp_callback(wakeup_acmp_bits);
    }

    /* clear */
    iot2lp_para->wake_io_bits = 0;
    iot2lp_para->wake_acomp_bits = 0;
}

void bl_lp_wakeup_io_int_register(void (*wakeup_io_callback)(uint64_t wake_up_io_bits))
{
    lp_soft_callback.wakeup_io_callback = wakeup_io_callback;
}

void bl_lp_wakeup_acomp_int_register(void (*wakeup_acomp_callback)(uint32_t wake_up_acomp))
{
    lp_soft_callback.wakeup_acomp_callback = wakeup_acomp_callback;
}

int bl_lp_wakeup_io_get_mode(uint8_t io_num)
{
    uint64_t wakeup_io_bits = iot2lp_para->wake_io_bits;
    uint64_t wakeup_io_edge_bits = iot2lp_para->wake_io_edge_bits;
    uint8_t trig_mode;

    if (io_num >= BL_LP_WAKEUP_IO_MAX_NUM) {
        return -1;
    }

    if ((wakeup_io_bits & ((uint64_t)0x0001 << io_num)) == 0) {
        return 0;
    }

    if (io_num >= 16 && io_num <= 19) {
        trig_mode = g_lp_io_cfg_bak.io_16_19_aon_trig_mode;

        if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE) {
            if ((wakeup_io_edge_bits & ((uint64_t)0x0001 << io_num)) == 0) {
                return BL_LP_IO_WAKEUP_MODE_FALLING;
            } else {
                return BL_LP_IO_WAKEUP_MODE_RISING;
            }
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_FALLING_EDGE || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_FALLING_EDGE) {
            return BL_LP_IO_WAKEUP_MODE_FALLING;
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_RISING_EDGE || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_RISING_EDGE) {
            return BL_LP_IO_WAKEUP_MODE_RISING;
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_LOW_LEVEL || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_LOW_LEVEL) {
            return BL_LP_IO_WAKEUP_MODE_LOW;
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_HIGH_LEVEL || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_HIGH_LEVEL) {
            return BL_LP_IO_WAKEUP_MODE_HIGH;
        } else {
            return -1;
        }
    }

    if (io_num <= 7) {
        trig_mode = g_lp_io_cfg_bak.io_0_7_pds_trig_mode;
    } else if (io_num >= 8 && io_num <= 15) {
        trig_mode = g_lp_io_cfg_bak.io_8_15_pds_trig_mode;
    } else if (io_num >= 20 && io_num <= 27) {
        trig_mode = g_lp_io_cfg_bak.io_20_27_pds_trig_mode;
    } else if (io_num >= 28 && io_num <= 34) {
        trig_mode = g_lp_io_cfg_bak.io_28_34_pds_trig_mode;
    } else {
        return -1;
    }

    if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_FALLING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_FALLING;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_HIGH_LEVEL) {
        return BL_LP_IO_WAKEUP_MODE_HIGH;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_RISING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_RISING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_RISING;
    } else {
        return -1;
    }
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

int bl_lp_get_wake_io_state(void)
{
    return (int)gp_lp_env->gpio_stat;
}

void bl_lp_set_resume_wifi(void)
{
    assert(gp_lp_env);

    gp_lp_env->wifi_hw_resume = 1;
}

void bl_lp_clear_resume_wifi(void)
{
    assert(gp_lp_env);

    gp_lp_env->wifi_hw_resume = 0;
}

int bl_lp_get_resume_wifi(void)
{
    assert(gp_lp_env);

    return gp_lp_env->wifi_hw_resume;
}

void bl_set_fw_ready(void)
{
    assert(gp_lp_env);

    gp_lp_env->wifi_fw_ready = 1;
}

void bl_clear_fw_ready(void)
{
    assert(gp_lp_env);

    gp_lp_env->wifi_fw_ready = 0;
}

int bl_check_fw_ready(void)
{
    assert(gp_lp_env);

    return gp_lp_env->wifi_fw_ready;
}

static void bl_lp_set_aon_io(bl_lp_aon_io_cfg_t cfg)
{
    uint8_t pu, pd, ie;

    ie = cfg.ie;
    if (cfg.res_mode == BL_LP_IO_RES_PULL_UP) {
        pu = 1;
        pd = 0;
    } else if (cfg.res_mode == BL_LP_IO_RES_PULL_DOWN) {
        pu = 0;
        pd = 1;
    } else {
        pu = 0;
        pd = 0;
    }

    if (pu | pd | ie) {
        /* set pin's aonPadCfg */
        HBN_AON_PAD_CFG_Type aonPadCfg;
        aonPadCfg.ctrlEn = 1;
        aonPadCfg.ie = ie;
        aonPadCfg.oe = 0;
        aonPadCfg.pullUp = pu;
        aonPadCfg.pullDown = pd;
        HBN_Aon_Pad_Cfg(ENABLE, (cfg.pin - 16), &aonPadCfg);
    }

    if (ie & cfg.unmask) {
        uint32_t mask = 0;

        mask = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        mask = BL_GET_REG_BITS_VAL(mask, HBN_PIN_WAKEUP_MASK);
        mask = mask & ~(1 << (cfg.pin - 16));

        /* set trigMode */
        HBN_Aon_Pad_WakeUpCfg(DISABLE, cfg.trigMode, mask, 0, 7);

        /* UnMask Hbn_Irq Wakeup PDS*/
        pm_pds_wakeup_src_en(PDS_WAKEUP_BY_HBN_IRQ_OUT_EN_POS);
    }
}

static void bl_lp_io_wakeup_init(bl_lp_io_cfg_t *io_wakeup_cfg)
{
    uint32_t tmpVal;
    uint64_t io_unmask = io_wakeup_cfg->io_wakeup_unmask;
    uint8_t res_mode, pu, pd, ie;

    if (io_wakeup_cfg == NULL) {
        return;
    }

    uint32_t sf_pin_select = 0;
    uint8_t sf0_pin = 0xFF;
    uint8_t sf1_pin = 0xFF;

    /* to compliant with PDS1\2\3\7, all gpio set high resistance state, except Flash_IO and XTAL_IO */
    /* get sw uasge 0 */
    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_0);
    /* get flash type */
    sf_pin_select = (tmpVal >> 14) & 0x3f;

    if (sf_pin_select & (1 << 2)) {
        /* Flash0's pin is io4~9 */
        sf0_pin = 4;
    }

    if (sf_pin_select & (1 << 3)) {
        /* Flash1's pin is io10~15 */
        sf1_pin = 10;
    }

    uint8_t xtal32k_in_pin = 0xFF;
    uint8_t xtal32k_out_pin = 0xFF;

    /* if pmu select XTAL32K as clock, GPIO16\17 keep as analog mode */
    if (BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_GLB), HBN_F32K_SEL)) {
        /* rtc use xtal32k, GPIO16\17 is xtal32k in\out */
        xtal32k_in_pin = 16;
        xtal32k_out_pin = 17;
    }

    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 0;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioMode = GPIO_MODE_INPUT;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.gpioPin = (uint8_t)0;
    gpio_cfg.gpioFun = GPIO_FUN_GPIO;

    for (uint8_t i = 0; i < 35; i++) {
        /* except Flash0_IO (io4~9) */
        if ((sf0_pin != 0xFF) && (i >= sf0_pin) && (i < (sf0_pin + 6))) {
            continue;
        }

        /* except Flash1_IO (io10~15) */
        if ((sf1_pin != 0xFF) && (i >= sf1_pin) && (i < (sf1_pin + 6))) {
            continue;
        }

        /* except xtal32k_IO (io16~17) */
        if ((i == xtal32k_in_pin) || (i == xtal32k_out_pin)) {
            continue;
        }

        /* except UART_IO, if need log */
        // if ((i == BL616_UART_TX) || (i == BL616_UART_RX)) {
        //     continue;
        // }

        if ((io_unmask >> i) & 0x1) {
            /* wakeup pin's GLB_GPIO_FUNC_Type set as GPIO_FUN_GPIO */
            gpio_cfg.gpioPin = (uint8_t)i;
            GLB_GPIO_Init(&gpio_cfg);
        } else {
            /* set high resistance state */
            *((volatile uint32_t *)(0x200008C4 + i * 4)) = 0x00400B00;
        }
    }

    /* pds io 0~15 */
    ie = io_wakeup_cfg->io_0_15_ie;
    res_mode = io_wakeup_cfg->io_0_15_res;
    if (res_mode == BL_LP_IO_RES_PULL_UP) {
        pu = 1;
        pd = 0;
    } else if (res_mode == BL_LP_IO_RES_PULL_DOWN) {
        pu = 0;
        pd = 1;
    } else {
        pu = 0;
        pd = 0;
    }
    PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(PDS_GPIO_GROUP_SET_GPIO0_GPIO15, pu, pd, ie);

    if ((io_wakeup_cfg->io_0_15_ie)) {
        if (io_unmask & 0xFFFF) {
            if (io_unmask & 0xFF) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_1_GPIO0_GPIO7);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_1_GPIO0_GPIO7, io_wakeup_cfg->io_0_7_pds_trig_mode);
            }

            if (io_unmask & 0xFF00) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_2_GPIO8_GPIO15);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_2_GPIO8_GPIO15, io_wakeup_cfg->io_8_15_pds_trig_mode);
            }

            tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
            tmpVal &= ~0xFFFF;
            tmpVal |= ((~io_unmask) & 0xFFFF);
            BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);

            pm_pds_wakeup_src_en(PDS_WAKEUP_BY_PDS_GPIO_IRQ_EN_POS);
        }
    }

    /* aon io 16~19 */
    bl_lp_aon_io_cfg_t aon_io_cfg;
    for (uint8_t aon_pin_id = 16; aon_pin_id < 20; aon_pin_id++) {
        aon_io_cfg.pin = aon_pin_id;
        aon_io_cfg.ie = *(((volatile uint8_t *)&io_wakeup_cfg->io_16_ie) + (aon_pin_id - 16));
        aon_io_cfg.res_mode = *(((volatile uint8_t *)&io_wakeup_cfg->io_16_res) + (aon_pin_id - 16));
        aon_io_cfg.unmask = (io_unmask >> aon_pin_id) & 0x01;
        aon_io_cfg.trigMode = io_wakeup_cfg->io_16_19_aon_trig_mode;
        bl_lp_set_aon_io(aon_io_cfg);
    }

    /* pds io 20~34 */
    ie = io_wakeup_cfg->io_20_34_ie;
    res_mode = io_wakeup_cfg->io_20_34_res;
    if (res_mode == BL_LP_IO_RES_PULL_UP) {
        pu = 1;
        pd = 0;
    } else if (res_mode == BL_LP_IO_RES_PULL_DOWN) {
        pu = 0;
        pd = 1;
    } else {
        pu = 0;
        pd = 0;
    }
    PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(PDS_GPIO_GROUP_SET_GPIO20_GPIO36, pu, pd, ie);

    if ((io_wakeup_cfg->io_20_34_ie)) {
        if ((io_unmask >> 20) & 0x7FFF) {
            if ((io_unmask >> 20) & 0xFF) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_3_GPIO20_GPIO27);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_3_GPIO20_GPIO27, io_wakeup_cfg->io_20_27_pds_trig_mode);
            }

            if ((io_unmask >> 20) & 0x7F00) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_4_GPIO28_GPIO34);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_4_GPIO28_GPIO34, io_wakeup_cfg->io_28_34_pds_trig_mode);
            }

            tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
            tmpVal &= ~0xFFFF0000;
            tmpVal |= ((((~io_unmask) >> 20) & 0x7FFF) << 16);
            BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);

            pm_pds_wakeup_src_en(PDS_WAKEUP_BY_PDS_GPIO_IRQ_EN_POS);
        }
    }
}

int bl_lp_io_wakeup_cfg(bl_lp_io_cfg_t *io_wakeup_cfg)
{
    gp_lp_io_cfg = io_wakeup_cfg;
    return 0;
}

static void bl_lp_set_gpio_as_analog(uint8_t pin)
{
    GLB_GPIO_Cfg_Type cfg;
    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 0;
    cfg.smtCtrl = 0;
    cfg.gpioPin = pin;
    cfg.gpioFun = GPIO_FUN_ANALOG;
    GLB_GPIO_Init(&cfg);
}

uint32_t bl_lp_set_acomp(uint8_t chan, uint8_t pin, uint8_t pos_edge_en, uint8_t neg_edge_en)
{
    struct bflb_acomp_config_s acompCfg = {
        .mux_en = ENABLE,                                      /*!< ACOMP mux enable */
        .pos_chan_sel = AON_ACOMP_CHAN_ADC0,                   /*!< ACOMP negtive channel select */
        .neg_chan_sel = AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1, /*!< ACOMP positive channel select */
        .vio_sel = BL616_ACOMP_VREF_1V65,                      /*!< ACOMP vref select */
        .scaling_factor =
            AON_ACOMP_SCALING_FACTOR_1,          /*!< ACOMP level select factor */
        .bias_prog = AON_ACOMP_BIAS_POWER_MODE1, /*!< ACOMP bias current control */
        .hysteresis_pos_volt =
            AON_ACOMP_HYSTERESIS_VOLT_NONE, /*!< ACOMP hysteresis voltage for positive */
        .hysteresis_neg_volt =
            AON_ACOMP_HYSTERESIS_VOLT_NONE, /*!< ACOMP hysteresis voltage for negtive */
    };

    if (pin == GLB_GPIO_PIN_20) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC0;
    } else if (pin == GLB_GPIO_PIN_19) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC1;
    } else if (pin == GLB_GPIO_PIN_2) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC2;
    } else if (pin == GLB_GPIO_PIN_3) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC3;
    } else if (pin == GLB_GPIO_PIN_14) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC4;
    } else if (pin == GLB_GPIO_PIN_13) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC5;
    } else if (pin == GLB_GPIO_PIN_12) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC6;
    } else if (pin == GLB_GPIO_PIN_10) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC7;
    } else {
        return -1;
    }

    /* Config Gpio as Analog */
    bl_lp_set_gpio_as_analog(pin);

    /* Config Comp0/1 */
    bflb_acomp_init(chan, &acompCfg);
    bflb_acomp_enable(chan);

    HBN_Clear_IRQ(HBN_INT_ACOMP0 + chan * 2);

    /* enable/disable POSEDGE */
    if (pos_edge_en) {
        HBN_Enable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_POSEDGE);
    } else {
        HBN_Disable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_POSEDGE);
    }

    /* enable/disable NEGEDGE */
    if (neg_edge_en) {
        HBN_Enable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_NEGEDGE);
    } else {
        HBN_Disable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_NEGEDGE);
    }

    /* UnMask Hbn_Irq Wakeup PDS*/
    pm_pds_wakeup_src_en(PDS_WAKEUP_BY_HBN_IRQ_OUT_EN_POS);

    return 0;
}

uint32_t bl_lp_set_acomp_pin(uint32_t acomp_chan)
{
    uint32_t tmpPin = 0;
    if (acomp_chan == AON_ACOMP_CHAN_ADC0) {
        tmpPin = GLB_GPIO_PIN_20;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC1) {
        tmpPin = GLB_GPIO_PIN_19;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC2) {
        tmpPin = GLB_GPIO_PIN_2;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC3) {
        tmpPin = GLB_GPIO_PIN_3;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC4) {
        tmpPin = GLB_GPIO_PIN_14;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC5) {
        tmpPin = GLB_GPIO_PIN_13;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC6) {
        tmpPin = GLB_GPIO_PIN_12;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC7) {
        tmpPin = GLB_GPIO_PIN_10;
    } else {
        return -1;
    }

    bl_lp_set_gpio_as_analog(tmpPin);
    return 0;
}

static void bl_lp_acomp_wakeup_init(bl_lp_acomp_cfg_t *acomp_wakeup_cfg)
{
    if (acomp_wakeup_cfg->acomp0_en && acomp_wakeup_cfg->acomp0_trig_mode) {
        bl_lp_set_acomp(0, acomp_wakeup_cfg->acomp0_io_num, (acomp_wakeup_cfg->acomp0_trig_mode & 0x02), (acomp_wakeup_cfg->acomp0_trig_mode & 0x01));
    } else {
        bflb_acomp_disable(0);
    }

    if (acomp_wakeup_cfg->acomp1_en && acomp_wakeup_cfg->acomp1_trig_mode) {
        bl_lp_set_acomp(1, acomp_wakeup_cfg->acomp1_io_num, (acomp_wakeup_cfg->acomp1_trig_mode & 0x02), (acomp_wakeup_cfg->acomp1_trig_mode & 0x01));
    } else {
        bflb_acomp_disable(1);
    }

    arch_delay_us(15);
    HBN_Clear_IRQ(HBN_INT_ACOMP0);
    HBN_Clear_IRQ(HBN_INT_ACOMP1);
}

int bl_lp_wakeup_acomp_get_mode(uint8_t acomp_num)
{
    uint8_t wakeup_acomp_bits = iot2lp_para->wake_acomp_bits;
    uint8_t wakeup_acomp_edge_bits = iot2lp_para->wake_acomp_edge_bits;

    if (acomp_num == 0) {
        if (g_lp_acomp_cfg_bak.acomp0_en == BL_LP_ACOMP_DISABLE || (wakeup_acomp_bits & 0x01) == 0) {
            return 0;
        }
        if (g_lp_acomp_cfg_bak.acomp0_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING) {
            return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
        } else if (g_lp_acomp_cfg_bak.acomp0_trig_mode == BL_LP_ACOMP_TRIG_EDGE_RISING) {
            return BL_LP_ACOMP_WAKEUP_MODE_RISING;
        } else if (g_lp_acomp_cfg_bak.acomp0_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING_RISING) {
            if (wakeup_acomp_edge_bits & 0x01) {
                return BL_LP_ACOMP_WAKEUP_MODE_RISING;
            } else {
                return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
            }
        } else {
            return -1;
        }
    }

    if (acomp_num == 1) {
        if (g_lp_acomp_cfg_bak.acomp1_en == BL_LP_ACOMP_DISABLE || (wakeup_acomp_bits & 0x02) == 0) {
            return 0;
        }
        if (g_lp_acomp_cfg_bak.acomp1_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING) {
            return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
        } else if (g_lp_acomp_cfg_bak.acomp1_trig_mode == BL_LP_ACOMP_TRIG_EDGE_RISING) {
            return BL_LP_ACOMP_WAKEUP_MODE_RISING;
        } else if (g_lp_acomp_cfg_bak.acomp1_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING_RISING) {
            if (wakeup_acomp_edge_bits & 0x02) {
                return BL_LP_ACOMP_WAKEUP_MODE_RISING;
            } else {
                return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
            }
        } else {
            return -1;
        }
    }

    return -2;
}

int bl_lp_acomp_wakeup_cfg(bl_lp_acomp_cfg_t *acomp_wakeup_cfg)
{
    gp_lp_acomp_cfg = acomp_wakeup_cfg;
    return 0;
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


