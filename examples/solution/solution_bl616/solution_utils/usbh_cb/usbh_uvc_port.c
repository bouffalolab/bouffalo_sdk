
#include "bflb_core.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"

#include "usbh_uvc_stream.h"

#if (0)
#define UVC_INFO(a, ...) printf("[UVC info]:" a, ##__VA_ARGS__)
#else
#define UVC_INFO(a, ...)
#endif

#define UVC_ERR(a, ...)       printf("[UVC err]:" a, ##__VA_ARGS__)

#define ATTR_FAST_RAM_SECTION ATTR_TCM_SECTION

/************************************  ************************************ */
volatile uint32_t g_uvc_total_frame_cnt = 0;

ATTR_FAST_RAM_SECTION void usbh_video_fps_record(void)
{
    g_uvc_total_frame_cnt++;
}

ATTR_FAST_RAM_SECTION void usbh_video_transfer_abort_callback(void)
{
}

/************************************ UVC ISO BUFF PORT ************************************ */
#define VIDEO_ISO_INTERVAL (2)
#define VIDEO_ISO_PACKETS  (8 * VIDEO_ISO_INTERVAL)
#define VIDEO_EP_MPS       3072

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t video_buffer1[VIDEO_ISO_PACKETS][VIDEO_EP_MPS];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t video_buffer2[VIDEO_ISO_PACKETS][VIDEO_EP_MPS];

void usbh_voide_iso_buff_get(struct uvc_iso_buff_s *uvc_iso_buff)
{
    if (uvc_iso_buff == NULL) {
        UVC_ERR("uvc_iso_buff is NULL\r\n");
        return;
    }
    uvc_iso_buff->buff_1 = (uint8_t *)video_buffer1;
    uvc_iso_buff->buff_2 = (uint8_t *)video_buffer2;
    uvc_iso_buff->buff_size = sizeof(video_buffer1);
    uvc_iso_buff->iso_interval = VIDEO_ISO_INTERVAL;
    uvc_iso_buff->packet_cnt = VIDEO_ISO_PACKETS;
    uvc_iso_buff->ep_mps = VIDEO_EP_MPS;

    UVC_INFO("uvc_iso_buff: buff_1 0x%08X, buff_2 0x%08X, size %d, interval %d, packets %d, mps %d\r\n",
             (uint32_t)uvc_iso_buff->buff_1, (uint32_t)uvc_iso_buff->buff_2,
             uvc_iso_buff->buff_size, uvc_iso_buff->iso_interval,
             uvc_iso_buff->packet_cnt, uvc_iso_buff->ep_mps);
}

/************************************ UVC FRAME PORT ************************************ */
#include "frame_queue_ctrl.h"
#include "frame_queue.h"

volatile bool uvc_frame_flag = false;
static struct usbh_videoframe uvc_frame = { 0 };

#if (CONFIG_SOLUTION_FUNC_UVC_JPEG)
#define USBH_UVC_FORMAT USBH_VIDEO_FORMAT_MJPEG
#define IMG_FRAME_CTRL  g_jpeg_frame_ctrl
static jpeg_frame_t queue_frame = { 0 };
#elif (CONFIG_SOLUTION_FUNC_UVC_YUYV)
#define USBH_UVC_FORMAT USBH_VIDEO_FORMAT_UNCOMPRESSED
#define IMG_FRAME_CTRL  g_yuyv_frame_ctrl
static pyuyv_frame_t queue_frame = { 0 };
#endif

ATTR_FAST_RAM_SECTION struct usbh_videoframe *usbh_uvc_frame_alloc(void)
{
    int ret;

    if (uvc_frame_flag == true) {
        UVC_ERR("uvc frame is busy\r\n");
        return NULL;
    }

    /* alloc new jpeg frame */
    ret = frame_queue_alloc_lock(IMG_FRAME_CTRL, (frame_elem_t *)&queue_frame, 0);
    if (ret < 0) {
        UVC_INFO("alloc failed\r\n");
        return NULL;
    }
    UVC_INFO("alloc id %d, addr 0x%08X\r\n", queue_frame.elem_base.frame_id, (uint32_t)queue_frame.elem_base.frame_addr);

    uvc_frame.frame_buf = queue_frame.elem_base.frame_addr;
    uvc_frame.frame_bufsize = queue_frame.elem_base.frame_size;
    uvc_frame.frame_format = USBH_UVC_FORMAT;

    uvc_frame_flag = true;
    return &uvc_frame;
}

