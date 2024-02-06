/**
  ******************************************************************************
  * @file    bflb_flash_secreg.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"
#include "bflb_sflash.h"
#include "bflb_xip_sflash.h"
#include "bflb_flash_secreg_port.h"
#include "bflb_flash_secreg.h"

struct flash_params_s {
    uint32_t jedec_id;
    bflb_flash_secreg_param_t param;
};

static const ATTR_TCM_CONST_SECTION struct flash_params_s flash_params[] = {
    {
        /*!< w25q32jv */
        .jedec_id = 0xEF4016,
        .param = {
            .region_offset = 1,
            .region_count = 3,
            .region_size = 0x10,
            .secreg_size = 0x1,
            .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
            .lb_share = 0,
            .lb_offset = 11,
            .lb_write_len = 1,
            .lb_read_len = 1,
        },
    },
    {
        /*!< zb25q64 */
        .jedec_id = 0x5E4017,
        .param = {
            .region_offset = 1,
            .region_count = 3,
            .region_size = 0x10,
            .secreg_size = 0x4,
            .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
            .lb_share = 0,
            .lb_offset = 11,
            .lb_write_len = 1,
            .lb_read_len = 1,
        },
    },
    {
        /*!< gt25q32a */
        .jedec_id = 0xC46016,
        .param = {
            .region_offset = 0,
            .region_count = 3,
            .region_size = 0x4,
            .secreg_size = 0x4,
            .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
            .lb_share = 0,
            .lb_offset = 11,
            .lb_write_len = 1,
            .lb_read_len = 1,
        },
    },
    {
        /*!< gt25q16a */
        .jedec_id = 0xC46015,
        .param = {
            .region_offset = 0,
            .region_count = 1,
            .region_size = 0x10,
            .secreg_size = 0x10,
            .api_type = BFLB_FLASH_SECREG_API_TYPE_GENERAL,
            .lb_share = 1,
            .lb_offset = 10,
            .lb_write_len = 1,
            .lb_read_len = 1,
        },
    }
};

#define SECREG_OPLOCK_LOCKED 0x8A4F2D06
#define SECREG_OPLOCK_UNLOCK 0xB71E9C3A

static spi_flash_cfg_type *pg_flash_cfg;

static __ALIGNED(4) bflb_flash_secreg_param_t g_param = { .raw = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
static volatile uint32_t g_oplock = SECREG_OPLOCK_LOCKED;

struct callapi_content {
    uint32_t offset;
    uintptr_t flag;
    uint8_t aes_enable;
    uint8_t before_ret0;
};

static bool ATTR_TCM_SECTION bflb_flash_secreg_callapi_before(const spi_flash_cfg_type *flash_cfg, struct callapi_content *content)
{
    int ret;
    const uint8_t group = 0;
    const uint8_t bank = 0;
    uint8_t io_mode = flash_cfg->io_mode & 0xf;

    content->flag = bflb_irq_save();
    bflb_xip_sflash_opt_enter(&content->aes_enable);
    ret = bflb_xip_sflash_state_save((void *)flash_cfg, &content->offset, group, bank);
    content->before_ret0 = (ret == 0);

    if (ret != 0) {
        bflb_sflash_set_xip_cfg((void *)flash_cfg, io_mode, 1, 0, 32, bank);
    }

    return content->before_ret0;
}

static void ATTR_TCM_SECTION bflb_flash_secreg_callapi_after(const spi_flash_cfg_type *flash_cfg, struct callapi_content *content)
{
    const uint8_t group = 0;
    const uint8_t bank = 0;

    if (content->before_ret0) {
        bflb_xip_sflash_state_restore((void *)flash_cfg, content->offset, group, bank);
    }

    bflb_xip_sflash_opt_exit(content->aes_enable);
    bflb_irq_restore(content->flag);
}

/*****************************************************************************
* @brief        lock flash secreg operation (nested not supproted)
* 
* 
* @retval int               0:Success, -1:Already lock
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_oplock(void)
{
    if (g_oplock == SECREG_OPLOCK_LOCKED) {
        return -1;
    }

    g_oplock = SECREG_OPLOCK_LOCKED;
    return 0;
}

/*****************************************************************************
* @brief        unlock flash secreg operation (nested not supproted)
* 
* 
* @retval int               0:Success, -1:Already unlock
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_opunlock(void)
{
    if (g_oplock == SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    g_oplock = SECREG_OPLOCK_UNLOCK;
    return 0;
}

/*****************************************************************************
* @brief        get flash secreg param
* 
* @param[out]   param       pointer to save param pointer or NULL
* 
* @retval int               0:Success, -1:Not found secreg param
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_param(const bflb_flash_secreg_param_t **param)
{
    uint32_t array_length;
    uint32_t i;
    uint32_t jid;
    uint32_t dummy;

    jid = bflb_flash_get_jedec_id();

    /*!< only search once */
    if (*((uint64_t *)&g_param) == 0xffffffffffffffff) {
        array_length = sizeof(flash_params) / sizeof(struct flash_params_s);

        for (i = 0; i < array_length; i++) {
            if (flash_params[i].jedec_id == jid) {
                break;
            }
        }

        if (i >= array_length) {
            return -1;
        }

        arch_memcpy(&g_param, &flash_params[i].param, sizeof(g_param));
        bflb_flash_get_cfg((void *)&pg_flash_cfg, &dummy);
    }

    if (param) {
        *param = &g_param;
    }

    return 0;
}

