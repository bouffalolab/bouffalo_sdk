#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "bl_lp.h"
#include "bl616d_hbn.h"
#include "bl616d_pds.h"
#include "bl616d_pm.h"
#include "bl616d_xip_recovery.h"

#if (1)
#define BL_LP_LOG        printf
#define BL_LP_TIME_DEBUG 1
#else
#define BL_LP_LOG
#define BL_LP_TIME_DEBUG 0
#endif

#define GET_OFFSET(_type, _member) ((unsigned long)(&((_type *)0)->_member))

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

static uint64_t g_rtc_timestamp_before_sleep_us = 0;    /* rtc 1 */
static uint64_t g_rtc_timestamp_after_sleep_us = 0;     /* rtc 2 */
static uint64_t g_mtimer_timestamp_before_sleep_us = 0; /* mtime 1 */
static uint64_t g_mtimer_timestamp_after_sleep_us = 0;  /* mtime 2 */
static uint64_t g_virtual_timestamp_base_us = 0;        /* virtual time base */

static struct lp_env *gp_lp_env = NULL;
ATTR_TCM_CONST_SECTION iot2lp_para_t *const iot2lp_para = (iot2lp_para_t *)IOT2LP_PARA_ADDR;
extern uint32_t *export_get_rx_buffer1_addr(void);

bl_lp_fw_cfg_t lpfw_cfg = {
    .tim_wakeup_en = 1,
    .rtc_timeout_us = (60 * 1000 * 1000),
    .dtim_origin = 10,
    .dtim_num = 0,
};

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
        iot2lp_para->mtimer_parameter->mtimer_timeout_en = 0;
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

static void bl_lp_print_wifi_param(void)
{
    if (iot2lp_para && iot2lp_para->wifi_parameter) {
        lp_fw_wifi_para_t *b_wifi_param = iot2lp_para->wifi_parameter;

        BL_LP_LOG("=== WiFi Param Content Before PDS ===\r\n");
        BL_LP_LOG("wifi_param: %x\r\n", b_wifi_param);

        BL_LP_LOG("ap_channel: %u\r\n", b_wifi_param->ap_channel);
        BL_LP_LOG("tim_wakeup_en: %u\r\n", b_wifi_param->tim_wakeup_en);
        BL_LP_LOG("aid: %u\r\n", b_wifi_param->aid);
        BL_LP_LOG("bssid: %02X:%02X:%02X:%02X:%02X:%02X\r\n", b_wifi_param->bssid[0], b_wifi_param->bssid[1],
                  b_wifi_param->bssid[2], b_wifi_param->bssid[3], b_wifi_param->bssid[4], b_wifi_param->bssid[5]);
        BL_LP_LOG("local_mac: %02X:%02X:%02X:%02X:%02X:%02X\r\n", b_wifi_param->local_mac[0],
                  b_wifi_param->local_mac[1], b_wifi_param->local_mac[2], b_wifi_param->local_mac[3],
                  b_wifi_param->local_mac[4], b_wifi_param->local_mac[5]);
        BL_LP_LOG("bcmc_dtim_mode: %u\r\n", b_wifi_param->bcmc_dtim_mode);
        BL_LP_LOG("last_beacon_dtim_count: %u\r\n", b_wifi_param->last_beacon_dtim_count);
        BL_LP_LOG("beacon_dtim_period: %u\r\n", b_wifi_param->beacon_dtim_period);
        BL_LP_LOG("bcn_target_level: %d\r\n", b_wifi_param->bcn_target_level);
        BL_LP_LOG("dtim_num: %u\r\n", b_wifi_param->dtim_num);
        BL_LP_LOG("beacon_leg_rate: %u\r\n", b_wifi_param->beacon_leg_rate);
        BL_LP_LOG("beacon_interval_tu: %u\r\n", b_wifi_param->beacon_interval_tu);
        BL_LP_LOG("wifi_rx_buff: 0x%08X\r\n", (unsigned int)b_wifi_param->wifi_rx_buff);
        BL_LP_LOG("buf_addr: 0x%08X\r\n", b_wifi_param->buf_addr);
        BL_LP_LOG("pack_env: 0x%08X\r\n", b_wifi_param->pack_env);
        BL_LP_LOG("==========================================\r\n");
    } else {
        BL_LP_LOG("WiFi param is NULL before PDS\r\n");
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
                         "la      a0, freertos_risc_v_trap_handler\n\t" // mcu default_trap_handler
                         "ori     a0, a0, 3\n\t"
                         "csrw    mtvec, a0\n\t");
}

