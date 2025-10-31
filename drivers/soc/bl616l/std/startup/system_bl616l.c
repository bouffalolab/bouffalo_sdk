#include "bl616l_clock.h"
#include "bl616l_pds.h"
#include "bl616l_hbn.h"
#include "bl616l_psram.h"
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

static const pmp_config_entry_t pmp_entry_tab[16] = {
    /* unused */
    [8] = { .entry_flag = ENTRY_FLAG_ADDR_OFF },

    /* 0x20000000 ~ 0x30000000, size:256M, peripherals/wireless/nocache_ram */
    [9] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x20000000,
        .entry_pa_length = PMP_REG_SZ_256M,
    },

    /* 0x60000000 ~ 0x62000000, size:32M, ocram/wram */
    [10] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x60000000,
        .entry_pa_length = PMP_REG_SZ_32M,
    },

    /* base:0x90000000 ~ 0x90040000, size:256K, rom */
    [11] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x90000000,
        .entry_pa_length = PMP_REG_SZ_256K,
    },

    /* 0x80000000 ~ 0x84000000, size:64M, flash */
    [12] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x80000000,
        .entry_pa_length = PMP_REG_SZ_64M,
    },

#ifdef CONFIG_PSRAM
    /* 0x88000000 ~ 0x89000000, size:16M, psram */
    [13] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W | ENTRY_FLAG_PERM_X,
        .entry_pa_base = 0x88000000,
        .entry_pa_length = PMP_REG_SZ_16M,
    },
#endif

    /* 0xE0000000 ~ 0xF0000000, size:256M, cpu reg */
    [14] = {
        .entry_flag = ENTRY_FLAG_ADDR_NAPOT | ENTRY_FLAG_M_MODE_L | ENTRY_FLAG_PERM_R | ENTRY_FLAG_PERM_W,
        .entry_pa_base = 0xE0000000,
        .entry_pa_length = PMP_REG_SZ_256M,
    },

    /* base:0x00000000, size:4G, All remaining address Spaces */
    [15] = {
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
    /* 0:2.2v, 1:2.3v, 2:2.4v .... 7:2.9v */
    bodCfg.bodThreshold = 7;
    /*1:BOD will cause reset, 0:BOD will not cause reset */
    bodCfg.enablePorInBod = 1;
    HBN_Set_BOD_Cfg(&bodCfg);
}

void SystemInit(void)
{
    uint32_t i = 0;

    /* CPU Prefetching barrier */
    if (BL616L_PSRAM_INIT_DONE == 0) {
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
    } else if (em_size == 16 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x01); // GLB_WRAM144KB_EM16KB
    } else if (em_size == 32 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x03); // GLB_WRAM128KB_EM32KB
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00); // GLB_WRAM160KB_EM0KB
    }
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG3, tmpVal);
}

void System_Post_Init(void)
{
    /* Bootrom not use dcache,so ignore this flush*/
#ifndef BOOTROM
    csi_dcache_clean();
    csi_icache_invalid();
#endif

#ifndef CONFIG_FREERTOS
    /* global IRQ enable */
    __enable_irq();
#endif

    /* config chip pod */
    System_BOD_Init();

}
