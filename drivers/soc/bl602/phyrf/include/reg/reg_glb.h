#ifndef _REG_GLB_H_
#define _REG_GLB_H_

#include "co_int.h"
#include "arch.h"
#include "reg_access.h"

static inline uint32_t glb_glb_id_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline uint32_t glb_chip_rdy_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t glb_fclk_sw_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline uint32_t glb_reg_bclk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void glb_reg_bclk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(0x40000000,localVal);
}

static inline uint32_t glb_reg_hclk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void glb_reg_hclk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(0x40000000,localVal);
}

static inline uint32_t glb_hbn_root_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline uint32_t glb_reg_pll_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void glb_reg_pll_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(0x40000000,localVal);
}

static inline uint32_t glb_reg_bclk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_reg_bclk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000000,localVal);
}

static inline uint32_t glb_reg_hclk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_reg_hclk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000000,localVal);
}

static inline uint32_t glb_reg_fclk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_fclk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000000,localVal);
}

static inline uint32_t glb_reg_pll_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_pll_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000000);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000000,localVal);
}

static inline uint32_t glb_ble_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void glb_ble_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(0x40000004,localVal);
}

static inline uint32_t glb_ble_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void glb_ble_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(0x40000004,localVal);
}

static inline uint32_t glb_wifi_mac_wt_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void glb_wifi_mac_wt_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(0x40000004,localVal);
}

static inline uint32_t glb_wifi_mac_core_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void glb_wifi_mac_core_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000004);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(0x40000004,localVal);
}

static inline uint32_t glb_dma_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void glb_dma_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_ir_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void glb_ir_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_ir_clk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void glb_ir_clk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_sf_clk_sel2_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void glb_sf_clk_sel2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_sf_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void glb_sf_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_sf_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void glb_sf_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_sf_clk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void glb_sf_clk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_hbn_uart_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t glb_uart_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_uart_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_uart_clk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void glb_uart_clk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000008);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(0x40000008,localVal);
}

static inline uint32_t glb_i2c_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void glb_i2c_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(0x4000000c,localVal);
}

static inline uint32_t glb_i2c_clk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void glb_i2c_clk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(0x4000000c,localVal);
}

static inline uint32_t glb_spi_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_spi_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x4000000c,localVal);
}

static inline uint32_t glb_spi_clk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void glb_spi_clk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000000c);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0)&((uint32_t)0x0000001f);
    REG_PL_WR(0x4000000c,localVal);
}

static inline uint32_t glb_swrst_s30_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_swrst_s30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000010,localVal);
}

static inline uint32_t glb_swrst_s20_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_swrst_s20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000010,localVal);
}

static inline uint32_t glb_swrst_s01_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_swrst_s01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000010,localVal);
}

static inline uint32_t glb_swrst_s00_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_swrst_s00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000010);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000010,localVal);
}

static inline uint32_t glb_swrst_s1a7_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void glb_swrst_s1a7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a6_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void glb_swrst_s1a6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a5_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_swrst_s1a5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a4_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_swrst_s1a4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a3_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void glb_swrst_s1a3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a2_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void glb_swrst_s1a2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a1_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_swrst_s1a1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a0_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_swrst_s1a0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1f_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void glb_swrst_s1f_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1e_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void glb_swrst_s1e_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1d_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void glb_swrst_s1d_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1c_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void glb_swrst_s1c_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1b_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void glb_swrst_s1b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s1a_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void glb_swrst_s1a_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s19_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void glb_swrst_s19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s18_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_swrst_s18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s17_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_swrst_s17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s16_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void glb_swrst_s16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s15_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_swrst_s15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s14_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_swrst_s14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s13_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_swrst_s13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s12_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_swrst_s12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s11_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_swrst_s11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_swrst_s10_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_swrst_s10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000014);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000014,localVal);
}

static inline uint32_t glb_pka_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void glb_pka_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(0x40000018,localVal);
}

static inline uint32_t glb_reg_ctrl_reset_dummy_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void glb_reg_ctrl_reset_dummy_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(0x40000018,localVal);
}

static inline uint32_t glb_reg_ctrl_sys_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_reg_ctrl_sys_reset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000018,localVal);
}

static inline uint32_t glb_reg_ctrl_cpu_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_ctrl_cpu_reset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000018,localVal);
}

static inline uint32_t glb_reg_ctrl_pwron_rst_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_ctrl_pwron_rst_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000018);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000018,localVal);
}

static inline uint32_t glb_cgen_m_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000020);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void glb_cgen_m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000020);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(0x40000020,localVal);
}

static inline uint32_t glb_cgen_s1a_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000024);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void glb_cgen_s1a_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000024);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(0x40000024,localVal);
}

static inline uint32_t glb_cgen_s1_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000024);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void glb_cgen_s1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000024);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(0x40000024,localVal);
}

static inline uint32_t glb_cgen_s3_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000028);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_cgen_s3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000028);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000028,localVal);
}

static inline uint32_t glb_cgen_s2_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000028);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_cgen_s2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000028);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000028,localVal);
}

