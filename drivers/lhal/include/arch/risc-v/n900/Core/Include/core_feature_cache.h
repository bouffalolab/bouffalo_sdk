/*
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __CORE_FEATURE_CACHE_H__
#define __CORE_FEATURE_CACHE_H__
/*!
 * @file     core_feature_cache.h
 * @brief    Cache feature API header file for Nuclei N/NX Core
 */
/*
 * Cache Feature Configuration Macro:
 * 1. __ICACHE_PRESENT:  Define whether I-Cache Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 2. __DCACHE_PRESENT:  Define whether D-Cache Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 3. __CCM_PRESENT:  Define whether Nuclei Cache Control and Maintainence(CCM) Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"
#include <stddef.h>

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)

/* ##########################  Cache functions  #################################### */
/**
 * \defgroup NMSIS_Core_Cache       Cache Functions
 * \brief    Functions that configure Instruction and Data Cache.
 * @{
 *
 * Nuclei provide Cache Control and Maintainence(CCM) for software to control and maintain
 * the internal L1 I/D Cache of the RISC-V Core, software can manage the cache flexibly to
 * meet the actual application scenarios.
 *
 * The CCM operations have 3 types: by single address, by all and flush pipeline.
 * The CCM operations are done via CSR registers, M/S/U mode has its own CSR registers to
 * do CCM operations. By default, CCM operations are not allowed in S/U mode, you can execute
 * \ref EnableSUCCM in M-Mode to enable it.
 *
 * * API names started with M<operations>, such as \ref MInvalICacheLine must be called in M-Mode only.
 * * API names started with S<operations>, such as \ref SInvalICacheLine should be called in S-Mode.
 * * API names started with U<operations>, such as \ref UInvalICacheLine should be called in U-Mode.
 *
 */

#define CACHE_LINE_SIZE 64

/**
 * \brief Cache CCM Operation Fail Info
 */
typedef enum CCM_OP_FINFO {
    CCM_OP_SUCCESS = 0x0,        /*!< Lock Succeed */
    CCM_OP_EXCEED_ERR = 0x1,     /*!< Exceed the the number of lockable ways(N-Way I/D-Cache, lockable is N-1) */
    CCM_OP_PERM_CHECK_ERR = 0x2, /*!< PMP/sPMP/Page-Table X(I-Cache)/R(D-Cache) permission check failed, or belong to Device/Non-Cacheable address range */
    CCM_OP_REFILL_BUS_ERR = 0x3, /*!< Refill has Bus Error */
    CCM_OP_ECC_ERR = 0x4         /*!< Deprecated, ECC Error, this error code is removed in later Nuclei CCM RTL design, please don't use it */
} CCM_OP_FINFO_Type;

/**
 * \brief Cache CCM Command Types
 */
typedef enum CCM_CMD {
    CCM_DC_INVAL = 0x0,       /*!< Unlock and invalidate D-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_WB = 0x1,          /*!< Flush the specific D-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_WBINVAL = 0x2,     /*!< Unlock, flush and invalidate the specific D-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_LOCK = 0x3,        /*!< Lock the specific D-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_UNLOCK = 0x4,      /*!< Unlock the specific D-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_WBINVAL_ALL = 0x6, /*!< Unlock and flush and invalidate all the valid and dirty D-Cache lines */
    CCM_DC_WB_ALL = 0x7,      /*!< Flush all the valid and dirty D-Cache lines */
    CCM_DC_INVAL_ALL = 0x17,  /*!< Unlock and invalidate all the D-Cache lines */
    CCM_IC_INVAL = 0x8,       /*!< Unlock and invalidate I-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_IC_LOCK = 0xb,        /*!< Lock the specific I-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_IC_UNLOCK = 0xc,      /*!< Unlock the specific I-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_IC_INVAL_ALL = 0xd    /*!< Unlock and invalidate all the I-Cache lines */
} CCM_CMD_Type;

/**
 * \brief Cache Information Type
 */
