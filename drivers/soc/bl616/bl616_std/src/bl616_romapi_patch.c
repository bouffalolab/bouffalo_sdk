/**
  ******************************************************************************
  * @file    bl808_romapi_patch.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 Bouffalo Lab</center></h2>
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

#include "bl616_romapi_patch.h"
#include "bl616_romdriver_e907.h"
#include "soft_crc.h"

/* WiFi PLL Config*/
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_32M_38P4M_40M = {
    .clkpllRefdivRatio = 2,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 0,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 0,        /*!< pll_icp_1u */
    .clkpllIcp5u = 2,        /*!< pll_icp_5u */
    .clkpllRz = 3,           /*!< pll_rz */
    .clkpllCz = 1,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 1,      /*!< pll_r4_short */
    .clkpllC4En = 0,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 1,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 1,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_24M = {
    .clkpllRefdivRatio = 1,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 0,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 0,        /*!< pll_icp_1u */
    .clkpllIcp5u = 2,        /*!< pll_icp_5u */
    .clkpllRz = 3,           /*!< pll_rz */
    .clkpllCz = 1,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 1,      /*!< pll_r4_short */
    .clkpllC4En = 0,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 1,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 1,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_26M = {
    .clkpllRefdivRatio = 1,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 1,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 1,        /*!< pll_icp_1u */
    .clkpllIcp5u = 0,        /*!< pll_icp_5u */
    .clkpllRz = 5,           /*!< pll_rz */
    .clkpllCz = 2,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 0,      /*!< pll_r4_short */
    .clkpllC4En = 1,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 0,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 0,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION wifiPllCfg_960M[GLB_XTAL_MAX] = {
    { NULL, 0x0, 0 },                                 /*!< XTAL is None */
    { &wifiPllBasicCfg_24M, 0x1400000, 0 },           /*!< XTAL is 24M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1E00000, 0 }, /*!< XTAL is 32M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1900000, 0 }, /*!< XTAL is 38.4M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1800000, 0 }, /*!< XTAL is 40M */
    { &wifiPllBasicCfg_26M, 0x1276276, 0 },           /*!< XTAL is 26M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1E00000, 0 }, /*!< XTAL is RC32M */
};

/* Audio PLL Config*/
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION audioPllBasicCfg_24M_26M = {
    .clkpllRefdivRatio = 2,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 1,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 1,        /*!< pll_icp_1u */
    .clkpllIcp5u = 0,        /*!< pll_icp_5u */
    .clkpllRz = 5,           /*!< pll_rz */
    .clkpllCz = 2,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 0,      /*!< pll_r4_short */
    .clkpllC4En = 1,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 3,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 0,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 0,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION audioPllBasicCfg_32M_38P4M_40M = {
    .clkpllRefdivRatio = 4,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 1,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 1,        /*!< pll_icp_1u */
    .clkpllIcp5u = 0,        /*!< pll_icp_5u */
    .clkpllRz = 5,           /*!< pll_rz */
    .clkpllCz = 2,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 0,      /*!< pll_r4_short */
    .clkpllC4En = 1,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 3,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 0,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 0,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION audioPllCfg_491P52M[GLB_XTAL_MAX] = {
    { NULL, 0x0, 20 },                                /*!< XTAL is None */
    { &audioPllBasicCfg_24M_26M, 0x147AE, 20 },       /*!< XTAL is 24M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x1EB85, 20 }, /*!< XTAL is 32M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x19999, 20 }, /*!< XTAL is 38.4M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x18937, 20 }, /*!< XTAL is 40M */
    { &audioPllBasicCfg_24M_26M, 0x12E79, 20 },       /*!< XTAL is 26M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x1EB85, 20 }, /*!< XTAL is RC32M */
};
const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION audioPllCfg_451P58M[GLB_XTAL_MAX] = {
    { NULL, 0x0, 20 },                                /*!< XTAL is None */
    { &audioPllBasicCfg_24M_26M, 0x12D0D, 20 },       /*!< XTAL is 24M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x1C394, 20 }, /*!< XTAL is 32M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x17851, 20 }, /*!< XTAL is 38.4M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x16943, 20 }, /*!< XTAL is 40M */
    { &audioPllBasicCfg_24M_26M, 0x115E5, 20 },       /*!< XTAL is 26M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x1C394, 20 }, /*!< XTAL is RC32M */
};
const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION audioPllCfg_400M[GLB_XTAL_MAX] = {
    { NULL, 0x0, 8 },                                /*!< XTAL is None */
    { &audioPllBasicCfg_24M_26M, 0x10AAA, 8 },       /*!< XTAL is 24M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x19000, 8 }, /*!< XTAL is 32M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x14D55, 8 }, /*!< XTAL is 38.4M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x14000, 8 }, /*!< XTAL is 40M */
    { &audioPllBasicCfg_24M_26M, 0xF627, 8 },        /*!< XTAL is 26M */
    { &audioPllBasicCfg_32M_38P4M_40M, 0x19000, 8 }, /*!< XTAL is RC32M */
};

/* Audio PLL Config*/
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION audioPll384BasicCfg_24M_26M = {
    .clkpllRefdivRatio = 2,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 1,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 1,        /*!< pll_icp_1u */
    .clkpllIcp5u = 0,        /*!< pll_icp_5u */
    .clkpllRz = 5,           /*!< pll_rz */
    .clkpllCz = 2,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 0,      /*!< pll_r4_short */
    .clkpllC4En = 1,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 2,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 0,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 0,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION audioPll384BasicCfg_32M_38P4M_40M = {
    .clkpllRefdivRatio = 4,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 1,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 1,        /*!< pll_icp_1u */
    .clkpllIcp5u = 0,        /*!< pll_icp_5u */
    .clkpllRz = 5,           /*!< pll_rz */
    .clkpllCz = 2,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 0,      /*!< pll_r4_short */
    .clkpllC4En = 1,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 2,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 0,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 0,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION audioPllCfg_384M[GLB_XTAL_MAX] = {
    { NULL, 0x0, 8 },                                   /*!< XTAL is None */
    { &audioPll384BasicCfg_24M_26M, 0x10000, 8 },       /*!< XTAL is 24M */
    { &audioPll384BasicCfg_32M_38P4M_40M, 0x18000, 8 }, /*!< XTAL is 32M */
    { &audioPll384BasicCfg_32M_38P4M_40M, 0x14000, 8 }, /*!< XTAL is 38.4M */
    { &audioPll384BasicCfg_32M_38P4M_40M, 0x13333, 8 }, /*!< XTAL is 40M */
    { &audioPll384BasicCfg_24M_26M, 0xEC4E, 8 },        /*!< XTAL is 26M */
    { &audioPll384BasicCfg_32M_38P4M_40M, 0x18000, 8 }, /*!< XTAL is RC32M */
};

const GLB_SLAVE_GRP_0_TBL_Type ATTR_CLOCK_CONST_SECTION glb_slave_grp_0_table[GLB_SLAVE_GRP_0_MAX] = {
    { GLB_ADC_CFG0_OFFSET, GLB_GPADC_32M_DIV_EN_POS, GLB_GPADC_32M_CLK_SEL_POS, GLB_GPADC_32M_CLK_DIV_POS, GLB_GPADC_32M_DIV_EN_LEN, GLB_GPADC_32M_CLK_SEL_LEN, GLB_GPADC_32M_CLK_DIV_LEN },
    { GLB_IR_CFG0_OFFSET, GLB_IR_CLK_EN_POS, 0, GLB_IR_CLK_DIV_POS, GLB_IR_CLK_EN_LEN, 0, GLB_IR_CLK_DIV_LEN },
    { GLB_I2C_CFG0_OFFSET, GLB_I2C_CLK_EN_POS, GLB_I2C_CLK_SEL_POS, GLB_I2C_CLK_DIV_POS, GLB_I2C_CLK_EN_LEN, GLB_I2C_CLK_SEL_LEN, GLB_I2C_CLK_DIV_LEN },
    { GLB_SPI_CFG0_OFFSET, GLB_SPI_CLK_EN_POS, GLB_SPI_CLK_SEL_POS, GLB_SPI_CLK_DIV_POS, GLB_SPI_CLK_EN_LEN, GLB_SPI_CLK_SEL_LEN, GLB_SPI_CLK_DIV_LEN },
    { GLB_PEC_CFG0_OFFSET, GLB_PEC_CLK_EN_POS, GLB_PEC_CLK_SEL_POS, GLB_PEC_CLK_DIV_POS, GLB_PEC_CLK_EN_LEN, GLB_PEC_CLK_SEL_LEN, GLB_PEC_CLK_DIV_LEN },
    { GLB_DBI_CFG0_OFFSET, GLB_DBI_CLK_EN_POS, GLB_DBI_CLK_SEL_POS, GLB_DBI_CLK_DIV_POS, GLB_DBI_CLK_EN_LEN, GLB_DBI_CLK_SEL_LEN, GLB_DBI_CLK_DIV_LEN },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_AUTO_DIV_EN_POS, 0, 0, GLB_REG_AUDIO_AUTO_DIV_EN_LEN, 0, 0 },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_ADC_CLK_EN_POS, 0, GLB_REG_AUDIO_ADC_CLK_DIV_POS, GLB_REG_AUDIO_ADC_CLK_EN_LEN, 0, GLB_REG_AUDIO_ADC_CLK_DIV_LEN },
    { GLB_AUDIO_CFG1_OFFSET, GLB_REG_AUDIO_SOLO_CLK_EN_POS, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_POS, GLB_REG_AUDIO_SOLO_CLK_EN_LEN, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_LEN },
    { GLB_CAM_CFG0_OFFSET, GLB_REG_CAM_REF_CLK_EN_POS, GLB_REG_CAM_REF_CLK_SRC_SEL_POS, GLB_REG_CAM_REF_CLK_DIV_POS, GLB_REG_CAM_REF_CLK_EN_LEN, GLB_REG_CAM_REF_CLK_SRC_SEL_LEN, GLB_REG_CAM_REF_CLK_DIV_LEN },
    { GLB_SDH_CFG0_OFFSET, GLB_REG_SDH_CLK_EN_POS, GLB_REG_SDH_CLK_SEL_POS, GLB_REG_SDH_CLK_DIV_POS, GLB_REG_SDH_CLK_EN_LEN, GLB_REG_SDH_CLK_SEL_LEN, GLB_REG_SDH_CLK_DIV_LEN },
    { GLB_PSRAM_CFG0_OFFSET, GLB_REG_PSRAMB_CLK_EN_POS, GLB_REG_PSRAMB_CLK_SEL_POS, GLB_REG_PSRAMB_CLK_DIV_POS, GLB_REG_PSRAMB_CLK_EN_LEN, GLB_REG_PSRAMB_CLK_SEL_LEN, GLB_REG_PSRAMB_CLK_DIV_LEN },
};

typedef struct
{
    uint32_t jedecID;
    char *name;
    const SPI_Flash_Cfg_Type *cfg;
} Flash_Info_t;

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_GD_LQ64E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xa0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3d,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 500,
    .timeE32k = 2000,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_GD_Q64E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_16JV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02, /*Q08BV,Q16DV: 0x02.Q32FW,Q32FV: 0x01 */
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xa0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3d,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_64JV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 0,
    .cReadMode = 0xFF,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1600,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_256FV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xa0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = 0x24,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1600,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Mxic_25U256 = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc2,

    .deBurstWrapCmd = 0xC0,
    .deBurstWrapCmdDmyClk = 0x00,
    .deBurstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .deBurstWrapData = 0x10,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0,
    .qeBit = 0x06,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x15,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA5,

    .burstWrapCmd = 0xC0,
    .burstWrapCmdDmyClk = 0x00,
    .burstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .burstWrapData = 0x02,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x38,
    .qppAddrMode = SF_CTRL_ADDR_4_LINES,

    .ioMode = (SF_CTRL_QIO_MODE | 0x20),
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1000,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION Flash_Info_t flashInfos[] = {
    {
        .jedecID = 0x1740ef,
        //.name="WB_64JV_64_33",
        .cfg = &flashCfg_Winb_64JV,
    },
    {
        .jedecID = 0x1940ef,
        //.name="WB_256FV_256_33",
        .cfg = &flashCfg_Winb_256FV,
    },
    {
        .jedecID = 0x1760ef,
        //.name="WB_64JW_64_18",
        .cfg = &flashCfg_Winb_64JV,
    },
    {
        .jedecID = 0x1570ef,
        //.name="WB_16JV_16_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x1740c8,
        //.name="GD_64E_64_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x1760c8,
        //.name="GD_LQ64E_64_18",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x1860c8,
        //.name="GD_LQ128E_128_18",
        .cfg = &flashCfg_GD_LQ64E,
    },
    {
        .jedecID = 0x15345e,
        //.name="ZB_WQ16A_16_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x15405e,
        //.name="ZB_16B_16_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x16405e,
        //.name="ZB_32B_32_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x17405e,
        //.name="ZB_VQ64_64_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x15605e,
        //.name="ZB_VQ16_16_33",
        .cfg = &flashCfg_GD_Q64E,
    },
    {
        .jedecID = 0x3925c2,
        //.name="MX_U25643G_256_18",
        .cfg = &flashCfg_Mxic_25U256,
    },
    {
        .jedecID = 0x144020,
        //.name="XM_QH80_08_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x154020,
        //.name="XM_QH16_16_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x164020,
        //.name="XM_QH32_32_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x174020,
        //.name="XM_QH64_64_33",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x174220,
        //.name="XM_QW64_64_1833",
        .cfg = &flashCfg_Winb_16JV,
    },
    {
        .jedecID = 0x17400b,
        //.name="XT_F64B_64_33",
        .cfg = &flashCfg_GD_LQ64E,
    },
};

//CLOCK
//EFUSE
//PDS

//FLASH
/****************************************************************************/ /**
 * @brief  Erase flash one 32K block
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  blkNum: flash 32K block number
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    uint32_t cnt = 0;
    uint8_t is32BitsAddr = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    BL_Err_Type stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uintptr_t)&flashCmd) % 4 == 0) {
        ARCH_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        ARCH_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    is32BitsAddr = (flashCfg->ioMode & 0x20);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    if (is32BitsAddr > 0) {
        flashCmd.addrSize++;
        flashCmd.cmdBuf[0] = (flashCfg->blk32EraseCmd << 24) | ((BFLB_SPIFLASH_BLK32K_SIZE * blkNum) >> 8);
        flashCmd.cmdBuf[1] = ((BFLB_SPIFLASH_BLK32K_SIZE * blkNum) << 24);
    } else {
        flashCmd.cmdBuf[0] = (flashCfg->blk32EraseCmd << 24) | (BFLB_SPIFLASH_BLK32K_SIZE * blkNum);
    }

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        arch_delay_us(500);
        cnt++;

        if (cnt > flashCfg->timeE32k * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Erase flash one region
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  startaddr: start address to erase
 * @param  endaddr: end address(include this address) to erase
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr)
{
    uint32_t len = 0;
    uint32_t eraseLen = 0;
    BL_Err_Type ret = SUCCESS;

    if (startaddr > endaddr) {
        return ERROR;
    }

    while (startaddr <= endaddr) {
        len = endaddr - startaddr + 1;

        if (flashCfg->blk64EraseCmd != BFLB_SPIFLASH_CMD_INVALID &&
            (startaddr & (BFLB_SPIFLASH_BLK64K_SIZE - 1)) == 0 &&
            len > (BFLB_SPIFLASH_BLK64K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 64K margin address,and length > 64K-sector size, erase one first */
            ret = SFlash_Blk64_Erase(flashCfg, startaddr / BFLB_SPIFLASH_BLK64K_SIZE);
            eraseLen = BFLB_SPIFLASH_BLK64K_SIZE;
        } else if (flashCfg->blk32EraseCmd != BFLB_SPIFLASH_CMD_INVALID &&
                   (startaddr & (BFLB_SPIFLASH_BLK32K_SIZE - 1)) == 0 &&
                   len > (BFLB_SPIFLASH_BLK32K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 32K margin address,and length > 32K-sector size, erase one first */
            ret = SFlash_Blk32_Erase(flashCfg, startaddr / BFLB_SPIFLASH_BLK32K_SIZE);
            eraseLen = BFLB_SPIFLASH_BLK32K_SIZE;
        } else {
            /* Sector erase */
            startaddr = ((startaddr) & (~(flashCfg->sectorSize * 1024 - 1)));
            ret = SFlash_Sector_Erase(flashCfg, startaddr / flashCfg->sectorSize / 1024);
            eraseLen = flashCfg->sectorSize * 1024;
        }

        startaddr += eraseLen;

        if (ret != SUCCESS) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************//**
 * @brief  Clear flash status register
 *
 * @param  flashCfg: Flash configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t ret = 0;
    uint32_t qeValue = 0;
    uint32_t regValue = 0;
    uint32_t readValue = 0;
    uint8_t readRegValue0 = 0;
    uint8_t readRegValue1 = 0;

    if((flashCfg->ioMode&0xf)==SF_CTRL_QO_MODE || (flashCfg->ioMode&0xf)==SF_CTRL_QIO_MODE){
        qeValue = 1;
    }

    SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&readRegValue0, 1);
    SFlash_Read_Reg(flashCfg, 1, (uint8_t *)&readRegValue1, 1);
    readValue = (readRegValue0|(readRegValue1<<8));
    if ((readValue & (~((1<<(flashCfg->qeIndex*8+flashCfg->qeBit)) |
                        (1<<(flashCfg->busyIndex*8+flashCfg->busyBit)) |
                        (1<<(flashCfg->wrEnableIndex*8+flashCfg->wrEnableBit))))) == 0){
        return SUCCESS;
    }

    ret = SFlash_Write_Enable(flashCfg);
    if (SUCCESS != ret) {
        return ERROR;
    }
    if (flashCfg->qeWriteRegLen == 2) {
        regValue = (qeValue<<(flashCfg->qeIndex*8+flashCfg->qeBit));
        SFlash_Write_Reg(flashCfg, 0, (uint8_t *)&regValue, 2);
    } else {
        if (flashCfg->qeIndex == 0) {
            regValue = (qeValue<<flashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(flashCfg, 0, (uint8_t *)&regValue, 1);
        ret = SFlash_Write_Enable(flashCfg);
        if (SUCCESS != ret) {
            return ERROR;
        }
        if (flashCfg->qeIndex == 1) {
            regValue = (qeValue<<flashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(flashCfg, 1, (uint8_t *)&regValue, 1);
    }
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get flash config according to flash ID patch
 *
 * @param  flashID: Flash ID
 * @param  pFlashCfg: Flash config pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t i;
    uint8_t buf[sizeof(SPI_Flash_Cfg_Type) + 8];
    uint32_t crc, *pCrc;
    uint32_t xipOffset;
    char flashCfgMagic[] = "FCFG";

    if (flashID == 0) {
        xipOffset = SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0);
        SF_Ctrl_Set_Flash_Image_Offset(0, 0, SF_CTRL_FLASH_BANK0);
        XIP_SFlash_Read_Via_Cache_Need_Lock(8 + BL616_FLASH_XIP_BASE, buf, sizeof(SPI_Flash_Cfg_Type) + 8);
        SF_Ctrl_Set_Flash_Image_Offset(xipOffset, 0, SF_CTRL_FLASH_BANK0);

        if (ARCH_MemCmp(buf, flashCfgMagic, 4) == 0) {
            crc = BFLB_Soft_CRC32((uint8_t *)buf + 4, sizeof(SPI_Flash_Cfg_Type));
            pCrc = (uint32_t *)(buf + 4 + sizeof(SPI_Flash_Cfg_Type));

            if (*pCrc == crc) {
                ARCH_MemCpy_Fast(pFlashCfg, (uint8_t *)buf + 4, sizeof(SPI_Flash_Cfg_Type));
                return SUCCESS;
            }
        }
    } else {
        if (RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock(flashID, pFlashCfg) == SUCCESS) {
            return SUCCESS;
        }
        for (i = 0; i < sizeof(flashInfos) / sizeof(flashInfos[0]); i++) {
            if (flashInfos[i].jedecID == flashID) {
                ARCH_MemCpy_Fast(pFlashCfg, flashInfos[i].cfg, sizeof(SPI_Flash_Cfg_Type));
                return SUCCESS;
            }
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Identify one flash patch
 *
 * @param  callFromFlash: code run at flash or ram
 * @param  flashPinCfg: Bit 7: autoscan, Bit6-0: flash GPIO config
 * @param  restoreDefault: Wether restore default flash GPIO config
 * @param  pFlashCfg: Flash config pointer
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return Flash ID
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION SF_Cfg_Flash_Identify_Ext(uint8_t callFromFlash, uint8_t flashPinCfg,
                                                    uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, SF_Ctrl_Bank_Select bank)
{
    uint32_t jdecId = 0;
    uint32_t i = 0;
    uint32_t ret = 0;

    ret = SF_Cfg_Flash_Identify(callFromFlash, flashPinCfg, restoreDefault, pFlashCfg, group, bank);
    if (callFromFlash) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, pFlashCfg->ioMode & 0xf, 1, 0, 32, bank);
    }
    if ((ret & BFLB_FLASH_ID_VALID_FLAG) != 0) {
        return ret;
    }

    jdecId = (ret & 0xffffff);
    for (i = 0; i < sizeof(flashInfos) / sizeof(flashInfos[0]); i++) {
        if (flashInfos[i].jedecID == jdecId) {
            ARCH_MemCpy_Fast(pFlashCfg, flashInfos[i].cfg, sizeof(SPI_Flash_Cfg_Type));
            break;
        }
    }
    if (i == sizeof(flashInfos) / sizeof(flashInfos[0])) {
        return jdecId;
    } else {
        return (jdecId | BFLB_FLASH_ID_VALID_FLAG);
    }
}

/****************************************************************************/ /**
 * @brief  Read data from flash via XIP
 *
 * @param  addr: flash read start address
 * @param  data: data pointer to store data read from flash
 * @param  len: data length to read
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t offset;

    addr = addr & (BL616_FLASH_XIP_END - BL616_FLASH_XIP_BASE - 1);
    addr |= BL616_FLASH_XIP_BASE;

    offset = SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0);
    SF_Ctrl_Set_Flash_Image_Offset(0, 0, SF_CTRL_FLASH_BANK0);
    /* Flash read */
    ARCH_MemCpy_Fast(data, (void *)(uintptr_t)(addr - SF_Ctrl_Get_Flash_Image_Offset(0, SF_CTRL_FLASH_BANK0)), len);
    SF_Ctrl_Set_Flash_Image_Offset(offset, 0, SF_CTRL_FLASH_BANK0);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Save flash controller state
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  offset: CPU XIP flash offset pointer
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset,
                                                   uint8_t group, SF_Ctrl_Bank_Select bank)
{
    /* XIP_SFlash_Delay */
    volatile uint32_t i = 32 * 2;

    while (i--)
        ;

    if (bank == SF_CTRL_FLASH_BANK1) {
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
    }
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    /* Exit form continous read for accepting command */
    SFlash_Reset_Continue_Read(pFlashCfg);
    /* For disable command that is setting register instaed of send command, we need write enable */
    SFlash_DisableBurstWrap(pFlashCfg);
    /* Enable 32Bits addr mode again in case reset command make it reset */
    SFlash_Set32BitsAddrMode(pFlashCfg, ENABLE);
    if ((pFlashCfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        /* Enable QE again in case reset command make it reset */
        SFlash_Qspi_Enable(pFlashCfg);
    }
    /* Deburst again to make sure */
    SFlash_DisableBurstWrap(pFlashCfg);

    /* Clear offset setting*/
    *offset = SF_Ctrl_Get_Flash_Image_Offset(group, bank);
    SF_Ctrl_Set_Flash_Image_Offset(0, group, bank);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Erase flash one region
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  startaddr: start address to erase
 * @param  len: data length to erase
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr, int len,
                                                        uint8_t group, SF_Ctrl_Bank_Select bank)
{
    BL_Err_Type stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Erase(pFlashCfg, startaddr, startaddr + len - 1);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

/****************************************************************************/ /**
 * @brief  Program flash one region
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  addr: start address to be programed
 * @param  data: data pointer to be programed
 * @param  len: data length to be programed
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len,
                                                        uint8_t group, SF_Ctrl_Bank_Select bank)
{
    BL_Err_Type stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Program(pFlashCfg, ioMode, addr, data, len);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

/****************************************************************************/ /**
 * @brief  Read data from flash
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  addr: flash read start address
 * @param  data: data pointer to store data read from flash
 * @param  len: data length to read
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len,
                                                       uint8_t group, SF_Ctrl_Bank_Select bank)
{
    BL_Err_Type stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat = SFlash_Read(pFlashCfg, ioMode, 0, addr, data, len);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

/****************************************************************************/ /**
 * @brief  Get Flash Jedec ID
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  data: data pointer to store Jedec ID Read from flash
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data,
                                                             uint8_t group, SF_Ctrl_Bank_Select bank)
{
    BL_Err_Type stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetJedecId(pFlashCfg, data);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Flash Device ID
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  is32BitsAddr: Is flash addr mode in 32-bits
 * @param  data: data pointer to store Device ID Read from flash
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, BL_Fun_Type is32BitsAddr, uint8_t *data,
                                                              uint8_t group, SF_Ctrl_Bank_Select bank)
{
    BL_Err_Type stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetDeviceId(data, is32BitsAddr);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Flash Unique ID
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  data: data pointer to store Device ID Read from flash
 * @param  idLen: Unique id len
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t idLen,
                                                              uint8_t group, SF_Ctrl_Bank_Select bank)
{
    BL_Err_Type stat;
    uint32_t offset;
    uint8_t aesEnable = 0;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);

    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        SFlash_GetUniqueId(data, idLen);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    XIP_SFlash_Opt_Exit(aesEnable);

    return SUCCESS;
}

/****************************************************************************//**
 * @brief  Clear flash status register need lock
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  group: CPU group id 0 or 1
 * @param  bank: Flash bank select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg,
                                                                        uint8_t group, SF_Ctrl_Bank_Select bank)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode&0xf;

    stat=XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, bank);
    } else {
        stat=SFlash_Clear_Status_Register(pFlashCfg);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    return stat;
}

/****************************************************************************/ /**
 * @brief  reconfigure WIFIPLL clock
 *
 * @param  xtalType: XTAL frequency type
 * @param  pllCfg: PLL configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_WIFI_PLL(GLB_XTAL_Type xtalType, const GLB_WA_PLL_Cfg_Type *pllCfgList)
{
    GLB_PLL_REF_CLK_Type refClk;

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_PLL_REFCLK_RC32M;
    } else {
        refClk = GLB_PLL_REFCLK_XTAL;
    }

    GLB_Power_Off_WIFIPLL();
    GLB_WIFIPLL_Ref_Clk_Sel(refClk);
    GLB_Power_On_WIFIPLL(&(pllCfgList[xtalType]), 1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  reconfigure AUPLL clock
 *
 * @param  xtalType: XTAL frequency type
 * @param  pllCfg: PLL configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_AUDIO_PLL(GLB_XTAL_Type xtalType, const GLB_WA_PLL_Cfg_Type *pllCfgList)
{
    GLB_PLL_REF_CLK_Type refClk;

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_PLL_REFCLK_RC32M;
    } else {
        refClk = GLB_PLL_REFCLK_XTAL;
    }

    GLB_Power_Off_AUPLL();
    GLB_AUPLL_Ref_Clk_Sel(refClk);
    GLB_Power_On_AUPLL(&(pllCfgList[xtalType]), 1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Configure AUPLL clock to 384M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_AUDIO_PLL_To_384M(void)
{
    /* GLB_XTAL_Type */
    uint8_t xtalType = GLB_XTAL_NONE;

    /* we take 384M for CPU use,so set LDO to 1.2V*/
    HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_1P20V);
    HBN_Get_Xtal_Type(&xtalType);
    return GLB_Config_AUDIO_PLL((GLB_XTAL_Type)xtalType, audioPllCfg_384M);
}

/****************************************************************************/ /**
 * @brief  Configure AUPLL clock to 400M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_AUDIO_PLL_To_400M(void)
{
    /* GLB_XTAL_Type */
    uint8_t xtalType = GLB_XTAL_NONE;

    HBN_Get_Xtal_Type(&xtalType);
    return GLB_Config_AUDIO_PLL((GLB_XTAL_Type)xtalType, audioPllCfg_400M);
}

/****************************************************************************/ /**
 * @brief  Configure AUPLL clock to 451P58M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_AUDIO_PLL_To_451P58M(void)
{
    /* GLB_XTAL_Type */
    uint8_t xtalType = GLB_XTAL_NONE;

    HBN_Get_Xtal_Type(&xtalType);
    return GLB_Config_AUDIO_PLL((GLB_XTAL_Type)xtalType, audioPllCfg_451P58M);
}

/****************************************************************************/ /**
 * @brief  Configure AUPLL clock to 491P52M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_AUDIO_PLL_To_491P52M(void)
{
    /* GLB_XTAL_Type */
    uint8_t xtalType = GLB_XTAL_NONE;

    HBN_Get_Xtal_Type(&xtalType);
    return GLB_Config_AUDIO_PLL((GLB_XTAL_Type)xtalType, audioPllCfg_491P52M);
}

/****************************************************************************/ /**
 * @brief  chip clock out0 select
 *
 * @param  clkOutType: chip clock out0 output type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out0_Sel(GLB_CHIP_CLK_OUT_0_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out0 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_0_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_0_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  chip clock out1 select
 *
 * @param  clkOutType: chip clock out1 output type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out1_Sel(GLB_CHIP_CLK_OUT_1_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out1 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_1_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_1_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  chip clock out2 select
 *
 * @param  clkOutType: chip clock out2 output type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out2_Sel(GLB_CHIP_CLK_OUT_2_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out2 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_2_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_2_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  chip clock out3 select
 *
 * @param  clkOutType: chip clock out3 output type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out3 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_3_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_3_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set flash id value
 *
 * @param  idValue: flash id value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Flash_Id_Value(uint32_t idValue)
{
    BL_WR_REG(GLB_BASE, GLB_HW_RSV1, (idValue | BFLB_FLASH_ID_VALID_FLAG));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get flash id value
 *
 * @param  None
 *
 * @return flash id
 *
*******************************************************************************/
uint32_t GLB_Get_Flash_Id_Value(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_HW_RSV1);
    if ((tmpVal & BFLB_FLASH_ID_VALID_FLAG) != 0) {
        return (tmpVal & BFLB_FLASH_ID_VALID_MASK);
    }

    return 0x00000000;
}

/****************************************************************************/ /**
 * @brief  Enable PDS power on PLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Pu_PLL_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_PU_RST_CLKPLL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    BL_WR_REG(PDS_BASE, PDS_PU_RST_CLKPLL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable PDS power on PLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Pu_PLL_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_PU_RST_CLKPLL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    BL_WR_REG(PDS_BASE, PDS_PU_RST_CLKPLL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable power down sleep
 *
 * @param  cfg: power down sleep configuration 1
 * @param  cfg4: power down sleep configuration 2
 * @param  pdsSleepCnt: power down sleep count cycle
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt)
{
    uint32_t tmpVal = 0;
    /* sleepForever set 1, chip will sleep forever */
    /* PDS sleep time 1~PDS_WARMUP_LATENCY_CNT <=> error */
    /* PDS sleep time >PDS_WARMUP_LATENCY_CNT <=> correct */
    if ((pdsSleepCnt) && (pdsSleepCnt <= PDS_WARMUP_LATENCY_CNT)) {
        return ERROR;
    } else if (pdsSleepCnt > PDS_WARMUP_LATENCY_CNT) {
        BL_WR_REG(PDS_BASE, PDS_TIME1, pdsSleepCnt - PDS_WARMUP_LATENCY_CNT);
    } else {
    }

    /* PDS_CTL4 config */
    BL_WR_REG(PDS_BASE, PDS_CTL4, *(uint32_t *)cfg4);

    /* PDS_CTL config */
    if (cfg->pdsStart) {
        /* clear pds int */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
        tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
        BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

        tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
        tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
        BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

        BL_WR_REG(PDS_BASE, PDS_CTL, (*(uint32_t *)cfg & ~(1 << 0)));
        BL_WR_REG(PDS_BASE, PDS_CTL, (*(uint32_t *)cfg | (1 << 0)));
    } else {
        BL_WR_REG(PDS_BASE, PDS_CTL, *(uint32_t *)cfg);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep force configure
 *
 * @param  defaultLvCfg: power down sleep default level configuration
 * @param  ramCfg: ram configuration
 * @param  pdsSleepCnt: power down sleep time count
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)
{
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3), (PDS_CTL5_Type *)&(defaultLvCfg->pdsCtl5));
    PDS_Enable((PDS_CTL_Type *)&(defaultLvCfg->pdsCtl), (PDS_CTL4_Type *)&(defaultLvCfg->pdsCtl4), pdsSleepCnt);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Xtal value
 *
 * @param  xtalVal:Xtal value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_Xtal_Value(uint32_t *xtalVal)
{
    uint32_t tmpVal = 0;
    uint8_t xtalType = 0;

    if (NULL == xtalVal) {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    if (HBN_XTAL_FLAG_VALUE == BL_GET_REG_BITS_VAL(tmpVal, HBN_XTAL_STS)) {
        xtalType = BL_GET_REG_BITS_VAL(tmpVal, HBN_XTAL_TYPE);
        switch (xtalType) {
            case GLB_XTAL_NONE:
                *xtalVal = 0;
                break;
            case GLB_XTAL_24M:
                *xtalVal = 24000000;
                break;
            case GLB_XTAL_32M:
                *xtalVal = 32000000;
                break;
            case GLB_XTAL_38P4M:
                *xtalVal = 38400000;
                break;
            case GLB_XTAL_40M:
                *xtalVal = 40000000;
                break;
            case GLB_XTAL_26M:
                *xtalVal = 26000000;
                break;
            case GLB_XTAL_RC32M:
                *xtalVal = 32000000;
                break;
            default:
                *xtalVal = 0;
                break;
        }
        return SUCCESS;
    }

    *xtalVal = 0;
    return ERROR;
}

/****************************************************************************/ /**
 * @brief  HBN enable ACOMP interrupt
 *
 * @param  acompId: HBN Acomp ID
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_AON_ACOMP_ID_TYPE(acompId));
    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    if (acompId == AON_ACOMP0_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    } else if (acompId == AON_ACOMP1_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable ACOMP interrupt
 *
 * @param  acompId: HBN Acomp ID
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_AON_ACOMP_ID_TYPE(acompId));
    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    if (acompId == (uint8_t)AON_ACOMP0_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    } else if (acompId == (uint8_t)AON_ACOMP1_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set ADC clock
 *
 * @param  enable: enable frequency divider or not
 * @param  clkSel: ADC clock selection
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_ADC_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3F));

    /* disable ADC clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    BL_WR_REG(GLB_BASE, GLB_ADC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_ADC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_ADC_CFG0, tmpVal);
#endif

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set IR clock divider
 *
 * @param  enable: enable or disable IR clock
 * @param  clkSel: IR clock type
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_IR_CLK(uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_IR_CLK_SRC_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3F));

    (void)clkSel;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_IR_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_IR_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_IR_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set I2C clock
 *
 * @param  enable: Enable or disable I2C clock
 * @param  clkSel: clock selection
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_I2C_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_I2C_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_I2C_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_I2C_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set SPI clock
 *
 * @param  enable: Enable or disable SPI clock
 * @param  clkSel: clock selection
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SPI_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PEC clock
 *
 * @param  enable: Enable or disable PEC clock
 * @param  clkSel: clock selection
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PEC_CLK(uint8_t enable, GLB_PEC_CLK_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PEC_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    tmpVal >>= 1;
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PEC_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_PEC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    tmpVal >>= 1;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_PEC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    tmpVal >>= 1;
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PEC_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PEC_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_PEC_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set DBI clock
 *
 * @param  enable: Enable or disable DBI clock
 * @param  clkSel: clock selection
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DBI_CLK(uint8_t enable, GLB_DBI_CLK_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_DBI_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DBI_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_DBI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DBI_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DBI_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_DBI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DBI_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DBI_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_DBI_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set audio auto clock
 *
 * @param  divEn: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Audio_AUTO_CLK(uint8_t divEn)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    if (divEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set audio ADC clock
 *
 * @param  enable: ENABLE or DISABLE
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0x3F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_ADC_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set audio SOLO clock
 *
 * @param  enable: ENABLE or DISABLE
 * @param  div: divider
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0xFF));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_SOLO_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG1, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set CAM clock
 *
 * @param  enable: Enable or disable CAM clock
 * @param  clkSel: CAM clock type
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_CAM_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_CAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_SRC_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_CAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CAM_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set SDH clock
 *
 * @param  enable: Enable or disable
 * @param  clkSel: SDH clock type
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SDH_CLK(uint8_t enable, GLB_SDH_CLK_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SDH_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x7));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SDH_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_SDH_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SDH_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SDH_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_SDH_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_SDH_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SDH_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SDH_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PSRAMB clock
 *
 * @param  enable: Enable or disable
 * @param  clkSel: PSRAMB clock type
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PSRAMB_PLL_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_PSRAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PSRAMB_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PSRAMB_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_PSRAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_PSRAM_CFG0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enter HBN
 *
 * @param  cfg: HBN APP Config
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Mode_Enter(HBN_APP_CFG_Type *cfg)
{
#ifndef BOOTROM
    uint32_t valLow = 0, valHigh = 0;
    uint64_t val;

    if (cfg->useXtal32k) {
        HBN_32K_Sel(HBN_32K_XTAL);
    } else {
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }

    /* always disable HBN pin pull up/down to reduce PDS/HBN current, 0x4000F014[16]=0 */
    HBN_Hw_Pu_Pd_Cfg(DISABLE);

    HBN_Pin_WakeUp_Mask(~(cfg->gpioWakeupSrc));
    if (cfg->gpioWakeupSrc != 0) {
        HBN_GPIO_INT_Enable(cfg->gpioTrigType);
    }

    /* HBN RTC config and enable */
    HBN_Clear_RTC_Counter();
    if (cfg->sleepTime != 0) {
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        val = valLow + ((uint64_t)valHigh << 32);
        val += cfg->sleepTime;
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, val & 0xffffffff, val >> 32, HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
    }

    HBN_Power_Down_Flash(cfg->flashCfg);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);

    HBN_Enable(cfg->gpioWakeupSrc, cfg->ldoLevel, cfg->hbnLevel, cfg->dcdcPuSeq);
#endif
}