void bl_lp_fw_bcn_loss_cfg(lp_fw_bcn_loss_level_t *cfg_table, uint16_t table_num, uint16_t loop_start,
                           uint16_t loss_max)
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

    iot2lp_para->bcn_loss_info->continuous_loss_cnt_max = loss_max;
    iot2lp_para->bcn_loss_info->bcn_loss_cfg_table = cfg_table;
    iot2lp_para->bcn_loss_info->bcn_loss_loop_start = loop_start;
    iot2lp_para->bcn_loss_info->bcn_loss_level_max = table_num;

    /* clear */
    iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
    iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
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
        { 10, 0, 2000, 6000 },  /* loss 0, unused */
        { 8, 0, 2000, 10000 },  /* loss 1 */
        { 6, 0, 3000, 12000 },  /* loss 2 */
        { 6, 0, 4000, 14000 },  /* loss 3 */
        { 6, 0, 6000, 18000 },  /* loss 4 */
        { 6, 1, 18000, 42000 }, /* loss 5, wakeup */

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

// static void bl_lp_fw_bcn_loss_init(void)
// {
//     memset(&beacon_loss_info, 0, sizeof(beacon_loss_info));
//     iot2lp_para->bcn_loss_info = &beacon_loss_info;

//     iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
//     iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
//     bl_lp_fw_bcn_loss_cfg_dtim_default(iot2lp_para->wifi_parameter->dtim_num);
// }

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
        rtc_cnt = (BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H));
        rtc_cnt <<= 32;
        rtc_cnt |= BL_RD_REG(HBN_BASE, HBN_RTC_TIME_L);

        /* calculate RTC Comp cnt */
        rtc_cnt += BL_US_TO_PDS_CNT(sleep_us);
    } else {
        rtc_cnt = rtc_wakeup_cmp_cnt;
    }

    // iot2lp_para = (iot2lp_para_t *)IOT2LP_PARA_ADDR;
    wakeup_source.rtc_wakeup_cnt = rtc_cnt;
    wakeup_source.rtc_wakeup_en = 1;

    HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, (uint32_t)rtc_cnt, (uint32_t)(rtc_cnt >> 32), HBN_RTC_COMP_BIT0_39);
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

bl_lp_fw_info_t *bl_lpfw_bin_get_info(void)
{
    return (bl_lp_fw_info_t *)__lpfw_load_addr;
}

int bl_lpfw_bin_check(void)
{
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();

    if (lpfw_info->magic_code != 0x7766706C) {
        BL_LP_LOG("lpfw magic code error\r\n");
        return -1;
    }

    if ((lpfw_info->lpfw_memory_start & 0x0FFFFFFF) != ((uint32_t)__lpfw_share_start__ & 0x0FFFFFFF)) {
        BL_LP_LOG("lpfw memory start address error: lpfw:0x%08X, app:0x%08X\r\n", lpfw_info->lpfw_memory_start,
                  (uint32_t)__lpfw_share_start__);
        return -2;
    }

    if ((lpfw_info->lpfw_memory_end - lpfw_info->lpfw_memory_start) >
        ((uint32_t)__lpfw_share_end__ - (uint32_t)__lpfw_share_start__)) {
        BL_LP_LOG("lpfw memory size_over\r\n");
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

    uint32_t lpfw_addr = ((uint32_t)__lpfw_share_start__ & 0x0FFFFFFF) | 0x60000000; /* cacheable */
    uint32_t lpfw_size = *((uint32_t *)__lpfw_load_addr - 7);

    BL_LP_LOG("lpfw_addr:0x%08x\r\n", lpfw_addr);
    BL_LP_LOG("lpfw_size:%d\r\n", lpfw_size);
    BL_LP_LOG("__lpfw_load_addr:0x%08x\r\n", __lpfw_load_addr);

    /* load */
    memcpy((void *)lpfw_addr, __lpfw_load_addr, lpfw_size);
    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)lpfw_addr, lpfw_size);

    return 0;
}

#if 1
#include "bflb_sec_sha.h"

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
int bl_lpfw_ram_verify(void)
{
    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    uint32_t lpfw_addr = ((uint32_t)__lpfw_share_start__ & 0x0FFFFFFF) | 0x60000000; /* cacheable */
    uint32_t lpfw_size = *((uint32_t *)__lpfw_load_addr - 7);
    uint8_t *lpfw_sha256 = (uint8_t *)(__lpfw_load_addr - 16);
    uint8_t result[32];

    /* hardware sha256 */
    lpfw_sec_sha256(lpfw_addr, lpfw_size, result);

    if (memcmp(result, lpfw_sha256, 32) != 0) {
        BL_LP_LOG("lpfw sha256 check failed\r\n");
        return -1;
    }
    return 0;
}

