/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */
#include "avutil/av_config.h"

#if defined(CONFIG_AEF_EQ_ENABLE) && CONFIG_AEF_EQ_ENABLE

#include <stdlib.h>
#include <stdio.h>
#include <xcodec.h>
#include "audio_file.h"
#include <aef/aef_param.h>

#define TEST_SAMPLERATE 16000

static void* g_comprIns   = NULL;
static void* g_compandIns = NULL;
static void* g_limitIns   = NULL;
static void* g_eqIns      = NULL;
static void* g_gainIns    = NULL;

t_aefCompanderParam g_companderParam = {
                                        .enable = 1,
                                        .compress_trd = -9.5,
                                        .expand_trd = -67.1,
                                        .attack_time = 0.005,
                                        .release_time = 0.05,
                                        .average_time = 0.05,
                                        .hold_time = 0.05,
                                        .compr_ratio = 1.5,
                                        .expand_ratio = 2.0,
                                        .width = 8
                                        };

t_aefCompressorParam g_compressorParam = {
                                        .enable = 1,
                                        .threshold=-10,
                                        .attack_time = 0.005,
                                        .release_time=0.320,
                                        .average_time = 0.010,
                                        .ratio = 15,
                                        .width=5
                                        };

t_aefLimitParam g_limitParam = {
                                .enable = 1,
                                .threshold = -9.4,
                                .attack_time = 0.005,
                                .release_time = 0.320
                                };

t_aefEqFilterParam g_eq_params[] = {
                                    {1, AEF_BQF_HP2, 0.0, 0.7, 50},
                                    {1, AEF_BQF_HP2, 0.0, 0.8, 60},
                                    {1, AEF_BQF_PEAK, 1.8, 1.2, 60},
                                    {1, AEF_BQF_HS2, 6.2, 0.54, 4000},
                                    {1, AEF_BQF_PEAK, 5.0, 2.2, 5400},
                                    {1, AEF_BQF_PEAK, 5.0, 2.5, 10500},
                                    {0, AEF_BQF_PEAK, 2.6, 1.3, 180},
                                    {0, AEF_BQF_PEAK, 2.6, 1.3, 180},
                                    {0, AEF_BQF_PEAK, 2.6, 1.3, 180},
                                    {0, AEF_BQF_PEAK, 2.6, 1.3, 180},
                                    };

t_aefGainParam g_gainParam = {
                            .enable = 1,
                            .mute = 0,
                            .gain = 0.8,
                            .phase = 0
                            };

static int sona_init(int samplerate)
{
    // compressor
    g_comprIns = aef_compressor_init(samplerate, &g_compressorParam);
    if (g_comprIns == NULL) {
        printf("comressor init failed\n");
        return -1;
    }
    // compander
    g_compandIns = aef_compander_init(samplerate, &g_companderParam);
    if (g_compandIns == NULL) {
        printf("compander init failed\n");
        return -1;
    }
    // limit
    g_limitIns = aef_limiter_init(samplerate, &g_limitParam);
    if (g_limitIns == NULL) {
        printf("limiter init failed\n");
        return -1;
    }
    //gain
    g_gainIns = aef_gain_init(&g_gainParam);
    if (g_gainIns == NULL) {
        printf("gain init failed\n");
        return -1;
    }
    //eq
    g_eqIns = aef_eq_init(samplerate, (sizeof(g_eq_params)/sizeof(t_aefEqFilterParam)));
    if (g_eqIns == NULL) {
        printf("eq init failed\n");
        return -1;
    }
    aef_eq_set_enable(g_eqIns, 1) ;// globle eq enable, defaut is 1 ON in init, so no need call if want enable
    for (int i = 0; i < (sizeof(g_eq_params)/sizeof(t_aefEqFilterParam)); i++) {
        if (aef_eq_set_param(g_eqIns, i, &g_eq_params[i]) < 0) {
            printf("eq bqf[%d] set failed\n", i);
            return -1;
        }
    }

    return 0;
}

