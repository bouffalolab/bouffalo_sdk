/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>


#ifndef __MSP_PCM__
#define __MSP_PCM__

#include <xutils/device.h>
#include <xutils/driver.h>

//#include <xutils/xringbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

// Provide management of digital audio capture and playback
#define PCM_EVT_WRITE (1 << 0)
#define PCM_EVT_READ (1 << 1)
#define PCM_EVT_XRUN (1 << 2)/** Stopped: underrun (playback) or overrun (capture) detected */

/** Non blocking mode (flag for open mode) \hideinitializer */
#define MSP_PCM_NONBLOCK        0x00000001
/** Async notification (flag for open mode) \hideinitializer */
#define MSP_PCM_ASYNC           0x00000002

typedef enum _msp_pcm_state {
	/** Open */
	MSP_PCM_STATE_OPEN = 0,
	/** Setup installed */
	MSP_PCM_STATE_SETUP,
	/** Ready to start */
	MSP_PCM_STATE_PREPARED,
	/** Running */
	MSP_PCM_STATE_RUNNING,
	/** Stopped: underrun (playback) or overrun (capture) detected */
	MSP_PCM_STATE_XRUN,
	/** Draining: running (playback) or stopped (capture) */
	MSP_PCM_STATE_DRAINING,
	/** Paused */
	MSP_PCM_STATE_PAUSED,
	/** Hardware is suspended */
	MSP_PCM_STATE_SUSPENDED,
	/** Hardware is disconnected */
	MSP_PCM_STATE_DISCONNECTED,
	MSP_PCM_STATE_LAST = MSP_PCM_STATE_DISCONNECTED,
	/** Private - used internally in the library - do not use*/
	MSP_PCM_STATE_PRIVATE1 = 1024
} msp_pcm_state_t;

typedef enum _msp_pcm_stream {
    MSP_PCM_STREAM_PLAYBACK = 0,
    MSP_PCM_STREAM_CAPTURE,
    MSP_PCM_STREAM_LAST = MSP_PCM_STREAM_CAPTURE,
} msp_pcm_stream_t;

typedef enum _msp_pcm_access {
    /** msp_pcm_readi/msp_pcm_writei access */
    MSP_PCM_ACCESS_RW_INTERLEAVED,
    /** msp_pcm_readn/msp_pcm_writen access */
    MSP_PCM_ACCESS_RW_NONINTERLEAVED,
    MSP_PCM_ACCESS_LAST = MSP_PCM_ACCESS_RW_NONINTERLEAVED
} msp_pcm_access_t;

typedef enum _snd_pcm_format {
    SND_PCM_FORMAT_UNKNOWN = -1,
    SND_PCM_FORMAT_S8 = 0,
    SND_PCM_FORMAT_U8,
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S16_BE,
    SND_PCM_FORMAT_U16_LE,
    SND_PCM_FORMAT_U16_BE,
    SND_PCM_FORMAT_S24_LE,
    SND_PCM_FORMAT_S24_BE,
    SND_PCM_FORMAT_U24_LE,
    SND_PCM_FORMAT_U24_BE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_S32_BE,
    SND_PCM_FORMAT_U32_LE,
    SND_PCM_FORMAT_U32_BE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT_BE,
    SND_PCM_FORMAT_FLOAT64_LE,
    SND_PCM_FORMAT_FLOAT64_BE,
    SND_PCM_FORMAT_S20_LE = 24,
    SND_PCM_FORMAT_S20_BE,
    SND_PCM_FORMAT_U20_LE,
    SND_PCM_FORMAT_U20_BE,
    SND_PCM_FORMAT_SPECIAL = 31,
    SND_PCM_FORMAT_S24_3LE = 32,
    SND_PCM_FORMAT_S24_3BE,
    SND_PCM_FORMAT_U24_3LE,
    SND_PCM_FORMAT_U24_3BE,
    SND_PCM_FORMAT_S20_3LE,
    SND_PCM_FORMAT_S20_3BE,
    SND_PCM_FORMAT_U20_3LE,
    SND_PCM_FORMAT_U20_3BE,
    SND_PCM_FORMAT_S18_3LE,
    SND_PCM_FORMAT_S18_3BE,
    SND_PCM_FORMAT_U18_3LE,
    SND_PCM_FORMAT_U18_3BE,
    SND_PCM_FORMAT_LAST = SND_PCM_FORMAT_U18_3BE,
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    SND_PCM_FORMAT_S16 = SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_U16 = SND_PCM_FORMAT_U16_LE,
    SND_PCM_FORMAT_S24 = SND_PCM_FORMAT_S24_LE,
    SND_PCM_FORMAT_U24 = SND_PCM_FORMAT_U24_LE,
    SND_PCM_FORMAT_S32 = SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_U32 = SND_PCM_FORMAT_U32_LE,
    SND_PCM_FORMAT_FLOAT = SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT64 = SND_PCM_FORMAT_FLOAT64_LE,
    SND_PCM_FORMAT_S20 = SND_PCM_FORMAT_S20_LE,
    SND_PCM_FORMAT_U20 = SND_PCM_FORMAT_U20_LE,
    SND_PCM_FORMAT_S24_3 = SND_PCM_FORMAT_S24_3LE,
    SND_PCM_FORMAT_U24_3 = SND_PCM_FORMAT_U24_3LE,
    SND_PCM_FORMAT_S20_3 = SND_PCM_FORMAT_S20_3LE,
    SND_PCM_FORMAT_U20_3 = SND_PCM_FORMAT_U20_3LE,
    SND_PCM_FORMAT_S18_3 = SND_PCM_FORMAT_S18_3LE,
    SND_PCM_FORMAT_U18_3 = SND_PCM_FORMAT_U18_3LE,
#else
    SND_PCM_FORMAT_S16 = SND_PCM_FORMAT_S16_BE,
    SND_PCM_FORMAT_U16 = SND_PCM_FORMAT_U16_BE,
    SND_PCM_FORMAT_S24 = SND_PCM_FORMAT_S24_BE,
    SND_PCM_FORMAT_U24 = SND_PCM_FORMAT_U24_BE,
    SND_PCM_FORMAT_S32 = SND_PCM_FORMAT_S32_BE,
    SND_PCM_FORMAT_U32 = SND_PCM_FORMAT_U32_BE,
    SND_PCM_FORMAT_FLOAT = SND_PCM_FORMAT_FLOAT_BE,
    SND_PCM_FORMAT_FLOAT64 = SND_PCM_FORMAT_FLOAT64_BE,
    SND_PCM_FORMAT_S20 = SND_PCM_FORMAT_S20_BE,
    SND_PCM_FORMAT_U20 = SND_PCM_FORMAT_U20_BE,
    SND_PCM_FORMAT_S24_3 = SND_PCM_FORMAT_S24_3BE,
    SND_PCM_FORMAT_U24_3 = SND_PCM_FORMAT_U24_3BE,
    SND_PCM_FORMAT_S20_3 = SND_PCM_FORMAT_S20_3BE,
    SND_PCM_FORMAT_U20_3 = SND_PCM_FORMAT_U20_3BE,
    SND_PCM_FORMAT_S18_3 = SND_PCM_FORMAT_S18_3BE,
    SND_PCM_FORMAT_U18_3 = SND_PCM_FORMAT_U18_3BE,
#endif
} snd_pcm_format_t;

