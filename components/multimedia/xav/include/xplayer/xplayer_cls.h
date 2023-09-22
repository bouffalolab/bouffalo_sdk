/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __XPLAYER_CLS_H__
#define __XPLAYER_CLS_H__

#include "avutil/common.h"
#include "xplayer/xplayer_typedef.h"
#ifdef __linux__
#include "xutils/list.h"
#endif

__BEGIN_DECLS__

typedef struct xplayer_struct xplayer_t;
typedef struct xplayer_cls xplayer_cls_t;

struct xplayer_struct  {
    char                         *url;
    xplayer_eventcb_t            event_cb;            ///< callback of the xplayer event
    void                         *user_data;          ///< user data for event_cb
    uint64_t                     start_time;          ///< begin play time
    enum xplayer_status          status;

    uint8_t                      vol;
    float                        speed;
    struct xplayer_cls           *cls;
    msp_mutex_t                  lock;
};

#define GET_DERIVED_PLAYER(p) (((char*)(p)) + sizeof(xplayer_t))

struct xplayer_ops  {
    int   (*init)                   (xplayer_t *xplayer);
    int   (*get_config)             (xplayer_t *xplayer, xplayer_cnf_t *conf);
    int   (*set_config)             (xplayer_t *xplayer, const xplayer_cnf_t *conf);
    int   (*play)                   (xplayer_t *xplayer);
    int   (*pause)                  (xplayer_t *xplayer);
    int   (*resume)                 (xplayer_t *xplayer);
    int   (*seek)                   (xplayer_t *xplayer, uint64_t timestamp);
    int   (*stop)                   (xplayer_t *xplayer);
    void  (*uninit)                 (xplayer_t *xplayer);

    int   (*get_media_info)         (xplayer_t *xplayer, xmedia_info_t *minfo);
    int   (*get_time)               (xplayer_t *xplayer, xplay_time_t *time);
    int   (*set_speed)              (xplayer_t *xplayer, float speed);
    int   (*set_vol)                (xplayer_t *xplayer, uint8_t vol);
    int   (*set_mute)               (xplayer_t *xplayer, uint8_t mute);

#if !CONFIG_AV_AUDIO_ONLY_SUPPORT
    int   (*switch_audio_track)     (xplayer_t *xplayer, uint8_t idx);
    int   (*switch_subtitle_track)  (xplayer_t *xplayer, uint8_t idx);
    int   (*set_subtitle_url)       (xplayer_t *xplayer, const char *url);
    int   (*set_subtitle_visible)   (xplayer_t *xplayer, uint8_t visible);
    int   (*set_video_visible)      (xplayer_t *xplayer, uint8_t visible);
    int   (*set_video_crop)         (xplayer_t *xplayer, const xwindow_t *win);
    int   (*set_fullscreen)         (xplayer_t *xplayer, uint8_t onoff);
    int   (*set_display_window)     (xplayer_t *xplayer, const xwindow_t *win);
    int   (*set_display_format)     (xplayer_t *xplayer, xdisplay_format_t format);
    int   (*set_video_rotate)       (xplayer_t *xplayer, xrotate_type_t type);
#endif
};


struct xplayer_cls  {
    const char                   *name;
    size_t                       priv_size;
    const struct xplayer_ops     *ops;
    msp_slist_t                      node;       ///< for xplayer class list
};

__END_DECLS__

#endif /* __XPLAYER_CLS_H__ */

