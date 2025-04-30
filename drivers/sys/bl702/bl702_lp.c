#include <assert.h>
#include "hardware/timer_reg.h"
#include "ef_data_reg.h"
#include "sf_ctrl_reg.h"
#include "bl702_l1c.h"
#include "bl702_clock.h"
#include "bl702_romdriver.h"
#include "bl702_aon.h"
#include "bl702_pds.h"
#include "bl702_hbn.h"
#include "bl702_pm.h"
#include "bflb_core.h"
#include "bflb_rtc.h"
#include "bflb_sec_sha.h"
#include "bflb_acomp.h"
#include "bflb_flash.h"
#include "bflb_sflash.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"
#include "bflb_dma.h"
#include "bl_lp.h"
#include "bl_sys.h"
#include <arch/risc-v/e24/clic.h>
#include "tzc_sec_reg.h"


#define LP_HOOK(x, ...) if( &lp_hook_##x ) {lp_hook_##x(__VA_ARGS__);}

#define LP_FW_PRE_JUMP_ADDR   0x20010000

#define BL702_ACOMP_VREF_1V65 33

#define BL_LP_SOFT_INT_TRIG             (bflb_irq_set_pending(MSOFT_IRQn))
#define BL_LP_SOFT_INT_CLEAR            (bflb_irq_clear_pending(MSOFT_IRQn))


extern uint32_t __itcm_load_addr;
extern uint32_t __tcm_code_start__;
extern uint32_t __tcm_code_end__;

extern void bl_lp_debug_record_time(iot2lp_para_t *iot_lp_para, char *info_str);
extern void bl_lp_debug_clean_time(iot2lp_para_t *iot_lp_para);
extern void bl_lp_debug_dump_time(iot2lp_para_t *iot_lp_para);

bl_lp_fw_cfg_t lpfw_cfg = {
    .rtc_timeout_us = (1 * 1000 * 1000),
    .pds_timeout_us = (10 * 1000 * 1000),
};

/* EM Select, will get from glb register */
static uint8_t emSel = 0;

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

static struct lp_env *gp_lp_env = NULL;

// static void bl_lp_io_wakeup_init(bl_lp_io_cfg_t *io_wakeup_cfg);
static void bl_lp_acomp_wakeup_init(bl_lp_acomp_cfg_t *acomp_wakeup_cfg);

static bl_lp_io_cfg_t *gp_lp_io_cfg = NULL;
static bl_lp_io_cfg_t g_lp_io_cfg_bak;
static bl_lp_acomp_cfg_t *gp_lp_acomp_cfg = NULL;
static bl_lp_acomp_cfg_t g_lp_acomp_cfg_bak;
static void bl_lp_soft_irq(void);
static bl_lp_soft_irq_callback_t lp_soft_callback = { NULL };

#define GET_OFFSET(_type, _member) ((unsigned long)(&((_type *)0)->_member))

void ATTR_HBN_CODE_SECTION load_tcm_code(void)
{
    uint32_t *pSrc, *pDest;

    /* Copy ITCM code */
    pSrc = &__itcm_load_addr;
    pDest = &__tcm_code_start__;

    for (; pDest < &__tcm_code_end__;) {
        *pDest++ = *pSrc++;
    }
}

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

        /* restore tcm code */
        "call load_tcm_code\n\t"

        ".option push\n\t"
        ".option norelax\n\t"
        "la gp, __global_pointer$\n\t"
        ".option pop\n\t"

        "lw      x0, 0(a0)\n\t"
        "lw      x1, 4(a0)\n\t"
        "lw      x2, 8(a0)\n\t"
        // "lw      x3, 12(a0)\n\t"
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
        // "la      a0, freertos_risc_v_trap_handler\n\t" // mcu default_trap_handler
        "la      a0, freertos_risc_v_trap_handler\n\t"
        "ori     a0, a0, 3\n\t"
        "csrw    mtvec, a0\n\t"

        /* Intial the mtvt, MUST BE 64 bytes aligned*/
        ".weak __Vectors\n\t"
        "la t0, __Vectors\n\t"
        "csrw mtvt, t0\n\t"


        );
}

static void ATTR_TCM_SECTION lp_clock_init(void)
{
    GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_DLL144M);
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
}


__WEAK void ATTR_TCM_SECTION bl_lp_recovery_hardware(void)
{
    lp_clock_init();

    bl_lp_xip_recovery();
}

