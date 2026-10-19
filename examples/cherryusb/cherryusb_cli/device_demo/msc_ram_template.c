/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file msc_ram_template.c
 * @brief CherryUSB mass-storage device backed by a RAM disk.
 */

#include "usbd_core.h"
#include "usbd_msc.h"

/** @name MSC device configuration
 * @{ */
#define MSC_IN_EP       0x81
#define MSC_OUT_EP      0x02

#define USBD_VID        0xFFFF
#define USBD_PID        0xFFFF
#define USBD_MAX_POWER  100

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif
/** @} */

/** @brief USB device descriptor for the RAM-disk MSC device. */
static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01)
};

/** @brief USB configuration descriptor for the RAM-disk MSC device. */
static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02)
};

/** @brief USB device-qualifier descriptor. */
static const uint8_t device_quality_descriptor[] = {
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};

/** @brief USB string descriptors. */
static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB_MSC_DEMO",         /* Product */
    "2022123456",                 /* Serial Number */
};

/** @brief Return the device descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @return Device descriptor address.
 */
static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

/** @brief Return the configuration descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @return Configuration descriptor address.
 */
static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    return config_descriptor;
}

/** @brief Return the device-qualifier descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @return Device-qualifier descriptor address.
 */
static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

/** @brief Return a USB string descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @param[in] index String descriptor index.
 * @return Descriptor string, or NULL when the index is unsupported.
 */
static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index > 3) {
        return NULL;
    }
    return string_descriptors[index];
}

/** @brief Descriptor callback table for the RAM-disk MSC device. */
const struct usb_descriptor msc_ram_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};

/** @brief Handle USB device lifecycle events.
 * @param[in] busid USB device-controller index.
 * @param[in] event CherryUSB device event identifier.
 */
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            USB_LOG_INFO("MSC configured done\r\n");
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

/** @name RAM-disk geometry
 * @{ */
#define BLOCK_SIZE  512
#define BLOCK_COUNT 128
/** @} */

/** @brief Storage for one logical RAM-disk block. */
typedef struct
{
    uint8_t BlockSpace[BLOCK_SIZE]; /**< Block payload bytes. */
} BLOCK_TYPE;

/** @brief Complete backing store for the RAM disk. */
BLOCK_TYPE mass_block[BLOCK_COUNT];

/** @brief Report the RAM-disk capacity.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] lun Logical unit number; unused.
 * @param[out] block_num Receives the logical block count.
 * @param[out] block_size Receives the block size in bytes.
 */
void usbd_msc_get_cap(uint8_t busid, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
    *block_num = BLOCK_COUNT; //Pretend having so many buffer,not has actually.
    *block_size = BLOCK_SIZE;
}
/** @brief Read bytes from a RAM-disk sector.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] lun Logical unit number; unused.
 * @param[in] sector Sector index to read.
 * @param[out] buffer Destination buffer.
 * @param[in] length Number of bytes to copy.
 * @retval 0 Request processed; an out-of-range sector is logged.
 */
int usbd_msc_sector_read(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    if (sector < BLOCK_COUNT) {
        memcpy(buffer, mass_block[sector].BlockSpace, length);
    } else {
        USB_LOG_ERR("Read sector out of range\r\n");
    }
    return 0;
}

/** @brief Write bytes to a RAM-disk sector.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] lun Logical unit number; unused.
 * @param[in] sector Sector index to write.
 * @param[in] buffer Source buffer.
 * @param[in] length Number of bytes to copy.
 * @retval 0 Request processed; an out-of-range sector is logged.
 */
int usbd_msc_sector_write(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    if (sector < BLOCK_COUNT) {
        memcpy(mass_block[sector].BlockSpace, buffer, length);
    } else {
        USB_LOG_ERR("Write sector out of range\r\n");
    }
    return 0;
}

static struct usbd_interface intf0;

/** @brief Initialize the RAM-disk MSC device.
 * @param[in] busid USB device-controller index.
 * @param[in] reg_base USB controller register base.
 */
void msc_ram_init(uint8_t busid, uintptr_t reg_base)
{
    usbd_desc_register(busid, &msc_ram_descriptor);
    usbd_add_interface(busid, usbd_msc_init_intf(busid, &intf0, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

#if defined(CONFIG_USBDEV_MSC_POLLING)
/** @brief Service the MSC class in polling mode.
 * @param[in] busid USB device-controller index.
 */
void msc_ram_polling(uint8_t busid)
{
    usbd_msc_polling(busid);
}
#endif