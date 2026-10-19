/**
 * @file bflbwifid_instance.c
 * @brief Daemon instance ownership and local IPC endpoint lifecycle
 */

#include "bflbwifid_instance.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "bflbwifi_ipc.h"
#include "bflbwifid_log.h"

#define STARTUP_HANDOFF_RETRY_COUNT    30
#define STARTUP_HANDOFF_RETRY_DELAY_US 100000

typedef struct {
    dev_t dev;
    ino_t ino;
    bool valid;
} path_identity_t;

static path_identity_t g_socket_identity;
static path_identity_t g_pid_identity;

static const char *paths_socket_path(const bflbwifid_paths_t *paths)
{
    if (paths && paths->socket_path && paths->socket_path[0] != '\0') {
        return paths->socket_path;
    }

    return BFLBWIFID_DEFAULT_SOCKET_PATH;
}

static const char *paths_pid_file(const bflbwifid_paths_t *paths)
{
    if (paths && paths->pid_file && paths->pid_file[0] != '\0') {
        return paths->pid_file;
    }

    return BFLBWIFID_DEFAULT_PID_FILE;
}

static int record_path_identity(const char *path, path_identity_t *identity)
{
    struct stat st;

    if (!path || !identity) {
        return -1;
    }

    if (lstat(path, &st) != 0) {
        return -1;
    }

    identity->dev = st.st_dev;
    identity->ino = st.st_ino;
    identity->valid = true;
    return 0;
}

static void unlink_if_identity_matches(const char *path, path_identity_t *identity, const char *desc)
{
    struct stat st;

    if (!path || !identity || !identity->valid) {
        return;
    }

    if (lstat(path, &st) != 0) {
        if (errno != ENOENT) {
            BFLBWIFID_LOGW("Failed to stat %s %s: %s", desc, path, strerror(errno));
        }
        identity->valid = false;
        return;
    }

    if (st.st_dev == identity->dev && st.st_ino == identity->ino) {
        if (unlink(path) != 0 && errno != ENOENT) {
            BFLBWIFID_LOGW("Failed to unlink %s %s: %s", desc, path, strerror(errno));
        }
    } else {
        BFLBWIFID_LOGI("Skip removing %s %s: path already belongs to another instance", desc, path);
    }

    identity->valid = false;
}

static int read_pid_file(const bflbwifid_paths_t *paths, pid_t *pid_out)
{
    FILE *fp;
    long pid_value;
    const char *pid_file = paths_pid_file(paths);

    if (!pid_out) {
        return -1;
    }

    fp = fopen(pid_file, "r");
    if (!fp) {
        return -1;
    }

    if (fscanf(fp, "%ld", &pid_value) != 1) {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    *pid_out = (pid_t)pid_value;
    return 0;
}

static bool process_is_alive(pid_t pid)
{
    if (pid <= 0) {
        return false;
    }

    if (kill(pid, 0) == 0) {
        return true;
    }

    return errno == EPERM;
}

static int socket_path_is_active(const bflbwifid_paths_t *paths)
{
    int fd;
    int saved_errno;
    struct sockaddr_un addr;
    struct stat st;
    const char *socket_path = paths_socket_path(paths);

    if (lstat(socket_path, &st) != 0) {
        return 0;
    }

    if (!S_ISSOCK(st.st_mode)) {
        BFLBWIFID_LOGW("Socket path %s exists but is not a socket, treat it as stale", socket_path);
        return 0;
    }

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        return 0;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (strlen(socket_path) >= sizeof(addr.sun_path)) {
        BFLBWIFID_LOGE("Socket path too long: %s", socket_path);
        close(fd);
        return -1;
    }
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
        close(fd);
        return 1;
    }

    saved_errno = errno;
    close(fd);

    return saved_errno != ENOENT && saved_errno != ECONNREFUSED;
}

