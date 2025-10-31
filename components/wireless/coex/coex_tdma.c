
#include "coex.h"

#if defined(CONFIG_COEX_TDMA)
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <shell.h>

#include <utils_list.h>
#include <bflb_timer.h>

#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

static TickType_t _get_systick(void)
{
    BaseType_t isInISR = xPortIsInsideInterrupt();

    if (isInISR) {
        return xTaskGetTickCountFromISR();
    } else {
        return xTaskGetTickCount();
    }
}

#define COEX_CLI(fmt, ...)   printf("[%d][Coex][Cli] "fmt"\r\n", _get_systick(), ##__VA_ARGS__)
#define COEX_DBG(fmt, ...)   //printf("[%d][Coex][Info] %s "fmt"\r\n", _get_systick(), __FUNCTION__, ##__VA_ARGS__)
#define COEX_INFO(fmt, ...)  //printf("[%d][Coex][Info] %s "fmt"\r\n", _get_systick(), __FUNCTION__, ##__VA_ARGS__)
#define COEX_WARN(fmt, ...)  //printf("[%d][Coex][Warn] %s "fmt"\r\n", _get_systick(), __FUNCTION__, ##__VA_ARGS__)
#define COEX_ERROR(fmt, ...) printf("[%d][Coex][Error] %s "fmt"\r\n", _get_systick(), __FUNCTION__, ##__VA_ARGS__)

// Default TDMA Parameter
#define COEX_TIME_FOREVER    0xFFFFFFFF
#define INIT_CMD_QUEUE_CNT   6
#define INIT_DUTY_CYCLE      102400     // in us
#define INIT_CTXT_CREDITS    6          // credits used for adjust duration
#define MIN_CTXT_DURATION    15000      // in us

// RF context state
enum rf_ctxt_state {
    RF_CTXT_UNUSED = 0,
    RF_CTXT_INITED,
    RF_CTXT_SCHED,
};

// RF context for wireless module
struct rf_ctxt_tag {
    // List header for chaining
    struct utils_list_hdr list_hdr;
    // State of RF context
    uint32_t state;

    // Role of RF context
    uint32_t role;
    // Index of RF context
    uint32_t index;
    // Activity level of RF context
    uint32_t activity_level;
    // Credits of RF context
    uint32_t credits;
    // Duration of RF context, in us
    uint32_t duration;
    // Fixed duration of RF context, in us
    uint32_t fixed_duration;
    // Fixed times, under COEX_TDMA_SCHED
    uint32_t fixed_times;
    // Single exclusive state
    uint32_t single_exclusive;
    // Event callback under timer callback
    coex_notify_cb cb;
    // Pointer to be passed to the callback
    void *env;
};

// Coex tdma status
enum coex_tdma_state {
    COEX_TDMA_UNUSED = 0,
    COEX_TDMA_INITED,
    COEX_TDMA_SCHED,
    COEX_TDMA_MAX
};

// Coex module internal environment
struct coex_env_tag {
    // List of onoging rf_ctxt
    struct utils_list sched_ctxt;
    // Coex diable
    volatile uint32_t disable;
    // Coex event message queue
    void* evt_mq;                 // for msg queue 6 *
    // Current state
    uint32_t state;               // UNUSED  INITED   SCHED
    // Current duty cycle, in us
    uint32_t duty_cycle;
    // Current init RF context count
    uint32_t rf_ctxt_init_cnt;
    // Current sched RF context count
    uint32_t rf_ctxt_sched_cnt;
    // Current tdma paused
    uint32_t paused;
    // Current RF on/off
    uint32_t rf_on;
    // Current rf contxt
    struct rf_ctxt_tag* cur_ctxt;
    // Fixed rf contxt
    struct rf_ctxt_tag* fixed_ctxt;
    // Fixed times
    uint32_t fixed_times;
    // Coex timer, for tdma op and rf_ctxt cb
    struct bflb_device_s *tmr;       // hardware  timer
    // Coex timer cfg
    struct bflb_timer_config_s cfg;
    // Coex timer callback env
    void *env;
    // Coex timer valid msg time
    uint64_t valid_isr_msg_time;
};

// Coex tdma activity operation
enum coex_activity_op {
    DECREASE_ACTIVITY = 0,
    REMAIN_ACTIVITY,
    INCREASE_ACTIVITY,
};

// Activity to determine tdma policy
struct coex_tdma_act_pol {
    // Activity used to add/del duty cycle
    int act_op;
    // Credits used to adjust time duty cycle
    int credits_offset;
    // Single exclusive before add into duty cycle
    int single_exclusive;
    // Force sync used to calibrate duty cycle
    int force_sync;
    // Yield on rf when ctxt is current ctxt
    int yield;
    // fix ctxt
    int fixed;
};

// Coex tdma ctxt opration
enum coex_tdma_ctxt_op {
    CTXT_NONE = 0,
    CTXT_ADD,
    CTXT_UPD,
    CTXT_DEL,
    CTXT_UNK,
};

typedef int (*coex_event_tdma_func)(int role, void* arg);

static TaskHandle_t coex_task_handle;
static struct rf_ctxt_tag rf_ctxt[COEX_ROLE_MAX];
static struct coex_env_tag coex_env;
static struct coex_tdma_act_pol coex_tdma_act_pol_tab[ACT_MAX] = {
    // BLE
    [ACT_START_ADV]             = {INCREASE_ACTIVITY, +3, 0, 0, 0, 0},

