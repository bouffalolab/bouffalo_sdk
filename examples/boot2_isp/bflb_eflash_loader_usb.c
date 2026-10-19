/**
  ******************************************************************************
  * @file    bflb_sp_eflash_loader_usb.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include "bflb_sp_port.h"
#if BFLB_SP_BOOT2_SUPPORT_USB_IAP
#include "bflb_eflash_loader.h"
#include "bflb_sp_common.h"
#include "xz_config.h"

#include "usbd_core.h"
#include "usbd_cdc_acm.h"

/*!< endpoint address */
#define CDC_IN_EP          0x81
#define CDC_OUT_EP         0x02
#define CDC_INT_EP         0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE    (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

#ifdef CONFIG_USBDEV_ADVANCE_DESC
static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02)
};

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

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB_CDC_DEMO",         /* Product */
    "2022123456",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    return config_descriptor;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index > 3) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor cdc_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};
#else
/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    '_', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    '_', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
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
#endif
    0x00
};
#endif
static volatile bool usb_init_done_flag = false;
static volatile bool usb_handshake_flag = false;
static volatile bool usb_acm_ready_flag = false;
static volatile bool acm_tx_busy_flag = false;
static volatile bool acm_rx_busy_flag = false;

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usb_out_buff[CDC_MAX_MPS];

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            usb_acm_ready_flag = false;
            acm_tx_busy_flag = false;
            acm_rx_busy_flag = false;
            usb_handshake_flag = false;
            USB_LOG_INFO("CDC_ACM USBD_EVENT_RESET\r\n");
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            usb_acm_ready_flag = true;
            USB_LOG_INFO("CDC_ACM USBD_EVENT_RESUME\r\n");
            break;
        case USBD_EVENT_SUSPEND:
            usb_acm_ready_flag = false;
            acm_tx_busy_flag = false;
            acm_rx_busy_flag = false;
            usb_handshake_flag = false;
            USB_LOG_INFO("CDC_ACM USBD_EVENT_SUSPEND\r\n");
            break;
        case USBD_EVENT_CONFIGURED:
            usb_acm_ready_flag = true;
            USB_LOG_INFO("CDC_ACM EVENT_CONFIGURED done\r\n");
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;
        default:
            break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes);
void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes);

/*!< endpoint call back */
struct usbd_endpoint cdc_acm_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_acm_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;

void cdc_acm_init(uint8_t busid, uintptr_t reg_base)
{
#ifdef CONFIG_USBDEV_ADVANCE_DESC
    usbd_desc_register(busid, &cdc_descriptor);
#else
    usbd_desc_register(busid, cdc_descriptor);
#endif
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(0, &intf0));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(0, &intf1));
    usbd_add_endpoint(busid, &cdc_acm_out_ep);
    usbd_add_endpoint(busid, &cdc_acm_in_ep);
    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if (usb_acm_ready_flag == false) {
        return;
    }

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, CDC_IN_EP, NULL, 0);
    } else {
        acm_tx_busy_flag = false;
        // USB_LOG_RAW("upld done\r\n");
    }
}

void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    BOOT2_MSG_DBG("usb rx %d, rx_buf_len %d\r\n", nbytes, g_rx_buf_len);

    if (usb_handshake_flag == false) {
        g_rx_buf_len = nbytes;
        acm_rx_busy_flag = false;
        return;
    }

    uint16_t datalen;
    uint8_t *buf = g_eflash_loader_readbuf[g_rx_buf_index];

    if (g_rx_buf_len + nbytes > BFLB_EFLASH_LOADER_READBUF_SIZE) {
        nbytes = BFLB_EFLASH_LOADER_READBUF_SIZE - g_rx_buf_len;
    }
    arch_memcpy(buf + g_rx_buf_len, usb_out_buff, nbytes);
    g_rx_buf_len += nbytes;

    if (g_rx_buf_len < 4) {
        goto continue_exit;
    }

    datalen = buf[2] + (buf[3] << 8) + 4;
    if (g_rx_buf_len < datalen) {
        goto continue_exit;
    }

    acm_rx_busy_flag = false;
    return;

continue_exit:
    if (usb_acm_ready_flag == false) {
        return;
    }
    acm_rx_busy_flag = true;
    usbd_ep_start_read(0, CDC_OUT_EP, usb_out_buff, CDC_MAX_MPS);
    return;
}

