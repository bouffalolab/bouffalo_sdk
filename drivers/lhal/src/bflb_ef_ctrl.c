#include "bflb_ef_ctrl.h"
#include "hardware/ef_ctrl_reg.h"
#include "bflb_clock.h"

#if defined(BL602) || defined(BL702) || defined(BL702L)
#define BFLB_EF_CTRL_BASE ((uint32_t)0x40007000)
#elif defined(BL616) || defined(BL616L) || defined(BL808) || defined(BL606P)
#define BFLB_EF_CTRL_BASE ((uint32_t)0x20056000)
#elif defined(BL628) || defined(BL616D)
#define BFLB_EF_CTRL_BASE ((uint32_t)0x2000C000)
#endif

#define EF_CTRL_EFUSE_CTRL_PROTECT (0xbf << 8)
#define EF_CTRL_OP_MODE_AUTO 0
#if defined(BL616L)
#define EF_CTRL_PARA_DFT 1
#define EF_CTRL_EFUSE_CYCLE_PROTECT (0xbf << 24)
#else
#define EF_CTRL_PARA_DFT 0
#define EF_CTRL_EFUSE_CYCLE_PROTECT (0x0)
#endif
#if defined(BL702) || defined(BL602) || defined(BL702L)
#define EF_CTRL_EF_CLK 0
#define EF_CTRL_SAHB_CLK 1
#endif
#define EF_CTRL_DFT_TIMEOUT_VAL (160 * 1000)

#if defined(BL702) || defined(BL602) || defined(BL702L)
#define EF_CTRL_EFUSE_R0_SIZE 128
#elif defined(BL606P) || defined(BL808)
#define EF_CTRL_EFUSE_R0_SIZE 128
#define EF_CTRL_EFUSE_R1_SIZE 128
#elif defined(BL616) || defined(BL628)
#define EF_CTRL_EFUSE_R0_SIZE 512
#elif defined(BL616D)
#define EF_CTRL_EFUSE_R0_SIZE 256
#elif defined(BL616L)
#define EF_CTRL_EFUSE_R0_SIZE 128
#else
#define EF_CTRL_EFUSE_R0_SIZE 128
#endif

#define EF_CTRL_DATA0_CLEAR bflb_ef_ctrl_clear_data_reg0(dev)
#define EF_CTRL_DATA1_CLEAR bflb_ef_ctrl_clear_data_reg1(dev)

#if defined(BL702) || defined(BL602) || defined(BL702L)
extern void bflb_efuse_switch_cpu_clock_save(void);
extern void bflb_efuse_switch_cpu_clock_restore(void);
#endif

#if defined(BL616L)
extern void AON_LDO18_IO_Switch_Efuse(uint8_t enable);
#define bflb_power_on_efuse()  AON_LDO18_IO_Switch_Efuse(1)
#define bflb_power_off_efuse() AON_LDO18_IO_Switch_Efuse(0)
#elif defined(BL616D)
extern void AON_Set_Switch_For_Efuse(uint8_t enable);
#define bflb_power_on_efuse()  AON_Set_Switch_For_Efuse(1);
#define bflb_power_off_efuse()  AON_Set_Switch_For_Efuse(0);
#else
#define bflb_power_on_efuse()
#define bflb_power_off_efuse()
#endif

__UNUSED static ATTR_TCM_SECTION size_t bflb_ef_ctrl_strlen(const char *s)
{
    const char *sc;
    for (sc = s; *sc != '\0'; ++sc) {}
    return sc - s;
}