/*****************************************************************************
* @brief        for each security register region do callback
* 
* @param[in]    cb          user callback
* @param[in]    data        user callback param
* 
* @retval int               0:Success, -1:Not found secreg param
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_region_foreach(region_cb cb, void *data)
{
    int ret;
    const bflb_flash_secreg_param_t *param;
    bflb_flash_secreg_region_info_t info;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    for (uint32_t i = 0; i < param->region_count; i++) {
        if ((ret = bflb_flash_secreg_get_info_by_idx(i, &info)) != 0) {
            return ret;
        }

        if (!cb(&info, data)) {
            break;
        }
    }

    return 0;
}

/*****************************************************************************
* @brief        get security register region lock bits
* 
* @param[out]   lockbits    pointer to save bitmap, some flash only one bit
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_lockbits(uint8_t *lockbits)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< get lock bit */
        ret = flash_secreg_apis[param->api_type].get_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count, param->lb_offset, &lb);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    *lockbits = lb;
    return 0;
}

/*****************************************************************************
* @brief        set security register region lock bits (need opunlock)
* 
* @param[in]    lockbits    pointer to save bitmap, some flash only one bit
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_set_lockbits(uint8_t lockbits)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb_mask;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (param->lb_share) {
        if (lockbits > 1) {
            /*!< invalid lockbits */
            return -1;
        }
    } else {
        lb_mask = ~(0xff << param->region_count);
        if (lb_mask & lockbits) {
            /*!< invalid lockbits */
            return -1;
        }
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< set lock bit */
        ret = flash_secreg_apis[param->api_type].set_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count,
            param->lb_offset, lockbits, param->lb_write_len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        check security register region is OTP locked
* 
* @param[in]    index       security register region index
* 
* @retval int               0:Not locked, 1:OTP locked, negative value:ERROR
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_locked(uint8_t index)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< get lock bit */
        ret = flash_secreg_apis[param->api_type].get_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count, param->lb_offset, &lb);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return param->lb_share ? (lb > 0) : ((lb & (1 << index)) > 0);
}

/*****************************************************************************
* @brief        lock security register region (need opunlock)
* 
* @param[in]    index       security register region index
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_set_locked(uint8_t index)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if (param->lb_share) {
        lb = 0x1;
    } else {
        lb = 1 << index;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< set lock bit */
        ret = flash_secreg_apis[param->api_type].set_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count,
            param->lb_offset, lb, param->lb_write_len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        get security register region info
* 
* @param[in]    index       security register region index
* @param[in]    info        security register region info
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_info_by_idx(uint8_t index, bflb_flash_secreg_region_info_t *info)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t lb;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< get lock bit */
        ret = flash_secreg_apis[param->api_type].get_lock(
            pg_flash_cfg, param->lb_share ? 1 : param->region_count, param->lb_offset, &lb);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    info->address = index * param->secreg_size * 256;
    info->physical_address = (index + param->region_offset) * param->region_size * 256;
    info->region_size = param->region_size * 256;
    info->secreg_size = param->secreg_size * 256;
    info->index = index;
    info->locked = param->lb_share ? (lb > 0) : ((lb & (1 << index)) > 0);
    info->lockbit_share = param->lb_share;

    return 0;
}