static inline uint32_t glb_reg_mbist_rst_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void glb_reg_mbist_rst_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(0x40000030,localVal);
}

static inline uint32_t glb_wifi_mbist_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_wifi_mbist_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000030,localVal);
}

static inline uint32_t glb_ocram_mbist_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_ocram_mbist_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000030,localVal);
}

static inline uint32_t glb_tag_mbist_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_tag_mbist_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000030,localVal);
}

static inline uint32_t glb_hsram_mbist_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_hsram_mbist_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000030,localVal);
}

static inline uint32_t glb_irom_mbist_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_irom_mbist_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000030);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000030,localVal);
}

static inline uint32_t glb_wifi_mbist_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t glb_ocram_mbist_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t glb_tag_mbist_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t glb_hsram_mbist_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t glb_irom_mbist_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t glb_wifi_mbist_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t glb_ocram_mbist_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t glb_tag_mbist_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t glb_hsram_mbist_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t glb_irom_mbist_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000034);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline uint32_t glb_r_adc_dac_test_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000040);
    return ((localVal & ((uint32_t)0x00000006)) >> 1);
}

static inline void glb_r_adc_dac_test_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000040);
    localVal &= ~((uint32_t)0x00000006);
    localVal |= (x << 1)&((uint32_t)0x00000006);
    REG_PL_WR(0x40000040,localVal);
}

static inline uint32_t glb_r_qfn56_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000040);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_r_qfn56_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000040);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000040,localVal);
}

static inline uint32_t glb_hbn_apb_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void glb_hbn_apb_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_pds_apb_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void glb_pds_apb_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_hsel_option_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void glb_hsel_option_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_bmx_gating_dis_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void glb_bmx_gating_dis_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_bmx_busy_option_dis_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void glb_bmx_busy_option_dis_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_bmx_err_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_bmx_err_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_bmx_arb_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void glb_bmx_arb_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_bmx_timeout_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void glb_bmx_timeout_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000050);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(0x40000050,localVal);
}

static inline uint32_t glb_bmx_dbg_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000054);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void glb_bmx_dbg_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000054);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(0x40000054,localVal);
}

static inline uint32_t glb_bmx_err_tz_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000054);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t glb_bmx_err_dec_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000054);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t glb_bmx_err_addr_dis_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000054);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_bmx_err_addr_dis_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000054);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000054,localVal);
}

static inline uint32_t glb_bmx_err_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000058);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline uint32_t glb_bmx_dbg_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000005c);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline uint32_t glb_rsvd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000060);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline uint32_t glb_reg_sram_ret_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000070);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_sram_ret_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000070);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x40000070,localVal);
}

static inline uint32_t glb_reg_sram_slp_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000074);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_sram_slp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000074);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x40000074,localVal);
}

static inline uint32_t glb_reg_sram_parm_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000078);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_sram_parm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000078);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x40000078,localVal);
}

static inline uint32_t glb_em_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000007c);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void glb_em_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000007c);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(0x4000007c,localVal);
}

static inline uint32_t glb_uart_swap_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void glb_uart_swap_set_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_p7_jtag_use_io_2_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void glb_p7_jtag_use_io_2_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_p6_sdio_use_io_0_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void glb_p6_sdio_use_io_0_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_p5_dac_test_with_jtag_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_p5_dac_test_with_jtag_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_p4_adc_test_with_jtag_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_p4_adc_test_with_jtag_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_p3_cci_use_io_2_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void glb_p3_cci_use_io_2_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_p2_dac_test_with_cci_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void glb_p2_dac_test_with_cci_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_p1_adc_test_with_cci_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_p1_adc_test_with_cci_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_reg_cci_use_sdio_pin_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_cci_use_sdio_pin_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_reg_cci_use_jtag_pin_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void glb_reg_cci_use_jtag_pin_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_reg_spi_0_swap_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void glb_reg_spi_0_swap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_reg_spi_0_master_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void glb_reg_spi_0_master_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_sel_embedded_sflash_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void glb_sel_embedded_sflash_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_swap_sflash_io_3_io_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_swap_sflash_io_3_io_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_jtag_swap_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void glb_jtag_swap_set_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_reg_ext_rst_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_ext_rst_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_reg_bd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_bd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000080);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000080,localVal);
}

static inline uint32_t glb_debug_ndreset_gate_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_debug_ndreset_gate_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000090,localVal);
}

static inline uint32_t glb_cpu_rtc_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void glb_cpu_rtc_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(0x40000090,localVal);
}

static inline uint32_t glb_cpu_rtc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void glb_cpu_rtc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(0x40000090,localVal);
}

static inline uint32_t glb_cpu_rtc_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    return ((localVal & ((uint32_t)0x0001ffff)) >> 0);
}

static inline void glb_cpu_rtc_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000090);
    localVal &= ~((uint32_t)0x0001ffff);
    localVal |= (x << 0)&((uint32_t)0x0001ffff);
    REG_PL_WR(0x40000090,localVal);
}

static inline uint32_t glb_gpadc_32m_div_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a4);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_gpadc_32m_div_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a4);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x400000a4,localVal);
}

