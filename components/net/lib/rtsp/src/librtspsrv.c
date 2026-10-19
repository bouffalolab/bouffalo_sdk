#include "librtspsrv.h"
#include "rtsp_srv.h"
#include "rtp.h"
#include "util.h"
#include "log.h"

/* RTSP stream relative callback functions. */
static get_frm_t get_frm;
static rtsp_report_fn_t report_fn;
static int gAudio_en = 0;
static int gVideo_en = 1;
static int gVideo_fps = 25;
static int gAudioFmt = RTP_PT_AAC;
static int gVideoFmt = RTP_PT_JPEG;

void rtsp_set_video_fps(int fps)
{
    gVideo_fps = fps;
}

int rtsp_get_video_fps(void)
{
    return gVideo_fps;
}

void set_strm_cb(get_frm_t get)
{
    get_frm = get;
    return;
}

void rtsp_set_strm_report_cb(rtsp_report_fn_t fn)
{
    report_fn = fn;
}

void get_strm_cb(get_frm_t *get)
{
    if (get) {
        *get = get_frm;
    }
    return;
}

void rtsp_strm_report_cb(int action)
{
    if (report_fn) {
        report_fn(action);
    }
    return;
}


void rtsp_set_audioFmt(int audfmt)
{
    if (audfmt  == RTSP_AUDIOFMT_AAC)
        gAudioFmt = RTP_PT_AAC;
    else if (audfmt == RTSP_AUDIOFMT_PCMA)
        gAudioFmt = RTP_PT_PCMA;
    else if (audfmt == RTSP_AUDIOFMT_PCMU)
        gAudioFmt = RTP_PT_PCMU;
    else if (audfmt == RTSP_AUDIOFMT_OPUS)
        gAudioFmt = RTP_PT_OPUS;
}

void rtsp_set_videoFmt(int vdofmt)
{
    if (vdofmt  == RTSP_VIDEOFMT_H264)
        gVideoFmt = RTP_PT_H264;
    else if (vdofmt == RTSP_VIDEOFMT_MJPEG)
        gVideoFmt = RTP_PT_JPEG;
  
}

int rtsp_get_videoFmt(void)
{
    return gVideoFmt;
}

int rtsp_get_audioFmt(void)
{
    return gAudioFmt;
}

int rtsp_get_audio_en(void)
{
    return gAudio_en;
}

int rtsp_get_video_en(void)
{
    return gVideo_en;
}

void rtsp_set_video_en(int en)
{
    gVideo_en = en;
}

void rtsp_set_audio_en(int en)
{
    gAudio_en = en;
}

int rtsp_init_lib(void)
{
    if (!get_frm) {
        printf("*** RTSP stream callback MUST be set properly!\r\n");
        return -1;
    }
    start_rtsp_srv();
    return 0;
}

int rtsp_deinit_lib(void)
{
    stop_rtsp_srv();
    return 0;
}