typedef struct CacheInfo {
    uint32_t linesize;  /*!< Cache Line size in bytes */
    uint32_t ways;      /*!< Cache ways */
    uint32_t setperway; /*!< Cache set per way */
    uint32_t size;      /*!< Cache total size in bytes */
} CacheInfo_Type;

#if __riscv_xlen == 32
#define CCM_SUEN_SUEN_Msk (0xFFFFFFFFUL) /*!< CSR CCM_SUEN: SUEN Mask */
#else
#define CCM_SUEN_SUEN_Msk (0xFFFFFFFFFFFFFFFFUL) /*!< CSR CCM_SUEN: SUEN Mask */
#endif

/**
 * \brief  Enable CCM operation in Supervisor/User Mode
 * \details
 * This function enable CCM operation in Supervisor/User Mode.
 * If enabled, CCM operations in supervisor/user mode will
 * be allowed.
 * \remarks
 * - This function can be called in M-Mode only.
 * \sa
 * - \ref DisableSUCCM
*/
__STATIC_FORCEINLINE void EnableSUCCM(void)
{
    __RV_CSR_SET(CSR_CCM_SUEN, CCM_SUEN_SUEN_Msk);
}

/**
 * \brief  Disable CCM operation in Supervisor/User Mode
 * \details
 * This function disable CCM operation in Supervisor/User Mode.
 * If not enabled, CCM operations in supervisor/user mode will
 * trigger a *illegal intruction* exception.
 * \remarks
 * - This function can be called in M-Mode only.
 * \sa
 * - \ref EnableSUCCM
*/
__STATIC_FORCEINLINE void DisableSUCCM(void)
{
    __RV_CSR_CLEAR(CSR_CCM_SUEN, CCM_SUEN_SUEN_Msk);
}

/**
 * \brief  Flush pipeline after CCM operation
 * \details
 * This function is used to flush pipeline after CCM operations
 * on Cache, it will ensure latest instructions or data can be
 * seen by pipeline.
 * \remarks
 * - This function can be called in M/S/U-Mode only.
*/
__STATIC_FORCEINLINE void FlushPipeCCM(void)
{
    __RV_CSR_WRITE(CSR_CCM_FPIPE, 0x1);
}
/** @} */ /* End of Doxygen Group NMSIS_Core_Cache */
#endif

#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)

/**
 * \defgroup NMSIS_Core_ICache      I-Cache Functions
 * \ingroup  NMSIS_Core_Cache
 * \brief    Functions that configure Instruction Cache.
 * @{
 */

/**
 * \brief  Check ICache Unit Present or Not
 * \details
 * This function check icache unit present or not via mcfg_info csr
 * \remarks
 * - This function might not work for some old nuclei processors
 * - Please make sure the version of your nuclei processor contain ICACHE bit in mcfg_info
 * \return 1 if present otherwise 0
*/
__STATIC_INLINE int32_t ICachePresent(void)
{
    if (__RV_CSR_READ(CSR_MCFG_INFO) & MCFG_INFO_ICACHE) {
        return 1;
    }
    return 0;
}

/**
 * \brief  Enable ICache
 * \details
 * This function enable I-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache enable.
 * \sa
 * - \ref DisableICache
*/
__STATIC_FORCEINLINE void EnableICache(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_EN);
}

/**
 * \brief  Disable ICache
 * \details
 * This function Disable I-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache enable.
 * \sa
 * - \ref EnableICache
 */
__STATIC_FORCEINLINE void DisableICache(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_EN);
}

/**
 * \brief  Enable ICache ECC
 * \details
 * This function enable I-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache ECC enable.
 * \sa
 * - \ref DisableICacheECC
*/
__STATIC_FORCEINLINE void EnableICacheECC(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EN);
}

/**
 * \brief  Disable ICache ECC
 * \details
 * This function disable I-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache ECC enable.
 * \sa
 * - \ref EnableICacheECC
*/
__STATIC_FORCEINLINE void DisableICacheECC(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EN);
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
/**
 * \brief  Get I-Cache Information
 * \details
 * This function get I-Cache Information
 * \remarks
 * - This function can be called in M-Mode only.
 * - You can use this function in combination with cache lines operations
 * \sa
 * - \ref GetDCacheInfo
 */
