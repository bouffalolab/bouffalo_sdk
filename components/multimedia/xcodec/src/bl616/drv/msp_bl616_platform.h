#ifndef  __MSP_BL616_PLATFORM_H__
#define  __MSP_BL616_PLATFORM_H__

#include <stdio.h>
#include <stdint.h>

#define GLB_BASE               ((uint32_t)0x20000000)
#define DMA_BASE               ((uint32_t)0x2000c000)
#define HBN_BASE               ((uint32_t)0x2000f000)
#define AUPWM_BASE             ((uint32_t)0x20055000)
#define AUADC_BASE             ((uint32_t)0x2000a000)
#define IRQ_NUM_BASE           (16)
#define AUDIO_IRQn             (IRQ_NUM_BASE + 4)    // Audio Interrupt
#define DMA0_ALL_IRQn          (IRQ_NUM_BASE + 15)   //

#define SOC_CACHE_ALIGN        (32)
#define SOC_AUDIO_TX_BUF       (AUPWM_BASE + 0x94)   // DMA_ADDR_AUDIO_TDR
#define SOC_AUDIO_RX_BUF       (AUADC_BASE + 0xC88)  // DMA_ADDR_SOLO_RDR

/**
 * @brief Memory access macro
 */
#define BL_RD_WORD(addr)       (*((volatile uint32_t *)(uintptr_t)(addr)))
#define BL_WR_WORD(addr, val)  ((*(volatile uint32_t *)(uintptr_t)(addr)) = (val))
#define BL_RD_SHORT(addr)      (*((volatile uint16_t *)(uintptr_t)(addr)))
#define BL_WR_SHORT(addr, val) ((*(volatile uint16_t *)(uintptr_t)(addr)) = (val))
#define BL_RD_BYTE(addr)       (*((volatile uint8_t *)(uintptr_t)(addr)))
#define BL_WR_BYTE(addr, val)  ((*(volatile uint8_t *)(uintptr_t)(addr)) = (val))
#define BL_RDWD_FRM_BYTEP(p)   ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | (p[0]))

#define BL_WRWD_TO_BYTEP(p, val)   \
    {                              \
        p[0] = val & 0xff;         \
        p[1] = (val >> 8) & 0xff;  \
        p[2] = (val >> 16) & 0xff; \
        p[3] = (val >> 24) & 0xff; \
    }

/**
 * @brief Register access macro
 */
#define BL_RD_REG16(addr, regname)                BL_RD_SHORT(addr + regname##_OFFSET)
#define BL_WR_REG16(addr, regname, val)           BL_WR_SHORT(addr + regname##_OFFSET, val)
#define BL_RD_REG(addr, regname)                  BL_RD_WORD(addr + regname##_OFFSET)
#define BL_WR_REG(addr, regname, val)             BL_WR_WORD(addr + regname##_OFFSET, val)
#define BL_SET_REG_BIT(val, bitname)              ((val) | (1U << bitname##_POS))
#define BL_CLR_REG_BIT(val, bitname)              ((val)&bitname##_UMSK)
#define BL_GET_REG_BITS_VAL(val, bitname)         (((val)&bitname##_MSK) >> bitname##_POS)
#define BL_SET_REG_BITS_VAL(val, bitname, bitval) (((val)&bitname##_UMSK) | ((uint32_t)(bitval) << bitname##_POS))
#define BL_IS_REG_BIT_SET(val, bitname)           (((val) & (1U << (bitname##_POS))) != 0)



#define ATTR_STRINGIFY(x)                   #x
#define ATTR_TOSTRING(x)                    ATTR_STRINGIFY(x)
#define ATTR_UNI_SYMBOL                     __FILE__ ATTR_TOSTRING(__LINE__)
#define ATTR_CLOCK_SECTION                  __attribute__((section(".sclock_rlt_code." ATTR_UNI_SYMBOL)))
#define ATTR_CLOCK_CONST_SECTION            __attribute__((section(".sclock_rlt_const." ATTR_UNI_SYMBOL)))
#define ATTR_TCM_SECTION                    __attribute__((section(".tcm_code." ATTR_UNI_SYMBOL)))
#define ATTR_TCM_CONST_SECTION              __attribute__((section(".tcm_const." ATTR_UNI_SYMBOL)))
#define ATTR_DTCM_SECTION                   __attribute__((section(".tcm_data")))
#define ATTR_HSRAM_SECTION                  __attribute__((section(".hsram_code")))
#define ATTR_DMA_RAM_SECTION                __attribute__((section(".system_ram")))
#define ATTR_NOCACHE_RAM_SECTION            __attribute__((section(".nocache_ram")))
#define ATTR_NOCACHE_NOINIT_RAM_SECTION     __attribute__((section(".nocache_noinit_ram")))
#define ATTR_HBN_RAM_SECTION                __attribute__((section(".hbn_ram_code")))
#define ATTR_HBN_RAM_CONST_SECTION          __attribute__((section(".hbn_ram_data")))
#define ATTR_EALIGN(x)                      __attribute__((aligned(x)))
#define ATTR_FALLTHROUGH()                  __attribute__((fallthrough))
#define ATTR_USED                           __attribute__((__used__))












//

#define CHECK_PARAM(expr) //((expr) ? (void)0 : while(1);)

/**
 * @brief Error type definition
 */
typedef enum
{
  SUCCESS  = 0,
  ERROR   = 1,
  TIMEOUT = 2,
}BL_Err_Type;

/**
 * @brief Functional type definition
 */
typedef enum
{
  DISABLE  = 0,
  ENABLE   = 1,
}BL_Fun_Type;

/**
 * @brief Status type definition
 */
typedef enum
{
  RESET  = 0,
  SET   = 1,
}BL_Sts_Type;

/**
 * @brief Mask type definition
 */
typedef enum
{
  UNMASK = 0,
  MASK = 1
}BL_Mask_Type;

#define IS_BL_MASK_TYPE(type)          (((type) == MASK) || ((type) == UNMASK))












#define ARCH_MemCpy      arch_memcpy
#define ARCH_MemSet      arch_memset
#define ARCH_MemCmp      arch_memcmp
#define ARCH_MemCpy4     arch_memcpy4
#define ARCH_MemCpy_Fast arch_memcpy_fast
#define ARCH_MemSet4     arch_memset4

void *arch_memcpy(void *dst, const void *src, uint32_t n);
void *arch_memset(void *s, uint8_t c, uint32_t n);
int arch_memcmp(const void *s1, const void *s2, uint32_t n);
uint32_t *arch_memcpy4(uint32_t *dst, const uint32_t *src, uint32_t n);
void *arch_memcpy_fast(void *pdst, const void *psrc, uint32_t n);
uint32_t *arch_memset4(uint32_t *dst, const uint32_t val, uint32_t n);
void memcopy_to_fifo(void *fifo_addr, uint8_t *data, uint32_t length);
void fifocopy_to_mem(void *fifo_addr, uint8_t *data, uint32_t length);
int arch_ctzll(uint64_t *val, uint32_t *bit);
int arch_clzll(uint64_t *val, uint32_t *bit);
int arch_ffsll(uint64_t *val, uint32_t *bit);

















#if (__riscv_xlen == 32)
#include <core_rv32.h>
#elif (__riscv_xlen == 64)
#include <core_rv64.h>
#else
#error "XLEN of riscv not supported"
#endif





#endif

