#include "bl602_mfg_media.h"

static uint8_t rf_para_on_flash=0;

int8_t mfg_media_read_macaddr_need_lock(uint8_t mac[6],uint8_t reload)
{
    if(rf_para_on_flash){
        return mfg_flash_read_macaddr(mac,reload);
    }else{
        return mfg_efuse_read_macaddr(mac,reload);
    }
}

int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload)
{
    int ret;
    uintptr_t flag;

    flag = bflb_irq_save();
    ret = mfg_media_read_macaddr_need_lock(mac, reload);
    bflb_irq_restore(flag);

    return ret;
}

int mfg_media_read_macaddr(uint8_t mac[6], uint8_t reload)
{
    return mfg_media_read_macaddr_need_lock(mac, reload);
}
