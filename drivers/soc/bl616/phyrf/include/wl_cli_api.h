#ifndef __WL_CLI_API_H__
#define __WL_CLI_API_H__

typedef struct
{
    uint32_t rxok;
    uint32_t rxend;
    float ppm_dsss;
    float ppm_ofdm;
    int8_t rssi;
} cli_ate_ret_t;

void
cmd_init_register(char *buf, int len, int argc, char **argv);
void cmd_channel_register(char *buf, int len, int argc, char **argv);
void cmd_bandwidth_register(char *buf, int len, int argc, char **argv);
void cmd_tx_stop_register(char *buf, int len, int argc, char **argv);
void cmd_tx11b_start_register(char *buf, int len, int argc, char **argv);
void cmd_tx11g_start_register(char *buf, int len, int argc, char **argv);
void cmd_tx11n_start_register(char *buf, int len, int argc, char **argv);
void cmd_rx_start_at(char *buf, int len, int argc, char **argv);
void cmd_rx_poll_stat_at(char *buf, int len, int argc, char **argv);
void cmd_rx_stop_at(char *buf, int len, int argc, char **argv);
void cmd_tx11ax_su_start_register(char *buf, int len, int argc, char **argv);
void cmd_tx11ax_er_start_register(char *buf, int len, int argc, char **argv);
void cmd_tx11ax_tb_start_register(char *buf, int len, int argc, char **argv);
void cmd_le_tx_test(char *buf, int len, int argc, char **argv);
void cmd_le_rx_test(char *buf, int len, int argc, char **argv);
void cmd_le_test_stop(char *buf, int len, int argc, char **argv);
void cmd_cw_stop_register(char *buf, int len, int argc, char **argv);
void cmd_cw_start_register(char *buf, int len, int argc, char **argv);
void cmd_bz_set_target_power_register(char *buf, int len, int argc, char **argv);
void cmd_rx_start_ate(char *buf, int len, int argc, char **argv);
void cmd_rx_ate_get_ret(cli_ate_ret_t* ret);

#endif