    // BT
    [ACT_BT_SCAN_START]         = {INCREASE_ACTIVITY, +3, 1, 0, 0, 0},
    [ACT_BT_SCAN_DONE]          = {INCREASE_ACTIVITY, -3, 1, 0, 0, 0},
    [ACT_BT_CONNECT_START]      = {INCREASE_ACTIVITY, +3, 1, 0, 0, 0},
    [ACT_BT_CONNECT_DONE_OK]    = {INCREASE_ACTIVITY, -3, 1, 0, 0, 0},
    [ACT_BT_CONNECT_DONE_FAIL]  = {INCREASE_ACTIVITY, -3, 1, 0, 0, 0},
    [ACT_BT_DISCONNECT_START]   = {INCREASE_ACTIVITY,  0, 1, 0, 0, 0},
    [ACT_BT_DISCONNECT_DONE]    = {INCREASE_ACTIVITY,  0, 1, 0, 0, 0},
    [ACT_BT_A2DP_START]         = {INCREASE_ACTIVITY, +3, 1, 0, 0, 0},
    [ACT_BT_A2DP_STOP]          = {INCREASE_ACTIVITY, -3, 1, 0, 0, 0},

    // Wi-Fi
    [ACT_STA_SCAN_START]        = {INCREASE_ACTIVITY, +3, 1, 0, 0, 0},
    [ACT_STA_SCAN_DONE]         = {DECREASE_ACTIVITY, -3, 0, 0, 0, 0},
    [ACT_STA_CONNECT_START]     = {INCREASE_ACTIVITY, +3, 1, 0, 0, 0},
    [ACT_STA_CONNECT_DONE_OK]   = {REMAIN_ACTIVITY,   -3, 0, 0, 0, 0},
    [ACT_STA_CONNECT_DONE_FAIL] = {DECREASE_ACTIVITY, -3, 0, 0, 0, 0},
    [ACT_STA_DISCONNECT_START]  = {REMAIN_ACTIVITY,   +0, 0, 0, 0, 0},
    [ACT_STA_DISCONNECT_DONE]   = {DECREASE_ACTIVITY, -0, 0, 0, 0, 0},
    [ACT_STA_DPSM_START]        = {REMAIN_ACTIVITY,   -3, 0, 0, 0, 0},
    [ACT_STA_DPSM_YIELD]        = {REMAIN_ACTIVITY,   -0, 0, 0, 1, 0},
    [ACT_STA_DPSM_STOP]         = {REMAIN_ACTIVITY,   +3, 0, 0, 0, 0},
    [ACT_STA_ROC_REQ]           = {REMAIN_ACTIVITY,   +0, 0, 0, 0, 1},
    [ACT_STA_TBTT_UPDATE]       = {REMAIN_ACTIVITY,   +0, 0, 1, 0, 0},
    [ACT_SOFTAP_START]          = {INCREASE_ACTIVITY, +0, 1, 0, 0, 0},
    [ACT_SOFTAP_STOP]           = {DECREASE_ACTIVITY, -0, 0, 0, 0, 0},
    [ACT_SOFTAP_TBTT_UPDATE]    = {REMAIN_ACTIVITY,   -0, 0, 1, 0, 0},

    // Thread
    [ACT_START_PAN]             = {INCREASE_ACTIVITY, 0, 1, 0, 0, 0},
    [ACT_STOP_PAN]              = {DECREASE_ACTIVITY, 0, 1, 0, 0, 0},

    // Dummy
    [ACT_DUMMY_ADD_ACT]         = {INCREASE_ACTIVITY,  0, 0, 0, 0, 0},
    [ACT_DUMMY_DEL_ACT]         = {DECREASE_ACTIVITY,  0, 0, 0, 0, 0},
};

static uint32_t coex_tdma_tmr_yield_time(struct rf_ctxt_tag* ctxt)
{
    uint32_t yield = ctxt->duration - bflb_timer_get_countervalue(coex_env.tmr);

    return (yield > ctxt->duration) ? 0 : yield;
}

static void coex_tdma_tmr_isr(int irq, void *arg)
{
    bool status = bflb_timer_get_compint_status(coex_env.tmr, TIMER_COMP_ID_0);

    if (status) {
        // Clear interrupt
        bflb_timer_compint_clear(coex_env.tmr, TIMER_COMP_ID_0);

        // Check validity
        uint64_t now = bflb_mtimer_get_time_us();
        if (coex_env.disable || !arg || now < coex_env.valid_isr_msg_time)
        {
            return;
        }

        // Send tmr evt msg to coex module
        struct coex_evt_arg evt = {
            .role = 0,
            .type = COEX_EVT_TMR_ISR_HANDLE,
            .arg.tmr_isr.time = now,
            .arg.tmr_isr.env = arg,
        };
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        int ret = xQueueSendFromISR(coex_env.evt_mq, &evt, &xHigherPriorityTaskWoken);
        if (pdPASS != ret) {
            COEX_ERROR("send err: %d", ret);
        }
    }
}

static void coex_tdma_tmr_init(void)
{
    // Default timer config
    coex_env.cfg.counter_mode    = TIMER_COUNTER_MODE_UP;      /* up when match occur */
    coex_env.cfg.clock_source    = TIMER_CLKSRC_XTAL;          /* timer clk = XCLK/(div + 1 )*/
    coex_env.cfg.clock_div       = 39;                         /* for bl616/bl808/bl606p is 39, for bl702 is 31 */
    coex_env.cfg.trigger_comp_id = TIMER_COMP_ID_0;
    coex_env.cfg.comp0_val       = 0xFFFFFFFF;                 /* match value 0 */
    coex_env.cfg.comp1_val       = 0xFFFFFFFF;                 /* match value 1 */
    coex_env.cfg.comp2_val       = 0xFFFFFFFF;                 /* match value 2 */
    coex_env.cfg.preload_val     = 0;                          /* preload value */

    // Get timer
    coex_env.tmr = bflb_device_get_by_name("timer0");
}

