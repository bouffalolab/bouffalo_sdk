#include "bl616_glb.h"
#include "bl616_xip_sflash.h"
#include "bl616_sf_cfg.h"
#include "bl616_ef_cfg.h"
#include "bflb_flash.h"

static uint32_t flash1_size = 4 * 1024 * 1024;
static uint32_t flash2_size = 2 * 1024 * 1024;
static uint32_t g_jedec_id = 0;
static uint32_t g_jedec_id2 = 0;
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
    .cReadMode = 0x20,

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
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static SPI_Flash_Cfg_Type g_flash2_cfg = {
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
    .cReadMode = 0x20,

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
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 33 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};

static bflb_efuse_device_info_type deviceInfo;

uint32_t bflb_flash2_get_jedec_id(void)
{
    uint32_t jid = 0;

    jid = ((g_jedec_id2 & 0xff) << 16) + (g_jedec_id2 & 0xff00) + ((g_jedec_id2 & 0xff0000) >> 16);
    return jid;
}

static void flash_get_clock_delay(SPI_Flash_Cfg_Type *cfg)
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
}

static void ATTR_TCM_SECTION flash_set_cmds(SPI_Flash_Cfg_Type *p_flash_cfg)
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
}

static void ATTR_TCM_SECTION flash_set_qspi_enable(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        SFlash_Qspi_Enable(p_flash_cfg);
    }
}

static void ATTR_TCM_SECTION flash_set_l1c_wrap(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if (((p_flash_cfg->ioMode >> 4) & 0x01) == 1) {
        L1C_Set_Wrap(DISABLE);
    } else {
        L1C_Set_Wrap(ENABLE);
        if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(p_flash_cfg);
        }
    }
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
    ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, p_flash_cfg);
    if (ret == 0) {
        p_flash_cfg->mid = (jid & 0xff);
    }

    // p_flash_cfg->ioMode = 0x11;
    // p_flash_cfg->cReadSupport = 0x00;

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
 * @brief flash2 init
 *
 * @return int
 */
static int ATTR_TCM_SECTION flash2_init(void)
{
    int stat = -1;
    uint32_t ret = 0;
    uint32_t jid = 0;

    SF_Ctrl_Bank2_Cfg sfBank2Cfg = {
        .sbus2Select = ENABLE,
        .bank2RxClkInvertSrc = DISABLE,
        .bank2RxClkInvertSel = DISABLE,
        .bank2DelaySrc = DISABLE,
        .bank2ClkDelay = 1,
        .doDelay = 0,
        .diDelay = 0,
        .oeDelay = 0,
        .remap = SF_CTRL_REMAP_4MB,
        .remapLock = 1,
    };
    SF_Ctrl_Cmds_Cfg cmdsCfg = {
        .ackLatency = 1,
        .cmdsCoreEn = 1,
        .cmdsEn = 1,
        .cmdsWrapMode = 1,
        .cmdsWrapLen = SF_CTRL_WRAP_LEN_4096,
    };

    if (deviceInfo.memoryInfo == 0) {
        /* memoryInfo==0, external flash */
        flash1_size = 64 * 1024 * 1024;
        flash2_size = 0;
    } else if (deviceInfo.memoryInfo == 1) {
        flash1_size = 2 * 1024 * 1024;
        flash2_size = 0;
    } else if (deviceInfo.memoryInfo == 2) {
        flash1_size = 4 * 1024 * 1024;
        flash2_size = 0;
    } else if (deviceInfo.memoryInfo == 3) {
        /* memoryInfo==3, embedded 4MB+2MB flash */
        flash1_size = 4 * 1024 * 1024;
        flash2_size = 2 * 1024 * 1024;
    } else {
        flash1_size = 8 * 1024 * 1024;
        flash2_size = 0;
    }

    if (flash2_size > 0) {
        SF_Cfg_Sbus2_Flash_Init(SF_IO_EMB_SWAP_IO3IO0_AND_SF2, &sfBank2Cfg);
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
        ret = SF_Cfg_Flash_Identify_Ext(0, SF_IO_EMB_SWAP_IO3IO0_AND_SF2, 0, &g_flash2_cfg, 0, SF_CTRL_FLASH_BANK1);
        if ((ret & BFLB_FLASH_ID_VALID_FLAG) == 0) {
            return -1;
        }
        g_flash2_cfg.ioMode = 0x11;
        g_flash2_cfg.cReadSupport = 0;
        g_flash2_cfg.cReadMode = 0xff;
        SFlash_GetJedecId(&g_flash2_cfg, (uint8_t *)&jid);
        jid &= 0xFFFFFF;
        g_jedec_id2 = jid;

        SF_Ctrl_Cmds_Set(&cmdsCfg, SF_CTRL_FLASH_BANK1);
        stat = SFlash_IDbus_Read_Enable(&g_flash2_cfg, (g_flash2_cfg.ioMode & 0xf), 0, SF_CTRL_FLASH_BANK1);
        if (0 != stat) {
            return -1;
        }
        SF_Ctrl_Sbus2_Revoke_replace();
    }

    return 0;
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

    bflb_ef_ctrl_get_device_info(&deviceInfo);

    jedec_id = GLB_Get_Flash_Id_Value();
    if (jedec_id != 0) {
        ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jedec_id, &g_flash_cfg);
        if (ret == 0) {
            g_jedec_id = jedec_id;
            g_flash_cfg.ioMode &= 0x0f;
            flash_get_clock_delay(&g_flash_cfg);
            flash2_init();
            return 0;
        }
    }

    g_flash_cfg.ioMode &= 0x0f;
    ret = flash_config_init(&g_flash_cfg, (uint8_t *)&jedec_id);

    g_flash_cfg.ioMode &= 0x0f;
    flash_get_clock_delay(&g_flash_cfg);
    GLB_Set_Flash_Id_Value(g_jedec_id);

    flash2_init();

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