static void sona_process(float *data, int samples)
{
    aef_eq_process(g_eqIns, data, data,samples);//data in/out can be same buffer
    aef_compressor_process(g_comprIns, data, data, samples);//data in/out can be same buffer
    aef_compander_process(g_compandIns, data, data, samples);//data in/out can be same buffer
    aef_limiter_process(g_limitIns, data, data, samples);//data in/out can be same buffer
    aef_gain_process(g_gainIns, data, data, samples);
}

static inline int16_t _round16(float data)
{
    if (data < 0) {
        data -= 0.5;
    } else if (data > 0) {
        data += 0.5;
    }

    if (data > 32767) {
        data = 32767;
    } else if (data <-32768) {
        data = -32768;
    }

    return (int16_t)data;
}

#ifdef CONFIG_SHELL
static void cmd_peq_set_func(int argc, char **argv)
#else
static void cmd_peq_set_func(char *buf, int len, int argc, char **argv)
#endif
{   
    int ret = -1;

    if (argc < 2) {
        return;
    }

    if (g_eqIns == NULL) {
        printf("please init eq!\r\n");
        return;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "GET") == 0) {
            char *aefBQFType;
            for (int i = 0; i < (sizeof(g_eq_params)/sizeof(t_aefEqFilterParam)); i++) {
                if (AEF_BQF_HS2 == g_eq_params[i].type) {
                    aefBQFType = "HS2";
                } else if (AEF_BQF_LS2 == g_eq_params[i].type) {
                    aefBQFType = "LS2";
                } else if (AEF_BQF_HP1 == g_eq_params[i].type) {
                    aefBQFType = "HP1";
                } else if (AEF_BQF_HP2 == g_eq_params[i].type) {
                    aefBQFType = "HP2";
                } else if (AEF_BQF_LP1 == g_eq_params[i].type) {
                    aefBQFType = "LP1";
                } else if (AEF_BQF_LP2 == g_eq_params[i].type) {
                    aefBQFType = "LP2";
                } else if (AEF_BQF_PEAK == g_eq_params[i].type) {
                    aefBQFType = "Peak";
                } else if (AEF_BQF_NOTCH == g_eq_params[i].type) {
                    aefBQFType = "Notch";
                } else {
                    printf("eq get error\r\n");
                    return; 
                }

                if(g_eq_params[i].enable){
                    printf("MSP_PEQ GET %d ON %s %.2f %.3f %.1f\r\n", i, aefBQFType, g_eq_params[i].gain, g_eq_params[i].q, g_eq_params[i].freq);
                }else{
                    printf("MSP_PEQ GET %d OFF %s %.2f %.3f %.1f\r\n", i, aefBQFType, g_eq_params[i].gain, g_eq_params[i].q, g_eq_params[i].freq);
                }
            }
        } else {
            printf("param is error:%s\r\n", argv[1]);
            return;
        }
    }else if (argc == 8) {
        if (strcmp(argv[1], "SET") == 0) {
            int id;
            id = atoi(argv[2]);
            /*enable*/
            if (strcmp(argv[3], "ON") == 0) {
                g_eq_params[id].enable = 1;
            } else if (strcmp(argv[3], "OFF") == 0) {
                g_eq_params[id].enable = 0;
            } else {
                printf("param is error:%s\r\n", argv[2]);
                return;
            }
            /*type*/
            if (strcmp(argv[4], "HS2") == 0) {
                g_eq_params[id].type = AEF_BQF_HS2;
            } else if (strcmp(argv[4], "LS2") == 0) {
                g_eq_params[id].type = AEF_BQF_LS2;
            } else if (strcmp(argv[4], "HP1") == 0) {
                g_eq_params[id].type = AEF_BQF_HP1;
            } else if (strcmp(argv[4], "HP2") == 0) {
                g_eq_params[id].type = AEF_BQF_HP2;
            } else if (strcmp(argv[4], "LP1") == 0) {
                g_eq_params[id].type = AEF_BQF_LP1;
            } else if (strcmp(argv[4], "LP2") == 0) {
                g_eq_params[id].type = AEF_BQF_LP2;
            } else if (strcmp(argv[4], "Peak") == 0) {
                g_eq_params[id].type = AEF_BQF_PEAK;
            } else if (strcmp(argv[4], "Notch") == 0) {
                g_eq_params[id].type = AEF_BQF_NOTCH;
            } else {
               return; 
            }
            
            g_eq_params[id].gain = atof(argv[5]);
            g_eq_params[id].q = atof(argv[6]);
            g_eq_params[id].freq = atof(argv[7]);
            ret = aef_eq_set_param(g_eqIns, id, &g_eq_params[id]);
            if (ret == 0) {
                printf("set eq success!\r\n");
                return;
            } else {
                printf("set eq failed!\r\n");
                return;
            }
        } else {
            printf("param is error:%s\r\n", argv[1]);
            return;
        }
    } else if (argc == 3 || argc == 4) {
        if (strcmp(argv[1], "SET") == 0) {
            if(argc == 3){
                if (strcmp(argv[2], "ON") == 0) {
                    ret = aef_eq_set_enable(g_eqIns, 1);
                    if (ret == 0) {
                        printf("enable eq!\r\n");
                    }
                } else if (strcmp(argv[2], "OFF") == 0) {
                    ret = aef_eq_set_enable(g_eqIns, 0);
                    if (ret == 0) {
                        printf("disable eq!\r\n");
                    }
                } else {
                    printf("param is error\r\n");
                    return;
                }
            }else if(argc == 4){
                int id;
                id = atoi(argv[2]);
                if (strcmp(argv[3], "ON") == 0) {
                    g_eq_params[id].enable = 1;
                    ret = aef_eq_set_param(g_eqIns, id, &g_eq_params[id]);
                    if (ret == 0) {
                        printf("enable eq!\r\n");
                    }
                } else if (strcmp(argv[3], "OFF") == 0) {
                    g_eq_params[id].enable = 0;
                    ret = aef_eq_set_param(g_eqIns, id, &g_eq_params[id]);
                    if (ret == 0) {
                        printf("disable eq!\r\n");
                    }
                } else {
                    printf("param is error\r\n");
                    return;
                }
            }
        }else {
            printf("param is error\r\n");
            return;
        }
    }
}

