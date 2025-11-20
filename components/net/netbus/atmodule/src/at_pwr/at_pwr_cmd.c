/**
  ******************************************************************************
  * @file    at_http_cmd.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "at_main.h"
#include "at_core.h"

#ifdef LP_APP
#include "wifi_mgmr_ext.h"

/// TWT Flow type
enum twt_flow_type
{
    /// Announced TWT (with PS-Poll)
    TWT_ANNOUNCED,
    /// Unannounced TWT (without PS-Poll)
    TWT_UNANNOUNCED
};

int lp_set_wakeup_by_io(uint8_t io, uint8_t mode);
int lp_delete_wakeup_by_io(uint8_t io);
void app_pm_enter_hbn(int level);
int app_lp_timer_config(int mode, uint32_t ms);
void app_pm_exit_pds15(void);
void app_pm_enter_pds15(void); // Added forward declaration

static void pwr_enable_receive_broadcast_multicast(int flag)
{
    extern int enable_multicast_broadcast;

    if (flag) {
        enable_multicast_broadcast = 1;
    } else {
        enable_multicast_broadcast = 0;
    }
}

static int at_pwr_cmd_pwrmode(int argc, const char **argv)
{
    int pwr_mode;
    int level = 0;
    int level_valid = 0;

    AT_CMD_PARSE_NUMBER(0, &pwr_mode);
    AT_CMD_PARSE_OPT_NUMBER(1, &level, level_valid);

    if (pwr_mode == 0 && argc == 1) {
        app_pm_exit_pds15();
    } else if (pwr_mode == 1 && argc == 2 && (level_valid && level >=0 && level <=2)) {
        app_pm_enter_hbn(level);
    } else if (pwr_mode == 2) {
        if (level_valid && level == 1 && argc == 2) {
            pwr_enable_receive_broadcast_multicast(1);
            app_pm_enter_pds15();
        } else if ((level_valid && level == 0 && argc == 2) || (level_valid == 0 && argc == 1)) {
            pwr_enable_receive_broadcast_multicast(0);
            app_pm_enter_pds15();
        } else {
            AT_CMD_PRINTF("[AT_PWR] Error: invalid level for pwr_mode 2\r\n");
            return AT_RESULT_CODE_ERROR;
        }
    } else {
        AT_CMD_PRINTF("[AT_PWR] Error: invalid pwr_mode value %d\r\n", pwr_mode);
        return AT_RESULT_CODE_ERROR;
    }

    return AT_RESULT_CODE_OK;
}

static int at_wakeup_timer_cmd(int argc, const char **argv)
{
    int mode;
    int timeouts_ms;
    int ret;

    AT_CMD_PARSE_NUMBER(0, &mode);
    AT_CMD_PARSE_NUMBER(1, &timeouts_ms);

    if (mode < 0 || mode > 2 || argc != 2 || timeouts_ms <= 0) {
        AT_CMD_PRINTF("[AT_PWR] Error: invalid mode value %d\r\n", mode);
        return AT_RESULT_CODE_ERROR;
    }

    ret = app_lp_timer_config(mode, timeouts_ms);
    if (ret) {
        return AT_RESULT_CODE_ERROR;
    }

    return AT_RESULT_CODE_OK;
}

static int at_dtim_cmd(int argc, const char **argv)
{
    int dtim;

    AT_CMD_PARSE_NUMBER(0, &dtim);
    if (dtim < 0 || dtim > 256) {
        return AT_RESULT_CODE_ERROR;
    }

    void set_dtim_config(int dtim);
    set_dtim_config(dtim);
    return AT_RESULT_CODE_OK;
}

static int clear_dtim_cmd(int argc, const char **argv)
{
    //void clear_dtim_config(void);
    //clear_dtim_config();
    return AT_RESULT_CODE_OK;
}

static int at_wkio_cmd(int argc, const char **argv)
{
    int wkio;
    int mode;

    if (argc != 2) {
        return AT_RESULT_CODE_ERROR;
    }

    AT_CMD_PARSE_NUMBER(0, &wkio);
    AT_CMD_PARSE_NUMBER(1, &mode);

    int ret = lp_set_wakeup_by_io(wkio, mode);

    if (ret) {
        return AT_RESULT_CODE_ERROR;
    }

    return AT_RESULT_CODE_OK;
}

static int at_dewkio_cmd(int argc, const char **argv)
{
    int wkio;

    AT_CMD_PARSE_NUMBER(0, &wkio);

    int ret = lp_delete_wakeup_by_io(wkio);
    if (!ret) {
        return AT_RESULT_CODE_OK;
    }

    return AT_RESULT_CODE_ERROR;
}

static int at_start_keepalive_cmd(int argc, const char **argv)
{
    int period;

    AT_CMD_PARSE_NUMBER(0, &period);
    if (period <= 0 || (period > 3600000)) {
        return AT_RESULT_CODE_ERROR;
    }

    AT_CMD_PRINTF("at_start_keepalive_cmd period:%d\r\n", period);
    int app_create_keepalive_timer(uint32_t periods);
    app_create_keepalive_timer(period);
    return AT_RESULT_CODE_OK;
}

static int at_stop_keepalive_cmd(int argc, const char **argv)
{
    int app_delete_keepalive_timer(void);
    app_delete_keepalive_timer();
    return AT_RESULT_CODE_OK;
}

static int at_start_arp_send_cmd(int argc, const char **argv)
{
    int period;

    AT_CMD_PARSE_NUMBER(0, &period);

    if (period <= 0 || period > 3600000) {
        return AT_RESULT_CODE_ERROR;
    }

    int app_pm_create_arp_announce_timer(uint32_t seconds);
    app_pm_create_arp_announce_timer(period);
    return AT_RESULT_CODE_OK;
}

static int at_stop_arp_send_cmd(int argc, const char **argv)
{
    int app_pm_delete_arp_announce_timer(void);
    app_pm_delete_arp_announce_timer();
    return AT_RESULT_CODE_OK;
}

static int at_twt_param_cmd(int argc, const char **argv)
{
    int s, t, e, n, m;
    int ret;

    AT_CMD_PARSE_NUMBER(0, &s);
    AT_CMD_PARSE_NUMBER(1, &t);
    AT_CMD_PARSE_NUMBER(2, &e);
    AT_CMD_PARSE_NUMBER(3, &n);
    AT_CMD_PARSE_NUMBER(4, &m);

    int app_pm_twt_param_set(int s, int t, int e, int n, int m);
    ret = app_pm_twt_param_set(s, t, e, n, m);
    if (ret) {
        return AT_RESULT_CODE_ERROR;
    }

    return AT_RESULT_CODE_OK;
}

static int at_twt_sleep_cmd(int argc, const char **argv)
{
    //void app_pm_twt_enter(void);
    //app_pm_twt_enter();
    return AT_RESULT_CODE_OK;
}

static int twt_flows_check(uint8_t id)
{
    struct twt_status_info twt_confs[8]; // Array to hold TWT configurations
    uint8_t twt_num = 0; // Number of active TWT flows
    int ret;

    // Get TWT status using WiFi manager function
    ret = wifi_mgmr_sta_twt_statusget(twt_confs, &twt_num);

    printf("twt num:%d\r\n", twt_num);

    if (ret != 0) {
        return -1;
    }

    if (twt_num == 0) {
        return -1;
    } else {
        // Process each active TWT flow
        for (int i = 0; i < twt_num && i < 8; i++) {
            if (id == twt_confs[i].flow_id) {
                return 0;
            }
        }
    }

    return -1;
}

static int at_twt_teardown_cmd(int argc, const char **argv)
{
    int neg_type = 0;
    int all_twt  = 0;
    int flow_id  = 0;
    int ret;

    if (argc < 1) {
        AT_CMD_PRINTF("Error: Missing <negotiation_type> (-n).\r\n");
        return AT_RESULT_CODE_ERROR;
    }
    AT_CMD_PARSE_NUMBER(0, &neg_type);
    if (neg_type != 0) {
        AT_CMD_PRINTF("Warning: Only Individual negotiation (0) supported, use 0.\r\n");
        return AT_RESULT_CODE_ERROR;
    }

    if (argc < 2) {
        AT_CMD_PRINTF("Error: Missing <all_twt> (-a).\r\n");
        return AT_RESULT_CODE_ERROR;
    }
    AT_CMD_PARSE_NUMBER(1, &all_twt);
    if (all_twt != 0 && all_twt != 1) {
        AT_CMD_PRINTF("Error: <all_twt> must be 0 or 1.\r\n");
        return AT_RESULT_CODE_ERROR;
    }

    if (all_twt == 0) {
        if (argc < 3) {
            AT_CMD_PRINTF("Error: Missing <twt_flow_id> (-i) when -a 0.\r\n");
            return AT_RESULT_CODE_ERROR;
        }
        AT_CMD_PARSE_NUMBER(2, &flow_id);
        ret = twt_flows_check(flow_id);
        if (ret) { 
            AT_CMD_PRINTF("Error: <twt_flow_id> must be in 0-7.\r\n");
            return AT_RESULT_CODE_ERROR;
        }
    } else {
        flow_id = 0; 
    }

    AT_CMD_PRINTF("TWT Teardown: neg_type=%d, all_twt=%d", neg_type, all_twt);
    if (!all_twt)
        AT_CMD_PRINTF(", flow_id=%d", flow_id);
    AT_CMD_PRINTF("\r\n");

    twt_teardown_params_struct_t params;
    memset(&params, 0, sizeof(params));
    params.neg_type = neg_type;
    params.all_twt  = all_twt;
    params.id       = flow_id;

    extern int wifi_mgmr_sta_twt_teardown(twt_teardown_params_struct_t *params);
    ret = wifi_mgmr_sta_twt_teardown(&params);

    if (ret == 0) {
        AT_CMD_PRINTF("TWT teardown request sent successfully\r\n");
        return AT_RESULT_CODE_OK;
    } else {
        AT_CMD_PRINTF("TWT teardown request failed, error=%d\r\n", ret);
        return AT_RESULT_CODE_ERROR;
    }
}


static int at_clock_source_set_cmd(int argc, const char **argv)
{
    int source;
    int ret;

    AT_CMD_PARSE_NUMBER(0, &source);

    int app_set_clock_source(uint8_t source);
    ret = app_set_clock_source(source);
    if (ret) {
        return AT_RESULT_CODE_ERROR;
    }

    return AT_RESULT_CODE_OK;
}

static int at_clock_source_get_cmd(int argc, const char **argv)
{
    uint8_t source = 0;
    int app_get_clock_source(uint8_t *source);
    int ret = app_get_clock_source(&source);
    if (ret != 0) {
        AT_CMD_PRINTF("[AT_PWR] Error: failed to get clock source\r\n");
        return AT_RESULT_CODE_ERROR;
    }
    at_response_string("+GET_CLOCK:%d\r\n", source);
    return AT_RESULT_CODE_OK;
}

static int at_twt_status_cmd(int argc, const char **argv)
{
    struct twt_status_info twt_confs[8]; // Array to hold TWT configurations
    uint8_t twt_num = 0; // Number of active TWT flows
    int ret;

    // Get TWT status using WiFi manager function
    ret = wifi_mgmr_sta_twt_statusget(twt_confs, &twt_num);

    if (ret != 0) {
        at_response_string("+TWT:INACTIVE\r\n");
        return AT_RESULT_CODE_OK;
    }

    if (twt_num == 0) {
        // No active TWT sessions
        at_response_string("+TWT:INACTIVE\r\n");
    } else {
        // Process each active TWT flow
        for (int i = 0; i < twt_num && i < 8; i++) {
            // Get flow type string
            const char *flow_type_str = (twt_confs[i].flow_type == 0) ? "ANNOUNCED" : "UNANNOUNCED";

            // Output TWT parameters from the struct
            at_response_string("+TWT:ACTIVE:%d,%d,%s,%d,%d,%d\r\n",
                             twt_confs[i].flow_id,
                             twt_confs[i].flow_type,
                             flow_type_str,
                             twt_confs[i].wake_int_exp,
                             twt_confs[i].min_twt_wake_dur,
                             twt_confs[i].wake_int_mantissa);
        }
    }

    return AT_RESULT_CODE_OK;
}

static int at_listen_itv_get_cmd(int argc, const char **argv)
{
    int listen_itv = wifi_mgmr_sta_get_listen_itv();

    if (!wifi_mgmr_sta_state_get()) {
        AT_CMD_PRINTF("[AT_PWR] Error: failed to get listen_itv\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    at_response_string("+LISTEN_ITV:%d\r\n", listen_itv);
    return AT_RESULT_CODE_OK;
}

static int at_dtim_get_cmd(int argc, const char **argv)
{
    int dtim = wifi_mgmr_get_dtim();

    if (dtim < 0 || dtim == 0xff) {
        AT_CMD_PRINTF("[AT_PWR] Error: failed to get listen_itv\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    at_response_string("+DTIM:%d\r\n", dtim);
    return AT_RESULT_CODE_OK;
}

static int at_twt_supported_get_cmd(int argc, const char **argv)
{
    int support = wifi_mgmr_twt_support();

    if (support < 0 || support == 0xff) {
        AT_CMD_PRINTF("[AT_PWR] Error: failed to get listen_itv\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    at_response_string("+TWT_SUPPORT:%d\r\n", support);
    return AT_RESULT_CODE_OK;
}

static int at_pwr_clear_cmd(int argc, const char **argv)
{
    int pwr_info_clear(void);
    pwr_info_clear();
    return AT_RESULT_CODE_OK;
}

static int at_pwr_get_cmd(int argc, const char **argv)
{
    uint64_t predect_pwr;

    uint64_t pwr_info_get(void);
    predect_pwr = pwr_info_get();
    at_response_string("+AVERAGE PWR:%lld\r\n", predect_pwr);
    return AT_RESULT_CODE_OK;
}

static int at_lp_interval_get_cmd(int argc, const char **argv)
{
    uint8_t lp_interval;

    uint8_t lp_interval_get(void);
    lp_interval = lp_interval_get();
    at_response_string("+LP INTERVAL:%d\r\n", lp_interval);
    return AT_RESULT_CODE_OK;
}

static const at_cmd_struct at_pwr_cmd[] = {
    {"+PWR",                NULL, at_pwr_cmd_pwrmode, NULL, 1, 3},
    {"+SLWKDTIM",           NULL, at_dtim_cmd, NULL, 1, 1},
    {"+SLCLDTIM",           NULL, NULL, clear_dtim_cmd, 0, 0},
    {"+SLWKIO",             NULL, at_wkio_cmd, NULL, 2, 2},
    {"+DEWKIO",             NULL, at_dewkio_cmd, NULL, 1, 1},
    {"+SLWKTIMER",          NULL, at_wakeup_timer_cmd, NULL, 2, 2},
    {"+START_KEEPALIVE",    NULL, at_start_keepalive_cmd, NULL, 1, 1},
    {"+STOP_KEEPALIVE",     NULL, NULL, at_stop_keepalive_cmd, 0, 0},
    {"+START_ARP",          NULL, at_start_arp_send_cmd, NULL, 1, 1},
    {"+STOP_ARP",           NULL, NULL, at_stop_arp_send_cmd, 0, 0},
    {"+TWT_PARAM",          NULL, at_twt_param_cmd, NULL, 5, 5},
    {"+TWT_SLEEP",          NULL, NULL, at_twt_sleep_cmd, 0, 0},
    {"+TWT_TEARDOWN",       NULL, at_twt_teardown_cmd, at_twt_teardown_cmd, 0, 3},
    {"+TWT_STATUS",         at_twt_status_cmd, NULL, NULL, 0, 0},
    {"+SET_CLOCK",          NULL, at_clock_source_set_cmd, NULL, 1, 1},
    {"+GET_CLOCK",          NULL, NULL, at_clock_source_get_cmd, 0, 0},
    {"+GET_AP_DTIM",        at_dtim_get_cmd, NULL, NULL, 0, 1},
    {"+GET_TWT_SUPPORTED",  at_twt_supported_get_cmd, NULL, NULL, 0, 1},
    {"+PWR_CLEAR",          NULL, NULL, at_pwr_clear_cmd, 0, 0},
    {"+PWR_GET",            NULL, NULL, at_pwr_get_cmd, 0, 0},
    {"+LP_INTERVAL_GET",    NULL, NULL, at_lp_interval_get_cmd, 0, 0},
    {NULL,              NULL, NULL, NULL, 0, 0},
};

bool at_pwr_cmd_regist(void)
{
    if (at_cmd_register(at_pwr_cmd, sizeof(at_pwr_cmd) / sizeof(at_pwr_cmd[0])) == 0)
        return true;
    else
        return false;
}
#endif
