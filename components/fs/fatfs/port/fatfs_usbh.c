#include "ff.h"
#include "diskio.h"
#include "usbh_core.h"
#include "usbh_msc.h"

#include "bflb_l1c.h"

struct usbh_msc *active_msc_class;

int USB_disk_status(void)
{
    return 0;
}

int USB_disk_initialize(void)
{
    if (active_msc_class == NULL) {
        printf("Fatfs USBH MSC class is NULL!\r\n");
        return RES_NOTRDY;
    }
    return RES_OK;
}

int USB_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    bflb_l1c_dcache_clean_range((uint8_t *)buff, 1);
    bflb_l1c_dcache_clean_range((uint8_t *)buff + active_msc_class->blocksize * count - 1, 1);
    bflb_l1c_dcache_invalidate_range((uint8_t *)buff, active_msc_class->blocksize * count);

    if (usbh_msc_scsi_read10(active_msc_class, sector, buff, count) < 0) {
        return RES_ERROR;
    }

    bflb_l1c_dcache_invalidate_range((uint8_t *)buff, active_msc_class->blocksize * count);

    return RES_OK;
}

int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    bflb_l1c_dcache_clean_range((uint8_t *)buff, active_msc_class->blocksize * count);

    if (usbh_msc_scsi_write10(active_msc_class, sector, buff, count) < 0) {
        return RES_ERROR;
    }

    return RES_OK;
}

int USB_disk_ioctl(BYTE cmd, void *buff)
{
    int result = 0;

    switch (cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = active_msc_class->blocksize;
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = active_msc_class->blocknum;
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

DSTATUS USB_Translate_Result_Code(int result)
{
    return result;
}

void fatfs_usbh_driver_register(struct usbh_msc *msc_class)
{
    FATFS_DiskioDriverTypeDef USBH_DiskioDriver = { NULL };

    USBH_DiskioDriver.disk_status = USB_disk_status;
    USBH_DiskioDriver.disk_initialize = USB_disk_initialize;
    USBH_DiskioDriver.disk_write = USB_disk_write;
    USBH_DiskioDriver.disk_read = USB_disk_read;
    USBH_DiskioDriver.disk_ioctl = USB_disk_ioctl;
    USBH_DiskioDriver.error_code_parsing = USB_Translate_Result_Code;

    active_msc_class = msc_class;
    disk_driver_callback_init(DEV_USB, &USBH_DiskioDriver);
}
