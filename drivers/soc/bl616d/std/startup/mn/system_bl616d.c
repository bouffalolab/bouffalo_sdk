#include "bl616d_clock.h"
#include "bl616d_pds.h"
#include "bl616d_hbn.h"
#include "bl616d_psram.h"
#include "tzc_sec_reg.h"
#if defined(CPU_MODEL_A0)
#include "rv_hart.h"
#include "rv_pmp.h"
#else
#include <nmsis_core.h>
#endif

/* sf_ctrl_2 */
#define SF_CTRL_2_OFFSET       (0x70)
#define SF_CTRL_SF_IF_BK2_MODE (1 << 29U)
#define SF_CTRL_SF_IF_BK2_EN   (1 << 30U)

static void Tzc_Sec_PSRAMB_Access_Set_Not_Lock(uint8_t region, uint32_t startAddr, uint32_t endAddr, uint8_t group)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_PSRAMB_TZSRG_CTRL);
    tmpVal &= (~(3 << (region * 2)));
    tmpVal |= (group << (region * 2));
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_PSRAMB_TZSRG_CTRL, tmpVal);

    tmpVal = ((((endAddr >> 10) & 0xffff) - 1) & 0xffff) | (((startAddr >> 10) & 0xffff) << 16);
    BL_WR_WORD(TZC_SEC_BASE + TZC_SEC_TZC_PSRAMB_TZSRG_R0_OFFSET + region * 4, tmpVal);

    /* set enable but not lock */
    tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_PSRAMB_TZSRG_CTRL);
    tmpVal |= 1 << (region + 16);
    //tmpVal |= 1<<(region+24);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_PSRAMB_TZSRG_CTRL, tmpVal);
}

#if 0
static void Tzc_Sec_ROM_Access_Set_Not_Lock(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)
{
    uint32_t tmpVal = 0;
    uint32_t alignEnd = (startAddr + length + 1023) & ~0x3FF;

    /* check the parameter */
    group = group & 0xf;

    tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL);
    tmpVal &= (~(0xf << (region * 4)));
    tmpVal |= (group << (region * 4));
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL, tmpVal);

    tmpVal = (((alignEnd >> 10) & 0xffff) - 1) | (((startAddr >> 10) & 0xffff) << 16);
    BL_WR_WORD(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_R0_OFFSET + region * 4, tmpVal);

    /* set enable and lock */
    tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL);
    tmpVal |= 1 << (region + 16);
    // tmpVal |= 1 << (region + 24);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL, tmpVal);
}

static void pmp_init(void)
{
    const pmp_config_entry_t pmp_entry_tab[6] = {
        /* no access 0x00000000-0x20000000*/
        [0] = {
            .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L,
            .entry_pa_base = 0x00000000,
            .entry_pa_length = PMP_REG_SZ_512M,
        },
        /* no access 0x40000000-0x60000000*/
        [1] = {
            .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L,
            .entry_pa_base = 0x40000000,
            .entry_pa_length = PMP_REG_SZ_512M,
        },
        /* no access 0x70000000-0x80000000*/
        [2] = {
            .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L,
            .entry_pa_base = 0x70000000,
            .entry_pa_length = PMP_REG_SZ_256M,
        },
        /* no access 0x80000000-0x90000000*/
        [3] = {
            .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L,
            .entry_pa_base = 0x80000000,
            .entry_pa_length = PMP_REG_SZ_256M,
        },
        /* no access 0xB0000000-0xC0000000*/
        [4] = {
            .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L,
            .entry_pa_base = 0xB0000000,
            .entry_pa_length = PMP_REG_SZ_256M,
        },
        /* no access 0xC0000000-0xE0000000*/
        [5] = {
            .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L,
            .entry_pa_base = 0xC0000000,
            .entry_pa_length = PMP_REG_SZ_512M,
        }
    };
    rvpmp_init(pmp_entry_tab, sizeof(pmp_entry_tab) / sizeof(pmp_config_entry_t));
}
#endif