/****************************************************************************/ /**
 * @brief  Check efuse busy status
 *
 * @param dev  ef control device pointer
 *
 * @return 1 for busy 0 for not
 *
*******************************************************************************/
int ATTR_TCM_SECTION bflb_ef_ctrl_busy(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_ef_ctrl_busy
    return romapi_bflb_ef_ctrl_busy(dev);
#else
    uint32_t reg_val;

    reg_val = getreg32(BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    if (reg_val & EF_CTRL_EF_IF_0_BUSY_MASK) {
        return 1;
    }

    return 0;
#endif
}

/****************************************************************************/ /**
 * @brief  Check efuse busy status
 *
 * @param dev  ef control device pointer
 *
 * @return 1 for busy 0 for not
 *
*******************************************************************************/
__UNUSED static int ATTR_TCM_SECTION bflb_ef_ctrl_update_para(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_ef_ctrl_update_para
    return bflb_ef_ctrl_update_para(dev);
#else
#if defined(BL616L)
    bflb_ef_ctrl_para_t para = {
        .pd_1st = 0x03,  /*!< stable */
        .pd_cs_s = 0x30, /*!< >500ns */
        .cs = 0x0d,      /*!< >6.6ns */
        .rd_adr = 0x0a,  /*!< >6.3ns */
        .rd_dat = 0x0b,  /*!< >199ns */
        .rd_dmy = 0x0f,  /*!< >14.9ns */
        .pd_cs_h = 0x0d, /*!< >1ns */
        .ps_cs = 0x20,   /*!< >50ns */
        .wr_adr = 0x0d,  /*!< >6.3ns */
        .pp = 0x479,     /*!< *4 >11-13us */
        .pi = 0x0d,      /*!< *4 >14.9ns */
    };
    uint32_t xclk = 0;

    xclk = bflb_clk_get_system_clock(BFLB_SYSTEM_XCLK);
    if (xclk != 0) {
        xclk = xclk / 1000000;
        para.pp = 12 * xclk / 4;
    }

    return bflb_ef_ctrl_set_para(&para);
#else
    return 0;
#endif
#endif
}

/****************************************************************************/ /**
 * @brief  Switch efuse region 0 control to AHB clock
 *
 * @param dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
static void ATTR_TCM_SECTION bflb_ef_ctrl_switch_ahb_clk_r0(struct bflb_device_s *dev)
{
    uint32_t reg_val;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    while (bflb_ef_ctrl_busy(dev) == 1) {
        timeout--;
        if (timeout == 0) {
            break;
        }
    }

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
#if defined(BL702) || defined(BL602) || defined(BL702L)
              (EF_CTRL_SAHB_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
#endif
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    /* Add delay for CLK to be stable */
    arch_delay_us(4);
}

/****************************************************************************/ /**
 * @brief  Switch efuse region 1 control to AHB clock
 *
 * @param dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
#ifdef EF_CTRL_EFUSE_R1_SIZE
static void ATTR_TCM_SECTION bflb_ef_ctrl_switch_ahb_clk_r1(struct bflb_device_s *dev)
{
    uint32_t reg_val;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    while (bflb_ef_ctrl_busy(dev) == 1) {
        timeout--;
        if (timeout == 0) {
            break;
        }
    }

    /* Note:ef_if_ctrl_1 has no EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS bit as ef_if_ctrl_0,
	   so we select it(them) in ef_if_ctrl_0 */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_1_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_1_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_1_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_1_RW_POS) |
              (0 << EF_CTRL_EF_IF_1_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);

    /* Add delay for CLK to be stable */
    arch_delay_us(4);
}
#endif

/****************************************************************************/ /**
 * @brief  Program efuse region 0
 *
 * @param dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
__UNUSED static void ATTR_TCM_SECTION bflb_ef_ctrl_program_efuse_r0(struct bflb_device_s *dev)
{
    uint32_t reg_val;

    /* Select auto mode and select ef clock */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