#ifdef CONFIG_SHELL
static void cmd_limiter_set_func(int argc, char **argv)
#else
static void cmd_limiter_set_func(char *buf, int len, int argc, char **argv)
#endif
{
    int ret = -1;

    if (g_limitIns == NULL) {
        printf("please init limit!\r\n");
        return;
    }

    if (argc == 2) {
        if (strcmp(argv[1], "GET") == 0) {
            if(g_limitParam.enable){
                printf("MSP_LIMITER GET ON %.2f %.3f %.3f\r\n", g_limitParam.threshold, g_limitParam.attack_time*1000, g_limitParam.release_time*1000);
            }else{
                printf("MSP_LIMITER GET OFF %.2f %.3f %.3f\r\n", g_limitParam.threshold, g_limitParam.attack_time*1000, g_limitParam.release_time*1000);
            }
        } else {
            printf("param error!\r\n");
        }
    } else if (argc == 6) {
        if (strcmp(argv[2], "ON") == 0) {
            g_limitParam.enable = 1;    
        } else if (strcmp(argv[2], "OFF") == 0) {
            g_limitParam.enable = 0;
            ret = aef_limiter_set_param(g_limitIns, &g_limitParam);
            if (ret == 0) {
                printf("set limiter success!\r\n");
            } else {
                printf("set limiter failed!\r\n");
            }
            return;
        } else {
            printf("param is error:%s\r\n", argv[1]);
            return;
        }

        g_limitParam.threshold = atof(argv[3]);
        g_limitParam.attack_time = atof(argv[4])/1000;
        g_limitParam.release_time = atof(argv[5])/1000;
        ret = aef_limiter_set_param(g_limitIns, &g_limitParam);
        if (ret == 0) {
            printf("set limiter success!\r\n");
        } else {
            printf("set limiter failed!\r\n");
        }
    } else {
        printf("param error!\r\n");
    }
}

