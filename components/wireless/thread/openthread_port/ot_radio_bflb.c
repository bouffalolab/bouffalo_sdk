#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include CHIP_HDR
#include CHIP_GLB_HDR
#include <bflb_mtimer.h>
#include <bflb_efuse.h>
#include <bflb_flash.h>
#include <lmac154.h>
#include <zb_timer.h>

#include <openthread_port.h>
#include <ot_radio_trx.h>
#include <ot_utils_ext.h>

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

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    return bflb_mtimer_get_time_us();
}

otError otPlatRadioEnable(otInstance *aInstance) 
{
    ot_radioEnable();
    
    bflb_irq_attach(M154_INT_IRQn, (irq_callback)lmac154_get2015InterruptHandler(), NULL);
    bflb_irq_enable(M154_INT_IRQn);

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance) 
{
    bflb_irq_disable(M154_INT_IRQn);
    lmac154_disableRx();

    return OT_ERROR_NONE;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel) 
{
    uint8_t ch = lmac154_getChannel();

#if (OPENTHREAD_FTD) || (OPENTHREAD_MTD)
    if (lmac154_isRxStateWhenIdle() != otThreadGetLinkMode(aInstance).mRxOnWhenIdle) {
        lmac154_setRxStateWhenIdle(otThreadGetLinkMode(aInstance).mRxOnWhenIdle);
    }
#endif

    if (ch != (aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN)) {

        lmac154_disableRx();

        lmac154_setChannel(aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN);
    }

    lmac154_enableRx();

    return OT_ERROR_NONE;
}