__attribute__((used)) void ATTR_HBN_CODE_SECTION lp_fw_pre(void)
{
    // Restore EM select
    BL_WR_REG(GLB_BASE, GLB_SEAM_MISC, emSel);

    /* Enable jtag */
    *((volatile uint32_t *)0x40000080) = 0x0001000C;

    /* clear HBN Rsv0 & Rsv1*/
    putreg32(0x0, 0x4000F100);
    putreg32(0x0, 0x4000F104);

    /* jump to lp_fw */
    void (*pFunc)(void);
    pFunc = (void (*)(void))bl_lp_recovery_hardware;
    pFunc();
}

void bl_lp_fw_init()
{
    /* clean iot2lp_para */
    memset(iot2lp_para, 0, (uint32_t)&iot2lp_para->reset_keep - (uint32_t)iot2lp_para);

#if (BL_LP_TIME_DEBUG)
    /* nocache ram */
    static lp_fw_time_debug_t time_debug_buff[TIME_DEBUG_NUM_MAX] = { 0 };
    iot2lp_para->time_debug = (void *)((uint32_t)time_debug_buff & 0x4FFFFFFF);
    bl_lp_debug_clean_time(iot2lp_para);
#endif

    uint32_t flash_cfg_len;
    bflb_flash_get_cfg((uint8_t **)&iot2lp_para->flash_cfg, &flash_cfg_len);

    iot2lp_para->flash_jdec_id=bflb_flash_get_jedec_id();
    //bflb_sflash_get_jedecid((spi_flash_cfg_type*)&iot2lp_para->flash_cfg, (uint8_t *)&iot2lp_para->flash_jdec_id);

    BL_LP_LOG("flash jdec_id 0x%08lX\r\n", (unsigned long)iot2lp_para->flash_jdec_id);

    bl_lp_xip_para_save();
}

__WEAK void bl_lp_io_init(uint64_t value)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    if(value & HBN_WAKEUP_GPIO_9) {
        bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_0);
    }
    if(value & HBN_WAKEUP_GPIO_10) {
        bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_0);
    }
    if(value & HBN_WAKEUP_GPIO_11) {
        bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_0);
    }
    if(value & HBN_WAKEUP_GPIO_12) {
        bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_0);
    }
}


static void bl_lp_io_wakeup_init(bl_lp_io_cfg_t *io_wakeup_cfg)
{
    uint64_t io_unmask = io_wakeup_cfg->io_wakeup_unmask;
    uint64_t ie;


    if (io_wakeup_cfg == NULL) {
        return;
    }

    HBN_Hw_Pu_Pd_Cfg(DISABLE);

    /* set ie& mask& inten of aon io 9~13 */
    ie = (io_wakeup_cfg->io_9_ie << 0)
        | (io_wakeup_cfg->io_10_ie << 1)
        | (io_wakeup_cfg->io_11_ie << 2)
        | (io_wakeup_cfg->io_12_ie << 3);
    bl_lp_io_init(ie);
    HBN_Aon_Pad_IeSmt_Cfg(ie&0x1F);
    if ((io_unmask >> 9) & 0x1f) {
        HBN_Pin_WakeUp_Mask((~(uint8_t)((io_unmask >> 9))&0x1f));
        HBN_GPIO_INT_Enable(io_wakeup_cfg->io_9_12_pds_trig_mode);
        PDS_IntEn(PDS_INT_HBN_IRQ_OUT0,ENABLE);
    } else {
        HBN_Aon_Pad_IeSmt_Cfg(0);
    }

}

