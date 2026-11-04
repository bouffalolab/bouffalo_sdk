#define CFG_AMSDU_4K
#define CFG_TXDESC0 4
#define CFG_TXDESC1 16
#define CFG_TXDESC2 1
#define CFG_TXDESC3 4
// Each AP VIF submits its Beacon through a static descriptor.
#define CFG_TXDESC4 1

#undef CFG_TWT

// Enable AP mode and Beacon transmission for WFA STA/AP certification.
#undef CFG_BCN
#define CFG_BCN

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
