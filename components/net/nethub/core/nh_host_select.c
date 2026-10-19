#include "nh_host_select.h"

#include "nh_endpoint.h"
#include "nh_internal.h"

#include "FreeRTOS.h"
#include "task.h"

#define DBG_TAG "NETHUB_HOST_SELECT"
#include "log.h"

typedef struct {
    nethub_host_select_state_t state;
    bool lock_in_progress;
} nh_host_select_ctx_t;

typedef struct {
    BaseType_t in_isr;
    UBaseType_t saved;
} nh_host_select_lock_t;

static nh_host_select_ctx_t g_host_select = {
    .state = NETHUB_HOST_SELECT_FIXED,
};

static nh_host_select_lock_t nethub_host_select_enter(void)
{
    nh_host_select_lock_t lock;

    lock.in_isr = xPortIsInsideInterrupt();
    if (lock.in_isr) {
        lock.saved = taskENTER_CRITICAL_FROM_ISR();
    } else {
        lock.saved = 0;
        taskENTER_CRITICAL();
    }

    return lock;
}

static void nethub_host_select_exit(nh_host_select_lock_t lock)
{
    if (lock.in_isr) {
        taskEXIT_CRITICAL_FROM_ISR(lock.saved);
    } else {
        taskEXIT_CRITICAL();
    }
}

static bool nethub_host_is_selectable(nethub_channel_t host)
{
    return host == NETHUB_CHANNEL_SDIO || host == NETHUB_CHANNEL_USB;
}

static nethub_host_select_state_t nethub_host_state_for_channel(nethub_channel_t host)
{
    switch (host) {
        case NETHUB_CHANNEL_SDIO:
            return NETHUB_HOST_SELECT_LOCKED_SDIO;
        case NETHUB_CHANNEL_USB:
            return NETHUB_HOST_SELECT_LOCKED_USB;
        default:
            return NETHUB_HOST_SELECT_LOCKED_NONE;
    }
}

static bool nethub_host_state_matches(nethub_host_select_state_t state, nethub_channel_t host)
{
    return (state == NETHUB_HOST_SELECT_LOCKED_SDIO && host == NETHUB_CHANNEL_SDIO) ||
           (state == NETHUB_HOST_SELECT_LOCKED_USB && host == NETHUB_CHANNEL_USB);
}

static nethub_channel_t nethub_host_channel_for_state(nethub_host_select_state_t state)
{
    switch (state) {
        case NETHUB_HOST_SELECT_LOCKED_SDIO:
            return NETHUB_CHANNEL_SDIO;
        case NETHUB_HOST_SELECT_LOCKED_USB:
            return NETHUB_CHANNEL_USB;
        default:
            return NETHUB_CHANNEL_MAX;
    }
}

static void nethub_host_select_start_locked(void)
{
    g_host_select.state = NETHUB_HOST_SELECT_PROBING;
    g_host_select.lock_in_progress = false;
}

static void nethub_host_select_log_probing(void)
{
    LOG_I("dual profile host select probing timeout=disabled\r\n");
}

bool nethub_host_selection_is_dual(void)
{
    return true;
}

void nethub_host_select_reset(void)
{
    nh_host_select_lock_t lock = nethub_host_select_enter();

    g_host_select.state = NETHUB_HOST_SELECT_INIT;
    g_host_select.lock_in_progress = false;

    nethub_host_select_exit(lock);
}

int nethub_host_select_start(void)
{
    nh_host_select_lock_t lock;

    lock = nethub_host_select_enter();
    if (g_host_select.state == NETHUB_HOST_SELECT_LOCKED_SDIO ||
        g_host_select.state == NETHUB_HOST_SELECT_LOCKED_USB) {
        nethub_host_select_exit(lock);
        return NETHUB_OK;
    }

    nethub_host_select_start_locked();
    nethub_host_select_exit(lock);

    nethub_host_select_log_probing();
    return NETHUB_OK;
}

bool nethub_host_report_candidate(nethub_channel_t host)
{
    nh_host_select_lock_t lock;
    bool accepted = false;
    bool need_lock = false;
    bool started = false;
    int ret;

    if (!nethub_host_is_selectable(host)) {
        return false;
    }

    lock = nethub_host_select_enter();
    if (g_host_select.state == NETHUB_HOST_SELECT_INIT) {
        nethub_host_select_start_locked();
        started = true;
    }

    if (g_host_select.lock_in_progress) {
        accepted = false;
    } else if (g_host_select.state == NETHUB_HOST_SELECT_PROBING) {
        g_host_select.lock_in_progress = true;
        accepted = true;
        need_lock = true;
    } else if (nethub_host_state_matches(g_host_select.state, host)) {
        accepted = true;
    }
    nethub_host_select_exit(lock);

    if (started) {
        nethub_host_select_log_probing();
    }

    if (!need_lock) {
        return accepted;
    }

    ret = nh_core_set_active_host_link(host);
    if (ret != NETHUB_OK) {
        lock = nethub_host_select_enter();
        g_host_select.state = NETHUB_HOST_SELECT_LOCKED_NONE;
        g_host_select.lock_in_progress = false;
        nethub_host_select_exit(lock);
        LOG_E("dual profile host lock %s failed: %d\r\n",
              nethub_channel_to_string(host), ret);
        return false;
    }

    lock = nethub_host_select_enter();
    g_host_select.state = nethub_host_state_for_channel(host);
    g_host_select.lock_in_progress = false;
    nethub_host_select_exit(lock);

    LOG_I("dual profile locked host=%s\r\n", nethub_channel_to_string(host));
    return true;
}

bool nethub_host_is_locked_active(nethub_channel_t host)
{
    nh_host_select_lock_t lock;
    bool locked;

    lock = nethub_host_select_enter();
    locked = nethub_host_state_matches(g_host_select.state, host);
    nethub_host_select_exit(lock);
    return locked;
}

nethub_channel_t nethub_host_selected(void)
{
    nh_host_select_lock_t lock;
    nethub_host_select_state_t state;

    lock = nethub_host_select_enter();
    state = g_host_select.state;
    nethub_host_select_exit(lock);
    return nethub_host_channel_for_state(state);
}

void nethub_host_select_get_status(nethub_host_select_status_t *status)
{
    nh_host_select_lock_t lock;

    if (status == NULL) {
        return;
    }

    status->dual_profile = true;
    lock = nethub_host_select_enter();

    status->state = g_host_select.state;
    status->selected_host = nethub_host_channel_for_state(g_host_select.state);
    nethub_host_select_exit(lock);
}

const char *nethub_host_select_state_name(nethub_host_select_state_t state)
{
    switch (state) {
        case NETHUB_HOST_SELECT_FIXED:
            return "fixed";
        case NETHUB_HOST_SELECT_INIT:
            return "init";
        case NETHUB_HOST_SELECT_PROBING:
            return "probing";
        case NETHUB_HOST_SELECT_LOCKED_SDIO:
            return "locked_sdio";
        case NETHUB_HOST_SELECT_LOCKED_USB:
            return "locked_usb";
        case NETHUB_HOST_SELECT_LOCKED_NONE:
            return "locked_none";
        default:
            return "unknown";
    }
}
