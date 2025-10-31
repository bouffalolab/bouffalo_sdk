#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#ifdef BL702L
#include <bl702l.h>
#else
#include <bl702.h>
#endif
#include <bl_irq.h>
#include <bl_timer.h>
#include <bl_wireless.h>
#include <lmac154.h>

#include <openthread_port.h>
#include <ot_radio_trx.h>
#include <ot_utils_ext.h>
#ifdef CFG_PDS_ENABLE
#include <ot_pds_ext.h>
#endif

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64) 
{
    uint8_t temp;

    bl_wireless_mac_addr_get(aIeeeEui64);

    for (int i = 0; i < OT_EXT_ADDRESS_SIZE / 2; i ++) {
        temp = aIeeeEui64[OT_EXT_ADDRESS_SIZE - i - 1];
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - i - 1] = aIeeeEui64[i];
        aIeeeEui64[i] = temp;
    }

    if (aIeeeEui64[OT_EXT_ADDRESS_SIZE - 1] == 0 && aIeeeEui64[OT_EXT_ADDRESS_SIZE - 2] == 0) {
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - 1] = aIeeeEui64[OT_EXT_ADDRESS_SIZE - 3];
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - 2] = aIeeeEui64[OT_EXT_ADDRESS_SIZE - 4];
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - 3] = aIeeeEui64[OT_EXT_ADDRESS_SIZE - 5];
    }
}

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    return bl_timer_now_us64();
}

otError otPlatRadioEnable(otInstance *aInstance) 
{
    ot_radioEnable();

#ifdef CFG_PDS_ENABLE
    otPds_setDataPollCsma(CFG_DATA_POLL_CSMA);
#endif

    bl_irq_register(M154_IRQn, lmac154_get2015InterruptHandler());
    bl_irq_enable(M154_IRQn);

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance) 
{
    bl_irq_disable(M154_IRQn);
    lmac154_disableRx();

    return OT_ERROR_NONE;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel) 
{
    uint8_t ch = aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;

#ifdef BL702L
    extern bool bz_phy_optimize_tx_channel(uint32_t channel);
    bz_phy_optimize_tx_channel(2405 + 5 * ch);
#endif

    lmac154_setChannel((lmac154_channel_t)ch);
#if (OPENTHREAD_FTD) || (OPENTHREAD_MTD)
    lmac154_setRxStateWhenIdle(otThreadGetLinkMode(aInstance).mRxOnWhenIdle);
#endif
    lmac154_enableRx();

    return OT_ERROR_NONE;
}