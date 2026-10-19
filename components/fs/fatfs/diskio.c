/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "stddef.h"

FATFS_DiskioDriverTypeDef pDiskioDriver[DEV_NUM_MAX] = { NULL };

/*-----------------------------------------------------------------------*/
/* init driver callback                                                  */
/*-----------------------------------------------------------------------*/
void disk_driver_callback_init(uint8_t pdrv, FATFS_DiskioDriverTypeDef *pNewDriver)
{
    if (pdrv < DEV_NUM_MAX) {
        pDiskioDriver[pdrv] = *pNewDriver;
    }
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    if (pdrv > DEV_NUM_MAX) {
        return STA_NOINIT;
    }

    if (pDiskioDriver[pdrv].disk_status) {
        result = pDiskioDriver[pdrv].disk_status();
    }

    /* translate the reslut code here */
    if (pDiskioDriver[pdrv].error_code_parsing) {
        stat = pDiskioDriver[pdrv].error_code_parsing(result);
    } else {
        stat = result;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    if (pdrv > DEV_NUM_MAX) {
        return STA_NOINIT;
    }

    if (pDiskioDriver[pdrv].disk_initialize) {
        result = pDiskioDriver[pdrv].disk_initialize();
    }

    /* translate the reslut code here */
    if (pDiskioDriver[pdrv].error_code_parsing) {
        stat = pDiskioDriver[pdrv].error_code_parsing(result);
    } else {
        stat = result;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    if (pdrv > DEV_NUM_MAX) {
        return STA_NOINIT;
    }

    if (pDiskioDriver[pdrv].disk_read) {
        result = pDiskioDriver[pdrv].disk_read(buff, sector, count);
    }

    /* translate the reslut code here */
    if (pDiskioDriver[pdrv].error_code_parsing) {
        stat = pDiskioDriver[pdrv].error_code_parsing(result);
    } else {
        stat = result;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    LBA_t sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to write */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    if (pdrv > DEV_NUM_MAX) {
        return STA_NOINIT;
    }

    if (pDiskioDriver[pdrv].disk_write) {
        result = pDiskioDriver[pdrv].disk_write(buff, sector, count);
    }

    /* translate the reslut code here */
    if (pDiskioDriver[pdrv].error_code_parsing) {
        stat = pDiskioDriver[pdrv].error_code_parsing(result);
    } else {
        stat = result;
    }

    return stat;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    if (pdrv > DEV_NUM_MAX) {
        return STA_NOINIT;
    }

    if (pDiskioDriver[pdrv].disk_ioctl) {
        result = pDiskioDriver[pdrv].disk_ioctl(cmd, buff);
    }

    /* translate the reslut code here */
    if (pDiskioDriver[pdrv].error_code_parsing) {
        stat = pDiskioDriver[pdrv].error_code_parsing(result);
    } else {
        stat = result;
    }

    return stat;
}
