/*********************************************************************
 * File Name    : librtspsrv.h
 * Description  : 
 * Author       : Hu Lizhen
 * Create Date  : 2012-11-15
 ********************************************************************/

#ifndef __LIBRTSPSRV_H__
#define __LIBRTSPSRV_H__


#if defined (__cplusplus) || defined (_cplusplus)
extern "C" {
#endif
#include <stdint.h>
#define DFL_RTSP_PORT   8554   /* Default RTSP port. */
#define DFL_HTTP_PORT   0       /* Not support RTSP over HTTP yet. */

#define MAX_CHN_NUM 3                 /* Max channel number. */
#define MAX_FRM_SZ  (1 * 100 * 1024) /* Max frame size bytes. */

enum {
    RTSP_STRM_REPORT_CLIENT_EXIT = 0,
    RTSP_STRM_REPORT_MAX,        
};
    
/* Frame type. */
enum frm_type {
    FRM_TYPE_I = 1,
    FRM_TYPE_B,
    FRM_TYPE_P,
    FRM_TYPE_A,
    FRM_TYPE_M,
    FRM_TYPE_MAX,
};

enum chn_type {
    CHN_TYPE_MAIN,
    CHN_TYPE_MINOR,
};

enum {
    RTSP_AUDIOFMT_AAC = 0,
    RTSP_AUDIOFMT_PCMA,
    RTSP_AUDIOFMT_PCMU,
    RTSP_AUDIOFMT_OPUS,
};

enum {
    RTSP_VIDEOFMT_MJPEG = 0,
    RTSP_VIDEOFMT_H264,
};

/* Stream resource information. */
struct strm_info {
    int chn_no;                 /* Channel number. */
    enum chn_type chn_type;
    char *file_name;            /* Local AV file name. */
    void *usr_data;
};

/* Frame information used for getting frame. */
struct frm_info {
    unsigned int frm_sz;        /* Frame size. */
    enum frm_type frm_type;     /* Frame type. */
    char *frm_buf;              /* Frame buffer: store pure av data. */
    uint32_t timestamp;
};

/*
 * Callback function to start, stop and get media resource.
 */
typedef int (*get_frm_t)(struct strm_info *, struct frm_info *);

typedef void (*rtsp_report_fn_t)(int action);

void set_strm_cb(get_frm_t get);
void get_strm_cb(get_frm_t *get);
int rtsp_init_lib(void);
int rtsp_deinit_lib(void);
void rtsp_set_video_en(int en);
void rtsp_set_audio_en(int en);
int rtsp_get_audio_en(void);
int rtsp_get_video_en(void);
int rtsp_get_audioFmt(void);
int rtsp_get_videoFmt(void);
void rtsp_set_videoFmt(int vdofmt);
void rtsp_set_audioFmt(int audfmt);
void rtsp_strm_report_cb(int action);
void rtsp_set_strm_report_cb(rtsp_report_fn_t fn);
void rtsp_set_video_fps(int fps);
int rtsp_get_video_fps(void);

#if defined (__cplusplus) || defined (_cplusplus)
}
#endif

#endif /* __LIBRTSPSRV_H__ */

