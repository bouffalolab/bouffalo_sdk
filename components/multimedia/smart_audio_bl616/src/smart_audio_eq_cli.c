/*
 * Copyright (C) 2017-2023 Bouffalolab Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "avutil/av_config.h"
#include "aef/aef_param.h"

#define MAX_EQ_DATA_BYTE 512

#define TAG "EQSET"

// extern int strsplit(char **array, size_t count, char *data, const char *delim);

// static int *g_eq_array  = NULL;
// static int  g_array_pos = -1;
// static void cmd_eqset_func(char *buf, int len, int argc, char **argv)
// {
//     if (argc == 3) {
//         if (strcmp(argv[1], "push") == 0) {
//             if (strcmp(argv[2], "start") == 0) {
//                 if (g_eq_array == NULL) {
//                     g_eq_array = (int *)malloc(MAX_EQ_DATA_BYTE);
//                 }
//                 g_array_pos = 0;
//                 printf("push start ok\r\n");
//                 return;
//             } else if (strcmp(argv[2], "end") == 0) {
//                 if (g_eq_array) {
//                     // for (int i = 0; i < g_array_pos; i++) {
//                     //     printf("0x%08x\r\n", g_eq_array[i]);
//                     // }
//                     board_eq_set_param((void*)g_eq_array, g_array_pos * sizeof(int));
//                     free(g_eq_array);

//                     printf("push end %d\r\n", g_array_pos);
//                     g_eq_array  = NULL;
//                     g_array_pos = -1;
//                     return;
//                 }
//             } else {
//                 //printf("%s\r\n", argv[2]);
//                 if (g_array_pos >= 0) {
//                     char *endptr;
//                     char *str_array[20];
//                     int   count = strsplit(str_array, 20, argv[2], ",");
//                     for (int i = 0; i < count; i++) {
//                         //printf("%d, %s\r\n", i, str_array[i]);
//                         g_eq_array[g_array_pos] = strtol(&str_array[i][2], &endptr, 16);
//                         g_array_pos++;
//                     }
//                     printf("push data %d\r\n", count);
//                     return;
//                 }
//             }
//         } /* push */
//     } /*argc == 3*/
 
//     if (argc == 2) {
//         if (strcmp(argv[1], "show") == 0) {
//             board_eq_show_param();
//             return;
//         }
//     }
//     printf("usage:\r\n");
//     printf("\teq push start\r\n");
//     printf("\teq push \"0x1a96b,0xcad2a,0x1a96b,0x352d6,0xe5695\"\r\n");
//     printf("\teq push end\r\n");
// }

// //extern int board_drc_set_param(void *data, int byte);
// static void cmd_drcset_func(char *buf, int len, int argc, char **argv)
// {
//     int  eq_array[2];
//     int  array_pos = 0;

//     if (argc == 3) {
//         if (strcmp(argv[1], "set") == 0) {
//             // drc buf update
//             printf("%s\r\n", argv[2]);
//             char *endptr;
//             char *str_array[8];
//             int   count = strsplit(str_array, 8, argv[2], ",");
//             for (int i = 0; i < count; i++) {
//                 //printf("%d, %s\r\n", i, str_array[i]);
//                 eq_array[array_pos] = strtoul(&str_array[i][2], &endptr, 16);
//                 array_pos++;
//             }
//             // drc set
//             printf("---> drc set \"0x%08x,0x%08x\"\r\n", eq_array[0], eq_array[1]);
//             board_drc_set_param((void*)eq_array, sizeof(eq_array));
//         }
//     } else {
//         printf("usage:\r\n");
//         printf("\tdrc set \"0xFF428800,0x44245011\"\r\n");
//     }
// }

#if defined(CONFIG_AEF_EQ_ENABLE) && CONFIG_AEF_EQ_ENABLE
#include <aef/aef_param.h>

