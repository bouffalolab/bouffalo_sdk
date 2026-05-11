#include "bflbwifid_instance.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include "nethub_log.h"

static int read_pid_file(const char *path, pid_t *pid)
{
    FILE *fp;
    long value;

    if (!path || !pid) {
        return -1;
    }

    fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    if (fscanf(fp, "%ld", &value) != 1 || value <= 0) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    *pid = (pid_t)value;
    return 0;
}

int bflbwifid_instance_prepare_startup(const nethub_paths_t *paths)
{
    pid_t pid;

    if (!paths || !paths->socket_path || !paths->pid_file) {
        return -1;
    }

    if (read_pid_file(paths->pid_file, &pid) == 0) {
        if (kill(pid, 0) == 0 || errno == EPERM) {
            NETHUB_LOGE("bflbwifid is already running (pid=%ld)", (long)pid);
            return -1;
        }
    }

    unlink(paths->socket_path);
    unlink(paths->pid_file);
    return 0;
}

int bflbwifid_instance_write_pid_file(const nethub_paths_t *paths)
{
    FILE *fp;

    if (!paths || !paths->pid_file) {
        return -1;
    }

    fp = fopen(paths->pid_file, "w");
    if (!fp) {
        NETHUB_LOGE("Failed to open pid file %s: %s", paths->pid_file, strerror(errno));
        return -1;
    }

    fprintf(fp, "%ld\n", (long)getpid());
    fclose(fp);
    return 0;
}

int bflbwifid_instance_create_server(const nethub_paths_t *paths)
{
    struct sockaddr_un addr;
    int fd;

    if (!paths || !paths->socket_path || strlen(paths->socket_path) >= sizeof(addr.sun_path)) {
        return -1;
    }

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        NETHUB_LOGE("socket failed: %s", strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, paths->socket_path, strlen(paths->socket_path) + 1u);

    unlink(paths->socket_path);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        NETHUB_LOGE("bind %s failed: %s", paths->socket_path, strerror(errno));
        close(fd);
        return -1;
    }

    if (listen(fd, 8) != 0) {
        NETHUB_LOGE("listen failed: %s", strerror(errno));
        close(fd);
        unlink(paths->socket_path);
        return -1;
    }

    chmod(paths->socket_path, 0666);
    return fd;
}

void bflbwifid_instance_cleanup(const nethub_paths_t *paths, int *server_fd)
{
    if (server_fd && *server_fd >= 0) {
        close(*server_fd);
        *server_fd = -1;
    }
    if (paths) {
        if (paths->socket_path) {
            unlink(paths->socket_path);
        }
        if (paths->pid_file) {
            unlink(paths->pid_file);
        }
    }
}