/****************************************************************************/ /**
 * @brief  Enable HBN mode
 *
 * @param  aGPIOIeCfg: AON GPIO input enable config. Bit(s) of Wakeup GPIO(s) must not be set to
 *                     0(s),say when use GPIO7 as wake up pin,aGPIOIeCfg should be 0x01.
 * @param  ldoLevel: LDO volatge level
 * @param  hbnLevel: HBN work level
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Enable(uint32_t aGPIOIeCfg, HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel, uint8_t dcdcPuSeq)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));
    CHECK_PARAM(IS_HBN_LEVEL_TYPE(hbnLevel));

    /* Setting from guide */
    /* HBN_SRAM Retion */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RETRAM_RET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RETRAM_SLP);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmpVal);

    /* AON GPIO IE */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    /* HBN mode LDO level */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_RT_VOUT_SEL, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* power on dcdc18 sequence  */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    if (dcdcPuSeq) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_DCDC18_AON);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_DCDC18_AON);
    }
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Select RC32M */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbnLevel) {
        case HBN_LEVEL_0:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            break;

        case HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            break;

        default:
            break;
    }
    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Enable HBN mode */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    while (1) {
        arch_delay_ms(1000);
    }
#endif
}

/****************************************************************************/ /**
 * @brief  Enable HBN PIR
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_Enable(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PIR_EN);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable HBN PIR
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_Disable(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PIR_EN);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Config HBN PIR interrupt
 *
 * @param  pirIntCfg: HBN PIR interrupt configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg)
{
#ifndef BOOTROM
    uint32_t tmpVal;
    uint32_t bit4 = 0;
    uint32_t bit5 = 0;
    uint32_t bitVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);

    /* low trigger interrupt */
    if (pirIntCfg->lowIntEn == ENABLE) {
        bit5 = 0;
    } else {
        bit5 = 1;
    }

    /* high trigger interrupt */
    if (pirIntCfg->highIntEn == ENABLE) {
        bit4 = 0;
    } else {
        bit4 = 1;
    }

    bitVal = bit4 | (bit5 << 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_DIS, bitVal);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select HBN PIR low pass filter
 *
 * @param  lpf: HBN PIR low pass filter selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_LPF_Sel(HBN_PIR_LPF_Type lpf)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_PIR_LPF_TYPE(lpf));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_LPF_SEL, lpf);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select HBN PIR high pass filter
 *
 * @param  hpf: HBN PIR high pass filter selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_HPF_Sel(HBN_PIR_HPF_Type hpf)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_PIR_HPF_TYPE(hpf));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_HPF_SEL, hpf);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set HBN PIR threshold value
 *
 * @param  threshold: HBN PIR threshold value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_PIR_Threshold(uint16_t threshold)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM((threshold <= 0x3FFF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_VTH);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_VTH, threshold);
    BL_WR_REG(HBN_BASE, HBN_PIR_VTH, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set HBN PIR interval value
 *
 * @param  interval: HBN PIR interval value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_PIR_Interval(uint16_t interval)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM((interval <= 0xFFF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_INTERVAL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_INTERVAL, interval);
    BL_WR_REG(HBN_BASE, HBN_PIR_INTERVAL, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set HBN bod config
 *
 * @param  enable: ENABLE or DISABLE, if enable, Power up Brown Out Reset
 * @param  threshold: bod threshold
 * @param  mode: bod work mode with por
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOD_Config(uint8_t enable, HBN_BOD_THRES_Type threshold, HBN_BOD_MODE_Type mode)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_BOD_THRES_TYPE(threshold));
    CHECK_PARAM(IS_HBN_BOD_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_BOR_CFG);
    if (enable) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOD, 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOD, 0);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_VTH, threshold);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_SEL, mode);
    BL_WR_REG(HBN_BASE, HBN_BOR_CFG, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Keep RC32K On during rtc power domain off
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note  Don't turn off rc32k, which move to RTC Domain
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Keep_On_RC32K(void)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_RC32K);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set HBN Gpio Keep
 *
 * @param  gpioKeep:HBN gpio keep reg cfg
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Gpio_Keep(uint8_t gpioKeep)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_KEEP_STS, HBN_GPIO_KEEP_FLAG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_KEEP_PIN, gpioKeep);
    BL_WR_REG(HBN_BASE, HBN_RSV3, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable GPIO interrupt
 *
 * @param  gpioIntTrigType: HBN GPIO interrupt trigger type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_GPIO_INT_Enable(HBN_GPIO_INT_Trigger_Type gpioIntTrigType)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(gpioIntTrigType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable GPIO interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_GPIO_INT_Disable(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, 0);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN aon pad ctrl configuration
 *
 * @param  aonPadCtl1: ctrl value 1
 * @param  aonpadCtl2: ctrl value 2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2)
{
#ifndef BOOTROM
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, aonPadCtl1);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_1, aonPadCtl2);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Set AON_HW control aon pad or not
 *
 * @param  aonPadHwCtrlEn: hw control aon pad enable
 * @param  aonGpio: aon pad number
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, HBN_AON_PAD_Type aonGpio)
{
#ifndef BOOTROM
    uint32_t tmpVal;
    uint32_t enAonCtrlGpio;

    /* hbn_pad_ctrl_0 */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
    enAonCtrlGpio = BL_GET_REG_BITS_VAL(tmpVal, HBN_REG_EN_AON_CTRL_GPIO);

    if (aonPadHwCtrlEn) {
        enAonCtrlGpio |= (1 << aonGpio);
    } else {
        enAonCtrlGpio &= ~(1 << aonGpio);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_EN_AON_CTRL_GPIO, enAonCtrlGpio);

    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable BOD interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_BOD_IRQ(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable BOD interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_BOD_IRQ(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN aon pad debbug pull config
 *
 * @param  puPdEn: Enable or disable aon pad pull down and pull up
 * @param  ieSmtEn: Enable or disable aon pad IE and SMT
 * @param  trigMode: trigger mode
 * @param  maskVal: int mask
 * @param  dlyEn: Enable or disable aon pad wakeup delay function
 * @param  dlySec: aon pad wakeup delay sec 1 to 7
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Aon_Pad_WakeUpCfg(BL_Fun_Type puPdEn, HBN_GPIO_INT_Trigger_Type trigMode, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM(((dlySec >= 1) && (dlySec <= 7)));
    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(trigMode));
    CHECK_PARAM((maskVal <= 0x3FF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_EN, dlyEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_SEL, dlySec);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_EN_HW_PU_PD, puPdEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, trigMode);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set Embedded Flash Pullup enabe or disable
 *
 * @param  cfg: Enable or disable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    if (cfg->enableBodInt) {
        HBN_Enable_BOD_IRQ();
    } else {
        HBN_Disable_BOD_IRQ();
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_BOR_CFG);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_VTH, cfg->bodThreshold);
    if (cfg->enablePorInBod) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_BOD_SEL);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_BOD_SEL);
    }

    if (cfg->enableBod) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_BOD);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_BOD);
    }

    BL_WR_REG(HBN_BASE, HBN_BOR_CFG, tmpVal);
