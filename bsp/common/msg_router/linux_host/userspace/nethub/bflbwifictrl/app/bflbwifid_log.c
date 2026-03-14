/**
 * @file bflbwifid_log.c
 * @brief Shared logging helpers for bflbwifid modules
 */

#include "bflbwifid_log.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bflbwifi_ipc.h"

static pthread_mutex_t g_log_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_log_fd = -1;

static void log_format_timestamp(char *buf, size_t len)
{
    struct tm tm_info;
    time_t now;

    if (!buf || len == 0) {
        return;
    }

    now = time(NULL);
    if (localtime_r(&now, &tm_info) == NULL ||
        strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm_info) == 0) {
        snprintf(buf, len, "1970-01-01 00:00:00");
    }
}

static void log_write_fd_unlocked(int fd, const char *buf, size_t len)
{
    size_t written = 0;

    while (written < len) {
        ssize_t ret = write(fd, buf + written, len - written);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        if (ret == 0) {
            break;
        }
        written += (size_t)ret;
    }
}

int bflbwifid_log_init(const char *log_file)
{
    int fd;

    if (!log_file || log_file[0] == '\0') {
        errno = EINVAL;
        return -1;
    }

    fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        return -1;
    }

    pthread_mutex_lock(&g_log_lock);
    if (g_log_fd >= 0) {
        close(g_log_fd);
    }
    g_log_fd = fd;
    pthread_mutex_unlock(&g_log_lock);

    return 0;
}

void bflbwifid_log_deinit(void)
{
    pthread_mutex_lock(&g_log_lock);
    if (g_log_fd >= 0) {
        close(g_log_fd);
        g_log_fd = -1;
    }
    pthread_mutex_unlock(&g_log_lock);
}

void bflbwifid_log_write(const char *level, const char *fmt, ...)
{
    char message[MAX_BUFFER_SIZE];
    char timestamp[64];
    char line[MAX_BUFFER_SIZE];
    va_list args;
    int line_len;

    log_format_timestamp(timestamp, sizeof(timestamp));

    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    line_len = snprintf(line, sizeof(line), "[%s] [%s] %s\n", timestamp, level ? level : "INFO", message);
    if (line_len < 0) {
        return;
    }
    if ((size_t)line_len >= sizeof(line)) {
        line_len = (int)(sizeof(line) - 1u);
    }

    pthread_mutex_lock(&g_log_lock);
    log_write_fd_unlocked(STDERR_FILENO, line, (size_t)line_len);
    if (g_log_fd >= 0) {
        log_write_fd_unlocked(g_log_fd, line, (size_t)line_len);
    }
    pthread_mutex_unlock(&g_log_lock);
}
