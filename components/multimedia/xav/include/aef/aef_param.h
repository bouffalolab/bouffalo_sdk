#ifndef _AEF_PARAM_H
#define _AEF_PARAM_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************
 EQ
***********************/ 
typedef enum _BQF_TYPE_ {
    AEF_BQF_AP1 = 0,
    AEF_BQF_LP1,
    AEF_BQF_HP1,
    AEF_BQF_LS1,
    AEF_BQF_HS1,
    AEF_BQF_LP2,
    AEF_BQF_HP2,
    AEF_BQF_LS2,
    AEF_BQF_HS2,
    AEF_BQF_PEAK,
    AEF_BQF_NOTCH,
    AEF_BQF_AP2,
    AEF_BQF_BP2,
} e_aefBQFType;

typedef struct
{
	int enable;
	int type; //e_aefBQFType
	float gain;
	float q;
	float freq;
}t_aefEqFilterParam;

void *aef_eq_init(int fs, int segments);
int aef_eq_set_enable(void *ins, int enable);
int aef_eq_set_param(void *ins, int bqfID, t_aefEqFilterParam *bqf);
int aef_eq_process(void *ins, float *inData, float *outData, int samples);
int aef_eq_free(void *ins);

/***********************
 Limiter
***********************/ 
typedef struct
{
	int enable;
 	float threshold;
	float attack_time; //by second
	float release_time;//by second
}t_aefLimitParam;

void *aef_limiter_init(int fs, t_aefLimitParam *param);
int aef_limiter_set_param(void *ins, t_aefLimitParam *param);
int aef_limiter_process(void *ins, float *inData, float *outData, int samples);
int aef_limiter_free(void *ins);

/***********************
 Compressor
***********************/ 
typedef struct
{
	int enable;
	float threshold; //by dB, <0
	float attack_time; // by second
	float release_time;// by second
	float average_time; // by second
	float ratio; // should >= 1, < 128
	float width;//	softknee width ,dB, >=0, < 20
}t_aefCompressorParam;

void *aef_compressor_init(int fs, t_aefCompressorParam *param);
int aef_compressor_set_param(void *ins, t_aefCompressorParam *param);
int aef_compressor_process(void *ins, float *inData, float *outData, int samples);
int aef_compressor_free(void *ins);

/***********************
 Compressor
***********************/ 
typedef struct
{
    int enable;
    float compress_trd; //commpress threshold
    float expand_trd;// expand threshold
    float attack_time; // by second 0.0005 default
    float release_time;// 0.2 default
    float average_time; //0.05 default
    float hold_time; //0.05 default
    float compr_ratio; // 5 default, 1 ~ INF
    float expand_ratio;// 5 default, 1 ~ INF
    float width;	//	softknee width
}t_aefCompanderParam;

void *aef_compander_init(int fs, t_aefCompanderParam *param);
int aef_compander_set_param(void *ins, t_aefCompanderParam *param);
int aef_compander_process(void *ins, float *inData, float *outData, int samples);
int aef_compander_free(void *ins);

/***********************
 noisegate
***********************/ 
typedef struct
{
	int enable;
 	float threshold;
	float attack_time; //by second, 0.0002 default
	float release_time;// 0.2 default
	float hold_time;// 0.2 default
}t_aefNoiseGtParam;

void *aef_noisegate_init(int fs, t_aefNoiseGtParam *param);
int aef_noisegate_set_param(void *ins, t_aefNoiseGtParam *param);
int aef_noisegate_process(void *ins, float *inData, float *outData, int samples);
int aef_noisegate_free(void *ins);


/***********************
  gain
***********************/ 
typedef struct
{
	int enable;
	int mute; // 1 -mute
	float gain; //dB
	int phase; //// 0 - orignal, 1 - 180 degree
}t_aefGainParam;

void *aef_gain_init(t_aefGainParam *param);
int aef_gain_set_param(void *ins, t_aefGainParam *param);
int aef_gain_process(void *ins, float *inData, float *outData, int samples);
int aef_gain_free(void *ins);

void *get_peq_ins();
void *get_gain_ins();
void *get_limiter_ins();
void *get_noisegate_ins();
void *get_compander_ins();
void *get_compressor_ins();

#ifdef __cplusplus
}
#endif

#endif