#ifdef CONFIG_SHELL
static void cmd_gain_set_func(int argc, char **argv)
#else
static void cmd_gain_set_func(char *buf, int len, int argc, char **argv)
#endif
{
    int ret = -1;

    if (g_gainIns == NULL) {
        printf("please init gain!\r\n");
        return;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "GET") == 0) {
            if(g_gainParam.enable){
                printf("MSP_GAIN GET ON %d %.2f %d\r\n", g_gainParam.mute, g_gainParam.gain, g_gainParam.phase);
            }else{
                printf("MSP_GAIN GET OFF %d %.2f %d\r\n", g_gainParam.mute, g_gainParam.gain, g_gainParam.phase);
            }
        } else {
            printf("param error!\r\n");
        }
    } else if (argc == 6) {
        if (strcmp(argv[2], "ON") == 0) {
            g_gainParam.enable = 1;    
        } else if (strcmp(argv[2], "OFF") == 0) {
            g_gainParam.enable = 0;
            ret = aef_gain_set_param(g_gainIns, &g_gainParam);
            if (ret == 0) {
                printf("set gain success!\r\n");
            } else {
                printf("set gain failed!\r\n");
            }
            return;
        } else {
            printf("param is error:%s\r\n", argv[2]);
            return;
        }

        g_gainParam.mute= atoi(argv[3]);
        g_gainParam.gain = atof(argv[4]);
        g_gainParam.phase = atoi(argv[5]);
        ret = aef_gain_set_param(g_gainIns, &g_gainParam);
        if (ret == 0) {
            printf("set gain success!\r\n");
        } else {
            printf("set gain failed!\r\n");
        }
    } else {
        printf("param error!\r\n");
    }
}

#ifdef CONFIG_SHELL
static void cmd_compander_set_func(int argc, char **argv)
#else
static void cmd_compander_set_func(char *buf, int len, int argc, char **argv)
#endif
{
    int ret = -1;

    if (g_compandIns == NULL) {
        printf("please init compander!\r\n");
        return;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "GET") == 0) {
            char *enable;
            if(g_companderParam.enable){
                enable = "ON";
            }else{
                enable = "OFF";
            }
            printf("MSP_COMPAND GET %s %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\r\n", enable, g_companderParam.attack_time*1000, g_companderParam.release_time*1000, g_companderParam.average_time*1000,
                                                                                                    g_companderParam.hold_time*1000, g_companderParam.expand_trd, g_companderParam.expand_ratio,
                                                                                                  g_companderParam.compress_trd, g_companderParam.compr_ratio,  g_companderParam.width);
        } else {
            printf("param error!\r\n");
        }
    } else if (argc == 12) {
        if (strcmp(argv[2], "ON") == 0) {
            g_companderParam.enable = 1;    
        } else if (strcmp(argv[2], "OFF") == 0) {
            g_companderParam.enable = 0;
            ret = aef_compander_set_param(g_compandIns, &g_companderParam);
            if (ret == 0) {
                printf("set compander success!\r\n");
            } else {
                printf("set compander failed!\r\n");
            }
            return;
        } else {
            printf("param is error:%s\r\n", argv[2]);
            return;
        }

        g_companderParam.attack_time = atof(argv[3])/1000;
        g_companderParam.release_time = atof(argv[4])/1000;
        g_companderParam.average_time = atof(argv[5])/1000;
        g_companderParam.hold_time = atof(argv[6])/1000;
        g_companderParam.expand_trd = atof(argv[7]);
        g_companderParam.expand_ratio = atof(argv[8]);
        g_companderParam.compress_trd = atof(argv[9]);
        g_companderParam.compr_ratio = atof(argv[10]);
        g_companderParam.width = atof(argv[11]);
        
        ret = aef_compander_set_param(g_compandIns, &g_companderParam);
        if (ret == 0) {
            printf("set compander success!\r\n");
        } else {
            printf("set compander failed!\r\n");
        }
    } else {
        printf("param error!\r\n");
    }
}

