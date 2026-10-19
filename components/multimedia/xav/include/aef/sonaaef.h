#ifndef _SONA_AEF_H
#define _SONA_AEF_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum _BQF_Type_ {
    BQF_AP1 = 0,
    BQF_LP1,
    BQF_HP1,
    BQF_LS1,
    BQF_HS1,
    BQF_LP2,
    BQF_HP2,
    BQF_LS2,
    BQF_HS2,
    BQF_PEAK,
    BQF_NOTCH,
    BQF_AP2,
    BQF_BP2,
} eBQFType;

typedef struct {
    int enable;
    int type;
    float gain;        ///< -24~18
    float q;           ///< 0.1~50
    float freq;
} tBiQuadFilterParam;

//return eq-Instance
void* sona_eq_create(int fs, int segments, int maxframelen);
int sona_eq_set_enable(void*eqIns, int enable);
int sona_eq_set_param(void* eqIns, int bqfID, tBiQuadFilterParam* bqfparam);
void sona_eq_process(void* eqIns, short* in, short* out, int samples);
void sona_eq_destroy(void* eqIns);


#ifdef  __cplusplus
}
#endif

#endif