t_aefCompanderParam g_companderParam = {
                                        .enable = 1,
                                        .compress_trd = -0.005,
                                        .expand_trd = -0.08,
                                        .attack_time = 0.0005,
                                        .release_time = 0.2,
                                        .average_time = 0.05,
                                        .hold_time = 0.05,
                                        .compr_ratio = 5,
                                        .expand_ratio = 5,
                                        .width = 0
                                        };

t_aefCompressorParam g_compressorParam = {
                                        .enable = 1,
                                        .threshold = -10,
                                        .attack_time = 0.005,
                                        .release_time = 0.320,
                                        .average_time = 0.010,
                                        .ratio = 15,
                                        .width = 5
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

    void *eqIns;
    eqIns = get_peq_ins();
    if (eqIns == NULL) {
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
            ret = aef_eq_set_param(eqIns, id, &g_eq_params[id]);
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
                    ret = aef_eq_set_enable(eqIns, 1);
                    if (ret == 0) {
                        printf("enable eq!\r\n");
                    }
                } else if (strcmp(argv[2], "OFF") == 0) {
                    ret = aef_eq_set_enable(eqIns, 0);
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
                    ret = aef_eq_set_param(eqIns, id, &g_eq_params[id]);
                    if (ret == 0) {
                        printf("enable eq!\r\n");
                    }
                } else if (strcmp(argv[3], "OFF") == 0) {
                    g_eq_params[id].enable = 0;
                    ret = aef_eq_set_param(eqIns, id, &g_eq_params[id]);
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

    void *limitIns;
    limitIns = get_limiter_ins();
    if (limitIns == NULL) {
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
            ret = aef_limiter_set_param(limitIns, &g_limitParam);
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
        ret = aef_limiter_set_param(limitIns, &g_limitParam);
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

    void *gainIns;
    gainIns = get_gain_ins();
    if (gainIns == NULL) {
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
            ret = aef_gain_set_param(gainIns, &g_gainParam);
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
        ret = aef_gain_set_param(gainIns, &g_gainParam);
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

    void *companderIns;
    companderIns = get_compander_ins();
    if (companderIns == NULL) {
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
            ret = aef_compander_set_param(companderIns, &g_companderParam);
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
        
        ret = aef_compander_set_param(companderIns, &g_companderParam);
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

    void *compressorIns;
    compressorIns = get_compressor_ins();
    if (compressorIns == NULL) {
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
            ret = aef_compressor_set_param(compressorIns, &g_compressorParam);
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
        
        ret = aef_compressor_set_param(compressorIns, &g_compressorParam);
        if (ret == 0) {
            printf("set compressor success!\r\n");
        } else {
            printf("set compressor failed!\r\n");
        }
    } else {
        printf("param error!\r\n");
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>
// SHELL_CMD_EXPORT_ALIAS(cmd_eqset_func, eq, set eq);
// SHELL_CMD_EXPORT_ALIAS(cmd_drcset_func, drc, set drc);
SHELL_CMD_EXPORT_ALIAS(cmd_peq_set_func, MSP_PEQ, set peq);
SHELL_CMD_EXPORT_ALIAS(cmd_limiter_set_func, MSP_LIMITER, set limiter);
SHELL_CMD_EXPORT_ALIAS(cmd_gain_set_func, MSP_GAIN, set gain);
SHELL_CMD_EXPORT_ALIAS(cmd_compander_set_func, MSP_COMPAND, set compander);
SHELL_CMD_EXPORT_ALIAS(cmd_compressor_set_func, MSP_COMPRESSOR, set compressor);
#else
#include <cli.h>
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    // {"eq", "set eq", cmd_eqset_func},
    // {"drc", "set drc", cmd_drcset_func},
    {"MSP_PEQ", "set peq", cmd_peq_set_func},
    {"MSP_LIMITER", "set limiter", cmd_limiter_set_func},
    {"MSP_GAIN", "set gain", cmd_gain_set_func},
    {"MSP_COMPAND", "set compander", cmd_compander_set_func},
    {"MSP_COMPRESSOR", "set compressor", cmd_compressor_set_func},
};
#endif
#endif

int smartapp_eq_cli_init(void)
{
    return 0;
}
