#ifndef __LMAC154_PDS_H__
#define __LMAC154_PDS_H__

/****************************************************************************//**
 * @brief  Store registers to backup memory
 *
 * @param  retentionMem: retention memory to hold registers configurations
 *
 * @return None
 *
*******************************************************************************/
void lmac154_sleepStoreRegs(uint32_t *retentionMem);

/****************************************************************************//**
 * @brief  Restore registers from backup memory
 *
 * @param  retentionMem: retention memory hold register configurations
 *
 * @return None
 *
*******************************************************************************/
void lmac154_sleepRestoreRegs(uint32_t *retentionMem);

#endif