#include CHIP_HDR
#include CHIP_GLB_HDR
#include <bflb_mtimer.h>
#include <bflb_efuse.h>
#include <bflb_flash.h>
#include <lmac154.h>
#if defined (BL702L) || defined (BL616)
#include <lmac154_fpt.h>
#endif
#include <zb_timer.h>

#include <openthread_port.h>
#include <ot_radio_trx.h>

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64) 
{
    int i;
    uint32_t flash_id = 0;

    memset(aIeeeEui64, 0, 8);
    for (i = MAC_ADDRESS_MAX_NUM - 1; i >= 0; i --) {
        if (!bflb_efuse_is_mac_address_slot_empty(i, 0)) {
            bflb_efuse_read_mac_address_opt(i, aIeeeEui64, 0);
            return;
        }
    }

    aIeeeEui64[0] = 0x8C;
    aIeeeEui64[1] = 0xFD;
    aIeeeEui64[2] = 0xF0;
    flash_id = bflb_flash_get_jedec_id();
    aIeeeEui64[3] = (flash_id >> 24) & 0xff;
    aIeeeEui64[4] = (flash_id >> 16) & 0xff;
    aIeeeEui64[5] = (flash_id >> 8) & 0xff;
    aIeeeEui64[6] = flash_id & 0xff;
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    lmac154_enable1stStack();
    lmac154_registerEventCallback(LMAC154_STACK_1, ot_radioRxDoneCallback);

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    lmac154_disable1stStack();
    lmac154_registerEventCallback(LMAC154_STACK_1, NULL);

    return OT_ERROR_NONE;
}
