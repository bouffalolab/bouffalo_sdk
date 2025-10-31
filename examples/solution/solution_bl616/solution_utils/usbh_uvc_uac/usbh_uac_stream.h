
#ifndef USBH_UAC_STREAM_H
#define USBH_UAC_STREAM_H

#include "usbh_core.h"
#include "usbh_audio.h"

struct usbh_audioframe {
    uint8_t *frame_buf;
    uint32_t frame_bufsize;
    uint32_t frame_size;
};

// #define AUDIO_MIC_ISO_INTERVAL     (1)
// #define AUDIO_MIC_ISO_PACKETS      (AUDIO_MIC_ISO_INTERVAL)
// #define AUDIO_MIC_EP_MPS           1024

#define AUDIO_SPEAKER_ISO_INTERVAL (1)
#define AUDIO_SPEAKER_ISO_PACKETS  (AUDIO_SPEAKER_ISO_INTERVAL)
#define AUDIO_SPEAKER_EP_MPS       1024

/* mic */

struct uac_in_iso_buff_s {
    uint8_t *buff_1;
    uint8_t *buff_2;
    uint32_t buff_size;
    uint16_t iso_interval;
    uint16_t packet_cnt;
    uint16_t ep_mps;
};

extern volatile uint32_t audio_mic_complete_count;

/* implemented by user */
void usbh_audio_mic_iso_buff_get(struct uac_in_iso_buff_s *uac_iso_buff);

/* implemented by user */
struct usbh_audioframe *usbh_uac_mic_frame_alloc(void);
int usbh_uac_mic_frame_free(struct usbh_audioframe *frame);
int usbh_uac_mic_frame_send(struct usbh_audioframe *frame);

/* api */
void usbh_audio_mic_stream_start(uint32_t freq);
void usbh_audio_mic_stream_stop(void);

/* speaker */

/* implemented by user */
int usbh_uac_speaker_frame_recv(struct usbh_audioframe **frame, uint32_t timeout);
int usbh_uac_speaker_frame_free(struct usbh_audioframe *frame);

/* api */
void usbh_audio_speaker_stream_start(uint32_t freq);
void usbh_audio_speaker_stream_stop(void);

// struct usbh_audioframe *usbh_uac_speaker_frame_alloc(void);
// int usbh_uac_speaker_frame_send(struct usbh_audioframe *frame);

#endif