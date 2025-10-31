#undef CFG_TXDESC0
#undef CFG_TXDESC1
#undef CFG_TXDESC2
#undef CFG_TXDESC3
#undef CFG_TXDESC4
#undef CFG_TWT

#define CFG_TXDESC0 4
#define CFG_TXDESC1 16
#define CFG_TXDESC2 1
#define CFG_TXDESC3 4
#define CFG_TXDESC4 4
#define CFG_TWT 8
//#define CONFIG_UNSINGLE_MAC_ADDR

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
