#include "bl616_clock.h"
#include "bl616_pds.h"
#include "bl616_hbn.h"
#include "bl616_psram.h"
#include "tzc_sec_reg.h"
#include "rv_hart.h"
#include "rv_pmp.h"

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

#if (!defined(CONFIG_PMP_NO_INIT) && !defined(CONFIG_BOOT2))

static const pmp_config_entry_t pmp_entry_tab[8] = {
    /* unused */
    [0] = { .entry_flag = ENTRY_FLAG_ADDR_OFF },

    /* 0x20000000 ~ 0x30000000, size:256M, peripherals/wireless/nocache_ram */
    [1] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x20000000,
        .entry_pa_length = PMP_REG_SZ_256M,
    },

    /* 0x62000000 ~ 0x64000000, size:32M, ocram/wram */
    [2] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x62000000,
        .entry_pa_length = PMP_REG_SZ_32M,
    },

    /* base:0x90000000 ~ 0x90040000, size:128K, rom */
    [3] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x90000000,
        .entry_pa_length = PMP_REG_SZ_128K,
    },

    /* 0xA0000000 ~ 0xA4000000, size:64M, flash */
    [4] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0xA0000000,
        .entry_pa_length = PMP_REG_SZ_64M,
    },

#ifdef CONFIG_PSRAM
    /* 0xA8000000 ~ 0xA9000000, size:16M, psram */
    [5] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0xA8000000,
        .entry_pa_length = PMP_REG_SZ_16M,
    },
#endif

    /* 0xE0000000 ~ 0xF0000000, size:256M, cpu reg */
    [6] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W,
        .entry_pa_base = 0xE0000000,
        .entry_pa_length = PMP_REG_SZ_256M,
    },

    /* base:0x00000000, size:4G, All remaining address Spaces */
    [7] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L,
        .entry_pa_base = 0x00000000,
        .entry_pa_length = PMP_REG_SZ_4G,
    },
};

void pmp_init(void)
{
    rvpmp_init(pmp_entry_tab, sizeof(pmp_entry_tab) / sizeof(pmp_entry_tab[0]));
}
#endif /* (!defined(CONFIG_PMP_NO_INIT) && !defined(CONFIG_BOOT2)) */

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
    HBN_BOD_CFG_Type bodCfg;
    bodCfg.enableBod = 1;
    /*0:BOD not trigger interrupt,1:trigger interrupt*/
    bodCfg.enableBodInt = 0;
    /* 0:2.05v,1:2.10v,2:2.15v....7:2.4v */
    bodCfg.bodThreshold = 7;
    /*1:BOD will cause reset */
    bodCfg.enablePorInBod = 1;
    HBN_Set_BOD_Cfg(&bodCfg);
}

void SystemInit(void)
{
    uint32_t i = 0;

    /* turn off U power */
    *(volatile uint32_t *)(0x2000e500) = (1 << 5);
    *(volatile uint32_t *)(0x2000e504) = 0;

    /* CPU Prefetching barrier */
    if (BL616_PSRAM_INIT_DONE == 0) {
        Tzc_Sec_PSRAMB_Access_Set_Not_Lock(0, 0x0, 64 * 1024 * 1024, 0);
    }
    Tzc_Sec_ROM_Access_Set_Not_Lock(1, 0x90020000, ((256 * 1024 * 1024) - (128 * 1024)), 0);
    flash_bank2_access_init();

#if (!defined(CONFIG_PMP_NO_INIT) && !defined(CONFIG_BOOT2))
    pmp_init();
#endif

    /* enable mstatus FS */
    uint32_t mstatus = __get_MSTATUS();
    mstatus |= (1 << 13);
    __set_MSTATUS(mstatus);

    /* enable mxstatus THEADISAEE */
    uint32_t mxstatus = __get_MXSTATUS();
    mxstatus |= (1 << 22);
    /* enable mxstatus MM */
    mxstatus |= (1 << 15);
    __set_MXSTATUS(mxstatus);

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    /* Every interrupt should be clear by software*/
    for (i = 0; i < IRQn_LAST; i++) {
        CLIC->CLICINT[i].IE = 0;
        CLIC->CLICINT[i].IP = 0;
#ifdef CONFIG_IRQ_USE_VECTOR
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
#else
        CLIC->CLICINT[i].ATTR = 0; /* use no vector interrupt */
#endif
    }

    /* tspend interrupt will be clear auto*/
    /* tspend use positive interrupt */
#ifdef CONFIG_IRQ_USE_VECTOR
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x3;
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x3;
#else
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x2;
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x2;
#endif
    csi_dcache_enable();
    csi_icache_enable();

    /* disable mexstatus SPUSHEN and SPSWAPEN for ipush/ipop*/
    uint32_t mexstatus = __get_MEXSTATUS();
    mexstatus &= ~(0x3 << 16);
    __set_MEXSTATUS(mexstatus);

    BL_WR_REG(GLB_BASE, GLB_UART_CFG1, 0xffffffff);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG2, 0x0000ffff);

    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    uint32_t tmpVal = 0;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SRAM_CFG3);

    if (em_size == 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00); // GLB_WRAM160KB_EM0KB
    } else if (em_size == 32 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x03); // GLB_WRAM128KB_EM32KB
    } else if (em_size == 64 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x0f); // GLB_WRAM96KB_EM64KB
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00); // GLB_WRAM160KB_EM0KB
    }
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG3, tmpVal);
}

static int ATTR_TCM_SECTION __attribute__((noinline)) system_setup_xtal_config()
{
    uint32_t tmpVal = 0;
    uint8_t mcuXclkSel;
    uint8_t mcuRootClkSel;
    int cnt = 0;

    mcuXclkSel = HBN_Get_MCU_XCLK_Sel();
    mcuRootClkSel = HBN_Get_MCU_Root_CLK_Sel();

    /* switch clock to rc32m */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);

    /* set xtal config */
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);

#ifdef CONFIG_XTAL_POWER_TYPE_ACTIVE
    /* for osc */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_AMP_CTRL_AON, 3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_EXT_SEL_AON, 1);
#else
    /* for crystal */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_AMP_CTRL_AON, 2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_EXT_SEL_AON, 0);
#endif
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    for (cnt = 0; cnt < 16; cnt++) {
        __NOP();
    }

    /* switch clock back */
    HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
    HBN_Set_MCU_Root_CLK_Sel(mcuRootClkSel);

    return 0;
}

void System_Post_Init(void)
{
    /* config chip pod */
    System_BOD_Init();

    /* Bootrom not use dcache,so ignore this flush*/
#ifndef BOOTROM
    csi_dcache_clean();
    csi_icache_invalid();
#endif

    system_setup_xtal_config();

#ifndef CONFIG_FREERTOS
    /* global IRQ enable */
    __enable_irq();
#endif
}
