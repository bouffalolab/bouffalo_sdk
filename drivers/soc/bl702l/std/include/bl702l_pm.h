#ifndef __BL702L_PM_H__
#define __BL702L_PM_H__

#include "bl702l_common.h"

enum pm_pds_sleep_level {
    PM_PDS_LEVEL_31 = 31,
};

enum pm_hbn_sleep_level {
    PM_HBN_LEVEL_0,
    PM_HBN_LEVEL_1,
    PM_HBN_LEVEL_2,
};

enum pm_event_type {
    PM_HBN_WAKEUP_EVENT_NONE,
    PM_HBN_GPIO9_WAKEUP_EVENT,
    PM_HBN_GPIO10_WAKEUP_EVENT,
    PM_HBN_GPIO11_WAKEUP_EVENT,
    PM_HBN_GPIO12_WAKEUP_EVENT,
    PM_HBN_RTC_WAKEUP_EVENT,
    PM_HBN_BOR_WAKEUP_EVENT,
    PM_HBN_ACOMP0_WAKEUP_EVENT,
    PM_HBN_ACOMP1_WAKEUP_EVENT,
};

typedef struct
{
    uint8_t pdsLevel;        /*!< PDS level */
    uint8_t turnOffRF;       /*!< Wheather turn off RF */
    uint8_t powerDownFlash;  /*!< Whether power down flash */
    uint8_t ocramRetetion;   /*!< Whether OCRAM Retention */
    uint8_t turnoffPLL;      /*!< Whether trun off PLL */
    uint8_t turnoffDLL;      /*!< Whether trun off PLL */
    uint8_t flashContRead;   /*!< Whether enable flash continue read */
    uint8_t ioKeepSel;       /*!< PDS io keep select */
    uint8_t pdsLdoEn;        /*!< Whether enable PDS control LDO */
    uint8_t fastRecovery;    /*!< Whether enable fast recovery */
    uint32_t sleepTime;      /*!< PDS sleep time */
    uint32_t *flashCfg;      /*!< Flash config pointer, used when power down flash */
    uint32_t ldoLevel;       /*!< LDO level */
    void (*preCbFun)(void);  /*!< Pre callback function */
    void (*postCbFun)(void); /*!< Post callback function */
} PM_PDS_CFG_Type;

#ifdef __cplusplus
extern "C" {
#endif

BL_Err_Type pm_pds_wakeup_src_en(uint32_t WakeupType);
void pm_pds_enable(uint32_t *cfg);
void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);
void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint32_t sleep_time);
void pm_pds_io_wakeup_en(uint32_t pin,int trigMode, int pu, int pd);
void pm_hbn_io_wakeup_en(uint32_t pin,int trigMode, int pu, int pd);
BL_Err_Type pm_set_io_keep(uint32_t pin,uint32_t value);
uint32_t pm_acomp_wakeup_en(uint8_t acompNo, uint8_t pin, uint8_t pos_edge_en, uint8_t neg_edge_en);

#ifdef __cplusplus
}
#endif

#endif