ATTR_TCM_SECTION uint32_t bflb_flash_get_image_offset(void)
{
    return SF_Ctrl_Get_Flash_Image_Offset(0, 0);
}

/**
 * @brief erase flash via sbus
 *
 * @param flash absolute startaddr
 * @param flash absolute endaddr
 * @return int
 */
int ATTR_TCM_SECTION bflb_flash_erase(uint32_t startaddr, uint32_t len)
{
    int stat = -1;
    uintptr_t flag;

    if ((startaddr + len) > (flash1_size + flash2_size)) {
        return -ENOMEM;
    } else if ((startaddr + len) <= flash1_size) {
        flag = bflb_irq_save();
        stat = XIP_SFlash_Erase_Need_Lock(&g_flash_cfg, startaddr, len, 0, 0);
        bflb_irq_restore(flag);
    } else if (startaddr >= flash1_size) {
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
        stat = SFlash_Erase(&g_flash2_cfg, startaddr, startaddr + len - 1);
        SF_Ctrl_Sbus2_Revoke_replace();
    } else {
        flag = bflb_irq_save();
        stat = XIP_SFlash_Erase_Need_Lock(&g_flash_cfg, startaddr, flash1_size - startaddr, 0, 0);
        bflb_irq_restore(flag);
        if (stat != 0) {
            return stat;
        }
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
        stat = SFlash_Erase(&g_flash2_cfg, flash1_size, startaddr + len - flash1_size - 1);
        SF_Ctrl_Sbus2_Revoke_replace();
    }

    return stat;
}

/**
 * @brief write flash data via sbus
 *
 * @param flash absolute addr
 * @param data
 * @param len
 * @return int
 */
int ATTR_TCM_SECTION bflb_flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    int stat = -1;
    uintptr_t flag;

    if ((addr + len) > (flash1_size + flash2_size)) {
        return -ENOMEM;
    } else if ((addr + len) <= flash1_size) {
        flag = bflb_irq_save();
        stat = XIP_SFlash_Write_Need_Lock(&g_flash_cfg, addr, data, len, 0, 0);
        bflb_irq_restore(flag);
    } else if (addr >= flash1_size) {
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
        stat = SFlash_Program(&g_flash2_cfg, SF_CTRL_DO_MODE, addr, data, len);
        SF_Ctrl_Sbus2_Revoke_replace();
    } else {
        flag = bflb_irq_save();
        stat = XIP_SFlash_Write_Need_Lock(&g_flash_cfg, addr, data, flash1_size - addr, 0, 0);
        bflb_irq_restore(flag);
        if (stat != 0) {
            return stat;
        }
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
        stat = SFlash_Program(&g_flash2_cfg, SF_CTRL_DO_MODE, flash1_size, data + (flash1_size - addr), addr + len - flash1_size);
        SF_Ctrl_Sbus2_Revoke_replace();
    }

    return stat;
}

/**
 * @brief read flash data via sbus
 *
 * @param flash absolute addr
 * @param data
 * @param len
 * @return int
 */
int ATTR_TCM_SECTION bflb_flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    int stat = -1;
    uintptr_t flag;

    if ((addr + len) > (flash1_size + flash2_size)) {
        return -ENOMEM;
    } else if ((addr + len) <= flash1_size) {
        flag = bflb_irq_save();
        stat = XIP_SFlash_Read_Need_Lock(&g_flash_cfg, addr, data, len, 0, 0);
        bflb_irq_restore(flag);
    } else if (addr >= flash1_size) {
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
        stat = SFlash_Read(&g_flash2_cfg, SF_CTRL_DO_MODE, 0, addr, data, len);
        SF_Ctrl_Sbus2_Revoke_replace();
    } else {
        flag = bflb_irq_save();
        stat = XIP_SFlash_Read_Need_Lock(&g_flash_cfg, addr, data, flash1_size - addr, 0, 0);
        bflb_irq_restore(flag);
        if (stat != 0) {
            return stat;
        }
        SF_Ctrl_Sbus2_Replace(SF_CTRL_PAD2);
        stat = SFlash_Read(&g_flash2_cfg, SF_CTRL_DO_MODE, 0, flash1_size, data + (flash1_size - addr), addr + len - flash1_size);
        SF_Ctrl_Sbus2_Revoke_replace();
    }

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
    SFlash_IDbus_Read_Enable(&g_flash_cfg, g_flash_cfg.ioMode & 0xf, cont_read, 0);

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