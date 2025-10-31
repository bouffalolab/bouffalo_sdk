
/**
 ****************************************************************************************
 *
 * @file reg_access.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef REG_ACCESS_H_
#define REG_ACCESS_H_

/*****************************************************************************
 * Addresses within RWNX_ADDR_CPU
 *****************************************************************************/
#define RAM_LMAC_FW_ADDR     0x00000000

/*****************************************************************************
 * Addresses within RWNX_ADDR_SYSTEM
 *****************************************************************************/
/* Shard RAM */
#define SHARED_RAM_START_ADDR         0x00000000

/* IPC registers */
#define IPC_REG_BASE_ADDR             0x00800000

/* System Controller Registers */
#define SYSCTRL_SIGNATURE_ADDR        0x00900000
#define SYSCTRL_MISC_CNTL_ADDR        0x009000E0
#define  BOOTROM_ENABLE               BIT(4)
#define  FPGA_B_RESET                 BIT(1)
#define  SOFT_RESET                   BIT(0)

/* MAC platform */
#define NXMAC_SW_SET_PROFILING_ADDR   0x00B08564
#define NXMAC_SW_CLEAR_PROFILING_ADDR 0x00B08568

/* Modem Status */
#define MDM_HDMCONFIG_ADDR            0x00C00000

/* Modem Config */
#define MDM_MEMCLKCTRL0_ADDR          0x00C00848
#define MDM_CLKGATEFCTRL0_ADDR        0x00C00874

/* AGC (trident) */
#define AGC_RWNXAGCCNTL_ADDR          0x00C02060

/* LDPC RAM*/
#define PHY_LDPC_RAM_ADDR             0x00C09000

/* FCU (elma )*/
#define FCU_RWNXFCAGCCNTL_ADDR        0x00C09034

/* AGC RAM */
#define PHY_AGC_UCODE_ADDR            0x00C0A000

/* RIU */
#define RIU_RWNXVERSION_ADDR          0x00C0B000
#define RIU_RWNXDYNAMICCONFIG_ADDR    0x00C0B008
#define RIU_AGCMEMBISTSTAT_ADDR       0x00C0B238
#define RIU_AGCMEMSIGNATURESTAT_ADDR  0x00C0B23C
#define RIU_RWNXAGCCNTL_ADDR          0x00C0B390

/* FCU RAM */
#define PHY_FCU_UCODE_ADDR            0x00C0E000

/* */
#define FPGAB_MPIF_SEL_ADDR           0x00C10030

/*****************************************************************************
 * Macros for generated register files
 *****************************************************************************/
/* Macros for IPC registers access (used in reg_ipc_app.h) */
#define REG_IPC_APP_RD(env, INDEX)                                      \
    (*(volatile u32*)((u8*)env + IPC_REG_BASE_ADDR + 4*(INDEX)))

#define REG_IPC_APP_WR(env, INDEX, value)                               \
    (*(volatile u32*)((u8*)env + IPC_REG_BASE_ADDR + 4*(INDEX)) = value)

/* Macro used in reg_mac_core.h */
#define REG_PL_RD(addr)         0
#define REG_PL_WR(addr, value)

#endif /* REG_ACCESS_H_ */
