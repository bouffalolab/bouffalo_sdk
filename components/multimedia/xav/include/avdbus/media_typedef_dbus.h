/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_AV_DBUS_SUPPORT) && CONFIG_AV_DBUS_SUPPORT
#ifndef __MEDIA_TYPEDEF_DBUS_H__
#define __MEDIA_TYPEDEF_DBUS_H__

#include "avutil/common.h"

__BEGIN_DECLS__

#define MEDIA_DBUS_SERVER                           "org.media.server"
#define MEDIA_DBUS_PATH                             "/org/media/path"
#define MEDIA_DBUS_INTERFACE                        "org.media.interface"

#define DBUS_OBJECT_PATH_MAX                        150
#define DBUS_INTERFACE_MAX                          150
#define DBUS_METHOD_SIGNAL_PROP_MAX                 50
#define DBUS_AUTH_MODE_MAX                          64

#define MEDIA_DBUS_METHOD_PLAY                      "Play"
#define MEDIA_DBUS_METHOD_PAUSE                     "Pause"
#define MEDIA_DBUS_METHOD_RESUME                    "Resume"
#define MEDIA_DBUS_METHOD_STOP                      "Stop"
#define MEDIA_DBUS_METHOD_SEEK                      "Seek"
#define MEDIA_DBUS_METHOD_SET_SPEED                 "SetSpeed"
#define MEDIA_DBUS_METHOD_GET_SPEED                 "GetSpeed"
#define MEDIA_DBUS_METHOD_MUTE                      "Mute"
#define MEDIA_DBUS_METHOD_UNMUTE                    "UnMute"
#define MEDIA_DBUS_METHOD_GET_VOL                   "GetVol"
#define MEDIA_DBUS_METHOD_SET_VOL                   "SetVol"
#define MEDIA_DBUS_METHOD_GET_STATUS                "GetStatus"
#define MEDIA_DBUS_METHOD_GET_TIME                  "GetTime"
#define MEDIA_DBUS_METHOD_GET_MEDIA_INFO            "GetMediaInfo"
#define MEDIA_DBUS_METHOD_SWITCH_AUDIO_TRACK        "SwitchAudioTrack"
#define MEDIA_DBUS_METHOD_SWITCH_SUBTITLE_TRACK     "SwitchSubtitleTrack"
#define MEDIA_DBUS_METHOD_SET_SUBTITLE_URL          "SetSubtitleUrl"
#define MEDIA_DBUS_METHOD_SET_SUBTITLE_VISIBLE      "SetSubtitleVisible"
#define MEDIA_DBUS_METHOD_SET_VIDEO_VISIBLE         "SetVideoVisible"
#define MEDIA_DBUS_METHOD_SET_VIDEO_CROP            "SetVideoCrop"
#define MEDIA_DBUS_METHOD_SET_DISPLAY_WINDOW        "SetDisplayWindow"
#define MEDIA_DBUS_METHOD_SET_FULLSCREEN            "SetFullScreen"
#define MEDIA_DBUS_METHOD_SET_DISPLAY_FORMAT        "SetDisplayFormat"
#define MEDIA_DBUS_METHOD_SET_VIDEO_ROTATE          "SetVideoRotate"

#define MEDIA_DBUS_SIGNAL_PLAY_ERROR                "PlayError"
#define MEDIA_DBUS_SIGNAL_PLAY_START                "PlayStart"
#define MEDIA_DBUS_SIGNAL_PLAY_PAUSE                "PlayPause"
#define MEDIA_DBUS_SIGNAL_PLAY_RESUME               "PlayResume"
#define MEDIA_DBUS_SIGNAL_PLAY_STOP                 "PlayStop"
#define MEDIA_DBUS_SIGNAL_PLAY_FINISH               "PlayFinish"
#define MEDIA_DBUS_SIGNAL_PLAY_UNDER_RUN            "PlayUnderRun"
#define MEDIA_DBUS_SIGNAL_PLAY_OVER_RUN             "PlayOverRun"
#define MEDIA_DBUS_SIGNAL_PLAY_MUTE                 "PlayMute"
#define MEDIA_DBUS_SIGNAL_PLAY_VOL_CHANGE           "PlayVolChange"

__END_DECLS__

#endif /* __MEDIA_TYPEDEF_DBUS_H__ */
#endif