#ifdef CONFIG_SHELL
static void cmd_compressor_set_func(int argc, char **argv)
#else
static void cmd_compressor_set_func(char *buf, int len, int argc, char **argv)
#endif
{
    int ret = -1;

    if (g_comprIns == NULL) {
        printf("please init compressor!\r\n");
        return;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "GET") == 0) {
            char *enable;
            if(g_compressorParam.enable){
                enable = "ON";
            }else{
                enable = "OFF";
            }
            printf("MSP_COMPRESSOR GET %s %.2f %.4f %.4f %.4f %.4f %.4f\r\n", enable, g_compressorParam.threshold, g_compressorParam.attack_time*1000, g_compressorParam.release_time*1000, 
                                                                                      g_compressorParam.average_time*1000, g_compressorParam.ratio, g_compressorParam.width);
        } else {
            printf("param error!\r\n");
        }
    } else if (argc == 9) {
        if (strcmp(argv[2], "ON") == 0) {
            g_compressorParam.enable = 1;    
        } else if (strcmp(argv[2], "OFF") == 0) {
            g_compressorParam.enable = 0;
            ret = aef_compressor_set_param(g_comprIns, &g_compressorParam);
            if (ret == 0) {
                printf("set compressor success!\r\n");
            } else {
                printf("set compressor failed!\r\n");
            }
            return;
        } else {
            printf("param is error:%s\r\n", argv[2]);
            return;
        }

        g_compressorParam.threshold = atof(argv[3]);
        g_compressorParam.attack_time = atof(argv[4])/1000;
        g_compressorParam.release_time = atof(argv[5])/1000;
        g_compressorParam.average_time = atof(argv[6])/1000;
        g_compressorParam.ratio = atof(argv[7]);
        g_compressorParam.width = atof(argv[8]);
        
        ret = aef_compressor_set_param(g_comprIns, &g_compressorParam);
        if (ret == 0) {
            printf("set compressor success!\r\n");
        } else {
            printf("set compressor failed!\r\n");
        }
    } else {
        printf("param error!\r\n");
    }
}

// cli set up
#ifdef CONFIG_SHELL
#include <shell.h>
SHELL_CMD_EXPORT_ALIAS(cmd_peq_set_func, MSP_PEQ, set peq);
SHELL_CMD_EXPORT_ALIAS(cmd_limiter_set_func, MSP_LIMITER, set limiter);
SHELL_CMD_EXPORT_ALIAS(cmd_gain_set_func, MSP_GAIN, set gain);
SHELL_CMD_EXPORT_ALIAS(cmd_compander_set_func, MSP_COMPAND, set compander);
SHELL_CMD_EXPORT_ALIAS(cmd_compressor_set_func, MSP_COMPRESSOR, set compressor);
#else
#include <cli.h>
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"MSP_PEQ", "set peq", cmd_peq_set_func},
    {"MSP_LIMITER", "set limiter", cmd_limiter_set_func},
    {"MSP_GAIN", "set gain", cmd_gain_set_func},
    {"MSP_COMPAND", "set compander", cmd_compander_set_func},
    {"MSP_COMPRESSOR", "set compressor", cmd_compressor_set_func},
};
#endif


#define OUTPUT_BUF_SIZE     (21 * 1024)         /*for ringbuffer*/ 
#define OUTPUT_PERIOD       (1024)              /*32ms = 1024/(16000*2/1000), sample rate=16kHz, sample depth=16bit*/

static xcodec_t codec;
static xcodec_output_t codec_output_ch;

static uint8_t g_start_run = 0;
static m_ringbuf_t loop_output_ring_buffer;

static void data_process(const uint8_t *buffer, size_t count)
{
    int16_t *data_u16 = NULL;
    float *data_float = NULL;
    int i = 0;
    size_t num = count / 2;
    uint32_t res = 0, output_num = 0;

    data_u16 = msp_zalloc(num * sizeof(int16_t));
    data_float = msp_zalloc(num * sizeof(float));
    memcpy(data_u16, buffer, count);

    for (i = 0; i < num; i++) {
        data_float[i] = ((float)data_u16[i]) / (0x7fff);
    }

    sona_process(data_float, num);

    for (i = 0; i < num; i++) {
        data_u16[i] = _round16(data_float[i] * (0x7fff));   
    }

    while (output_num < count) {
        res = xcodec_output_write_async(&codec_output_ch, data_u16 + output_num, count - output_num);
        msp_msleep(5);
        output_num += res;
        if(0 == res && (count <= output_num + 32))
            break;
        //printf("res:%d, output_num:%d, src:%x\r\n", res, output_num, *(uint32_t *)0x2000C100);
    }

    msp_free(data_u16);
    msp_free(data_float);

    return;
}