static inline uint32_t glb_gpadc_32m_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a4);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_gpadc_32m_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a4);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x400000a4,localVal);
}

static inline uint32_t glb_gpadc_32m_clk_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a4);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void glb_gpadc_32m_clk_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a4);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(0x400000a4,localVal);
}

static inline uint32_t glb_reg_en_platform_wakeup_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void glb_reg_en_platform_wakeup_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_dig_clk_src_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void glb_dig_clk_src_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_dig_512k_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void glb_dig_512k_comp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_dig_512k_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void glb_dig_512k_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_dig_512k_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void glb_dig_512k_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_dig_32k_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void glb_dig_32k_comp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_dig_32k_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void glb_dig_32k_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_dig_32k_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void glb_dig_32k_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000a8);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(0x400000a8,localVal);
}

static inline uint32_t glb_en_gpio_bt_coex_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void glb_en_gpio_bt_coex_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(0x400000ac,localVal);
}

static inline uint32_t glb_coex_bt_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void glb_coex_bt_bw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(0x400000ac,localVal);
}

static inline uint32_t glb_coex_bt_pti_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    return ((localVal & ((uint32_t)0x00000780)) >> 7);
}

static inline void glb_coex_bt_pti_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    localVal &= ~((uint32_t)0x00000780);
    localVal |= (x << 7)&((uint32_t)0x00000780);
    REG_PL_WR(0x400000ac,localVal);
}

static inline uint32_t glb_coex_bt_channel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void glb_coex_bt_channel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000ac);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(0x400000ac,localVal);
}

static inline uint32_t glb_uart_sig_7_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void glb_uart_sig_7_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_6_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_uart_sig_6_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_5_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void glb_uart_sig_5_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_4_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void glb_uart_sig_4_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_3_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void glb_uart_sig_3_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_2_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_uart_sig_2_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_1_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void glb_uart_sig_1_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_0_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void glb_uart_sig_0_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c0);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(0x400000c0,localVal);
}

static inline uint32_t glb_uart_sig_11_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void glb_uart_sig_11_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(0x400000c4,localVal);
}

static inline uint32_t glb_uart_sig_10_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_uart_sig_10_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x400000c4,localVal);
}

static inline uint32_t glb_uart_sig_9_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void glb_uart_sig_9_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(0x400000c4,localVal);
}

static inline uint32_t glb_uart_sig_8_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void glb_uart_sig_8_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000c4);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(0x400000c4,localVal);
}

static inline uint32_t glb_reg_dbg_ll_ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000d0);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_dbg_ll_ctrl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000d0);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400000d0,localVal);
}

static inline uint32_t glb_reg_dbg_lh_ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000d4);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_dbg_lh_ctrl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000d4);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400000d4,localVal);
}

static inline uint32_t glb_reg_dbg_hl_ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000d8);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_dbg_hl_ctrl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000d8);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400000d8,localVal);
}

static inline uint32_t glb_reg_dbg_hh_ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000dc);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_dbg_hh_ctrl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000dc);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400000dc,localVal);
}

static inline uint32_t glb_debug_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000e0);
    return ((localVal & ((uint32_t)0xfffffffe)) >> 1);
}

static inline uint32_t glb_debug_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400000e0);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_debug_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400000e0);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x400000e0,localVal);
}

static inline uint32_t glb_real_gpio_1_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline uint32_t glb_reg_gpio_1_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_1_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_1_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_1_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_1_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_1_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_1_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_1_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_1_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_1_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_1_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_1_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_real_gpio_0_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline uint32_t glb_reg_gpio_0_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_0_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_0_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_0_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_0_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_0_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_0_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_0_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_0_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_0_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_reg_gpio_0_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_0_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000100);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000100,localVal);
}

static inline uint32_t glb_real_gpio_3_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline uint32_t glb_reg_gpio_3_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_3_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_3_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_3_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_3_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_3_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_3_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_3_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_3_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_3_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_3_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_3_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_real_gpio_2_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline uint32_t glb_reg_gpio_2_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_2_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_2_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_2_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_2_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_2_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_2_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_2_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_2_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_2_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_reg_gpio_2_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_2_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000104);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000104,localVal);
}

static inline uint32_t glb_real_gpio_5_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline uint32_t glb_reg_gpio_5_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_5_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_5_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_5_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_5_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_5_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_5_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_5_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_5_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_5_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_5_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_5_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_real_gpio_4_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline uint32_t glb_reg_gpio_4_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_4_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_4_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_4_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_4_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_4_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_4_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_4_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_4_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_4_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_4_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_4_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000108);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000108,localVal);
}

static inline uint32_t glb_reg_gpio_7_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_7_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_7_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_7_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_7_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_7_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_7_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_7_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_7_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_7_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_7_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_7_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_6_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_6_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_6_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_6_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_6_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_6_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_6_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_6_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_6_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_6_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_6_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_6_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000010c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000010c,localVal);
}

