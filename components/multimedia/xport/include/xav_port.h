/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __XAV_PORT_H__
#define __XAV_PORT_H__

int config_av_stream_cache_size_default(void);
int config_player_task_stack_size(void);
int config_web_cache_task_stack_size(void);
int config_av_stream_cache_threshold_default(void);
int config_av_probe_size_max(void);
int config_av_sample_num_per_frame_max(void);
int config_av_atempoer_enable(void);
int config_av_mixer_enable(void);
int config_av_aef_enable(void);

#endif
