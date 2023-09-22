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

//提供管理数字音频的捕捉和回放
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

typedef int msp_pcm_uframes_t;
typedef int msp_pcm_sframes_t;

typedef struct msp_pcm_hw_params {
    int   access;
    int   format;
    int   subformat;
    int   sample_bits;
    int   frame_bits;
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

int msp_pcm_hw_params_alloca(msp_pcm_hw_params_t **p);
int msp_pcm_sw_params_alloca(msp_pcm_sw_params_t **p);

int msp_pcm_open(msp_pcm_t **pcm, const char *name, msp_pcm_stream_t stream, int mode);
int msp_pcm_close(msp_pcm_t *pcm);

int msp_pcm_pause(msp_pcm_t *pcm, int enable);
int msp_pcm_drop(msp_pcm_t *pcm);
int msp_pcm_drain(msp_pcm_t *pcm);

int msp_pcm_hw_params_set_access(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_access_t _access);
int msp_pcm_hw_params_set_format(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, int val);
int msp_pcm_hw_params_set_buffer_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val);
int msp_pcm_hw_params_set_period_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val, int *dir);
int msp_pcm_hw_params_current(msp_pcm_t *pcm, msp_pcm_hw_params_t *params);
int msp_pcm_hw_free(msp_pcm_t *pcm);
int msp_pcm_hw_params_any(msp_pcm_t *pcm, msp_pcm_hw_params_t *params);
int msp_pcm_hw_params(msp_pcm_t *pcm, msp_pcm_hw_params_t *params);
int msp_pcm_prepare(msp_pcm_t *pcm);
int msp_pcm_wait(msp_pcm_t *pcm, int timeout);
int msp_pcm_recover(msp_pcm_t *pcm, int err, int silent);

int msp_pcm_hw_params_set_channels(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val);
int msp_pcm_hw_params_set_rate(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val, int dir);
int msp_pcm_hw_params_set_rate_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int *val, int *dir);
int msp_pcm_hw_params_set_rate_resample(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val);
int msp_pcm_hw_params_set_export_buffer(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val);
int msp_pcm_hw_params_set_period_wakeup(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val);
int msp_pcm_hw_params_set_period_time(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val, int dir);
int msp_pcm_hw_params_set_period_size(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t val, int dir);
int msp_pcm_hw_params_set_periods(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val, int dir);
int msp_pcm_hw_params_set_buffer_time(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val, int dir);
int msp_pcm_hw_params_set_buffer_size(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t val);
int msp_pcm_set_params(msp_pcm_t *pcm, int format, msp_pcm_access_t acc, unsigned int channels, unsigned int rate, int soft_resample, unsigned int latency);


int msp_pcm_sw_params_current(msp_pcm_t *pcm, msp_pcm_sw_params_t *params);
int msp_pcm_sw_params_set_tstamp_mode(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, int val);
int msp_pcm_sw_params_set_avail_min(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val);
int msp_pcm_sw_params_set_period_event(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, int val);
int msp_pcm_sw_params_set_start_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val);
int msp_pcm_sw_params_set_stop_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val);
int msp_pcm_sw_params_set_silence_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val);
int msp_pcm_sw_params_set_silence_size(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val);
int msp_pcm_sw_params(msp_pcm_t *pcm, msp_pcm_sw_params_t *params);

msp_pcm_sframes_t msp_pcm_avail(msp_pcm_t *pcm);
msp_pcm_sframes_t msp_pcm_writei(msp_pcm_t *pcm, const void *buffer, msp_pcm_uframes_t size);
msp_pcm_sframes_t msp_pcm_readi(msp_pcm_t *pcm, void *buffer, msp_pcm_uframes_t size);
msp_pcm_sframes_t msp_pcm_writen(msp_pcm_t *pcm, void **bufs, msp_pcm_uframes_t size);
msp_pcm_sframes_t msp_pcm_readn(msp_pcm_t *pcm, void **bufs, msp_pcm_uframes_t size);


msp_pcm_sframes_t msp_pcm_bytes_to_frames(msp_pcm_t *pcm, ssize_t bytes);
ssize_t msp_pcm_frames_to_bytes(msp_pcm_t *pcm, msp_pcm_sframes_t frames);

#ifdef __cplusplus
}
#endif

#endif