int bl_lp_io_wakeup_cfg(bl_lp_io_cfg_t *io_wakeup_cfg)
{
    gp_lp_io_cfg = io_wakeup_cfg;
    return 0;
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

uint32_t bl_lp_set_acomp(uint8_t chan, uint8_t pin, uint8_t pos_edge_en, uint8_t neg_edge_en)
{
    struct bflb_acomp_config_s acompCfg = {
        .mux_en = ENABLE,                                      /*!< ACOMP mux enable */
        .pos_chan_sel = AON_ACOMP_CHAN_ADC0,                   /*!< ACOMP negtive channel select */
        .neg_chan_sel = AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1, /*!< ACOMP positive channel select */
        .vio_sel = BL702_ACOMP_VREF_1V65,                      /*!< ACOMP vref select */
        .scaling_factor =
            AON_ACOMP_SCALING_FACTOR_1,          /*!< ACOMP level select factor */
        .bias_prog = AON_ACOMP_BIAS_POWER_MODE3, /*!< ACOMP bias current control */
        .hysteresis_pos_volt =
            AON_ACOMP_HYSTERESIS_VOLT_NONE, /*!< ACOMP hysteresis voltage for positive */
        .hysteresis_neg_volt =
            AON_ACOMP_HYSTERESIS_VOLT_NONE, /*!< ACOMP hysteresis voltage for negtive */
    };


    uint32_t channel = 0;

    bflb_acomp_gpio_2_chanid(pin,(uint32_t*)&channel);
    acompCfg.pos_chan_sel =channel;

    /* Config Gpio as Analog */
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, pin, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);

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

    /* UnMask Hbn_Irq1 Wakeup PDS*/
    PDS_IntEn(PDS_INT_HBN_IRQ_OUT1,ENABLE);

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

int bl_lp_acomp_wakeup_cfg(bl_lp_acomp_cfg_t *acomp_wakeup_cfg)
{
    gp_lp_acomp_cfg = acomp_wakeup_cfg;
    return 0;
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


static void bl_lp_clear_wakeup_reason(void)
{
    /* clean wakeup reason */
    iot2lp_para->wakeup_reason = LPFW_WAKEUP_UNKOWN;
    /* clean wakeup bits */
    iot2lp_para->wake_io_bits = 0;
    iot2lp_para->wake_acomp_bits = 0;
    iot2lp_para->wake_io_edge_bits = 0;
    iot2lp_para->wake_acomp_edge_bits = 0;
}

static void bl_lp_clear_rtc_param(void)
{
    iot2lp_para->rtc_wakeup_en = 0;
    iot2lp_para->rtc_wakeup_cnt = 0;
}

static void rtc_wakeup_init(uint64_t rtc_wakeup_cmp_cnt, uint64_t sleep_us)
{
    uint64_t rtc_cnt;

    if (sleep_us == 0) {
        return;
    }

    /* Clear RTC Compare type */
    HBN_Clear_RTC_INT();

    if (rtc_wakeup_cmp_cnt == 0) {
        /* Get RTC Cnt  */
        uint32_t valLow = 0, valHigh = 0;
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        rtc_cnt = valLow + ((uint64_t)valHigh << 32);
        /* calculate RTC Comp cnt */
        rtc_cnt += BL_US_TO_PDS_CNT(sleep_us);
    } else {
        rtc_cnt = rtc_wakeup_cmp_cnt;
    }

    iot2lp_para->rtc_wakeup_cnt = rtc_cnt;
    iot2lp_para->rtc_wakeup_en = 1;

    /* Set RTC compare mode, and Set RTC Wakeup time */
    HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, rtc_cnt & 0xffffffff, rtc_cnt >> 32, HBN_RTC_COMP_BIT0_39);
    /* Enable rtc */
    HBN_Enable_RTC_Counter();
}

static void bl_lp_set_wakeup_reason(void)
{
    if (PDS_Get_IntStatus(PDS_INT_PDS_SLEEP_CNT)) {
        iot2lp_para->wakeup_reason |= LPFW_WAKEUP_PDSTIMER;
    } else if (PDS_Get_IntStatus(PDS_INT_HBN_IRQ_OUT0)) {
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO9)) {
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_IO;
            iot2lp_para->wake_io_bits |= (1<<9);
            HBN_Clear_IRQ(HBN_INT_GPIO9);
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO10)) {
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_IO;
            iot2lp_para->wake_io_bits |= (1<<10);
            HBN_Clear_IRQ(HBN_INT_GPIO10);
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO11)) {
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_IO;
            iot2lp_para->wake_io_bits |= (1<<11);
            HBN_Clear_IRQ(HBN_INT_GPIO11);
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO12)) {
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_IO;
            iot2lp_para->wake_io_bits |= (1<<12);
            HBN_Clear_IRQ(HBN_INT_GPIO12);
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO13)) {
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_IO;
            iot2lp_para->wake_io_bits |= (1<<13);
            HBN_Clear_IRQ(HBN_INT_GPIO13);
        }
        /* Get RTC Cnt  */
        uint64_t current_rtc_cnt;
        uint32_t valLow = 0, valHigh = 0;
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        current_rtc_cnt = valLow + ((uint64_t)valHigh << 32);
        if( current_rtc_cnt >= iot2lp_para->rtc_wakeup_cnt ){
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_RTC;
        }

    } else if (PDS_Get_IntStatus(PDS_INT_HBN_IRQ_OUT1)) {
        /* ACOMP0 */
        if (SET == HBN_Get_INT_State(HBN_INT_ACOMP0)) {
            iot2lp_para->wake_acomp_bits |= (1<<0);
            iot2lp_para->wake_acomp_edge_bits |= (bflb_acomp_get_result(AON_ACOMP0_ID)<<0);
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_ACOMP;
            HBN_Clear_IRQ(HBN_INT_ACOMP0);
        }
        /* ACOMP1 */
        if (SET == HBN_Get_INT_State(HBN_INT_ACOMP1)) {
            iot2lp_para->wake_acomp_bits |= (1<<1);
            iot2lp_para->wake_acomp_edge_bits |= (bflb_acomp_get_result(AON_ACOMP1_ID)<<1);
            iot2lp_para->wakeup_reason |= LPFW_WAKEUP_ACOMP;
            HBN_Clear_IRQ(HBN_INT_ACOMP1);
        }
    } else {
        iot2lp_para->wakeup_reason |= LPFW_WAKEUP_RTC;
    }
}

