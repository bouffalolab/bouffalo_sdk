/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef CLI_XCODEC_H
#define CLI_XCODEC_H

#include "xcodec_dev.h"

int xcodec_cli_init(void);
/* Execute queued commands from the audio task. */
void xcodec_cli_process(xcodec_dev_handle_t play_dev);

/* CLI helpers implemented in main.c, called from the audio task. */
int audio_set_echo_channel(int channel);
void audio_start_tone(int amplitude);
void audio_print_status(void);

#endif
