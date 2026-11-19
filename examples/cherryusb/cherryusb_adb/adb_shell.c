/*
 * Copyright (c) 2024, Bouffalo Lab
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file adb_shell.c
 * @brief Minimal shell served over the ADB "shell:" service.
 */

#include <stdbool.h>
#include <string.h>

#include "bflb_mtimer.h"
#include "mm.h"
#include "usb_version.h"

#include "FreeRTOS.h"
#include "task.h"

#include "adb_demo.h"

static volatile bool shell_interactive;

/**
 * @brief Select between interactive and one-shot shell behaviour.
 * @param[in] interactive true after a bare "shell:" open request.
 */
void adb_shell_set_interactive(bool interactive)
{
    shell_interactive = interactive;
}

/**
 * @brief Report whether the current shell is interactive.
 * @return true when an interactive shell is open.
 */
bool adb_shell_is_interactive(void)
{
    return shell_interactive;
}

/** @brief Print the interactive shell banner. */
static void adb_shell_banner(void)
{
    adb_demo_printf("\r\nCherryUSB ADB demo shell (%s)\r\n", CHERRYUSB_VERSION_STR);
    adb_demo_printf("Type 'help' for a list of commands.\r\n");
}

/**
 * @brief Enter the interactive shell and print its prompt.
 */
void adb_shell_enter_interactive(void)
{
    shell_interactive = true;
    adb_shell_banner();
    adb_demo_printf(ADB_SHELL_PROMPT);
}

/** @brief Print the built-in command list. */
static void adb_shell_help(void)
{
    adb_demo_printf("Available commands:\r\n"
                    "  help          show this help\r\n"
                    "  echo <text>   print text\r\n"
                    "  free          show the SDK heap usage\r\n"
                    "  uptime        show the device uptime\r\n"
                    "  version       show the CherryUSB version\r\n"
                    "  exit          close the shell (also quit / Ctrl-D)\r\n");
}

/**
 * @brief Execute one shell command line.
 * @param[in] line NUL terminated command line without the trailing newline.
 * @retval 0 Command executed.
 * @retval ADB_SHELL_EXIT The interactive shell should be closed.
 */
int adb_shell_execute(const char *line)
{
    char command[128];
    size_t len;
    const char *arg;
    uint64_t uptime_ms;

    /* Copy the line so trailing spaces/tabs can be stripped. */
    len = strlen(line);
    if (len >= sizeof(command)) {
        len = sizeof(command) - 1;
    }
    memcpy(command, line, len);
    command[len] = '\0';
    while (len && (command[len - 1] == ' ' || command[len - 1] == '\t')) {
        command[--len] = '\0';
    }
    line = command;

    while (*line == ' ') {
        line++;
    }

    if (*line == '\0') {
        return 0;
    }

    if (!strcmp(line, "help")) {
        adb_shell_help();
        return 0;
    }

    if (!strcmp(line, "exit") || !strcmp(line, "quit")) {
        return ADB_SHELL_EXIT;
    }

    if (!strcmp(line, "free")) {
        adb_demo_printf("heap: %u bytes free, largest block: %u bytes\r\n",
                        (unsigned int)kfree_size(0),
                        (unsigned int)kmax_free_size(0));
        return 0;
    }

    if (!strcmp(line, "uptime")) {
        uptime_ms = bflb_mtimer_get_time_ms();
        adb_demo_printf("uptime: %u.%03u s\r\n",
                        (unsigned int)(uptime_ms / 1000),
                        (unsigned int)(uptime_ms % 1000));
        return 0;
    }

    if (!strcmp(line, "version")) {
        adb_demo_printf("CherryUSB %s (0x%06x)\r\n", CHERRYUSB_VERSION_STR, CHERRYUSB_VERSION);
        return 0;
    }

    if (!strncmp(line, "echo", 4) && (line[4] == ' ' || line[4] == '\0')) {
        arg = line + 4;
        while (*arg == ' ') {
            arg++;
        }
        adb_demo_printf("%s\r\n", arg);
        return 0;
    }

    adb_demo_printf("%s: not found\r\n", line);
    return 0;
}
