#ifdef __MACSW_CONFIG_INTERNEL_ALLOW__
#define CFG_AMSDU_4K
#define CFG_TXDESC0 4
#define CFG_TXDESC1 16
#define CFG_TXDESC2 1
#define CFG_TXDESC3 4
#define CFG_TXDESC4 4
#endif

#undef CFG_TWT
#undef CFG_BCN

#define CFG_TWT 8
//#define CONFIG_UNSINGLE_MAC_ADDR

#undef CFG_STA_MAX
#define CFG_STA_MAX 2

#undef CFG_BARX
#undef CFG_BATX
#undef CFG_REORD_BUF
#define CFG_BARX 4
#define CFG_BATX 4
#define CFG_REORD_BUF 8

#define WL_BB_TPC 1
#define WL_BB_PD 1

#define CFG_WFA
#define CFG_RM
//#define CONFIG_ANTDIV_STATIC
//#define CONFIG_ANTDIV_DYNAMIC