static void coex_tdma_tmr_start(struct rf_ctxt_tag* ctxt, uint32_t yield_time)
{
    coex_env.cfg.comp0_val = ctxt->duration + yield_time;

    bflb_timer_init(coex_env.tmr, &coex_env.cfg);
    bflb_irq_attach(coex_env.tmr->irq_num, coex_tdma_tmr_isr, ctxt);
    bflb_irq_enable(coex_env.tmr->irq_num);

    bflb_timer_start(coex_env.tmr);

    COEX_INFO("duration: %d", ctxt->duration + yield_time);
}

static void coex_tdma_tmr_stop(void)
{
    bflb_irq_disable(coex_env.tmr->irq_num);
    bflb_timer_stop(coex_env.tmr);
}

static void coex_tdma_ctxt_rf_alloc(int role)
{
    #if defined(BL616)
    #define PTR_CONFIG_ADDR  0x24920004
    #define PTR_CONFIG2_ADDR 0x24920028
    #endif

    #if (CONFIG_COEX_TDMA_NONE == COEX_NONE_RF_DOWN)
    // Whether RF on/off
    if (COEX_ROLE_MAX == role)
    {
        COEX_INFO("Off");
        // TODO, rf off
        coex_env.rf_on = 0;

        return;
    }

    if (!coex_env.rf_on)
    {
        // TODO, rf on
        coex_env.rf_on = 1;
    }
    #endif

    // PTA force
    switch (role)
    {
    case COEX_ROLE_BT:
        COEX_INFO("bt");
        #if defined(BL616)
        *(volatile uint32_t*)(PTR_CONFIG_ADDR)  = 0x501CC01E;
        *(volatile uint32_t*)(PTR_CONFIG2_ADDR) = 0x00030000;
        #endif
        break;
    case COEX_ROLE_WIFI:
        COEX_INFO("wifi");
        // Abort BT/154, Allow WiFi
        #if defined(BL616)
        *(volatile uint32_t*)(PTR_CONFIG_ADDR)  = 0x5013c01e;
        *(volatile uint32_t*)(PTR_CONFIG2_ADDR) = 0x000f0000;
        #endif
        break;
    case COEX_ROLE_THREAD:
        COEX_INFO("thread");
        // Abort WiFi/BT， Allow 154
        #if defined(BL616)
        *(volatile uint32_t*)(PTR_CONFIG_ADDR)  = 0x501f001e;
        *(volatile uint32_t*)(PTR_CONFIG2_ADDR) = 0x00030000;
        #endif
        break;
    case COEX_ROLE_DUMMY:
        COEX_INFO("dummy");
        // Abort WiFi/BT/154
        #if defined(BL616)
        *(volatile uint32_t*)(PTR_CONFIG_ADDR)  = 0x501fc01e;
        *(volatile uint32_t*)(PTR_CONFIG2_ADDR) = 0x000f0000;
        #endif
        break;
    default:
        break;
    }
}

static struct rf_ctxt_tag * coex_tdma_get_fixed_dur_ctxt(void)
{
    struct utils_list_hdr *ele;
    struct rf_ctxt_tag *ctxt;

    ele = utils_list_pick(&coex_env.sched_ctxt);
    while (ele)
    {
        ctxt = (struct rf_ctxt_tag *)ele;
        if (ctxt->fixed_times)
        {
            return ctxt;
        }
        ele = utils_list_next(ele);
    }

    return NULL;
}

static void coex_tdma_ctxt_distribute(void)
{
    uint32_t duty_cycle_rem = coex_env.duty_cycle;
    uint32_t sched_cnt_rem = coex_env.rf_ctxt_sched_cnt;
    struct utils_list_hdr *ele;
    struct rf_ctxt_tag *ctxt;
    int total_credits = 0;

    ele = utils_list_pick(&coex_env.sched_ctxt);
    while (ele)
    {
        ctxt = (struct rf_ctxt_tag *)ele;
        if (!ctxt->fixed_times)
        {
            total_credits += ctxt->credits;
        }
        else
        {
            sched_cnt_rem--;
        }
        ele = utils_list_next(ele);
    }

    ele = utils_list_pick(&coex_env.sched_ctxt);
    while (ele)
    {
        ctxt = (struct rf_ctxt_tag *)ele;
        if (ctxt->fixed_times)
        {
            ctxt->duration = ctxt->fixed_duration;
        }
        else if (sched_cnt_rem == 1)
        {
            ctxt->duration = duty_cycle_rem;
            sched_cnt_rem --;
        }
        else
        {
            ctxt->duration = coex_env.duty_cycle * ctxt->credits / total_credits;
            sched_cnt_rem --;
        }
        duty_cycle_rem -= ctxt->duration;
        ele = utils_list_next(ele);
    }
}

static inline struct rf_ctxt_tag* coex_tdma_ctxt_next(struct rf_ctxt_tag* ctxt)
{
    return (struct rf_ctxt_tag *)((ctxt->list_hdr.next) ? (ctxt->list_hdr.next) : (coex_env.sched_ctxt.first));
}