int bflbwifid_instance_prepare_startup(const bflbwifid_paths_t *paths)
{
    pid_t existing_pid;
    bool pid_file_present;
    bool process_alive;
    int socket_active;
    int attempt;
    const char *pid_file = paths_pid_file(paths);
    const char *socket_path = paths_socket_path(paths);

    for (attempt = 0; attempt < STARTUP_HANDOFF_RETRY_COUNT; attempt++) {
        pid_file_present = (read_pid_file(paths, &existing_pid) == 0);
        process_alive = pid_file_present &&
                        existing_pid != getpid() &&
                        process_is_alive(existing_pid);
        socket_active = socket_path_is_active(paths);

        if (socket_active < 0) {
            return -1;
        }

        if (!process_alive && !socket_active) {
            break;
        }

        if (attempt == 0) {
            if (process_alive) {
                BFLBWIFID_LOGW("Previous bflbwifid instance (PID=%d) is still exiting, waiting for handoff",
                               existing_pid);
            } else {
                BFLBWIFID_LOGW("Previous bflbwifid socket %s is still active, waiting for handoff",
                               socket_path);
            }
        }

        usleep(STARTUP_HANDOFF_RETRY_DELAY_US);
    }

    pid_file_present = (read_pid_file(paths, &existing_pid) == 0);
    process_alive = pid_file_present &&
                    existing_pid != getpid() &&
                    process_is_alive(existing_pid);

    if (process_alive) {
        BFLBWIFID_LOGE("Another bflbwifid instance is already running (PID=%d)", existing_pid);
        return -1;
    }

    if (pid_file_present) {
        if (unlink(pid_file) != 0 && errno != ENOENT) {
            BFLBWIFID_LOGW("Failed to remove stale PID file %s: %s", pid_file, strerror(errno));
        }
    }

    socket_active = socket_path_is_active(paths);
    if (socket_active < 0) {
        return -1;
    }

    if (socket_active) {
        BFLBWIFID_LOGE("Another bflbwifid instance already owns %s", socket_path);
        return -1;
    }

    return 0;
}

int bflbwifid_instance_write_pid_file(const bflbwifid_paths_t *paths)
{
    char buf[32];
    char tmp_path[256];
    int fd;
    pid_t pid;
    const char *pid_file = paths_pid_file(paths);

    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp.%d", pid_file, (int)getpid());

    fd = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        BFLBWIFID_LOGE("Failed to create PID file: %s", strerror(errno));
        return -1;
    }

    pid = getpid();
    snprintf(buf, sizeof(buf), "%d\n", pid);
    if (write(fd, buf, strlen(buf)) < 0) {
        BFLBWIFID_LOGE("Failed to write PID file: %s", strerror(errno));
        close(fd);
        unlink(tmp_path);
        return -1;
    }

    close(fd);

    if (rename(tmp_path, pid_file) != 0) {
        BFLBWIFID_LOGE("Failed to install PID file: %s", strerror(errno));
        unlink(tmp_path);
        return -1;
    }

    if (record_path_identity(pid_file, &g_pid_identity) != 0) {
        BFLBWIFID_LOGW("Failed to record PID file identity");
        g_pid_identity.valid = false;
    }

    BFLBWIFID_LOGI("PID file created: %s (PID=%d)", pid_file, pid);

    return 0;
}

int bflbwifid_instance_create_server(const bflbwifid_paths_t *paths)
{
    int server_fd;
    int reuse = 1;
    struct sockaddr_un addr;
    const char *socket_path = paths_socket_path(paths);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        BFLBWIFID_LOGE("Failed to create socket: %s", strerror(errno));
        return -1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (strlen(socket_path) >= sizeof(addr.sun_path)) {
        BFLBWIFID_LOGE("Socket path too long: %s", socket_path);
        close(server_fd);
        return -1;
    }
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (access(socket_path, F_OK) == 0 &&
        unlink(socket_path) != 0 && errno != ENOENT) {
        BFLBWIFID_LOGE("Failed to remove stale socket %s: %s", socket_path, strerror(errno));
        close(server_fd);
        return -1;
    }

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        BFLBWIFID_LOGE("Failed to bind socket: %s", strerror(errno));
        close(server_fd);
        return -1;
    }

    chmod(socket_path, 0777);

    if (listen(server_fd, 5) < 0) {
        BFLBWIFID_LOGE("Failed to listen: %s", strerror(errno));
        close(server_fd);
        unlink(socket_path);
        return -1;
    }

    if (record_path_identity(socket_path, &g_socket_identity) != 0) {
        BFLBWIFID_LOGW("Failed to record socket identity");
        g_socket_identity.valid = false;
    }

    BFLBWIFID_LOGI("Unix socket server started: %s", socket_path);

    return server_fd;
}

void bflbwifid_instance_cleanup(const bflbwifid_paths_t *paths, int *server_fd)
{
    const char *socket_path = paths_socket_path(paths);
    const char *pid_file = paths_pid_file(paths);

    BFLBWIFID_LOGI("Cleaning up resources...");

    if (server_fd && *server_fd >= 0) {
        close(*server_fd);
        *server_fd = -1;
    }

    unlink_if_identity_matches(socket_path, &g_socket_identity, "socket");
    unlink_if_identity_matches(pid_file, &g_pid_identity, "PID file");

    BFLBWIFID_LOGI("Cleanup complete");
}
