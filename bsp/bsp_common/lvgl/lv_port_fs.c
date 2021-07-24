/**
 * @file lv_port_fs_templ.c
 *
 */

/*Copy this file as "lv_port_fs.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs.h"
#include "ff.h"
#include "stdio.h"
#include "bflb_platform.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Create a type to store the required data about your file.
 * If you are using a File System library
 * it already should have a File type.
 * For example FatFS has `file_t `. In this case use `typedef file_t  file_t`*/
typedef FIL file_t;

/*Similarly to `file_t` create a type for directory reading too */
typedef DIR dir_t;

FATFS FS_OBJ_SD;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static lv_fs_res_t fs_open(lv_fs_drv_t *drv, void *file_p, const char *path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos);
static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p);
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);
static lv_fs_res_t fs_remove(lv_fs_drv_t *drv, const char *path);
static lv_fs_res_t fs_trunc(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_rename(lv_fs_drv_t *drv, const char *oldname, const char *newname);
static lv_fs_res_t fs_free(lv_fs_drv_t *drv, uint32_t *total_p, uint32_t *free_p);
static lv_fs_res_t fs_dir_open(lv_fs_drv_t *drv, void *rddir_p, const char *path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p);

extern void fatfs_sd_driver_register(void);

/**********************
 *  STATIC VARIABLES
 **********************/
typedef uint8_t lv_fs_res_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /* Add a simple drive to open images */
    lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.file_size = sizeof(file_t);
    fs_drv.letter = 'S';
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;
    fs_drv.free_space_cb = fs_free;
    fs_drv.size_cb = fs_size;
    fs_drv.remove_cb = fs_remove;
    fs_drv.rename_cb = fs_rename;
    fs_drv.trunc_cb = fs_trunc;

    fs_drv.rddir_size = sizeof(dir_t);
    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
lv_fs_res_t res_fatfs_to_lv(FRESULT res)
{
    if (res == FR_OK) {
        return LV_FS_RES_OK;
    }

    switch (res) {
        case (FR_DISK_ERR):
            res = LV_FS_RES_HW_ERR;
            break;

        case (FR_NO_FILE):
            res = LV_FS_RES_NOT_EX;
            break;

        case (FR_NO_PATH):
            res = LV_FS_RES_NOT_EX;
            break;

        case (FR_NOT_ENOUGH_CORE):
            res = LV_FS_RES_OUT_OF_MEM;
            break;

        case (FR_LOCKED):
            res = LV_FS_RES_LOCKED;

        case (FR_TOO_MANY_OPEN_FILES):
            res = LV_FS_RES_LOCKED;
            break;

        case (FR_NO_FILESYSTEM):
            res = LV_FS_RES_FS_ERR;
            break;

        case (FR_WRITE_PROTECTED):
            res = LV_FS_RES_DENIED;
            break;

        case (FR_TIMEOUT):
            res = LV_FS_RES_TOUT;
            break;

        default:
            res = LV_FS_RES_UNKNOWN;
    }

    return res;
}

/* Initialize your Storage device and File system. */
static void fs_init(void)
{
    /*E.g. for FatFS initialize the SD card and FatFS itself*/

    /*You code here*/
    fatfs_sd_driver_register();
    f_mount(&FS_OBJ_SD, "sd:", 1);
}

/**
 * Open a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_open(lv_fs_drv_t *drv, void *file_p, const char *path, lv_fs_mode_t mode)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    BYTE fatfs_mode;
    char *path_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_buf = (char *)lv_mem_alloc(sizeof(char) * (strlen(path) + 4));
            sprintf(path_buf, "SD:/%s", path);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    switch (mode) {
        case (LV_FS_MODE_RD):
            fatfs_mode = FA_READ;
            break;

        case (LV_FS_MODE_WR):
            fatfs_mode = FA_WRITE;
            break;

        case (LV_FS_MODE_WR | LV_FS_MODE_RD):
            fatfs_mode = FA_WRITE | FA_READ;
            break;

        default:
            fatfs_mode = LV_FS_MODE_RD;
            break;
    }

    res = f_open((file_t *)file_p, path_buf, fatfs_mode);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_buf);
    return res;
}

/**
 * Close an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_close((file_t *)file_p);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Read data from an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_read((file_t *)file_p, buf, btr, br);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Write into a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_write((file_t *)file_p, buf, btw, bw);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_lseek((file_t *)file_p, pos);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Give the size of a file bytes
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param size pointer to a variable to store the size
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p)
{
    //lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    *size_p = f_size((file_t *)file_p);
    return LV_FS_RES_OK;
}
/**
 * Give the position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    //lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    *pos_p = f_tell((file_t *)file_p);
    return LV_FS_RES_OK;
}

/**
 * Delete a file
 * @param drv pointer to a driver where this function belongs
 * @param path path of the file to delete
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_remove(lv_fs_drv_t *drv, const char *path)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    char *path_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_buf = (char *)lv_mem_alloc(sizeof(char) * (strlen(path) + 4));
            sprintf(path_buf, "SD:/%s", path);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_unlink(path_buf);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_buf);
    return res;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_fs_open )
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_trunc(lv_fs_drv_t *drv, void *file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_truncate((file_t *)file_p);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Rename a file
 * @param drv pointer to a driver where this function belongs
 * @param oldname path to the file
 * @param newname path with the new name
 * @return LV_FS_RES_OK or any error from 'fs_res_t'
 */
static lv_fs_res_t fs_rename(lv_fs_drv_t *drv, const char *oldname, const char *newname)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    char *path_old_buf = NULL;
    char *path_new_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_old_buf = (char *)lv_mem_alloc(sizeof(char) * (strlen(oldname) + 4));
            path_new_buf = (char *)lv_mem_alloc(sizeof(char) * (strlen(newname) + 4));
            sprintf(path_old_buf, "SD:/%s", oldname);
            sprintf(path_new_buf, "SD:/%s", newname);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_rename(path_old_buf, path_new_buf);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_old_buf);
    lv_mem_free(path_new_buf);
    return res;
}