__STATIC_INLINE int32_t GetICacheInfo(CacheInfo_Type *info)
{
    if (info == NULL) {
        return -1;
    }
    CSR_MICFGINFO_Type csr_ccfg;
    csr_ccfg.d = __RV_CSR_READ(CSR_MICFG_INFO);
    info->setperway = (1UL << csr_ccfg.b.set) << 3;
    info->ways = (1 + csr_ccfg.b.way);
    if (csr_ccfg.b.lsize == 0) {
        info->linesize = 0;
    } else {
        info->linesize = (1UL << (csr_ccfg.b.lsize - 1)) << 3;
    }
    info->size = info->setperway * info->ways * info->linesize;
    return 0;
}

/**
 * \brief  Invalidate one I-Cache line specified by address in M-Mode
 * \details
 * This function unlock and invalidate one I-Cache line specified
 * by the address.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache lines specified by address in M-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void MInvalICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate I-Cache range by address and size in M-Mode
 * \details
 * This function unlock and invalidate a range of I-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be invalidated.
 * \remarks
 * This function Must be executed in M-Mode only.
 * \param [in]    addr    Start address of the memory range to be invalidated
 * \param [in]    size    Size in bytes of the memory range to be invalidated
 */
__STATIC_INLINE void MInvalICacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        MInvalICacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Invalidate one I-Cache line specified by address in S-Mode
 * \details
 * This function unlock and invalidate one I-Cache line specified
 * by the address.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache lines specified by address in S-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void SInvalICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate I-Cache range by address and size in S-Mode
 * \details
 * This function unlock and invalidate a range of I-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be invalidated.
 * \remarks
 * This function Must be executed in M/S-Mode only.
 * \param [in]    addr    Start address of the memory range to be invalidated
 * \param [in]    size    Size in bytes of the memory range to be invalidated
 */
__STATIC_INLINE void SInvalICacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        SInvalICacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Invalidate one I-Cache line specified by address in U-Mode
 * \details
 * This function unlock and invalidate one I-Cache line specified
 * by the address.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache lines specified by address in U-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void UInvalICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate I-Cache range by address and size in U-Mode
 * \details
 * This function unlock and invalidate a range of I-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be invalidated.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    Start address of the memory range to be invalidated
 * \param [in]    size    Size in bytes of the memory range to be invalidated
 */
__STATIC_INLINE void UInvalICacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        UInvalICacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Lock one I-Cache line specified by address in M-Mode
 * \details
 * This function lock one I-Cache line specified by the address.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_MDATA);
}

/**
 * \brief  Lock several I-Cache lines specified by address in M-Mode
 * \details
 * This function lock several I-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_MDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one I-Cache line specified by address in S-Mode
 * \details
 * This function lock one I-Cache line specified by the address.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_SDATA);
}

/**
 * \brief  Lock several I-Cache lines specified by address in S-Mode
 * \details
 * This function lock several I-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_SDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one I-Cache line specified by address in U-Mode
 * \details
 * This function lock one I-Cache line specified by the address.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_UDATA);
}

/**
 * \brief  Lock several I-Cache lines specified by address in U-Mode
 * \details
 * This function lock several I-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_UDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Unlock one I-Cache line specified by address in M-Mode
 * \details
 * This function unlock one I-Cache line specified by the address.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void MUnlockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several I-Cache lines specified by address in M-Mode
 * \details
 * This function unlock several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void MUnlockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one I-Cache line specified by address in S-Mode
 * \details
 * This function unlock one I-Cache line specified by the address.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void SUnlockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several I-Cache lines specified by address in S-Mode
 * \details
 * This function unlock several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void SUnlockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one I-Cache line specified by address in U-Mode
 * \details
 * This function unlock one I-Cache line specified by the address.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void UUnlockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several I-Cache lines specified by address in U-Mode
 * \details
 * This function unlock several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void UUnlockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate all I-Cache lines in M-Mode
 * \details
 * This function invalidate all I-Cache lines.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalICache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all I-Cache lines in S-Mode
 * \details
 * This function invalidate all I-Cache lines.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalICache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all I-Cache lines in U-Mode
 * \details
 * This function invalidate all I-Cache lines.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalICache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}
#endif    /* defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */
/** @} */ /* End of Doxygen Group NMSIS_Core_ICache */
#endif    /* defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1) */

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1)
/**
 * \defgroup NMSIS_Core_DCache      D-Cache Functions
 * \ingroup  NMSIS_Core_Cache
 * \brief    Functions that configure Data Cache.
 * @{
 */

