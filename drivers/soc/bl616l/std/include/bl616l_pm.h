#ifndef __BL616L_PM_H__
#define __BL616L_PM_H__

#include "bl616l_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t lp_mode_en    : 1; /*!< Whether Enable LDO's or DCDC's lowpower Mode during PDS */
    uint8_t voltage_level : 4; /* LDO's or DCDC's voltage level during PDS */
    uint8_t rsv           : 3;
} PM_PWR_Cfg;

typedef struct
{
    uint8_t pdsLevel;             /*!< PDS level */
    uint8_t turnOffRF;            /*!< Wheather turn off RF */
    uint8_t powerDownFlash;       /*!< Whether power down flash */
    uint8_t ramRetEn;             /*!< Whether OCRAM Retention */
    uint8_t turnoffRC32M;         /*!< Whether trun off RC32M */
    uint8_t turnoffXtal;          /*!< Whether trun off Xtal */
    uint8_t turnoffWifiPLL;       /*!< Whether trun off WiFi PLL */
    uint8_t pdsClkType;           /*!< pds_clk type */
    uint8_t pdsGpioDetClkType;    /*!< pds gpio det clk type */
    PM_PWR_Cfg ldo_sys_cfg;       /*!< Power Config of ldo_sys */
    PM_PWR_Cfg ldo_soc_cfg;       /*!< Power Config of ldo_soc */
    uint16_t ldo_sys_lp_en_cnt;   /*!< delay_cnt before ldo_sys enable lowpower mode */
    uint16_t ldo_sys_exit_lp_cnt; /*!< delay_cnt before ldo_sys exit lowpower mode */
    uint16_t ldo_soc_lp_en_cnt;   /*!< delay_cnt before ldo_soc enable lowpower mode */
    uint16_t ldo_soc_exit_lp_cnt; /*!< delay_cnt before ldo_soc exit lowpower mode */
    uint32_t sleepTime;           /*!< PDS sleep time */
    uint32_t *flashCfg;           /*!< Flash config pointer, used when power down flash */
    void (*preCbFun)(void);       /*!< Pre callback function */
    void (*postCbFun)(void);      /*!< Post callback function */
} PM_PDS_CFG_Type;

typedef struct {
    uint64_t io_ie;
    uint64_t io_pu;
    uint64_t io_pd;
    uint8_t io_0_5_trig_mode;
    uint8_t io_6_36_trig_mode[31];
    uint64_t io_wakeup_unmask;
} lp_gpio_cfg_Type;

/* PDS sleep level defines */
#define PM_PDS_LEVEL_1   (1)
#define PM_PDS_LEVEL_2   (2)
#define PM_PDS_LEVEL_3   (3)
#define PM_PDS_LEVEL_7   (7)
#define PM_PDS_LEVEL_11  (11)
#define PM_PDS_LEVEL_15  (15)

/* HBN sleep level defines */
#define PM_HBN_LEVEL_0   (0)
#define PM_HBN_LEVEL_1   (1)
#define PM_HBN_LEVEL_2   (2)

/* PM event type defines */
#define PM_WAKEUP_EVENT_NONE                  (0)
#define PM_HBN_GPIO0_WAKEUP_EVENT             (1)
#define PM_HBN_GPIO1_WAKEUP_EVENT             (2)
#define PM_HBN_GPIO2_WAKEUP_EVENT             (3)
#define PM_HBN_GPIO3_WAKEUP_EVENT             (4)
#define PM_HBN_GPIO4_WAKEUP_EVENT             (5)
#define PM_HBN_GPIO5_WAKEUP_EVENT             (6)
#define PM_HBN_RTC_WAKEUP_EVENT               (7)
#define PM_HBN_BOD_WAKEUP_EVENT               (8)

