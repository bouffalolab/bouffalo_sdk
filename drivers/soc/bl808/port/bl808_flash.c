#include "bl808_glb.h"
#include "bl808_xip_sflash.h"
#include "bl808_sf_cfg.h"
#include "bflb_flash.h"

static uint32_t g_jedec_id = 0;
static SPI_Flash_Cfg_Type g_flash_cfg = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /* reg */
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
    .cReadSupport = 0,
    .cReadMode = 0xa0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /* erase */
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /* write */
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = 0x10,
    .clkDelay = 0,
    .clkInvert = 0x03,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /* id */
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .enter32BitsAddrCmd = 0xb7,
    .exit32BitsAddrCmd = 0xe9,
    .sectorSize = 4,
    .pageSize = 256,

    /* read */
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

    /* AC */
    .timeEsector = 500,
    .timeE32k = 2000,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};

/**
 * @brief flash_get_clock_delay
 *
 * @return int
 */
static int flash_get_clock_delay(SPI_Flash_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_0);
    /* bit0-3 for clk delay */
    if (BL_IS_REG_BIT_SET(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN)) {
        cfg->clkDelay = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_READ_DLY_N) + 1;
    } else {
        cfg->clkDelay = 0;
    }
    cfg->clkInvert = 0;
    /* bit0 for clk invert */
    cfg->clkInvert |= ((BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_CLK_OUT_INV_SEL) & 1) << 0);
    /* bit1 for rx clk invert */
    cfg->clkInvert |= ((BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_CLK_SF_RX_INV_SEL) & 1) << 1);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_1);
    /* bit4-6 for do delay */
    cfg->clkDelay |= ((BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_IO_0_DO_DLY_SEL) & 7) << 4);
    /* bit2-4 for di delay */
    cfg->clkInvert |= ((BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_IO_0_DI_DLY_SEL) & 7) << 2);
    /* bit5-7 for oe delay */
    cfg->clkInvert |= ((BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_IO_0_OE_DLY_SEL) & 7) << 5);

    return 0;
}

/**
 * @brief flash_set_cmds
 *
 * @return int
 */
static int ATTR_TCM_SECTION flash_set_cmds(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    SF_Ctrl_Cmds_Cfg cmds_cfg = {
        .ackLatency = 1,
        .cmdsCoreEn = 1,
        .cmdsEn = 1,
        .cmdsWrapMode = 1,
        .cmdsWrapLen = 9,
    };

    if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        cmds_cfg.cmdsWrapMode = 2;
        cmds_cfg.cmdsWrapLen = 2;
    }
    SF_Ctrl_Cmds_Set(&cmds_cfg, 0);
    
    return 0;
}

/**
 * @brief flash_set_qspi_enable
 *
 * @return int
 */
static int ATTR_TCM_SECTION flash_set_qspi_enable(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        SFlash_Qspi_Enable(p_flash_cfg);
    }

    return 0;
}

/**
 * @brief flash_set_l1c_wrap
 *
 * @return int
 */
static int ATTR_TCM_SECTION flash_set_l1c_wrap(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if (((p_flash_cfg->ioMode >> 4) & 0x01) == 1) {
        L1C_Set_Wrap(DISABLE);
    } else {
        L1C_Set_Wrap(ENABLE);
        if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(p_flash_cfg);
        }
    }

    return 0;
}

/**
 * @brief flash_config_init
 *
 * @return int
 */
static int ATTR_TCM_SECTION flash_config_init(SPI_Flash_Cfg_Type *p_flash_cfg, uint8_t *jedec_id)
{
    int ret = -1;
    uint8_t isAesEnable = 0;
    uint32_t jid = 0;
    uint32_t offset = 0;
    uintptr_t flag;

    flag = bflb_irq_save();
    XIP_SFlash_Opt_Enter(&isAesEnable);
    XIP_SFlash_State_Save(p_flash_cfg, &offset, 0, 0);
    SFlash_GetJedecId(p_flash_cfg, (uint8_t *)&jid);
    arch_memcpy(jedec_id, (uint8_t *)&jid, 3);
    jid &= 0xFFFFFF;
    g_jedec_id = jid;
    ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, p_flash_cfg, 0, 0);
    if (ret == 0) {
        p_flash_cfg->mid = (jid & 0xff);
    }

    /* Set flash controler from p_flash_cfg */
    flash_set_cmds(p_flash_cfg);
    flash_set_qspi_enable(p_flash_cfg);
    flash_set_l1c_wrap(p_flash_cfg);
    XIP_SFlash_State_Restore(p_flash_cfg, offset, 0, 0);
    XIP_SFlash_Opt_Exit(isAesEnable);
    bflb_irq_restore(flag);

    return ret;
}

/**
 * @brief multi flash adapter
 *
 * @return int
 */