/**
 * \brief  Check DCache Unit Present or Not
 * \details
 * This function check dcache unit present or not via mcfg_info csr
 * \remarks
 * - This function might not work for some old nuclei processors
 * - Please make sure the version of your nuclei processor contain DCACHE bit in mcfg_info
 * \return 1 if present otherwise 0
*/
__STATIC_INLINE int32_t DCachePresent(void)
{
    if (__RV_CSR_READ(CSR_MCFG_INFO) & MCFG_INFO_DCACHE) {
        return 1;
    }
    return 0;
}

/**
 * \brief  Enable DCache
 * \details
 * This function enable D-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache enable.
 * \sa
 * - \ref DisableDCache
*/
__STATIC_FORCEINLINE void EnableDCache(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_EN);
}

/**
 * \brief  Disable DCache
 * \details
 * This function Disable D-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache enable.
 * \sa
 * - \ref EnableDCache
 */
__STATIC_FORCEINLINE void DisableDCache(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_EN);
}

/**
 * \brief  Enable DCache ECC
 * \details
 * This function enable D-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache ECC enable.
 * \sa
 * - \ref DisableDCacheECC
*/
__STATIC_FORCEINLINE void EnableDCacheECC(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EN);
}

/**
 * \brief  Disable DCache ECC
 * \details
 * This function disable D-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache ECC enable.
 * \sa
 * - \ref EnableDCacheECC
*/
__STATIC_FORCEINLINE void DisableDCacheECC(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EN);
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
/**
 * \brief  Get D-Cache Information
 * \details
 * This function get D-Cache Information
 * \remarks
 * - This function can be called in M-Mode only.
 * - You can use this function in combination with cache lines operations
 * \sa
 * - \ref GetICacheInfo
 */
__STATIC_INLINE int32_t GetDCacheInfo(CacheInfo_Type *info)
{
    if (info == NULL) {
        return -1;
    }
    CSR_MDCFGINFO_Type csr_ccfg;
    csr_ccfg.d = __RV_CSR_READ(CSR_MDCFG_INFO);
    info->setperway = (1UL << csr_ccfg.b.set) << 3;
    info->ways = (1 + csr_ccfg.b.way);
    if (csr_ccfg.b.lsize == 0) {
        info->linesize = 0;
    } else {
        info->linesize = (1UL << (csr_ccfg.b.lsize - 1)) << 3;
    }
    info->size = info->setperway * info->ways * info->linesize;
    return 0;
}

/**
 * \brief  Invalidate one D-Cache line specified by address in M-Mode
 * \details
 * This function unlock and invalidate one D-Cache line specified
 * by the address.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several D-Cache lines specified by address in M-Mode
 * \details
 * This function unlock and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void MInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate D-Cache range by address and size in M-Mode
 * \details
 * This function unlock and invalidate a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be invalidated.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    Start address of the memory range to be invalidated
 * \param [in]    size    Size in bytes of the memory range to be invalidated
 */
__STATIC_INLINE void MInvalDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        MInvalDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Invalidate one D-Cache line specified by address in S-Mode
 * \details
 * This function unlock and invalidate one D-Cache line specified
 * by the address.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several D-Cache lines specified by address in S-Mode
 * \details
 * This function unlock and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void SInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate D-Cache range by address and size in S-Mode
 * \details
 * This function unlock and invalidate a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be invalidated.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    Start address of the memory range to be invalidated
 * \param [in]    size    Size in bytes of the memory range to be invalidated
 */