static void coex_tdma_ctxt_switch(struct rf_ctxt_tag* ctxt, int start_timer, uint32_t yield_time)
{
    COEX_INFO("ctxt: %p, duration: %d, yield_time: %d", ctxt, ctxt ? ctxt->duration : 0, yield_time);

    struct coex_notify_args notify;
    int ctxt_switched = 0;

    if (!ctxt)
    {
        #if (CONFIG_COEX_TDMA_NONE == COEX_NONE_RF_DOWN)
        // Notify wireless module absence
        if (coex_env.cur_ctxt)
        {
            notify.event = COEX_NTY_RF_ABSENCE;
            notify.duration = 0;
            coex_env.cur_ctxt->cb(coex_env.cur_ctxt->env, &notify);
        }

        coex_env.cur_ctxt = NULL;
        coex_tdma_ctxt_rf_alloc(COEX_ROLE_MAX);
        #endif

        return;
    }

    if (coex_env.rf_ctxt_sched_cnt == 1)
    {
        coex_env.cur_ctxt = ctxt;
        coex_tdma_ctxt_rf_alloc(ctxt->role);

        // Notify wireless module presence
        notify.event = COEX_NTY_RF_PRESENCE;
        notify.duration = 0xFFFFFFFF;
        if (ctxt->cb)
        {
            ctxt->cb(ctxt->env, &notify);
        }
        return;
    }

    if (coex_env.paused)
    {
        COEX_WARN("switch rf_ctxt while paused!");
        return;
    }

    // Whether current ctxt is this ctxt
    if (coex_env.fixed_ctxt && coex_env.fixed_times)
    {
        // Notify wireless module absence
        if (coex_env.cur_ctxt && coex_env.cur_ctxt != coex_env.fixed_ctxt)
        {
            notify.event = COEX_NTY_RF_ABSENCE;
            notify.duration = 0;
            coex_env.cur_ctxt->cb(coex_env.cur_ctxt->env, &notify);
            ctxt_switched = 1;
        }

        // Fix ctxt
        ctxt = coex_env.fixed_ctxt;
        coex_env.cur_ctxt = ctxt;
        coex_env.fixed_times--;
    }
    else if (ctxt != coex_env.cur_ctxt)
    {
        // Notify wireless module absence
        if (coex_env.cur_ctxt)
        {
            notify.event = COEX_NTY_RF_ABSENCE;
            notify.duration = 0;
            coex_env.cur_ctxt->cb(coex_env.cur_ctxt->env, &notify);
        }

        // Update cur_ctxt
        coex_env.cur_ctxt = ctxt;
        ctxt_switched = 1;
    }

    // Update ISR valid timer
    coex_env.valid_isr_msg_time = bflb_mtimer_get_time_us();

    // Restart timer
    if (start_timer)
    {
        coex_tdma_tmr_stop();
        // COEX_CLI("start timer");
        coex_tdma_tmr_start(ctxt, yield_time);
    }

    if (ctxt_switched)
    {
        if (ctxt->fixed_times)
        {
            ctxt->fixed_times--;
        }

        // RF alloc
        coex_tdma_ctxt_rf_alloc(ctxt->role);

        // Notify wireless module presence
        notify.event = COEX_NTY_RF_PRESENCE;
        notify.duration = ctxt->duration + yield_time;
        if (ctxt->cb)
        {
            ctxt->cb(ctxt->env, &notify);
        }
    }
}

static void coex_tdma_ctxt_add(struct rf_ctxt_tag* ctxt, struct coex_tdma_act_pol* policy)
{
    COEX_INFO("credits_offset: %d, credits: %d", policy->credits_offset, ctxt->credits);

    // Ctxt update
    ctxt->state = RF_CTXT_SCHED;
    ctxt->credits += policy->credits_offset;
    ctxt->single_exclusive = policy->single_exclusive;

    // Coex_env update
    utils_list_push_back(&coex_env.sched_ctxt, &ctxt->list_hdr);
    coex_env.rf_ctxt_sched_cnt++;
    coex_env.state = COEX_TDMA_SCHED;

    // First ctxt
    if (coex_env.rf_ctxt_sched_cnt == 1)
    {
        // Switch to this ctxt, no need to start timer
        coex_tdma_ctxt_switch(ctxt, 0, 0);
        // Set single exlcusive
        coex_env.paused = ctxt->single_exclusive;
        return;
    }

    // Distribute duration for rf_ctxt
    coex_tdma_ctxt_distribute();

    // Already whether paused
    if (!coex_env.paused)
    {
        // Check whether second ctxt to add or sync 
        if (coex_env.rf_ctxt_sched_cnt == 2 || policy->force_sync)
        {
            coex_tdma_ctxt_switch(ctxt, 1, 0);
        }
    }
}

static void coex_tdma_ctxt_upd(struct rf_ctxt_tag* ctxt, struct coex_tdma_act_pol* policy)
{
    int old_exclusive = ctxt->single_exclusive;
    int pause_stopped = 0;

    // Ctxt update
    ctxt->single_exclusive = policy->single_exclusive;
    ctxt->credits += policy->credits_offset;

    // Whether pause stopped
    if (coex_env.paused && old_exclusive &&
        !ctxt->single_exclusive && coex_env.cur_ctxt == ctxt)
    {
        pause_stopped = 1;
        coex_env.paused = 0;
    }

    // Whether fix ctxt
    if (coex_env.rf_ctxt_sched_cnt > 1 && policy->fixed)
    {
        coex_env.fixed_ctxt = ctxt;
        coex_env.fixed_times = 1;
        return;
    }

    // Whether yield
    if (policy->yield)
    {
        // Not this ctxt, just return
        if (coex_env.cur_ctxt != ctxt)
        {
            return;
        }

        if (coex_env.rf_ctxt_sched_cnt == 1)
        {
            // Yeild in this ctxt, and no ctxt switch
            coex_tdma_ctxt_switch(NULL, 0, 0);
        }
        else
        {
            // Yeild in this ctxt and calculate yield time
            coex_tdma_ctxt_switch(coex_tdma_ctxt_next(ctxt), 1, coex_tdma_tmr_yield_time(ctxt));
            // Distribute duration for rf_ctxt
            coex_tdma_ctxt_distribute();
        }
        return;
    }

    // Only one sched ctxt, cur_ctxt must be ctxt, no need switch
    if (coex_env.rf_ctxt_sched_cnt == 1)
    {
        // Dont care pause issue, check whether sync
        if (policy->force_sync)
        {
            coex_tdma_ctxt_switch(ctxt, 0, 0);
        }
        return;
    }

    // Distribute duration for rf_ctxt
    coex_tdma_ctxt_distribute();

    // Stop pause, immediately switch to next ctxt
    if (pause_stopped)
    {
        // No need to consider sync issue
        coex_tdma_ctxt_switch(coex_tdma_ctxt_next(ctxt), 1, 0);
        return;
    }

    // No pause issue, check whether sync
    if (policy->force_sync)
    {
        coex_tdma_ctxt_switch(ctxt, 1, 0);
    }
}

