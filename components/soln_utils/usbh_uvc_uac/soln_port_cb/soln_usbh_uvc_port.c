
#include "bflb_core.h"
#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"

#include "usbh_uvc_stream.h"

#ifdef CONFIG_SOLN_VID_UVC_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_VID_UVC_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_UVC"
#include "log.h"

#define USB_UVC_AUTO_RESET_EN      0
#define USB_UVC_AUTO_RESET_PLUS_EN 0

#define ATTR_FAST_RAM_SECTION      ATTR_TCM_SECTION

/************************************  ************************************ */
static uint32_t s_uvc_total_frame_count = 0;

ATTR_FAST_RAM_SECTION void usbh_video_fps_record(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_uvc_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

uint32_t soln_usbh_video_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_uvc_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_usbh_video_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_uvc_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

#if (!USB_UVC_AUTO_RESET_EN)

void usbh_video_transfer_abort_callback(void)
{
}

#else
/*************************** usbh auto reset *************************** */
static volatile bool usbh_reset_flag = false;
static volatile bool usbh_uvc_run_flag = false;
static struct usb_osal_timer *usbh_stop_timer = NULL;
static struct usb_osal_timer *usbh_start_timer = NULL;
#if USB_UVC_AUTO_RESET_PLUS_EN
static struct usb_osal_timer *uvc_check_timer = NULL;
static volatile uint32_t uvc_check_frame_cnt = 0xffffffff;
#endif

/* uvc abort callback */
ATTR_FAST_RAM_SECTION void usbh_video_transfer_abort_callback(void)
{
    if (usbh_uvc_run_flag == false) {
        LOG_E("uvc is not running\r\n");
        return;
    }

    if (usbh_reset_flag) {
        LOG_E("usbh reset repeat\r\n");
        return;
    }
    usbh_reset_flag = true;

    LOG_E("usbh reset start\r\n");

    if (usbh_stop_timer) {
        usb_osal_timer_start(usbh_stop_timer);
    }
}

/* usbh stop timer callback */
static void usbh_stop_cb(void *argument)
{
    LOG_E("usb stop\r\n");
    usbh_deinitialize(0);

    LOG_E("usb stop done\r\n");

    if (usbh_start_timer) {
        usb_osal_timer_start(usbh_start_timer);
    }
}

/* usbh start timer callback */
static void usbh_start_cb(void *argument)
{
    LOG_E("usb start\r\n");
    struct bflb_device_s *usb_dev = bflb_device_get_by_name(BFLB_NAME_USB_V2);
    usbh_initialize(0, usb_dev->reg_base);

    usbh_reset_flag = false;
}

#if USB_UVC_AUTO_RESET_PLUS_EN
static void usbh_uvc_check_cb(void *argument)
{
    if (usbh_uvc_run_flag == false || usbh_reset_flag == true) {
        return;
    }

    if (uvc_check_frame_cnt == 0xffffffff) {
        uvc_check_frame_cnt = soln_usbh_video_get_total_frame_count();
        return;
    }

    uint32_t frame_count = soln_usbh_video_get_total_frame_count();
    if (frame_count == uvc_check_frame_cnt) {
        LOG_E("uvc no frame, trigger reset\r\n");
        usbh_video_transfer_abort_callback();
    } else {
        uvc_check_frame_cnt = frame_count;
    }
}
#endif

/*  */
static void usbh_video_auto_reset_init(void)
{
    usbh_reset_flag = false;
    usbh_uvc_run_flag = true;

    if (usbh_stop_timer == NULL) {
        usbh_stop_timer = usb_osal_timer_create("usbh_stop", 20, usbh_stop_cb, NULL, 0);
    }
    if (usbh_start_timer == NULL) {
        usbh_start_timer = usb_osal_timer_create("usbh_start", 20, usbh_start_cb, NULL, 0);
    }

#if USB_UVC_AUTO_RESET_PLUS_EN
    if (uvc_check_timer == NULL) {
        uvc_check_timer = usb_osal_timer_create("uvc_check", 400, usbh_uvc_check_cb, NULL, true);
    }
    uvc_check_frame_cnt = 0xffffffff;
    usb_osal_timer_start(uvc_check_timer);
#endif
}

static void usbh_video_auto_reset_deinit(void)
{
    usbh_uvc_run_flag = false;

#if USB_UVC_AUTO_RESET_PLUS_EN
    if (uvc_check_timer) {
        usb_osal_timer_stop(uvc_check_timer);
    }
#endif
}

#if IS_ENABLED(CONFIG_SHELL)
#include "shell.h"

static int cmd_usbh_reset(int argc, char **argv)
{
    usbh_video_transfer_abort_callback();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_usbh_reset, uvc_usbh_reset, Start HiBooster receiver);
#endif

#endif

/************************************ UVC ISO BUFF PORT ************************************ */
#define VIDEO_ISO_INTERVAL (2)
#define VIDEO_ISO_PACKETS  (8 * VIDEO_ISO_INTERVAL)

#if IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
#define VIDEO_EP_MPS 1024
#else
#define VIDEO_EP_MPS 3072
#endif

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t video_buffer1[VIDEO_ISO_PACKETS][VIDEO_EP_MPS];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t video_buffer2[VIDEO_ISO_PACKETS][VIDEO_EP_MPS];

void usbh_voide_iso_buff_get(struct uvc_iso_buff_s *uvc_iso_buff)
{
    if (uvc_iso_buff == NULL) {
        LOG_E("uvc_iso_buff is NULL\r\n");
        return;
    }
    uvc_iso_buff->buff_1 = (uint8_t *)video_buffer1;
    uvc_iso_buff->buff_2 = (uint8_t *)video_buffer2;
    uvc_iso_buff->buff_size = sizeof(video_buffer1);
    uvc_iso_buff->iso_interval = VIDEO_ISO_INTERVAL;
    uvc_iso_buff->packet_cnt = VIDEO_ISO_PACKETS;
    uvc_iso_buff->ep_mps = VIDEO_EP_MPS;

    LOG_D("uvc_iso_buff: buff_1 0x%08X, buff_2 0x%08X, size %d, interval %d, packets %d, mps %d\r\n",
          (uint32_t)uvc_iso_buff->buff_1, (uint32_t)uvc_iso_buff->buff_2,
          uvc_iso_buff->buff_size, uvc_iso_buff->iso_interval,
          uvc_iso_buff->packet_cnt, uvc_iso_buff->ep_mps);
}

/************************************ UVC FRAME PORT ************************************ */
#include "soln_fbq.h"

volatile bool uvc_frame_flag = false;
static struct usbh_videoframe uvc_frame = { 0 };

#if IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
#define USBH_UVC_FORMAT USBH_VIDEO_FORMAT_MJPEG
#define IMG_FRAME_CTRL  soln_fbq_vid_jpeg_local()
#elif IS_ENABLED(CONFIG_SOLN_VID_UVC_YUYV_EN)
#define USBH_UVC_FORMAT USBH_VIDEO_FORMAT_UNCOMPRESSED
#define IMG_FRAME_CTRL  soln_fbq_vid_raw_local()
#endif
static fbq_elem_t *queue_frame;

ATTR_FAST_RAM_SECTION struct usbh_videoframe *usbh_uvc_frame_alloc(void)
{
    int ret;

    if (uvc_frame_flag == true) {
        LOG_E("uvc frame is busy\r\n");
        return NULL;
    }

    /* alloc new jpeg frame */
    ret = fbq_alloc(IMG_FRAME_CTRL, &queue_frame, 0);
    if (ret != FBQ_OK) {
        LOG_D("alloc failed\r\n");
        return NULL;
    }
#if IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
    queue_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_UVC;
#elif IS_ENABLED(CONFIG_SOLN_VID_UVC_YUYV_EN)
    queue_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_IMG_RAW_UVC;
#endif
    LOG_D("alloc id %d, addr 0x%08X\r\n", queue_frame->id, (uint32_t)queue_frame->data);

    uvc_frame.frame_buf = queue_frame->data;
    uvc_frame.frame_bufsize = queue_frame->capacity;
    uvc_frame.frame_format = USBH_UVC_FORMAT;

    uvc_frame_flag = true;
    return &uvc_frame;
}

int usbh_uvc_frame_free(struct usbh_videoframe *frame)
{
    if (uvc_frame_flag == false) {
        LOG_E("uvc frame is not allocated\r\n");
        return -1;
    }

    LOG_D("free frame id %d\r\n", queue_frame->id);

    fbq_free(queue_frame);
    queue_frame = NULL;
    uvc_frame_flag = false;

    return 0;
}

ATTR_FAST_RAM_SECTION int usbh_uvc_frame_send(struct usbh_videoframe *frame)
{
    if (uvc_frame_flag == false) {
        LOG_E("uvc frame is not allocated\r\n");
        return -1;
    }

    if (frame == NULL) {
        LOG_E("uvc frame is NULL\r\n");
        return -1;
    }

    if (frame->frame_buf != queue_frame->data) {
        LOG_E("uvc frame buf is not match\r\n");
        return -1;
    }

    if (frame->frame_size > queue_frame->capacity) {
        LOG_E("uvc frame size over: %d -> %d\r\n", frame->frame_size, queue_frame->capacity);
        return -1;
    }

    LOG_D("send frame id %d\r\n", queue_frame->id);

#if IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
    /* cache */
    bflb_l1c_dcache_invalidate_range(queue_frame->data, frame->frame_size);
#elif IS_ENABLED(CONFIG_SOLN_VID_UVC_YUYV_EN)
    soln_fbq_img_raw_ext_t *raw_ext = soln_fbq_img_raw_ext(queue_frame);
    raw_ext->x_start = 0;
    raw_ext->y_start = 0;
    raw_ext->x_end = CONFIG_SOLN_VID_DEFAULT_WIDTH - 1;
    raw_ext->y_end = CONFIG_SOLN_VID_DEFAULT_HEIGHT - 1;
    raw_ext->format = IMG_RAW_FRAME_FORMAT_YUYV;
    /* cache */
    // bflb_l1c_dcache_invalidate_range(queue_frame.elem_base.frame_addr, frame->frame_size);
#endif

    queue_frame->size = frame->frame_size;
    (void)fbq_push_mask(IMG_FRAME_CTRL, queue_frame, FBQ_OUTPUT_ALL, 0);
    fbq_free(queue_frame);
    queue_frame = NULL;

    uvc_frame_flag = false;
    return 0;
}

/************************************ DMA PORT ************************************ */
static struct bflb_device_s *dma_usb;
static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s dma_uvc_lli_pool[VIDEO_ISO_PACKETS + 1];

#define UVC_DMA_DEBUG 0

#if (UVC_DMA_DEBUG)
/* debug */
volatile uint32_t dma_tranf_size;
volatile uint32_t dma_index;
#endif

void usbh_video_dma_init(void)
{
    struct bflb_dma_channel_config_s usb_dma_cfg;

    usb_dma_cfg.direction = DMA_MEMORY_TO_MEMORY;
    usb_dma_cfg.src_req = DMA_REQUEST_NONE;
    usb_dma_cfg.dst_req = DMA_REQUEST_NONE;
    usb_dma_cfg.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    usb_dma_cfg.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
#if defined(BL616CL)
    usb_dma_cfg.src_burst_count = DMA_BURST_INCR16;
    usb_dma_cfg.dst_burst_count = DMA_BURST_INCR16;
#else
    usb_dma_cfg.src_burst_count = DMA_BURST_INCR8;
    usb_dma_cfg.dst_burst_count = DMA_BURST_INCR8;
#endif
    usb_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    usb_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    dma_usb = bflb_device_get_by_name("dma0_ch2");
    bflb_dma_channel_init(dma_usb, &usb_dma_cfg);

    for (uint8_t i = 0; i < VIDEO_ISO_PACKETS + 1; i++) {
        dma_uvc_lli_pool[i].control.WORD = bflb_dma_feature_control(dma_usb, DMA_CMD_GET_LLI_CONTROL, 0);
    }
}

ATTR_FAST_RAM_SECTION void usbh_video_dma_lli_fill(uint32_t desc_index, uint32_t src_addr, uint32_t dst_addr, uint32_t nbytes)
{
    dma_uvc_lli_pool[desc_index].src_addr = src_addr;
    dma_uvc_lli_pool[desc_index].dst_addr = dst_addr;
    dma_uvc_lli_pool[desc_index].control.bits.TransferSize = nbytes / 2;

    dma_uvc_lli_pool[desc_index].control.bits.I = 0;
    dma_uvc_lli_pool[desc_index].nextlli = 0;

    if (desc_index > 0) {
        dma_uvc_lli_pool[desc_index - 1].nextlli = (uint32_t)&dma_uvc_lli_pool[desc_index];
    }

#if (UVC_DMA_DEBUG)
    if (nbytes % 2 != 0) {
        LOG_E("nbytes %d is not even, desc_index %d\r\n", nbytes, desc_index);
    }
    if (desc_index == 0) {
        dma_tranf_size = 0;
    }
    dma_index = desc_index;
    dma_tranf_size += nbytes;
#endif
}

ATTR_FAST_RAM_SECTION void usbh_video_dma_start(void)
{
    if (!dma_usb) {
        LOG_E("dma_usb is NULL\r\n");
        return;
    }

    bflb_dma_feature_control(dma_usb, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&dma_uvc_lli_pool[0]);
    bflb_dma_channel_start(dma_usb);

#if (UVC_DMA_DEBUG)
    LOG_D("dma: %d %d\r\n", dma_index, dma_tranf_size);
#endif
}

ATTR_FAST_RAM_SECTION void usbh_video_dma_stop(void)
{
    if (!dma_usb) {
        LOG_E("dma_usb is NULL\r\n");
        return;
    }

    bflb_dma_channel_stop(dma_usb);
}

ATTR_FAST_RAM_SECTION bool usbh_video_dma_isbusy(void)
{
    if (!dma_usb) {
        LOG_E("dma_usb is NULL\r\n");
        return false;
    }

    return bflb_dma_channel_isbusy(dma_usb);
}

/************************************ USBH CB ************************************ */

void usbh_video_run(struct usbh_video *video_class)
{
    LOG_I("Starting UVC %s mode ...\r\n", USBH_UVC_FORMAT == USBH_VIDEO_FORMAT_MJPEG ? "MJPEG" : "YUYV");

    usb_osal_msleep(200);

#if (USB_UVC_AUTO_RESET_EN)
    usbh_video_auto_reset_init();
#endif

    usbh_video_stream_start(CONFIG_SOLN_VID_DEFAULT_WIDTH, CONFIG_SOLN_VID_DEFAULT_HEIGHT, USBH_UVC_FORMAT);
}

void usbh_video_stop(struct usbh_video *video_class)
{
    LOG_I("Stop UVC %s mode !\r\n", USBH_UVC_FORMAT == USBH_VIDEO_FORMAT_MJPEG ? "MJPEG" : "YUYV");

#if (USB_UVC_AUTO_RESET_EN)
    usbh_video_auto_reset_deinit();
#endif

    usbh_video_stream_stop();
}

/************************************  ************************************ */
