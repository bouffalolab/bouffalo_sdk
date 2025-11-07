
void cmd_hello(int argc, char *argv[]);
void cmd_phy(int argc, char *argv[]);
#ifdef CMD_TXL_CNTRL_PUSH_AC_ENABLE
void cmd_ac_set(int argc, char *argv[]);
#endif
#ifdef CFG_FOR_COEXISTENCE_TEST_STOPAP_PATCH
void cmd_ap_stop(int argc, char *argv[]);
#endif
void wifi_scan_cmd(int argc, char *argv[]);
void wifi_connect_cmd(int argc, char *argv[]);
void wifi_disconnect_cmd(int argc, char *argv[]);
void lwip_cmd(int argc, char *argv[]);
#ifdef CFG_FHOST_MONITOR
void cmd_wifi_sniffer_on(int argc, char *argv[]);
void cmd_wifi_sniffer_off(int argc, char *argv[]);
#endif
void cmd_wifi_state_get(int argc, char *argv[]);
void cmd_wifi_sta_rssi_get(int argc, char *argv[]);
void cmd_wifi_sta_channel_get(int argc, char *argv[]);
void cmd_wifi_sta_ssid_passphr_get(int argc, char *argv[]);
void cmd_wifi_sta_mac_get(int argc, char *argv[]);
void wifi_enable_autoreconnect_cmd(int argc, char *argv[]);
void wifi_disable_autoreconnect_cmd(int argc, char *argv[]);
void wifi_sta_ps_on_cmd(int argc, char *argv[]);
void wifi_sta_ps_off_cmd(int argc, char *argv[]);
void wifi_sta_ps_set_cmd(int argc, char **argv);
void wifi_sta_info_cmd(int argc, char *argv[]);
void wifi_ap_sta_list_get_cmd(int argc, char *argv[]);
void wifi_ap_sta_delete_cmd(int argc, char *argv[]);
void wifi_mgmr_ap_start_cmd(int argc, char *argv[]);
void wifi_mgmr_ap_stop_cmd(int argc, char *argv[]);
void cmd_wifi_ap_mac_get(int argc, char *argv[]);
void cmd_wifi_ap_conf_max_sta(int argc, char *argv[]);
void cmd_wifi_raw_send(int argc, char *argv[]);

#ifdef CONFIG_CLI_WIFI_DUBUG
void cmd_wifi(int argc, char *argv[]);
#endif
void wifi_sta_static_ipv4(int argc, char *argv[]);
#if MACSW_IPERF
void cmd_iperf(int argc, char *argv[]);
#endif
void cmd_rc(int argc, char *argv[]);
void cmd_rate(int argc, char *argv[]);
void cmd_non_pref_chan(int argc, char **argv);
void cmd_non_pref_chan_notify(int argc, char **argv);
void cmd_wifi_mgmr_sta_twt_setup(int argc, char **argv);
void cmd_wifi_mgmr_sta_twt_teardown(int argc, char **argv);
void cmd_wifi_adhoc_send(int argc, char *argv[]);
void cmd_adhoc_start(int argc, char *argv[]);
void cmd_adhoc_stop(int argc, char *argv[]);
void cmd_wifi_mode_set (int argc, char *argv[]);
void cmd_wifi_wps_pbc(int argc, char **argv);
void wifi_ap_bcn_mode_set_cmd(int argc, char **argv);
void wifi_mgmr_ap_acl_set_cmd(int argc, char **argv);
void wifi_mgmr_ap_acl_enable_cmd(int argc, char **argv);
void cmd_wifi_stats(int argc, char **argv);
void cmd_wifi_get_tx_remaining(int argc, char *argv[]);
#ifdef CONFIG_ANTDIV_STATIC
void wifi_antenna_scan_cmd(int argc, char *argv[]);
void wifi_antenna_scan_connect_cmd(int argc, char *argv[]);
#endif