__STATIC_INLINE void SInvalDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        SInvalDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Invalidate one D-Cache line specified by address in U-Mode
 * \details
 * This function unlock and invalidate one D-Cache line specified
 * by the address.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several D-Cache lines specified by address in U-Mode
 * \details
 * This function unlock and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void UInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate D-Cache range by address and size in U-Mode
 * \details
 * This function unlock and invalidate a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be invalidated.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    Start address of the memory range to be invalidated
 * \param [in]    size    Size in bytes of the memory range to be invalidated
 */
__STATIC_INLINE void UInvalDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        UInvalDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Flush one D-Cache line specified by address in M-Mode
 * \details
 * This function flush one D-Cache line specified by the address.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void MFlushDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WB);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush several D-Cache lines specified by address in M-Mode
 * \details
 * This function flush several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
__STATIC_INLINE void MFlushDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WB);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush D-Cache range by address and size in M-Mode
 * \details
 * This function flush a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be flushed.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    Start address of the memory range to be flushed
 * \param [in]    size    Size in bytes of the memory range to be flushed
 */
__STATIC_INLINE void MFlushDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        MFlushDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Flush one D-Cache line specified by address in S-Mode
 * \details
 * This function flush one D-Cache line specified by the address.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void SFlushDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WB);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush several D-Cache lines specified by address in S-Mode
 * \details
 * This function flush several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
__STATIC_INLINE void SFlushDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WB);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush D-Cache range by address and size in S-Mode
 * \details
 * This function flush a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be flushed.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    Start address of the memory range to be flushed
 * \param [in]    size    Size in bytes of the memory range to be flushed
 */
__STATIC_INLINE void SFlushDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        SFlushDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Flush one D-Cache line specified by address in U-Mode
 * \details
 * This function flush one D-Cache line specified by the address.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void UFlushDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WB);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush several D-Cache lines specified by address in U-Mode
 * \details
 * This function flush several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
__STATIC_INLINE void UFlushDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WB);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush D-Cache range by address and size in U-Mode
 * \details
 * This function flush a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be flushed.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    Start address of the memory range to be flushed
 * \param [in]    size    Size in bytes of the memory range to be flushed
 */
__STATIC_INLINE void UFlushDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        UFlushDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Flush and invalidate one D-Cache line specified by address in M-Mode
 * \details
 * This function flush and invalidate one D-Cache line specified by the address.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
__STATIC_INLINE void MFlushInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WBINVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate several D-Cache lines specified by address in M-Mode
 * \details
 * This function flush and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
__STATIC_INLINE void MFlushInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WBINVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush and invalidate D-Cache range by address and size in M-Mode
 * \details
 * This function flush and invalidate a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be flushed and invalidated.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    Start address of the memory range to be flushed and invalidated
 * \param [in]    size    Size in bytes of the memory range to be flushed and invalidated
 */
__STATIC_INLINE void MFlushInvalDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        MFlushInvalDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Flush and invalidate one D-Cache line specified by address in S-Mode
 * \details
 * This function flush and invalidate one D-Cache line specified by the address.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
__STATIC_INLINE void SFlushInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WBINVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate several D-Cache lines specified by address in S-Mode
 * \details
 * This function flush and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
__STATIC_INLINE void SFlushInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WBINVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush and invalidate D-Cache range by address and size in S-Mode
 * \details
 * This function flush and invalidate a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be flushed and invalidated.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    Start address of the memory range to be flushed and invalidated
 * \param [in]    size    Size in bytes of the memory range to be flushed and invalidated
 */
__STATIC_INLINE void SFlushInvalDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        SFlushInvalDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Flush and invalidate one D-Cache line specified by address in U-Mode
 * \details
 * This function flush and invalidate one D-Cache line specified by the address.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
__STATIC_INLINE void UFlushInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WBINVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate several D-Cache lines specified by address in U-Mode
 * \details
 * This function flush and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