static void coex_tdma_ctxt_del(struct rf_ctxt_tag* ctxt, struct coex_tdma_act_pol* policy)
{
    struct rf_ctxt_tag* next_ctxt;
    struct coex_notify_args notify;

    // Ctxt update
    ctxt->state = RF_CTXT_INITED;
    ctxt->fixed_times = 0;
    if (policy)
    {
        ctxt->credits += policy->credits_offset;
        ctxt->single_exclusive = policy->single_exclusive;
        COEX_INFO("credits_offset: %d, credits: %d", policy->credits_offset, ctxt->credits);
    }

    // Only one sched ctxt
    if (coex_env.rf_ctxt_sched_cnt == 1)
    {
        // Only one ctxt, must be current ctxt
        if (coex_env.cur_ctxt != ctxt)
        {
            COEX_ERROR("cur_ctxt is not this ctxt while only one ctxt!");
            return;
        }

        // Notify deinited
        notify.event = COEX_NTY_RF_ABSENCE;
        notify.duration = 0;
        ctxt->cb(ctxt->env, &notify);

        // First stop timer
        coex_tdma_tmr_stop();

        #if (CONFIG_COEX_TDMA_NONE == COEX_NONE_RF_DOWN)
        coex_tdma_ctxt_rf_alloc(COEX_ROLE_MAX);
        #else
        // Do nothing
        #endif

        // Update coex_env
        utils_list_extract(&coex_env.sched_ctxt, &ctxt->list_hdr);
        coex_env.rf_ctxt_sched_cnt--;
        coex_env.state = COEX_TDMA_INITED;
        coex_env.duty_cycle = INIT_DUTY_CYCLE;
        coex_env.cur_ctxt = NULL;
        coex_env.paused = 0;
        return;
    }

    // Check if cur_ctxt is this ctxt
    next_ctxt = coex_tdma_ctxt_next(ctxt);
    utils_list_extract(&coex_env.sched_ctxt, &ctxt->list_hdr);
    coex_env.rf_ctxt_sched_cnt--;
    // Distribute duration for rf_ctxt
    coex_tdma_ctxt_distribute();

    // Reset fixed_ctxt
    if (coex_env.fixed_ctxt == ctxt)
    {
        coex_env.fixed_ctxt = NULL;
        coex_env.fixed_times = 0;
    }

    if (coex_env.cur_ctxt == ctxt)
    {
        // Always clear pause
        coex_env.paused = 0;
        coex_tdma_ctxt_switch(next_ctxt, (coex_env.rf_ctxt_sched_cnt == 1) ? 0 : 1, 0);
    }
}

static int coex_tdma_init_ctxt(int role, void* arg)
{
    // Check and init rf_ctxt
    struct rf_ctxt_tag* ctxt = &rf_ctxt[role];
    union evt_arg* param = (union evt_arg*)arg;
    struct coex_notify_args notify;

    // Check and init rf ctxt
    if (ctxt->state != RF_CTXT_UNUSED)
    {
        COEX_WARN("rf ctxt init again!");
        return COEX_FAIL;
    }

    if (!param->init.cb)
    {
        COEX_WARN("rf ctxt cb is NULL!");
        return COEX_FAIL;
    }

    memset(ctxt, 0, sizeof(struct rf_ctxt_tag));
    ctxt->state   = RF_CTXT_INITED;
    ctxt->role    = role;
    ctxt->index   = role;
    ctxt->credits = INIT_CTXT_CREDITS;
    ctxt->cb      = param->init.cb;
    ctxt->env     = param->init.env;

    // Init ok
    coex_env.rf_ctxt_init_cnt++;
    // Notify inited
    notify.event = COEX_NTY_INITED;
    notify.duration = 0;
    ctxt->cb(ctxt->env, &notify);

    return COEX_OK;
}

static int coex_tdma_deinit_ctxt(int role, void* arg)
{
    struct rf_ctxt_tag* ctxt = &rf_ctxt[role];
    (void)arg;
    struct coex_notify_args notify;

    // Check validity
    if (coex_env.state == COEX_TDMA_UNUSED)
    {
        return COEX_FAIL;
    }

    // Deinit ctxt
    switch (ctxt->state)
    {
    case RF_CTXT_SCHED:
        // TODO: should directly notify deinited?
        coex_tdma_ctxt_del(ctxt, NULL);
        break;
    case RF_CTXT_INITED:
        // Update ctxt
        ctxt->state = RF_CTXT_UNUSED;
        ctxt->credits = 0;

        // Update coex_env
        coex_env.rf_ctxt_init_cnt--;
        break;
    default:
        break;
    }

    // Notify deinited
    notify.event = COEX_NTY_DEINITED;
    notify.duration = 0;
    ctxt->cb(ctxt->env, &notify);

    return COEX_OK;
}