static inline uint32_t glb_reg_gpio_9_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_9_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_9_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_9_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_9_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_9_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_9_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_9_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_9_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_9_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_9_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_9_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_8_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_8_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_8_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_8_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_8_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_8_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_8_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_8_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_8_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_8_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_8_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_8_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000110);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000110,localVal);
}

static inline uint32_t glb_reg_gpio_11_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_11_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_11_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_11_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_11_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_11_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_11_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_11_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_11_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_11_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_11_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_11_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_10_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_10_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_10_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_10_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_10_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_10_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_10_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_10_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_10_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_10_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_10_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_10_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000114);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000114,localVal);
}

static inline uint32_t glb_reg_gpio_13_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_13_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_13_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_13_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_13_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_13_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_13_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_13_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_13_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_13_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_13_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_13_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_12_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_12_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_12_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_12_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_12_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_12_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_12_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_12_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_12_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_12_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_12_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_12_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000118);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000118,localVal);
}

static inline uint32_t glb_reg_gpio_15_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_15_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_15_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_15_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_15_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_15_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_15_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_15_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_15_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_15_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_15_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_15_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_14_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_14_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_14_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_14_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_14_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_14_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_14_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_14_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_14_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_14_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_14_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_14_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000011c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000011c,localVal);
}

static inline uint32_t glb_reg_gpio_17_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_17_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_17_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_17_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_17_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_17_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_17_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_17_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_17_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_17_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_17_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_17_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_16_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_16_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_16_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_16_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_16_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_16_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_16_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_16_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_16_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_16_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_16_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_16_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000120);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000120,localVal);
}

static inline uint32_t glb_reg_gpio_19_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_19_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_19_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_19_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_19_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_19_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_19_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_19_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_19_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_19_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_19_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_19_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_18_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_18_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_18_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_18_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_18_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_18_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_18_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_18_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_18_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_18_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_18_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_18_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000124);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000124,localVal);
}

static inline uint32_t glb_reg_gpio_21_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_21_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_21_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_21_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_21_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_21_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_21_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_21_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_21_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_21_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_21_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_21_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_20_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_20_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_20_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_20_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_20_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_20_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_20_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_20_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_20_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_20_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_20_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_20_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000128);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000128,localVal);
}

static inline uint32_t glb_reg_gpio_23_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_23_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_23_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_23_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_23_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_23_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_23_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_23_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_23_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_23_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_23_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_23_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_22_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_22_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_22_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_22_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_22_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_22_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_22_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_22_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_22_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_22_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_22_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_22_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000012c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000012c,localVal);
}

static inline uint32_t glb_reg_gpio_25_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_25_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_25_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_25_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_25_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_25_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_25_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_25_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_25_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_25_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_25_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_25_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_24_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_24_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_24_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_24_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_24_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_24_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_24_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_24_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_24_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_24_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_24_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_24_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000130);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000130,localVal);
}

static inline uint32_t glb_reg_gpio_27_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_27_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_27_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_27_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_27_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_27_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_27_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_27_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_27_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_27_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_27_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_27_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_26_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_26_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_26_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_26_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_26_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_26_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_26_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_26_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_26_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_26_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_26_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_26_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000134);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000134,localVal);
}

static inline uint32_t glb_reg_gpio_29_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_29_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_29_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_29_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_29_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_29_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_29_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_29_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_29_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_29_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_29_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_29_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_28_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_28_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_28_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_28_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_28_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_28_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_28_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_28_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_28_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_28_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_28_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_28_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000138);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000138,localVal);
}

static inline uint32_t glb_reg_gpio_31_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_31_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_31_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_31_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_31_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_31_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_31_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_31_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_31_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_31_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_31_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_31_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_30_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_30_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_30_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_30_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_30_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_30_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_30_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_30_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_30_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_30_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_30_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_30_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000013c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000013c,localVal);
}

static inline uint32_t glb_reg_gpio_33_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_33_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_33_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_33_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_33_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_33_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_33_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_33_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_33_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_33_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_33_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_33_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_32_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_32_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_32_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_32_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_32_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_32_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_32_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_32_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_32_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_32_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_32_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_32_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000140);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000140,localVal);
}

static inline uint32_t glb_reg_gpio_35_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_35_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_35_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_35_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_35_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_35_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_35_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_35_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_35_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_35_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_35_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_35_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_34_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_34_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_34_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_34_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_34_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_34_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_34_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_34_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_34_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_34_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_34_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_34_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000144);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000144,localVal);
}

static inline uint32_t glb_reg_gpio_37_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_37_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_37_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_37_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_37_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_37_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_37_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_37_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_37_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_37_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_37_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_37_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_36_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_36_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_36_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_36_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_36_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_36_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_36_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_36_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_36_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_36_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_36_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_36_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000148);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000148,localVal);
}

static inline uint32_t glb_reg_gpio_39_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void glb_reg_gpio_39_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_39_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_39_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_39_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_39_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_39_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_39_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_39_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_39_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_39_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_39_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_38_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_38_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_38_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_38_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_38_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_38_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_38_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_38_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_38_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_38_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_38_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_38_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000014c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000014c,localVal);
}

