#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int cmd_rf_dump(int argc, char **argv);
int wifi_bcnint_set(int argc, char **argv);
int wifi_capcode_cmd(int argc, char **argv);
int wifi_scan_cmd(int argc, char **argv);
int wifi_scan_filter_cmd(int argc, char **argv);
int wifi_mon_cmd(int argc, char **argv);
int cmd_wifi_raw_send(int argc, char **argv);
int wifi_sta_ip_info(int argc, char **argv);
int wifi_sta_ip_set_cmd(int argc, char **argv);
int wifi_sta_ip_unset_cmd(int argc, char **argv);
int wifi_disconnect_cmd(int argc, char **argv);
int wifi_connect_cmd(int argc, char **argv);
int wifi_sta_get_state_cmd(int argc, char **argv);
int wifi_enable_autoreconnect_cmd(int argc, char **argv);
int wifi_disable_autoreconnect_cmd(int argc, char **argv);
int wifi_rc_fixed_enable(int argc, char **argv);
int wifi_rc_fixed_disable(int argc, char **argv);
int wifi_power_saving_on_cmd(int argc, char **argv);
int wifi_power_saving_off_cmd(int argc, char **argv);
int wifi_power_saving_set(int argc, char **argv);
int wifi_power_saving_get(int argc, char **argv);
int wifi_denoise_enable_cmd(int argc, char **argv);
int wifi_denoise_disable_cmd(int argc, char **argv);
int wifi_sniffer_on_cmd(int argc, char **argv);
int wifi_sniffer_off_cmd(int argc, char **argv);
int cmd_wifi_ap_start(int argc, char **argv);
int cmd_wifi_ap_stop(int argc, char **argv);
int cmd_wifi_ap_chan_switch(int argc, char **argv);
int cmd_wifi_ap_conf_max_sta(int argc, char **argv);
int cmd_wifi_dump(int argc, char **argv);
int cmd_wifi_cfg(int argc, char **argv);
int cmd_dump_reset(int argc, char **argv);
int cmd_wifi_coex_rf_force_on(int argc, char **argv);
int cmd_wifi_coex_rf_force_off(int argc, char **argv);
int cmd_wifi_coex_pti_force_on(int argc, char **argv);
int cmd_wifi_coex_pti_force_off(int argc, char **argv);
int cmd_wifi_coex_pta_set(int argc, char **argv);
int wifi_ap_sta_list_get_cmd(int argc, char **argv);
int wifi_ap_sta_delete_cmd(int argc, char **argv);
int wifi_edca_dump_cmd(int argc, char **argv);
int cmd_wifi_state_get(int argc, char **argv);
int cmd_wifi_power_table_update(int argc, char **argv);

int wifi_mgmr_cli_init(void);