static void flash_bank2_access_init(void)
{
    uint32_t reg_base = 0;
    uint32_t regval = 0;
    reg_base = BFLB_SF_CTRL_BASE;
    regval = getreg32(reg_base + SF_CTRL_2_OFFSET);
    regval |= SF_CTRL_SF_IF_BK2_EN;
    regval |= SF_CTRL_SF_IF_BK2_MODE;
    putreg32(regval, reg_base + SF_CTRL_2_OFFSET);
}

void System_BOD_Init(void)
{
    uint8_t enableBod = 1;
    /*0:BOD not trigger interrupt,1:trigger interrupt*/
    uint8_t enableBodInt = 1;
    /* 0:2.2v,1:2.3v,2:2.4v....7:2.9v */
    uint8_t bodThreshold = 7;

    AON_Set_BOD_Config(enableBod, bodThreshold, enableBodInt);
}

static void __cpu_pre_init(void)
{
    uint32_t i = 0;

    //pmp_init();

    /* enable mstatus FS */
    __enable_FPU();

    /* get interrupt level from info */
    __ECLIC_SetCfgNlbits(__ECLIC_GetInfoCtlbits());

    /* Every interrupt should be clear by software*/
    for (i = 0; i < IRQn_LAST; i++) {
        __ECLIC_DisableIRQ(i);
        __ECLIC_ClearPendingIRQ(i);
#ifdef CONFIG_IRQ_USE_VECTOR
        __ECLIC_SetShvIRQ(i, 1); /* use vector interrupt */
#else
        __ECLIC_SetShvIRQ(i, 0); /* use no vector interrupt */
#endif
    }

    /* tspend interrupt will be clear auto*/
    /* tspend use positive interrupt */
#ifdef CONFIG_IRQ_USE_VECTOR
    __ECLIC_SetShvIRQ(MSOFT_IRQn, 1);
    __ECLIC_SetTrigIRQ(MSOFT_IRQn, 1);
    //__ECLIC_SetShvIRQ(SDU_SOFT_RST_IRQn, 1);
    //__ECLIC_SetTrigIRQ(SDU_SOFT_RST_IRQn, 1);
#else
    __ECLIC_SetShvIRQ(MSOFT_IRQn, 0);
    __ECLIC_SetTrigIRQ(MSOFT_IRQn, 1);
    //__ECLIC_SetShvIRQ(SDU_SOFT_RST_IRQn, 0);
    //__ECLIC_SetTrigIRQ(SDU_SOFT_RST_IRQn, 1);
#endif
    EnableICache();
    EnableDCache();
}

static void __cpu_post_init(void)
{
    /* Bootrom not use dcache,so ignore this flush*/
#ifndef BOOTROM
    MFlushDCache();
    MInvalICache();
#endif

#ifndef CONFIG_FREERTOS
    /* global IRQ enable */
    __enable_irq();
#endif
}

#ifndef CPU_NP /* for AP */
void SystemInit(void)
{
#ifdef CONFIG_USB_DISABLED
    PDS_Turn_Off_USB();
#endif

    /* CPU Prefetching barrier */
    if (BL616D_PSRAM_INIT_DONE == 0) {
        Tzc_Sec_PSRAMB_Access_Set_Not_Lock(0, 0x0, 64 * 1024 * 1024, 0);
    }
    //Tzc_Sec_ROM_Access_Set_Not_Lock(1, 0x90020000, ((256 * 1024 * 1024) - (128 * 1024)), 0);
    flash_bank2_access_init();

    __cpu_pre_init();

    BL_WR_REG(GLB_BASE, GLB_UART_CFG1, 0xffffffff);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG2, 0x0000ffff);
}

void System_Post_Init(void)
{
    __cpu_post_init();

    /* config chip pod */
    System_BOD_Init();

    PDS_Trim_RC32M();
}

void SystemInit_NP(void)
{
    __cpu_pre_init();
    __cpu_post_init();
}
#else /* for NP */
void SystemInit(void)
{
    __cpu_pre_init();
}

void System_Post_Init(void)
{
    __cpu_post_init();
}
#endif