typedef snd_pcm_format_t msp_pcm_format_t;

typedef int msp_pcm_uframes_t;
typedef int msp_pcm_sframes_t;

typedef struct msp_pcm_hw_params {
    int   access;
    int   format;       /* snd_pcm_format_t */
    int   subformat;
    int   sample_bits;  /* valid bits per sample */
    int   frame_bits;   /* physical bits per frame */
    int   channels;
    int   rate;
    int   period_time;
    int   period_size;
    int   period_bytes;
    int   periods;
    int   buffer_time;
    int   buffer_size;
    int   buffer_bytes;
    int   tick_time;
    char *pcm_buffer;
    int   pcm_size;
} msp_pcm_hw_params_t;

typedef struct msp_pcm_sw_params {
    int tstamp_mode;
    unsigned int period_step;
    unsigned int sleep_min;
    msp_pcm_uframes_t avail_min;
    msp_pcm_uframes_t xfer_align;
    msp_pcm_uframes_t start_threshold;
    msp_pcm_uframes_t stop_threshold;
    msp_pcm_uframes_t silence_threshold;
    msp_pcm_uframes_t silence_size;
    msp_pcm_uframes_t boundary;
    unsigned int tstamp_type;
    int pads;
    unsigned char reserved[52];
    unsigned int period_event;
} msp_pcm_sw_params_t;

typedef struct _msp_pcm msp_pcm_t;

typedef void (*pcm_event_cb)(msp_pcm_t *pcm, int event_id, void *priv);
typedef struct {
    pcm_event_cb cb;
    void *priv;
} pcm_event_cb_t;

typedef struct msp_pcm_ops {
    int (*hw_params_set)(msp_pcm_t *pcm, struct msp_pcm_hw_params *params);
    int (*hw_get_size)(msp_pcm_t *pcm);
    int (*hw_get_remain_size)(msp_pcm_t *pcm);
    int (*prepare)(msp_pcm_t *pcm);
    int (*pause)(msp_pcm_t *pcm, int enable);
    int (*write)(msp_pcm_t *pcm, void *buf, int size);
    int (*read)(msp_pcm_t *pcm, void *buf, int size);
    int (*set_event)(msp_pcm_t *pcm, pcm_event_cb cb, void *priv);
} msp_pcm_ops_t;

struct _msp_pcm {
    const char *card_name;
    const char *pcm_name;
    void *hdl;
    msp_pcm_state_t state;
    msp_pcm_stream_t stream;
    int mode;
    pcm_event_cb_t event;
    int card;
    int device;
    int ref;
    //x_ringbuf_t ringbuffer;