int bl_lp_fw_enter(bl_lp_fw_cfg_t *bl_lp_fw_cfg)
{
    uint64_t rtc_sleep_us;
    uint64_t pds_sleep_us;
    if (bl_lp_fw_cfg == NULL) {
        return -1;
    }

    rtc_sleep_us = bl_lp_fw_cfg->rtc_timeout_us;
    pds_sleep_us = bl_lp_fw_cfg->pds_timeout_us;

    if ((rtc_sleep_us == 0) & (pds_sleep_us == 0)) {
        /* no wake-up source */
        return -2;
    }

    bl_lp_debug_record_time(iot2lp_para, "bl_lp_fw_enter");

    bl_lp_clear_wakeup_reason();

    LP_HOOK(pre_user, bl_lp_fw_cfg);

    __disable_irq();

    LP_HOOK(pre_sys, bl_lp_fw_cfg);

    rtc_sleep_us = bl_lp_fw_cfg->rtc_timeout_us;
    pds_sleep_us = bl_lp_fw_cfg->pds_timeout_us;

    bl_lp_clear_rtc_param();

#if 0
    /* get glb gpio inturrpt status */
    uint64_t glb_io_int_stat = bl_lp_check_gpio_int();
    iot2lp_para->wake_io_bits = glb_io_int_stat;
    uint8_t acomp_int_stat = bl_lp_check_acomp_int();
    iot2lp_para->wake_acomp_bits = acomp_int_stat;
#endif

    // BL_LP_LOG("rtc_sleep_ms: %lld\r\n", rtc_sleep_us / (uint64_t)1000);
    // BL_LP_LOG("pds_sleep_ms: %lld\r\n", pds_sleep_us / (uint64_t)1000);

    /* Set lp_fw as the wake-up entry  */
    extern void lp_fw_pre(void);
    pm_set_wakeup_callback((void (*)(void))lp_fw_pre);

    pm_pds_mask_all_wakeup_src();
    HBN_Pin_WakeUp_Mask(0x1F);
    PDS_Set_Vddcore_GPIO_IntMask(MASK);
    PDS_Set_Vddcore_GPIO_IntClear();

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

    /* lpfw cfg: system para */
    iot2lp_para->mcu_sts = bl_lp_fw_cfg->mcu_sts;
    iot2lp_para->lpfw_wakeup_cnt = 0;
    iot2lp_para->pattern = 0xAA5555AA;
    iot2lp_para->wakeup_flag = 0;
    iot2lp_para->flash_offset = bflb_sf_ctrl_get_flash_image_offset(0, SF_CTRL_SEL_FLASH);
    iot2lp_para->app_entry = (uintptr_t)lp_fw_restore_cpu_para;
    iot2lp_para->args[0] = GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR;
    iot2lp_para->l1c_way_dis = (getreg32(0x40009000) >> 8) & 0xF;

    /* Minimum limit */
    if (pds_sleep_us == 0) {
        /* Mask PDS_Timer Wakeup */
    }else if (pds_sleep_us < (PDS_WAKEUP_MINI_LIMIT_US + PDS_WAKEUP_DELAY_US)) {
        pds_sleep_us = PDS_WAKEUP_MINI_LIMIT_US;
    } else {
        pds_sleep_us -= PDS_WAKEUP_DELAY_US;
    }

    if (rtc_sleep_us == 0) {
        /* Mask RTC Wakeup */
    }else if (rtc_sleep_us < (PDS_WAKEUP_MINI_LIMIT_US + PDS_WAKEUP_DELAY_US)) {
        rtc_sleep_us = PDS_WAKEUP_MINI_LIMIT_US;
    } else {
        rtc_sleep_us -= PDS_WAKEUP_DELAY_US;
    }

    LP_HOOK(pre_sleep, iot2lp_para);

    L1C_Cache_Flush();

    bl_lp_debug_record_time(iot2lp_para, "lp_fw_save_cpu_para");

    lp_fw_save_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);

    if (iot2lp_para->wakeup_flag == 0) {

        if (rtc_sleep_us) {
            rtc_wakeup_init(0, rtc_sleep_us);

            /* UnMask Hbn_Irq0 Wakeup PDS*/
            PDS_IntEn(PDS_INT_HBN_IRQ_OUT0,ENABLE);
            PDS_IntMask(PDS_INT_WAKEUP, UNMASK);
        } else {
            /* Clear RTC Compare type */
            HBN_Clear_RTC_INT();
        }

        // BL_LP_LOG("pds_sleep_ms: %lld\r\n", pds_sleep_us / (uint64_t)1000);
        // BL_LP_LOG("rtc_wakeup_cnt: %lld\r\n", iot2lp_para->rtc_wakeup_cnt);

        /* pds15 enter */
        pm_pds_mode_enter(PM_PDS_LEVEL_31, BL_US_TO_PDS_CNT(pds_sleep_us));

        lp_fw_pre();
        iot2lp_para->wakeup_flag = 1;
        lp_fw_restore_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);
    }

    /* disable wdt */
    uint32_t regval = 0;

    putreg16(0xBABA, 0x4000A500 + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, 0x4000A500 + TIMER_WSAR_OFFSET);

    regval = getreg32(0x4000A500 + TIMER_WMER_OFFSET);
    regval &= ~TIMER_WE;
    putreg32(regval, 0x4000A500 + TIMER_WMER_OFFSET);

    bl_lp_debug_record_time(iot2lp_para, "return APP");

    L1C_Cache_Enable_Set(iot2lp_para->l1c_way_dis);