static void codec_output_event_cb_fun(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //printf("out_flag:%d\r\n", out_flag);
    } else if (event == XCODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //printf("output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    } else {

    }
}

#define WRITE_PCM_SIZE 1024
static void _codec_eq_task(void *arg)
{
    xcodec_error_t ret;
    xcodec_output_config_t output_config;
    codec.output_chs = &codec_output_ch;
    
    // uint32_t res = 0, output_num = 0;

    g_start_run = 1;    
    ret = xcodec_init(&codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error\n");
        g_start_run = 0;
        msp_task_exit(0);
    }

    uint8_t *output_buf = NULL;
    uint8_t *p_buf = NULL;

    output_buf = msp_malloc(OUTPUT_BUF_SIZE);

    if (output_buf == NULL){
        printf("malloc failed!\r\n");
        g_start_run = 0;
        msp_task_exit(0);
    }

    xcodec_dma_ch_t *dma_hdl_out = msp_zalloc_check(sizeof(xcodec_dma_ch_t)); 

    codec_output_ch.dma = dma_hdl_out;
    codec_output_ch.sound_channel_num = 1;
    codec_output_ch.ring_buf = &loop_output_ring_buffer;
    xcodec_output_open(&codec, &codec_output_ch, 0);
    /* output ch config */
    xcodec_output_attach_callback(&codec_output_ch, codec_output_event_cb_fun, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = output_buf;
    output_config.buffer_size = OUTPUT_BUF_SIZE;
    output_config.period = OUTPUT_PERIOD;
    output_config.mode = XCODEC_OUTPUT_DIFFERENCE;
    output_config.sound_channel_num = 1;

    xcodec_output_config(&codec_output_ch, &output_config);
    xcodec_output_digital_gain(&codec_output_ch, 50);
    xcodec_output_buffer_reset(&codec_output_ch);

    xcodec_output_link_dma(&codec_output_ch, dma_hdl_out);
    
    xcodec_output_start(&codec_output_ch);

    uint32_t num_iterations = wa_16K_16bit_mono_pcm_len / WRITE_PCM_SIZE;
    for(int i = 0; i < num_iterations; i++){
        const uint8_t *current_chunk = wa_16K_16bit_mono_pcm + (i * WRITE_PCM_SIZE);
        data_process(current_chunk, WRITE_PCM_SIZE);
    }

    uint32_t remaining_size = wa_16K_16bit_mono_pcm_len % WRITE_PCM_SIZE;
    if (remaining_size > 0) {
        const uint8_t *remaining_chunk = wa_16K_16bit_mono_pcm + (num_iterations * WRITE_PCM_SIZE);
        data_process(remaining_chunk, remaining_size);
    }

    // while (output_num < wa_16K_16bit_mono_pcm_len) {
    //     res = xcodec_output_write_async(&codec_output_ch, wa_16K_16bit_mono_pcm + output_num, wa_16K_16bit_mono_pcm_len - output_num);
    //     msp_msleep(5);
    //     output_num += res;
    //     //printf("res:%d, output_num:%d, src:%x\r\n", res, output_num, *(uint32_t *)0x2000C100);
    // }
    printf("stop code output\r\n");
    xcodec_output_stop(&codec_output_ch);
    xcodec_output_link_dma(&codec_output_ch, NULL);
    xcodec_output_detach_callback(&codec_output_ch);
    xcodec_output_close(&codec_output_ch);
    xcodec_uninit(&codec);
    
    msp_free(output_buf);
    msp_free(dma_hdl_out);
    g_start_run = 0;
    msp_task_exit(0);
}

#ifdef CONFIG_SHELL
#include <shell.h>
int cmd_codec_eq(int argc, char **argv)
{
    if (g_start_run) {
        printf("eq task is running\r\n");
        return 0;
    }

    static int flag = 1;
    if (flag) {
        sona_init(TEST_SAMPLERATE);
        printf("rate:%d\r\n", TEST_SAMPLERATE);
        flag = 0;
    }

    msp_task_new("eq_task", _codec_eq_task, NULL, 5120);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_codec_eq, codec_eq, codec eq test);
#endif


#endif