#if defined(BL702) || defined(BL602) || defined(BL702L)
              (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
#endif
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    /* Program */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
#if defined(BL702) || defined(BL602) || defined(BL702L)
              (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
#endif
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (1 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (1 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    /* Add delay for POR to be stable */
    arch_delay_us(4);

    /* Trigger */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
#if defined(BL702) || defined(BL602) || defined(BL702L)
              (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
#endif
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (1 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (1 << EF_CTRL_EF_IF_0_RW_POS) |
              (1 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
}

/****************************************************************************/ /**
 * @brief  Program efuse region 1
 *
 * @param dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
#ifdef EF_CTRL_EFUSE_R1_SIZE
static void ATTR_TCM_SECTION bflb_ef_ctrl_program_efuse_r1(struct bflb_device_s *dev)
{
    uint32_t reg_val;

    /* Select auto mode and select ef clock */
    /* Note:ef_if_ctrl_1 has no EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS bit as ef_if_ctrl_0,
	   so we select it(them) in ef_if_ctrl_0 */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_1_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_1_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_1_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_1_RW_POS) |
              (0 << EF_CTRL_EF_IF_1_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);

    /* Program */
    /* Note:ef_if_ctrl_1 has no EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS bit as ef_if_ctrl_0,
	so we select it(them) in ef_if_ctrl_0 */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (1 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    /* Add delay for POR to be stable */
    arch_delay_us(4);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_1_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_1_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_1_INT_CLR_POS) |
              (1 << EF_CTRL_EF_IF_1_RW_POS) |
              (0 << EF_CTRL_EF_IF_1_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_1_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_1_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_1_INT_CLR_POS) |
              (1 << EF_CTRL_EF_IF_1_RW_POS) |
              (1 << EF_CTRL_EF_IF_1_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Clear efuse region 0 data register
 *
 * @param  dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
static void ATTR_TCM_SECTION bflb_ef_ctrl_clear_data_reg0(struct bflb_device_s *dev)
{
    uint32_t *pefuse_start = (uint32_t *)(BFLB_EF_CTRL_BASE + 0x00);
    uint32_t i = 0;

    /* Switch to AHB clock */
    bflb_ef_ctrl_switch_ahb_clk_r0(dev);

    /* Clear data */
    for (i = 0; i < EF_CTRL_EFUSE_R0_SIZE / 4; i++) {
        pefuse_start[i] = 0;
    }
}

/****************************************************************************/ /**
 * @brief  Clear efuse region 1 data register
 *
 * @param  dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
#ifdef EF_CTRL_EFUSE_R1_SIZE
static void ATTR_TCM_SECTION bflb_ef_ctrl_clear_data_reg1(struct bflb_device_s *dev)
{
    uint32_t *pefuse_start = (uint32_t *)(BFLB_EF_CTRL_BASE + EF_CTRL_EFUSE_R0_SIZE);
    uint32_t i = 0;

    /* Switch to AHB clock */
    bflb_ef_ctrl_switch_ahb_clk_r1(dev);

    /* Clear data */
    for (i = 0; i < EF_CTRL_EFUSE_R1_SIZE / 4; i++) {
        pefuse_start[i] = 0;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Load efuse region 0
 *
 * @param dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
__UNUSED static void ATTR_TCM_SECTION bflb_ef_ctrl_load_efuse_r0(struct bflb_device_s *dev)
{
    uint32_t reg_val;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    EF_CTRL_DATA0_CLEAR;

    /* Trigger read */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
#if defined(BL702) || defined(BL602) || defined(BL702L)
              (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
#endif
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
#if defined(BL702) || defined(BL602) || defined(BL702L)
              (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
#endif
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (1 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    arch_delay_us(10);

    /* Wait for efuse control idle */
    do {
        reg_val = getreg32(BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
        timeout--;

        if (timeout == 0) {
            break;
        }
    } while ((reg_val & EF_CTRL_EF_IF_0_BUSY_MASK) || (!(reg_val & EF_CTRL_EF_IF_0_AUTOLOAD_DONE_MASK)));

    /* Switch to AHB clock */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
#if defined(BL702) || defined(BL602) || defined(BL702L)
              (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
#endif
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
}

/****************************************************************************/ /**
 * @brief  Load efuse region 0
 *
 * @param dev  ef control device pointer
 *
 * @return None
 *
*******************************************************************************/
#ifdef EF_CTRL_EFUSE_R1_SIZE
static void ATTR_TCM_SECTION bflb_ef_ctrl_load_efuse_r1(struct bflb_device_s *dev)
{
    uint32_t reg_val;

    EF_CTRL_DATA1_CLEAR;

    /* Trigger read */
    /* Note:ef_if_ctrl_1 has no EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS bit as ef_if_ctrl_0,
	so we select it(them) in ef_if_ctrl_0 */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_1_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_1_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_1_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_1_RW_POS) |
              (0 << EF_CTRL_EF_IF_1_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_1_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_1_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_1_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_1_RW_POS) |
              (1 << EF_CTRL_EF_IF_1_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);

    arch_delay_us(10);

    /* Wait for efuse control idle */
    do {
        reg_val = getreg32(BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);
    } while (reg_val & EF_CTRL_EF_IF_1_BUSY_MASK);

    do {
        reg_val = getreg32(BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
    } while (!(reg_val & EF_CTRL_EF_IF_0_AUTOLOAD_DONE_MASK));

    /* Switch to AHB clock since often read efuse data after load */
    /* Note:ef_if_ctrl_1 has no EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS bit as ef_if_ctrl_0,
	   so we select it(them) in ef_if_ctrl_0 */
    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
              (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
              (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_0_RW_POS) |
              (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    reg_val = (EF_CTRL_EFUSE_CTRL_PROTECT) | (EF_CTRL_EFUSE_CYCLE_PROTECT) |
              (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_1_MANUAL_EN_POS) |
              (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_1_CYC_MODIFY_POS) |
              (1 << EF_CTRL_EF_IF_1_INT_CLR_POS) |
              (0 << EF_CTRL_EF_IF_1_RW_POS) |
              (0 << EF_CTRL_EF_IF_1_TRIG_POS);

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_1_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  Check efuse auto load done
 *
 * @param dev  ef control device pointer
 *
 * @return 1 for auto load done 0 for not
 *
*******************************************************************************/
int ATTR_TCM_SECTION bflb_ef_ctrl_autoload_done(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_ef_ctrl_autoload_done
    return romapi_bflb_ef_ctrl_autoload_done(dev);
#else
    uint32_t reg_val;

    // if (dev == NULL) {
    //     dev = bflb_device_get_by_name("ef_ctrl");
    // }

    /* Switch to AHB clock */
    bflb_ef_ctrl_switch_ahb_clk_r0(dev);

    reg_val = getreg32(BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    if (reg_val & EF_CTRL_EF_IF_0_AUTOLOAD_DONE_MASK) {
        return 1;
    }

    return 0;
#endif
}

/****************************************************************************/ /**
 * @brief  Check efuse auto load done
 *
 * @param para  parameter for efuse program or read
 *
 * @return 1 for auto load done 0 for not
 *
*******************************************************************************/
int ATTR_TCM_SECTION bflb_ef_ctrl_set_para(bflb_ef_ctrl_para_t *para)
{
#ifdef romapi_bflb_ef_ctrl_set_para
    return romapi_bflb_ef_ctrl_set_para(para);
#else
    uint32_t reg_val;

    /* Switch to AHB clock */
    bflb_ef_ctrl_switch_ahb_clk_r0(NULL);

    reg_val = ((para->rd_dmy << 0) |
               (para->rd_dat << 6) |
               (para->rd_adr << 12) |
               (para->cs << 18) |
               (para->pd_cs_s << 24));

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CYC_0_OFFSET);

    reg_val = ((para->pi << 0) |
               (para->pp << 6) |
               (para->wr_adr << 14) |
               (para->ps_cs << 20) |
               (para->pd_cs_h << 26));

    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CYC_1_OFFSET);

    reg_val = getreg32(BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);
    reg_val |= EF_CTRL_EFUSE_CYCLE_PROTECT;
    reg_val |= (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS);
    putreg32(reg_val, BFLB_EF_CTRL_BASE + EF_CTRL_EF_IF_CTRL_0_OFFSET);

    return 0;
#endif
}

/****************************************************************************/ /**
 * @brief  write data to efuse
 *
 * @param dev  ef control device pointer
 * @param  offset: offset of efuse address to write
 * @param  pword: data pointer to buffer which is aligned to word
 * @param  count: count of data in words to write
 * @param  program: 1 for program to efuse,0 for not program now
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_ef_ctrl_write_direct(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t program)
{
#ifdef romapi_bflb_ef_ctrl_write_direct
    romapi_bflb_ef_ctrl_write_direct(dev, offset, pword, count, program);
#else
    uint32_t *pefuse_start = NULL;
    uint32_t region0_count = 0, region1_count = 0;
    uint32_t total_size = EF_CTRL_EFUSE_R0_SIZE;
    uintptr_t irq_stat;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

#ifdef EF_CTRL_EFUSE_R1_SIZE
    total_size += EF_CTRL_EFUSE_R1_SIZE;
#else
    (void)region1_count;
#endif

    bflb_ef_ctrl_update_para(dev);

    if (offset > total_size || (offset + count * 4) > total_size || pword == NULL) {
        if (program) {
            irq_stat = bflb_irq_save();
#if defined(BL702) || defined(BL602) || defined(BL702L)
            bflb_efuse_switch_cpu_clock_save();
#endif
            /* Switch to AHB clock */
            bflb_ef_ctrl_switch_ahb_clk_r0(dev);
            bflb_power_on_efuse();
            bflb_ef_ctrl_program_efuse_r0(dev);
            while (bflb_ef_ctrl_busy(dev) == 1) {
                timeout--;
                if (timeout == 0) {
                    break;
                }
                arch_delay_us(10);
            }
            bflb_power_off_efuse();
            arch_delay_us(100);
#ifdef EF_CTRL_EFUSE_R1_SIZE
            /* Switch to AHB clock */
            bflb_ef_ctrl_switch_ahb_clk_r1(dev);
            /* Add delay for CLK to be stable */
            arch_delay_us(4);
            bflb_ef_ctrl_program_efuse_r1(dev);
            arch_delay_us(100);
#endif
#if defined(BL702) || defined(BL602) || defined(BL702L)
            bflb_efuse_switch_cpu_clock_restore();
#endif
            bflb_irq_restore(irq_stat);
        }
        return;
    }

    if (offset < EF_CTRL_EFUSE_R0_SIZE) {
        if (offset + count * 4 <= EF_CTRL_EFUSE_R0_SIZE) {
            region0_count = count;
        } else {
            region0_count = (EF_CTRL_EFUSE_R0_SIZE - offset) / 4;
            region1_count = (offset + count * 4 - EF_CTRL_EFUSE_R0_SIZE) / 4;
        }
    } else {
        region1_count = count;
    }

    // if (dev == NULL) {
    //     dev = bflb_device_get_by_name("ef_ctrl");
    // }
    pefuse_start = (uint32_t *)(uintptr_t)(BFLB_EF_CTRL_BASE + offset);

    irq_stat = bflb_irq_save();
#if defined(BL702) || defined(BL602) || defined(BL702L)
    bflb_efuse_switch_cpu_clock_save();
#endif
    if (region0_count > 0) {
        /* Switch to AHB clock */
        bflb_ef_ctrl_switch_ahb_clk_r0(dev);

        arch_memcpy4(pefuse_start, pword, region0_count);
        pefuse_start += region0_count;
        pword += region0_count;

        if (program) {
            bflb_power_on_efuse();
            bflb_ef_ctrl_program_efuse_r0(dev);
            while (bflb_ef_ctrl_busy(dev) == 1) {
                timeout--;
                if (timeout == 0) {
                    break;
                }
                arch_delay_us(10);
            }
            bflb_power_off_efuse();
            arch_delay_us(100);
        }
    }
#ifdef EF_CTRL_EFUSE_R1_SIZE
    if (region1_count > 0) {
        /* Switch to AHB clock */
        bflb_ef_ctrl_switch_ahb_clk_r1(dev);

        /* Add delay for CLK to be stable */
        arch_delay_us(4);

        arch_memcpy4(pefuse_start, pword, region1_count);

        if (program) {
            bflb_ef_ctrl_program_efuse_r1(dev);
            arch_delay_us(100);
        }
    }
#endif
#if defined(BL702) || defined(BL602) || defined(BL702L)
    bflb_efuse_switch_cpu_clock_restore();
#endif

    bflb_irq_restore(irq_stat);
#endif
}

/****************************************************************************/ /**
 * @brief  Read data from efuse
 *
 * @param dev  ef control device pointer
 * @param  offset: offset of efuse address to read
 * @param  pword: data pointer to buffer which is aligned to word
 * @param  count: count of data in words to read
 * @param  reload: 1 for reload before read,0 for not
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_ef_ctrl_read_direct(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t reload)
{
#ifdef romapi_bflb_ef_ctrl_read_direct
    romapi_bflb_ef_ctrl_read_direct(dev, offset, pword, count, reload);
#else
    uint32_t *pefuse_start = NULL;
    uint32_t region0_count = 0, region1_count = 0;
    uint32_t total_size = EF_CTRL_EFUSE_R0_SIZE;
    uintptr_t irq_stat;

#ifdef EF_CTRL_EFUSE_R1_SIZE
    total_size += EF_CTRL_EFUSE_R1_SIZE;
#else
    (void)region1_count;
#endif

    bflb_ef_ctrl_update_para(dev);

    if (offset > total_size || (offset + count * 4) > total_size || pword == NULL) {
        if (reload) {
            irq_stat = bflb_irq_save();
#if defined(BL702) || defined(BL602) || defined(BL702L)
            bflb_efuse_switch_cpu_clock_save();
#endif
            bflb_ef_ctrl_load_efuse_r0(dev);
#ifdef EF_CTRL_EFUSE_R1_SIZE
            bflb_ef_ctrl_load_efuse_r1(dev);
#endif
#if defined(BL702) || defined(BL602) || defined(BL702L)
            bflb_efuse_switch_cpu_clock_restore();
#endif
            bflb_irq_restore(irq_stat);
        }
        return;
    }

    if (offset < EF_CTRL_EFUSE_R0_SIZE) {
        if (offset + count * 4 <= EF_CTRL_EFUSE_R0_SIZE) {
            region0_count = count;
        } else {
            region0_count = (EF_CTRL_EFUSE_R0_SIZE - offset) / 4;
            region1_count = (offset + count * 4 - EF_CTRL_EFUSE_R0_SIZE) / 4;
        }
    } else {
        region1_count = count;
    }

    // if (dev == NULL) {
    //     dev = bflb_device_get_by_name("ef_ctrl");
    // }
    pefuse_start = (uint32_t *)(uintptr_t)(BFLB_EF_CTRL_BASE + offset);

    irq_stat = bflb_irq_save();
#if defined(BL702) || defined(BL602) || defined(BL702L)
    bflb_efuse_switch_cpu_clock_save();
#endif
    if (region0_count > 0) {
        if (reload) {
            bflb_ef_ctrl_load_efuse_r0(dev);
        } else {
            bflb_ef_ctrl_switch_ahb_clk_r0(dev);
        }
        arch_memcpy4(pword, pefuse_start, region0_count);
        pword += region0_count;
        pefuse_start += region0_count;
    }
#ifdef EF_CTRL_EFUSE_R1_SIZE
    if (region1_count > 0) {
        if (reload) {
            bflb_ef_ctrl_load_efuse_r1(dev);
        } else {
            bflb_ef_ctrl_switch_ahb_clk_r1(dev);
        }
        arch_memcpy4(pword, pefuse_start, region0_count);
    }
#endif

#if defined(BL702) || defined(BL602) || defined(BL702L)
    bflb_efuse_switch_cpu_clock_restore();
#endif

    bflb_irq_restore(irq_stat);
#endif
}

/****************************************************************************/ /**
 * @brief  Efuse read common trim value
 *
 * @param dev  ef control device pointer
 * @param  name: Trim name
 * @param  trim: Trim data pointer
 * @param  reload: Reload efuse data before read
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_ef_ctrl_read_common_trim(struct bflb_device_s *dev, char *name, bflb_ef_ctrl_com_trim_t *trim, uint8_t reload)
{
#ifdef romapi_bflb_ef_ctrl_read_common_trim
    romapi_bflb_ef_ctrl_read_common_trim(dev, name, trim, reload);
#else
    uint32_t reg_val;
    uint32_t i = 0;
    const bflb_ef_ctrl_com_trim_cfg_t *trim_list = NULL;
    uint32_t trim_list_len;
    uintptr_t irq_stat;

    // if (dev == NULL) {
    //     dev = bflb_device_get_by_name("ef_ctrl");
    // }

    irq_stat = bflb_irq_save();

    bflb_ef_ctrl_update_para(dev);

#if defined(BL702) || defined(BL602) || defined(BL702L)
    bflb_efuse_switch_cpu_clock_save();
#endif
    if (reload) {
        /* Trigger read data from efuse */
        bflb_ef_ctrl_load_efuse_r0(dev);
#ifdef EF_CTRL_EFUSE_R1_SIZE
        bflb_ef_ctrl_load_efuse_r1(dev);
#endif
    }

    trim->en = 0;
    trim->parity = 0;
    trim->empty = 1;
    trim->len = 0;

    trim_list_len = bflb_ef_ctrl_get_common_trim_list(&trim_list);

    for (i = 0; i < trim_list_len; i++) {
        if (arch_memcmp(name, trim_list[i].name, bflb_ef_ctrl_strlen(name)) == 0) {
            /* switch clock */
            if (trim_list[i].en_addr < EF_CTRL_EFUSE_R0_SIZE * 8) {
                /* Switch to AHB clock */
                bflb_ef_ctrl_switch_ahb_clk_r0(dev);
            }
#ifdef EF_CTRL_EFUSE_R1_SIZE
            if (trim_list[i].en_addr >= EF_CTRL_EFUSE_R0_SIZE * 8) {
                /* Switch to AHB clock */
                bflb_ef_ctrl_switch_ahb_clk_r1(dev);
            }
#endif
            trim->len = trim_list[i].value_len;
            reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].en_addr / 32) * 4);
            if (reg_val & (1 << (trim_list[i].en_addr % 32))) {
                trim->en = 1;
            }
            reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].parity_addr / 32) * 4);
            if (reg_val & (1 << (trim_list[i].parity_addr % 32))) {
                trim->parity = 1;
            }

            if (((trim_list[i].value_addr % 32) + trim_list[i].value_len) > 32) {
                uint64_t tmpval64 = (uint64_t)getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4);
                tmpval64 |= (((uint64_t)getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4 + 4)) << 32);
                tmpval64 = tmpval64 >> (trim_list[i].value_addr % 32);
                trim->value = (uint32_t)(tmpval64 & (((uint64_t)1 << trim_list[i].value_len) - 1));
            } else {
                reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4);
                reg_val = reg_val >> (trim_list[i].value_addr % 32);
                trim->value = reg_val & ((1 << trim_list[i].value_len) - 1);
            }

            if (trim->en == 0 && trim->parity == 0 && trim->value == 0) {
                trim->empty = 1;
            } else {
                trim->empty = 0;
            }

            break;
        }
    }
#if defined(BL702) || defined(BL602) || defined(BL702L)
    bflb_efuse_switch_cpu_clock_restore();
#endif
    bflb_irq_restore(irq_stat);
#endif
}

