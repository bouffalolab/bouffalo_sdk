#ifndef NETHUB_PATHS_H
#define NETHUB_PATHS_H

#include "nethub_wifi_types.h"

typedef struct {
    const char *socket_path;
    const char *pid_file;
    const char *log_file;
} nethub_paths_t;

#endif /* NETHUB_PATHS_H */
