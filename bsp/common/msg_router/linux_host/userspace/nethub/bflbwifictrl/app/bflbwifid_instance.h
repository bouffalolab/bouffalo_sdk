/**
 * @file bflbwifid_instance.h
 * @brief Daemon instance ownership and local IPC endpoint lifecycle
 */

#ifndef BFLBWIFID_INSTANCE_H
#define BFLBWIFID_INSTANCE_H

#include "bflbwifid_paths.h"

int bflbwifid_instance_prepare_startup(const bflbwifid_paths_t *paths);
int bflbwifid_instance_write_pid_file(const bflbwifid_paths_t *paths);
int bflbwifid_instance_create_server(const bflbwifid_paths_t *paths);
void bflbwifid_instance_cleanup(const bflbwifid_paths_t *paths, int *server_fd);

#endif /* BFLBWIFID_INSTANCE_H */