int32_t bflb_eflash_loader_usb_init()
{
    hal_boot2_debug_usb_port_init();

    if (usb_init_done_flag == true) {
        usb_handshake_flag = false;
        return BFLB_EFLASH_LOADER_SUCCESS;
    }

    simple_malloc_init(g_malloc_buf, sizeof(g_malloc_buf));
    g_eflash_loader_readbuf[0] = vmalloc(BFLB_EFLASH_LOADER_READBUF_SIZE);
    g_eflash_loader_readbuf[1] = vmalloc(BFLB_EFLASH_LOADER_READBUF_SIZE);
    arch_memset(g_eflash_loader_readbuf[0], 0, BFLB_EFLASH_LOADER_READBUF_SIZE);
    arch_memset(g_eflash_loader_readbuf[1], 0, BFLB_EFLASH_LOADER_READBUF_SIZE);
    g_rx_buf_index = 0;
    g_rx_buf_len = 0;

    usb_handshake_flag = false;
    usb_acm_ready_flag = false;
    acm_tx_busy_flag = false;
    acm_rx_busy_flag = false;

    cdc_acm_init(0, 0);

    usb_init_done_flag = true;
    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_usb_deinit()
{
    // usbd_deinitialize(0);
    return BFLB_EFLASH_LOADER_SUCCESS;
}

void usb_send(uint8_t *buf, uint32_t len)
{
    if (usb_acm_ready_flag == false) {
        return;
    }
    // Print buffer content for debugging
    // for (uint32_t i = 0; i < len; i++) {
    //     BOOT2_MSG_DBG("%x ", buf[i]);
    // }
    // BOOT2_MSG_DBG("\r\n");

    bflb_l1c_dcache_clean_range(buf, len);
    acm_tx_busy_flag = true;
    usbd_ep_start_write(0, CDC_IN_EP, buf, len);

    while (acm_tx_busy_flag == true && usb_acm_ready_flag == true) {
        // Wait for the transfer to complete
    }
}

int32_t bflb_eflash_loader_usb_send(uint32_t *data, uint32_t len)
{
    usb_send((uint8_t *)data, len);

    BOOT2_MSG_DBG("usb send %d\r\n", len);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_usb_wait_tx_idle(uint32_t timeout)
{
    // while (acm_tx_busy_flag == true && usb_acm_ready_flag == true) {
    //     // Wait for the transfer to complete
    // }
    return BFLB_EFLASH_LOADER_SUCCESS;
}

uint32_t ATTR_TCM_SECTION *bflb_eflash_loader_usb_recv(uint32_t *recv_len, uint32_t maxlen, uint32_t timeout)
{
    uint32_t *ret_addr = NULL;
    uint16_t datalen = 0;
    uint32_t time_now, time_start;
    uint8_t *buf = (uint8_t *)g_eflash_loader_readbuf[g_rx_buf_index];

    BOOT2_MSG_DBG("usb_recv START\r\n");

    time_start = bflb_mtimer_get_time_ms();

    while (1) {
        /* check if timeout */
        time_now = bflb_mtimer_get_time_ms();
        if (time_now - time_start > timeout) {
            *recv_len = 0;
            return NULL;
        }

        /* check if USB is ready */
        if (usb_acm_ready_flag == false || usb_handshake_flag == false) {
            *recv_len = 0;
            return NULL;
        }
        /* wait for data */
        if (acm_rx_busy_flag == false) {
            break;
        }
    }

    /* check if data is valid */
    datalen = buf[2] + (buf[3] << 8);

    if (datalen + 4 < g_rx_buf_len || datalen > BFLB_EFLASH_LOADER_CMD_DATA_MAX_LEN) {
        *recv_len = 0;
        ret_addr = NULL;
    } else {
        *recv_len = datalen + 4;
        ret_addr = (uint32_t *)buf;
    }

    if (ret_addr != NULL) {
        BOOT2_MSG_DBG("usb recv %d, rx_buf_len %d\r\n", *recv_len, g_rx_buf_len);
    }

    /* move on to next buffer */
    g_rx_buf_index = (g_rx_buf_index + 1) % 2;
    g_rx_buf_len = 0;
    acm_rx_busy_flag = true;
    usbd_ep_start_read(0, CDC_OUT_EP, usb_out_buff, CDC_MAX_MPS);

    return ret_addr;
}

int32_t bflb_eflash_loader_check_handshake_buf(uint8_t *buf, uint32_t len)
{
    if (len == 0)
        return -1;

    for (int i = 0; i < len; i++) {
        if (buf[i] != BFLB_EFLASH_LOADER_HAND_SHAKE_BYTE)
            return -1;
    }
    return 0;
}

int32_t bflb_eflash_loader_usb_handshake_poll(uint32_t timeout)
{
    uint32_t time_now, time_start;

    BOOT2_MSG_DBG("usb iap handshake poll\r\n");

    usb_handshake_flag = false;

    time_start = bflb_mtimer_get_time_ms();

    while (1) {
        /* check if timeout*/
        time_now = bflb_mtimer_get_time_ms();
        if (time_now - time_start > timeout) {
            return -1;
        }

        /* check if USB is ready */
        if (usb_acm_ready_flag == false) {
            continue;
        }

        if (acm_rx_busy_flag == false) {
            if (g_rx_buf_len > 4 && bflb_eflash_loader_check_handshake_buf(usb_out_buff, g_rx_buf_len) == 0) {
                BOOT2_MSG_DBG("iap handshake %d 0x55 rcv\r\n", g_rx_buf_len);
                goto success_exit;
            } else {
                if (g_rx_buf_len > 0) {
                    BOOT2_MSG_DBG("iap handshake err, continue...\r\n");
                }
                g_rx_buf_len = 0;
                acm_rx_busy_flag = true;
                usbd_ep_start_read(0, CDC_OUT_EP, usb_out_buff, CDC_MAX_MPS);
                continue;
            }
        }
    }

success_exit:
    usb_out_buff[0] = 'O';
    usb_out_buff[1] = 'K';
    usb_send(usb_out_buff, 2);

    usb_handshake_flag = true;
    g_rx_buf_len = 0;
    acm_rx_busy_flag = true;
    usbd_ep_start_read(0, CDC_OUT_EP, usb_out_buff, CDC_MAX_MPS);
    return 0;
}

#endif
