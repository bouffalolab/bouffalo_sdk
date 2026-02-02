
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

otError otPlatRadioEnable(otInstance *aInstance) 
{
    uint32_t tag = otrEnterCrit();
    zb_timer_cfg(bflb_mtimer_get_time_us() >> LMAC154_US_PER_SYMBOL_BITS);
    otrExitCrit(tag);

    lmac154_setStd2015Extra(true);
    lmac154_setTxRetry(0);
    lmac154_fptClear();

    /** AIFS + preamble len + sfd + phy header + phy payload; currently, consider 42 as max enh-ack frame length */
    lmac154_setEnhAckWaitTime((LMAC154_AIFS + 10 + (6 + 42) * 2) << LMAC154_US_PER_SYMBOL_BITS);
    lmac154_setRxStateWhenIdle(true);

    if (otRadioVar_ptr->opt.bf.isCoexEnable) {
        lmac154_enableCoex();
    }
    else {
        lmac154_disableCoex();
    }
#if defined (BL702L)
    lmac154_setTxRxTransTime(0xA0);
    if (otRadioVar_ptr->opt.bf.isFtd) {
        lmac154_setFramePendingMode(LMAC154_FPT_ANY);
    }
    else {
        lmac154_setFramePendingMode(LMAC154_FPT_DATAREQ);
    }

    extern bool bz_phy_set_tx_power(int power_dbm);
    bz_phy_set_tx_power(bl_wireless_default_tx_power_get());

#ifdef CFG_PDS_ENABLE
    struct bflb_device_s *rtc = bflb_device_get_by_name("rtc");
    uint64_t rtcCount = bflb_rtc_get_time(rtc);

    zb_timer_cfg(((rtcCount * 1000000) >> LMAC154_US_PER_SYMBOL_BITS) / 32000);
    otPds_setDataPollCsma(CFG_DATA_POLL_CSMA);
#endif
#endif

#ifndef CFG_PDS_ENABLE
    tag = otrEnterCrit();
    zb_timer_cfg(bflb_mtimer_get_time_us() >> LMAC154_US_PER_SYMBOL_BITS);
    otrExitCrit(tag);
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

