#include "bl702_glb.h"
#include "bl702_xip_sflash.h"
#include "bl702_xip_sflash_ext.h"
#include "bl702_sf_cfg.h"
#include "bl702_sf_cfg_ext.h"
#include "bl702_ef_ctrl.h"
#include "bflb_flash.h"

static uint32_t g_jedec_id = 0;
static SPI_Flash_Cfg_Type g_flash_cfg;

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

static void ATTR_TCM_SECTION flash_sf2_gpio_init(void)
{
    uint32_t tmpVal;
    Efuse_Device_Info_Type devInfo;

    EF_Ctrl_Read_Device_Info(&devInfo);
    /* flash_cfg != BFLB_FLASH_CFG_SF1_EXT_17_22, flash pad use SF2 */
    if (devInfo.flash_cfg != BFLB_FLASH_CFG_SF1_EXT_17_22) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_USE_PSRAM__IO);
        /* GLB_CFG_GPIO_USE_PSRAM_IO == 0, need init SF2 flash gpio */
        if (BL_GET_REG_BITS_VAL(tmpVal, GLB_CFG_GPIO_USE_PSRAM_IO) == 0x00) {
            SF_Cfg_Init_Ext_Flash_Gpio(1);
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
    uint32_t jid = 0;
    uint32_t offset = 0;
    uintptr_t flag;

    flag = bflb_irq_save();
    XIP_SFlash_Opt_Enter();
    XIP_SFlash_State_Save(p_flash_cfg, &offset);
    flash_sf2_gpio_init();
    SFlash_GetJedecId(p_flash_cfg, (uint8_t *)&jid);
    arch_memcpy(jedec_id, (uint8_t *)&jid, 3);
    jid &= 0xFFFFFF;
    g_jedec_id = jid;
    ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, p_flash_cfg);
    if (ret == 0) {
        p_flash_cfg->mid = (jid & 0xff);
    }

    /* Set flash controler from p_flash_cfg */
    flash_set_qspi_enable(p_flash_cfg);
    flash_set_l1c_wrap(p_flash_cfg);
    XIP_SFlash_State_Restore(p_flash_cfg, p_flash_cfg->ioMode & 0x0f, offset);
    XIP_SFlash_Opt_Exit();
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
    uint8_t clkDelay = 1;
    uint8_t clkInvert = 1;
    uint32_t jedec_id = 0;
    uintptr_t flag;

    flag = bflb_irq_save();
    L1C_Cache_Flush();
    SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(0, &g_flash_cfg);
    L1C_Cache_Flush();
    bflb_irq_restore(flag);
    if (g_flash_cfg.mid != 0xff) {
        return 0;
    }
    clkDelay = g_flash_cfg.clkDelay;
    clkInvert = g_flash_cfg.clkInvert;
    g_flash_cfg.ioMode = g_flash_cfg.ioMode & 0x0f;

    ret = flash_config_init(&g_flash_cfg, (uint8_t *)&jedec_id);

    g_flash_cfg.clkDelay = clkDelay;
    g_flash_cfg.clkInvert = clkInvert;

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
    return SF_Ctrl_Get_Flash_Image_Offset();
}

void bflb_flash_set_iomode(uint8_t iomode)
{
    g_flash_cfg.ioMode &= ~0x0f;
    g_flash_cfg.ioMode |= iomode;
}

int ATTR_TCM_SECTION bflb_flash_erase(uint32_t startaddr, uint32_t len)
{
    int ret = -1;
    uintptr_t flag;

    flag = bflb_irq_save();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_Erase_Need_Lock(&g_flash_cfg, g_flash_cfg.ioMode & 0x0f, startaddr, startaddr + len - 1);
    XIP_SFlash_Opt_Exit();
    bflb_irq_restore(flag);

    return ret;
}

int ATTR_TCM_SECTION bflb_flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    int ret = -1;
    uintptr_t flag;

    flag = bflb_irq_save();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_Write_Need_Lock(&g_flash_cfg, g_flash_cfg.ioMode & 0x0f, addr, data, len);
    XIP_SFlash_Opt_Exit();
    bflb_irq_restore(flag);

    return ret;
}

int ATTR_TCM_SECTION bflb_flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    int ret = -1;
    uintptr_t flag;

    flag = bflb_irq_save();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_Read_Need_Lock(&g_flash_cfg, g_flash_cfg.ioMode & 0x0f, addr, data, len);
    XIP_SFlash_Opt_Exit();
    bflb_irq_restore(flag);

    return ret;
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