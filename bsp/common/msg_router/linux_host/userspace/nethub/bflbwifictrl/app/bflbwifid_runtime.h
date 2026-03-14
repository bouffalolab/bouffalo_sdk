/**
 * @file bflbwifid_runtime.h
 * @brief Shared runtime state declarations for bflbwifid modules
 */

#ifndef BFLBWIFID_RUNTIME_H
#define BFLBWIFID_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <signal.h>
#include <stdint.h>

#include "../include/bflbwifi.h"

typedef uint8_t bflbwifid_state_t;

#define BFLBWIFID_STATE_STARTING     ((bflbwifid_state_t)0u)
#define BFLBWIFID_STATE_READY        ((bflbwifid_state_t)1u)
#define BFLBWIFID_STATE_RECONNECTING ((bflbwifid_state_t)2u)
#define BFLBWIFID_STATE_STOPPING     ((bflbwifid_state_t)3u)

const char *bflbwifid_state_name(bflbwifid_state_t state);
int bflbwifid_get_vchan_host_state_line(char *buf, size_t len, uint8_t ctrl_backend);
int bflbwifid_refresh_runtime_state(bool log_changes);
void bflbwifid_get_runtime_snapshot(bflbwifid_state_t *state, bflbwifi_ctrl_status_t *ctrl_status);
int bflbwifid_runtime_start(const volatile sig_atomic_t *running_flag);
void bflbwifid_runtime_stop(void);

#endif /* BFLBWIFID_RUNTIME_H */