    msp_mutex_t mutex;
    msp_event_t evt;

    msp_pcm_hw_params_t *hw_params;
    msp_pcm_sw_params_t *sw_params;
    struct msp_pcm_ops *ops;
    msp_slist_t next;
};

typedef struct _msp_pcm_drv {
    msp_driver_t drv;
    struct msp_pcm_ops ops;
} msp_pcm_drv_t;

typedef struct msp_pcm_dev {
    msp_dev_t device;
    msp_pcm_t pcm;
} msp_pcm_dev_t;

int msp_pcm_new(msp_pcm_t **pcm, int type, const char *name, msp_pcm_stream_t stream, int mode);
void msp_pcm_set_ops(msp_pcm_t *pcm, int direction, struct msp_pcm_ops *ops);

int msp_pcm_open(msp_pcm_t **pcm, const char *name, msp_pcm_stream_t stream, int mode);
int msp_pcm_close(msp_pcm_t *pcm);

int msp_pcm_hw_params_current(msp_pcm_t *pcm, msp_pcm_hw_params_t *params);
int msp_pcm_hw_params(msp_pcm_t *pcm, msp_pcm_hw_params_t *params);

int msp_pcm_sw_params_current(msp_pcm_t *pcm, msp_pcm_sw_params_t *params);
int msp_pcm_sw_params(msp_pcm_t *pcm, msp_pcm_sw_params_t *params);

int msp_pcm_drop(msp_pcm_t *pcm);
int msp_pcm_drain(msp_pcm_t *pcm);
int msp_pcm_pause(msp_pcm_t *pcm, int enable);

msp_pcm_sframes_t msp_pcm_avail(msp_pcm_t *pcm);
msp_pcm_sframes_t msp_pcm_writei(msp_pcm_t *pcm, const void *buffer, msp_pcm_uframes_t size);
msp_pcm_sframes_t msp_pcm_readi(msp_pcm_t *pcm, void *buffer, msp_pcm_uframes_t size);
msp_pcm_sframes_t msp_pcm_readn(msp_pcm_t *pcm, void **bufs, msp_pcm_uframes_t size);
int msp_pcm_wait(msp_pcm_t *pcm, int timeout);

int msp_pcm_recover(msp_pcm_t *pcm, int err, int silent);
int msp_pcm_set_params(msp_pcm_t *pcm, snd_pcm_format_t format, msp_pcm_access_t acc, unsigned int channels, unsigned int rate, int soft_resample, unsigned int latency);

int snd_pcm_format_signed(snd_pcm_format_t format);
int snd_pcm_format_float(snd_pcm_format_t format);
int snd_pcm_format_little_endian(snd_pcm_format_t format);
int snd_pcm_format_big_endian(snd_pcm_format_t format);
int snd_pcm_format_width(snd_pcm_format_t format);
int snd_pcm_format_physical_width(snd_pcm_format_t format);

int msp_pcm_hw_params_any(msp_pcm_t *pcm, msp_pcm_hw_params_t *params);
int msp_pcm_hw_params_malloc(msp_pcm_hw_params_t **p);
void msp_pcm_hw_params_free(msp_pcm_hw_params_t *obj);
int msp_pcm_hw_params_alloca(msp_pcm_hw_params_t **p);

int msp_pcm_hw_params_set_access(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_access_t _access);
int msp_pcm_hw_params_get_format(const msp_pcm_hw_params_t *params, snd_pcm_format_t *val);
int msp_pcm_hw_params_set_format(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, snd_pcm_format_t val);
int msp_pcm_hw_params_get_channels(const msp_pcm_hw_params_t *params, unsigned int *val);
int msp_pcm_hw_params_set_channels(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val);
int msp_pcm_hw_params_get_rate(const msp_pcm_hw_params_t *params, unsigned int *val, int *dir);
int msp_pcm_hw_params_set_rate(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val, int dir);
int msp_pcm_hw_params_set_rate_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int *val, int *dir);
int msp_pcm_hw_params_get_period_size(const msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val, int *dir);
int msp_pcm_hw_params_set_period_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val, int *dir);
int msp_pcm_hw_params_get_buffer_size(const msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val);
int msp_pcm_hw_params_set_buffer_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val);

int msp_pcm_sw_params_alloca(msp_pcm_sw_params_t **p);
void msp_pcm_sw_params_free(msp_pcm_sw_params_t *obj);
int msp_pcm_sw_params_set_start_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val);
int msp_pcm_sw_params_get_start_threshold(const msp_pcm_sw_params_t *params, msp_pcm_uframes_t *val);
int msp_pcm_sw_params_set_stop_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val);

msp_pcm_sframes_t msp_pcm_bytes_to_frames(msp_pcm_t *pcm, ssize_t bytes);
ssize_t msp_pcm_frames_to_bytes(msp_pcm_t *pcm, msp_pcm_sframes_t frames);

#ifdef __cplusplus
}
#endif

#endif
