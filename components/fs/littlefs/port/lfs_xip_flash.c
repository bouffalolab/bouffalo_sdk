#include "lfs.h"
#include "bflb_flash.h"
#include "bflb_l1c.h"

#ifndef CONFIG_LITTLEFS_FLASH_ADDRESS
#error "must define CONFIG_LITTLEFS_FLASH_ADDRESS"
#endif

/*****************************************************************************
* @brief        Read a region in a block. Negative error codes are propagated
*               to the user.
* @param[in]    c           
* @param[in]    block       
* @param[in]    off         
* @param[out]   buffer      
* @param[in]    size        
* 
* @retval int               
*****************************************************************************/
int lfs_xip_flash_read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size)
{
    return bflb_flash_read(CONFIG_LITTLEFS_FLASH_ADDRESS + block * c->block_size + off,
                           (uint8_t *)buffer, size);
}

/*****************************************************************************
* @brief        Program a region in a block. The block must have previously
*               been erased. Negative error codes are propagated to the user.
*               May return LFS_ERR_CORRUPT if the block should be considered bad.
* @param[in]    c           
* @param[in]    block       
* @param[in]    off         
* @param[in]    buffer      
* @param[in]    size        
* 
* @retval int               
*****************************************************************************/
int lfs_xip_flash_prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size)
{
    return bflb_flash_write(CONFIG_LITTLEFS_FLASH_ADDRESS + block * c->block_size + off,
                            (uint8_t *)buffer, size);
}

/*****************************************************************************
* @brief        Erase a block. A block must be erased before being programmed.
*               The state of an erased block is undefined. Negative error codes
*               are propagated to the user.
*               May return LFS_ERR_CORRUPT if the block should be considered bad.
* @param[in]    c           
* @param[in]    block       
* 
* @retval int               
*****************************************************************************/
int lfs_xip_flash_erase(const struct lfs_config *c, lfs_block_t block)
{
    return bflb_flash_erase(CONFIG_LITTLEFS_FLASH_ADDRESS + block * c->block_size, c->block_size);
}

/*****************************************************************************
* @brief        Sync the state of the underlying block device. Negative error 
*               codes are propagated to the user.
* @param[in]    c           
* 
* @retval int               
*****************************************************************************/
int lfs_xip_flash_sync(const struct lfs_config *c)
{
    /*!< if use xip, may need to clean cache */
    return 0;
}