/****************************************************************************/ /**
 * @brief  Efuse write common trim value
 *
 * @param dev  ef control device pointer
 * @param  name: Trim name
 * @param  trim: Trim data pointer
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_ef_ctrl_write_common_trim(struct bflb_device_s *dev, char *name, uint32_t value, uint8_t program)
{
#ifdef romapi_bflb_ef_ctrl_write_common_trim
    romapi_bflb_ef_ctrl_write_common_trim(dev, name, value, program);
#else
    uint32_t reg_val;
    uint32_t i = 0;
    uint8_t parity = 0;
    const bflb_ef_ctrl_com_trim_cfg_t *trim_list = NULL;
    uint32_t trim_list_len;
    uintptr_t irq_stat;    
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    // if (dev == NULL) {
    //     dev = bflb_device_get_by_name("ef_ctrl");
    // }

    trim_list_len = bflb_ef_ctrl_get_common_trim_list(&trim_list);

    irq_stat = bflb_irq_save();

    bflb_ef_ctrl_update_para(dev);

    for (i = 0; i < trim_list_len; i++) {
        if (arch_memcmp(name, trim_list[i].name, bflb_ef_ctrl_strlen(name)) == 0) {
#if defined(BL702) || defined(BL602) || defined(BL702L)
            bflb_efuse_switch_cpu_clock_save();
#endif
            /* switch clock */
            if (trim_list[i].en_addr < EF_CTRL_EFUSE_R0_SIZE * 8) {
                /* Switch to AHB clock */
                bflb_ef_ctrl_switch_ahb_clk_r0(dev);
            }
