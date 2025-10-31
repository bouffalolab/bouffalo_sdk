#undef CFG_AMSDU_4K
#define CFG_AMSDU_8K
#undef CFG_TXDESC0
#undef CFG_TXDESC1
#undef CFG_TXDESC2
#undef CFG_TXDESC3
#undef CFG_TXDESC4
#define CFG_TXDESC0 12
#define CFG_TXDESC1 64
#define CFG_TXDESC2 12
#define CFG_TXDESC3 12
#define CFG_TXDESC4 2

#undef CFG_BARX
#undef CFG_BATX
#undef CFG_REORD_BUF
#define CFG_BARX 12
#define CFG_BATX 12
#define CFG_REORD_BUF 12

#define CONFIG_FHOST_RX_BUF_SECTION ".psram_noinit"