static int coex_tdma_flush_ctxt(void)
{
    struct coex_notify_args notify;

    for (int i = 0; i < COEX_ROLE_MAX; i++)
    {
        if (rf_ctxt[i].state >= RF_CTXT_INITED)
        {
            // Notify deinited
            notify.event = COEX_NTY_DEINITED;
            notify.duration = 0;
            rf_ctxt[i].cb(rf_ctxt[i].env, &notify);
        }
    }

    return COEX_OK;
}

static int coex_tdma_activity_check(struct rf_ctxt_tag* ctxt, struct coex_tdma_act_pol* policy)
{
    int activity_level = ctxt->activity_level;
    int credits = ctxt->credits;

    switch (ctxt->state)
    {
    case RF_CTXT_UNUSED:
        return COEX_FAIL;
    case RF_CTXT_INITED:
        if (activity_level || !credits || policy->act_op == DECREASE_ACTIVITY ||
            !coex_env.rf_ctxt_init_cnt)
        {
            return COEX_FAIL;
        }
        break;
    case RF_CTXT_SCHED:
        if (!activity_level || !credits || coex_env.state <= COEX_TDMA_INITED ||
            utils_list_is_empty(&coex_env.sched_ctxt) || !coex_env.rf_ctxt_sched_cnt)
        {
            return COEX_FAIL;
        }
        break;
    default:
        return COEX_FAIL;
    }

    return COEX_OK;
}

static int coex_tdma_activity_decide(struct rf_ctxt_tag* ctxt, struct coex_tdma_act_pol* policy)
{
    int old_act_lvl;
    int decision = CTXT_NONE;

    // Update rf context
    old_act_lvl = ctxt->activity_level;
    ctxt->activity_level += (policy->act_op - 1);

    // Decide
    switch (ctxt->state)
    {
    case RF_CTXT_INITED:
        if (ctxt->activity_level <= old_act_lvl)
        {
            break;
        }

        decision = CTXT_ADD;
        break;
    case RF_CTXT_SCHED:
        if (!ctxt->activity_level)
        {
            decision = CTXT_DEL;
            break;
        }

        decision = CTXT_UPD;
        break;
    default:
        break;
    }

    return decision;
}

static char *_coex_decision_str(int decision)
{
    const *str[] = {
        "NONE",
        "ADD ",
        "UPD ",
        "DEL ",
    };

    if (decision >= CTXT_UNK) {
        return str[CTXT_UNK];
    }

    return str[decision];
}

static int coex_tdma_activity(int role, void* arg)
{
    COEX_DBG("");

    struct rf_ctxt_tag* ctxt = &rf_ctxt[role];
    union evt_arg* param = (union evt_arg*)arg;
    struct coex_tdma_act_pol* policy;
    int type = param->set_act.type;

    if (type > ACT_MAX)
    {
        return COEX_FAIL;
    }

    // Get policy
    policy = &coex_tdma_act_pol_tab[type];

    // Check validity
    if (COEX_OK != coex_tdma_activity_check(ctxt, policy))
    {
        COEX_ERROR("rf ctxt wrong op, role: %d, level: %d, credits: %d, op: %d, ctxt state: %d, coex state: %d",
                role, ctxt->activity_level, ctxt->credits, policy->act_op, ctxt->state, coex_env.state);
        return COEX_FAIL;
    }

    // Decide to how to process ctxt
    int decision = coex_tdma_activity_decide(ctxt, policy);
    COEX_INFO("decision: %s", _coex_decision_str(decision));

    switch (decision)
    {
    case CTXT_ADD:
        coex_tdma_ctxt_add(ctxt, policy);
        break;
    case CTXT_UPD:
        coex_tdma_ctxt_upd(ctxt, policy);
        break;
    case CTXT_DEL:
        coex_tdma_ctxt_del(ctxt, policy);
        break;
    default:
        break;
    }

    return COEX_OK;
}

static int coex_tdma_tmr_isr_handle(int role, void* arg)
{
    COEX_DBG("");

    union evt_arg* param = (union evt_arg*)arg;
    struct rf_ctxt_tag* ctxt = (struct rf_ctxt_tag*)param->tmr_isr.env;
    (void)role;

    // Check validity
    if (param->tmr_isr.time < coex_env.valid_isr_msg_time ||
        !ctxt || coex_env.rf_ctxt_sched_cnt == 1)
    {
        // Drop
        COEX_WARN("Drop invalid tmr isr evt!");
        return COEX_OK;
    }

    // Switch to next ctxt
    coex_tdma_ctxt_switch(coex_tdma_ctxt_next(ctxt), 1, 0);

    return COEX_OK;
}

static void coex_tdma_func_call_ctxt_fix(void* arg)
{
    int role = ((int*)arg)[0];
    int times = ((int*)arg)[1];

    struct rf_ctxt_tag* ctxt = &rf_ctxt[role];

    // Check validity
    if (RF_CTXT_SCHED != ctxt->state ||
        (coex_env.fixed_ctxt && coex_env.fixed_ctxt != ctxt) ||
        coex_tdma_get_fixed_dur_ctxt())
    {
        COEX_CLI("Unvalid fix ctxt condition!");
        return;
    }

    coex_env.fixed_ctxt = &rf_ctxt[role];
    coex_env.fixed_times = times;
}

