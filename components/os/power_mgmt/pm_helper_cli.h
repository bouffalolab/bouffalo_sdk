#ifndef PM_HELPER_CLI_H
#define PM_HELPER_CLI_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PM_HELPER_CLI_ARP_GATEWAY = 0,
    PM_HELPER_CLI_ARP_LOCAL_IP,
} pm_helper_cli_arp_target_t;

typedef int (*pm_helper_cli_wakeup_timer_cb_t)(uint32_t timeout_ms, int broadcast, void *arg);
typedef int (*pm_helper_cli_tickless_cb_t)(void *arg);

typedef struct {
    pm_helper_cli_wakeup_timer_cb_t wakeup_timer_cb;
    pm_helper_cli_tickless_cb_t tickless_cb;
    void *arg;
    pm_helper_cli_arp_target_t arp_target;
    uint32_t arp_period_seconds;
    bool arp_send_immediately;
    bool arp_periodic;
} pm_helper_cli_cfg_t;

int pm_helper_cli_init(const pm_helper_cli_cfg_t *cfg);
int pm_helper_cli_arp_timer_start(uint32_t period_seconds);
int pm_helper_cli_arp_timer_stop(void);
int pm_helper_cli_keepalive_timer_start(uint32_t period_seconds);
int pm_helper_cli_keepalive_timer_stop(void);
int pm_helper_cli_twt_setup(int setup_type, int flow_type, int wake_int_exp,
                            int min_twt_wake_dur, int wake_int_mantissa);

#endif
