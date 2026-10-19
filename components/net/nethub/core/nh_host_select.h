#ifndef NH_HOST_SELECT_H
#define NH_HOST_SELECT_H

#include "nethub_defs.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    NETHUB_HOST_SELECT_FIXED = 0,
    NETHUB_HOST_SELECT_INIT,
    NETHUB_HOST_SELECT_PROBING,
    NETHUB_HOST_SELECT_LOCKED_SDIO,
    NETHUB_HOST_SELECT_LOCKED_USB,
    NETHUB_HOST_SELECT_LOCKED_NONE,
} nethub_host_select_state_t;

typedef struct {
    bool dual_profile;
    nethub_host_select_state_t state;
    nethub_channel_t selected_host;
} nethub_host_select_status_t;

bool nethub_host_selection_is_dual(void);
void nethub_host_select_reset(void);
int nethub_host_select_start(void);
bool nethub_host_report_candidate(nethub_channel_t host);
bool nethub_host_is_locked_active(nethub_channel_t host);
nethub_channel_t nethub_host_selected(void);
void nethub_host_select_get_status(nethub_host_select_status_t *status);

const char *nethub_host_select_state_name(nethub_host_select_state_t state);

#endif