static void coex_tdma_func_call_duration_fix(void* arg)
{
    int role     = ((int*)arg)[0];
    int duration = ((int*)arg)[1];
    int times    = ((int*)arg)[2];

    struct rf_ctxt_tag* ctxt = &rf_ctxt[role];
    struct rf_ctxt_tag* fixed_dur_ctxt = coex_tdma_get_fixed_dur_ctxt();

    // Check validity
    if (RF_CTXT_SCHED != ctxt->state || coex_env.fixed_ctxt ||
        (fixed_dur_ctxt && fixed_dur_ctxt != ctxt))
    {
        COEX_CLI("Unvalid fix duration condition!");
        return;
    }

    COEX_CLI("ctxt: %p, role: %d, duration: %d, times: %d",
                ctxt, role, duration, times);
    if (duration < MIN_CTXT_DURATION ||
        duration > (coex_env.duty_cycle - MIN_CTXT_DURATION*coex_env.rf_ctxt_sched_cnt))
    {
        COEX_CLI("Fixed duration is not allowed!");
        return;
    }

    ctxt->fixed_duration = duration;
    ctxt->fixed_times = times;
}

static int coex_tdma_func_call(int rroleole, void* arg)
{
    union evt_arg* param = (union evt_arg*)arg;
    if (param->func_call.func)
    {
        param->func_call.func(param->func_call.arg);
    }

    return COEX_OK;
}

static void coex_tdma_task(void *arg)
{
    struct coex_evt_arg evt;

    // Init Coex_env
    memset(&coex_env, 0, sizeof(coex_env));
    coex_env.evt_mq = xQueueCreate(INIT_CMD_QUEUE_CNT, sizeof(struct coex_evt_arg));
    if (!coex_env.evt_mq)
    {
        COEX_WARN("Init evt_mq failed!");
        goto end;
    }

    utils_list_init(&coex_env.sched_ctxt);
    coex_env.state = COEX_TDMA_INITED;
    coex_env.duty_cycle = INIT_DUTY_CYCLE;
    coex_tdma_tmr_init();

    // Process evt
    while (!coex_env.disable) {
        if (pdPASS == xQueueReceive(coex_env.evt_mq, &evt, COEX_TIME_FOREVER)) {
            int ret = 0;

            switch (evt.type) {
                case COEX_EVT_INIT:
                    COEX_INFO("----------------------------> COEX_EVT_INIT=%d", evt.type);
                    ret = coex_tdma_init_ctxt(evt.role, &evt.arg);
                    break;
                case COEX_EVT_DEINIT:
                    COEX_INFO("----------------------------> COEX_EVT_DEINIT=%d", evt.type);
                    ret = coex_tdma_deinit_ctxt(evt.role, &evt.arg);
                    break;
                case COEX_EVT_SET_ACTIVITY:
                    COEX_INFO("----------------------------> COEX_EVT_SET_ACTIVITY=%d", evt.type);
                    ret = coex_tdma_activity(evt.role, &evt.arg);
                    break;
                case COEX_EVT_TMR_ISR_HANDLE:
                    COEX_INFO("----------------------------> COEX_EVT_TMR_ISR_HANDLE=%d", evt.type);
                    ret = coex_tdma_tmr_isr_handle(evt.role, &evt.arg);
                    break;
                case COEX_EVT_FUNC_CALL:
                    COEX_INFO("----------------------------> COEX_EVT_FUNC_CALL=%d", evt.type);
                    ret = coex_tdma_func_call(evt.role, &evt.arg);
                    break;
                default:
                    break;
            }

            if (ret)
            {
                COEX_ERROR("Proecess fail, ret: %d, role: %d, type: %d", ret, evt.role, evt.type);
            }
        }
    }

    // Post-process, first stop timer
    coex_tdma_tmr_stop();
    // Notify deinited all
    coex_tdma_flush_ctxt();

    #if (CONFIG_COEX_TDMA_NONE == COEX_NONE_RF_DOWN)
    coex_tdma_ctxt_rf_alloc(COEX_ROLE_MAX);
    #else
    // Do nothing
    #endif

    // Clean
    vQueueDelete(coex_env.evt_mq);
    memset(&coex_env, 0, sizeof(coex_env));

end:
    vTaskDelete(NULL);
}

static char *_coex_nty_role_str(uint32_t role)
{
    const *str[] = {
        "BT    ",
        "WIFI  ",
        "THREAD",
        "DUMMY ",
        "UNKNOW",
    };

    if (role >= COEX_NTY_MAX) {
        return str[COEX_NTY_MAX];
    }

    return str[role];
}

static char *_coex_state_str(uint32_t state)
{
    const *str[] = {
        "UNUSED",
        "INITED",
        "SCHED ",
        "UNKNOW",
    };

    if (state >= COEX_TDMA_MAX) {
        return str[COEX_TDMA_MAX];
    }

    return str[state];
}

static void coex_tdma_ctxt_dump(int state)
{
    for (int i = 0; i < COEX_ROLE_MAX; i++) {
        if (rf_ctxt[i].state != state)
        {
            continue;
        }
        COEX_CLI("     role:%d(%s), act_lvl: %u, credits:%u, dur:%u, fix_dur:%d, times:%d, exclusive:%u",
                 rf_ctxt[i].role, _coex_nty_role_str(rf_ctxt[i].role), rf_ctxt[i].activity_level, rf_ctxt[i].credits, rf_ctxt[i].duration,
                 rf_ctxt[i].fixed_duration, rf_ctxt[i].fixed_times, rf_ctxt[i].single_exclusive);
    }
}

