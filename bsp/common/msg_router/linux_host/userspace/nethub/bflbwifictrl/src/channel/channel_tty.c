/**
 * @file channel_tty.c
 * @brief TTY channel implementation for bflbwifid
 * @details This file wraps the existing bflbwifi_tty.c implementation to conform
 *          to the unified channel_ops_t interface
 */

#include "channel.h"
#include "../include/bflbwifi_log.h"
#include <string.h>

/* External functions from bflbwifi_tty.c */
extern int bflbwifi_tty_init(const char *dev, int baudrate,
                             void (*callback)(const char *line, void *user_data),
                             void *user_data);
extern void bflbwifi_tty_deinit(void);
extern int bflbwifi_tty_send(const char *data, int len);
extern int bflbwifi_tty_send_command(const char *cmd);
extern int bflbwifi_tty_send_command_sync(const char *cmd, int timeout_ms);
extern int bflbwifi_tty_wait_for(const char *expected, int timeout_ms);
extern int bflbwifi_tty_get_last_response(char *buf, size_t len);
extern void bflbwifi_tty_clear_last_response(void);
extern void bflbwifi_tty_set_disconnect_callback(void (*callback)(void));
extern void bflbwifi_tty_set_reconnect_callback(void (*callback)(void));
extern int bflbwifi_tty_suspend_recv_thread(void);
extern int bflbwifi_tty_resume_recv_thread(void);
extern int bflbwifi_tty_get_fd(void);

/* ========== TTY Channel Implementation ========== */

/**
 * @brief TTY channel init wrapper
 */
static int channel_tty_init(const char *dev, int baudrate,
                             channel_data_callback_t callback, void *user_data)
{
    if (!dev) {
        BFLB_LOGE("TTY device path is NULL");
        return -1;
    }

    BFLB_LOGI("Initializing TTY channel: %s @ %d baud", dev, baudrate);
    return bflbwifi_tty_init(dev, baudrate, callback, user_data);
}

/**
 * @brief TTY channel deinit wrapper
 */
static void channel_tty_deinit(void)
{
    BFLB_LOGI("Deinitializing TTY channel");
    bflbwifi_tty_deinit();
}

/**
 * @brief TTY channel send wrapper
 */
static int channel_tty_send(const char *data, int len)
{
    return bflbwifi_tty_send(data, len);
}

/**
 * @brief TTY channel send command wrapper
 */
static int channel_tty_send_command(const char *cmd)
{
    return bflbwifi_tty_send_command(cmd);
}

/**
 * @brief TTY channel send command sync wrapper
 */
static int channel_tty_send_command_sync(const char *cmd, int timeout_ms)
{
    return bflbwifi_tty_send_command_sync(cmd, timeout_ms);
}

/**
 * @brief TTY channel wait for wrapper
 */
static int channel_tty_wait_for(const char *expected, int timeout_ms)
{
    return bflbwifi_tty_wait_for(expected, timeout_ms);
}

/**
 * @brief TTY channel get last response wrapper
 */
static int channel_tty_get_last_response(char *buf, size_t len)
{
    return bflbwifi_tty_get_last_response(buf, len);
}

/**
 * @brief TTY channel clear last response wrapper
 */
static void channel_tty_clear_last_response(void)
{
    bflbwifi_tty_clear_last_response();
}

/**
 * @brief TTY channel get state wrapper
 * @note TTY implementation doesn't expose get_state, return CONNECTED as default
 */
static channel_state_t channel_tty_get_state(void)
{
    /* TTY implementation manages state internally but doesn't expose it
     * Return CONNECTED if initialized, otherwise DISCONNECTED */
    extern int bflbwifi_tty_get_fd(void);
    return (bflbwifi_tty_get_fd() >= 0) ? CHANNEL_STATE_CONNECTED : CHANNEL_STATE_DISCONNECTED;
}

/**
 * @brief TTY channel set disconnect callback wrapper
 */
static void channel_tty_set_disconnect_callback(channel_disconnect_callback_t callback)
{
    bflbwifi_tty_set_disconnect_callback(callback);
}

/**
 * @brief TTY channel set reconnect callback wrapper
 */
static void channel_tty_set_reconnect_callback(channel_reconnect_callback_t callback)
{
    bflbwifi_tty_set_reconnect_callback(callback);
}

/**
 * @brief TTY channel suspend recv thread wrapper
 */
static int channel_tty_suspend_recv_thread(void)
{
    return bflbwifi_tty_suspend_recv_thread();
}

/**
 * @brief TTY channel resume recv thread wrapper
 */
static int channel_tty_resume_recv_thread(void)
{
    return bflbwifi_tty_resume_recv_thread();
}

/**
 * @brief TTY channel get fd wrapper
 */
static int channel_tty_get_fd(void)
{
    return bflbwifi_tty_get_fd();
}

/* TTY channel operations structure */
const channel_ops_t channel_tty_ops = {
    .type = CHANNEL_TYPE_TTY,
    .name = "tty",
    .caps = CHANNEL_CAP_LINK_EVENTS | CHANNEL_CAP_RX_THREAD_CTL | CHANNEL_CAP_RAW_FD,
    .init = channel_tty_init,
    .deinit = channel_tty_deinit,
    .send = channel_tty_send,
    .send_command = channel_tty_send_command,
    .send_command_sync = channel_tty_send_command_sync,
    .wait_for = channel_tty_wait_for,
    .get_last_response = channel_tty_get_last_response,
    .clear_last_response = channel_tty_clear_last_response,
    .get_state = channel_tty_get_state,
    .set_disconnect_callback = channel_tty_set_disconnect_callback,
    .set_reconnect_callback = channel_tty_set_reconnect_callback,
    .suspend_recv_thread = channel_tty_suspend_recv_thread,
    .resume_recv_thread = channel_tty_resume_recv_thread,
    .get_fd = channel_tty_get_fd,
};
