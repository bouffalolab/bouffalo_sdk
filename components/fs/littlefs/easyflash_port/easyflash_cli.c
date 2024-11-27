#include <stdio.h>
#include <shell.h>
#include <easyflash.h>

static void psm_set_cmd(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: psm_set [key] [value]\r\n");
        return;
    }
    ef_set_env(argv[1], argv[2]);
    ef_save_env();
}

static void psm_unset_cmd(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: psm_unset [key]\r\n");
        return;
    }
    ef_del_env(argv[1]);
    ef_save_env();
}

static void psm_dump_cmd(int argc, char **argv)
{
    ef_print_env();
}


static void psm_erase_cmd(int argc, char **argv)
{
    ef_env_set_default();
}


SHELL_CMD_EXPORT_ALIAS(psm_set_cmd, psm_set, cmd psm_set);
SHELL_CMD_EXPORT_ALIAS(psm_unset_cmd, psm_unset, cmd psm_unset);
SHELL_CMD_EXPORT_ALIAS(psm_dump_cmd, psm_dump, cmd psm_dump);
SHELL_CMD_EXPORT_ALIAS(psm_erase_cmd, psm_erase, cmd psm_erase);


