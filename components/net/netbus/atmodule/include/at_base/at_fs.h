#ifndef __AT_FS_H__
#define __AT_FS_H__

#include <stdio.h>

typedef void *at_dir_t;

#define AT_FILE_NAME_MAX 128 

struct at_direct {
    uint8_t d_type;

    size_t d_size;

    char d_name[AT_FILE_NAME_MAX+1];
};

int at_fs_open(const char *path, int flags);

int at_fs_close(int fd);

int at_fs_remove(const char *path);

size_t at_fs_read(int fd, void *ptr, size_t size);

size_t at_fs_write(int fd, const void *ptr, size_t size);

size_t at_fs_lseek(int fd, _off_t offset, int whence);

at_dir_t at_fs_opendir(const char *path);

int at_fs_readdir(at_dir_t dirp, struct at_direct *res);

int at_fs_closedir(at_dir_t dir);

int at_load_file(const char *path, char **buf, int *n);

int at_filesize_get(const char *ph);
int at_read_file(const char *ph, int offset, char *buf, int len);
int at_write_file(const char *ph, int offset, const char *buf, int len);

int at_fs_register(void);

#endif
