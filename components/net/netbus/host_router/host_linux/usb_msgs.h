#ifndef USB_MSGS_H_5YWKJHMO
#define USB_MSGS_H_5YWKJHMO

#ifdef BUILD_USERSPACE
#include <stdbool.h>
#include <stdint.h>
#else
#include <linux/kernel.h>
#endif

#include "rnm_msg.h"

#define TX_LEN_LIMIT 1514

#define USBWIFI_DATA_TYPE_CMD 0xA55A
#define USBWIFI_DATA_TYPE_PKT 0x6996

#define USB_DATA_FLAG_AP_PKT (1u << 0)

typedef struct {
    uint16_t type;
    uint16_t length;
    uint16_t flags;
    uint16_t payload_offset;
    uint32_t rsvd[8];
    uint8_t payload[];
} __attribute__((aligned(4))) usb_data_t;

#endif /* end of include guard: USB_MSGS_H_5YWKJHMO */