/*****************************************************************************
* @brief        valid security register region index and read/write len
* 
* @param[in]    index       security register region index
* @param[in]    offset      read/write offset
* @param[in]    len         read/write len
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_valid_by_idx(uint8_t index, uint32_t offset, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if ((offset + len) > param->secreg_size * 256) {
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        read security register region
* 
* @param[in]    index       security register region index
* @param[in]    offset      read offset
* @param[in]    data        read data pointer
* @param[in]    len         read len
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_read_by_idx(uint8_t index, uint32_t offset, void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint32_t address;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if ((offset + len) > param->secreg_size * 256) {
        return -1;
    }

    address = param->region_size * 256 * (index + param->region_offset) + offset;

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< read */
        ret = flash_secreg_apis[param->api_type].read(pg_flash_cfg, address, data, len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        write security register region (need opunlock)
* 
* @param[in]    index       security register region index
* @param[in]    offset      write offset
* @param[in]    data        write data pointer
* @param[in]    len         write len
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_write_by_idx(uint8_t index, uint32_t offset, const void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    if ((offset + len) > param->secreg_size * 256) {
        return -1;
    }

    address = param->region_size * 256 * (index + param->region_offset) + offset;

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< write */
        ret = flash_secreg_apis[param->api_type].write(pg_flash_cfg, address, data, len);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        erase security register region (need opunlock)
* 
* @param[in]    index       security register region index
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_erase_by_idx(uint8_t index)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    if (index >= param->region_count) {
        return -1;
    }

    address = param->region_size * 256 * (index + param->region_offset);

    if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
        /*!< erase */
        ret = flash_secreg_apis[param->api_type].erase(pg_flash_cfg, address);
    }

    bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************
* @brief        get security register region info
* 
* @param[in]    addr        security register region address
* @param[in]    info        security register region info
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_get_info(uint32_t addr, bflb_flash_secreg_region_info_t *info)
{
    const bflb_flash_secreg_param_t *param;
    uint8_t index;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    index = addr / param->secreg_size;

    return bflb_flash_secreg_get_info_by_idx(index, info);
}

/*****************************************************************************
* @brief        valid security register region address and read/write len
* 
* @param[in]    addr        security register region address
* @param[in]    len         read/write len
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_valid(uint32_t addr, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    uint32_t valid_len;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        read security register region
* 
* @param[in]    addr        security register region address
* @param[in]    data        read data pointer
* @param[in]    len         read len
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_read(uint32_t addr, void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t index;
    uint32_t offset;
    uint32_t curlen;
    uint32_t valid_len;
    uint32_t address;
    int ret = -1;

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    while (len) {
        index = addr / (param->secreg_size * 256);
        offset = addr % (param->secreg_size * 256);
        address = (index + param->region_offset) * (param->region_size * 256) + offset;
        curlen = param->secreg_size * 256 - offset;

        if (curlen > len) {
            curlen = len;
        }

        if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
            /*!< read */
            ret = flash_secreg_apis[param->api_type].read(pg_flash_cfg, address, data, len);
        }

        bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

        if (ret) {
            return ret;
        }

        len -= curlen;
        data += curlen;
        addr += curlen;
    }

    return 0;
}

/*****************************************************************************
* @brief        write security register region (need opunlock)
* 
* @param[in]    addr        security register region address
* @param[in]    data        write data pointer
* @param[in]    len         write len
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_write(uint32_t addr, const void *data, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t index;
    uint32_t offset;
    uint32_t curlen;
    uint32_t valid_len;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    while (len) {
        index = addr / (param->secreg_size * 256);
        offset = addr % (param->secreg_size * 256);
        address = (index + param->region_offset) * (param->region_size * 256) + offset;
        curlen = param->secreg_size * 256 - offset;

        if (curlen > len) {
            curlen = len;
        }

        if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
            /*!< write */
            ret = flash_secreg_apis[param->api_type].write(pg_flash_cfg, address, data, len);
        }

        bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

        if (ret) {
            return ret;
        }

        len -= curlen;
        data += curlen;
        addr += curlen;
    }

    return 0;
}

/*****************************************************************************
* @brief        erase security register region (need opunlock)
* 
* @param[in]    addr        security register region address
* @param[in]    len         erase len
* 
* @retval int               0:Success
*****************************************************************************/
int ATTR_TCM_SECTION bflb_flash_secreg_erase(uint32_t addr, uint32_t len)
{
    const bflb_flash_secreg_param_t *param;
    struct callapi_content content;
    uint8_t index;
    uint32_t offset;
    uint32_t curlen;
    uint32_t valid_len;
    uint32_t address;
    int ret = -1;

    if (g_oplock != SECREG_OPLOCK_UNLOCK) {
        return -1;
    }

    if (bflb_flash_secreg_get_param(&param)) {
        return -1;
    }

    valid_len = param->region_count * param->secreg_size * 256;

    if ((addr >= valid_len) || ((addr + len) > valid_len)) {
        return -1;
    }

    while (len) {
        index = addr / (param->secreg_size * 256);
        offset = addr % (param->secreg_size * 256);
        address = (index + param->region_offset) * (param->region_size * 256) + offset;
        curlen = param->secreg_size * 256 - offset;

        if (curlen > len) {
            curlen = len;
        }

        if (bflb_flash_secreg_callapi_before(pg_flash_cfg, &content)) {
            /*!< erase */
            ret = flash_secreg_apis[param->api_type].erase(pg_flash_cfg, address);
        }

        bflb_flash_secreg_callapi_after(pg_flash_cfg, &content);

        if (ret) {
            return ret;
        }

        len -= curlen;
        addr += curlen;
    }

    return 0;
}