#ifdef EF_CTRL_EFUSE_R1_SIZE
            if (trim_list[i].en_addr >= EF_CTRL_EFUSE_R0_SIZE * 8) {
                /* Switch to AHB clock */
                bflb_ef_ctrl_switch_ahb_clk_r1(dev);
            }
#endif
            reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].en_addr / 32) * 4);
            reg_val |= (1 << (trim_list[i].en_addr % 32));
            putreg32(reg_val, BFLB_EF_CTRL_BASE + (trim_list[i].en_addr / 32) * 4);

            parity = bflb_ef_ctrl_get_trim_parity(value, trim_list[i].value_len);
            if (parity) {
                reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].parity_addr / 32) * 4);
                reg_val |= (1 << (trim_list[i].parity_addr % 32));
                putreg32(reg_val, BFLB_EF_CTRL_BASE + (trim_list[i].parity_addr / 32) * 4);
            }

            if (((trim_list[i].value_addr % 32) + trim_list[i].value_len) > 32) {
                reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4);
                reg_val |= (value << (trim_list[i].value_addr % 32));
                putreg32(reg_val, BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4);

                reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4) + 4;
                reg_val |= (value >> (32 - (trim_list[i].value_addr % 32)));
                putreg32(reg_val, BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4 + 4);
            } else {
                reg_val = getreg32(BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4);
                reg_val |= (value << (trim_list[i].value_addr % 32));
                putreg32(reg_val, BFLB_EF_CTRL_BASE + (trim_list[i].value_addr / 32) * 4);
            }

            if (program) {
                /* program */
                if (trim_list[i].en_addr < EF_CTRL_EFUSE_R0_SIZE * 8) {
                    bflb_power_on_efuse();
                    bflb_ef_ctrl_program_efuse_r0(dev);
                    while (bflb_ef_ctrl_busy(dev) == 1) {
                        timeout--;
                        if (timeout == 0) {
                            break;
                        }
                        arch_delay_us(10);
                    }
                    bflb_power_off_efuse();
                    arch_delay_us(100);
                }
#ifdef EF_CTRL_EFUSE_R1_SIZE
                if (trim_list[i].en_addr >= EF_CTRL_EFUSE_R0_SIZE * 8) {
                    bflb_ef_ctrl_program_efuse_r1(dev);
                    arch_delay_us(100);
                }
#endif
            }