#define PM_PDS_WAKEUP_EVENT_MIN               (9)
#define PM_PDS_TIMER_WAKEUP_EVENT             (PM_PDS_WAKEUP_EVENT_MIN)  /* 9 */
#define PM_PDS_FROM_HBN_WAKEUP_EVENT          (10)
#define PM_PDS_GPIO_IRQ_WAKEUP_EVENT          (11)
#define PM_PDS_GPIO6_36_WAKEUP_EVENT          (12)
#define PM_PDS_TOUCH_WAKEUP_EVENT             (13)
#define PM_PDS_WIFI_WAKEUP_EVENT              (14)
#define PM_PDS_DM_SLP_WAKEUP_EVENT            (15)
#define PM_PDS_USBH_WKP_DET_WAKEUP_EVENT      (16)
#define PM_PDS_USBH_CONN_DET_WAKEUP_EVENT     (17)
#define PM_PDS_WIFI_TBTT_WAKEUP_EVENT         (18)
#define PM_PDS_WAKEUP_EVENT_MAX               (19)

#define PM_IO_TRIG_SYNC_FALLING_EDGE        (0x0) /*!< IO INT trigger type: sync falling edge trigger */
#define PM_IO_TRIG_SYNC_RISING_EDGE         (0x1) /*!< IO INT trigger type: sync rising edge trigger */
#define PM_IO_TRIG_SYNC_LOW_LEVEL           (0x2) /*!< IO INT trigger type: sync low level trigger */
#define PM_IO_TRIG_SYNC_HIGH_LEVEL          (0x3) /*!< IO INT trigger type: sync high level trigger */
#define PM_IO_TRIG_SYNC_RISING_FALLING_EDGE (0x4) /*!< IO INT trigger type: sync posedge negedge pulse trigger */
#define PM_IO_TRIG_ASYNC_FALLING_EDGE       (0x8) /*!< IO INT trigger type: sync falling edge trigger */
#define PM_IO_TRIG_ASYNC_RISING_EDGE        (0x9) /*!< IO INT trigger type: sync rising edge trigger */
#define PM_IO_TRIG_ASYNC_LOW_LEVEL          (0xA) /*!< IO INT trigger type: sync low level trigger */
#define PM_IO_TRIG_ASYNC_HIGH_LEVEL         (0xB) /*!< IO INT trigger type: sync high level trigger */
#define PM_PDS_IO_TRIG_NONE                 (0xF) /*!< IO INT trigger type: none */

#define PM_PDS_CLK_F32K                     (0x0) /* use f32k as pds clock */
#define PM_PDS_CLK_RC32M                    (0x1) /* use rc32m as pds clock */
#define PM_PDS_CLK_XTAL                     (0x2) /* use xtal as pds clock */
#define PM_PDS_CLK_XTAL_LP                  (0x3) /* use xtal_lp mode as pds clock */
#define PM_PDS_CLK_RC8M                     (0x4) /* use rc8m as pds clock */
#define PM_PDS_CLK_RC16M                    (0x5) /* use rc16m as pds clock */

BL_Err_Type pm_disable_gpio_keep(uint32_t pin);
BL_Err_Type pm_pds_wakeup_src_en(uint32_t WakeupType);
BL_Err_Type pm_pds_wakeup_src_disable(uint32_t WakeupType);
uint32_t pm_pds_get_all_wakeup_src(void);
BL_Sts_Type pm_pds_get_wakeup_src(uint32_t WakeupType);
BL_Err_Type pm_set_gpio_pu_pd_ie(int pin, int pu, int pd, int ie);
int pm_lowpower_gpio_cfg(lp_gpio_cfg_Type *gpio_cfg);
void pm_pds_mask_all_wakeup_src(void);
BL_Err_Type pm_set_gpio_int_mask(int pin, int int_mask);
BL_Err_Type pm_set_gpio_trig_mode_int_mask(int pin, int trig_mode, int int_mask);
void pm_pds_enable(uint32_t *cfg);
void pm_pds_mode_enter(uint32_t pds_level, uint32_t sleep_time);
void pm_hbn_mode_enter(uint32_t hbn_level, uint64_t sleep_time);
char *pm_get_trig_mode_desc(int index);
void pm_set_wakeup_callback(void (*wakeup_callback)(void));
void pm_set_boot2_app_jump_para(uint32_t para);
void pm_hbn_out0_irq_register(void);
void pm_hbn_out1_irq_register(void);
void HBN_OUT0_IRQ(void);
void HBN_OUT1_IRQ(void);
void pm_irq_callback(uint32_t event);
void pm_pds_irq_register(void);
void PDS_WAKEUP_IRQ(void);
#ifdef __cplusplus
}
#endif

#endif