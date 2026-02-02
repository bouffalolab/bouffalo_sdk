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

void lmac154_setFramePendingSourceMatch(bool isEnable);

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
 * @param  None 
 *
 * @return lmac154_fptMode_t
 *
*******************************************************************************/
lmac154_fptMode_t lmac154_getFramePendingMode(void);

/****************************************************************************//**
 * @brief  Get frame pending table result when receive a packet
 *
 * @param  tout: timeout to do frame pending table search
 * 
 * @param  isFramePended: whether frame pending is set for searched on received packet
 * 
 * @return int: return the index of neighbor table if matched; else, return -1
 *
*******************************************************************************/
int lmac154_framePendingResult(uint32_t tout, bool * isFramePended);

#if defined BL616D
/****************************************************************************//**
 * @brief  Set frame pending bit result if neighbor search fails
 *
 * @param  bFramePending, frame pending bit, true by default.
 * 
 * @return None
 *
*******************************************************************************/
void lmac154_setFramePendingBitIfNeighborSearchFails(bool bFramePending);

/****************************************************************************//**
 * @brief  Get frame pending bit result if neighbor search fails
 *
 * @param  None
 * 
 * @return True, if neighbor search fails.
 *
*******************************************************************************/
bool lmac154_getFramePendingBitIfNeighborSearchFails(void);
#endif
#endif