int ATTR_TCM_SECTION bflb_flash_init(void)
{
    int ret = -1;
    uint32_t jedec_id = 0;

    jedec_id = GLB_Get_Flash_Id_Value();
    if (jedec_id != 0) {
        ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jedec_id, &g_flash_cfg, 0, 0);
        if (ret == 0) {
            g_jedec_id = jedec_id;
            flash_get_clock_delay(&g_flash_cfg);
            return 0;
        }
    }


    ret = flash_config_init(&g_flash_cfg, (uint8_t *)&jedec_id);

    flash_get_clock_delay(&g_flash_cfg);
    GLB_Set_Flash_Id_Value(g_jedec_id);

    return ret;
}

uint32_t bflb_flash_get_jedec_id(void)
{
    uint32_t jid = 0;

    jid = ((g_jedec_id & 0xff) << 16) + (g_jedec_id & 0xff00) + ((g_jedec_id & 0xff0000) >> 16);
    return jid;
}

void bflb_flash_get_cfg(uint8_t **cfg_addr, uint32_t *len)
{
    *cfg_addr = (uint8_t *)&g_flash_cfg;
    *len = sizeof(SPI_Flash_Cfg_Type);
}

ATTR_TCM_SECTION uint32_t bflb_flash_get_image_offset(void)
{
    return SF_Ctrl_Get_Flash_Image_Offset(0, 0);
}

void bflb_flash_set_iomode(uint8_t iomode)
{
    g_flash_cfg.ioMode &= ~0x1f;
    if (iomode == 4) {
        g_flash_cfg.ioMode |= iomode;
    } else {
        g_flash_cfg.ioMode |= 0x10;
        g_flash_cfg.ioMode |= iomode;
    }
}

int ATTR_TCM_SECTION bflb_flash_erase(uint32_t startaddr, uint32_t len)
{
    int stat;
    uintptr_t flag;

    if (startaddr >= BL808_FLASH_XIP_END - BL808_FLASH_XIP_BASE) {
        return -ENOMEM;
    }

    flag = bflb_irq_save();
    stat = XIP_SFlash_Erase_Need_Lock(&g_flash_cfg, startaddr, len, 0, 0);
    bflb_irq_restore(flag);

    return stat;
}

int ATTR_TCM_SECTION bflb_flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    int stat;
    uintptr_t flag;

    if (addr >= BL808_FLASH_XIP_END - BL808_FLASH_XIP_BASE) {
        return -ENOMEM;
    }

    flag = bflb_irq_save();
    stat = XIP_SFlash_Write_Need_Lock(&g_flash_cfg, addr, data, len, 0, 0);
    bflb_irq_restore(flag);

    return stat;
}

int ATTR_TCM_SECTION bflb_flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    int stat;
    uintptr_t flag;

    if (addr >= BL808_FLASH_XIP_END - BL808_FLASH_XIP_BASE) {
        return -ENOMEM;
    }

    flag = bflb_irq_save();
    stat = XIP_SFlash_Read_Need_Lock(&g_flash_cfg, addr, data, len, 0, 0);
    bflb_irq_restore(flag);

    return stat;
}

int ATTR_TCM_SECTION bflb_flash_set_cache(uint8_t cont_read, uint8_t cache_enable, uint8_t cache_way_disable, uint32_t flash_offset)
{
    uint8_t isAesEnable = 0;
    uint32_t tmp[1];
    int stat;

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);

    XIP_SFlash_Opt_Enter(&isAesEnable);
    /* To make it simple, exit cont read anyway */
    SFlash_Reset_Continue_Read(&g_flash_cfg);

    if (g_flash_cfg.cReadSupport == 0) {
        cont_read = 0;
    }

    if (cont_read == 1) {
        stat = SFlash_Read(&g_flash_cfg, g_flash_cfg.ioMode & 0xf, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));

        if (0 != stat) {
            XIP_SFlash_Opt_Exit(isAesEnable);
            return -1;
        }
    }

    /* TODO: Set default value */

    SF_Ctrl_Set_Flash_Image_Offset(flash_offset, 0, 0);
    SFlash_IDbus_Read_Enable(&g_flash_cfg, g_flash_cfg.ioMode & 0xf, cont_read, SF_CTRL_FLASH_BANK0);

    XIP_SFlash_Opt_Exit(isAesEnable);

    return 0;
}

void bflb_flash_aes_init(struct bflb_flash_aes_config_s *config)
{
    uint8_t hw_key_enable = 0;

    if (config->key == NULL) {
        hw_key_enable = 1;
    }

    SF_Ctrl_AES_Set_Key_BE(config->region, (uint8_t *)config->key, config->keybits);
    SF_Ctrl_AES_Set_IV_BE(config->region, (uint8_t *)config->iv, config->start_addr);
    SF_Ctrl_AES_Set_Region(config->region, config->region_enable, hw_key_enable, config->start_addr, config->end_addr - 1, config->lock_enable);
}

void bflb_flash_aes_enable(void)
{
    SF_Ctrl_AES_Enable();
}

void bflb_flash_aes_disable(void)
{
    SF_Ctrl_AES_Disable();
}