#endif
    return SUCCESS;
}

#define GLB_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }
const uint32_t ATTR_CLOCK_CONST_SECTION usbPllSdmin_12M = 0x28000;
const uint32_t ATTR_CLOCK_CONST_SECTION sscDivSdmin_24M = 0x28000;

void glb_40M_delay_us(uint32_t us)
{
    for (uint32_t i = 0; i < us; i++) {
        GLB_CLK_SET_DUMMY_WAIT;
        GLB_CLK_SET_DUMMY_WAIT;
        GLB_CLK_SET_DUMMY_WAIT;
    }
}

/****************************************************************************/ /**
 * @brief  power on wifipll quickly
 *
 * @param  xtalType: XTAL frequency type
 * @param  pllType: only power on xtal
 *******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Fast_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t REG_PLL_BASE_ADDRESS = 0;
    uint32_t tmpVal = 0;

    REG_PLL_BASE_ADDRESS = GLB_BASE + GLB_WIFI_PLL_CFG0_OFFSET;

    /* Step1:config parameter */
    /* cfg1:Set wifipll_refclk_sel and wifipll_refdiv_ratio */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_REFDIV_RATIO, cfg->basicCfg->clkpllRefdivRatio);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 1, tmpVal);

    /* cfg2:Set wifipll_int_frac_sw,wifipll_icp_1u,wifipll_icp_5u */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_INT_FRAC_SW, cfg->basicCfg->clkpllIntFracSw);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_ICP_1U, cfg->basicCfg->clkpllIcp1u);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_ICP_5U, cfg->basicCfg->clkpllIcp5u);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 2, tmpVal);

    /* cfg3:Set wifipll_rz,wifipll_cz,wifipll_c3,wifipll_r4_short,wifipll_r4_en */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_RZ, cfg->basicCfg->clkpllRz);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_CZ, cfg->basicCfg->clkpllCz);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_C3, cfg->basicCfg->clkpllC3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_R4_SHORT, cfg->basicCfg->clkpllR4Short);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_C4_EN, cfg->basicCfg->clkpllC4En);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 3, tmpVal);

    /* cfg4:Set wifipll_sel_sample_clk */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SEL_SAMPLE_CLK, cfg->basicCfg->clkpllSelSampleClk);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 4, tmpVal);

    /* cfg5:Set wifipll_vco_speed */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 5);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_VCO_SPEED, cfg->basicCfg->clkpllVcoSpeed);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 5, tmpVal);

    /* cfg6:Set wifipll_sdm_bypass,wifipll_sdmin */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 6);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDM_CTRL_HW, cfg->basicCfg->clkpllSdmCtrlHw);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDM_BYPASS, cfg->basicCfg->clkpllSdmBypass);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDMIN, cfg->clkpllSdmin);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 6, tmpVal);

    /* cfg10:always set usbpll_sdmin */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 10);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USBPLL_SDMIN, usbPllSdmin_12M);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 10, tmpVal);

    /* cfg12:always set sscdiv_sdmin */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 12);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SSCDIV_SDMIN, sscDivSdmin_24M);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 12, tmpVal);

    /* Step2:config pu */
    /* cfg0 : pu_wifipll_sfreg=1 */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_WIFIPLL_SFREG, 1);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);

    /* delay > 2us */
    glb_40M_delay_us(3);

    /* cfg0 : pu_wifipll=1 */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_WIFIPLL, 1);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);

    /* delay > 2us */
    glb_40M_delay_us(3);

    /* toggle sdm_reset (pulse 0 > 1us) */
    /* cfg0 : wifipll_sdm_reset */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDM_RSTB, 1);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);
    glb_40M_delay_us(2);
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDM_RSTB, 0);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);
    glb_40M_delay_us(2);
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDM_RSTB, 1);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);

    /* Step3:reset pll */
    /* cfg0 : toggle wifipll_reset_fbdv, pulse 0 > 1us */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_FBDV_RSTB, 1);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);
    glb_40M_delay_us(2);
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_FBDV_RSTB, 0);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);
    glb_40M_delay_us(2);
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_FBDV_RSTB, 1);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 0, tmpVal);

    /* Step4:enable output clock */
    /* cfg8 : wifipll clock enable */
    tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 8);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV3);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV4);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV5);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV6);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV8);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV10);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV12);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV20);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV30);
    BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 8, tmpVal);

    if (waitStable) {
        /* Wait 1.5*30us    */
        glb_40M_delay_us(45);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power on xtal and wifipll quickly
 *
 * @param  xtalType: XTAL frequency type
 * @param  pllType: only power on xtal
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Fast_Power_On_XTAL_40M_And_WIFIPLL(void)
{
    uint32_t tmpVal;
    volatile GLB_PLL_REF_CLK_Type refClk;

    refClk = GLB_PLL_REFCLK_XTAL;

    /* power on xtal first */
    AON_Power_On_XTAL();

    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);

    HBN_Set_Xtal_Type(GLB_XTAL_40M);

    /* power on wifipll */
    GLB_Power_Off_WIFIPLL();
    GLB_WIFIPLL_Ref_Clk_Sel(refClk);
    GLB_Fast_Power_On_WIFIPLL(&wifiPllCfg_960M[GLB_XTAL_40M], 0);

    glb_40M_delay_us(30);

    /* if power on xtal, always set xclk from xtal */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);

    /* enable all PLL clock output */
    /* GLB reg_pll_en = 1, cannot be zero */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_PLL_EN);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG0, tmpVal);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Program data to efuse
 *
 * @param  offset: offset of efuse address to program
 * @param  pword: data pointer to buffer which is aligned to word
 * @param  count: count of data in words to program
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Program_Direct(uint32_t offset, uint32_t *pword, uint32_t count)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + offset);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    arch_delay_us(4);

    if (pword != NULL) {
        ARCH_MemCpy4(pEfuseStart0, pword, count);
    }

    EF_Ctrl_Program_Efuse_0();
}

/****************************************************************************/ /**
 * @brief  Read data from efuse
 *
 * @param  offset: offset of efuse address to read
 * @param  pword: data pointer to buffer which is aligned to word
 * @param  count: count of data in words to read
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Direct(uint32_t offset, uint32_t *pword, uint32_t count)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + offset);

    EF_Ctrl_Load_Efuse_R0();

    ARCH_MemCpy4(pword, pEfuseStart0, count);
}
