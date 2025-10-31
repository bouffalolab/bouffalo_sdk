#if 0
#include <aos/cli.h>
#include <msp/kernel.h>

static msp_task_t g_msp_loop_task;

extern int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume);
static void msp_loop_proc_task(void *arg)
{
    const char *url = "file:///mnt/wifiprov_timeout.mp3";

    while (1) {
        smtaudio_start(1/*MEDIA_SYSTEM*/, url, 0, 0);
        msp_msleep(7000);
    }
}

extern uint32_t g_pa_delay_1;
extern uint32_t g_pa_delay_2;

static void cmd_msp_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    static int init = 0;

    if (3 == argc) {
        g_pa_delay_1 = atoi(argv[1]);
        g_pa_delay_2 = atoi(argv[2]);
    }

    printf("========= g_pa_delay_1 = %d, g_pa_delay_2 = %d\r\n",
             g_pa_delay_1, g_pa_delay_2);

    if (0 == init) {
        msp_task_new_ext(&g_msp_loop_task, "msp_loop", msp_loop_proc_task, NULL, 4096, MSP_DEFAULT_APP_PRI + 4);
        init = 1;
    }
}

static void cmd_mspclock_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    void msp_clock_tree(void);
    msp_clock_tree();
}

static const struct cli_command _app_cli_cmd[] = {
    /*cli self*/
    {"msp", "msp test", cmd_msp_func},
    {"mspclock", "msp clock", cmd_mspclock_func},
};

void cli_reg_cmd_codectest(void)
{
    int ret;

    ret = aos_cli_register_commands(_app_cli_cmd, sizeof(_app_cli_cmd)/sizeof(struct cli_command));
    if (ret != 0) {
        return ret;
    }
}

#endif
