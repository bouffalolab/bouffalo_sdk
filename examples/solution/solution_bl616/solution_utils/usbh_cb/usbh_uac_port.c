#include "bflb_core.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "usbh_uac_stream.h"

#if (0)
#define UAC_INFO(a, ...) printf("[UAC info]:" a, ##__VA_ARGS__)
#else
#define UAC_INFO(a, ...)
#endif

#define UAC_ERR(a, ...)       printf("[UAC err]:" a, ##__VA_ARGS__)

#define ATTR_FAST_RAM_SECTION ATTR_TCM_SECTION

#if CONFIG_SOLUTION_FUNC_UAC_IN

/************************************ UAC MIC ISO BUFF PORT ************************************ */
#define AUDIO_MIC_ISO_INTERVAL (10)
#define AUDIO_MIC_ISO_PACKETS  (AUDIO_MIC_ISO_INTERVAL)
#define AUDIO_MIC_EP_MPS       512

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t audio_mic_buffer1[AUDIO_MIC_ISO_PACKETS][AUDIO_MIC_EP_MPS];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t audio_mic_buffer2[AUDIO_MIC_ISO_PACKETS][AUDIO_MIC_EP_MPS];

void usbh_audio_mic_iso_buff_get(struct uac_in_iso_buff_s *uac_iso_buff)
{
    if (uac_iso_buff == NULL) {
        UAC_ERR("uac_iso_buff is NULL\r\n");
        return;
    }
    uac_iso_buff->buff_1 = (uint8_t *)audio_mic_buffer1;
    uac_iso_buff->buff_2 = (uint8_t *)audio_mic_buffer2;
    uac_iso_buff->buff_size = sizeof(audio_mic_buffer1);
    uac_iso_buff->iso_interval = AUDIO_MIC_ISO_INTERVAL;
    uac_iso_buff->packet_cnt = AUDIO_MIC_ISO_PACKETS;
    uac_iso_buff->ep_mps = AUDIO_MIC_EP_MPS;

    UAC_INFO("uac_iso_buff: buff_1 0x%08X, buff_2 0x%08X, size %d, interval %d, packets %d, mps %d\r\n",
             (uint32_t)uac_iso_buff->buff_1, (uint32_t)uac_iso_buff->buff_2,
             uac_iso_buff->buff_size, uac_iso_buff->iso_interval,
             uac_iso_buff->packet_cnt, uac_iso_buff->ep_mps);
}

/************************************ UAC MIC FRAME PORT ************************************ */

static volatile bool uac_frame_flag = false;
static struct usbh_audioframe uac_mic_frame = { 0 };
static audio_in_frame_t audio_in_frame = { 0 };

ATTR_FAST_RAM_SECTION struct usbh_audioframe *usbh_uac_mic_frame_alloc(void)
{
    int ret;

    if (uac_frame_flag == true) {
        UAC_ERR("uac mic frame is busy\r\n");
        return NULL;
    }

    /* alloc new jpeg frame */
    ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_in_frame, 0);
    if (ret < 0) {
        UAC_INFO("alloc failed\r\n");
        return NULL;
    }
    UAC_INFO("alloc id %d, addr 0x%08X\r\n", audio_in_frame.elem_base.frame_id, (uint32_t)audio_in_frame.elem_base.frame_addr);

    uac_mic_frame.frame_buf = audio_in_frame.elem_base.frame_addr;
    uac_mic_frame.frame_bufsize = audio_in_frame.elem_base.frame_size;
    uac_mic_frame.frame_size = 0;

    uac_frame_flag = true;
    return &uac_mic_frame;
}

int usbh_uac_mic_frame_free(struct usbh_audioframe *frame)
{
    if (uac_frame_flag == false) {
        UAC_ERR("uac mic frame is not busy\r\n");
        return -1;
    }

    UAC_INFO("free id %d, addr 0x%08X\r\n", audio_in_frame.elem_base.frame_id, (uint32_t)audio_in_frame.elem_base.frame_addr);

    frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_in_frame);
    uac_frame_flag = false;

    return 0;
}

ATTR_FAST_RAM_SECTION int usbh_uac_mic_frame_send(struct usbh_audioframe *frame)
{
    if (uac_frame_flag == false) {
        UAC_ERR("uac mic frame is not busy\r\n");
        return -1;
    }

    if (frame == NULL) {
        UAC_ERR("uac mic frame is NULL\r\n");
        return -1;
    }

    if (frame->frame_buf != audio_in_frame.elem_base.frame_addr) {
        UAC_ERR("uac mic frame buf is not match\r\n");
        return -1;
    }

    if (frame->frame_size > audio_in_frame.elem_base.frame_size) {
        UAC_ERR("uac mic frame size over: %d->%d\r\n", audio_in_frame.elem_base.frame_size, frame->frame_size);
        return -1;
    }

    UAC_INFO("send frame id %d\r\n", audio_in_frame.elem_base.frame_id);

    if (frame->frame_size == 0) {
        UAC_ERR("uac mic frame size is 0\r\n");
    }
    audio_in_frame.data_size = frame->frame_size;
    UAC_INFO("uac mic size %d\r\n", audio_in_frame.data_size);

    /* pcm push */
    for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
        int ret = frame_queue_push(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_in_frame, i, 0);
        if (ret >= 0) {
            UAC_INFO("uav mic[%d]\r\n", i);
        }
    }
    /* unlock */
    frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_in_frame);

    uac_frame_flag = false;
    return 0;
}

#endif

/************************************ UAC Speaker FRAME PORT ************************************ */
#if CONFIG_SOLUTION_FUNC_UAC_OUT
int usbh_uac_speaker_frame_recv(struct usbh_audioframe **frame, uint32_t timeout)
{
    return 0;
}

int usbh_uac_speaker_frame_free(struct usbh_audioframe *frame)
{
    return 0;
}
#endif

/************************************ USBH CB ************************************ */

/* for test */
void usbh_audio_run(struct usbh_audio *audio_class)
{
#if CONFIG_SOLUTION_FUNC_UAC_OUT
    printf("Starting UAC Speaker ...\r\n");
    usbh_audio_speaker_stream_start(16000);
#endif

#if CONFIG_SOLUTION_FUNC_UAC_IN
    printf("Starting UAC Mic ...\r\n");
    usbh_audio_mic_stream_start(16000);
#endif
}

void usbh_audio_stop(struct usbh_audio *audio_class)
{
#if CONFIG_SOLUTION_FUNC_UAC_IN
    printf("Stop UAC Mic !\r\n");
    usbh_audio_mic_stream_stop();
#endif

#if CONFIG_SOLUTION_FUNC_UAC_OUT
    printf("Stop UAC Speaker !\r\n");
    usbh_audio_speaker_stream_stop();
#endif
}