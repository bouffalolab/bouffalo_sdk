#include "bflb_core.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#include "soln_fbq.h"

#include "usbh_uac_stream.h"

#ifdef CONFIG_SOLN_AUD_UAC_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_AUD_UAC_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_UAC"
#include "log.h"

#define ATTR_FAST_RAM_SECTION ATTR_TCM_SECTION

#if IS_ENABLED(CONFIG_SOLN_AUD_UAC_IN_EN)

/************************************ UAC MIC ISO BUFF PORT ************************************ */
#define AUDIO_MIC_ISO_INTERVAL (20) /* 20ms */
#define AUDIO_MIC_ISO_PACKETS  (AUDIO_MIC_ISO_INTERVAL)
#define AUDIO_MIC_EP_MPS       256

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t audio_mic_buffer1[AUDIO_MIC_ISO_PACKETS][AUDIO_MIC_EP_MPS];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t audio_mic_buffer2[AUDIO_MIC_ISO_PACKETS][AUDIO_MIC_EP_MPS];

void usbh_audio_mic_iso_buff_get(struct uac_in_iso_buff_s *uac_iso_buff)
{
    if (uac_iso_buff == NULL) {
        LOG_E("uac_iso_buff is NULL\r\n");
        return;
    }
    uac_iso_buff->buff_1 = (uint8_t *)audio_mic_buffer1;
    uac_iso_buff->buff_2 = (uint8_t *)audio_mic_buffer2;
    uac_iso_buff->buff_size = sizeof(audio_mic_buffer1);
    uac_iso_buff->iso_interval = AUDIO_MIC_ISO_INTERVAL;
    uac_iso_buff->packet_cnt = AUDIO_MIC_ISO_PACKETS;
    uac_iso_buff->ep_mps = AUDIO_MIC_EP_MPS;

    LOG_D("uac_iso_buff: buff_1 0x%08X, buff_2 0x%08X, size %d, interval %d, packets %d, mps %d\r\n",
          (uint32_t)uac_iso_buff->buff_1, (uint32_t)uac_iso_buff->buff_2,
          uac_iso_buff->buff_size, uac_iso_buff->iso_interval,
          uac_iso_buff->packet_cnt, uac_iso_buff->ep_mps);
}

/************************************ UAC MIC FRAME PORT ************************************ */

static volatile bool uac_frame_flag = false;
static struct usbh_audioframe uac_mic_frame = { 0 };
static fbq_elem_t *local_pcm_frame;

ATTR_FAST_RAM_SECTION struct usbh_audioframe *usbh_uac_mic_frame_alloc(void)
{
    int ret;

    if (uac_frame_flag == true) {
        LOG_E("uac mic frame is busy\r\n");
        return NULL;
    }

    /* alloc new jpeg frame */
    ret = fbq_alloc(soln_fbq_aud_pcm_local(), &local_pcm_frame, 0);
    if (ret != FBQ_OK) {
        LOG_D("alloc failed\r\n");
        return NULL;
    }
    local_pcm_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_UAC_IN;
    LOG_D("alloc id %d, addr 0x%08X\r\n", local_pcm_frame->id, (uint32_t)local_pcm_frame->data);

    uac_mic_frame.frame_buf = local_pcm_frame->data;
    uac_mic_frame.frame_bufsize = local_pcm_frame->capacity;
    uac_mic_frame.frame_size = 0;

    uac_frame_flag = true;
    return &uac_mic_frame;
}

int usbh_uac_mic_frame_free(struct usbh_audioframe *frame)
{
    if (uac_frame_flag == false) {
        LOG_E("uac mic frame is not busy\r\n");
        return -1;
    }

    LOG_D("free id %d, addr 0x%08X\r\n", local_pcm_frame->id, (uint32_t)local_pcm_frame->data);

    fbq_free(local_pcm_frame);
    local_pcm_frame = NULL;
    uac_frame_flag = false;

    return 0;
}

ATTR_FAST_RAM_SECTION int usbh_uac_mic_frame_send(struct usbh_audioframe *frame)
{
    if (uac_frame_flag == false) {
        LOG_E("uac mic frame is not busy\r\n");
        return -1;
    }

    if (frame == NULL) {
        LOG_E("uac mic frame is NULL\r\n");
        return -1;
    }

    if (frame->frame_buf != local_pcm_frame->data) {
        LOG_E("uac mic frame buf is not match\r\n");
        return -1;
    }

    if (frame->frame_size > local_pcm_frame->capacity) {
        LOG_E("uac mic frame size over: %d->%d\r\n", local_pcm_frame->capacity, frame->frame_size);
        return -1;
    }

    LOG_D("send frame id %d\r\n", local_pcm_frame->id);

    if (frame->frame_size == 0) {
        LOG_E("uac mic frame size is 0\r\n");
    }
    local_pcm_frame->size = frame->frame_size;
    LOG_D("uac mic size %d\r\n", local_pcm_frame->size);

    (void)fbq_push_mask(soln_fbq_aud_pcm_local(), local_pcm_frame, FBQ_OUTPUT_ALL, 0);
    fbq_free(local_pcm_frame);
    local_pcm_frame = NULL;

    uac_frame_flag = false;
    return 0;
}

#endif

/************************************ UAC Speaker FRAME PORT ************************************ */
#if IS_ENABLED(CONFIG_SOLN_AUD_UAC_OUT_EN)
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
#if IS_ENABLED(CONFIG_SOLN_AUD_UAC_OUT_EN)
    LOG_I("Starting UAC Speaker ...\r\n");
    usbh_audio_speaker_stream_start(16000);
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_UAC_IN_EN)
    LOG_I("Starting UAC Mic ...\r\n");
    usbh_audio_mic_stream_start(16000);
#endif
}

void usbh_audio_stop(struct usbh_audio *audio_class)
{
#if IS_ENABLED(CONFIG_SOLN_AUD_UAC_IN_EN)
    LOG_I("Stop UAC Mic !\r\n");
    usbh_audio_mic_stream_stop();
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_UAC_OUT_EN)
    LOG_I("Stop UAC Speaker !\r\n");
    usbh_audio_speaker_stream_stop();
#endif
}