/**
 * Get the free and total size of a driver in kB
 * @param drv pointer to a driver where this function belongs
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_free(lv_fs_drv_t *drv, uint32_t *total_p, uint32_t *free_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    char *path = NULL;
    FATFS *fs_obj;

    /* Add your code here*/
    switch (drv->letter) {
        case 'S':
            path = "SD:";
            fs_obj = &FS_OBJ_SD;
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_getfree(path, free_p, &fs_obj);
    *free_p = (fs_obj->csize) * (*free_p) / 1024;
    *total_p = (fs_obj->csize) * (fs_obj->n_fatent - 2) / 1024;

    res = res_fatfs_to_lv(res);
    return res;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv pointer to a driver where this function belongs
 * @param rddir_p pointer to a 'lv_fs_dir_t' variable
 * @param path path to a directory
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_open(lv_fs_drv_t *drv, void *rddir_p, const char *path)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    char *path_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_buf = (char *)lv_mem_alloc(sizeof(char) * (strlen(path) + 4));
            sprintf(path_buf, "SD:/%s", path);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_opendir(rddir_p, path_buf);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_buf);
    return res;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv pointer to a driver where this function belongs
 * @param rddir_p pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    FILINFO entry;

    /* Add your code here*/
    res = f_readdir(rddir_p, &entry);
    res = res_fatfs_to_lv(res);

    if (res == LV_FS_RES_OK) {
        sprintf(fn, "%s%s", (entry.fattrib & AM_DIR) ? "/" : "", entry.fname);
    }

    return res;
}

/**
 * Close the directory reading
 * @param drv pointer to a driver where this function belongs
 * @param rddir_p pointer to an initialized 'lv_fs_dir_t' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    f_closedir((dir_t *)rddir_p);
    res = res_fatfs_to_lv(res);
    return res;
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
