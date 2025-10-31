/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __CODEC_HFP_H__
#define __CODEC_HFP_H__

#include <stdint.h>

typedef struct
{
    uint16_t sample_rate;
    uint8_t bit_width;
    //uint8_t sound_channel_num;
} hfp_codec_cfg_t;

int codec_hfp_init(void);
int codec_hfp_deinit(void);
/* input */
void hfp_codec_input_init(hfp_codec_cfg_t config);
void hfp_codec_input_config(uint32_t buffer0, uint32_t bufffer1, uint32_t buf_size);
//void hfp_codec_input_start(void);
//void hfp_codec_input_stop(void);

/* output */
void hfp_codec_output_init(hfp_codec_cfg_t config);
void hfp_codec_output_config(uint32_t buffer0, uint32_t bufffer1, uint32_t buf_size);
//void hfp_codec_output_start(void);
//void hfp_codec_output_stop(void);
void codec_bt_hfp_cb(uint16_t interval_halfslot,
                        uint32_t tx_buffer_0,
                        uint32_t tx_buffer_1,
                        uint32_t rx_buffer_0,
                        uint32_t rx_buffer_1,
                        uint32_t tx_buffer_size,
                        uint32_t rx_buffer_size,
                        uint32_t start_time_halfslot,
                        uint8_t buffer_index);
#endif /* __CODEC_HFP_H__ */
