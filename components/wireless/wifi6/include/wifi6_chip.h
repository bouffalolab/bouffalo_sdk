#ifndef WIFI6_CHIP_H_
#define WIFI6_CHIP_H_

#define WIFI6_HE_MCS 1

#if defined(BL618DG)
#define WIFI6_HAS_5G 1
#define WIFI6_DEFAULT_HT40 1
#define WIFI6_MAX_FIXED_CHANNELS 42
#define WIFI6_HAS_CCA_CLI 0
#define WIFI6_MFG_MEDIA_HEADER <bl618dg_mfg_media.h>
#elif defined(BL616CL)
#define WIFI6_HAS_5G 0
#define WIFI6_DEFAULT_HT40 0
#define WIFI6_MAX_FIXED_CHANNELS 14
#define WIFI6_HAS_CCA_CLI 1
#define WIFI6_MFG_MEDIA_HEADER <bl616cl_mfg_media.h>
#elif defined(BL616)
#define WIFI6_HAS_5G 0
#define WIFI6_DEFAULT_HT40 0
#define WIFI6_MAX_FIXED_CHANNELS 14
#define WIFI6_HAS_CCA_CLI 0
#define WIFI6_MFG_MEDIA_HEADER <bl616_mfg_media.h>
#else
#define WIFI6_HAS_5G 0
#define WIFI6_DEFAULT_HT40 0
#define WIFI6_MAX_FIXED_CHANNELS 14
#define WIFI6_HAS_CCA_CLI 0
#endif

#endif
