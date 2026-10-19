/**
 * @file bflbwifid_runtime.c
 * @brief Shared runtime state and monitor loop for bflbwifid
 */

#include "bflbwifid_runtime.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/bflbwifi_log.h"
#include "../../virtualchan/nethub_vchan.h"
#include "bflbwifid_log.h"

typedef struct {
    pthread_t thread;
    pthread_mutex_t lock;
    bool started;
    bflbwifid_state_t state;
    bflbwifi_ctrl_status_t ctrl_status;
} daemon_runtime_t;

static daemon_runtime_t g_runtime = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .state = BFLBWIFID_STATE_STARTING,
};

static const volatile sig_atomic_t *g_running_flag;

static bflbwifid_state_t runtime_state_from_ctrl_status(const bflbwifi_ctrl_status_t *status)
{
    if (!status) {
        return BFLBWIFID_STATE_STARTING;
    }

    if (status->link_state == BFLBWIFI_CTRL_LINK_CONNECTED) {
        return BFLBWIFID_STATE_READY;
    }

    return BFLBWIFID_STATE_RECONNECTING;
}

static void runtime_set_state(bflbwifid_state_t state)
{
    pthread_mutex_lock(&g_runtime.lock);
    g_runtime.state = state;
    pthread_mutex_unlock(&g_runtime.lock);
}

static void *runtime_monitor_thread(void *arg)
{
    (void)arg;

    while (g_running_flag && *g_running_flag) {
        if (bflbwifid_refresh_runtime_state(true) != 0) {
            BFLBWIFID_LOGE("Failed to refresh runtime state");
        }
        usleep(500000);
    }

    runtime_set_state(BFLBWIFID_STATE_STOPPING);
    return NULL;
}

const char *bflbwifid_state_name(bflbwifid_state_t state)
{
    switch (state) {
        case BFLBWIFID_STATE_STARTING:
            return "STARTING";
        case BFLBWIFID_STATE_READY:
            return "READY";
        case BFLBWIFID_STATE_RECONNECTING:
            return "RECONNECTING";
        case BFLBWIFID_STATE_STOPPING:
            return "STOPPING";
        default:
            return "UNKNOWN";
    }
}

int bflbwifid_get_vchan_host_state_line(char *buf, size_t len, uint8_t ctrl_backend)
{
    nethub_vchan_state_snapshot_t snapshot;

    if (!buf || len == 0) {
        return -1;
    }

    buf[0] = '\0';

    if (ctrl_backend != BFLBWIFI_CTRL_BACKEND_VCHAN) {
        return 0;
    }

    if (nethub_vchan_get_state_snapshot(&snapshot) != 0) {
        return -1;
    }

    snprintf(buf, len, "VchanHostState: %s\n", nethub_vchan_host_state_name(snapshot.host_state));
    return 0;
}

int bflbwifid_refresh_runtime_state(bool log_changes)
{
    bflbwifi_ctrl_status_t ctrl_status;
    nethub_vchan_state_snapshot_t vchan_snapshot;
    bflbwifid_state_t old_state;
    bflbwifid_state_t new_state;
    bflbwifi_ctrl_link_state_t old_link;
    bool old_ota;
    int ret;

    ret = bflbwifi_get_ctrl_status(&ctrl_status);
    if (ret != 0) {
        return ret;
    }

    new_state = runtime_state_from_ctrl_status(&ctrl_status);

    pthread_mutex_lock(&g_runtime.lock);
    old_state = g_runtime.state;
    old_link = g_runtime.ctrl_status.link_state;
    old_ota = g_runtime.ctrl_status.ota_in_progress;
    g_runtime.ctrl_status = ctrl_status;
    g_runtime.state = new_state;
    pthread_mutex_unlock(&g_runtime.lock);

    if (log_changes &&
        (old_state != new_state ||
         old_link != ctrl_status.link_state ||
         old_ota != ctrl_status.ota_in_progress)) {
        if (ctrl_status.ctrl_backend == BFLBWIFI_CTRL_BACKEND_VCHAN &&
            nethub_vchan_get_state_snapshot(&vchan_snapshot) == 0) {
            BFLBWIFID_LOGI("Runtime state: daemon=%s backend=%s link=%s vchan_host=%s ota=%s",
                           bflbwifid_state_name(new_state),
                           bflbwifi_ctrl_backend_name(ctrl_status.ctrl_backend),
                           bflbwifi_ctrl_link_state_name(ctrl_status.link_state),
                           nethub_vchan_host_state_name(vchan_snapshot.host_state),
                           ctrl_status.ota_in_progress ? "YES" : "NO");
        } else {
            BFLBWIFID_LOGI("Runtime state: daemon=%s backend=%s link=%s ota=%s",
                           bflbwifid_state_name(new_state),
                           bflbwifi_ctrl_backend_name(ctrl_status.ctrl_backend),
                           bflbwifi_ctrl_link_state_name(ctrl_status.link_state),
                           ctrl_status.ota_in_progress ? "YES" : "NO");
        }
    }

    return 0;
}

void bflbwifid_get_runtime_snapshot(bflbwifid_state_t *state, bflbwifi_ctrl_status_t *ctrl_status)
{
    pthread_mutex_lock(&g_runtime.lock);
    if (state) {
        *state = g_runtime.state;
    }
    if (ctrl_status) {
        *ctrl_status = g_runtime.ctrl_status;
    }
    pthread_mutex_unlock(&g_runtime.lock);
}

int bflbwifid_runtime_start(const volatile sig_atomic_t *running_flag)
{
    int ret;

    if (!running_flag) {
        return EINVAL;
    }

    if (g_runtime.started) {
        return 0;
    }

    g_running_flag = running_flag;
    ret = pthread_create(&g_runtime.thread, NULL, runtime_monitor_thread, NULL);
    if (ret != 0) {
        g_running_flag = NULL;
        return ret;
    }

    g_runtime.started = true;
    return 0;
}

void bflbwifid_runtime_stop(void)
{
    int ret;

    if (g_runtime.started) {
        ret = pthread_join(g_runtime.thread, NULL);
        if (ret != 0) {
            BFLBWIFID_LOGW("Failed to join runtime thread: %s", strerror(ret));
        }
        g_runtime.started = false;
    }

    g_running_flag = NULL;
    runtime_set_state(BFLBWIFID_STATE_STOPPING);
}
