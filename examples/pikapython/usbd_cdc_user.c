#include "usbd_core.h"
#include "usbd_cdc.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CDC_BUFFER_SIZE (1024 * 8)

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
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
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
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
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[USB_CDC_BUFFER_SIZE];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[USB_CDC_BUFFER_SIZE];

struct circular_queue {
    uint8_t buffer[USB_CDC_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
};

struct circular_queue read_queue = {0};
struct circular_queue write_queue = {0};

volatile bool ep_tx_busy_flag = false;
volatile bool ep_rx_ready_flag = false;
volatile bool ep_rx_busy_flag = false;

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

void usbd_configure_done_callback(void)
{
    // printf("usbd configure done callback\r\n");
    /* setup first out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, USB_CDC_BUFFER_SIZE);
}

void usbd_cdc_acm_bulk_rx(uint8_t ep, uint32_t nbytes)
{
    /* append to read_queue */
    ep_rx_busy_flag = true;
    // printf("actual out len:%d\r\n", nbytes);

    for (int i = 0; i < nbytes; i++) {
        read_queue.buffer[read_queue.tail] = read_buffer[i];
        read_queue.tail = (read_queue.tail + 1) % USB_CDC_BUFFER_SIZE;
    }

    ep_rx_busy_flag = false;
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, USB_CDC_BUFFER_SIZE);
}

void usbd_cdc_acm_bulk_tx(uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
        // usb_cdc_fflush();
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_rx
};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_tx
};

struct usbd_interface intf0;
struct usbd_interface intf1;

/* function ------------------------------------------------------------------*/
void cdc_acm_init(void)
{
    const uint8_t data[10] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

    memcpy(&write_buffer[0], data, 10);
    memset(&write_buffer[10], 'a', 2038);

    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
    usbd_initialize();
}

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    if (dtr) {
        dtr_enable = 1;
    } else {
        dtr_enable = 0;
    }
}

int usbd_ep_write_sync(uint8_t ep, const void *buf, uint32_t nbytes)
{
    ep_tx_busy_flag = true;
    usbd_ep_start_write(ep, buf, nbytes);
    while (ep_tx_busy_flag) {
    }
    return 0;
}

int usbd_ep_read_sync(uint8_t ep, void *buf, uint32_t nbytes)
{
    memset(buf, 0, nbytes);
    ep_rx_ready_flag = false;
    usbd_ep_start_read(ep, buf, nbytes);
    while (!ep_rx_ready_flag) {
    }
    return 0;
}

int usb_cdc_fflush(void){
    if (ep_tx_busy_flag) {
        return 0;
    }
    /* fflush for write_queue */
    if (write_queue.head == write_queue.tail) {
        return 0;
    }
    int len = write_queue.tail - write_queue.head;
    if (len < 0) {
        len += USB_CDC_BUFFER_SIZE;
    }
    if (len > USB_CDC_BUFFER_SIZE) {
        len -= USB_CDC_BUFFER_SIZE;
    }
    for (int i = 0; i < len; i++) {
        write_buffer[i] = write_queue.buffer[write_queue.head];
        write_queue.head = (write_queue.head + 1) % USB_CDC_BUFFER_SIZE;
    }
    // printf("usb_cdc_fflush:%d\r\n", len);
    ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, write_buffer, len); 
}

int usb_cdc_user_putchar(char ch)
{
    // printf("usb_cdc_user_putchar:%c\r\n", ch);
    /* append to write_queue */
    write_queue.buffer[write_queue.tail] = ch;
    write_queue.tail = (write_queue.tail + 1) % USB_CDC_BUFFER_SIZE;
    if (ch == '\n'){
        usb_cdc_fflush();
    }
    return 0;
}

char usb_cdc_user_getchar(void)
{
    // usbd_ep_read_sync(CDC_OUT_EP, &read_buffer, 1);
    // printf("usb_cdc_user_getchar:%c\r\n", read_buffer[0]);
    char res = 0;

    /* read from read_buffer_user */
    while(1){
        if (ep_rx_busy_flag){
            printf("got ep_rx_busy_flag\r\n");
            continue;
        }
        /* read from read_queue */
        if (read_queue.head != read_queue.tail) {
            res = read_queue.buffer[read_queue.head];
            read_queue.head = (read_queue.head + 1) % USB_CDC_BUFFER_SIZE;
            // printf("got res:%c\r\n", res);
            // printf("read_queue.head:%d\r\n", read_queue.head);
            // printf("read_queue.tail:%d\r\n", read_queue.tail);
            break;
        }
        vTaskDelay(10);
    }
    return res;
}

void cdc_acm_data_send_with_dtr_test(void)
{
    // if (dtr_enable) {
        usbd_ep_read_sync(CDC_OUT_EP, read_buffer, USB_CDC_BUFFER_SIZE);
        usbd_ep_write_sync(CDC_IN_EP, read_buffer, strlen(read_buffer));
    // }
}