int usbh_uvc_frame_free(struct usbh_videoframe *frame)
{
    if (uvc_frame_flag == false) {
        UVC_ERR("uvc frame is not allocated\r\n");
        return -1;
    }

    UVC_INFO("free frame id %d\r\n", queue_frame.elem_base.frame_id);

    frame_queue_free_unlock(IMG_FRAME_CTRL, (frame_elem_t *)&queue_frame);
    uvc_frame_flag = false;

    return 0;
}

ATTR_FAST_RAM_SECTION int usbh_uvc_frame_send(struct usbh_videoframe *frame)
{
    int ret;

    if (uvc_frame_flag == false) {
        UVC_ERR("uvc frame is not allocated\r\n");
        return -1;
    }

    if (frame == NULL) {
        UVC_ERR("uvc frame is NULL\r\n");
        return -1;
    }

    if (frame->frame_buf != queue_frame.elem_base.frame_addr) {
        UVC_ERR("uvc frame buf is not match\r\n");
        return -1;
    }

    if (frame->frame_size > queue_frame.elem_base.frame_size) {
        UVC_ERR("uvc frame size over: %d -> %d\r\n", frame->frame_size, queue_frame.elem_base.frame_size);
        return -1;
    }

    UVC_INFO("send frame id %d\r\n", queue_frame.elem_base.frame_id);

#if (CONFIG_SOLUTION_FUNC_UVC_JPEG)
    queue_frame.data_size = frame->frame_size;
    /* cache */
    bflb_l1c_dcache_invalidate_range(queue_frame.elem_base.frame_addr, frame->frame_size);
#elif (CONFIG_SOLUTION_FUNC_UVC_YUYV)
    queue_frame.x_start = 0;
    queue_frame.y_start = 0;
    queue_frame.x_end = CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH - 1;
    queue_frame.y_end = CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT - 1;
#endif

    /* picture push */
    for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
        ret = frame_queue_push(IMG_FRAME_CTRL, (frame_elem_t *)&queue_frame, i, 0);
        if (ret >= 0) {
            UVC_INFO("uav[%d]\r\n", i);
        }
    }
    /* unlock */
    frame_queue_free_unlock(IMG_FRAME_CTRL, (frame_elem_t *)&queue_frame);

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
#if defined(BL616L)
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
        UVC_ERR("nbytes %d is not even, desc_index %d\r\n", nbytes, desc_index);
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
    bflb_dma_feature_control(dma_usb, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&dma_uvc_lli_pool[0]);
    bflb_dma_channel_start(dma_usb);

#if (UVC_DMA_DEBUG)
    printf("dma: %d %d\r\n", dma_index, dma_tranf_size);
#endif
}

ATTR_FAST_RAM_SECTION void usbh_video_dma_stop(void)
{
    bflb_dma_channel_stop(dma_usb);
}

ATTR_FAST_RAM_SECTION bool usbh_video_dma_isbusy(void)
{
    return bflb_dma_channel_isbusy(dma_usb);
}

/************************************ USBH CB ************************************ */

void usbh_video_run(struct usbh_video *video_class)
{
    printf("Starting UVC %s mode ...\r\n", USBH_UVC_FORMAT == USBH_VIDEO_FORMAT_MJPEG ? "MJPEG" : "YUYV");
    usbh_video_stream_start(CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH, CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT, USBH_UVC_FORMAT);
}

void usbh_video_stop(struct usbh_video *video_class)
{
    printf("Stop UVC %s mode !\r\n", USBH_UVC_FORMAT == USBH_VIDEO_FORMAT_MJPEG ? "MJPEG" : "YUYV");
    usbh_video_stream_stop();
}

/************************************  ************************************ */
