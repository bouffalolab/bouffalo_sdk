#include "nethub_ctrl_link.h"

#include <pthread.h>
#include <string.h>

#include "nethub_vchan.h"

static pthread_mutex_t g_link_lock = PTHREAD_MUTEX_INITIALIZER;
static nethub_ctrl_link_rx_cb_t g_rx_cb;
static void *g_rx_user_data;
static nethub_ctrl_link_event_cb_t g_event_cb;
static void *g_event_user_data;
static int g_link_initialized;

static void link_rx_trampoline(const void *data, size_t len)
{
    nethub_ctrl_link_rx_cb_t cb;
    void *user_data;

    pthread_mutex_lock(&g_link_lock);
    cb = g_rx_cb;
    user_data = g_rx_user_data;
    pthread_mutex_unlock(&g_link_lock);

    if (cb && data && len > 0) {
        cb((const uint8_t *)data, len, user_data);
    }
}

static void link_event_trampoline(nethub_vchan_link_state_t link_state, void *user_data)
{
    nethub_ctrl_link_event_cb_t cb;
    void *cb_user_data;
    nethub_wifi_link_state_t state;

    (void)user_data;
    state = (link_state == NETHUB_VCHAN_LINK_UP) ?
            NETHUB_WIFI_LINK_UP : NETHUB_WIFI_LINK_DOWN;

    pthread_mutex_lock(&g_link_lock);
    cb = g_event_cb;
    cb_user_data = g_event_user_data;
    pthread_mutex_unlock(&g_link_lock);

    if (cb) {
        cb(state, cb_user_data);
    }
}

int nethub_ctrl_link_init(nethub_ctrl_link_rx_cb_t rx_cb, void *user_data)
{
    int ret;

    pthread_mutex_lock(&g_link_lock);
    if (g_link_initialized) {
        pthread_mutex_unlock(&g_link_lock);
        return 0;
    }
    g_rx_cb = rx_cb;
    g_rx_user_data = user_data;
    pthread_mutex_unlock(&g_link_lock);

    ret = nethub_vchan_init();
    if (ret != 0) {
        pthread_mutex_lock(&g_link_lock);
        g_rx_cb = NULL;
        g_rx_user_data = NULL;
        pthread_mutex_unlock(&g_link_lock);
        return NETHUB_WIFI_ERR_LINK_DOWN;
    }

    ret = nethub_vchan_register_link_event_callback(link_event_trampoline, NULL);
    if (ret != 0) {
        nethub_vchan_deinit();
        pthread_mutex_lock(&g_link_lock);
        g_rx_cb = NULL;
        g_rx_user_data = NULL;
        pthread_mutex_unlock(&g_link_lock);
        return NETHUB_WIFI_ERR_LINK_DOWN;
    }

    ret = nethub_vchan_at_register_callback(link_rx_trampoline);
    if (ret != 0) {
        nethub_vchan_register_link_event_callback(NULL, NULL);
        nethub_vchan_deinit();
        pthread_mutex_lock(&g_link_lock);
        g_rx_cb = NULL;
        g_rx_user_data = NULL;
        pthread_mutex_unlock(&g_link_lock);
        return NETHUB_WIFI_ERR_LINK_DOWN;
    }

    pthread_mutex_lock(&g_link_lock);
    g_link_initialized = 1;
    pthread_mutex_unlock(&g_link_lock);
    return 0;
}

void nethub_ctrl_link_deinit(void)
{
    pthread_mutex_lock(&g_link_lock);
    if (!g_link_initialized) {
        pthread_mutex_unlock(&g_link_lock);
        return;
    }
    g_link_initialized = 0;
    g_rx_cb = NULL;
    g_rx_user_data = NULL;
    g_event_cb = NULL;
    g_event_user_data = NULL;
    pthread_mutex_unlock(&g_link_lock);

    nethub_vchan_register_link_event_callback(NULL, NULL);
    nethub_vchan_unregister_callback(NETHUB_VCHAN_DATA_TYPE_AT);
    nethub_vchan_deinit();
}

int nethub_ctrl_link_send(const void *data, size_t len)
{
    int ret;

    if (!data || len == 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    ret = nethub_vchan_at_send(data, len);
    return (ret < 0) ? NETHUB_WIFI_ERR_LINK_DOWN : 0;
}

int nethub_ctrl_link_recover(int timeout_ms)
{
    int ret = nethub_vchan_recover(timeout_ms);

    return (ret < 0) ? NETHUB_WIFI_ERR_LINK_DOWN : NETHUB_WIFI_OK;
}

int nethub_ctrl_link_get_state(nethub_wifi_link_state_t *state)
{
    nethub_vchan_state_snapshot_t snapshot;

    if (!state) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    if (nethub_vchan_get_state_snapshot(&snapshot) != 0) {
        *state = NETHUB_WIFI_LINK_DOWN;
        return NETHUB_WIFI_ERR_LINK_DOWN;
    }

    *state = (snapshot.link_state == NETHUB_VCHAN_LINK_UP) ?
             NETHUB_WIFI_LINK_UP : NETHUB_WIFI_LINK_DOWN;
    return 0;
}

int nethub_ctrl_link_register_event_callback(nethub_ctrl_link_event_cb_t cb, void *user_data)
{
    pthread_mutex_lock(&g_link_lock);
    g_event_cb = cb;
    g_event_user_data = user_data;
    pthread_mutex_unlock(&g_link_lock);
    return 0;
}
