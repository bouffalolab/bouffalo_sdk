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

otError otPlatRadioEnable(otInstance *aInstance) 
{
    uint32_t tag;

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
#endif
#if defined (BL702L) && defined (BL616)
    if (otRadioVar_ptr->opt.bf.isFtd) {
        lmac154_setFramePendingMode(LMAC154_FPT_ANY);
    }
    else {
        lmac154_setFramePendingMode(LMAC154_FPT_DATAREQ);
    }
#endif

    tag = otrEnterCrit();
    zb_timer_cfg(bflb_mtimer_get_time_us() >> LMAC154_US_PER_SYMBOL_BITS);
    otrExitCrit(tag);

    bflb_irq_attach(M154_INT_IRQn, 
                    (irq_callback)lmac154_registerEventCallback(ot_radioRxDoneCallback), 
                    NULL);
    bflb_irq_enable(M154_INT_IRQn);

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance) 
{
    bflb_irq_disable(M154_INT_IRQn);
    lmac154_disableRx();

    return OT_ERROR_NONE;
}
