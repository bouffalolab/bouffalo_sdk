#ifndef BFLBWIFID_INSTANCE_H
#define BFLBWIFID_INSTANCE_H

#include "nethub_paths.h"

int bflbwifid_instance_prepare_startup(const nethub_paths_t *paths);
int bflbwifid_instance_write_pid_file(const nethub_paths_t *paths);
int bflbwifid_instance_create_server(const nethub_paths_t *paths);
void bflbwifid_instance_cleanup(const nethub_paths_t *paths, int *server_fd);

#endif /* BFLBWIFID_INSTANCE_H */
