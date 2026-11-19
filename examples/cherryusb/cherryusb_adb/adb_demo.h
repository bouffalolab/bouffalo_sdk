/*
 * Copyright (c) 2024, Bouffalo Lab
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file adb_demo.h
 * @brief CherryUSB ADB device demo interfaces.
 */

#ifndef ADB_DEMO_H
#define ADB_DEMO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum ADB payload advertised to the host (A_CNXN maxdata). */
#define ADB_DEMO_MAX_PAYLOAD 4096

/** @brief Prompt printed by the interactive shell. */
#define ADB_SHELL_PROMPT "cherryadb:/ $ "

/** @brief Return value of adb_shell_execute() when the shell should close. */
#define ADB_SHELL_EXIT 1

/**
 * @brief Register the ADB device with CherryUSB and start its worker task.
 *
 * Must be called after the scheduler has been started.
 */
void adb_demo_init(void);

/**
 * @brief Queue one binary buffer to the ADB shell channel.
 *
 * Chunks longer than ADB_DEMO_MAX_PAYLOAD are split automatically and each
 * chunk waits for the previous IN transfer to complete. Must only be called
 * from the ADB worker task (or any other single task context).
 *
 * @param[in] data Buffer to send.
 * @param[in] len Number of bytes to send.
 */
void adb_demo_write(const void *data, uint32_t len);

/**
 * @brief printf-style helper that sends formatted text to the ADB shell.
 *
 * @param[in] fmt printf format string.
 * @return Number of bytes queued, or a negative value on formatting failure.
 */
int adb_demo_printf(const char *fmt, ...);

/* Implemented in adb_shell.c. */
void adb_shell_set_interactive(bool interactive);
bool adb_shell_is_interactive(void);
void adb_shell_enter_interactive(void);
int adb_shell_execute(const char *line);

#ifdef __cplusplus
}
#endif

#endif /* ADB_DEMO_H */
