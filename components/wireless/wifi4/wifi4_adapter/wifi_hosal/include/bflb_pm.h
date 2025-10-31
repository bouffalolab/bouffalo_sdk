#ifndef __BL_PM_H__
#define __BL_PM_H__
#include <stdint.h>
#include <utils_list.h>

#define    NODE_CAP_BIT_UAPSD_MODE        (1 << 0)
#define    NODE_CAP_BIT_MAC_IDLE          (1 << 1)
#define    NODE_CAP_BIT_MAC_DOZE          (1 << 2)
#define    NODE_CAP_BIT_RF_ONOFF          (1 << 3)
#define    NODE_CAP_BIT_WLAN_BLE_ABORT    (1 << 4)
#define    NODE_CAP_BIT_FORCE_SLEEP       (1 << 5)
#define    NODE_CAP_BIT_ALL_ALLOWED       (0xffff)

enum PM_EVENT_ABLE {
    PM_DISABLE = 0,
    PM_ENABLE,
};

enum WLAN_CODE_SLEEP_CONTROL {
    WLAN_CODE_PM_NOTIFY_START = 0,
    WLAN_CODE_PM_NOTIFY_STOP,
    WLAN_CODE_PM_START,
    WLAN_CODE_PM_STOP,
};

enum WLAN_CODE_ENTER_SLEEP {
    WLAN_CODE_PM_ENTER_SLEEP_PRE = 0,
    WLAN_CODE_PM_ENTER_SLEEP,
};

enum WLAN_CODE_EXIT_SLEEP {
    WLAN_CODE_PM_ENTER_SLEEP_PRE_FAILED = 0,
    WLAN_CODE_PM_EXIT_SLEEP_PRE,
    WLAN_CODE_PM_EXIT_SLEEP,
};

enum WLAN_CODE_BEACON_LOSS {
    WLAN_CODE_BEACON_LOSS = 0,
};

enum WLAN_CODE_SEND_NULLDATA {
   WLAN_CODE_PM_PAUSE = 0,
   WLAN_CODE_PM_NULLDATA_NOACK,
   WLAN_CODE_PM_NULLDATA_SEND_ERROR,
};

enum BLE_CODE_BLE_CONTROL {
    BLE_CODE_PM_TURNON_RF = 0,
    BLE_CODE_PM_TURNOFF_RF,
};

enum PM_LEVEL{
    PM_MODE_STA_NONE = 0,
    PM_MODE_STA_IDLE,
    PM_MODE_STA_MESH,
    PM_MODE_STA_DOZE,
    PM_MODE_STA_COEX,
    PM_MODE_STA_DOWN,
    PM_MODE_AP_IDLE,
    PM_MODE_MAX,
};

enum PM_EVEMT{
    WLAN_PM_EVENT_CONTROL = 0,
    WLAN_PM_EVENT_ENTER_SLEEP,
    WLAN_PM_EVENT_EXIT_SLEEP,
    WLAN_PM_EVENT_BEACON_LOSS,
    WLAN_PM_EVENT_SEND_NULLDATA,
    BLE_PM_EVENT_CONTROL,
    PM_EVENT_MAX,
};

typedef int (*bl_pm_cb_t)(void *arg);

int bl_pm_init(void);
int bl_pm_event_register(enum PM_EVEMT event, uint32_t code, uint32_t cap_bit, uint16_t pirority, bl_pm_cb_t ops, void *arg, enum PM_EVENT_ABLE enable);
int bl_pm_deinit(void);
int bl_pm_state_run(void);
int bl_pm_capacity_set(enum PM_LEVEL level);
int pm_post_event(enum PM_EVEMT event, uint32_t code, uint32_t *retval);
int bl_pm_event_switch(enum PM_EVEMT event, uint32_t code, enum PM_EVENT_ABLE enable);
#endif
