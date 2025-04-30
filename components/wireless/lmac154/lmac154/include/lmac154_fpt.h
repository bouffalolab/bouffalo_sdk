#ifndef __LMAC154_FPT_H__
#define __LMAC154_FPT_H__


typedef enum {
    LMAC154_FPT_DATAREQ             = 0,
    LMAC154_FPT_DATAREQ_DATA        = 1,
    LMAC154_FPT_ANY                 = 2
} lmac154_fptMode_t;

typedef enum {
    LMAC154_FPT_RESULT_NONE         = 0,
    LMAC154_FPT_RESULT_FALSE        = 1,
    LMAC154_FPT_RESULT_TRUE         = 2,
    LMAC154_FPT_RESULT_BUSY         = 3
} lmac154_fptResult_t;

typedef union {
    struct {
        uint32_t isExist:1;
        uint32_t isFramePended:1;
        uint32_t nbrIdx:7;
        uint32_t isSearchDone:1;
        uint32_t unused:22;
    } bf;

    uint32_t word;

} lmac154_fptSearchResult_t;


/****************************************************************************//**
 * @brief  Set frame pending table mode
 *
 * @param  mode: 
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setFramePendingMode(lmac154_fptMode_t mode);

/****************************************************************************//**
 * @brief  Get frame pending table mode
 *
 * @param  mode: frame pending table working mode
 *
 * @return None
 *
*******************************************************************************/
lmac154_fptMode_t lmac154_getFramePendingMode(void);

/****************************************************************************//**
 * @brief  Get frame pending table result when receive a packet
 *
 * @param  tout, timeout to get result with macro second unit
 * 
 * @return Result
 *
*******************************************************************************/
lmac154_fptSearchResult_t lmac154_framePendingResult(uint32_t tout);

#endif