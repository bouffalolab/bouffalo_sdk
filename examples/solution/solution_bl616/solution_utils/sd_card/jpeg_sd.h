#ifndef __JPEG_SD_H_
#define __JPEG_SD_H_

#include "ff.h"

extern FATFS g_fatfs_fs;

int fatfs_sd_init(void);
int save_jpeg_to_sdcard_init(void);

int filedir_delete(char *path);

#endif /* __JPEG_SD_H_ */