static inline uint32_t glb_reg_gpio_41_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_41_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_41_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_41_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_41_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_41_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_41_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_41_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_41_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_41_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_40_func_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void glb_reg_gpio_40_func_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_40_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_40_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_40_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_40_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_40_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_40_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_40_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_40_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_40_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_40_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000150);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000150,localVal);
}

static inline uint32_t glb_reg_gpio_43_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_43_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_43_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_43_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_43_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_43_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_43_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_43_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_43_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_43_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_42_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_42_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_42_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_42_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_42_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_42_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_42_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_42_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_42_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_42_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000154);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000154,localVal);
}

static inline uint32_t glb_reg_gpio_45_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_45_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_45_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_45_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_45_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_45_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_45_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_45_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_45_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_45_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_44_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_44_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_44_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_44_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_44_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_44_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_44_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_44_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_44_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_44_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000158);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000158,localVal);
}

static inline uint32_t glb_reg_gpio_47_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_47_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_47_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_47_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_47_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_47_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_47_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_47_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_47_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_47_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_46_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_46_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_46_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_46_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_46_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_46_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_46_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_46_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_46_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_46_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000015c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000015c,localVal);
}

static inline uint32_t glb_reg_gpio_49_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_49_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_49_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_49_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_49_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_49_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_49_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_49_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_49_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_49_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_48_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_48_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_48_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_48_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_48_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_48_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_48_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_48_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_48_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_48_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000160);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000160,localVal);
}

static inline uint32_t glb_reg_gpio_51_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_51_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_51_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_51_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_51_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_51_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_51_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_51_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_51_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_51_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_50_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_50_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_50_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_50_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_50_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_50_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_50_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_50_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_50_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_50_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000164);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000164,localVal);
}

static inline uint32_t glb_reg_gpio_53_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_53_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_53_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_53_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_53_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_53_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_53_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_53_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_53_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_53_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_52_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_52_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_52_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_52_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_52_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_52_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_52_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_52_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_52_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_52_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000168);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000168,localVal);
}

static inline uint32_t glb_reg_gpio_55_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_55_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_55_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_55_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_55_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_55_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_55_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_55_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_55_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_55_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_54_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_54_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_54_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_54_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_54_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_54_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_54_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_54_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_54_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_54_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000016c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000016c,localVal);
}

static inline uint32_t glb_reg_gpio_57_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_57_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_57_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_57_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_57_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_57_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_57_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_57_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_57_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_57_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_56_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_56_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_56_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_56_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_56_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_56_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_56_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_56_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_56_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_56_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000170);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000170,localVal);
}

static inline uint32_t glb_reg_gpio_59_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_59_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_59_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_59_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_59_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_reg_gpio_59_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_59_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_59_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_59_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_59_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_58_pd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_58_pd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_58_pu_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_58_pu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_58_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_reg_gpio_58_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_58_smt_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_58_smt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_58_ie_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_58_ie_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000174);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000174,localVal);
}

static inline uint32_t glb_reg_gpio_31_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t glb_reg_gpio_30_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline uint32_t glb_reg_gpio_29_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t glb_reg_gpio_28_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t glb_reg_gpio_27_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t glb_reg_gpio_26_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t glb_reg_gpio_25_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t glb_reg_gpio_24_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t glb_reg_gpio_23_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t glb_reg_gpio_22_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t glb_reg_gpio_21_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t glb_reg_gpio_20_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t glb_reg_gpio_19_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t glb_reg_gpio_18_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t glb_reg_gpio_17_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t glb_reg_gpio_16_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t glb_reg_gpio_15_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t glb_reg_gpio_14_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t glb_reg_gpio_13_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t glb_reg_gpio_12_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t glb_reg_gpio_11_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t glb_reg_gpio_10_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t glb_reg_gpio_9_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t glb_reg_gpio_8_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t glb_reg_gpio_7_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t glb_reg_gpio_6_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t glb_reg_gpio_5_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t glb_reg_gpio_4_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t glb_reg_gpio_3_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t glb_reg_gpio_2_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t glb_reg_gpio_1_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t glb_reg_gpio_0_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000180);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline uint32_t glb_reg_gpio_40_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t glb_reg_gpio_39_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t glb_reg_gpio_38_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t glb_reg_gpio_37_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t glb_reg_gpio_36_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t glb_reg_gpio_35_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t glb_reg_gpio_34_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t glb_reg_gpio_33_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t glb_reg_gpio_32_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000184);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline uint32_t glb_reg_gpio_31_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void glb_reg_gpio_31_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_30_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void glb_reg_gpio_30_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_29_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void glb_reg_gpio_29_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_28_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void glb_reg_gpio_28_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_27_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void glb_reg_gpio_27_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_26_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void glb_reg_gpio_26_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_25_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void glb_reg_gpio_25_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_24_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void glb_reg_gpio_24_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_23_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void glb_reg_gpio_23_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_22_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void glb_reg_gpio_22_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_21_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_21_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_20_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_20_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_19_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void glb_reg_gpio_19_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_18_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void glb_reg_gpio_18_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_17_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_17_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_16_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_16_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_15_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void glb_reg_gpio_15_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_14_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void glb_reg_gpio_14_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_13_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void glb_reg_gpio_13_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_12_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void glb_reg_gpio_12_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_11_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void glb_reg_gpio_11_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_10_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void glb_reg_gpio_10_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_9_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void glb_reg_gpio_9_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_8_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_reg_gpio_8_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_7_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_reg_gpio_7_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_6_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void glb_reg_gpio_6_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_5_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_5_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_4_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_4_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_3_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_reg_gpio_3_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_2_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_reg_gpio_2_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_1_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_1_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_0_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_0_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000188);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000188,localVal);
}