#ifdef CONF_PSRAM_RESTORE
    board_psram_x8_init();
#endif

    bl_lp_set_wakeup_reason();

    LP_HOOK(post_sys, iot2lp_para);

    PDS_IntClear();

    bl_lp_fw_cfg->wakeup_reason = iot2lp_para->wakeup_reason;

    /* register */
    bflb_irq_attach(MSOFT_IRQn, (irq_callback)bl_lp_soft_irq, NULL);
    /* trig soft int */
    BL_LP_SOFT_INT_TRIG;

#if 0
    BL_LP_LOG("--app wakeup reason %ld\r\n", bl_lp_fw_cfg->wakeup_reason);
    BL_LP_LOG("wake_io_bits 0x%016llx\r\n", iot2lp_para->wake_io_bits);
    BL_LP_LOG("wake_acomp_bits 0x%x\r\n", iot2lp_para->wake_acomp_bits);
    BL_LP_LOG("wake_io_edge_bits 0x%016llx\r\n", iot2lp_para->wake_io_edge_bits);
    BL_LP_LOG("wake_acomp_edge_bits 0x%x\r\n", iot2lp_para->wake_acomp_edge_bits);
#endif

#if (BL_LP_TIME_DEBUG)
    bl_lp_debug_dump_time(iot2lp_para);
    bl_lp_debug_clean_time(iot2lp_para);
#endif

    return iot2lp_para->wakeup_reason;
}


int bl_lp_init(void)
{
    assert(!gp_lp_env);

    emSel = BL_RD_REG(GLB_BASE, GLB_SEAM_MISC);

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

static void bl_lp_soft_irq(void)
{
    uint64_t wakeup_io_bits = iot2lp_para->wake_io_bits;
    uint32_t wakeup_acmp_bits = iot2lp_para->wake_acomp_bits;

    /* disable soft int */
    bflb_irq_disable(MSOFT_IRQn);
    /* clear soft int */
    BL_LP_SOFT_INT_CLEAR;

    /* exit user */
    LP_HOOK(post_user, iot2lp_para);

    if (wakeup_io_bits && lp_soft_callback.wakeup_io_callback) {
        lp_soft_callback.wakeup_io_callback(wakeup_io_bits);
    }

    if (wakeup_acmp_bits && lp_soft_callback.wakeup_acomp_callback) {
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

int bl_lp_get_wake_reason(void)
{
    return (int)iot2lp_para->wakeup_reason;
}

int bl_lp_get_wake_io_state(void)
{
    return (int)gp_lp_env->gpio_stat;
}
