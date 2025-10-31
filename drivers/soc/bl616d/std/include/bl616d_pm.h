#ifndef __BL616D_PM_H__
#define __BL616D_PM_H__

#include "bl616d_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum pm_pds_sleep_level {
    PM_PDS_LEVEL_1 = 1,
    PM_PDS_LEVEL_2 = 2,
    PM_PDS_LEVEL_3 = 3,
    PM_PDS_LEVEL_7 = 7,
    PM_PDS_LEVEL_15 = 15,
};

enum pm_hbn_sleep_level {
    PM_HBN_LEVEL_0,
    PM_HBN_LEVEL_1,
    PM_HBN_LEVEL_2,
};

enum pm_event_type {
    PM_HBN_WAKEUP_EVENT_NONE,
    PM_HBN_GPIO0_WAKEUP_EVENT,
    PM_HBN_GPIO1_WAKEUP_EVENT,
    PM_HBN_GPIO2_WAKEUP_EVENT,
    PM_HBN_GPIO3_WAKEUP_EVENT,
    PM_HBN_GPIO4_WAKEUP_EVENT,
    PM_HBN_GPIO5_WAKEUP_EVENT,
    PM_HBN_GPIO6_WAKEUP_EVENT,
    PM_HBN_GPIO7_WAKEUP_EVENT,
    PM_HBN_RTC_WAKEUP_EVENT,
    PM_HBN_BOD_WAKEUP_EVENT,
    PM_HBN_ACOMP0_WAKEUP_EVENT,
    PM_HBN_ACOMP1_WAKEUP_EVENT,
};

typedef struct {
    uint8_t lp_mode_en    : 1; /*!< Whether Enable LDO's or DCDC's lowpower Mode during PDS */
    uint8_t voltage_level : 4; /* LDO's or DCDC's voltage level during PDS */
    uint8_t rsv           : 3;
} PM_PWR_Cfg;

typedef struct
{
    uint8_t pdsLevel;         /*!< PDS level */
    PM_PWR_Cfg ldo18_aon_cfg; /*!< Power Config of ldo18_aon */
    PM_PWR_Cfg dcdc_sys_cfg;  /*!< Power Config of dcdc_sys */
    PM_PWR_Cfg ldo_soc_cfg;   /*!< Power Config of ldo_soc */
    uint8_t turnOffRF;        /*!< Wheather turn off RF */
    uint8_t powerDownFlash;   /*!< Whether power down flash */
    uint8_t ramRetEn;         /*!< Whether OCRAM Retention */
    uint8_t turnoffRC32M;     /*!< Whether trun off RC32M */
    uint8_t turnoffWifiPLL;   /*!< Whether trun off WiFi PLL */
    uint8_t turnoffCpuPLL;    /*!< Whether trun off CPU PLL */
    uint8_t ioKeepEn;         /*!< PDS io keep select */
    uint32_t sleepTime;       /*!< PDS sleep time */
    uint32_t *flashCfg;       /*!< Flash config pointer, used when power down flash */
    void (*preCbFun)(void);   /*!< Pre callback function */
    void (*postCbFun)(void);  /*!< Post callback function */
} PM_PDS_CFG_Type;

typedef struct {
    uint64_t io_ie;
    uint64_t io_pu;
    uint64_t io_pd;
    uint8_t io_0_7_trig_mode;
    uint8_t io_8_15_trig_mode;
    uint8_t io_16_23_trig_mode;
    uint8_t io_24_31_trig_mode;
    uint8_t io_32_39_trig_mode;
    uint8_t io_40_47_trig_mode;
    uint8_t io_48_52_trig_mode;
    uint64_t io_wakeup_unmask;
} lp_gpio_cfg_type;

void pm_pds_mask_all_wakeup_src(void);
BL_Err_Type pm_pds_wakeup_src_en(uint32_t WakeupType);
BL_Sts_Type pm_pds_get_wakeup_src(uint32_t WakeupType);
BL_Err_Type pm_set_gpio_pu_pd_ie(int pin, int pu, int pd, int ie);
BL_Err_Type pm_set_gpio_int_mask(int pin, int int_mask);
BL_Err_Type pm_set_gpio_trig_mode_int_mask(int pin, int trig_mode, int int_mask);
int pm_lowpower_gpio_cfg(lp_gpio_cfg_type *gpio_cfg);
char *pm_get_trig_mode_desc(int index);
void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);
void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint64_t sleep_time);
void pm_set_wakeup_callback(void (*wakeup_callback)(void));
void pm_set_boot2_app_jump_para(uint32_t para);
void pm_hbn_out0_irq_register(void);
void pm_hbn_out1_irq_register(void);
void HBN_OUT0_IRQ(void);
void HBN_OUT1_IRQ(void);
void pm_irq_callback(enum pm_event_type event);
void pm_pds_irq_register(void);
void PDS_WAKEUP_IRQ(void);

#ifdef __cplusplus
}
#endif

#endif