static inline uint32_t glb_reg_gpio_40_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_reg_gpio_40_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_39_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_reg_gpio_39_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_38_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void glb_reg_gpio_38_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_37_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_37_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_36_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_36_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_35_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_reg_gpio_35_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_34_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_reg_gpio_34_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_33_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_33_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_32_o_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_32_o_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000018c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000018c,localVal);
}

static inline uint32_t glb_reg_gpio_31_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void glb_reg_gpio_31_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_30_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void glb_reg_gpio_30_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_29_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void glb_reg_gpio_29_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_28_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void glb_reg_gpio_28_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_27_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void glb_reg_gpio_27_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_26_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void glb_reg_gpio_26_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_25_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void glb_reg_gpio_25_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_24_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void glb_reg_gpio_24_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_23_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void glb_reg_gpio_23_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_22_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void glb_reg_gpio_22_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_21_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_reg_gpio_21_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_20_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_reg_gpio_20_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_19_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void glb_reg_gpio_19_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_18_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void glb_reg_gpio_18_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_17_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void glb_reg_gpio_17_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_16_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void glb_reg_gpio_16_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_15_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void glb_reg_gpio_15_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_14_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void glb_reg_gpio_14_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_13_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void glb_reg_gpio_13_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_12_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void glb_reg_gpio_12_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_11_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void glb_reg_gpio_11_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_10_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void glb_reg_gpio_10_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_9_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void glb_reg_gpio_9_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_8_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_reg_gpio_8_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_7_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_reg_gpio_7_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_6_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void glb_reg_gpio_6_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_5_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_5_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_4_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_4_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_3_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_reg_gpio_3_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_2_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_reg_gpio_2_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_1_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_1_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_0_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_0_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000190);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000190,localVal);
}

static inline uint32_t glb_reg_gpio_40_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_reg_gpio_40_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_39_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_reg_gpio_39_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_38_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void glb_reg_gpio_38_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_37_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_reg_gpio_37_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_36_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_reg_gpio_36_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_35_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_reg_gpio_35_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_34_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_reg_gpio_34_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_33_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_reg_gpio_33_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_32_oe_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_reg_gpio_32_oe_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000194);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000194,localVal);
}

static inline uint32_t glb_reg_gpio_int_mask1_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400001a0);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_gpio_int_mask1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400001a0);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400001a0,localVal);
}

static inline uint32_t glb_gpio_int_stat1_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400001a8);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline uint32_t glb_reg_gpio_int_clr1_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400001b0);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_gpio_int_clr1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400001b0);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400001b0,localVal);
}

static inline uint32_t glb_reg_gpio_int_mode_set1_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400001c0);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_gpio_int_mode_set1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400001c0);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400001c0,localVal);
}

static inline uint32_t glb_reg_gpio_int_mode_set2_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400001c4);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_gpio_int_mode_set2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400001c4);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400001c4,localVal);
}

static inline uint32_t glb_reg_gpio_int_mode_set3_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x400001c8);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void glb_reg_gpio_int_mode_set3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x400001c8);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(0x400001c8,localVal);
}

static inline uint32_t glb_ppu_dll_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void glb_ppu_dll_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_pu_dll_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void glb_pu_dll_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void glb_dll_reset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_refclk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void glb_dll_refclk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_cp_hiz_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void glb_dll_cp_hiz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_cp_op_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void glb_dll_cp_op_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_delay_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void glb_dll_delay_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_post_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void glb_dll_post_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_vctrl_force_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void glb_dll_vctrl_force_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_prechg_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void glb_dll_prechg_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_prechg_reg_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void glb_dll_prechg_reg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_prechg_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void glb_dll_prechg_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_vctrl_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void glb_dll_vctrl_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_clk_57p6m_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_dll_clk_57p6m_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_clk_96m_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void glb_dll_clk_96m_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_clk_144m_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void glb_dll_clk_144m_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_clk_288m_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_dll_clk_288m_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dll_clk_mmdiv_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_dll_clk_mmdiv_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_ten_dll_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_ten_dll_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dtest_en_dll_outclk_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_dtest_en_dll_outclk_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_dtest_en_dll_refclk_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_dtest_en_dll_refclk_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000200);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000200,localVal);
}

static inline uint32_t glb_pu_ldo18io_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void glb_pu_ldo18io_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_pulldown_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void glb_ldo18io_pulldown_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_pulldown_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void glb_ldo18io_pulldown_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void glb_ldo18io_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_sw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void glb_ldo18io_sw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_sw2_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void glb_ldo18io_sw2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_sw3_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void glb_ldo18io_sw3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_vout_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void glb_ldo18io_vout_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_cc_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void glb_ldo18io_cc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_sstart_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x00000060)) >> 5);
}

