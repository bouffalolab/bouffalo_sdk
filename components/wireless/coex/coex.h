#ifndef _COEX_H_
#define _COEX_H_

#include <stdio.h>
#include <stdint.h>

#define COEX_MODE_TDMA    1
#define COEX_MODE_PTI     2

#define COEX_NONE_NULL    1
#define COEX_NONE_RF_DOWN 2

#ifndef CONFIG_COEX_WIFI_MODE
#define CONFIG_COEX_WIFI_MODE 0
#endif

#ifndef CONFIG_COEX_THREAD_MODE
#define CONFIG_COEX_THREAD_MODE 0
#endif

#ifndef CONFIG_COEX_BT_MODE
#define CONFIG_COEX_BT_MODE 0
#endif

#if (CONFIG_COEX_WIFI_MODE == COEX_MODE_PTI) || (CONFIG_COEX_THREAD_MODE == COEX_MODE_PTI) || (CONFIG_COEX_BT_MODE == COEX_MODE_PTI)
#define CONFIG_COEX_PTI
#endif

#if (CONFIG_COEX_WIFI_MODE == COEX_MODE_TDMA) || (CONFIG_COEX_THREAD_MODE == COEX_MODE_TDMA) || (CONFIG_COEX_BT_MODE == COEX_MODE_TDMA)
#define CONFIG_COEX_TDMA
#ifndef CONFIG_COEX_TDMA_NONE
#define CONFIG_COEX_TDMA_NONE   COEX_NONE_NULL
#endif
#endif

// Wireless module role
enum coex_role {
    COEX_ROLE_BT = 0,
    COEX_ROLE_WIFI,
    COEX_ROLE_THREAD,
    COEX_ROLE_DUMMY,
    COEX_ROLE_MAX,
};

// Wireless module event type
enum coex_event {
    // Init Wireless module
    COEX_EVT_INIT = 0,
    // Deinit Wireless module
    COEX_EVT_DEINIT,
    // Set Wireless module activity
    COEX_EVT_SET_ACTIVITY,
    // Get Coex module activity
    COEX_EVT_GET_ACTIVITY,
    // Self event
    COEX_EVT_TMR_ISR_HANDLE,
    // Function call in Coex module
    COEX_EVT_FUNC_CALL,
    COEX_EVT_MAX,
};

// Wireless module event activity type
enum coex_event_activity {
    // BLE
    ACT_START_ADV,
 
    //BT
    ACT_BT_SCAN_START,
    ACT_BT_SCAN_DONE,
    ACT_BT_CONNECT_START,
    ACT_BT_CONNECT_DONE_OK,
    ACT_BT_CONNECT_DONE_FAIL,
    ACT_BT_DISCONNECT_START,
    ACT_BT_DISCONNECT_DONE,
    ACT_BT_A2DP_START,
    ACT_BT_A2DP_STOP,

    // Wi-Fi
    ACT_STA_SCAN_START,
    ACT_STA_SCAN_DONE,
    ACT_STA_CONNECT_START,
    ACT_STA_CONNECT_DONE_OK,
    ACT_STA_CONNECT_DONE_FAIL,
    ACT_STA_DISCONNECT_START,
    ACT_STA_DISCONNECT_DONE,
    ACT_STA_DPSM_START,
    ACT_STA_DPSM_YIELD,
    ACT_STA_DPSM_STOP,
    ACT_STA_ROC_REQ,
    ACT_STA_TBTT_UPDATE,
    ACT_SOFTAP_START,
    ACT_SOFTAP_STOP,
    ACT_SOFTAP_TBTT_UPDATE,

    // Thread
    ACT_START_PAN,
    ACT_STOP_PAN,

    // Dummy
    ACT_DUMMY_ADD_ACT,
    ACT_DUMMY_DEL_ACT,

    ACT_MAX,
};

// Callback for notify
struct coex_notify_args;
typedef void (*coex_notify_cb)(void* env, struct coex_notify_args* args);
typedef void (*coex_func_call)(void* args);

// Union of event args
union evt_arg
{
    // INIT
    struct {
        coex_notify_cb cb;
        void* env;
    } init;

    // DEINIT
    struct {
        int empty;
    } deinit;

    // Set activity
    struct {
        // Activity type
        int type;
        // Activity happen time
        int now;
    } set_act;

    // Get activity
    struct {
        int empty;
    } get_act;

    // Tmr ISR event
    struct {
        uint64_t time;
        void* env;
    } tmr_isr;

    // Function call event
    struct {
        coex_func_call func;
        int arg[4];
    } func_call;
};

// Wireless module event args
struct coex_evt_arg
{
    // Wireless module role
    int role;
    // Event type
    int type;
    // Union of event args
    union evt_arg arg;
};

// Coex module event
enum coex_notify {
    COEX_NTY_INITED = 0,
    COEX_NTY_DEINITED,
    COEX_NTY_RF_PRESENCE,
    COEX_NTY_RF_ABSENCE,
    COEX_NTY_MAX,
};

// Coex module notify args
struct coex_notify_args {
    int event;
    int duration;
};

// Coex return value
#define COEX_OK    (0)
#define COEX_FAIL  (-1)

int coex_init(void);
int coex_deinit(void);
int coex_event(struct coex_evt_arg* arg);
#endif // _COEX_H_