void bl_lp_fw_init(void)
{
    /* Global iot2lp_para pointer is already initialized as const */
    PDS_IntClear();
    PDS_Mask_All_Wakeup_Src();
    HBN_Pin_WakeUp_Mask(0xF);

    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    /* clean iot2lp_para */
    memset(iot2lp_para, 0, (uint32_t)&iot2lp_para->reset_keep - (uint32_t)iot2lp_para);

#if (BL_LP_TIME_DEBUG)
    /* time debug */
    static ATTR_NOCACHE_RAM_SECTION lp_fw_time_debug_t time_debug_buff[TIME_DEBUG_NUM_MAX] = { 0 };
    memset(time_debug_buff, 0, sizeof(time_debug_buff));
    iot2lp_para->time_debug = time_debug_buff;
#endif

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
    // beacon_loss_info.bcn_loss_cfg_table = bcn_loss_cfg;
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
    lp_fw_uart_cfg.uart_tx_io = 11;
    lp_fw_uart_cfg.uart_rx_io = 42;
    lp_fw_uart_cfg.baudrate = 2000000;
    iot2lp_para->uart_config = &lp_fw_uart_cfg;

    memset(&lp_fw_clock_cfg, 0, sizeof(lp_fw_clock_cfg));
    lp_fw_clock_cfg.mcu_clk_sel = GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M;
    lp_fw_clock_cfg.hclk_div = 1;
    lp_fw_clock_cfg.bclk_div = 0;
    lp_fw_clock_cfg.xclk_sel = HBN_MCU_XCLK_XTAL;
    iot2lp_para->clock_config = &lp_fw_clock_cfg;

    /* lpfw info */
    memset(&lp_info_struct, 0, sizeof(lp_info_struct));
    iot2lp_para->lp_info = &lp_info_struct;
    bl_lp_info_clear(&lp_info_struct);

    bl_lp_xip_para_save();

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
    assert(!gp_lp_env);

    gp_lp_env = malloc(sizeof(struct lp_env));
    assert(gp_lp_env);

    memset(gp_lp_env, 0, sizeof(struct lp_env));

    bl_lp_fw_init();

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

static void print_lpfw_cfg_content(bl_lp_fw_cfg_t *cfg)
{
    printf("=== LPFW Configuration Content ===\n");
    printf("tim_wakeup_en: %u\n", cfg->tim_wakeup_en);
    printf("lpfw_copy: %u\n", cfg->lpfw_copy);
    printf("lpfw_verify: %u\n", cfg->lpfw_verify);
    printf("channel: %u\n", cfg->channel);
    printf("rssi: %d\n", cfg->rssi);
    printf("bssid: %02X:%02X:%02X:%02X:%02X:%02X\n", cfg->bssid[0], cfg->bssid[1], cfg->bssid[2], cfg->bssid[3],
           cfg->bssid[4], cfg->bssid[5]);
    printf("mac: %02X:%02X:%02X:%02X:%02X:%02X\n", cfg->mac[0], cfg->mac[1], cfg->mac[2], cfg->mac[3], cfg->mac[4],
           cfg->mac[5]);
    printf("bcmc_dtim_mode: %u\n", cfg->bcmc_dtim_mode);
    printf("dtim_num: %u\n", cfg->dtim_num);
    printf("dtim_origin: %u\n", cfg->dtim_origin);
    printf("rsv: %u\n", cfg->rsv);
    printf("mtimer_timeout_mini_us: %u\n", cfg->mtimer_timeout_mini_us);
    printf("mtimer_timeout_max_us: %u\n", cfg->mtimer_timeout_max_us);
    printf("mcu_sts: 0x%08X\n", cfg->mcu_sts);
    printf("aid: %u\n", cfg->aid);
    printf("rtc_wakeup_cmp_cnt: 0x%016llX\n", cfg->rtc_wakeup_cmp_cnt);
    printf("rtc_timeout_us: %llu\n", cfg->rtc_timeout_us);
    printf("wakeup_reason: %d\n", cfg->wakeup_reason);
    printf("lpfw_recv_cnt: %u\n", cfg->lpfw_recv_cnt);
    printf("lpfw_loss_cnt: %u\n", cfg->lpfw_loss_cnt);
    printf("buf_addr: 0x%08X\n", cfg->buf_addr);
    printf("pack_env: 0x%08X\n", cfg->pack_env);
    printf("================================\n");
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

int ATTR_TCM_SECTION bl_lp_fw_enter(bl_lp_fw_cfg_t *bl_lp_fw_cfg)
{
    uint64_t rtc_sleep_us, rtc_wakeup_cmp_cnt;
    uint32_t dtim_num, bcn_past_num, beacon_interval_us;
    uint64_t last_beacon_rtc_us;
    uint64_t rtc_cnt, rtc_now_us;
    uint32_t pds_sleep_us;
    uint32_t period_dtim;
    uint32_t bcn_loss_level;
    int32_t rtc32k_error_us;

    lp_fw_bcn_loss_level_t *bcn_loss_cfg = NULL;

    if (bl_lp_fw_cfg == NULL) {
        return -1;
    }

    bl_lp_debug_record_time(iot2lp_para, "bl_lp_fw_enter");

    // print_lpfw_cfg_content(bl_lp_fw_cfg);

    /* clean wakeup reason */
    memset(&wakeup_reason, 0, sizeof(wakeup_reason));
    iot2lp_para->wakeup_reason_info = &wakeup_reason;

    /* clean wakeup source */
    iot2lp_para->wakeup_source_parameter->rtc_wakeup_en = 0;
    iot2lp_para->wakeup_source_parameter->rtc_wakeup_cnt = 0;

    LP_HOOK(pre_user, bl_lp_fw_cfg);

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

    /* ready sleep*/
    iot2lp_para->wakeup_flag = 0;
    iot2lp_para->pattern = 0xAA5555AA;
    iot2lp_para->app_entry = (uintptr_t)lp_fw_restore_cpu_para;
    iot2lp_para->args[0] = GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR;

    /* cacheable */
    pm_set_wakeup_callback(
        (void (*)(void))((uint32_t)__lpfw_share_start__ | 0x60000000) /*(void (*)(void))LP_FW_PRE_JUMP_ADDR*/);

    LP_HOOK(pre_sleep, iot2lp_para);

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

        if (dtim_num <= 3) {
            dtim_num += (4 - dtim_num);
        }

        BL_LP_LOG("period_dtim:%d\r\n", period_dtim);
        BL_LP_LOG("dtim_num:%d\r\n", dtim_num);
        BL_LP_LOG("bcn_past_num:%d\r\n", bcn_past_num);
        BL_LP_LOG("beacon_interval_us:%d\r\n", beacon_interval_us);
        BL_LP_LOG("rc32k code %d\r\n", iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext);
        /* last bcn -> next bcn */
        pds_sleep_us = (dtim_num + bcn_past_num) * beacon_interval_us;

        /* rc32k error compensation (last bcn -> next bcn) */
        rtc32k_error_us =
            (int32_t)((int64_t)pds_sleep_us * iot2lp_para->rc32k_trim_parameter->rtc32k_error_ppm / (1000 * 1000));

        /* now -> next bcn */
        pds_sleep_us = pds_sleep_us - (uint32_t)(rtc_now_us - last_beacon_rtc_us);

        /* phy init time */
        pds_sleep_us -= 280000;
        pds_sleep_us -= 500;

        /* rc32k error */
        if (rtc32k_error_us > 0 || pds_sleep_us > (-rtc32k_error_us)) {
            pds_sleep_us += rtc32k_error_us;
        } else {
            pds_sleep_us = 0;
        }
    } else {
        pds_sleep_us = 0;
    }

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
        /* debug cpu parameter */

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
    // CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x3;
#else
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x2;
    // CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x2;
#endif

    /* disable mexstatus SPUSHEN and SPSWAPEN for ipush/ipop*/
    uint32_t mexstatus = __get_MEXSTATUS();
    mexstatus &= ~(0x3 << 16);
    __set_MEXSTATUS(mexstatus);

    bl_lp_debug_record_time(iot2lp_para, "return APP");

    /* clock re-init */

    /* watch-dog disable */

    /* rc32k code restored */
    HBN_Set_RC32K_R_Code(iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext);

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


int bl_lp_get_bcn_delay_ready(void)
{
    if (iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_status <
        iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_size) {
        return 0;
    } else {
        return 1;
    }
}


ATTR_TCM_SECTION void bl_lp_bcn_loss_cnt_clear(void)
{
    if (!iot2lp_para->bcn_loss_info) {
        return;
    }

    /* clear continuous loss cnt */
    iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
    iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
}

ATTR_TCM_SECTION void bl_lp_bcn_timestamp_update(uint64_t beacon_timestamp_us, uint64_t rtc_timestamp_us, uint32_t mode)
{
    /* update timestamp_us */
    iot2lp_para->last_beacon_stamp_rtc_valid = mode;
    iot2lp_para->last_beacon_stamp_rtc_us = rtc_timestamp_us;
    iot2lp_para->last_beacon_stamp_beacon_us = beacon_timestamp_us;
}

ATTR_TCM_SECTION uint32_t bl_lp_get_beacon_interval_tu(void)
{
    return iot2lp_para->wifi_parameter->beacon_interval_tu;
}

ATTR_TCM_SECTION uint32_t bl_lp_get_dtim_num(void)
{
    return iot2lp_para->wifi_parameter->dtim_num;
}