static inline void glb_ldo18io_sstart_delay_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x00000060);
    localVal |= (x << 5)&((uint32_t)0x00000060);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ldo18io_sstart_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void glb_ldo18io_sstart_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_ten_ldo18io_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_ten_ldo18io_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000220);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000220,localVal);
}

static inline uint32_t glb_pu_leddrv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void glb_pu_leddrv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(0x40000224,localVal);
}

static inline uint32_t glb_ir_rx_gpio_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void glb_ir_rx_gpio_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(0x40000224,localVal);
}

static inline uint32_t glb_leddrv_ibias_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void glb_leddrv_ibias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(0x40000224,localVal);
}

static inline uint32_t glb_led_din_polarity_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void glb_led_din_polarity_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(0x40000224,localVal);
}

static inline uint32_t glb_led_din_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_led_din_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000224,localVal);
}

static inline uint32_t glb_led_din_reg_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_led_din_reg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000224);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000224,localVal);
}

static inline uint32_t glb_usb_rcv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t glb_usb_vip_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t glb_usb_vim_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t glb_usb_bd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t glb_pu_usb_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void glb_pu_usb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_usb_sus_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void glb_usb_sus_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_usb_spd_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void glb_usb_spd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_usb_enum_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void glb_usb_enum_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_usb_data_convert_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t glb_usb_oeb_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t glb_usb_oeb_reg_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t glb_usb_oeb_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t glb_usb_rout_pmos_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void glb_usb_rout_pmos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_usb_rout_nmos_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void glb_usb_rout_nmos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_pu_usb_ldo_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void glb_pu_usb_ldo_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_usb_ldo_vfb_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void glb_usb_ldo_vfb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000228);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(0x40000228,localVal);
}

static inline uint32_t glb_usb_slewrate_p_rise_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void glb_usb_slewrate_p_rise_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_slewrate_p_fall_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void glb_usb_slewrate_p_fall_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_slewrate_m_rise_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void glb_usb_slewrate_m_rise_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_slewrate_m_fall_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void glb_usb_slewrate_m_fall_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_res_pullup_tune_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void glb_usb_res_pullup_tune_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_reg_usb_use_ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void glb_reg_usb_use_ctrl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_str_drv_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void glb_usb_str_drv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_reg_usb_use_xcvr_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_reg_usb_use_xcvr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_bd_vth_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void glb_usb_bd_vth_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_v_hys_p_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void glb_usb_v_hys_p_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_usb_v_hys_m_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void glb_usb_v_hys_m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000022c);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(0x4000022c,localVal);
}

static inline uint32_t glb_gpdac_reserved_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void glb_gpdac_reserved_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(0x40000308,localVal);
}

static inline uint32_t glb_gpdac_test_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    return ((localVal & ((uint32_t)0x00000e00)) >> 9);
}

static inline void glb_gpdac_test_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    localVal &= ~((uint32_t)0x00000e00);
    localVal |= (x << 9)&((uint32_t)0x00000e00);
    REG_PL_WR(0x40000308,localVal);
}

static inline uint32_t glb_gpdac_ref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void glb_gpdac_ref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(0x40000308,localVal);
}

static inline uint32_t glb_gpdac_test_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void glb_gpdac_test_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(0x40000308,localVal);
}

static inline uint32_t glb_gpdacb_rstn_ana_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_gpdacb_rstn_ana_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000308,localVal);
}

static inline uint32_t glb_gpdaca_rstn_ana_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_gpdaca_rstn_ana_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000308);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000308,localVal);
}

static inline uint32_t glb_gpdac_a_outmux_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void glb_gpdac_a_outmux_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(0x4000030c,localVal);
}

static inline uint32_t glb_gpdac_a_rng_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_gpdac_a_rng_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x4000030c,localVal);
}

static inline uint32_t glb_gpdac_ioa_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_gpdac_ioa_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x4000030c,localVal);
}

static inline uint32_t glb_gpdac_a_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_gpdac_a_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x4000030c);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x4000030c,localVal);
}

static inline uint32_t glb_gpdac_b_outmux_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void glb_gpdac_b_outmux_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(0x40000310,localVal);
}

static inline uint32_t glb_gpdac_b_rng_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void glb_gpdac_b_rng_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(0x40000310,localVal);
}

static inline uint32_t glb_gpdac_iob_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void glb_gpdac_iob_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(0x40000310,localVal);
}

static inline uint32_t glb_gpdac_b_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void glb_gpdac_b_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000310);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(0x40000310,localVal);
}

static inline uint32_t glb_gpdac_a_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000314);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void glb_gpdac_a_data_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000314);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(0x40000314,localVal);
}

static inline uint32_t glb_gpdac_b_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000314);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void glb_gpdac_b_data_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(0x40000314);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(0x40000314,localVal);
}