#if defined(BL702) || defined(BL602) || defined(BL702L)
            bflb_efuse_switch_cpu_clock_restore();
#endif
            break;
        }
    }
    bflb_irq_restore(irq_stat);
#endif
}

/****************************************************************************/ /**
 * @brief  Whether a value bits is all zero
 *
 * @param  val: value to check
 * @param  start: start bit
 * @param  len: total length of bits to check
 *
 * @return 1 for all bits zero 0 for others
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION bflb_ef_ctrl_is_all_bits_zero(uint32_t val, uint8_t start, uint8_t len)
{
#ifdef romapi_bflb_ef_ctrl_is_all_bits_zero
    return romapi_bflb_ef_ctrl_is_all_bits_zero(val, start, len);
#else
    uint32_t mask = 0;

    val = (val >> start);

    if (len >= 32) {
        mask = 0xffffffff;
    } else {
        mask = (1 << len) - 1;
    }

    if ((val & mask) == 0) {
        return 1;
    } else {
        return 0;
    }
#endif
}

/****************************************************************************/ /**
 * @brief  Efuse Ctrl get zero bit count
 *
 * @param  val: Value to count
 *
 * @return Zero bit count
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION bflb_ef_ctrl_get_byte_zero_cnt(uint8_t val)
{
#ifdef romapi_bflb_ef_ctrl_get_byte_zero_cnt
    return romapi_bflb_ef_ctrl_get_byte_zero_cnt(val);
#else
    uint32_t cnt = 0;
    uint32_t i = 0;

    for (i = 0; i < 8; i++) {
        if ((val & (1 << i)) == 0) {
            cnt += 1;
        }
    }

    return cnt;
#endif
}

/****************************************************************************/ /**
 * @brief  Analog Trim parity calculate
 *
 * @param  val: Value of efuse trim data
 * @param  len: Length of bit to calculate
 *
 * @return Parity bit value
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION bflb_ef_ctrl_get_trim_parity(uint32_t val, uint8_t len)
{
#ifdef romapi_bflb_ef_ctrl_get_trim_parity
    return romapi_bflb_ef_ctrl_get_trim_parity(val, len);
#else
    uint8_t cnt = 0;
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        if (val & (1 << i)) {
            cnt++;
        }
    }

    return cnt & 0x01;
#endif
}
