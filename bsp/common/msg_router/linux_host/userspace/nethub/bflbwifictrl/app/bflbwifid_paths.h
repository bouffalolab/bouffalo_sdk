/**
 * @file bflbwifid_paths.h
 * @brief Shared local path defaults for bflbwifid and bflbwifictrl
 */

#ifndef BFLBWIFID_PATHS_H
#define BFLBWIFID_PATHS_H

#define BFLBWIFID_DEFAULT_SOCKET_PATH "/tmp/bflbwifi.sock"
#define BFLBWIFID_DEFAULT_PID_FILE    "/var/run/bflbwifid.pid"
#define BFLBWIFID_DEFAULT_LOG_FILE    "/var/log/bflbwifi.log"

typedef struct {
    const char *socket_path;
    const char *pid_file;
    const char *log_file;
} bflbwifid_paths_t;

#endif /* BFLBWIFID_PATHS_H */
