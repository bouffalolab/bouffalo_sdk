/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "shell.h"

#include "audio_file.h"
#include <xcodec.h>

static void codec_template_init(int argc, char **argv)
{
    printf("codec_template_init\r\n");
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_codec_template(int argc, char **argv)
{
    codec_template_init(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_codec_template, codec_template, wifi codec template test);
#endif