__STATIC_INLINE void UFlushInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WBINVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush and invalidate D-Cache range by address and size in U-Mode
 * \details
 * This function flush and invalidate a range of D-Cache lines specified by a start address and byte size.
 *          It calculates the aligned start/end addresses and the number of cache lines to be flushed and invalidated.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    Start address of the memory range to be flushed and invalidated
 * \param [in]    size    Size in bytes of the memory range to be flushed and invalidated
 */
__STATIC_INLINE void UFlushInvalDCacheRange(unsigned long addr, unsigned long size)
{
    /* Read cache line size from CSR */
    unsigned long cache_line_size = CACHE_LINE_SIZE;

    if (size == 0)
        return;

    /* Align start address to cache line boundary (downward) */
    unsigned long start_aligned = addr & ~(cache_line_size - 1);
    /* Align end address to cache line boundary (upward) */
    unsigned long end_aligned = (addr + size + cache_line_size - 1) & ~(cache_line_size - 1);

    /* Calculate total number of cache lines to flush */
    unsigned long lines = (end_aligned - start_aligned) / cache_line_size;

    if (lines > 0) {
        UFlushInvalDCacheLines(start_aligned, lines);
    }
}

/**
 * \brief  Lock one D-Cache line specified by address in M-Mode
 * \details
 * This function lock one D-Cache line specified by the address.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_MDATA);
}

/**
 * \brief  Lock several D-Cache lines specified by address in M-Mode
 * \details
 * This function lock several D-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_MDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one D-Cache line specified by address in S-Mode
 * \details
 * This function lock one D-Cache line specified by the address.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_SDATA);
}

/**
 * \brief  Lock several D-Cache lines specified by address in S-Mode
 * \details
 * This function lock several D-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_SDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one D-Cache line specified by address in U-Mode
 * \details
 * This function lock one D-Cache line specified by the address.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_UDATA);
}

/**
 * \brief  Lock several D-Cache lines specified by address in U-Mode
 * \details
 * This function lock several D-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_UDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Unlock one D-Cache line specified by address in M-Mode
 * \details
 * This function unlock one D-Cache line specified by the address.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void MUnlockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several D-Cache lines specified by address in M-Mode
 * \details
 * This function unlock several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void MUnlockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one D-Cache line specified by address in S-Mode
 * \details
 * This function unlock one D-Cache line specified by the address.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void SUnlockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several D-Cache lines specified by address in S-Mode
 * \details
 * This function unlock several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void SUnlockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one D-Cache line specified by address in U-Mode
 * \details
 * This function unlock one D-Cache line specified by the address.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void UUnlockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several D-Cache lines specified by address in U-Mode
 * \details
 * This function unlock several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void UUnlockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate all D-Cache lines in M-Mode
 * \details
 * This function invalidate all D-Cache lines.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all D-Cache lines in S-Mode
 * \details
 * This function invalidate all D-Cache lines.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all D-Cache lines in U-Mode
 * \details
 * This function invalidate all D-Cache lines.
 * In U-Mode, this operation will be automatically
 * translated to flush and invalidate operations by hardware.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush all D-Cache lines in M-Mode
 * \details
 * This function flush all D-Cache lines.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void MFlushDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush all D-Cache lines in S-Mode
 * \details
 * This function flush all D-Cache lines.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void SFlushDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush all D-Cache lines in U-Mode
 * \details
 * This function flush all D-Cache lines.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void UFlushDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate all D-Cache lines in M-Mode
 * \details
 * This function flush and invalidate all D-Cache lines.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and locked
 */
__STATIC_INLINE void MFlushInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate all D-Cache lines in S-Mode
 * \details
 * This function flush and invalidate all D-Cache lines.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and locked
 */
__STATIC_INLINE void SFlushInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate all D-Cache lines in U-Mode
 * \details
 * This function flush and invalidate all D-Cache lines.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and locked
 */
__STATIC_INLINE void UFlushInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}
#endif /* defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */

/** @} */ /* End of Doxygen Group NMSIS_Core_DCache */
#endif    /* defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_CACHE_H__ */