static void coex_tdma_info_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (coex_env.disable || coex_env.state == COEX_TDMA_UNUSED)
    {
        COEX_ERROR("Coex not enable!");
        return;
    }

    COEX_CLI("Coex info:");
    COEX_CLI("     state:%s, paused:%u, duty_cycle:%u, cur_ctxt:%p, fixed_ctxt:%p, fixed_times:%d, valid_time:%u",
             _coex_state_str(coex_env.state), coex_env.paused, coex_env.duty_cycle, coex_env.cur_ctxt,
             coex_env.fixed_ctxt, coex_env.fixed_times, coex_env.valid_isr_msg_time);
    COEX_CLI("Ctxt info:");
    COEX_CLI("init_cnt (%d):", coex_env.rf_ctxt_init_cnt-coex_env.rf_ctxt_sched_cnt);
    coex_tdma_ctxt_dump(RF_CTXT_INITED);
    COEX_CLI("sched_cnt (%d):", coex_env.rf_ctxt_sched_cnt);
    coex_tdma_ctxt_dump(RF_CTXT_SCHED);
}

static void coex_tdma_ctxt_fix(int argc, char **argv)
{
    if (argc != 3) {
        goto err;
    }

    int role = atoi(argv[1]);
    int times = atoi(argv[2]);

    // Send tmr evt msg to coex module
    struct coex_evt_arg evt = {
        .role = 0,
        .type = COEX_EVT_FUNC_CALL,
        .arg.func_call.func = coex_tdma_func_call_ctxt_fix,
        .arg.func_call.arg[0] = role,
        .arg.func_call.arg[1] = times,
    };

    xQueueSend(coex_env.evt_mq, &evt, COEX_TIME_FOREVER);
    return;
err:
    COEX_CLI("usage: coex_ctx_fix 0/1/2/3 1/0 (fix ctxt bt/wifi/thread/dummy enable/disbale)");
}

static void coex_tdma_duration_fix(int argc, char **argv)
{
    if (argc != 4) {
        goto err;
    }

    int role = atoi(argv[1]);
    int duration = atoi(argv[2]);
    int times = atoi(argv[3]);

    // Send tmr evt msg to coex module
    struct coex_evt_arg evt = {
        .role = 0,
        .type = COEX_EVT_FUNC_CALL,
        .arg.func_call.func = coex_tdma_func_call_duration_fix,
        .arg.func_call.arg[0] = role,
        .arg.func_call.arg[1] = duration*1000,
        .arg.func_call.arg[2] = times,
    };

    xQueueSend(coex_env.evt_mq, &evt, COEX_TIME_FOREVER);
    return;
err:
    COEX_CLI("usage: coex_dur_fix 0/1/2/3(bt/wifi/thread/dummy) duration(ms) times(duty_cyle)");
}

static void coexist_dummy_ctxt_notify_cb(void *env, struct coex_notify_args *args)
{
    COEX_DBG("dummy ctxt notify cb, event: %d", args->event);
}

static void coex_tdma_ctxt_dummy_enable(int argc, char **argv)
{
    if (argc != 2) {
        goto err;
    }

    int enable = atoi(argv[1]);
    if (enable)
    {
        // Dummy ctxt init
        struct coex_evt_arg init_arg = {
            .role             = COEX_ROLE_DUMMY,
            .type             = COEX_EVT_INIT,
            .arg.init.cb      = coexist_dummy_ctxt_notify_cb,
            .arg.init.env     = NULL,
        };
        xQueueSend(coex_env.evt_mq, &init_arg, COEX_TIME_FOREVER);

        // Dummy ctxt add
        struct coex_evt_arg add_arg = {
            .role             = COEX_ROLE_DUMMY,
            .type             = COEX_EVT_SET_ACTIVITY,
            .arg.set_act.type = ACT_DUMMY_ADD_ACT,
        };
        xQueueSend(coex_env.evt_mq, &add_arg, COEX_TIME_FOREVER);
    }
    else
    {
        // Dummy ctxt deinit
        struct coex_evt_arg deinit_arg = {
            .role             = COEX_ROLE_DUMMY,
            .type             = COEX_EVT_DEINIT,
        };
        xQueueSend(coex_env.evt_mq, &deinit_arg, COEX_TIME_FOREVER);
    }

    return;
err:
    COEX_CLI("usage: coex_ctxt_dummy 1/0 (add/del dummy ctxt)");
}

#if defined(CONFIG_SHELL)
SHELL_CMD_EXPORT_ALIAS(coex_tdma_info_status, coex_status, coex module info status);
SHELL_CMD_EXPORT_ALIAS(coex_tdma_ctxt_fix, coex_fix_ctxt, coex module fix ctxt);
SHELL_CMD_EXPORT_ALIAS(coex_tdma_duration_fix, coex_fix_dur, coex module fix duration);
SHELL_CMD_EXPORT_ALIAS(coex_tdma_ctxt_dummy_enable, coex_dummy, coex module add/del dummy ctxt);
#endif

int coex_init_tdma(void)
{
    if (pdPASS != xTaskCreate(coex_tdma_task, "Coex", 512, NULL, 31, &coex_task_handle))
    {
        return COEX_FAIL;
    }

    return COEX_OK;
}

int coex_deinit_tdma(void)
{
    coex_env.disable = 1;
    return COEX_OK;
}

int coex_event_tdma(struct coex_evt_arg* arg)
{
    if (!arg || !coex_task_handle || coex_env.disable)
    {
        return COEX_FAIL;
    }

#if 0
    struct coex_evt_arg* param = (struct coex_evt_arg*)arg;
    if (COEX_EVT_SET_ACTIVITY == param->type)
    {
        param->arg.set_act.now = bflb_mtimer_get_time_us();
    }
#endif

    return ((pdTRUE == xQueueSend(coex_env.evt_mq, arg, COEX_TIME_FOREVER)) ? COEX_OK : COEX_FAIL);
}
#endif // defined(CONFIG_COEX_TDMA)
