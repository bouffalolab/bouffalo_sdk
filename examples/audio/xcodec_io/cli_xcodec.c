/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "cli_xcodec.h"
#include "board.h"
#include "board_audio.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "shell.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    AUDIO_STATUS,
    AUDIO_VOLUME,
    AUDIO_MUTE,
    AUDIO_ECHO,
    AUDIO_TONE,
    AUDIO_REG,
} audio_command_type_t;

typedef struct {
    audio_command_type_t type;
    int value;
    int reg_value;
} audio_command_t;

static QueueHandle_t audio_commands;

static void cmd_xcodec(int argc, char **argv)
{
    audio_command_t command = {.reg_value = -1};
    int limit = 0;
    if (argc == 1 || (argc == 2 && !strcmp(argv[1], "status"))) {
        command.type = AUDIO_STATUS;
    } else if ((argc == 2 || argc == 3) && !strcmp(argv[1], "tone")) {
        command.type = AUDIO_TONE;
        command.value = 4096;
        limit = 8192;
    } else if ((argc == 2 || argc == 3) && !strcmp(argv[1], "echo")) {
        command.type = AUDIO_ECHO;
        command.value = -1;
        limit = BOARD_AUDIO_CHANNELS - 1;
    } else if (argc == 3 && !strcmp(argv[1], "volume")) {
        command.type = AUDIO_VOLUME;
        limit = 100;
    } else if (argc == 3 && !strcmp(argv[1], "mute")) {
        command.type = AUDIO_MUTE;
        limit = 1;
    } else if ((argc == 3 || argc == 4) && !strcmp(argv[1], "reg")) {
        command.type = AUDIO_REG;
        limit = 0x1ff;
    } else {
        goto usage;
    }
    if (argc >= 3) {
        char *end;
        errno = 0;
        long value = strtol(argv[2], &end, command.type == AUDIO_REG ? 0 : 10);
        if (errno || !argv[2][0] || *end || value < 0 || value > limit) {
            goto usage;
        }
        command.value = (int)value;
    }
    if (argc == 4) {
        char *end;
        errno = 0;
        long value = strtol(argv[3], &end, 0);
        if (errno || !argv[3][0] || *end || value < 0 || value > 255) {
            goto usage;
        }
        command.reg_value = (int)value;
    }
    if (xQueueSend(audio_commands, &command, 0) != pdPASS) {
        printf("xcodec: command queue full\r\n");
    }
    return;
usage:
    printf("xcodec [status|volume 0..100|mute 0..1|echo [channel]|tone [0..8192]|reg address [value]]\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_xcodec, xcodec, audio IO debug);

static void print_status(xcodec_dev_handle_t play_dev)
{
    int volume = -1;
    bool muted = false;
    xcodec_dev_get_out_vol(play_dev, &volume);
    xcodec_dev_get_out_mute(play_dev, &muted);
    printf("xcodec: volume=%d mute=%u\r\n", volume, (unsigned)muted);
    audio_print_status();
    static const int regs[] = {0x0a, 0x0b, 0x3a, 0x3b, 0x3c, 0x3d, 0x80, 0x81};
    for (unsigned i = 0; i < sizeof(regs) / sizeof(regs[0]); ++i) {
        int value = -1;
        int ret = xcodec_dev_read_reg(play_dev, regs[i], &value);
        printf("xcodec: reg=0x%02x value=0x%02x ret=%d\r\n", regs[i], value, ret);
    }
}

int xcodec_cli_init(void)
{
    audio_commands = xQueueCreate(8, sizeof(audio_command_t));
    if (!audio_commands) {
        return -ENOMEM;
    }
    extern void shell_init_with_task(struct bflb_device_s *shell);
    shell_init_with_task(bflb_device_get_by_name("uart0"));
    return 0;
}

void xcodec_cli_process(xcodec_dev_handle_t play_dev)
{
    audio_command_t command;
    /* Only this task owns device control and audio IO. */
    if (xQueueReceive(audio_commands, &command, 0) == pdPASS) {
        int control_ret = 0;
        switch (command.type) {
            case AUDIO_VOLUME:
                control_ret = xcodec_dev_set_out_vol(play_dev, command.value);
                break;
            case AUDIO_MUTE:
                control_ret = xcodec_dev_set_out_mute(play_dev, command.value != 0);
                break;
            case AUDIO_ECHO:
                control_ret = audio_set_echo_channel(command.value);
                break;
            case AUDIO_TONE:
                audio_start_tone(command.value);
                break;
            case AUDIO_REG: {
                int value = -1;
                if (command.reg_value >= 0) {
                    control_ret = xcodec_dev_write_reg(play_dev, command.value, command.reg_value);
                }
                if (!control_ret) {
                    control_ret = xcodec_dev_read_reg(play_dev, command.value, &value);
                }
                printf("xcodec: reg=0x%02x value=0x%02x ret=%d\r\n",
                       command.value, value, control_ret);
                break;
            }
            case AUDIO_STATUS:
                print_status(play_dev);
                break;
        }
        printf("xcodec: command=%u value=%d ret=%d\r\n",
               (unsigned)command.type, command.value, control_ret);
    }
}