static inline uint32_t glb_tzc_glb_clk_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t glb_tzc_glb_mbist_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline uint32_t glb_tzc_glb_dbg_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t glb_tzc_glb_bmx_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t glb_tzc_glb_l2c_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t glb_tzc_glb_sram_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t glb_tzc_glb_misc_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t glb_tzc_glb_ctrl_ungated_ap_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t glb_tzc_glb_ctrl_sys_reset_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t glb_tzc_glb_ctrl_cpu_reset_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t glb_tzc_glb_ctrl_pwron_rst_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t glb_tzc_glb_swrst_s30_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t glb_tzc_glb_swrst_s01_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t glb_tzc_glb_swrst_s00_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f00);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline uint32_t glb_tzc_glb_swrst_s1f_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t glb_tzc_glb_swrst_s1e_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline uint32_t glb_tzc_glb_swrst_s1d_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t glb_tzc_glb_swrst_s1c_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t glb_tzc_glb_swrst_s1b_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t glb_tzc_glb_swrst_s1a_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t glb_tzc_glb_swrst_s19_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t glb_tzc_glb_swrst_s18_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t glb_tzc_glb_swrst_s17_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t glb_tzc_glb_swrst_s16_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t glb_tzc_glb_swrst_s15_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t glb_tzc_glb_swrst_s14_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t glb_tzc_glb_swrst_s13_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t glb_tzc_glb_swrst_s12_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t glb_tzc_glb_swrst_s11_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t glb_tzc_glb_swrst_s10_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t glb_tzc_glb_swrst_s2f_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t glb_tzc_glb_swrst_s2e_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t glb_tzc_glb_swrst_s2d_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t glb_tzc_glb_swrst_s2c_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t glb_tzc_glb_swrst_s2b_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t glb_tzc_glb_swrst_s2a_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t glb_tzc_glb_swrst_s29_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t glb_tzc_glb_swrst_s28_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t glb_tzc_glb_swrst_s27_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t glb_tzc_glb_swrst_s26_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t glb_tzc_glb_swrst_s25_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t glb_tzc_glb_swrst_s24_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t glb_tzc_glb_swrst_s23_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t glb_tzc_glb_swrst_s22_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t glb_tzc_glb_swrst_s21_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t glb_tzc_glb_swrst_s20_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f04);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline uint32_t glb_tzc_glb_gpio_31_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t glb_tzc_glb_gpio_30_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline uint32_t glb_tzc_glb_gpio_29_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t glb_tzc_glb_gpio_28_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t glb_tzc_glb_gpio_27_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t glb_tzc_glb_gpio_26_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t glb_tzc_glb_gpio_25_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t glb_tzc_glb_gpio_24_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t glb_tzc_glb_gpio_23_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t glb_tzc_glb_gpio_22_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t glb_tzc_glb_gpio_21_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t glb_tzc_glb_gpio_20_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t glb_tzc_glb_gpio_19_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t glb_tzc_glb_gpio_18_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t glb_tzc_glb_gpio_17_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t glb_tzc_glb_gpio_16_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t glb_tzc_glb_gpio_15_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t glb_tzc_glb_gpio_14_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t glb_tzc_glb_gpio_13_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t glb_tzc_glb_gpio_12_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t glb_tzc_glb_gpio_11_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t glb_tzc_glb_gpio_10_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t glb_tzc_glb_gpio_9_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t glb_tzc_glb_gpio_8_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t glb_tzc_glb_gpio_7_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t glb_tzc_glb_gpio_6_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t glb_tzc_glb_gpio_5_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t glb_tzc_glb_gpio_4_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t glb_tzc_glb_gpio_3_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t glb_tzc_glb_gpio_2_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t glb_tzc_glb_gpio_1_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t glb_tzc_glb_gpio_0_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f08);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline uint32_t glb_tzc_glb_gpio_59_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t glb_tzc_glb_gpio_58_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t glb_tzc_glb_gpio_57_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t glb_tzc_glb_gpio_56_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t glb_tzc_glb_gpio_55_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t glb_tzc_glb_gpio_54_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t glb_tzc_glb_gpio_53_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t glb_tzc_glb_gpio_52_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t glb_tzc_glb_gpio_51_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t glb_tzc_glb_gpio_50_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t glb_tzc_glb_gpio_49_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t glb_tzc_glb_gpio_48_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t glb_tzc_glb_gpio_47_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t glb_tzc_glb_gpio_46_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t glb_tzc_glb_gpio_45_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t glb_tzc_glb_gpio_44_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t glb_tzc_glb_gpio_43_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t glb_tzc_glb_gpio_42_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t glb_tzc_glb_gpio_41_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t glb_tzc_glb_gpio_40_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t glb_tzc_glb_gpio_39_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t glb_tzc_glb_gpio_38_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t glb_tzc_glb_gpio_37_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t glb_tzc_glb_gpio_36_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t glb_tzc_glb_gpio_35_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t glb_tzc_glb_gpio_34_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t glb_tzc_glb_gpio_33_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t glb_tzc_glb_gpio_32_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(0x40000f0c);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

#endif