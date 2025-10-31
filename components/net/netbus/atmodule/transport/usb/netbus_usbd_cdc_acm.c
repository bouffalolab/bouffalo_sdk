#include "bflb_mtimer.h"

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_ecm.h"
#include "netbus_usbd_cdc_acm.h"

#define DBG_TAG "ECM"
#include "log.h"

#define CDC_ACM_EVENT_TX    0x1

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[512];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[512];
static netbus_usb_cdc_t *gpctx = NULL;

extern volatile uint8_t g_cdc_acm_init_done;
/* usb event handler */
void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            LOG_W("USBD_EVENT_RESET\r\n");
            //ecm_rx_busy_flag = false;
            //ecm_tx_busy_flag = false;
            break;
        case USBD_EVENT_CONNECTED:
            LOG_W("USBD_EVENT_CONNECTED\r\n");
            break;
        case USBD_EVENT_DISCONNECTED:
            //usb_ecm_ready_flag = false;
            //emac_link_up_flag = false;
            LOG_W("USBD_EVENT_DISCONNECTED\r\n");
            break;
        case USBD_EVENT_RESUME:
            LOG_W("USBD_EVENT_RESUME\r\n");
            //usb_ecm_ready_flag = true;
            break;
        case USBD_EVENT_SUSPEND:
            LOG_W("USBD_EVENT_SUSPEND\r\n");
            //usb_ecm_ready_flag = false;
            //emac_link_up_flag = false;
            break;
        case USBD_EVENT_CONFIGURED:
            LOG_W("USBD_EVENT_CONFIGURED\r\n");
            g_cdc_acm_init_done = 1;
            /* setup first out ep read transfer */
            usbd_ep_start_read(0, CDC_ACM_OUT_EP, read_buffer, sizeof(read_buffer));
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            LOG_W("USBD_EVENT_SET_REMOTE_WAKEUP\r\n");
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            LOG_W("USBD_EVENT_CLR_REMOTE_WAKEUP\r\n");
            break;

        default:
            break;
    }

    if (gpctx && gpctx->cb) {
        gpctx->cb(gpctx->arg, event);
    }
}

static void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xStreamBufferSendFromISR(gpctx->rxbuf, read_buffer, nbytes, &xHigherPriorityTaskWoken);
#if 0
    printf("%s, buf:[%d] \r\n", __func__, nbytes);
    for  (int i = 0; i < nbytes; i++) {
        printf(" %02X", read_buffer[i]);
    }
    printf("\r\n");
#endif
#if 0
    printf("nbytes:%d\r\n", nbytes);
    for (int i = 0; i < nbytes; i++) {
        putchar(read_buffer[i]);
    }
#endif
    /* setup next out ep read transfer */
    usbd_ep_start_read(0, CDC_ACM_OUT_EP, read_buffer, sizeof(read_buffer));

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(0, CDC_ACM_IN_EP, NULL, 0);
    }

    xEventGroupSetBitsFromISR(gpctx->event, CDC_ACM_EVENT_TX, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*!< endpoint call back */
static struct usbd_endpoint cdc_acm_out_ep = {
    .ep_addr = CDC_ACM_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

static struct usbd_endpoint cdc_acm_in_ep = {
    .ep_addr = CDC_ACM_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

static struct usbd_interface intf2;
static struct usbd_interface intf3;

static void cdc_acm_init(void)
{
    /* CDC ACM init */
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf2));
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf3));
    usbd_add_endpoint(0, &cdc_acm_out_ep);
    usbd_add_endpoint(0, &cdc_acm_in_ep);
}

int netbus_usb_cdcacm_init(netbus_usb_cdc_t *pctx, uint32_t txbuf_size, uint32_t rxbuf_size)
{
    gpctx = pctx;
    pctx->event   = xEventGroupCreate();
    pctx->w_mutex = xSemaphoreCreateMutex();
	pctx->rxbuf   = xStreamBufferCreate(rxbuf_size, 1);
    cdc_acm_init();
    xEventGroupSetBits(gpctx->event, CDC_ACM_EVENT_TX);
	return 0;
}

int netbus_usb_cdcacm_callback_set(netbus_usb_cdc_t *pctx, netbus_usb_cdc_func_t pfunc, void *arg)
{
    pctx->cb = pfunc;
    pctx->arg = arg;

    return 0;
}

int netbus_usb_cdcacm_send(netbus_usb_cdc_t *pctx, const uint8_t *p_data, uint32_t len, uint32_t timeout)
{
    int ret = 0;
	int send_len = 0;
	uint32_t remain_len = len;

    if (xSemaphoreTake(pctx->w_mutex, portMAX_DELAY) != pdTRUE) {
		return -1;
	}
	while (remain_len) {

        xEventGroupWaitBits(pctx->event, CDC_ACM_EVENT_TX, pdTRUE, pdTRUE, timeout);

        ret = (remain_len > sizeof(write_buffer)) ? sizeof(write_buffer) : remain_len;
        memcpy(write_buffer, p_data, ret);

		send_len += ret;
		remain_len -= ret;
        usbd_ep_start_write(0, CDC_ACM_IN_EP, write_buffer, ret);

        //for (int i = 0; i < ret; i++) {
        //    putchar(write_buffer[i]);
        //}
	}
	xSemaphoreGive(pctx->w_mutex);
	return send_len;
}

int netbus_usb_cdcacm_receive(netbus_usb_cdc_t *pctx, uint8_t *p_buffer, uint32_t buf_len, uint32_t timeout)
{
	return xStreamBufferReceive(pctx->rxbuf, p_buffer, buf_len, timeout);
}

int netbus_usb_cdcacm_deinit(netbus_usb_cdc_t *ctx)
{
	vStreamBufferDelete(ctx->rxbuf);
    vSemaphoreDelete(ctx->w_mutex);
    vEventGroupDelete(ctx->event);
	return 0;
}


