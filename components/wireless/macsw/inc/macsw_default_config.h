#define CFG_AMSDU_4K

#if defined(BL618DG)
#define CFG_RXL_BUFFER1_AMSDU_CNT 4
#define CFG_REORD_BUF 16
#else
#define CFG_RXL_BUFFER1_AMSDU_CNT 1
#define CFG_REORD_BUF 12
#endif

#define CFG_TXDESC0 1
#define CFG_TXDESC1 32
#define CFG_TXDESC2 1
#define CFG_TXDESC3 1
#define CFG_TXDESC4 4

#define CFG_TWT 8
#define CFG_BARX 2
#define CFG_BATX 1
